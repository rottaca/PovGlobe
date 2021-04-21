#pragma once

#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#include "constants.hpp"

class RTTMeasure
{
private:
    static absolute_time_t last_hall_sensor_event;

    static int64_t measured_intervals[N_MAGNETS];
    static int8_t curr_segment_index;

    RTTMeasure();
    void init();
public:
    ~RTTMeasure();

    bool rotationDetected();
    uint32_t getCurrentColumn(uint32_t maxColumnsPerRotation);

    static RTTMeasure& getInstance();
    static void gpio_hall_sensor_callback(uint gpio, uint32_t events);
};
