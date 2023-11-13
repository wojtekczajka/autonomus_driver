#include "distance/distance_client.h"
#include <nlohmann/json.hpp>

#include <curl/curl.h>

#include <iostream>

double DistanceClient::distance = 0.0;

DistanceClient::DistanceClient(Logger& logger, const std::string& serverURL) : serverURL(serverURL), logger(logger) {}

DistanceClient::~DistanceClient() {}

double DistanceClient::getDistance() {
    cpr::Response r = cpr::Get(serverURL);
    nlohmann::json json_response = nlohmann::json::parse(r.text);
    return json_response["distance"];
}