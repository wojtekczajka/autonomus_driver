#include "distance/distance_client.h"

#include <curl/curl.h>

#include <iostream>

double DistanceClient::distance = 0.0;

DistanceClient::DistanceClient(Logger& logger, const std::string& serverURL) : serverURL(serverURL), logger(logger) {
    ws = easywsclient::WebSocket::from_url("ws://localhost:8000/distance");
    if (!ws) {
        logger.error("FAILED TO CREATE WEBSOCKET CONNECTION FOR DISTANCE");
    }
}

DistanceClient::~DistanceClient() {
    if (ws) {
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
    if (isClosed()) {
        logger.error("DISTANCE WEBSOCKET IS CLOSED");
        return -1.0;
    }
    std::string response;
    ws->send("get distance");
    try {
        pollAndDispatch();
        return DistanceClient::distance;
    } catch (const std::exception& e) {
        logger.error("ERROR OCCURED GETTING DISTANCE");
        return -1.0;
    }
}