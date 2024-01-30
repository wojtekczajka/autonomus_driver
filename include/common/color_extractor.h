#pragma once

#include <opencv2/opencv.hpp>

namespace ColorExtractor {
    cv::Mat detectBlueColor(const cv::Mat& frame);
    cv::Mat detectRedColor(const cv::Mat& frame);
    cv::Mat detectBlackColor(const cv::Mat& frame);
    cv::Mat detectYellowColor(const cv::Mat& frame);
    cv::Mat detectGreenColor(const cv::Mat& frame);

    static const cv::Scalar lowerBlue(90, 145, 85);
    static const cv::Scalar upperBlue(150, 255, 255);
    static const cv::Scalar lowerRed(105, 10, 75);
    static const cv::Scalar upperRed(180, 125, 255);
    static const cv::Scalar lowerBlack(0, 0, 0);
    static const cv::Scalar upperBlack(180, 255, 70);
    static const cv::Scalar lowerYellow(20, 70, 100);
    static const cv::Scalar upperYellow(50, 255, 255);
    static const cv::Scalar lowerGreen(60, 100, 100);
    static const cv::Scalar upperGreen(90, 255, 255);
}