#include "road_sign_detector/speed_limit_detector.h"

SpeedLimitDetector::SpeedLimitDetector() {
}

int SpeedLimitDetector::detectSpeedLimit(const cv::Mat& frame) {
    detectRedCircles(frame);
    return 0;
}

void SpeedLimitDetector::detectRedCircles(const cv::Mat& frame) {
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    // Set the red color detection range based on your provided values
    cv::Scalar lowerRed(118, 17, 124);
    cv::Scalar upperRed(180, 126, 255);

    // Threshold the HSV image to get only red colors
    cv::Mat redMask;
    cv::inRange(hsvImage, lowerRed, upperRed, redMask);

    // Optional: Apply some morphological operations to clean up the mask
    cv::morphologyEx(redMask, redMask, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(redMask, redMask, cv::MORPH_CLOSE, cv::Mat());

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(redMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat temp(frame);
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double perimeter = cv::arcLength(contours[i], true);
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);

        // Set a threshold for circularity
        if (circularity > 0.8) { // you can adjust this threshold
            cv::Rect boundingRect = cv::boundingRect(contours[i]);
            cv::Point center = (boundingRect.br() + boundingRect.tl()) * 0.5;
            int radius = boundingRect.width / 2;
            cv::circle(temp, center, radius, cv::Scalar(0, 255, 0), 2);
        }
    }

    // Display the result
    cv::imshow("aaa", temp);

    
}
