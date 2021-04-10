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

uint32_t RTTMeasure::getCurrentColumn(uint32_t maxColumnsPerRotation, uint64_t& timeUntilNextColumn){
    const uint32_t columns_per_segment = maxColumnsPerRotation / N_MAGNETS;

    const int64_t exp_segment_dt = measured_intervals[curr_segment_index];
    const int64_t dt_since_hall_sensor_event = absolute_time_diff_us(last_hall_sensor_event, get_absolute_time());

    const uint32_t columns_since_segment_start = dt_since_hall_sensor_event*columns_per_segment/exp_segment_dt;
    const uint32_t column_offset_for_segment_start = curr_segment_index*columns_per_segment;
    const uint32_t column = (columns_since_segment_start + column_offset_for_segment_start) % maxColumnsPerRotation; 
    
    //printf("RTT             : %lld us\n", rtt);
    //printf("Time Since Event: %lld us\n", dt_since_hall_sensor_event);
    //printf("Curr Column     : %lu\n", curr_column);
    
    //uint64_t avg = 0;
    //for(int i = 0; i < N_MAGNETS; i++){
    //  avg += measured_intervals[i];
    //}
    //avg /= N_MAGNETS;
    
    //printf("Segment Times (%lld us avg): ", avg);
    //for(int i = 0; i < N_MAGNETS; i++){
    //  printf("%lld, ", measured_intervals[i] - avg);
    //}
    //printf("\n");
    
    //const uint32_t time_per_column =  exp_segment_dt / columns_per_segment;
    //const uint32_t time_in_curr_column = dt_since_hall_sensor_event - time_per_column*columns_since_segment_start;
    
    //const int64_t time_until_next_column = time_per_column - time_in_curr_column;
    //if(time_until_next_column > 0){
    //  timeUntilNextColumn = time_until_next_column;
    //}else{
    //  timeUntilNextColumn = 0;
    //}
    //printf("TimePerCol: %lu, TimeInCurrCol: %lu, Sleep Time: %lld\n", time_per_column, time_in_curr_column, time_until_next_column);
    return column;
}

void RTTMeasure::gpio_hall_sensor_callback(uint gpio, uint32_t events) {
    if (gpio != GPIO_HALL_SENSOR)
        return;
    
    const absolute_time_t curr_time = get_absolute_time();
    if (!is_nil_time(last_hall_sensor_event)) {
        const int64_t new_dt = absolute_time_diff_us (last_hall_sensor_event, curr_time);
        //const int64_t old_dt = measured_intervals[curr_segment_index];
        measured_intervals[curr_segment_index] = new_dt;
        
        //rtt += new_dt - old_dt;

        curr_segment_index = (curr_segment_index + 1) % N_MAGNETS;
    }
    last_hall_sensor_event = curr_time;
}