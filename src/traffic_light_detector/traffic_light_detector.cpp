#include "traffic_light_detector/traffic_light_detector.h"
#include "common/color_extractor.h"

TrafficLightDetector::TrafficLightDetector() {};

void TrafficLightDetector::detectTrafficLight(const cv::Mat& frame) {
    cv::imshow("black", ColorExtractor::detectBlackColor(frame));
}