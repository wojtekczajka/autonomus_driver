#include "road_lane_detector/road_lane_detector_canny.h"

#include "common/config_parser.h"

RoadLaneDetectorCanny::RoadLaneDetectorCanny() : xPosition(0),
                                                 rightVerticalLaneDetected(false),
                                                 leftVerticalLaneDetected(false),
                                                 leftHorizontalBottomLaneDetected(false),
                                                 leftHorizontalTopLaneDetected(false),
                                                 rightHorizontalBottomLaneDetected(false),
                                                 rightHorizontalTopLaneDetected(false),
                                                 turnLeftDetected(false),
                                                 turnRightDetected(false) {
    ConfigParser configParser("camera_config.txt");
    frameWidth = configParser.getValue<int>("FRAME_WIDTH", 640);
    frameHeight = configParser.getValue<int>("FRAME_HEIGHT", 368);
    int radius = frameWidth / 2;
    bottomCircleMask = cv::Mat::zeros(cv::Point(frameWidth, frameHeight), CV_8U);
    cv::circle(bottomCircleMask, cv::Point(frameWidth / 2, frameHeight), radius / 3.5, cv::Scalar(255), -1);
}

cv::Mat convertFrameToGrayscale(const cv::Mat& frame) {
    cv::Mat grayscaleFrame;
    cv::cvtColor(frame, grayscaleFrame, cv::COLOR_BGR2GRAY);
    return grayscaleFrame;
}

void RoadLaneDetectorCanny::autoCanny(const cv::Mat& frame, double sigma) {
    cv::Scalar v = cv::mean(frame);
    double mean = v.val[0];

    int lower = std::max(0.0, (1.0 - sigma) * mean);
    int upper = std::min(255.0, (1.0 + sigma) * mean);
    cv::Canny(frame, frameAfterCanny, lower, upper);
}

void fillTriangleWithZeros(cv::Mat& grayFrame, const cv::Point& point0, const cv::Point& point1, const cv::Point& point2) {
    cv::Size size = grayFrame.size();
    cv::Mat mask = cv::Mat::zeros(size, CV_8U);
    std::vector<cv::Point> points = {point0, point1, point2};
    cv::fillConvexPoly(mask, points, cv::Scalar(255), 8, 0);
    grayFrame.setTo(cv::Scalar(0), mask);
}

cv::Mat RoadLaneDetectorCanny::cropRoiFromFrame(const cv::Mat& frame) {
    cv::Mat croppedFrame = frame.clone();
    croppedFrame.setTo(cv::Scalar(0), bottomCircleMask);
    fillTriangleWithZeros(croppedFrame, cv::Point(0, frame.rows * 0.8), cv::Point(0, 0), cv::Point(frame.cols / 2, 0));
    fillTriangleWithZeros(croppedFrame, cv::Point(frame.cols, frame.rows * 0.8), cv::Point(frame.cols, 0), cv::Point(frame.cols / 2, 0));
    // cv::imshow("cropped frame", croppedFrame);
    return croppedFrame;
}

float calculateAverageBrightness(const cv::Mat& grayFrame) {
    float sum = 0.0;
    int totalPixels = grayFrame.rows * grayFrame.cols;

    for (int y = 0; y < grayFrame.rows; y++) {
        for (int x = 0; x < grayFrame.cols; x++) {
            sum += grayFrame.at<uchar>(y, x);
        }
    }
    return sum / totalPixels;
}

