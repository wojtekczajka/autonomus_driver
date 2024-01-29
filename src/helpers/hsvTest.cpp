#include <opencv2/opencv.hpp>
#include <iostream>

void on_trackbar(int, void*) {
}

int main(int argc, char** argv) {
    cv::Mat frame = cv::imread(argv[1]); 
    if(frame.empty()) {
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }

    cv::Mat hsvImage;
    cv::cvtColor(frame, hsvImage, cv::COLOR_BGR2HSV);

    cv::namedWindow("Red Color Detection", 1);

    int lowH = 0, highH = 180, lowS = 0, highS = 255, lowV = 0, highV = 255;
    cv::createTrackbar("Low H", "Red Color Detection", &lowH, 180, on_trackbar);
    cv::createTrackbar("High H", "Red Color Detection", &highH, 180, on_trackbar);
    cv::createTrackbar("Low S", "Red Color Detection", &lowS, 255, on_trackbar);
    cv::createTrackbar("High S", "Red Color Detection", &highS, 255, on_trackbar);
    cv::createTrackbar("Low V", "Red Color Detection", &lowV, 255, on_trackbar);
    cv::createTrackbar("High V", "Red Color Detection", &highV, 255, on_trackbar);

    while (true) {
        cv::Mat mask;
        cv::inRange(hsvImage, cv::Scalar(lowH, lowS, lowV), cv::Scalar(highH, highS, highV), mask);

        cv::imshow("Red Color Detection", mask);

        // Wait for a key press for 30ms
        if (cv::waitKey(30) >= 0) {
            break;
        }
    }

    return 0;
}