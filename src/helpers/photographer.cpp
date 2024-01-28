#include <iostream>

#include "camera/camera.h"

int main() {
    int num = 108;
    Logger logger;
    Camera camera(logger);

    if (!camera.isOpened()) {
        std::cerr << "Error: Unable to access the camera." << std::endl;
        return -1;
    }

    cv::Mat frame;
    while (true) {
        if (!camera.readFrame()) {
            std::cerr << "Error: Could not read frame." << std::endl;
            break;
        }
        
        frame = camera.getCurrentFrame();

        cv::imshow("Captured Frame", frame);
        cv::waitKey(0);

        std::string saveFrameOption;
        std::cout << "Do you want to save this frame? (yes/no): ";
        std::cin >> saveFrameOption;

        if (saveFrameOption == "yes") {
            std::string frameFilename("frame" + std::to_string(num) + ".jpg");
            cv::imwrite(frameFilename, frame);  

            std::cout << num << std::endl;
            ++num;
        } else {
            std::cout << "Frame not saved." << std::endl;
        }
    }

    cv::destroyAllWindows();

    return 0;
}
