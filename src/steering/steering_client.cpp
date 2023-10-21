#include "steering/steering_client.h"

#include "common/config_parser.h"

std::string SteeringClient::return_msg;

SteeringClient::SteeringClient(Logger& logger, const std::string& serverURL)
    : logger(logger), serverURL(serverURL), currentSpeedValue(0), currentTurnValue(0) {
    ws = easywsclient::WebSocket::from_url(serverURL);
    if (!ws) {
        logger.error("FAILED TO CREATE WEBSOCKET CONNECTION FOR CONTROL");
    }
}

SteeringClient::~SteeringClient() {
    if (ws) {
        ws->close();
        delete ws;
    }
}

bool SteeringClient::start() {
    return excecuteControlRequest("start");
}

bool SteeringClient::stop() {
    currentSpeedValue = 0;
    return excecuteControlRequest("stop");
}

bool SteeringClient::turnLeft(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnLeft", value);
        return false;
    }
    return excecuteControlRequest("turn_left", value);
}

bool SteeringClient::turnRight(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnRight", value);
        return false;
    }
    return excecuteControlRequest("turn_right", value);
}

bool SteeringClient::center() {
    currentTurnValue = 0;
    return excecuteControlRequest("center");
}

bool SteeringClient::driveForward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveForward", value);
        return false;
    }
    currentSpeedValue = value;
    return excecuteControlRequest("drive_forward", value);
}

bool SteeringClient::driveBackward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveBackward", value);
        return false;
    }
    currentSpeedValue = value;
    return excecuteControlRequest("drive_backward", value);
}

bool SteeringClient::isValidValue(const int& value) const {
    return (value >= 0 && value <= 100);
}

void SteeringClient::logInvalidValue(const std::string& action, const int& value) const {
    logger.error("Invalid value for " + action + ": " + std::to_string(value));
}

bool SteeringClient::isClosed() {
    return ws && ws->getReadyState() == easywsclient::WebSocket::CLOSED;
}

void SteeringClient::pollAndDispatch() {
    if (ws) {
        ws->poll();
        ws->dispatch(handleMessage);
    }
}

void SteeringClient::handleMessage(const std::string& message) {
    SteeringClient::return_msg = message;
}

bool SteeringClient::excecuteControlRequest(const std::string& action, int value) {
    if (action == previousAction && value == previousValue || action == "center" && previousAction == "center") {
        logger.info("Skipping action");
        return true;
    }

    if (isClosed()) {
        logger.error("CONTROL WEBSOCKET IS CLOSED");
        return false;
    }

    previousAction = action;
    previousValue = value;
    std::string postFields = "{\"action\":\"" + action + "\",\"value\":" + std::to_string(value) + "}";
    ws->send(postFields);
    pollAndDispatch();
    if (SteeringClient::return_msg == "Action performed successfully") {
        logger.info("WEBSOCKET REQUEST SUCCESSFUL FOR " + action);
        return true;
    } else {
        logger.error("WEBSOCKET REQUEST FAILED FOR " + action + ": " + SteeringClient::return_msg);
        return false;
    }
}
