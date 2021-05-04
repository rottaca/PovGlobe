#pragma once

#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/sync.h"

#include "constants.hpp"


#define N_TIME_MEASURES (N_MAGNETS)
#define N_COLUMNS_PER_SEGMENT (N_HORIZONTAL_RESOLUTION / N_MAGNETS)

class RTTMeasure
{
private:

    static critical_section_t critical_section;

    static absolute_time_t m_measured_intervals[N_TIME_MEASURES];
    static int8_t m_curr_segment_index;
    static absolute_time_t m_last_hall_sensor_event;

    RTTMeasure();
    void init();
public:
    ~RTTMeasure();

    bool rotationDetected();
    uint32_t getCurrentColumn();

    static RTTMeasure& getInstance();
    static void gpio_hall_sensor_callback(uint gpio, uint32_t events);
};
