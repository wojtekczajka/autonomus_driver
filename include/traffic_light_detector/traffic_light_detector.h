#pragma once 

#include <opencv2/opencv.hpp>

class TrafficLightDetector {
   public:
    enum TrafficLight {
        TRAFFIC_LIGHT_GREEN,
        TRAFFIC_LIGHT_YELOW,
        TRAFFIC_LIGHT_RED,
    };

    TrafficLightDetector();
    void detectTrafficLight(const cv::Mat& frame);
    bool isTrafficLight();
    TrafficLight getTrafficLight();
    std::string getTrafficLightStr();
    cv::Rect getSignPosition();

   private:
    bool signDetected;
    cv::Rect signPosition;
    TrafficLight trafficLight;
    std::vector<std::vector<cv::Point>> contours;
};