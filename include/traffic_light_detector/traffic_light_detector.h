#pragma once 

#include <opencv2/opencv.hpp>

class TrafficLightDetector {
   public:
    enum TrafficLight {
        TRAFFIC_LIGHT_RED,
        TRAFFIC_LIGHT_YELLOW,
        TRAFFIC_LIGHT_GREEN,
    };

    TrafficLightDetector();
    void detectTrafficLight(const cv::Mat& frame);
    bool isTrafficLightDetected();
    TrafficLight getTrafficLight();
    std::string getTrafficLightStr();
    cv::Rect getTrafficLightPosition();

   private:
    bool trafficLightDetected;
    cv::Rect trafficLightPosition;
    TrafficLight trafficLight;
    std::vector<std::vector<cv::Point>> contours;

    bool detectRedLight(const cv::Mat& croppedContour, const cv::Rect& rect, const cv::Rect& part);
    bool detectGreenLight(const cv::Mat& croppedContour, const cv::Rect& rect, const cv::Rect& part);
    bool detectYellowLight(const cv::Mat& croppedContour, const cv::Rect& rect, const cv::Rect& part);
    bool isCircularAndCentered(const cv::Mat& mask, const cv::Rect& rect, double areaThreshold, const cv::Rect& part);
};