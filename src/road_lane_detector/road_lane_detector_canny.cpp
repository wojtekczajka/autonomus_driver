#include "road_lane_detector/road_lane_detector_canny.h"

#include "common/config_parser.h"

RoadLaneDetectorCanny::RoadLaneDetectorCanny(IFrameDispatcherClient& frameDispatcherClient) : frameDispatcherClient(frameDispatcherClient), xPosition(0), rightVerticalLaneDetected(false), leftVerticalLaneDetected(false), topHorizontalLaneDetected(false) {
    ConfigParser configParser("camera_config.txt");
    frameWidth = configParser.getValue<int>("FRAME_WIDTH", 640);
    frameHeight = configParser.getValue<int>("FRAME_HEIGHT", 368);
    int radius = frameWidth / 2;
    topCircleMask = cv::Mat::ones(cv::Point(frameWidth, frameHeight), CV_8U) * 255;
    cv::circle(topCircleMask, cv::Point(frameWidth / 2, frameHeight + 20), radius, cv::Scalar(0), -1);
    bottomCircleMask = cv::Mat::zeros(cv::Point(frameWidth, frameHeight), CV_8U);
    cv::circle(bottomCircleMask, cv::Point(frameWidth / 2, frameHeight), radius / 3.5, cv::Scalar(255), -1);
}

cv::Mat convertFrameToGrayscale(const cv::Mat& frame) {
    cv::Mat grayscaleFrame;
    cv::cvtColor(frame, grayscaleFrame, cv::COLOR_BGR2GRAY);
    return grayscaleFrame;
}

cv::Mat autoCanny(const cv::Mat& frame, double sigma = 0.33) {
    cv::Scalar v = cv::mean(frame);
    double mean = v.val[0];

    int lower = std::max(0.0, (1.0 - sigma) * mean);
    int upper = std::min(255.0, (1.0 + sigma) * mean);
    cv::Mat edged;
    cv::Canny(frame, edged, lower, upper);

    return edged;
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

    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8U);
    int halfHeight = croppedFrame.rows * 0.4;
    mask.rowRange(0, halfHeight).setTo(255);
    croppedFrame.setTo(0, mask);
    croppedFrame.setTo(cv::Scalar(0), bottomCircleMask);
    fillTriangleWithZeros(croppedFrame, cv::Point(0, frame.rows), cv::Point(0, 0), cv::Point(frame.cols / 2, 0));
    fillTriangleWithZeros(croppedFrame, cv::Point(frame.cols, frame.rows), cv::Point(frame.cols, 0), cv::Point(frame.cols / 2, 0));
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
                if (grayFrame.at<uchar>(y, x - distance) < grayFrame.at<uchar>(y, x + distance)) {
                    // std::cout << brightness << std::endl;
                    // std::cout << (int) grayFrame.at<uchar>(y, x - distance) << " " << (int) grayFrame.at<uchar>(y, x + distance) << std::endl;
                    toReturn.at<uchar>(y, x) = 0;
                } else if (grayFrame.at<uchar>(y, x - distance) > grayFrame.at<uchar>(y, x + distance) && (int) grayFrame.at<uchar>(y, x - distance) - grayFrame.at<uchar>(y, x + distance) < 2 * brightness) {
                    toReturn.at<uchar>(y, x) = 0;
                }
            } else if (edgedFrame.at<uchar>(y, x) > 0 && x > edgedFrame.cols / 2) {
                if (grayFrame.at<uchar>(y, x + distance) < grayFrame.at<uchar>(y, x - distance)) {
                    toReturn.at<uchar>(y, x) = 0;
                } else if (grayFrame.at<uchar>(y, x + distance) > grayFrame.at<uchar>(y, x - distance) && (int) grayFrame.at<uchar>(y, x + distance) - grayFrame.at<uchar>(y, x - distance) < 2 * brightness) {
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

cv::Mat RoadLaneDetectorCanny::filterEdges(cv::Mat& edgedFrame, const cv::Mat& grayFrame) {
    cv::Mat insideEdges = findLaneInsideEdges(edgedFrame, grayFrame);
    // frameDispatcherClient.sendFrame(insideEdges, "inside");
    return insideEdges;
    // return removeHorizontallEdges(insideEdges);
}

cv::Mat RoadLaneDetectorCanny::preprocessFrame(const cv::Mat& frame) {
    cv::Mat greyFrame = convertFrameToGrayscale(frame);
    frameDispatcherClient.sendFrame(cropRoiFromFrame(greyFrame), "cropped frame");
    cv::medianBlur(greyFrame, greyFrame, 11);
    cv::Mat resultFrame = greyFrame;
    resultFrame = autoCanny(resultFrame);
    resultFrame = filterEdges(resultFrame, greyFrame);
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
    cv::Mat frame(processedFrame);
    drawDetectedLines(frame, detectedLines, cv::Scalar(255, 255, 255));
    frameDispatcherClient.sendFrame(frame, "detected lines");
    return detectedLines;
}

void RoadLaneDetectorCanny::classifyPoints(const std::vector<cv::Vec4i>& lines) {
    rightPoints.clear();
    leftPoints.clear();
    horizontalPoints.clear();

    for (cv::Vec4i line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        if (std::abs(slope) < 0.5) {
            horizontalPoints.push_back(cv::Point(x1, y1));
            horizontalPoints.push_back(cv::Point(x2, y2));
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
    if (topHorizontalLaneDetected)
        cv::fitLine(horizontalPoints, topHorizontalLane, cv::DIST_L2, 0, 0.01, 0.01);
}

void calculateIntersection(const cv::Vec4f& line, double& y, double& x) {
    double t = (y - line[3]) / line[1];
    x = line[0] * t + line[2];
}

double RoadLaneDetectorCanny::calculateDecentering(const int& imageCols, const int& imageHeight) {
    double center_y = imageHeight;
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

void RoadLaneDetectorCanny::processFrame(const cv::Mat frame) {
    cv::Mat preprocessedFrame = preprocessFrame(frame);
    std::vector<cv::Vec4i> detectedLines = detectLines(preprocessedFrame);
    classifyPoints(detectedLines);
    rightVerticalLaneDetected = rightPoints.size();
    leftVerticalLaneDetected = leftPoints.size();
    topHorizontalLaneDetected = horizontalPoints.size();
    findLanes();
    if (rightVerticalLaneDetected && leftVerticalLaneDetected)
        xPosition = calculateDecentering(frame.cols, frame.rows);
}

cv::Vec4f RoadLaneDetectorCanny::getRightVerticalLane() {
    return rightVerticalLane;
}

cv::Vec4f RoadLaneDetectorCanny::getLeftVerticalLane() {
    return leftVerticalLane;
}

cv::Vec4f RoadLaneDetectorCanny::getTopHorizontalLane() {
    return topHorizontalLane;
}

bool RoadLaneDetectorCanny::isRightVerticalLaneDetected() {
    return rightVerticalLaneDetected;
}

bool RoadLaneDetectorCanny::isLeftVerticalLaneDetected() {
    return leftVerticalLaneDetected;
}

bool RoadLaneDetectorCanny::isTopHorizontalLaneDetected() {
    return topHorizontalLaneDetected;
}