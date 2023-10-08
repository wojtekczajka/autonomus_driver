#include "road_lane_detector/road_lane_detector_canny.h"

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

cv::Mat cropRoiFromFrame(const cv::Mat& frame) {
    cv::Mat croppedFrame = frame.clone();
    croppedFrame(cv::Rect(0, 0, frame.cols, frame.rows * 0.3)) = cv::Scalar(0);
    return croppedFrame;
}

cv::Mat preprocessFrame(const cv::Mat& frame) {
    cv::Mat resultFrame = convertFrameToGrayscale(frame);
    cv::medianBlur(resultFrame, resultFrame, 15);
    resultFrame = autoCanny(resultFrame);
    resultFrame = cropRoiFromFrame(resultFrame);

    return resultFrame;
}

std::vector<cv::Vec4i> detectLines(const cv::Mat& processedFrame) {
    std::vector<cv::Vec4i> detectedLines;
    cv::HoughLinesP(processedFrame, detectedLines, 6, CV_PI / 180, 110, 30, 10);
    // visualizeLines(processedFrame, detectedLines);
    return detectedLines;
}

int RoadLaneDetectorCanny::getXPosition(const cv::Mat& frame) {
    int xPosition = 0;
    return 0;
}

std::pair<std::vector<cv::Point>, std::vector<cv::Point>> findRightAndLeftPoints(const std::vector<cv::Vec4i>& lines) {
    std::vector<cv::Point> right_points;
    std::vector<cv::Point> left_points;

    for (cv::Vec4i line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        if (std::abs(slope) < 0.5 || std::abs(slope) > 10) {
            continue;
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

std::pair<cv::Vec4i, cv::Vec4i> findLeftAndRightLine(const std::pair<std::vector<cv::Point>, std::vector<cv::Point>>& rightAndLeftPoints) {
    cv::Vec4i leftLine, rightLine;
    if (rightAndLeftPoints.first.size() > 1) {
        fitLine(rightAndLeftPoints.first, rightLine, cv::DIST_L2, 0, 0.01, 0.01);
    } else {
        std::cout << "RIGHT LINE NOT FOUND, RETURNING LAST DETECTED" << std::endl;
    }

    if (rightAndLeftPoints.second.size() > 1) {
        fitLine(rightAndLeftPoints.second, leftLine, cv::DIST_L2, 0, 0.01, 0.01);
    } else {
        std::cout << "LEFT LINE NOT FOUND, RETURNING LAST DETECTED" << std::endl;
    }

    return std::make_pair(rightLine, leftLine);
}

std::pair<cv::Vec4i, cv::Vec4i> RoadLaneDetectorCanny::detectLane(const cv::Mat& frame) {
    cv::Mat preprocessedFrame = preprocessFrame(frame);
    std::vector<cv::Vec4i> detectedLines = detectLines(preprocessedFrame);
    std::pair<std::vector<cv::Point>, std::vector<cv::Point>> rightAndLeftPoints = findRightAndLeftPoints(detectedLines);
    return findLeftAndRightLine(rightAndLeftPoints);
}

void processFrame(const cv::Mat frame) {
    return;
}