#include "steering_control/steering_control.h"

#include "common/config_parser.h"

SteeringControl::SteeringControl(Logger& logger)
    : logger(logger), curl(curl_easy_init()), headers(nullptr), currentSpeedValue(0), currentTurnValue(0) {
    if (!curl) {
        logger.error("Failed to initialize cURL.");
    }
    ConfigParser configParser("curl_config.txt");
    url = configParser.getValue<std::string>("URL", "http://localhost:8000/control/");
    logger.info(fmt::format("STEERING SERVICE URL: {}", url));

    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
}

SteeringControl::~SteeringControl() {
    if (curl) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

bool SteeringControl::start() {
    return executeCurl("start");
}

bool SteeringControl::stop() {
    currentSpeedValue = 0;
    return executeCurl("stop");
}

bool SteeringControl::turnLeft(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnLeft", value);
        return false;
    }
    // executeCurl("set_speed_for_right_motor", currentSpeedValue * 0.3);
    // executeCurl("set_speed_for_left_motor", 0);
    return executeCurl("turn_left", value);
}

bool SteeringControl::turnRight(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnRight", value);
        return false;
    }
    // executeCurl("set_speed_for_right_motor", 0);
    // executeCurl("set_speed_for_left_motor", currentSpeedValue * 0.3);
    return executeCurl("turn_right", value);
}

bool SteeringControl::center() {
    currentTurnValue = 0;
    executeCurl("drive_forward", currentSpeedValue);
    return executeCurl("center");
}

bool SteeringControl::driveForward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveForward", value);
        return false;
    }
    currentSpeedValue = value;
    return executeCurl("drive_forward", value);
}

bool SteeringControl::driveBackward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveBackward", value);
        return false;
    }
    currentSpeedValue = value;
    return executeCurl("drive_backward", value);
}

bool SteeringControl::isValidValue(const int& value) const {
    return (value >= 0 && value <= 100);
}

void SteeringControl::logInvalidValue(const std::string& action, const int& value) const {
    logger.error("Invalid value for " + action + ": " + std::to_string(value));
}

bool SteeringControl::executeCurl(const std::string& action, int value) {
    if (!curl) {
        logger.error("cURL handle not initialized.");
        return false;
    }

    std::string postFields = "{\"action\":\"" + action + "\",\"value\":" + std::to_string(value) + "}";
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());

    std::string response_data;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        logger.info("HTTP POST REQUEST SUCCESSFUL FOR " + action);
        logger.info("Response Data: " + response_data);
        return true;
    } else {
        logger.error("HTTP POST request failed for " + action + ": " + curl_easy_strerror(res));
        return false;
    }
}

size_t SteeringControl::writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    std::string* response = output;
    response->append(static_cast<char*>(contents), total_size);
    return total_size;
}
