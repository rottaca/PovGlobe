#include <stdio.h>

#include "RTTMeasure.hpp"

int64_t RTTMeasure::measured_intervals[N_MAGNETS] = {};
int8_t RTTMeasure::curr_segment_index = 0;
absolute_time_t RTTMeasure::last_hall_sensor_event = nil_time;

RTTMeasure::RTTMeasure()
{
    init();

    gpio_set_irq_enabled_with_callback(GPIO_HALL_SENSOR, GPIO_IRQ_EDGE_RISE, true, &RTTMeasure::gpio_hall_sensor_callback);
}

RTTMeasure::~RTTMeasure()
{
}

void RTTMeasure::init(){
    for (size_t i = 0; i < N_MAGNETS; i++)
    {
        measured_intervals[i] = 0;
    }
    curr_segment_index = 0;
    last_hall_sensor_event = nil_time;
}

RTTMeasure& RTTMeasure::getInstance() {
    static RTTMeasure instance;
    return instance;
}
bool RTTMeasure::rotationDetected(){
    const int64_t dt_since_hall_sensor_event = absolute_time_diff_us(last_hall_sensor_event, get_absolute_time());
    return (dt_since_hall_sensor_event < 1000000);
}

uint32_t RTTMeasure::getCurrentColumn(uint32_t maxColumnsPerRotation){
    const uint32_t columns_per_segment = maxColumnsPerRotation / N_MAGNETS;

    const int64_t exp_segment_dt = measured_intervals[curr_segment_index];
    const int64_t dt_since_hall_sensor_event = absolute_time_diff_us(last_hall_sensor_event, get_absolute_time());

    const uint32_t columns_since_segment_start = dt_since_hall_sensor_event*columns_per_segment/exp_segment_dt;
    const uint32_t column_offset_for_segment_start = curr_segment_index*columns_per_segment;
    const uint32_t column = (columns_since_segment_start + column_offset_for_segment_start) % maxColumnsPerRotation; 
    return column;
}

void RTTMeasure::gpio_hall_sensor_callback(uint gpio, uint32_t events) {
    if (gpio != GPIO_HALL_SENSOR)
        return;
    
    const absolute_time_t curr_time = get_absolute_time();
    if (!is_nil_time(last_hall_sensor_event)) {
        const int64_t new_dt = absolute_time_diff_us (last_hall_sensor_event, curr_time);
        const int64_t old_dt = measured_intervals[curr_segment_index];
        measured_intervals[curr_segment_index] = new_dt;
        
        //rtt += new_dt - old_dt;

        curr_segment_index = (curr_segment_index + 1) % N_MAGNETS;
    }
    last_hall_sensor_event = curr_time;
}