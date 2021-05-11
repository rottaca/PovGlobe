#pragma once

#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/sync.h"
#include "string.h"

#include "constants.hpp"


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
    uint32_t getCurrentColumn();

    static RTTMeasure& getInstance();
    static void gpio_hall_sensor_callback(uint gpio, uint32_t events);
};


inline bool RTTMeasure::rotationDetected(){
    return absolute_time_diff_us(m_last_hall_sensor_event, get_absolute_time()) < 1000000;
}

inline uint32_t RTTMeasure::getCurrentColumn(){
    // uint32_t segments[N_MAGNETS];
    
    critical_section_enter_blocking(&critical_section);
    const uint8_t curr_segment_index = m_curr_segment_index;
    const absolute_time_t last_hall_sensor_event = m_last_hall_sensor_event;
    const uint32_t exp_segment_time = m_measured_intervals[m_curr_segment_index];
    // memcpy(segments, m_measured_intervals, N_MAGNETS*sizeof(uint32_t));
    critical_section_exit(&critical_section);

    const uint32_t column_offset_for_segment_start = curr_segment_index*N_COLUMNS_PER_SEGMENT;
    const uint32_t dt_since_hall_sensor_event = absolute_time_diff_us(last_hall_sensor_event, get_absolute_time());
    const uint32_t columns_since_segment_start = dt_since_hall_sensor_event*N_COLUMNS_PER_SEGMENT/exp_segment_time;

    // if (absolute_time_diff_us(m_last_print, get_absolute_time()) > 100000){
    //     printf("%u %u %u %u %u %u %u %u\n", segments[0],  segments[1],  segments[2],  segments[3],  segments[4],  segments[5],  segments[6],  segments[7]);
    //     m_last_print = get_absolute_time();
    // }
        
    
    uint32_t column = columns_since_segment_start + column_offset_for_segment_start;
    if (column >= N_HORIZONTAL_RESOLUTION) {
        column = N_HORIZONTAL_RESOLUTION - 1U;
    }
    
    return column;
}