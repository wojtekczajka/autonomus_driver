#pragma once

#include <curl/curl.h>

#include <string>

#include "common/logger.h"
#include "easywsclient/easywsclient.hpp"
#include "steering/steering_client_interface.h"

class SteeringClient : public ISteeringClient {
   public:
    SteeringClient(Logger& logger, const std::string& serverURL);
    ~SteeringClient();

    bool start() override;
    bool stop() override;
    bool center() override;
    bool turnLeft(int value) override;
    bool turnRight(int value) override;
    bool driveForward(int value) override;
    bool driveBackward(int value) override;

    bool isClosed();
    void pollAndDispatch();

   private:
    static void handleMessage(const std::string& message);

    bool isValidValue(const int& value) const;
    bool excecuteControlRequest(const std::string& action, int value = 0);
    void logInvalidValue(const std::string& action, const int& value) const;

    static std::string return_msg;

    Logger& logger;
    easywsclient::WebSocket::pointer ws;
    std::string serverURL;
    int currentSpeedValue;
    int currentTurnValue;
    std::string previousAction;
    int previousValue;
};