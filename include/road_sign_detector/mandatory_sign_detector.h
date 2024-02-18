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

    bool signDetected;
    cv::Rect signPosition;
    TurnSignType signType;
    int counter = 0;
    std::vector<std::vector<cv::Point>> contours;
    std::map<TurnSignType, cv::Mat> referenceImages;

    TurnSignType classifyArrow(const cv::Mat& roi);
    double compareImages(const cv::Mat& img1, const cv::Mat& img2);
};