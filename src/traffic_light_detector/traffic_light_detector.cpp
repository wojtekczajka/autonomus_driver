#include "traffic_light_detector/traffic_light_detector.h"

#include "common/color_extractor.h"

TrafficLightDetector::TrafficLightDetector(){};

void TrafficLightDetector::detectTrafficLight(const cv::Mat& frame) {
    cv::Mat blackMask = ColorExtractor::detectBlackColor(frame);
    contours.clear();
    cv::findContours(blackMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
        return cv::contourArea(a) < cv::contourArea(b);
    });

    for (const auto& contour : contours) {
        cv::Rect rect = cv::boundingRect(contour);

        if (rect.x >= 0 && rect.y >= 0 &&
            rect.x + rect.width <= frame.cols &&
            rect.y + rect.height <= frame.rows &&
            rect.height >= 2 * rect.width) {

            cv::Mat croppedContour = frame(rect);

            int thirdHeight = rect.height / 3;
            cv::Rect upperThird(rect.x, rect.y, rect.width, thirdHeight);
            cv::Rect middleThird(rect.x, rect.y + thirdHeight, rect.width, thirdHeight);
            cv::Rect lowerThird(rect.x, rect.y + 2 * thirdHeight, rect.width, thirdHeight);
            
            bool redLightDetected = detectRedLight(croppedContour, rect, upperThird);
            bool yellowLightDetected = detectYellowLight(croppedContour, rect, middleThird);
            bool greenLightDetected = detectGreenLight(croppedContour, rect, lowerThird);

            if (redLightDetected && !yellowLightDetected && !greenLightDetected) {
                trafficLightDetected = true;
                trafficLightPosition = rect;
                trafficLight = TrafficLight::TRAFFIC_LIGHT_RED;
            } else if (!redLightDetected && yellowLightDetected && !greenLightDetected) {
                trafficLightDetected = true;
                trafficLightPosition = rect;
                trafficLight = TrafficLight::TRAFFIC_LIGHT_YELLOW;
            } else if (!redLightDetected && !yellowLightDetected && greenLightDetected) {
                trafficLightDetected = true;
                trafficLightPosition = rect;
                trafficLight = TrafficLight::TRAFFIC_LIGHT_GREEN;
            } else {
                trafficLightDetected = false;
            }
        }
    }
}

bool TrafficLightDetector::detectRedLight(const cv::Mat& croppedContour, const cv::Rect& rect, const cv::Rect& part) {
    cv::Mat redMask = ColorExtractor::detectRedColor(croppedContour);
    return isCircularAndCentered(redMask, rect, 175, part); 
}

bool TrafficLightDetector::detectYellowLight(const cv::Mat& croppedContour, const cv::Rect& rect, const cv::Rect& part) {
    cv::Mat yellowMask = ColorExtractor::detectYellowColor(croppedContour);
    return isCircularAndCentered(yellowMask, rect, 175, part); // Replace 175 with your area threshold for yellow
}

bool TrafficLightDetector::detectGreenLight(const cv::Mat& croppedContour, const cv::Rect& rect, const cv::Rect& part) {
    cv::Mat greenMask = ColorExtractor::detectGreenColor(croppedContour);
    return isCircularAndCentered(greenMask, rect, 175, part); // Replace 175 with your area threshold for green
}

bool TrafficLightDetector::isCircularAndCentered(const cv::Mat& mask, const cv::Rect& rect, double areaThreshold, const cv::Rect& part) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        double area = cv::contourArea(contour);
        if (area < areaThreshold) {
            continue;
        }

        double perimeter = cv::arcLength(contour, true);
        double circularity = (4 * CV_PI * area) / (perimeter * perimeter);
        if (circularity < 0.6) {
            continue;
        }

        cv::Moments moments = cv::moments(mask, true);
        cv::Point2f center(moments.m10 / moments.m00 + rect.x, moments.m01 / moments.m00 + rect.y);

        if (part.contains(center)) {
            return true;
        }
    }
    return false;
}

bool TrafficLightDetector::isTrafficLightDetected() {
    return trafficLightDetected;
}

TrafficLightDetector::TrafficLight TrafficLightDetector::getTrafficLight() {
    return trafficLight;
}

cv::Rect TrafficLightDetector::getTrafficLightPosition() {
    return trafficLightPosition;
}

std::string TrafficLightDetector::getTrafficLightStr() {
    switch (trafficLight) {
        case TRAFFIC_LIGHT_GREEN:
            return "Traffic Light: Green";
        case TRAFFIC_LIGHT_YELLOW:
            return "Traffic Light: Yellow";
        case TRAFFIC_LIGHT_RED:
            return "Traffic Light: Red";
        default:
            return "Traffic Light: Unknown";
    }
}