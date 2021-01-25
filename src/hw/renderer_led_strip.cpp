#include "renderer_led_strip.hpp"
#include <iostream>
#include "core/globe.hpp"

#include <bcm2835.h>


unsigned char led_lut_r[256] = {
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

unsigned char led_lut_g[256] = {

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
unsigned char led_lut_b[256] = {

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

RendererLedStrip::RendererLedStrip(RpmMeasureBase& rpm_measure_base)
    :RendererBase(rpm_measure_base)
    , m_last_curr_temporal_pos(-1)
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
    //init each LED
    for (ledIndex = 4; ledIndex < spiFrameLength - (endFrameLength * bytesPerLED); ledIndex += bytesPerLED)
    {
        m_led_data[ledIndex] = 255; //0xE0 | 11;
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
        exit(1);
    }

    if (!bcm2835_spi_begin())
    {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        exit(1);
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
    auto start = std::chrono::high_resolution_clock::now();
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3);
    const int width = framebuffer.getWidth();
    const int height = framebuffer.getHeight();

    auto rpmData = m_rpmMeasure.getRpmData();
    if (!rpmData.valid || rpmData.curr_temporal_pos == m_last_curr_temporal_pos) {
        return;
    } else if ( (m_last_curr_temporal_pos < rpmData.curr_temporal_pos) && (rpmData.curr_temporal_pos - m_last_curr_temporal_pos > 1)){
      std::cout << "Skipped " << rpmData.curr_temporal_pos - m_last_curr_temporal_pos - 1 << " steps!" << std::endl;
      std::cout << "Last " << m_last_curr_temporal_pos << " curr " << rpmData.curr_temporal_pos << std::endl;
    }
    m_last_curr_temporal_pos = rpmData.curr_temporal_pos;

    const auto led_idx = [](size_t i) {return 4 + i*4; };
    const float max_brightness = 128.0f;
    const float pixel_scaling = max_brightness/255.0f;

    auto& led_data = m_led_data;
    const auto set_pixel = [led_lut_b, led_lut_g, led_lut_r, &framebuffer, &led_data, pixel_scaling](int x, int i, int led_idx) {
        const unsigned char r = framebuffer(x, i, 0)*pixel_scaling;
        const unsigned char g = framebuffer(x, i, 1)*pixel_scaling;
        const unsigned char b = framebuffer(x, i, 2)*pixel_scaling;
        led_data[led_idx + 1] = led_lut_b[b];
        led_data[led_idx + 2] = led_lut_g[g];
        led_data[led_idx + 3] = led_lut_r[r];
       // printf("%d, %d, %d\n", (int)(b*pixel_scaling),(int)(g*pixel_scaling),(int)(r*pixel_scaling));
    };      

    // Render first half of globe from top to bottom
    for (int i = 0; i < height; i++) {
        set_pixel(rpmData.curr_temporal_pos, i, led_idx(i));
    }
    // If double sided rendering is enabled (and if the globe as leds on the opposite side as well,
    // render the other side of the globe from bottom to top (with current wiring)
    // This might need adaption if the wiring of leds changes.
    if (m_doublesided) {
        const int framebuffer_x = (rpmData.curr_temporal_pos + width / 2) % width;
        for (int i = 0; i < height; i++) {
          set_pixel(framebuffer_x, i, led_idx(2*height -1 - i));
        }
    }

    bcm2835_spi_writenb(m_led_data.data(), m_led_data.size());
    bcm2835_delayMicroseconds(100);
    
    auto finish = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = finish - start;
    //std::cout << "Time To process" << duration.count() << std::endl;
}
