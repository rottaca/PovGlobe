#pragma once	

#include "core/renderer_base.hpp"

#include <vector>

class RendererLedStrip : public RendererBase
{
public:
    RendererLedStrip(
        RpmMeasureBase& rpm_measure_base, int led_strip_gpio_signal_pin);
    ~RendererLedStrip();

    void initialize(Globe& globe);

protected:
    int m_led_strip_gpio_pin;
    int m_led_strip_gpio_pin;
    int m_last_curr_temporal_pos;
    bool m_doublesided;


    std::vector<char> m_led_data;


    virtual void render(const Framebuffer&);
};
