#include "road_lane_detector/road_lane_detector_contours.h"

cv::Mat extractOrangeColor(const cv::Mat& inputFrame) {
    cv::medianBlur(inputFrame, inputFrame, 5);
    cv::Mat hsvFrame, mask, result;
    cv::cvtColor(inputFrame, hsvFrame, cv::COLOR_BGR2HSV);
    cv::Scalar minHSV(0, 60, 40);
    cv::Scalar maxHSV(15, 255, 255);
    cv::inRange(hsvFrame, minHSV, maxHSV, mask);
    cv::bitwise_and(inputFrame, inputFrame, result, mask);

    return result;
}

cv::Mat getGrayFrameFromHsv(const cv::Mat& hsvFrame) {
    cv::Mat hsv_channels[3];
    cv::split(hsvFrame, hsv_channels);
    return hsv_channels[2];
}

cv::Mat croppRoiFromFrame(const cv::Mat& frame) {
    cv::Mat croppedFrame = frame.clone();
    croppedFrame(cv::Rect(0, 0, frame.cols, frame.rows * 0.3)) = cv::Scalar(0);
    return croppedFrame;
}

std::vector<std::vector<cv::Point>> findPointsVectors(const std::vector<std::vector<cv::Point>>& contours, const int& searchY = 250) {
    std::vector<std::vector<cv::Point>> foundPointsVectors(2);
    bool pointsFound = true;

    for (size_t i = 0; i < contours.size(); i++) {
        std::vector<cv::Point> foundPoints;
        std::copy_if(contours[i].begin(), contours[i].end(), std::back_inserter(foundPointsVectors[i]),
                     [searchY](const cv::Point& point) { return point.y == searchY; });
        if (!foundPointsVectors[i].size()) {
            pointsFound = false;
            break;
        }
    }

    if (!pointsFound) {
        throw std::runtime_error("No points found for one or more Y values.");
    }

    return foundPointsVectors;
}

double calculateDistance(const cv::Point& p1, const cv::Point& p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}

std::pair<cv::Point, cv::Point> findClosestPoints(const std::vector<cv::Point>& vector1, const std::vector<cv::Point>& vector2) {
    double minDistance = std::numeric_limits<double>::max();
    cv::Point closestPoint1, closestPoint2;
    for (const cv::Point& p1 : vector1) {
        for (const cv::Point& p2 : vector2) {
            double distance = calculateDistance(p1, p2);
            if (distance < minDistance) {
                minDistance = distance;
                closestPoint1 = p1;
                closestPoint2 = p2;
            }
        }
    }
    return std::make_pair(closestPoint1, closestPoint2);
}

int RoadLaneDetectorContours::getXPosition(const cv::Mat& frame) {
    cv::Mat orangeFrame = extractOrangeColor(croppRoiFromFrame(frame));
    cv::Mat grayFrame = getGrayFrameFromHsv(orangeFrame);
    bool pointsDetected = false;
    int lineY = grayFrame.rows * 0.6;
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(grayFrame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    contours.erase(std::remove_if(contours.begin(), contours.end(),
                                  [](const std::vector<cv::Point>& contour) {
                                      return cv::contourArea(contour) <= 750;
                                  }),
                   contours.end());
    if (contours.size() != 2) {
        throw std::runtime_error("Number of contours should equal 2: " + contours.size());
    }
    std::vector<std::vector<cv::Point>> foundPointsVectors(2);
    for (int i = 0; i < 10; ++i, lineY -= 5) {
        try {
            foundPointsVectors = findPointsVectors(contours, lineY);
            pointsDetected = true;
        } catch (std::runtime_error er) {
            continue;
        }
    }
    if (!pointsDetected) {
        throw std::runtime_error("No points detected");
    }
    std::pair<cv::Point, cv::Point> result = findClosestPoints(foundPointsVectors[0], foundPointsVectors[1]);
    int frameVerticallCenter = frame.cols / 2;
    int lineVerticallCenter = (result.first.x + result.second.x) / 2;
    int verticalOffset = lineVerticallCenter - frameVerticallCenter;
    return verticalOffset;
}

std::pair<cv::Vec4i, cv::Vec4i> RoadLaneDetectorContours::detectLane(const cv::Mat& frame) {
        return std::pair<cv::Vec4i, cv::Vec4i>();
}

void RoadLaneDetectorContours::processFrame(const cv::Mat frame) {
    return;
}