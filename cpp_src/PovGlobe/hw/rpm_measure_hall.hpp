#pragma once

#include "core/rpm_measure_base.hpp"
#include <stdint.h>
#include <chrono>
#include <deque>
#include <fstream>
#include <mutex>

class RpmMeasureHall : public RpmMeasureBase
{
public:
    RpmMeasureHall(int gpio_pin);
    ~RpmMeasureHall();

    virtual void initialize(Globe& globe);
    virtual RpmData getRpmData();

    void _pulse(int gpio, int level, uint32_t tick);

    /* Need a static callback to link with C. */
    static void _pulseEx(int gpio, int level, uint32_t tick, void* user);

private:
    int m_gpio_pin;
    int m_history_size;
    std::mutex m_mutex;
    std::chrono::time_point<std::chrono::steady_clock> m_last_event_time;
    std::deque<std::chrono::duration<float, std::milli>> m_delta_time_deque;

    void edgeDetected();  
};