cv::Mat findLaneInsideEdges(const cv::Mat& edgedFrame, const cv::Mat& grayFrame) {
    cv::Mat toReturn = edgedFrame;
    int distance = 10;
    float brightness = calculateAverageBrightness(grayFrame);
    for (int y = 0; y < edgedFrame.rows; y++) {
        for (int x = 0; x < edgedFrame.cols; x++) {
            if (edgedFrame.at<uchar>(y, x) > 0 && x < edgedFrame.cols / 2) {
                if (grayFrame.at<uchar>(y, x - distance) < grayFrame.at<uchar>(y, x + distance) ||
                    grayFrame.at<uchar>(y, x - distance) > grayFrame.at<uchar>(y, x + distance) && (int)grayFrame.at<uchar>(y, x - distance) - grayFrame.at<uchar>(y, x + distance) < 2 * brightness) {
                    // std::cout << brightness << std::endl;
                    // std::cout << (int) grayFrame.at<uchar>(y, x - distance) << " " << (int) grayFrame.at<uchar>(y, x + distance) << std::endl;
                    toReturn.at<uchar>(y, x) = 0;
                }
            } else if (edgedFrame.at<uchar>(y, x) > 0 && x > edgedFrame.cols / 2) {
                if (grayFrame.at<uchar>(y, x + distance) < grayFrame.at<uchar>(y, x - distance) ||
                    grayFrame.at<uchar>(y, x + distance) > grayFrame.at<uchar>(y, x - distance) && (int)grayFrame.at<uchar>(y, x + distance) - grayFrame.at<uchar>(y, x - distance) < 2 * brightness) {
                    toReturn.at<uchar>(y, x) = 0;
                }
            }
        }
    }
    return toReturn;
}

cv::Mat removeHorizontallEdges(const cv::Mat& edgedFrame) {
    cv::Mat toReturn = edgedFrame;
    for (int y = 0; y < edgedFrame.rows; y++) {
        for (int x = 0; x < edgedFrame.cols; x++) {
            if (edgedFrame.at<uchar>(y, x) > 0) {
                if (edgedFrame.at<uchar>(y, x - 1) == edgedFrame.at<uchar>(y, x + 1))
                    toReturn.at<uchar>(y, x) = 0;
            }
        }
    }
    return toReturn;
}

cv::Mat RoadLaneDetectorCanny::filterEdges(const cv::Mat& edgedFrame, const cv::Mat& grayFrame) {
    cv::Mat insideEdges = findLaneInsideEdges(edgedFrame, grayFrame);
    return insideEdges;
}

cv::Mat RoadLaneDetectorCanny::preprocessFrame(const cv::Mat& frame) {
    cv::Mat greyFrame = convertFrameToGrayscale(frame);
    cv::medianBlur(greyFrame, greyFrame, 7);
    autoCanny(greyFrame);
    cv::Mat resultFrame = filterEdges(frameAfterCanny, greyFrame);
    resultFrame = cropRoiFromFrame(resultFrame);
    return resultFrame;
}

void drawDetectedLines(cv::Mat& image, const std::vector<cv::Vec4i>& lines, cv::Scalar color, int thickness = 10) {
    for (const cv::Vec4i& line : lines) {
        cv::line(image, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, thickness);
    }
}

std::vector<cv::Vec4i> RoadLaneDetectorCanny::detectLines(const cv::Mat& processedFrame) {
    std::vector<cv::Vec4i> detectedLines;
    cv::HoughLinesP(processedFrame, detectedLines, 2, CV_PI / 180, 50, 20, 50);
    // cv::Mat frame(processedFrame);
    // drawDetectedLines(frame, detectedLines, cv::Scalar(255, 255, 255));
    // cv::imshow("detected vertical lines", frame);
    // cv::waitKey(1);
    return detectedLines;
}

std::vector<cv::Vec4i> detectHorizontalLines(const cv::Mat& processedFrame) {
    std::vector<cv::Vec4i> detectedLines;
    cv::HoughLinesP(processedFrame, detectedLines, 2, CV_PI / 180, 50, 5, 40);
    cv::Mat frame(processedFrame);
    drawDetectedLines(frame, detectedLines, cv::Scalar(255, 255, 255));
    cv::imshow("detected lines", frame);
    // cv::waitKey(1);
    return detectedLines;
}

void RoadLaneDetectorCanny::classifyPoints(const std::vector<cv::Vec4i>& lines) {
    rightPoints.clear();
    leftPoints.clear();

    for (cv::Vec4i line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        if (std::abs(slope) < 0.5) {
            continue;
        } else {
            if (slope <= 0) {
                leftPoints.push_back(cv::Point(x1, y1));
                leftPoints.push_back(cv::Point(x2, y2));
            } else {
                rightPoints.push_back(cv::Point(x1, y1));
                rightPoints.push_back(cv::Point(x2, y2));
            }
        }
    }
}

