#include "common/drawer.h"

Drawer::Drawer() : fontFace(cv::FONT_HERSHEY_SIMPLEX), fontScale(1.0), color(cv::Scalar(255, 255, 255)), thickness(2), lineType(cv::LINE_AA), verticalSpacing(30), startingY(30) {}

void Drawer::drawLanes(cv::Mat& frame, const cv::Vec4f& rightLane, const cv::Vec4f& leftLane, bool rightLaneDetected, bool leftLaneDetected) {
    if (rightLaneDetected) {
        drawLineY(frame, rightLane, frame.rows, frame.rows / 2, cv::Scalar(0, 255, 0));
    }
    if (leftLaneDetected) {
        drawLineY(frame, leftLane, frame.rows, frame.rows / 2, cv::Scalar(0, 255, 0));
    }
}

cv::Mat Drawer::updateFrame(const cv::Mat& frame, const std::map<std::string, std::string>& updates) {
    for (const auto& [key, value] : updates) {
        updateText(key, value);
    }
    cv::Mat textRectangle = cv::Mat::zeros(frame.rows, frame.cols, frame.type());
    drawAllText(textRectangle);
    return concatenateFrames(frame, textRectangle);
}

void Drawer::addText(const std::string& key, const std::string& value) {
    textEntries[key] = value;
}

void Drawer::updateText(const std::string& key, const std::string& value) {
    textEntries[key] = value;
}

void Drawer::drawAllText(cv::Mat& frame) {
    int y = startingY;
    for (const auto& entry : textEntries) {
        std::string text = entry.first + ": " + entry.second;
        cv::putText(frame, text, cv::Point(10, y), fontFace, fontScale, color, thickness, lineType);
        y += verticalSpacing;
    }
}

void Drawer::drawLineY(cv::Mat& image, const cv::Vec4f& line, const int& y1, const int& y2, cv::Scalar color, int thickness) {
    float a = line[1] / line[0];
    float b = line[3] - (a * line[2]);
    cv::line(image, cv::Point2f((y1 - b) / a, y1), cv::Point2f((y2 - b) / a, y2), color, thickness);
}

void Drawer::drawLineX(cv::Mat& image, const cv::Vec4f& line, const int& x1, const int& x2, cv::Scalar color, int thickness) {
    float a = line[1] / line[0];
    float b = line[3] - (a * line[2]);
    cv::line(image, cv::Point2f(x1, a * x1 + b), cv::Point2f(x2, a * x2 + b), color, thickness);
}

void Drawer::putText(cv::Mat& image, const std::string& text, const cv::Point& position, int fontFace, double fontScale, cv::Scalar color, int thickness, int lineType) {
    cv::putText(image, text, position, fontFace, fontScale, color, thickness, lineType);
}

cv::Mat Drawer::concatenateFrames(const cv::Mat& frame1, const cv::Mat& frame2) {
    cv::Mat displayImage;
    cv::hconcat(frame1, frame2, displayImage);
    return displayImage;
}