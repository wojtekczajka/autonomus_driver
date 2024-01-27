#include "road_sign_detector/speed_limit_detector.h"
#include "common/color_extractor.h"

SpeedLimitDetector::SpeedLimitDetector() {
    tessApi = new tesseract::TessBaseAPI();
    tessApi->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    tessApi->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    tessApi->SetVariable("user_defined_dpi", "70");
}

SpeedLimitDetector::~SpeedLimitDetector() {
    tessApi->End();
    delete tessApi;
    if (text)
        delete[] text;
}

void SpeedLimitDetector::detectSpeedLimit(const cv::Mat& frame) {
    cv::Mat redMask = ColorExtractor::detectRedColor(frame);
    contours.clear();
    cv::findContours(redMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double perimeter = cv::arcLength(contours[i], true);
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);

        if (circularity > 0.7 && area > 2000) { // you can adjust this threshold
            signPosition = cv::boundingRect(contours[i]);
            speedLimitValue = recognizeSpeed(frame(signPosition));
            if (speedLimitValue % 10 == 0 && speedLimitValue >= 10 && speedLimitValue <= 140) {
                signDetected = true;
                return;
            }
        }
    }
    signDetected = false;
}

int SpeedLimitDetector::recognizeSpeed(const cv::Mat& frame) {
    cv::Mat grayRoi;
    cv::cvtColor(frame, grayRoi, cv::COLOR_BGR2GRAY);
    tessApi->SetImage(grayRoi.data, grayRoi.cols, grayRoi.rows, 1, grayRoi.step);
    text = tessApi->GetUTF8Text();
    if (text == nullptr)
        return 0;
    int num = atoi(text);
    return num;
}

bool SpeedLimitDetector::isSignDetected() {
    return signDetected;
}

cv::Rect SpeedLimitDetector::getSignPosition() {
    return signPosition;
}

int SpeedLimitDetector::getSpeedLimitValue() {
    return speedLimitValue;
}

std::string SpeedLimitDetector::getSpeedLimitValueStr() {
    return "Speed Limit " + std::to_string(speedLimitValue);
}
