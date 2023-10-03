#include "road_line_detector/road_line_detector.h"

RoadLineDetector::RoadLineDetector() {}

RoadLineDetector::~RoadLineDetector() {}

void RoadLineDetector::processFrame(const cv::Mat& frame) {
    cv::Mat processedFrame;
    std::vector<cv::Vec4i> detectedLines;
    cv::imshow("frame", frame);
    cv::waitKey(0);
    cv::Mat greyscaleFrame = convertFrameToGrayscale(frame);
    cv::Mat frameWithLines = greyscaleFrame;
    cv::Mat frameWithLinesResult = greyscaleFrame;
    cv::medianBlur( greyscaleFrame, greyscaleFrame, 23);
    cv::Canny(greyscaleFrame, processedFrame, 45, 125, 3);
    cv::imshow("frame after canny", processedFrame);
    cv::waitKey(0);
    processedFrame = cropRoiFromFrame(processedFrame);
    cv:imshow("cropped frame", processedFrame);
    cv::waitKey(0);
    cv::HoughLinesP(processedFrame, detectedLines, 6, CV_PI / 60, 110, 30, 10);
    for (size_t i = 0; i < detectedLines.size(); i++) {
        cv::Vec4i l = detectedLines[i];
        line(frameWithLines, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    }
    std::cout << detectedLines.size() << std::endl;
    cv::imshow("detected lines", frameWithLines);
    cv::waitKey(0);

    std::vector<cv::Point> all_point_pairs_left;
    std::vector<cv::Point> all_point_pairs_right;
    for (cv::Vec4i line : detectedLines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[3];
        double slope = static_cast<double>(y2 - y1) / (x2 - x1);
        std::cout << slope << std::endl;
        if (std::abs(slope) < 0.5) { // check if abs not return int :)
            continue;
        }
        if (slope <= 0) {
            all_point_pairs_left.push_back(cv::Point(x1, y1));
            all_point_pairs_left.push_back(cv::Point(x2, y2));
        } else {
            all_point_pairs_right.push_back(cv::Point(x1, y1));
            all_point_pairs_right.push_back(cv::Point(x2, y2));
        }
    }
    int image_height = greyscaleFrame.rows;
    double min_y = image_height * 0.6;
    double max_y = image_height;
    cv::Vec4f left_line_params, right_line_params;
    try {
    fitLine(all_point_pairs_left, left_line_params, cv::DIST_L2, 0, 0.01, 0.01);
    fitLine(all_point_pairs_right, right_line_params, cv::DIST_L2, 0, 0.01, 0.01);
    } catch (const std::exception& e) {
        e.what();
    }
    cv::line(frame,  cv::Point(left_line_params[2], left_line_params[3]), cv::Point(left_line_params[2] + left_line_params[0] * 100, left_line_params[3] + left_line_params[1] * 100), cv::Scalar(255, 0, 0), 2);
    cv::line(frame,  cv::Point(right_line_params[2], right_line_params[3]), cv::Point(right_line_params[2] + right_line_params[0] * 100, right_line_params[3] + right_line_params[1] * 100), cv::Scalar(255, 0, 0), 2);
    cv::imshow("left fit and right fit", frame);
    cv::waitKey(0);

    float vx = left_line_params[0];
    float vy = left_line_params[1];
    float x0 = left_line_params[2];
    float y0 = left_line_params[3];
    float x1 = x0 + (min_y - y0) * vx / vy;
    float x2 = x0 + (max_y - y0) * vx / vy;
    cv::line(frame,  cv::Point(x1, min_y), cv::Point(x2, max_y), cv::Scalar(255, 0, 0), 2);
    cv::imshow("line detector result", frame);
    cv::waitKey(0);
}

cv::Mat RoadLineDetector::convertFrameToGrayscale(const cv::Mat& frame) {
    cv::Mat grayscaleFrame;
    cv::cvtColor(frame, grayscaleFrame, cv::COLOR_BGR2GRAY);
    return grayscaleFrame;
}

cv::Mat RoadLineDetector::cropRoiFromFrame(const cv::Mat& frame) {
    int width = frame.cols;
    int height = frame.rows;

    cv::Mat mask = cv::Mat::zeros(frame.size(), CV_8UC1);
    std::vector<cv::Point> triangleVertices;
    triangleVertices.push_back(cv::Point(0, height));      
    triangleVertices.push_back(cv::Point(width / 2, 0));   
    triangleVertices.push_back(cv::Point(width, height));  

    cv::fillPoly(mask, std::vector<std::vector<cv::Point>>(1, triangleVertices), cv::Scalar(255));
    int rectHeight = height / 2;
    cv::Rect topRect(0, 0, width, rectHeight);

    // Fill the rectangular region in the mask
    cv::rectangle(mask, topRect, cv::Scalar(0), -1);  // -1 indicates filled rectangle

    // rectHeight = height;
    // int rectWidth = width / 4;            // 1/4 of the frame width
    // int rectX = (width - rectWidth) / 2;  // Centered horizontally
    // topRect = cv::Rect(rectX, 0, rectWidth, rectHeight);
    // cv::rectangle(mask, topRect, cv::Scalar(0), -1);

    // Create a masked image by bitwise AND operation
    cv::Mat masked_image;
    cv::bitwise_and(frame, frame, masked_image, mask);

    return masked_image;
}

cv::Mat RoadLineDetector::getEdgeDetectionResult() const {
    return cv::Mat();
}