#pragma once	

#include "core/renderer_base.hpp"

#include <fstream>
#include <vector>

class RendererLedStrip : public RendererBase
{
public:
    RendererLedStrip(
        RpmMeasureBase& rpm_measure_base);
    ~RendererLedStrip();

    void initialize(Globe& globe);

protected:
    int m_last_curr_temporal_pos;
    bool m_doublesided;


    std::vector<char> m_led_data;
    std::ofstream myfile;


    virtual void render(const Framebuffer&);
};
