#include "road_sign_detector/speed_limit_detector.h"

SpeedLimitDetector::SpeedLimitDetector() {
}

int SpeedLimitDetector::detectSpeedLimit(const cv::Mat& frame) {
    tessApi = new tesseract::TessBaseAPI();
    // Initialize tesseract to use English (eng) and the LSTM OCR engine.
    tessApi->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    tessApi->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    detectRedCircles(frame);
    return 0;
}

SpeedLimitDetector::~SpeedLimitDetector() {
    tessApi->End();
    delete tessApi;
}

void SpeedLimitDetector::detectRedCircles(const cv::Mat& frame) {
    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    // Set the red color detection range based on your provided values
    cv::Scalar lowerRed(108, 10, 75);
    cv::Scalar upperRed(180, 125, 255);

    // Threshold the HSV image to get only red colors
    cv::Mat redMask;
    cv::inRange(hsvImage, lowerRed, upperRed, redMask);
    // Optional: Apply some morphological operations to clean up the mask
    cv::morphologyEx(redMask, redMask, cv::MORPH_OPEN, cv::Mat());
    cv::morphologyEx(redMask, redMask, cv::MORPH_CLOSE, cv::Mat());
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(redMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double perimeter = cv::arcLength(contours[i], true);
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);
        std::cout << circularity << std::endl;
        std::cout << area << std::endl;
        // Set a threshold for circularity
        if (circularity > 0.7 && area > 2000) { // you can adjust this threshold
            cv::Rect boundingRect = cv::boundingRect(contours[i]);
            int radius = boundingRect.width / 2;
            int detectedSpeed = recognizeSpeed(frame, boundingRect);
            if (detectedSpeed % 10 == 0 && detectedSpeed >= 10 && detectedSpeed <= 140) {
                std::cout << detectedSpeed << std::endl;
                break;
            }
            // break;
        }
    }

    // Display the result
    // cv::imshow("aaa", temp);

    
}

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

int SpeedLimitDetector::recognizeSpeed(const cv::Mat& frame, const cv::Rect& rect) {
    cv::Mat roi = frame(rect);
    tessApi->SetImage(roi.data, roi.cols, roi.rows, 3, roi.step);
    const char* text = tessApi->GetUTF8Text();
    if (text == nullptr)
        return 0;
    int num = atoi(text);
    delete[] text;
    return num;
}
