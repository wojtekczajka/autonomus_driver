#include "road_sign_detector/turn_sign_detector.h"

TurnSignDetector::TurnSignDetector() {
}

void TurnSignDetector::detectTurnSign(const cv::Mat& frame) {
    cv::imwrite("raw.png", frame);
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    cv::Scalar lowerBlue(61, 200, 115);
    cv::Scalar upperBlue(180, 255, 255);

    cv::Mat blueMask;
    cv::inRange(hsvImage, lowerBlue, upperBlue, blueMask);
    cv::morphologyEx(blueMask, blueMask, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(blueMask, blueMask, cv::MORPH_CLOSE, cv::Mat());
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(blueMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    cv::imwrite("blueMask.png", blueMask);
    cv::Mat temp(frame);
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double perimeter = cv::arcLength(contours[i], true);
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);
        std::cout << circularity << std::endl;
        std::cout << area << std::endl;
        if (circularity > 0.7 && area > 2000) {  // you can adjust this threshold
            cv::Rect boundingRect = cv::boundingRect(contours[i]);
            double radius = boundingRect.width / 2;
            cv::Mat roi = blueMask(boundingRect);
            cv::imwrite("roi.png", roi);
            cv::Moments m = cv::moments(roi, true);
            if (m.mu11 < 0)
                std::cout << "LEFT SIGN" << std::endl;
            else
                std::cout << "RIGHT SIGN" << std::endl;
            cv::Point center = (boundingRect.br() + boundingRect.tl()) * 0.5;
            cv::circle(temp, center, radius, cv::Scalar(0, 255, 0), 2);
            cv::imwrite("result.png", temp);
        }
    }
    cv::imshow("aaa", blueMask);
}
