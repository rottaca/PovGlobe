#include "helper.hpp"

#include <iostream>
#include <algorithm>


LoopTimer::LoopTimer(std::string tag)
    : m_avgLoopTime(-1)
    , m_maxLoopTime(-1)
    , m_last_time_print(std::chrono::steady_clock::now())
    , m_tag(tag)
    , m_curr_idx(0)
{
  for (int i = 0; i < history_size; i++){
    m_last_times[i] = m_last_time_print;
  }
}

LoopTimer::~LoopTimer()
{
}

std::chrono::duration<float, std::milli> LoopTimer::loopDone()
{
    const auto curr_time = std::chrono::steady_clock::now();
    const int next_idx = (m_curr_idx + 1) % history_size;
    

    const std::chrono::duration<float, std::milli> delta_print = curr_time - m_last_time_print;
    if (delta_print.count() > 1000) {
        m_last_time_print = curr_time;
        std::cout << "[" << m_tag << "]: Loop Time is avg " << getAvgDuration() << " ms, max " << getMaxDuration() << " ms)." << std::endl;
    }

    const std::chrono::duration<float, std::milli> delta = curr_time - m_last_times[m_curr_idx];
    m_curr_idx = next_idx;
    m_last_times[m_curr_idx] = curr_time;
    return delta;
}

float LoopTimer::getAvgDuration()
{  
    float avgTime = 0;
    for (int i = 0; i < history_size - 1; i++){

        const int curr_idx = (m_curr_idx + i + 1) % history_size;
        const int next_idx = (m_curr_idx + i + 2) % history_size;

        const auto curr = m_last_times[curr_idx];
        const auto next = m_last_times[next_idx];

        const auto delta = next - curr;                                                           
        avgTime += std::chrono::duration_cast<std::chrono::milliseconds>(delta).count();
    }
  
    return avgTime/(history_size - 1);
}

float LoopTimer::getMaxDuration()
{  
    float maxTime = 0;
    for (int i = 0; i < history_size - 1; i++){
        const int curr_idx = (m_curr_idx + i + 1) % history_size;
        const int next_idx = (m_curr_idx + i + 2) % history_size;

        const auto curr = m_last_times[curr_idx];
        const auto next = m_last_times[next_idx];

        const auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(next - curr);

      if (delta.count() > maxTime){
        maxTime = delta.count();
      }                                                  
    }
  
    return maxTime;
}