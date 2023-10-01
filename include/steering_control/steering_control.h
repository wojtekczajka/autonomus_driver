#pragma once

#include <string>

#include "common/logger.h"

class SteeringControl {
   public:
    SteeringControl(const std::string& scriptPath, Logger& logger);

    bool start();
    bool stop();
    bool turnLeft(int value);
    bool turnRight(int value);
    bool center();
    bool driveForward(int value);
    bool driveBackward(int value);

   private:
    bool isValidValue(int value) const;
    bool executeCommand(const std::string& command) const;
    
    void logInvalidValue(const std::string& action, const int& value) const;

    std::string scriptPath;
    Logger& logger;
};