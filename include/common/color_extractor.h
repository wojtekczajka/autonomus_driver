#pragma once

#include <opencv2/opencv.hpp>

namespace ColorExtractor {
    cv::Mat detectBlueColor(const cv::Mat& frame);
    cv::Mat detectRedColor(const cv::Mat& frame);

    static const cv::Scalar lowerBlue(71, 155, 115);
    static const cv::Scalar upperBlue(180, 255, 255);
    static const cv::Scalar lowerRed(108, 10, 75);
    static const cv::Scalar upperRed(180, 125, 255);
}