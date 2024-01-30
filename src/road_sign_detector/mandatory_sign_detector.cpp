#include "road_sign_detector/mandatory_sign_detector.h"

#include <mlpack/core/data/load.hpp>

#include "common/color_extractor.h"

MandatorySignDetector::MandatorySignDetector() {
    // Load the training data.
    arma::mat train_data;
    mlpack::data::Load("/home/ubuntu/git-repos/autonomus_driver/src/helpers/normalized_data.csv", train_data);

    // Load the training labels.
    arma::Row<size_t> labels;
    mlpack::data::Load("/home/ubuntu/git-repos/autonomus_driver/frames/training_data/labels.csv", labels);

    const size_t inputSize = train_data.n_rows;
    const size_t numClasses = 3;
    const double lambda = 0.01;
    const double delta = 1.0;

    lsvm = mlpack::LinearSVM<>(train_data, labels, numClasses, lambda, delta);
}

MandatorySignDetector::TurnSignType MandatorySignDetector::classifyArrow(const cv::Mat& roi) {
    double minValues[] = {4.421540e-01, 2.609100e-04, 1.154530e-05, 1.428230e-05, -2.795840e-06, -4.636740e-05, -6.634440e-07};
    double maxValues[] = {7.301360e-01, 9.192100e-03, 4.781860e-03, 4.157580e-03, 7.181300e-08, 5.367200e-05, 1.795230e-06};

    cv::Mat resizedRoi;
    cv::resize(roi, resizedRoi, cv::Size(64, 64));
    cv::Moments m = cv::moments(resizedRoi, true);
    double huMoments[7];
    cv::HuMoments(m, huMoments);
    arma::rowvec featureVector(7);
    for (int i = 0; i < 7; ++i) {
        featureVector(i) = (huMoments[i] - minValues[i]) / (maxValues[i] - minValues[i]);
    }
    std::cout << "featureVector size: " << featureVector.n_elem << std::endl;
    arma::Row<size_t> predictions;
    lsvm.Classify(featureVector.t(), predictions);
    int prediction = predictions[0];
    std::cout << "prediction: " << prediction << std::endl;
    cv::imwrite("aaa" + std::to_string(huMoments[0]) + ".png", resizedRoi);
    std::cout << m.mu11 << std::endl;
    if (m.mu11 <= MandatorySignDetector::mu11_THRESHOLD_LEFT) {
        return MandatorySignDetector::TurnSignType::TURN_LEFT;
    } else if (m.mu11 > MandatorySignDetector::mu11_THRESHOLD_LEFT && m.mu11 <= MandatorySignDetector::mu11_THRESHOLD_RIGHT) {
        return MandatorySignDetector::TurnSignType::TURN_RIGHT;
    } else {
        return MandatorySignDetector::TurnSignType::DRIVE_FORWARD;
    }
}

void MandatorySignDetector::detectMandatorySign(const cv::Mat& frame) {
    cv::Mat blueMask = ColorExtractor::detectBlueColor(frame);
    contours.clear();
    cv::findContours(blueMask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double perimeter = cv::arcLength(contours[i], true);
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);
        std::cout << circularity << " " << area << std::endl;
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
