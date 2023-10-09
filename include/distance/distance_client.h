#pragma once

#include <curl/curl.h>

#include <string>

class DistanceClient {
   public:
    DistanceClient(const std::string& baseUrl);
    ~DistanceClient();

    double getDistance();

   private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

    std::string baseUrl;
    CURL* curl;
    std::string response;
    double distance;
};
