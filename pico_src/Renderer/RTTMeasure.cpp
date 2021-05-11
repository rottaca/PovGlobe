#include <stdio.h>

#include "RTTMeasure.hpp"

uint32_t RTTMeasure::m_measured_intervals[N_MAGNETS] = {};
int8_t RTTMeasure::m_curr_segment_index = 0;
critical_section_t RTTMeasure::critical_section = {};
absolute_time_t RTTMeasure::m_last_hall_sensor_event = nil_time;
absolute_time_t RTTMeasure::m_last_print = nil_time;

RTTMeasure::RTTMeasure()
{
    init();
    gpio_set_irq_enabled_with_callback(PIN_HALL_SENSOR, GPIO_IRQ_EDGE_RISE, true, &RTTMeasure::gpio_hall_sensor_callback);
}

RTTMeasure::~RTTMeasure()
{
}

void RTTMeasure::init(){
    for (size_t i = 0; i < N_MAGNETS; i++)
    {
        m_measured_intervals[i] = get_absolute_time();
    }
    m_curr_segment_index = 0;
    m_last_hall_sensor_event = nil_time;

    critical_section_init(&critical_section);
}

RTTMeasure& RTTMeasure::getInstance() {
    static RTTMeasure instance;
    return instance;
}

void RTTMeasure::gpio_hall_sensor_callback(uint gpio, uint32_t events) {
    if (gpio != PIN_HALL_SENSOR)
        return;
    
    const absolute_time_t curr_time = get_absolute_time();

    critical_section_enter_blocking(&critical_section);
    if (!is_nil_time(m_last_hall_sensor_event)){
        m_measured_intervals[m_curr_segment_index] = (8 * m_measured_intervals[m_curr_segment_index] +
                                                      2 * absolute_time_diff_us(m_last_hall_sensor_event, curr_time)) / 10;
    }
    m_last_hall_sensor_event = curr_time;
    m_curr_segment_index = (m_curr_segment_index + 1U) % N_MAGNETS;
    critical_section_exit(&critical_section);
}