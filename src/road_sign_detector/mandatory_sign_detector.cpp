#include "road_sign_detector/mandatory_sign_detector.h"
#include "common/color_extractor.h"

MandatorySignDetector::MandatorySignDetector() { }

MandatorySignDetector::TurnSignType MandatorySignDetector::classifyArrow(const std::vector<cv::Point>& contour) {
    cv::Moments m = cv::moments(contour, true);
    if (m.mu11 <= MandatorySignDetector::mu11_THRESHOLD_LEFT) {
        return MandatorySignDetector::TurnSignType::TURN_LEFT;
    } else if (m.mu11 > MandatorySignDetector::mu11_THRESHOLD_LEFT && m.mu11 <= MandatorySignDetector::mu11_THRESHOLD_RIGHT) {
        return MandatorySignDetector::TurnSignType::TURN_RIGHT;
    } else {
        return MandatorySignDetector::TurnSignType::DRIVE_FORWARD;
    }
}

void MandatorySignDetector::detectTurnSign(const cv::Mat& frame) {

    cv::Mat blueMask = ColorExtractor::detectBlueColor(frame);
    contours.clear();
    cv::findContours(blueMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat temp(frame);
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double perimeter = cv::arcLength(contours[i], true);
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);
        if (circularity > 0.7 && area > 2000) {  // you can adjust this threshold
            signPosition = cv::boundingRect(contours[i]);
            signType = classifyArrow(contours[i]);
            signDetected = true;
            return;
        }
    }
    signDetected = false;
}

bool MandatorySignDetector::isSignDetected() {
    return signDetected;
}

MandatorySignDetector::TurnSignType MandatorySignDetector::getSignType() {
    return signType;
}

std::string MandatorySignDetector::getSignTypeStr() {
    switch (signType) {
        case TurnSignType::TURN_LEFT:
            return "Mandatory Turn Left";
        case TurnSignType::TURN_RIGHT:
            return "Mandatory Turn Right";
        case TurnSignType::DRIVE_FORWARD:
            return "Mandatory Drive Forward";
        default:
            return "Unknown";
    }
}

cv::Rect MandatorySignDetector::getSignPosition() {
    return signPosition;
}
