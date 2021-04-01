#include <stdio.h>

#include "RTTMeasure.hpp"

int64_t RTTMeasure::round_trip_time = 0;
absolute_time_t RTTMeasure::last_hall_sensor_event = nil_time;

RTTMeasure::RTTMeasure()
{
    last_hall_sensor_event = nil_time;
    round_trip_time = 0;
    gpio_set_irq_enabled_with_callback(GPIO_HALL_SENSOR, GPIO_IRQ_EDGE_RISE, true, &RTTMeasure::gpio_hall_sensor_callback);
}

RTTMeasure::~RTTMeasure()
{
}

RTTMeasure& RTTMeasure::getInstance() {
    static RTTMeasure instance;
    return instance;
}

int64_t RTTMeasure::getRtt(){
    return round_trip_time;
}

absolute_time_t RTTMeasure::getTimeLastHallSensorEvent() {
    return last_hall_sensor_event;
}

int64_t RTTMeasure::getDeltaTimeSinceLastEvent(){
    return absolute_time_diff_us(last_hall_sensor_event, get_absolute_time());
}


void RTTMeasure::gpio_hall_sensor_callback(uint gpio, uint32_t events) {
    if (gpio != GPIO_HALL_SENSOR)
        return;

    const absolute_time_t curr_time = get_absolute_time();
    if (!is_nil_time(last_hall_sensor_event)) {
        round_trip_time = absolute_time_diff_us (last_hall_sensor_event, curr_time);
    }
    last_hall_sensor_event = curr_time;
}