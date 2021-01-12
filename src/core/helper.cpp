#include "helper.hpp"

#include <iostream>


LoopTimer::LoopTimer(std::string tag)
    : m_avgLoopTime(-1)
    , m_last_time_print(std::chrono::steady_clock::now())
    , m_last_time(std::chrono::steady_clock::now())
    , m_tag(tag)
{
}

LoopTimer::~LoopTimer()
{
}

std::chrono::duration<float, std::milli> LoopTimer::loopDone()
{
    auto elapsed_time = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> delta = elapsed_time - m_last_time;
    std::chrono::duration<float, std::milli> delta_print = elapsed_time - m_last_time_print;

    if (m_avgLoopTime < 0) {
        m_avgLoopTime = delta.count();
    }
    else {
        m_last_time = elapsed_time;
        m_avgLoopTime = 0.9f * m_avgLoopTime + 0.1f * delta.count();
    }

    if (delta_print.count() > 1000) {
        m_last_time_print = elapsed_time;
        std::cout << "[" << m_tag << "]: Loop Time is " << delta.count() << " ms (avg " << m_avgLoopTime << " ms)." << std::endl;
    }

    return delta;
}
