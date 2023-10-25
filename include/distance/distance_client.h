#pragma once

#include <curl/curl.h>

#include <string>

#include "common/logger.h"
#include "easywsclient/easywsclient.hpp"
#include "distance/distance_client_interface.h"

class DistanceClient : public IDistanceClient {
   public:
    DistanceClient(Logger& logger, const std::string& serverURL);
    ~DistanceClient();

    double getDistance() override;
    bool isClosed();
    void pollAndDispatch();

   private:
    static void handleMessage(const std::string& message);

    Logger& logger;
    easywsclient::WebSocket::pointer ws;
    std::string serverURL;
    static double distance;
};
