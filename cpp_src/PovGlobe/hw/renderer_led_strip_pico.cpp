#include "renderer_led_strip_pico.hpp"
#include <iostream>
#include "core/globe.hpp"

#include <bcm2835.h>

RendererLedStripPico::RendererLedStripPico()
:m_fd(-1)
{
    
}

RendererLedStripPico::~RendererLedStripPico()
{
    bcm2835_spi_end();
    bcm2835_close();
}

void RendererLedStripPico::initialize(Globe& globe)
{
    RendererBase::initialize(globe);
    initSPI(globe);
}

void RendererLedStripPico::initSPI(Globe& globe) {

    int spiFrameLength = globe.getHorizontalNumPixels()*globe.getVerticalNumPixelsWithLeds()*3U;

    m_led_data.resize(spiFrameLength);
 
    int16_t ledIndex = 0;

    //Init the start Frame
    //init each LED
    for (ledIndex = 0; ledIndex < spiFrameLength/3; ledIndex ++)
    {
        m_led_data[3*ledIndex] = 10;
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
    
    /*
    because of core_freq = 250, rpi2 timings count !
    BCM2835_SPI_CLOCK_DIVIDER_65536 	
    65536 = 3.814697260kHz on Rpi2, 6.1035156kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_32768 	
    32768 = 7.629394531kHz on Rpi2, 12.20703125kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_16384 	
    16384 = 15.25878906kHz on Rpi2, 24.4140625kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_8192 	
    8192 = 30.51757813kHz on Rpi2, 48.828125kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_4096 	
    4096 = 61.03515625kHz on Rpi2, 97.65625kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_2048 	
    2048 = 122.0703125kHz on Rpi2, 195.3125kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_1024 	
    1024 = 244.140625kHz on Rpi2, 390.625kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_512 	
    512 = 488.28125kHz on Rpi2, 781.25kHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_256 	
    256 = 976.5625kHz on Rpi2, 1.5625MHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_128 	
    128 = 1.953125MHz on Rpi2, 3.125MHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_64 	
    64 = 3.90625MHz on Rpi2, 6.250MHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_32 	
    32 = 7.8125MHz on Rpi2, 12.5MHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_16 	
    16 = 15.625MHz on Rpi2, 25MHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_8 	
    8 = 31.25MHz on Rpi2, 50MHz on RPI3
    BCM2835_SPI_CLOCK_DIVIDER_4 	
    4 = 62.5MHz on Rpi2, 100MHz on RPI3. Dont expect this speed to work reliably.
    BCM2835_SPI_CLOCK_DIVIDER_2 	
    2 = 125MHz on Rpi2, 200MHz on RPI3, fastest you can get. Dont expect this speed to work reliably.
    BCM2835_SPI_CLOCK_DIVIDER_1 	
    1 = 3.814697260kHz on Rpi2, 6.1035156kHz on RPI3, same as 0/65536
    */
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    std::cout << "SPI initialized!" << std::endl;
    std::cout << "Bytes per Transmission: " << m_led_data.size() << std::endl;
    std::cout << "End Byte: " << (int)m_led_data[spiFrameLength - 1] << std::endl;

    usleep(1000000);
}

void RendererLedStripPico::render(const Framebuffer& framebuffer)
{
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3U);
    int buff_idx = 0;
    for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
      for(size_t i = 0; i < framebuffer.getHeight(); i++){
        m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 0)/2];
        m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 1)/2];
        m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 2)/2];
      }
    }
    
    int bytes_sent = 0;
    int default_junk_size = 2048;
    int junk_nr = 0;
    while (bytes_sent < m_led_data.size()) {
        int junk_size = default_junk_size;

        if (bytes_sent + default_junk_size > m_led_data.size()) {
            junk_size -= (bytes_sent + default_junk_size) - m_led_data.size();
        }
        std::cout << bytes_sent << "-" << bytes_sent + junk_size << ": " << junk_nr << std::endl;
        for (size_t j = 0; j < junk_size; j++) {
            int rx = bcm2835_spi_transfer(m_led_data[bytes_sent+j]);
            if (rx != junk_nr) {
                std::cout << bytes_sent + j << ": " << rx << " != " << junk_nr << std::endl;
            }
        }

        bytes_sent += junk_size;
        junk_nr++;
    }

    int res = bcm2835_spi_transfer(42);
    if (res != 255) {
        std::cout << "error: " << res << std::endl;
    }
}    