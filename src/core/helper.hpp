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
    float getAvgDuration();
    float getMaxDuration();
    
    void resetTimer();

private:
    
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
    
    static const int history_size = 1000; 
    
    int m_curr_idx;
    TimePoint m_last_times[history_size];
    
    TimePoint m_last_time_print;
    float m_avgLoopTime;
    float m_maxLoopTime;
    std::string m_tag;
};
