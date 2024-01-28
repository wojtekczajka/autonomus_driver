#pragma once

#include <opencv2/opencv.hpp>

class MandatorySignDetector {
   public:
    enum TurnSignType {
        TURN_RIGHT,
        TURN_LEFT,
        DRIVE_FORWARD,
    };

    MandatorySignDetector();
    void detectMandatorySign(const cv::Mat& frame);
    bool isSignDetected();
    TurnSignType getSignType();
    std::string getSignTypeStr();
    cv::Rect getSignPosition();

   private:
    static constexpr int mu11_THRESHOLD_LEFT = 85000;
    static constexpr int mu11_THRESHOLD_RIGHT = 185000;

    bool signDetected;
    cv::Rect signPosition;
    TurnSignType signType;
    std::vector<std::vector<cv::Point>> contours;

    TurnSignType classifyArrow(const std::vector<cv::Point>& contour);
};