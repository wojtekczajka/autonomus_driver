#include "distance/distance_client.h"
#include <nlohmann/json.hpp>

#include <curl/curl.h>

#include <iostream>
#include <cpr/cpr.h> 

double DistanceClient::distance = 0.0;

DistanceClient::DistanceClient(Logger& logger, const std::string& serverURL) : serverURL(serverURL), logger(logger) {
    // ws = easywsclient::WebSocket::from_url("ws://localhost:8000/distance");
    // if (!ws) {
    //     logger.error("FAILED TO CREATE WEBSOCKET CONNECTION FOR DISTANCE");
    // }
}

DistanceClient::~DistanceClient() {
    if (ws) {
        std::cout << "distance destructor" << std::endl;
        ws->close();
        delete ws;
    }
}

bool DistanceClient::isClosed() {
    return ws && ws->getReadyState() == easywsclient::WebSocket::CLOSED;
}

void DistanceClient::pollAndDispatch() {
    if (ws) {
        ws->poll();
        ws->dispatch(handleMessage);
    }
}

void DistanceClient::handleMessage(const std::string& message) {
    DistanceClient::distance = std::stod(message);
}

double DistanceClient::getDistance() {
    cpr::Response r = cpr::Get(cpr::Url{"http://127.0.0.1:8000/distance"});
    nlohmann::json json_response = nlohmann::json::parse(r.text);
    return json_response["distance"];
}