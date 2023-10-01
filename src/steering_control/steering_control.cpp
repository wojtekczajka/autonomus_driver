#include "steering_control/steering_control.h"

SteeringControl::SteeringControl(const std::string& scriptPath, Logger& logger) : scriptPath(scriptPath),
                                                                                  logger(logger) {}

bool SteeringControl::start() {
    return executeCommand("start");
}

bool SteeringControl::stop() {
    return executeCommand("stop");
}

bool SteeringControl::turnLeft(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnLeft", value);
        return false;
    }
    return executeCommand("turn_left --value " + std::to_string(value));
}

bool SteeringControl::turnRight(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("turnRight", value);
        return false;
    }
    return executeCommand("turn_right --value " + std::to_string(value));
}

bool SteeringControl::center() {
    return executeCommand("center");
}

bool SteeringControl::driveForward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveForward", value);
        return false;
    }
    return executeCommand("drive_forward --value " + std::to_string(value));
}

bool SteeringControl::driveBackward(int value) {
    if (!isValidValue(value)) {
        logInvalidValue("driveBackward", value);
        return false;
    }
    return executeCommand("drive_backward --value " + std::to_string(value));
}

bool SteeringControl::executeCommand(const std::string& command) const {
    std::string fullCommand = ". /home/ubuntu/git-repos/Steering-for-PiRacer-Standard/venv/bin/activate && python3 " + scriptPath + " " + command;

    auto startTime = std::chrono::high_resolution_clock::now();
    int result = system(fullCommand.c_str());
    auto endTime = std::chrono::high_resolution_clock::now();

    auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    if (result == 0) {
        logger.info("SCRIPT EXCECUTED ACTION TIME FOR " + command + ": " + std::to_string(executionTime) + " ms");
    } else {
        logger.error("FAILED TO EXCECUTED ACTION: " + command + ": " + std::to_string(executionTime) + " ms");
    }

    return (result == 0);
}

bool SteeringControl::isValidValue(int value) const {
    return (value >= 0 && value <= 100);
}

void SteeringControl::logInvalidValue(const std::string& action, const int& value) const {
    logger.error("Invalid value for " + action + ": " + std::to_string(value));
}
