#include "renderer_led_strip_pico.hpp"
#include <iostream>
#include "core/globe.hpp"

#include <bcm2835.h>

#define SPI_MASTER_END_BYTE 42
#define SPI_SLAVE_END_BYTE 255
#define SPI_COM_START_BYTE 48
#define SPI_DATA_JUNK_SIZE 8192

unsigned char led_lut[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};

RendererLedStripPico::RendererLedStripPico()
    : m_fd(-1)
    , m_globe(nullptr)
    , m_is_synced(false)
{
}

RendererLedStripPico::~RendererLedStripPico()
{
    bcm2835_spi_end();
    bcm2835_close();
}

void RendererLedStripPico::initialize(Globe &globe)
{
    RendererBase::initialize(globe);
    m_globe = &globe;
    initSPI(globe);
}

void RendererLedStripPico::initSPI(Globe &globe)
{
    int spiFrameLength = globe.getHorizontalNumPixels() * globe.getVerticalNumPixelsWithLeds() * 3U;

    m_led_data.resize(spiFrameLength);

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
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_128);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    std::cout << "SPI initialized!" << std::endl;
    syncWithSlave(globe);
}

void RendererLedStripPico::syncWithSlave(const Globe &globe){
    const int rpi_vertical_res = globe.getVerticalNumPixelsWithLeds();
    const int rpi_horizontal_res = globe.getHorizontalNumPixels();
    const int num_channels = 3U;

    int num_start_bytes = 0;
    while(num_start_bytes < 3){
        std::cout << "Waiting for pico..." << std::endl;
        usleep(1000000);
        if(bcm2835_spi_transfer(0) != SPI_COM_START_BYTE){
            num_start_bytes = 0;
        }else{
            num_start_bytes ++;
        }
    }

    const int pico_vertical_res = bcm2835_spi_transfer(rpi_vertical_res);
    const int pico_horizontal_res = bcm2835_spi_transfer(rpi_horizontal_res);
    const int pico_channels = bcm2835_spi_transfer(num_channels);
    const int pico_junk_size_high = bcm2835_spi_transfer((uint8_t)((SPI_DATA_JUNK_SIZE) >> 8) & 0xff);
    const int pico_junk_size_low = bcm2835_spi_transfer((uint8_t)(SPI_DATA_JUNK_SIZE & 0xff));
    const int pico_junk_size = (((int32_t)pico_junk_size_high) << 8) | pico_junk_size_low;

    std::cout << "Checking pico config..." << std::endl;
    bool failed = false;
    if (pico_vertical_res != rpi_vertical_res){
        std::cout << "Vertical resolution does not match: " << pico_vertical_res << " (pico) vs " <<  rpi_vertical_res << " (rpi)." << std::endl;
        failed = true;
    }
    if (pico_horizontal_res != rpi_horizontal_res){
        std::cout << "Horizontal resolution does not match: " << pico_horizontal_res << " (pico) vs " <<  rpi_horizontal_res << " (rpi)." << std::endl;
        failed = true;
    }
    if (pico_channels != num_channels){
        std::cout << "Number of channels per pixel does not match: " << pico_channels << " (pico) vs " <<  num_channels << " (rpi)." << std::endl;
        failed = true;
    }
    if (pico_junk_size != SPI_DATA_JUNK_SIZE){
        std::cout << "SPI junk size does not match: " << pico_junk_size << " (pico) vs " <<  SPI_DATA_JUNK_SIZE << " (rpi)." << std::endl;
        failed = true;
    }

    if (failed){
        std::cout << "Pico configuration does not mach the compiled rpi binary. Aborting." << std::endl;
        exit(1);
    }else{
        std::cout << "Pico config matches rpi. Continuing.." << std::endl;
    }
}

void RendererLedStripPico::render(const Framebuffer &framebuffer, int32_t horizontal_offset)
{
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3U);
    int buff_idx = 0;
    for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
        const size_t j_offsetted = (j + horizontal_offset) %  framebuffer.getWidth();
        for (size_t i = 0; i < framebuffer.getHeight(); i++)
        {
            m_led_data[buff_idx++] = led_lut[framebuffer(j_offsetted, i, 0)];
            m_led_data[buff_idx++] = led_lut[framebuffer(j_offsetted, i, 1)];
            m_led_data[buff_idx++] = led_lut[framebuffer(j_offsetted, i, 2)];
        }
    }

    size_t bytes_sent = 0;
    size_t junk_nr = 0;
    size_t junk_size = 0;
    while (bytes_sent < m_led_data.size())
    {
        if (bytes_sent + SPI_DATA_JUNK_SIZE > m_led_data.size())
        {
            junk_size = m_led_data.size() - bytes_sent;
        } else {
            junk_size = SPI_DATA_JUNK_SIZE;
        }
        
        for (size_t j = 0; j < junk_size; j++)
        {
            int rx = bcm2835_spi_transfer(m_led_data[bytes_sent + j]);
            if (rx != junk_nr)
            {
                std::cout << "Error: Pico failed to recieve the data or it is not sending the correct response. Aborting." << std::endl;
                std::cout << "Junk " << junk_nr << ", byte nr " << j << " in junk, received: " << rx << std::endl;
                syncWithSlave(*m_globe);
                return;
            }
        }
        usleep(10);

        bytes_sent += junk_size;
        junk_nr++;
    }
    usleep(100);

    int res = bcm2835_spi_transfer(SPI_MASTER_END_BYTE);
    if (res != SPI_SLAVE_END_BYTE)
    {
        std::cout << "Error: Pico failed to recieve the data or it is not sending the correct response. Aborting." << std::endl;
        syncWithSlave(*m_globe);
        return;
    }
    usleep(1000);

    //syncWithSlave();
}

