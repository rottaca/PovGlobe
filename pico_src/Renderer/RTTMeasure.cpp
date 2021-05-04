#include <stdio.h>

#include "RTTMeasure.hpp"

absolute_time_t RTTMeasure::m_measured_intervals[N_TIME_MEASURES] = {};
int8_t RTTMeasure::m_curr_segment_index = 0;
critical_section_t RTTMeasure::critical_section = {};
absolute_time_t RTTMeasure::m_last_hall_sensor_event = nil_time;

RTTMeasure::RTTMeasure()
{
    init();
    gpio_set_irq_enabled_with_callback(PIN_HALL_SENSOR, GPIO_IRQ_EDGE_RISE, true, &RTTMeasure::gpio_hall_sensor_callback);
}

RTTMeasure::~RTTMeasure()
{
}

void RTTMeasure::init(){
    for (size_t i = 0; i < N_TIME_MEASURES; i++)
    {
        m_measured_intervals[i] = nil_time;
    }
    m_curr_segment_index = 0;
    m_last_hall_sensor_event = nil_time;

    critical_section_init(&critical_section);
}

RTTMeasure& RTTMeasure::getInstance() {
    static RTTMeasure instance;
    return instance;
}
bool RTTMeasure::rotationDetected(){
    return absolute_time_diff_us(m_last_hall_sensor_event, get_absolute_time()) < 1000000;
}

uint32_t RTTMeasure::getCurrentColumn(){
    critical_section_enter_blocking(&critical_section);
    const uint8_t curr_segment_index = m_curr_segment_index;
    const absolute_time_t last_hall_sensor_event = m_last_hall_sensor_event;
    const absolute_time_t last_last_hall_sensor_event = (curr_segment_index >= 2)?m_measured_intervals[curr_segment_index-2]:m_measured_intervals[N_TIME_MEASURES+curr_segment_index-2];
    critical_section_exit(&critical_section);

    const int32_t avg_time_of_segment = absolute_time_diff_us(last_last_hall_sensor_event, last_hall_sensor_event);
    const uint32_t column_offset_for_segment_start = curr_segment_index*N_COLUMNS_PER_SEGMENT;

    const int32_t dt_since_hall_sensor_event = absolute_time_diff_us(last_hall_sensor_event, get_absolute_time());
    uint32_t columns_since_segment_start = dt_since_hall_sensor_event*N_COLUMNS_PER_SEGMENT/avg_time_of_segment;
    
    const uint32_t column = (columns_since_segment_start + column_offset_for_segment_start) % N_HORIZONTAL_RESOLUTION;
    return column;
}

void RTTMeasure::gpio_hall_sensor_callback(uint gpio, uint32_t events) {
    if (gpio != PIN_HALL_SENSOR)
        return;
    
    const absolute_time_t curr_time = get_absolute_time();

    critical_section_enter_blocking(&critical_section);
    m_last_hall_sensor_event = curr_time;
    m_measured_intervals[m_curr_segment_index] = curr_time;
    m_curr_segment_index = (m_curr_segment_index + 1U) % N_TIME_MEASURES;
    critical_section_exit(&critical_section);
}