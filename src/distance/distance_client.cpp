#include "distance/distance_client.h"

#include <nlohmann/json.hpp>

DistanceClient::DistanceClient(Logger& logger, const std::string& serverURL) : serverURL(serverURL), logger(logger) {}

DistanceClient::~DistanceClient() {}

double DistanceClient::getDistance() {
    cpr::Response r = cpr::Get(serverURL);
    if (r.status_code == 200) {
        nlohmann::json json_response = nlohmann::json::parse(r.text);
        return json_response["distance"];
    } else {
        logger.error("Distance request failed: " + r.text);
        return -1.0;
    }
}