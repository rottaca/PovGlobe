#include "renderer_led_strip.hpp"
#include <iostream>
#include "core/globe.hpp"

#include <bcm2835.h>

RendererLedStrip::RendererLedStrip(RpmMeasureBase& rpm_measure_base, int led_strip_gpio_pin)
    :RendererBase(rpm_measure_base)
    , m_led_strip_gpio_pin(led_strip_gpio_pin)
    , m_last_curr_temporal_pos(-1)
    , m_ledstring({})
{

}

RendererLedStrip::~RendererLedStrip()
{
    //close the spi bus
    bcm2835_spi_end();
    bcm2835_close();
}

void RendererLedStrip::initialize(Globe& globe)
{
    RendererBase::initialize(globe);
    std::cout << "Initialize led strip..." << std::endl;

    int num_leds = globe.getHeight();
    m_last_curr_temporal_pos = -1;

    // If we use double sided rendering, the led strip has twice the amount of leds.
    if (globe.getDoubleSidedRendering()) {
        num_leds *= 2;
        m_doublesided = true;
    }
    else {
        m_doublesided = false;
    }

    //Set up Data Block for LEDs
    const uint8_t bytesPerLED = 4;
    const int16_t endFrameLength = round( (num_leds / 2.0f) / 8 );
    const int16_t spiFrameLength = (2+ num_leds +endFrameLength)*bytesPerLED;

    m_led_data.resize(spiFrameLength);

    int16_t ledIndex = 0;

    //Init the start Frame
    m_led_data[0] = 0;
    m_led_data[1] = 0;
    m_led_data[2] = 0;
    m_led_data[3] = 0;
    //Init the Driver LED
    m_led_data[4] = 255;
    m_led_data[5] = 0;
    m_led_data[6] = 0;
    m_led_data[7] = 0;
    //init each LED
    for (ledIndex = 8; ledIndex < spiFrameLength - (endFrameLength * bytesPerLED); ledIndex += bytesPerLED)
    {
        m_led_data[ledIndex] = 255;
        m_led_data[ledIndex + 1] = 0;
        m_led_data[ledIndex + 2] = 0;
        m_led_data[ledIndex + 3] = 0;
    }
    //init the end frame
    for (ledIndex; ledIndex < spiFrameLength; ledIndex += bytesPerLED)
    {
        m_led_data[ledIndex] = 255;
        m_led_data[ledIndex + 1] = 255;
        m_led_data[ledIndex + 2] = 255;
        m_led_data[ledIndex + 3] = 255;
    }


    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root??\n");
        return 1;
    }

    if (!bcm2835_spi_begin())
    {
        printf("bcm2835_spi_begin failedg. Are you running as root??\n");
        return 1;
    }

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    /*

    BCM2835_SPI_MODE0 		CPOL = 0, CPHA = 0
    BCM2835_SPI_MODE1 		CPOL = 0, CPHA = 1
    BCM2835_SPI_MODE2 		CPOL = 1, CPHA = 0
    BCM2835_SPI_MODE3 		CPOL = 1, CPHA = 1
    */
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); // The default
    /*
        BCM2835_SPI_CLOCK_DIVIDER_65536 	65536 = 262.144us = 3.814697260kHz
        BCM2835_SPI_CLOCK_DIVIDER_32768 	32768 = 131.072us = 7.629394531kHz
        BCM2835_SPI_CLOCK_DIVIDER_16384 	16384 = 65.536us = 15.25878906kHz
        BCM2835_SPI_CLOCK_DIVIDER_8192 		8192 = 32.768us = 30/51757813kHz
        BCM2835_SPI_CLOCK_DIVIDER_4096 		4096 = 16.384us = 61.03515625kHz
        BCM2835_SPI_CLOCK_DIVIDER_2048 		2048 = 8.192us = 122.0703125kHz
        BCM2835_SPI_CLOCK_DIVIDER_1024 		1024 = 4.096us = 244.140625kHz
        BCM2835_SPI_CLOCK_DIVIDER_512 		512 = 2.048us = 488.28125kHz
        BCM2835_SPI_CLOCK_DIVIDER_256 		256 = 1.024us = 976.5625kHz
        BCM2835_SPI_CLOCK_DIVIDER_128 		128 = 512ns = = 1.953125MHz
        BCM2835_SPI_CLOCK_DIVIDER_64 		64 = 256ns = 3.90625MHz
        BCM2835_SPI_CLOCK_DIVIDER_32 		32 = 128ns = 7.8125MHz
        BCM2835_SPI_CLOCK_DIVIDER_16 		16 = 64ns = 15.625MHz
        BCM2835_SPI_CLOCK_DIVIDER_8 		8 = 32ns = 31.25MHz
        BCM2835_SPI_CLOCK_DIVIDER_4 		4 = 16ns = 62.5MHz
        BCM2835_SPI_CLOCK_DIVIDER_2 		2 = 8ns = 125MHz, fastest you can get

    */
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
}


void RendererLedStrip::render(const Framebuffer& framebuffer)
{
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3);
    const int width = framebuffer.getWidth();
    const int height = framebuffer.getHeight();

    auto rpmData = m_rpmMeasure.getRpmData();
    if (!rpmData.valid || rpmData.curr_temporal_pos == m_last_curr_temporal_pos) {
        return;
    }
    int framebuffer_x = rpmData.curr_temporal_pos;
    m_last_curr_temporal_pos = framebuffer_x;

    const auto led_idx = [](size_t i) {return 8 + i*4 };

    // Render first half of globe from top to bottom
    for (int i = 0; i < height; i++) {
        int r = framebuffer(framebuffer_x, i, 0);
        int g = framebuffer(framebuffer_x, i, 1);
        int b = framebuffer(framebuffer_x, i, 2);
        m_led_data[led_idx(i) + 1] = r;
        m_led_data[led_idx(i) + 2] = g;
        m_led_data[led_idx(i) + 3] = b;
    }
    // If double sided rendering is enabled (and if the globe as leds on the opposite side as well,
    // render the other side of the globe from bottom to top (with current wiring)
    // This might need adaption if the wiring of leds changes.
    if (m_doublesided) {
        framebuffer_x = (framebuffer_x + width / 2) % width;
        for (int i = 0; i < height; i++) {
            int r = framebuffer(framebuffer_x, i, 0);
            int g = framebuffer(framebuffer_x, i, 1);
            int b = framebuffer(framebuffer_x, i, 2);
            // Fill second half from bottom to top (inverted)
            m_led_data[led_idx(i)+1] = r;
            m_led_data[led_idx(i) + 2] = g;
            m_led_data[led_idx(i) + 3] = b;
        }
    }

    bcm2835_spi_writenb(m_led_data.data(), m_led_data.size());
    bcm2835_delay(5);
}
