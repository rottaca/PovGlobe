#pragma once	

#include "core/renderer_base.hpp"
#include <ws2811.h>

// defaults for cmdline options
#define TARGET_FREQ             800000 //WS2811_TARGET_FREQ
#define DMA                     10
#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds

class RendererLedStrip: public RendererBase
{
public:
	RendererLedStrip(
		RpmMeasureBase& rpm_measure_base, int led_strip_gpio_pin);
	~RendererLedStrip();

	void initialize(Globe& globe);

protected:
	int m_led_strip_gpio_pin;
  int m_last_curr_temporal_pos;
  bool m_doublesided;
	ws2811_t m_ledstring;

	virtual void render(const Framebuffer&);
};
