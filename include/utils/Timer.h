#pragma once

#include <chrono>

class Timer
{
public:
    Timer()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    float GetTime()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start;
        return elapsed.count(); // seconds
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};