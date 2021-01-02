#include "rpm_measure_hall.hpp"

#include <pigpio.h>
#include <cstdlib>
#include <cmath>

RpmMeasureHall::RpmMeasureHall(int gpio_pin)
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
	if(level == 1){
		edgeDetected();
	}
}

void RpmMeasureHall::edgeDetected()
{
	auto curr_time = std::chrono::steady_clock::now();
	auto new_delta = curr_time - m_last_event_time;
	
	m_delta_time_deque.push_back(new_delta);
	if (m_delta_time_deque.size() > 1){
		m_delta_time_deque.pop_front();
	}
	m_last_event_time = curr_time;
}

void RpmMeasureHall::_pulseEx(int gpio, int level, uint32_t tick, void *user)
{
   /*
      Need a static callback to link with C.
   */

   RpmMeasureHall *mySelf = (RpmMeasureHall *) user;

   mySelf->_pulse(gpio, level, tick); /* Call the instance callback. */
}

void RpmMeasureHall::initialize(Globe& globe)
{
	RpmMeasureBase::initialize(globe);

	m_delta_time_deque.clear();

   if (gpioInitialise() < 0) return exit(1);
   
   gpioSetMode(m_gpio_pin, PI_INPUT);
   gpioSetPullUpDown(m_gpio_pin, PI_PUD_UP);
   gpioSetAlertFuncEx(m_gpio_pin, _pulseEx, this);
}

RpmMeasureHall::RpmData RpmMeasureHall::getRpmData()
{
	RpmMeasureHall::RpmData data;
	data.cycle_time = std::chrono::duration<float, std::milli>(0);
	for (const auto &v : m_delta_time_deque){
		data.cycle_time += v;
	}
	data.cycle_time  /= m_delta_time_deque.size();

	auto curr_time = std::chrono::steady_clock::now();
	auto dt_since_last_event = curr_time  - m_last_event_time;
	data.curr_temporal_pos = static_cast<int>(std::round(m_temporal_resolution*dt_since_last_event/data.cycle_time));
  data.curr_temporal_pos = data.curr_temporal_pos % m_temporal_resolution;
  
  data.valid = m_delta_time_deque.size() == 1;
  
	return data;
}
