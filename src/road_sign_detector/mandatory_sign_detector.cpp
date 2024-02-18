#include "road_sign_detector/mandatory_sign_detector.h"

#include "common/color_extractor.h"

MandatorySignDetector::MandatorySignDetector() {
    cv::Mat turnRightImg = cv::imread("../src/road_sign_detector/dict/turn_right.png", cv::IMREAD_GRAYSCALE);
    cv::Mat turnLeftImg = cv::imread("../src/road_sign_detector/dict/turn_left.png", cv::IMREAD_GRAYSCALE);
    cv::Mat driveForwardImg = cv::imread("../src/road_sign_detector/dict/drive_forward.png", cv::IMREAD_GRAYSCALE);

    referenceImages[TURN_RIGHT] = turnRightImg;
    referenceImages[TURN_LEFT] = turnLeftImg;
    referenceImages[DRIVE_FORWARD] = driveForwardImg;
}

MandatorySignDetector::TurnSignType MandatorySignDetector::classifyArrow(const cv::Mat& roi) {
    cv::Mat resizedRoi;
    cv::resize(roi, resizedRoi, cv::Size(64, 64));

    TurnSignType bestMatchType = DRIVE_FORWARD;
    double bestMatchScore = 0.0;

    for (const auto& [type, refImage] : referenceImages) {
        double score = compareImages(resizedRoi, refImage);
        if (score > bestMatchScore && score > 80.0) {
            bestMatchScore = score;
            bestMatchType = type;
        }
    }

    return bestMatchType;
}

double MandatorySignDetector::compareImages(const cv::Mat& img1, const cv::Mat& img2) {
    // Assuming both images are of same size and type
    cv::Mat diff;
    cv::absdiff(img1, img2, diff);
    diff = diff > 0; // Convert to binary image

    int totalPixels = diff.rows * diff.cols;
    int nonMatchingPixels = cv::countNonZero(diff);
    int matchingPixels = totalPixels - nonMatchingPixels;

    return (static_cast<double>(matchingPixels) / totalPixels) * 100.0;
}

void MandatorySignDetector::detectMandatorySign(const cv::Mat& frame) {
    cv::Mat blueMask = ColorExtractor::detectBlueColor(frame);
    contours.clear();
    cv::findContours(blueMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double perimeter = cv::arcLength(contours[i], true);
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);
        if (circularity > 0.7 && area > 600) {
            signPosition = cv::boundingRect(contours[i]);
            cv::Mat roi = blueMask(signPosition);
            cv::bitwise_not(roi, roi);
            signType = classifyArrow(roi);
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
