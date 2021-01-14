#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <math.h>
#include <mutex>
#include <assert.h>
#include <chrono>

#include <iostream>

#include <CImg.h>


constexpr double pi = 3.14159265358979323846;
constexpr double deg2rad = pi / 180.f;
constexpr double rad2deg = 180.f / pi;

class LoopTimer
{
public:
    LoopTimer(std::string tag);
    ~LoopTimer();

    std::chrono::duration<float, std::milli> loopDone();
    inline float getAvgDuration();
    inline void resetLoopStartTime();

private:
    std::chrono::time_point<std::chrono::steady_clock> m_last_time;
    std::chrono::time_point<std::chrono::steady_clock> m_last_time_print;
    float m_avgLoopTime;
    std::string m_tag;
};

float LoopTimer::getAvgDuration()
{
    return m_avgLoopTime;
}

void LoopTimer::resetLoopStartTime()
{
    m_last_time = std::chrono::steady_clock::now();
}