void RoadLaneDetectorCanny::classifyHorizontalPoints(const std::vector<cv::Vec4i>& lines) {
    horizontalPoints.clear();
    for (cv::Vec4i line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        if (std::abs(slope) > 0.3) {
            continue;
        } else {
            horizontalPoints.push_back(cv::Point(x1, y1));
            horizontalPoints.push_back(cv::Point(x2, y2));
        }
    }
}

void RoadLaneDetectorCanny::findLanes() {
    if (rightVerticalLaneDetected)
        cv::fitLine(rightPoints, rightVerticalLane, cv::DIST_L2, 0, 0.01, 0.01);
    if (leftVerticalLaneDetected)
        cv::fitLine(leftPoints, leftVerticalLane, cv::DIST_L2, 0, 0.01, 0.01);
}

void RoadLaneDetectorCanny::findTurns() {
    cv::Rect upperRoi(0, 0, preprocessedFrame.cols, preprocessedFrame.rows * 0.40);  // Define the ROI as the upper 20% of the image
    preprocessedFrame(upperRoi) = cv::Scalar(0);
    cv::Rect lowerRoi(0, preprocessedFrame.rows * 0.65, preprocessedFrame.cols, preprocessedFrame.rows * 0.35);  // Define the ROI as the lower 35% of the image
    preprocessedFrame(lowerRoi) = cv::Scalar(0);
    std::vector<cv::Vec4i> detectedLines = detectLines(preprocessedFrame);
    std::vector<cv::Point> lp, rp;
    for (cv::Vec4i line : detectedLines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        if (std::abs(slope) < 0.5) {
            continue;
        } else {
            if (slope <= 0) {
                lp.push_back(cv::Point(x1, y1));
                lp.push_back(cv::Point(x2, y2));
            } else {
                rp.push_back(cv::Point(x1, y1));
                rp.push_back(cv::Point(x2, y2));
            }
        }
    }
    turnLeftDetected = lp.empty();
    turnRightDetected = rp.empty();
}

void calculateIntersection(const cv::Vec4f& line, double& y, double& x) {
    double t = (y - line[3]) / line[1];
    x = line[0] * t + line[2];
}

double RoadLaneDetectorCanny::calculateDecentering(const int& imageCols, const int& imageHeight) {
    double center_y = imageHeight * 0.5;
    double leftX, rightX;

    calculateIntersection(leftVerticalLane, center_y, leftX);
    calculateIntersection(rightVerticalLane, center_y, rightX);

    double centerX = (leftX + rightX) / 2.0;

    double centerImageX = imageCols / 2.0;
    double decentering = centerX - centerImageX;

    return decentering;
}

int RoadLaneDetectorCanny::getXPosition() {
    return xPosition;
}

std::pair<cv::Vec4f, cv::Vec4f> RoadLaneDetectorCanny::getLanes() {
    return std::make_pair(rightVerticalLane, leftVerticalLane);
}

cv::Vec4f convertToLineEquation(const cv::Vec4i& line) {
    cv::Vec4f lineEquation;
    float x1 = static_cast<float>(line[0]);
    float y1 = static_cast<float>(line[1]);
    float x2 = static_cast<float>(line[2]);
    float y2 = static_cast<float>(line[3]);

    // Calculate vx, vy
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = sqrt(dx * dx + dy * dy);
    float vx = dx / length;
    float vy = dy / length;

    // Calculate x0, y0
    float x0 = x1;
    float y0 = y1;

    lineEquation[0] = vx;
    lineEquation[1] = vy;
    lineEquation[2] = x0;
    lineEquation[3] = y0;

    return lineEquation;
}

