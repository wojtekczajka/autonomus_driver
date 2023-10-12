#include "steering/steering_client.h"

#include "common/config_parser.h"

SteeringClient::SteeringClient(Logger& logger)
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

SteeringClient::~SteeringClient() {
    if (curl) {
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

bool SteeringClient::start() {
    return executeCurl("start");
}

bool SteeringClient::stop() {
    currentSpeedValue = 0;
    return executeCurl("stop");
}

bool SteeringClient::turnLeft(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnLeft", value);
        return false;
    }
    return executeCurl("turn_left", value);
}

bool SteeringClient::turnRight(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnRight", value);
        return false;
    }
    return executeCurl("turn_right", value);
}

bool SteeringClient::center() {
    currentTurnValue = 0;
    return executeCurl("center");
}

bool SteeringClient::driveForward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveForward", value);
        return false;
    }
    currentSpeedValue = value;
    return executeCurl("drive_forward", value);
}

bool SteeringClient::driveBackward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveBackward", value);
        return false;
    }
    currentSpeedValue = value;
    return executeCurl("drive_backward", value);
}

bool SteeringClient::isValidValue(const int& value) const {
    return (value >= 0 && value <= 100);
}

void SteeringClient::logInvalidValue(const std::string& action, const int& value) const {
    logger.error("Invalid value for " + action + ": " + std::to_string(value));
}

bool SteeringClient::executeCurl(const std::string& action, int value) {
    if (action == previousAction && value == previousValue || action == "center" && previousAction == "center") {
        logger.info("Skipping action");
        return true;
    }

    if (!curl) {
        logger.error("cURL handle not initialized.");
        return false;
    }

    previousAction = action;
    previousValue = value;
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

size_t SteeringClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    std::string* response = output;
    response->append(static_cast<char*>(contents), total_size);
    return total_size;
}
