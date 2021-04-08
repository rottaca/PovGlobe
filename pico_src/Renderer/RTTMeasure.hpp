#pragma once


#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/time.h"


#define GPIO_HALL_SENSOR 5
#define N_MAGNETS 8

class RTTMeasure
{
private:
    static absolute_time_t last_hall_sensor_event;

    static int64_t measured_intervals[N_MAGNETS];
    static int8_t curr_segment_index;

    static int64_t rtt;

    RTTMeasure();
    void init();
public:
    ~RTTMeasure();

    bool rotationDetected();
    uint32_t getCurrentColumn(uint32_t maxColumnsPerRotation);

    static RTTMeasure& getInstance();
    static void gpio_hall_sensor_callback(uint gpio, uint32_t events);
};
