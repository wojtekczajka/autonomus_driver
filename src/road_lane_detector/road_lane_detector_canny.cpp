#include "road_lane_detector/road_lane_detector_canny.h"

#include "common/config_parser.h"

RoadLaneDetectorCanny::RoadLaneDetectorCanny() : xPosition(0),
                                                 rightVerticalLaneDetected(false),
                                                 leftVerticalLaneDetected(false),
                                                 turnLeftDetected(false),
                                                 turnRightDetected(false) {
    ConfigParser configParser("camera_config.txt");
    frameWidth = configParser.getValue<int>("FRAME_WIDTH", 640);
    frameHeight = configParser.getValue<int>("FRAME_HEIGHT", 368);
    int radius = frameWidth / 2;
    bottomCircleMask = cv::Mat::zeros(cv::Point(frameWidth, frameHeight), CV_8U);
    cv::circle(bottomCircleMask, cv::Point(frameWidth / 2, frameHeight), radius / 3.5, cv::Scalar(255), -1);
    upperRoi = cv::Rect(0, 0, frameWidth, frameHeight * 0.65);
    lowerRoi = cv::Rect(0, frameHeight * 0.80, frameWidth, frameHeight * 0.20);
}

cv::Mat convertFrameToGrayscale(const cv::Mat& frame) {
    cv::Mat grayscaleFrame;
    cv::cvtColor(frame, grayscaleFrame, cv::COLOR_BGR2GRAY);
    return grayscaleFrame;
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
    return croppedFrame;
}

void RoadLaneDetectorCanny::autoCanny(const cv::Mat& frame, double sigma) {
    cv::Scalar v = cv::mean(frame);
    double mean = v.val[0];

    int lower = std::max(0.0, (1.0 - sigma) * mean);
    int upper = std::min(255.0, (1.0 + sigma) * mean);
    cv::Canny(frame, frameAfterCanny, lower, upper);
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

void RoadLaneDetectorCanny::findLanes() {
    if (rightVerticalLaneDetected)
        cv::fitLine(rightPoints, rightVerticalLane, cv::DIST_L2, 0, 0.01, 0.01);
    if (leftVerticalLaneDetected)
        cv::fitLine(leftPoints, leftVerticalLane, cv::DIST_L2, 0, 0.01, 0.01);
}

void RoadLaneDetectorCanny::findTurns() {
    cv::Mat tempFrame = preprocessedFrame.clone();
    tempFrame(upperRoi) = cv::Scalar(0);
    tempFrame(lowerRoi) = cv::Scalar(0);

    std::vector<cv::Vec4i> detectedLines = detectLines(tempFrame);
    classifyPoints(detectedLines);

    turnLeftDetected = leftPoints.empty();
    turnRightDetected = rightPoints.empty();
}

void RoadLaneDetectorCanny::findRoadForwardBeyondIntersectionDetected() {
    cv::Mat tempFrame = preprocessedFrame.clone();
    int blackenHeight = static_cast<int>(tempFrame.rows * 0.40);
    cv::Rect blackenRoi(0, tempFrame.rows - blackenHeight, tempFrame.cols, blackenHeight);
    tempFrame(blackenRoi) = cv::Scalar(0);

    std::vector<cv::Vec4i> detectedLines = detectLines(tempFrame);
    classifyPoints(detectedLines);

    roadForwardBeyondIntersectionDetected = leftPoints.size() && rightPoints.size();
}

void calculateIntersection(const cv::Vec4f& line, const double& y, double& x) {
    double t = (y - line[3]) / line[1];
    x = line[0] * t + line[2];
}

double RoadLaneDetectorCanny::calculateDecentering(const int& imageCols, const int& imageHeight) {
    double center_y = imageHeight * 0.5;
    double leftX, rightX, leftXBottom, rightXBottom;

    calculateIntersection(leftVerticalLane, center_y, leftX);
    calculateIntersection(rightVerticalLane, center_y, rightX);
    calculateIntersection(leftVerticalLane, imageHeight, leftXBottom);
    calculateIntersection(rightVerticalLane, imageHeight, rightXBottom);

    double centerX = (leftX + rightX) / 2.0;

    double centerImageX = imageCols / 2.0;
    double decentering = centerX - centerImageX;

    if (std::abs(leftX - rightX) > std::abs(leftXBottom - rightXBottom)) {
        rightVerticalLaneDetected = false;
        leftVerticalLaneDetected = false;
    }

    return decentering;
}

int RoadLaneDetectorCanny::getXPosition() {
    return xPosition;
}

std::pair<cv::Vec4f, cv::Vec4f> RoadLaneDetectorCanny::getLanes() {
    return std::make_pair(rightVerticalLane, leftVerticalLane);
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
    // findTurns();
    // findRoadForwardBeyondIntersectionDetected();
}

cv::Vec4f RoadLaneDetectorCanny::getRightVerticalLane() {
    return rightVerticalLane;
}

cv::Vec4f RoadLaneDetectorCanny::getLeftVerticalLane() {
    return leftVerticalLane;
}

bool RoadLaneDetectorCanny::isRightVerticalLaneDetected() {
    return rightVerticalLaneDetected;
}

bool RoadLaneDetectorCanny::isLeftVerticalLaneDetected() {
    return leftVerticalLaneDetected;
}

bool RoadLaneDetectorCanny::isTurnLeftDetected() {
    return turnLeftDetected;
}

bool RoadLaneDetectorCanny::isTurnRightDetected() {
    return turnRightDetected;
}

bool RoadLaneDetectorCanny::isRoadForwardBeyondIntersectionDetected() {
    return roadForwardBeyondIntersectionDetected;
}