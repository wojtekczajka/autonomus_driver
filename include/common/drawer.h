#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <map>

class Drawer {
public:
    Drawer();

    void addText(const std::string& key, const std::string& value);
    void updateText(const std::string& key, const std::string& value);
    void drawAllText(cv::Mat& frame);
    void drawLanes(cv::Mat& frame, const cv::Vec4f& rightLane, const cv::Vec4f& leftLane, bool rightLaneDetected, bool leftLaneDetected);
    cv::Mat updateFrame(const cv::Mat& frame, const std::map<std::string, std::string>& updates);

    static void drawLineY(cv::Mat& image, const cv::Vec4f& line, const int& y1, const int& y2, cv::Scalar color, int thickness = 2);
    static void drawLineX(cv::Mat& image, const cv::Vec4f& line, const int& x1, const int& x2, cv::Scalar color, int thickness = 2);
    static cv::Mat concatenateFrames(const cv::Mat& frame1, const cv::Mat& frame2);
    static void putText(cv::Mat& image, const std::string& text, const cv::Point& position, int fontFace, double fontScale, cv::Scalar color, int thickness, int lineType);
private:
    std::map<std::string, std::string> textEntries;
    int fontFace;
    double fontScale;
    cv::Scalar color;
    int thickness;
    int lineType;
    int verticalSpacing;
    int startingY;
};
