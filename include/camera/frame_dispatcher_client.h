#pragma once

#include <opencv2/opencv.hpp>

#include "common/logger.h"
#include "easywsclient/easywsclient.hpp"

class FrameDispatcherClient {
   public:
    FrameDispatcherClient(Logger& logger, const std::string& serverURL);
    ~FrameDispatcherClient();

    void sendFrame(const cv::Mat& frame, const std::string& frameName);
    bool isClosed();
    void pollAndDispatch();

   private:
    static std::string return_msg;
    easywsclient::WebSocket::pointer ws;
    std::string serverURL;
    Logger& logger;

    static void handleMessage(const std::string& message);
};