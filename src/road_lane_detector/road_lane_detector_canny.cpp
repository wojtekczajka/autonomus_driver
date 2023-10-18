#include "road_lane_detector/road_lane_detector_canny.h"

#include "common/config_parser.h"

RoadLaneDetectorCanny::RoadLaneDetectorCanny() : xPosition(0),
                                                 rightVerticalLaneDetected(false),
                                                 leftVerticalLaneDetected(false),
                                                 topHorizontalLaneDetected(false) {
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

cv::Mat RoadLaneDetectorCanny::cropRoiFromFrame(const cv::Mat& frame) {
    cv::Mat croppedFrame = frame.clone();
    croppedFrame.setTo(cv::Scalar(0), topCircleMask);    
    croppedFrame.setTo(cv::Scalar(0), bottomCircleMask);

    return croppedFrame;
}

cv::Mat RoadLaneDetectorCanny::preprocessFrame(const cv::Mat& frame) {
    cv::Mat resultFrame = convertFrameToGrayscale(frame);
    cv::imshow("cropped frame", cropRoiFromFrame(resultFrame));
    cv::equalizeHist(resultFrame, resultFrame);
    GaussianBlur(resultFrame, resultFrame, cv::Size(5, 5), 0);
    cv::threshold(resultFrame, resultFrame, 235, 255, cv::THRESH_BINARY);

    resultFrame = autoCanny(resultFrame);
    resultFrame = cropRoiFromFrame(resultFrame);
    
    return resultFrame;
}

void drawDetectedLines(cv::Mat& image, const std::vector<cv::Vec4i>& lines, cv::Scalar color, int thickness = 10) {
    for (const cv::Vec4i& line : lines) {
        cv::line(image, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, thickness);
    }
}

std::vector<cv::Vec4i> detectLines(const cv::Mat& processedFrame) {
    std::vector<cv::Vec4i> detectedLines;
    cv::HoughLinesP(processedFrame, detectedLines, 1, CV_PI / 180, 30, 30, 250);

    cv::Mat frame(processedFrame);
    // drawDetectedLines(frame, detectedLines, cv::Scalar(255, 255, 255));
    // cv::imshow("lines", frame);

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

void calculateIntersection(const cv::Vec4i& line, double y, double& x) {
    double t = (y - line[3]) / line[1];
    x = line[0] * t + line[2];
}

double RoadLaneDetectorCanny::calculateDecentering(const int& imageCols, const int& imageHeight) {
    double center_y = imageHeight * 0.8;
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

std::pair<cv::Vec4i, cv::Vec4i> RoadLaneDetectorCanny::getLanes() {
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

cv::Vec4i RoadLaneDetectorCanny::getRightVerticalLane() {
    return rightVerticalLane;
}

cv::Vec4i RoadLaneDetectorCanny::getLeftVerticalLane() {
    return leftVerticalLane;
}

cv::Vec4i RoadLaneDetectorCanny::getTopHorizontalLane() {
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