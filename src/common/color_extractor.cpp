#include "common/color_extractor.h"

cv::Mat ColorExtractor::detectBlueColor(const cv::Mat& frame) {
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    cv::Mat blueMask;
    cv::inRange(hsvImage, lowerBlue, upperBlue, blueMask);
    
    cv::morphologyEx(blueMask, blueMask, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(blueMask, blueMask, cv::MORPH_CLOSE, cv::Mat());

    return blueMask;
}

cv::Mat ColorExtractor::detectRedColor(const cv::Mat& frame) {
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    cv::Mat redMask;
    cv::inRange(hsvImage, lowerRed, upperRed, redMask);
    
    cv::morphologyEx(redMask, redMask, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(redMask, redMask, cv::MORPH_CLOSE, cv::Mat());

    return redMask;
}

cv::Mat ColorExtractor::detectBlackColor(const cv::Mat& frame) {
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    cv::Mat blackMask;
    cv::inRange(hsvImage, lowerBlack, upperBlack, blackMask);
    
    cv::morphologyEx(blackMask, blackMask, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(blackMask, blackMask, cv::MORPH_CLOSE, cv::Mat());

    return blackMask;
}

