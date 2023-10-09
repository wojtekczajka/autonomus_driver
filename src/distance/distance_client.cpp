#include <curl/curl.h>

#include <iostream>

#include "distance/distance_client.h"

DistanceClient::DistanceClient(const std::string& baseUrl) : baseUrl(baseUrl) {
    curl = curl_easy_init();
}

DistanceClient::~DistanceClient() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

double DistanceClient::getDistance() {
    if (!curl) {
        std::cerr << "Curl initialization failed." << std::endl;
        return -1.0; // Error value
    }

    std::string url = baseUrl + "/get_distance/";

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "HTTP request failed: " << curl_easy_strerror(res) << std::endl;
        return -1.0; // Error value
    }

    try {
        // Convert the response to a double
        double distance = std::stod(response);
        return distance;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing distance: " << e.what() << std::endl;
        return -1.0; // Error value
    }
}

size_t DistanceClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}
