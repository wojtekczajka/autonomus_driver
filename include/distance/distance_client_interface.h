#pragma once

class IDistanceClient {
public:
    virtual ~IDistanceClient() = default;
    virtual double getDistance() = 0;
};