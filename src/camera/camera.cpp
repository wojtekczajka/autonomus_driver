#include "camera/camera.h"

#include <chrono>

#include "common/config_parser.h"
#include "common/logger.h"

Camera::Camera() : cap(cv::VideoCapture(0)), fpsCounter(0), fpsStartTime(std::chrono::steady_clock::now()), logger(Logger("/dev/null")) {
    if (!isOpened()) {
        logger.error("COULDN'T CAPTURE VIDEO FROM CAMERA");
    }
    setCameraConfig();
}

Camera::~Camera() {
    if (isOpened()) {
        cap.release();
        logger.info("VIDEO CAPTURE RELEASED");
    }
}

void Camera::setCameraConfig() {
    ConfigParser configParser("camera_config.txt");
    int frameWidth = configParser.getValue<int>("FRAME_WIDTH", 320);
    int frameHeihgt = configParser.getValue<int>("FRAME_HEIGHT", 240);
    int propFPS = configParser.getValue<int>("PROP_FPS", 15);

    cap.set(cv::CAP_PROP_FRAME_WIDTH, frameWidth);
    logger.info(fmt::format("VIDEO CAPTURE FRAME WIDTH SET TO: {}", frameWidth));
    
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, frameHeihgt);
    logger.info(fmt::format("VIDEO CAPTURE FRAME HEIGHT SET TO: {}", frameHeihgt));
    
    cap.set(cv::CAP_PROP_FPS, propFPS);
    logger.info(fmt::format("VIDEO CAPTURE PROP FPS SET TO: {}", propFPS));
}

cv::Mat Camera::getCurrentFrame() const {
    return currentFrame;
}

bool Camera::isOpened() {
    return cap.isOpened();
}

bool Camera::readFrame() {
    if (!cap.read(currentFrame)) {
        return false;
    }
    cv::rotate(currentFrame, currentFrame, cv::ROTATE_180);
    fpsCounter++;
    return true;
}

double Camera::getFPS() {
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fpsStartTime).count();
    double fps = static_cast<double>(fpsCounter) / (elapsedTime / 1000.0);  // Convert milliseconds to seconds
    fpsCounter = 0;
    fpsStartTime = std::chrono::steady_clock::now();
    logger.info(fmt::format("CURRENT FPS VALUE: {}", fps));
    return fps;
}
