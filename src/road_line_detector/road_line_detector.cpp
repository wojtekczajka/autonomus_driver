#include "road_line_detector/road_line_detector.h"

RoadLineDetector::RoadLineDetector() {}

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
    std::cout << lower << " " << upper << std::endl;

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
    cv::imshow("canny", resultFrame);
    cv::waitKey(1);

    return resultFrame;
}

std::vector<cv::Vec4i> RoadLineDetector::detectLines(const cv::Mat& processedFrame) {
    std::vector<cv::Vec4i> detectedLines;
    cv::HoughLinesP(processedFrame, detectedLines, 6, CV_PI / 60, 110, 30, 10);

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
    cv::waitKey(0);

    return frameWithLines;
}

std::pair<std::vector<cv::Point>, std::vector<cv::Point>> RoadLineDetector::findRightAndLeftPoints(const std::vector<cv::Vec4i>& lines) {
    std::vector<cv::Point> right_points;
    std::vector<cv::Point> left_points;

    for (cv::Vec4i line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        std::cout << slope << std::endl;
        if (std::abs(slope) < 0.5 || std::isinf(slope)) {
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
    cv::Vec4i right_line;
    cv::Vec4i left_line;
    try {
        fitLine(rightAndLeftPoints.first, right_line, cv::DIST_L2, 0, 0.01, 0.01);
        fitLine(rightAndLeftPoints.second, left_line, cv::DIST_L2, 0, 0.01, 0.01);
    } catch (const std::exception& e) {
        e.what();
    }
    return std::make_pair(right_line, left_line);
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

// void RoadLineDetector::processFrame(const cv::Mat& frame) {
// make smallest from this function to keep logic in small fragments like preprocess, get edges, get lines, etc
// cv::Mat processedFrame = preprocessFrame(frame);
// std::vector<cv::Vec4i> detectedLines = detectLines(processedFrame);
// cv::imshow("frame", frame);
// cv::waitKey(0);
// cv::Mat greyscaleFrame = convertFrameToGrayscale(frame);
// greyscaleFrame = cropRoiFromFrame(greyscaleFrame);
// cv::Mat frameWithLines = greyscaleFrame;
// cv::Mat frameWithLinesResult = greyscaleFrame;
// cv::medianBlur( greyscaleFrame, greyscaleFrame, 15);
// cv::Canny(greyscaleFrame, processedFrame, 45, 125, 3);
// cv::imshow("frame after canny", processedFrame);
// cv::waitKey(0);
// // // processedFrame = cropRoiFromFrame(processedFrame);
// // cv:imshow("cropped frame", processedFrame);
// cv::waitKey(0);
// cv::HoughLinesP(processedFrame, detectedLines, 6, CV_PI / 60, 110, 30, 10);
// for (size_t i = 0; i < detectedLines.size(); i++) {
//     cv::Vec4i l = detectedLines[i];
//     line(frameWithLines, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
// }
// std::cout << detectedLines.size() << std::endl;
// cv::imshow("detected lines", frameWithLines);
// cv::waitKey(0);

// std::vector<cv::Point> all_point_pairs_left;
// std::vector<cv::Point> all_point_pairs_right;
// for (cv::Vec4i line : detectedLines) {
//     int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
//     double slope = static_cast<double>(y2 - y1) / (x2 - x1);
//     std::cout << slope << std::endl;
//     if (std::abs(slope) < 0.5 || std::isinf(slope)) { // check if abs not return int :)
//         continue;
//     }
//     if (slope <= 0) {
//         all_point_pairs_left.push_back(cv::Point(x1, y1));
//         all_point_pairs_left.push_back(cv::Point(x2, y2));
//     } else {
//         all_point_pairs_right.push_back(cv::Point(x1, y1));
//         all_point_pairs_right.push_back(cv::Point(x2, y2));
//     }
// }
// std::pair<std::vector<cv::Point>, std::vector<cv::Point>> rightAndLeftPoints = findRightAndLeftPoints(detectedLines);
// std::pair<cv::Vec4i, cv::Vec4i> rightAndLeftLine = findLeftAndRightLine(rightAndLeftPoints);
// int image_height = frame.rows;
// double min_y = image_height * 0.6; // 280
// double max_y = image_height; // 480
// cv::Vec4f left_line_params, right_line_params;
// try {
//     fitLine(rightAndLeftPoints.first, right_line_params, cv::DIST_L2, 0, 0.01, 0.01);
//     fitLine(rightAndLeftPoints.second, left_line_params, cv::DIST_L2, 0, 0.01, 0.01);
// } catch (const std::exception& e) {
//     e.what();
// }
// cv::line(frame, cv::Point(rightAndLeftLine.first[2], rightAndLeftLine.first[3]), cv::Point(rightAndLeftLine.first[2] + rightAndLeftLine.first[0] * 100, rightAndLeftLine.first[3] + rightAndLeftLine.first[1] * 100), cv::Scalar(255, 0, 0), 2);
// cv::line(frame, cv::Point(rightAndLeftLine.second[2], rightAndLeftLine.second[3]), cv::Point(rightAndLeftLine.second[2] + rightAndLeftLine.second[0] * 100, rightAndLeftLine.second[3] + rightAndLeftLine.second[1] * 100), cv::Scalar(255, 0, 0), 2);

// cv::imshow("left fit and right fit", frame);
// cv::waitKey(0);

// std::pair<cv::Point2f, cv::Point2f> leftLineinePoints = vectorToLinePoints(rightAndLeftLine.second, min_y, max_y);
// std::pair<cv::Point2f, cv::Point2f> rightLineinePoints = vectorToLinePoints(rightAndLeftLine.first, min_y, max_y);

// cv::line(frame, leftLineinePoints.first, leftLineinePoints.second, cv::Scalar(255, 0, 0), 2);
// cv::line(frame, rightLineinePoints.first, rightLineinePoints.second, cv::Scalar(255, 0, 0), 2);
// cv::imshow("line detector result", frame);
// cv::waitKey(0);
// }