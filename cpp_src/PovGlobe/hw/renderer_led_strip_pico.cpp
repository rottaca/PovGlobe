#include "renderer_led_strip_pico.hpp"
#include <iostream>
#include "core/globe.hpp"

#include <bcm2835.h>

RendererLedStripPico::RendererLedStripPico(const char* portname)
: m_portname(portname)
,m_fd(-1)
{
    
}

RendererLedStripPico::~RendererLedStripPico()
{
    
}

void RendererLedStripPico::initialize(Globe& globe)
{
    RendererBase::initialize(globe);
    m_fd = open(m_portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (m_fd < 0)
    {
        std::cout << "error " << errno << " opening " << m_portname << ": " << strerror (errno) << std::endl;
        return;
    }else{
        std::cout << "Connected to serial device" << std::endl;
    }

    set_interface_attribs (m_fd, B115200, 0);   // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (m_fd, 0);                     // set no blocking
}

void RendererLedStripPico::initSPI(Globe& globe) {

    int spiFrameLength = 6 + 2 + globe.getVerticalNumPixelsWithLeds()*3;

    m_led_data.resize(spiFrameLength);
 
    int16_t ledIndex = 0;

    //Init the start Frame
    m_led_data[0] = '+';
    m_led_data[1] = '*';
    m_led_data[2] = '+';
    m_led_data[3] = '*';
    m_led_data[4] = '+';
    m_led_data[5] = '*';
    m_led_data[6] = 0;
    //init each LED
    for (ledIndex = 7; ledIndex < spiFrameLength-1; ledIndex += 3)
    {
        m_led_data[ledIndex + 1] = 0;
        m_led_data[ledIndex + 2] = 0;
        m_led_data[ledIndex + 3] = 0;
    }
    // End Frame
    m_led_data[spiFrameLength - 1] = '\n';

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

void RendererLedStripPico::render(const Framebuffer& framebuffer)
{
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3U);
  
    for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
        m_led_data[6] = j;
        int buff_idx = 7;
        for(size_t i = 0; i < framebuffer.getHeight(); i++){
          m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 0)/2];
          m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 1)/2];
          m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 2)/2];
        }
        bcm2835_spi_writenb(m_led_data.data(), m_led_data.size());
    }

    // int n = read (m_fd, buf, sizeof buf);
    // if (n > 0){
    //     buf[n] = '\0';
    //     std::cerr << "PICO: " << buf << std::endl;
    // }
}

int RendererLedStripPico::set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
            std::cout << "error " << errno << " from tcgetattr" << std::endl;
            return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
            std::cout << "error " << errno << " from tcgetattr" << std::endl;
                return -1;
        }
        return 0;
}

void RendererLedStripPico::set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
            std::cout << "error " << errno << " from tcgetattr" << std::endl;
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
            std::cout << "error " << errno << " from tcgetattr" << std::endl;
}