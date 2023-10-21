#pragma once

#include <curl/curl.h>

#include <string>

#include "common/logger.h"
#include "easywsclient/easywsclient.hpp"

class DistanceClient {
   public:
    DistanceClient(Logger& logger, const std::string& serverURL);
    ~DistanceClient();

    double getDistance();
    bool isClosed();
    void pollAndDispatch();

   private:
    static void handleMessage(const std::string& message);

    Logger& logger;
    easywsclient::WebSocket::pointer ws;
    std::string serverURL;
    static double distance;
};
