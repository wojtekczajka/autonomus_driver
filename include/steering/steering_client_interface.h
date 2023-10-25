#pragma once

class ISteeringClient {
public:
    virtual ~ISteeringClient() = default;
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool center() = 0;
    virtual bool turnLeft(int value) = 0;
    virtual bool turnRight(int value) = 0;
    virtual bool driveForward(int value) = 0;
    virtual bool driveBackward(int value) = 0;
};