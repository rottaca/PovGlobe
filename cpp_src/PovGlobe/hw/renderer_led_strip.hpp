#pragma once	

#include "core/renderer_base.hpp"

#include <fstream>
#include <vector>

unsigned char led_lut[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

class RendererLedStrip : public RendererBase
{
public:
    RendererLedStrip(
        RpmMeasureBase& rpm_measure_base);
    ~RendererLedStrip();

    void initialize(Globe& globe);

protected:
    int m_last_temporal_pos;
    int m_next_temporal_pos;
    bool m_doublesided;
    bool m_nextRowInitialized;
    std::vector<char> m_led_data;
    std::ofstream myfile;
    std::vector<float> m_time1, m_time2;

    virtual void render(const Framebuffer&);

    void setPixel(const Framebuffer& framebuffer, 
                  uint32_t x, uint32_t pixel_index, uint32_t led_index);
                  
    void prepareLedData(const Framebuffer& framebuffer);   
};


inline void RendererLedStrip::setPixel(const Framebuffer& framebuffer, uint32_t x,
                                uint32_t pixel_index, uint32_t led_index) {

    const uint8_t pixel_scaling = 2U;

    const size_t buffer_index = 4U + led_index * 4U;

    const unsigned char r = framebuffer(x, pixel_index, 0U);
    const unsigned char g = framebuffer(x, pixel_index, 1U);
    const unsigned char b = framebuffer(x, pixel_index, 2U);
    m_led_data[buffer_index + 1U] = led_lut[b] / pixel_scaling;
    m_led_data[buffer_index + 2U] = led_lut[g] / pixel_scaling;
    m_led_data[buffer_index + 3U] = led_lut[r] / pixel_scaling;
};