#pragma once


#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/time.h"


#define GPIO_HALL_SENSOR 5


class RTTMeasure
{
private:
    static absolute_time_t last_hall_sensor_event;
    static int64_t round_trip_time;

    RTTMeasure();
public:
    ~RTTMeasure();

    absolute_time_t getTimeLastHallSensorEvent();
    int64_t getRtt();
    int64_t getDeltaTimeSinceLastEvent();


    static RTTMeasure& getInstance();
    static void gpio_hall_sensor_callback(uint gpio, uint32_t events);
};
