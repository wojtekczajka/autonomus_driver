#include "road_line_detector/road_line_detector.h"

RoadLineDetector::RoadLineDetector(Logger& logger) : logger(logger) {}

RoadLineDetector::~RoadLineDetector() {}

std::pair<cv::Point2f, cv::Point2f> vectorToLinePoints(const cv::Vec4f& vector, double min_y, double max_y) {
    float vx = vector[0];
    float vy = vector[1];
    float x0 = vector[2];
    float y0 = vector[3];

    float x1 = x0 + (min_y - y0) * vx / vy;
    float x2 = x0 + (max_y - y0) * vx / vy;

    cv::Point2f startPoint(x1, min_y);
    cv::Point2f endPoint(x2, max_y);

    return std::make_pair(startPoint, endPoint);
}

cv::Mat autoCanny(const cv::Mat& frame, double sigma = 0.33) {
    cv::Scalar v = cv::mean(frame);
    double median = v.val[0];

    int lower = std::max(0.0, (1.0 - sigma) * median);
    int upper = std::min(255.0, (1.0 + sigma) * median);
    cv::Mat edged;
    cv::Canny(frame, edged, lower, upper);

    return edged;
}

cv::Mat RoadLineDetector::preprocessFrame(const cv::Mat& frame) {
    cv::Mat greyscaleFrame = convertFrameToGrayscale(frame);
    cv::Mat temp = greyscaleFrame;

    greyscaleFrame = cropRoiFromFrame(greyscaleFrame);
    cv::medianBlur(greyscaleFrame, greyscaleFrame, 23);

    cv::Mat resultFrame = autoCanny(greyscaleFrame);
    // cv::imshow("canny", resultFrame);
    // cv::waitKey(1);

    return resultFrame;
}

std::vector<cv::Vec4i> RoadLineDetector::detectLines(const cv::Mat& processedFrame) {
    std::vector<cv::Vec4i> detectedLines;
    cv::HoughLinesP(processedFrame, detectedLines, 6, CV_PI / 120, 110, 30, 10);
    // visualizeLines(processedFrame, detectedLines);
    return detectedLines;
}

cv::Mat RoadLineDetector::visualizeLines(const cv::Mat& frame, const std::vector<cv::Vec4i>& lines) {
    cv::Mat frameWithLines = frame.clone();
    for (size_t i = 0; i < lines.size(); i++) {
        cv::Vec4i l = lines[i];
        line(frameWithLines, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    }

    std::cout << "Detected lines: " << lines.size() << std::endl;
    cv::imshow("detected lines", frameWithLines);
    cv::waitKey(1);

    return frameWithLines;
}

std::pair<std::vector<cv::Point>, std::vector<cv::Point>> RoadLineDetector::findRightAndLeftPoints(const std::vector<cv::Vec4i>& lines) {
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

// line – Output line parameters. In case of 2D fitting, it should be a vector of 4 elements (like Vec4f) - (vx, vy, x0, y0),
// where (vx, vy) is a normalized vector collinear to the line and (x0, y0) is a point on the line.
std::pair<cv::Vec4i, cv::Vec4i> RoadLineDetector::findLeftAndRightLine(const std::pair<std::vector<cv::Point>, std::vector<cv::Point>>& rightAndLeftPoints) {
    if (rightAndLeftPoints.first.size() > 1) {
        fitLine(rightAndLeftPoints.first, lastDetectedRightLine, cv::DIST_L2, 0, 0.01, 0.01);
    } else {
        logger.error("RIGHT LINE NOT FOUND, RETURNING LAST DETECTED");
    }
    
    if (rightAndLeftPoints.second.size() > 1) {
        fitLine(rightAndLeftPoints.second, lastDetectedLeftLine, cv::DIST_L2, 0, 0.01, 0.01);
    } else {
        logger.error("LEFT LINE NOT FOUND, RETURNING LAST DETECTED");
    }

    return std::make_pair(lastDetectedRightLine, lastDetectedLeftLine);
}

std::pair<cv::Vec4i, cv::Vec4i> RoadLineDetector::getRightAndLeftRoadLine(const cv::Mat& frame) {
    cv::Mat processedFrame = preprocessFrame(frame);
    std::vector<cv::Vec4i> detectedLines = detectLines(processedFrame);
    std::pair<std::vector<cv::Point>, std::vector<cv::Point>> rightAndLeftPoints = findRightAndLeftPoints(detectedLines);
    return findLeftAndRightLine(rightAndLeftPoints);
}

cv::Mat RoadLineDetector::convertFrameToGrayscale(const cv::Mat& frame) {
    cv::Mat grayscaleFrame;
    cv::cvtColor(frame, grayscaleFrame, cv::COLOR_BGR2GRAY);
    return grayscaleFrame;
}

cv::Mat RoadLineDetector::cropRoiFromFrame(const cv::Mat& frame) {
    cv::Mat croppedFrame = frame.clone();
    croppedFrame(cv::Rect(0, 0, frame.cols, frame.rows * 0.5)) = cv::Scalar(0);
    return croppedFrame;
}