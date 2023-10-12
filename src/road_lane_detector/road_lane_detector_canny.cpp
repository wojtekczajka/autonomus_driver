#include "road_lane_detector/road_lane_detector_canny.h"

RoadLaneDetectorCanny::RoadLaneDetectorCanny() : xPosition(0),
                                                 rightVerticalLane(false),
                                                 leftVerticalLane(false),
                                                 mask(cv::Mat::zeros(cv::Size(640, 368), CV_8U)),
                                                 trianglePoints({cv::Point(0, 0), cv::Point(0, 368 * 0.8), cv::Point(640 / 2, 0)}),
                                                 maskedTopRectangle(0, 0, 640, static_cast<int>(368 * 0.4)),
                                                 maskedBottomRectangle(0, static_cast<int>(368 * 0.9), 640, static_cast<int>(368 * 0.1)),
                                                 maskedLeftRectangle(0, 0, static_cast<int>(640 * 0.1), 368) {}

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

    cv::fillConvexPoly(mask, trianglePoints, cv::Scalar(255));

    croppedFrame.setTo(0, mask);
    croppedFrame(maskedTopRectangle) = cv::Scalar(0);
    croppedFrame(maskedBottomRectangle) = cv::Scalar(0);
    croppedFrame(maskedLeftRectangle) = cv::Scalar(0);

    return croppedFrame;
}

cv::Mat RoadLaneDetectorCanny::preprocessFrame(const cv::Mat& frame) {
    cv::Mat resultFrame = convertFrameToGrayscale(frame);
    cv::equalizeHist(resultFrame, resultFrame);
    GaussianBlur(resultFrame, resultFrame, cv::Size(5, 5), 0);
    cv::threshold(resultFrame, resultFrame, 225, 255, cv::THRESH_BINARY);
    resultFrame = autoCanny(resultFrame);
    int kernelSize = 5;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(resultFrame, resultFrame, cv::MORPH_CLOSE, kernel);
    resultFrame = cropRoiFromFrame(resultFrame);
    return resultFrame;
}

void drawDetectedLines(cv::Mat& image, const std::vector<cv::Vec4i>& lines, cv::Scalar color, int thickness = 10) {
    for (const cv::Vec4i& line : lines) {
        cv::line(image, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), color, thickness);
    }
}

std::vector<cv::Vec4i> detectLines(const cv::Mat& processedFrame) {
    cv::Mat frame(processedFrame);
    std::vector<cv::Vec4i> detectedLines;
    cv::HoughLinesP(frame, detectedLines, 1, CV_PI / 180, 20, 20, 3);
    // drawDetectedLines(frame, detectedLines, cv::Scalar(255, 255, 255));
    // cv::imshow("lines", frame);
    return detectedLines;
}

std::pair<std::vector<cv::Point>, std::vector<cv::Point>> findRightAndLeftPoints(const std::vector<cv::Vec4i>& lines) {
    std::vector<cv::Point> right_points;
    std::vector<cv::Point> left_points;

    for (cv::Vec4i line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        if (std::abs(slope) < 0.5) {
            continue;
            // todo horizontall line...
        }
        if (slope <= 0) {
            left_points.push_back(cv::Point(x1, y1));
            left_points.push_back(cv::Point(x2, y2));
        } else {
            right_points.push_back(cv::Point(x1, y1));
            right_points.push_back(cv::Point(x2, y2));
        }
    }
    return std::make_pair(right_points, left_points);
}

void RoadLaneDetectorCanny::findLeftAndRightLine(const std::pair<std::vector<cv::Point>, std::vector<cv::Point>>& rightAndLeftPoints) {
    if (rightLaneDetected)
        fitLine(rightAndLeftPoints.first, rightVerticalLane, cv::DIST_L2, 0, 0.01, 0.01);
    if (leftLaneDetected)
        fitLine(rightAndLeftPoints.second, leftVerticalLane, cv::DIST_L2, 0, 0.01, 0.01);
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

int RoadLaneDetectorCanny::getXPosition(const cv::Mat& frame) {
    return xPosition;
}

std::pair<cv::Vec4i, cv::Vec4i> RoadLaneDetectorCanny::getLanes() {
    return std::make_pair(rightVerticalLane, leftVerticalLane);
}

void RoadLaneDetectorCanny::processFrame(const cv::Mat frame) {
    cv::Mat preprocessedFrame = preprocessFrame(frame);
    std::vector<cv::Vec4i> detectedLines = detectLines(preprocessedFrame);
    std::pair<std::vector<cv::Point>, std::vector<cv::Point>> rightAndLeftPoints = findRightAndLeftPoints(detectedLines);
    rightLaneDetected = rightAndLeftPoints.first.size();
    leftLaneDetected = rightAndLeftPoints.second.size();
    findLeftAndRightLine(rightAndLeftPoints);
    if (rightLaneDetected && leftLaneDetected)
        xPosition = calculateDecentering(frame.cols, frame.rows);
}