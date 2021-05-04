#pragma once

#include "core/renderer_base.hpp"

#include <vector>

class Globe;

class RendererLedStripPico : public RendererBase
{
public:
    RendererLedStripPico();
    ~RendererLedStripPico();

    void initialize(Globe &globe);
    void initSPI(Globe &globe);

protected:
    int m_fd;
    bool m_is_synced;
    Globe *m_globe;
    std::vector<unsigned char> m_led_data;

    void syncWithSlave(const Globe &globe);
    virtual void render(const Framebuffer &, int32_t horizontal_offset);
};