cv::Point2f getIntersection(const cv::Vec4f& line1, const cv::Vec4f& line2) {
    // Extracting the components of the line equations
    float vx1 = line1[0], vy1 = line1[1], x0_1 = line1[2], y0_1 = line1[3];
    float vx2 = line2[0], vy2 = line2[1], x0_2 = line2[2], y0_2 = line2[3];

    // Computing the determinant
    float det = vx1 * vy2 - vx2 * vy1;
    if (fabs(det) < 1e-6) return cv::Point2f(-1, -1);  // Lines are parallel or coincident

    // Finding the intersection point
    float t = (vy2 * (x0_2 - x0_1) - vx2 * (y0_2 - y0_1)) / det;
    float intersectX = x0_1 + t * vx1;
    float intersectY = y0_1 + t * vy1;

    return cv::Point2f(intersectX, intersectY);
}

float distanceBetweenPoints(const cv::Point2f& point1, const cv::Point2f& point2) {
    return sqrt(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2));
}

void RoadLaneDetectorCanny::processFrame(const cv::Mat& frame) {
    preprocessedFrame = preprocessFrame(frame);
    std::vector<cv::Vec4i> detectedLines = detectLines(preprocessedFrame);
    classifyPoints(detectedLines);
    rightVerticalLaneDetected = rightPoints.size();
    leftVerticalLaneDetected = leftPoints.size();
    findLanes();
    if (rightVerticalLaneDetected && leftVerticalLaneDetected)
        xPosition = calculateDecentering(frame.cols, frame.rows);
    findTurns();
}

int RoadLaneDetectorCanny::getLeftDistance() {
    int a = leftVerticalLane[1] / leftVerticalLane[0];
    int b = leftVerticalLane[3] - (a * leftVerticalLane[2]);
    return ((frameHeight - b) / a);
}

int RoadLaneDetectorCanny::getRightDistance() {
    float a = rightVerticalLane[1] / rightVerticalLane[0];
    float b = rightVerticalLane[3] - (a * rightVerticalLane[2]);
    return frameWidth - ((frameHeight - b) / a);
}

cv::Vec4f RoadLaneDetectorCanny::getRightVerticalLane() {
    return rightVerticalLane;
}

cv::Vec4f RoadLaneDetectorCanny::getLeftVerticalLane() {
    return leftVerticalLane;
}

cv::Vec4f RoadLaneDetectorCanny::getLeftHorizontalBottomLane() {
    return leftHorizontalBottomLane;
}

cv::Vec4f RoadLaneDetectorCanny::getRightHorizontalBottomLane() {
    return rightHorizontalBottomLane;
}

cv::Vec4f RoadLaneDetectorCanny::getLeftHorizontalTopLane() {
    return leftHorizontalTopLane;
}

cv::Vec4f RoadLaneDetectorCanny::getRightHorizontalTopLane() {
    return rightHorizontalTopLane;
}

bool RoadLaneDetectorCanny::isRightVerticalLaneDetected() {
    return rightVerticalLaneDetected;
}

bool RoadLaneDetectorCanny::isLeftVerticalLaneDetected() {
    return leftVerticalLaneDetected;
}

bool RoadLaneDetectorCanny::isLeftHorizontalBottomLaneDetected() {
    return leftHorizontalBottomLaneDetected;
}

bool RoadLaneDetectorCanny::isRightHorizontalBottomLaneDetected() {
    return rightHorizontalBottomLaneDetected;
}

bool RoadLaneDetectorCanny::isLeftHorizontalTopLaneDetected() {
    return leftHorizontalTopLaneDetected;
}

bool RoadLaneDetectorCanny::isRightHorizontalTopLaneDetected() {
    return rightHorizontalTopLaneDetected;
}

bool RoadLaneDetectorCanny::isTurnLeftDetected() {
    return turnLeftDetected;
}

bool RoadLaneDetectorCanny::isTurnRightDetected() {
    return turnRightDetected;
}

int RoadLaneDetectorCanny::getLeftMidpointY() {
    return leftMidpointY;
}

int RoadLaneDetectorCanny::getRightMidpointY() {
    return rightMidpointY;
}