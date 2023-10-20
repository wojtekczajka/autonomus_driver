#pragma once

#include <curl/curl.h>
#include <string>
#include "common/logger.h"

class SteeringClient {
   public:
    SteeringClient(Logger& logger);
    ~SteeringClient();

    bool start();
    bool stop();
    bool center();
    bool turnLeft(int value);
    bool turnRight(int value);
    bool driveForward(int value);
    bool driveBackward(int value);

   private:
    bool isValidValue(const int& value) const;
    bool executeCurl(const std::string& action, int value = 0);

    void logInvalidValue(const std::string& action, const int& value) const;

    Logger& logger;
    CURL* curl;
    std::string url;
    struct curl_slist* headers;
    int currentSpeedValue;
    int currentTurnValue;
    std::string previousAction;
    int previousValue;

    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output);
};