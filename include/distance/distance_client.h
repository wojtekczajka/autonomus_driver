#pragma once

#include <cpr/cpr.h>

#include <string>

#include "common/logger.h"
#include "distance/distance_client_interface.h"

class DistanceClient : public IDistanceClient {
   public:
    DistanceClient(Logger& logger, const std::string& serverURL);
    ~DistanceClient();

    double getDistance() override;

   private:
    Logger& logger;
    cpr::Url serverURL;
    static double distance;
};
