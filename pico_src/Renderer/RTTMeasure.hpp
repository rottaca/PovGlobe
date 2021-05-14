#pragma once

#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/sync.h"
#include "string.h"

#include "constants.hpp"

#include <cmath>


#define N_COLUMNS_PER_SEGMENT (N_HORIZONTAL_RESOLUTION / N_MAGNETS)

class RTTMeasure
{
private:

    static critical_section_t critical_section;

    static uint32_t m_measured_intervals[N_MAGNETS];
    static int8_t m_curr_segment_index;
    static absolute_time_t m_last_hall_sensor_event;
    static absolute_time_t m_last_print;

    RTTMeasure();
    void init();
public:
    ~RTTMeasure();

    bool rotationDetected();
    float getCurrentColumn(const absolute_time_t& curr_time, uint32_t& column, uint32_t& opposite_column);

    static RTTMeasure& getInstance();
    static void gpio_hall_sensor_callback(uint gpio, uint32_t events);
};


inline bool RTTMeasure::rotationDetected(){
    return absolute_time_diff_us(m_last_hall_sensor_event, get_absolute_time()) < 1000000;
}

inline float RTTMeasure::getCurrentColumn(const absolute_time_t& curr_time, uint32_t& column, uint32_t& opposite_column){
    // uint32_t segments[N_MAGNETS];
    
    uint32_t total_rotation_time = 0;

    critical_section_enter_blocking(&critical_section);
    const uint8_t curr_segment_index = m_curr_segment_index;
    const uint8_t opposite_segment_index = (curr_segment_index + N_MAGNETS/2) % N_MAGNETS;
    const absolute_time_t last_hall_sensor_event = m_last_hall_sensor_event;
    const float exp_segment_time = m_measured_intervals[m_curr_segment_index];
    const uint32_t exp_opposite_segment_time = m_measured_intervals[opposite_segment_index];
    for(size_t i = 0; i < N_MAGNETS; i++){
        total_rotation_time += m_measured_intervals[i];
    }
    critical_section_exit(&critical_section);
    const float avg_segment_time = (float) total_rotation_time / N_MAGNETS;
    // printf("%d %d %d %d %d %d %d %d \n", m_measured_intervals[0] - avg_segment_time,
    //                                     m_measured_intervals[1] - avg_segment_time,
    //                                     m_measured_intervals[2] - avg_segment_time,
    //                                     m_measured_intervals[3] - avg_segment_time,
    //                                     m_measured_intervals[4] - avg_segment_time,
    //                                     m_measured_intervals[5] - avg_segment_time,
    //                                     m_measured_intervals[6] - avg_segment_time,
    //                                     m_measured_intervals[7] - avg_segment_time);

    const uint32_t column_offset_for_segment_start = curr_segment_index*N_COLUMNS_PER_SEGMENT;
    const float dt_since_hall_sensor_event = absolute_time_diff_us(last_hall_sensor_event, curr_time);
    const float columns_since_segment_start = dt_since_hall_sensor_event/exp_segment_time*N_COLUMNS_PER_SEGMENT;    
    float f_column = columns_since_segment_start + column_offset_for_segment_start;
    column = (uint32_t)round(f_column) % N_HORIZONTAL_RESOLUTION;

    const uint32_t opposite_column_offset_for_segment_start = opposite_segment_index*N_COLUMNS_PER_SEGMENT;
    const float opposite_columns_since_segment_start = (dt_since_hall_sensor_event)/exp_opposite_segment_time*N_COLUMNS_PER_SEGMENT;
    float f_opposite_column = opposite_columns_since_segment_start + opposite_column_offset_for_segment_start;
    opposite_column = (uint32_t)round(f_opposite_column) % N_HORIZONTAL_RESOLUTION;

    const float ratio = exp_segment_time / exp_opposite_segment_time;
    // if (ratio > 1){
    //     opposite_column = ((uint32_t)round(f_column + N_HORIZONTAL_RESOLUTION/2 + 2)) % N_HORIZONTAL_RESOLUTION;
    // }else{
    //     opposite_column = ((uint32_t)round(f_column + N_HORIZONTAL_RESOLUTION/2)) % N_HORIZONTAL_RESOLUTION;
    // }
    return ratio;
        
}