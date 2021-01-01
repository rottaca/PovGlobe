#include "renderer_led_strip.hpp"

#include "core/globe.hpp"

RendererLedStrip::RendererLedStrip(RpmMeasureBase& rpm_measure_base, int led_strip_gpio_pin)
	:RendererBase(rpm_measure_base)
	, m_led_strip_gpio_pin(led_strip_gpio_pin)
{
	
}

RendererLedStrip::~RendererLedStrip()
{
	ws2811_fini(&m_ledstring);
}


void RendererLedStrip::initialize(Globe& globe)
{
	RendererBase::initialize(globe);
	m_ledstring.freq = TARGET_FREQ;
	m_ledstring.dmanum = DMA;
	m_ledstring.channel[0].gpionum = m_led_strip_gpio_pin;
	m_ledstring.channel[0].count = globe.getHeight();
	m_ledstring.channel[0].invert = 0;
	m_ledstring.channel[0].brightness = 255;
	m_ledstring.channel[0].strip_type = STRIP_TYPE;
	
	m_ledstring.channel[1].gpionum = 0;
	m_ledstring.channel[1].count = 0;
	m_ledstring.channel[1].invert = 0;
	m_ledstring.channel[1].brightness = 0;
	m_ledstring.channel[1].strip_type = 0;
	
	ws2811_return_t ret = ws2811_init(&m_ledstring);
    if (ret != WS2811_SUCCESS){
		exit(2);
	}
}


void RendererLedStrip::render(const Framebuffer& framebuffer)
{
	auto rpmData = m_rpmMeasure.getRpmData();
	int framebuffer_x = rpmData.curr_temporal_pos;
	assert(framebuffer.getChannels() == 3);
	for (int i  = 0; i < framebuffer.getHeight(); i++){
		int r = framebuffer(framebuffer_x, i, 0);
		int g = framebuffer(framebuffer_x, i, 1);
		int b = framebuffer(framebuffer_x, i, 2);
		m_ledstring.channel[0].leds[i] = (r << 16) | (g << 8) | (b << 0);	
	}
	ws2811_return_t ret = ws2811_render(&m_ledstring);
	if (ret != WS2811_SUCCESS)
	{
		exit(3);
	}
}
