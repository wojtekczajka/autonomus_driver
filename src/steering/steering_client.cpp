#include "steering/steering_client.h"

#include "common/config_parser.h"

SteeringClient::SteeringClient(Logger& logger, const std::string& serverURL)
    : logger(logger), serverURL(serverURL), currentSpeedValue(0), currentTurnValue(0) {
}

SteeringClient::~SteeringClient() {}

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

bool SteeringClient::excecuteControlRequest(const std::string& action, int value) {
    if (action == previousAction && value == previousValue || action == "center" && previousAction == "center") {
        logger.info("Skipping action");
        return true;
    }

    previousAction = action;
    previousValue = value;
    cpr::Response r = cpr::Post(serverURL,
                                cpr::Body{"{\"action\":\"" + action + "\",\"value\":" + std::to_string(value) + "}"},
                                cpr::Header{{"Content-Type", "application/json"}});
    if (r.status_code == 200)
        return true;
    else {
        logger.error("Steering request failed: " + r.text);
        return false;
    }
}
