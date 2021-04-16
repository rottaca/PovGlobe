#include "renderer_led_strip_pico.hpp"
#include <iostream>
#include "core/globe.hpp"

#include <bcm2835.h>

RendererLedStripPico::RendererLedStripPico(const std::string portname)
: m_portname(portname)
,m_fd(-1)
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
    m_fd = open(m_portname.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (m_fd < 0)
    {
        std::cout << "error " << errno << " opening " << m_portname << ": " << strerror (errno) << std::endl;
        return;
    }else{
        std::cout << "Connected to serial device" << std::endl;
    }

    set_interface_attribs (m_fd, B3000000, 0);   // set speed to 115,200 bps, 8n1 (no parity)
    set_blocking (m_fd, 0);                     // set no blocking

    initSPI(globe);
}

void RendererLedStripPico::initSPI(Globe& globe) {

    int spiFrameLength = globe.getHorizontalNumPixels()*globe.getVerticalNumPixelsWithLeds()*3U + 1U;

    m_led_data.resize(spiFrameLength);
 
    int16_t ledIndex = 0;

    //Init the start Frame
    //init each LED
    for (ledIndex = 0; ledIndex < spiFrameLength; ledIndex += 3)
    {
        m_led_data[ledIndex + 0] = 0;
        m_led_data[ledIndex + 1] = 50;
        m_led_data[ledIndex + 2] = 0;
    }
    m_led_data[spiFrameLength -1] = 42;

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
}

void RendererLedStripPico::render(const Framebuffer& framebuffer)
{
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3U);
  
    /*for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
        m_led_data[6] = j;
        int buff_idx = 7;
        for(size_t i = 0; i < framebuffer.getHeight(); i++){
          m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 0)/2];
          m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 1)/2];
          m_led_data[buff_idx++] = led_lut[framebuffer(j, i, 2)/2];
        }
        //std::fill(m_led_data.begin(), m_led_data.end(), 42);
        //std::cout << "Sending "<<m_led_data.size()<< " bytes to pico" << std::endl;
        //bcm2835_spi_writenb(m_led_data.data(), m_led_data.size());
    }*/
    
    //for (size_t j = 0; j < m_led_data.size()-1; j++)
    //  m_led_data[j] = j % 50;
      
    
    for (size_t j = 0; j < m_led_data.size(); j++){
      //std::cout <<  (int)m_led_data[j] << "->" << (int)bcm2835_spi_transfer(m_led_data[j])<< " ";
      //bcm2835_spi_transfer(m_led_data[j]);
    }
    //std::cout << std::endl;
    //std::cout << m_led_data.size() << std::endl;
    bcm2835_spi_writenb(m_led_data.data(), m_led_data.size());

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