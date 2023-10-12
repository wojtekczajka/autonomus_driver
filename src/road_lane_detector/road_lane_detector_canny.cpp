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

    cv::Rect topRoi(0, 0, frame.cols, static_cast<int>(frame.rows * 0.4));
    cv::Rect bottomRoi(0, static_cast<int>(frame.rows * 0.9), frame.cols, static_cast<int>(frame.rows * 0.1));
    cv::Rect leftRoi(0, 0, static_cast<int>(frame.cols * 0.1), frame.rows);

    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8U);
    std::vector<cv::Point> trianglePoints;
    trianglePoints.push_back(cv::Point(0, 0));
    trianglePoints.push_back(cv::Point(0, frame.cols / 2));
    trianglePoints.push_back(cv::Point(frame.cols / 2, 0));
    cv::fillConvexPoly(mask, trianglePoints, cv::Scalar(255));

    croppedFrame.setTo(0, mask);
    croppedFrame(topRoi) = cv::Scalar(0);
    croppedFrame(bottomRoi) = cv::Scalar(0);
    croppedFrame(leftRoi) = cv::Scalar(0);
    
    return croppedFrame;
}

cv::Mat preprocessFrame(const cv::Mat& frame) {
    cv::Mat resultFrame = convertFrameToGrayscale(frame);
    cv::equalizeHist(resultFrame, resultFrame);
    cv::threshold(resultFrame, resultFrame, 225, 255, cv::THRESH_BINARY);
    GaussianBlur(resultFrame, resultFrame, cv::Size(3, 3), 0);
    resultFrame = autoCanny(resultFrame);
    int kernelSize = 5;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(resultFrame, resultFrame, cv::MORPH_CLOSE, kernel);
    // cv::imshow("canny frame", resultFrame);
    resultFrame = cropRoiFromFrame(resultFrame);
    cv::imshow("canny frame after roi", resultFrame);
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
    cv::HoughLinesP(frame, detectedLines, 1, CV_PI / 180, 10, 20, 10);
    drawDetectedLines(frame, detectedLines, cv::Scalar(255, 255, 255));
    cv::imshow("lines", frame);
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

// Function to calculate the intersection point of a line with a horizontal line at a specific height
void calculateIntersection(const cv::Vec4i& line, double y, double& x) {
    double t = (y - line[3]) / line[1];
    x = line[0] * t + line[2];
}

// Function to calculate the decentering relative to the center of the image
double calculateDecentering(const cv::Vec4i& leftLine, const cv::Vec4i& rightLine, const int& imageCols, const int imageHeight) {
    double center_y = imageHeight * 0.8;
    double leftX, rightX;

    // Calculate the intersection points for the left and right lines
    calculateIntersection(leftLine, center_y, leftX);
    calculateIntersection(rightLine, center_y, rightX);

    // Calculate the midpoint between the two intersection points
    double centerX = (leftX + rightX) / 2.0;

    // Calculate the decentered value relative to the center of the image
    double centerImageX = imageCols / 2.0;
    double decentering = centerX - centerImageX;

    return decentering;
}

int RoadLaneDetectorCanny::getXPosition(const cv::Mat& frame) {
    return xPosition;
}

std::pair<cv::Vec4i, cv::Vec4i> RoadLaneDetectorCanny::getLanes() {
    return lanes;
}

void RoadLaneDetectorCanny::processFrame(const cv::Mat frame) {
    lanes = detectLane(frame);
    xPosition = calculateDecentering(lanes.first, lanes.second, frame.cols, frame.rows);
}