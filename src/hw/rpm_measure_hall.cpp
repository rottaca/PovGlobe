#include "rpm_measure_hall.hpp"

#include <pigpio.h>
#include <cstdlib>
#include <cmath>


RpmMeasureHall::RpmMeasureHall(int gpio_pin) : m_history_size(50)
{
    m_gpio_pin = gpio_pin;
}

RpmMeasureHall::~RpmMeasureHall()
{
    gpioSetAlertFuncEx(m_gpio_pin, 0, this);
    gpioTerminate();
}

void RpmMeasureHall::_pulse(int gpio, int level, uint32_t tick)
{
    // 1 = change to high (a rising edge)
    if (level == 1) {
        edgeDetected();
    }
}

void RpmMeasureHall::edgeDetected()
{
    auto curr_time = std::chrono::steady_clock::now();
    auto new_delta = curr_time - m_last_event_time;
    
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      // Add delta time to the deque and limit its size
      m_delta_time_deque.push_back(new_delta);
      if (m_delta_time_deque.size() > m_history_size) {
          m_delta_time_deque.pop_front();
      }
      m_last_event_time = curr_time;
    }
}

void RpmMeasureHall::_pulseEx(int gpio, int level, uint32_t tick, void* user)
{
    // Forward the call to the member function
    RpmMeasureHall* mySelf = (RpmMeasureHall*)user;
    mySelf->_pulse(gpio, level, tick);
}

void RpmMeasureHall::initialize(Globe& globe)
{
    RpmMeasureBase::initialize(globe);

    m_delta_time_deque.clear();
    m_last_event_time = std::chrono::steady_clock::now();

    if (gpioInitialise() < 0) return exit(1);

    gpioSetMode(m_gpio_pin, PI_INPUT);
    gpioSetPullUpDown(m_gpio_pin, PI_PUD_UP);

    // Setup callback for rising and falling edges
    gpioSetAlertFuncEx(m_gpio_pin, _pulseEx, this);
}

RpmData RpmMeasureHall::getRpmData()
{
    RpmData data{};
    data.cycle_time = std::chrono::duration<float, std::milli>(0);
    
    std::chrono::time_point<std::chrono::steady_clock> last_event_time_copy;
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        // Calcualte average cycle time from history
        for (const auto& v : m_delta_time_deque) {
            data.cycle_time += v;
        }
        data.cycle_time /= m_delta_time_deque.size();
        last_event_time_copy = m_last_event_time;
    }
    
    const auto curr_time = std::chrono::steady_clock::now();
    const auto dt_since_last_event = curr_time - last_event_time_copy;
    // Calculate position along the x axis (temporal dimension) based on the 
    // elapsed time since the last rising edge.
    data.curr_temporal_pos = static_cast<int>(std::round(m_temporal_resolution * dt_since_last_event / data.cycle_time));
    // If we didn't receive an update before we expect to complete the rotation, something is wrong.
    // At least keep the value in a valid range
    data.curr_temporal_pos = std::min(data.curr_temporal_pos, m_temporal_resolution - 1);
    // The signal is valid as long as we have the expected number of samples in the history
    data.valid = (m_delta_time_deque.size() == m_history_size);
    
    return data;
}
