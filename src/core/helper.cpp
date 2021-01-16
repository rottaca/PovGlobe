#include "helper.hpp"

#include <iostream>
#include <algorithm>


LoopTimer::LoopTimer(std::string tag)
    : m_avgLoopTime(-1)
    , m_maxLoopTime(-1)
    , m_last_time_print(std::chrono::steady_clock::now())
    , m_last_time_max(std::chrono::steady_clock::now())
    , m_last_time(std::chrono::steady_clock::now())
    , m_tag(tag)
{
}

LoopTimer::~LoopTimer()
{
}

std::chrono::duration<float, std::milli> LoopTimer::loopDone()
{
    auto curr_time = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> delta = curr_time - m_last_time;

    if (m_avgLoopTime < 0) {
        m_avgLoopTime = delta.count();
        m_maxLoopTime = delta.count();
        m_last_time_max = curr_time;
    }
    else {
        m_last_time = curr_time;
        m_avgLoopTime = 0.9f * m_avgLoopTime + 0.1f * delta.count();
        
        if (delta.count() > m_maxLoopTime) {
          m_maxLoopTime = delta.count();
          m_last_time_max = curr_time;          
        }else if((curr_time - m_last_time_max).count() > 3000.0f) {
          m_maxLoopTime = delta.count();
          m_last_time_max = curr_time;       
        }
    }

    std::chrono::duration<float, std::milli> delta_print = curr_time - m_last_time_print;
    if (delta_print.count() > 1000) {
        m_last_time_print = curr_time;
        std::cout << "[" << m_tag << "]: Loop Time is " << delta.count() << " ms (avg " << m_avgLoopTime << " ms, max " << m_maxLoopTime << " ms)." << std::endl;
    }

    return delta;
}
