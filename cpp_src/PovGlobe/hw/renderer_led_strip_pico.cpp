#include "renderer_led_strip_pico.hpp"
#include <iostream>
#include "core/globe.hpp"

RendererLedStripPico::RendererLedStripPico(const std::string portname)
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
    m_fd = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (m_fd < 0)
    {
        std::cout << "error " << errno << " opening " << m_portname << ": " << strerror (errno) << std::endl;
        return;
    }else{
        std::cout << "Connected to serial device" << std::endl;
    }

    struct termios options;
  	tcgetattr(m_fd, &options);
  	options.c_cflag = B3000000 | CS8 | CLOCAL | CREAD;		//<Set baud rate
  	options.c_iflag = IGNPAR;
  	options.c_oflag = 0;
  	options.c_lflag = 0;
  	if (tcflush(m_fd, TCIFLUSH) != 0)
    {
        std::cout << "error " << errno << " from tcflush" << std::endl;
    }
  	if (tcsetattr(m_fd, TCSANOW, &options) != 0)
    {
        std::cout << "error " << errno << " from tcsetattr" << std::endl;
    }
}

void RendererLedStripPico::render(const Framebuffer& framebuffer)
{
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3U);
    
    int buff_size = 6 + 2 + framebuffer.getHeight()*framebuffer.getChannels();

    uint8_t buf[512];
    buf[0] = '+';
    buf[1] = '*';
    buf[2] = '+';
    buf[3] = '*';
    buf[4] = '+';
    buf[5] = '*';
    buf[6] = 0;
    buf[buff_size - 1] = '\n';
  
    for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
        //std::cout << "Write preamble for row " << j << std::endl;
        //std::cout << buff_size << std::endl;
        buf[6] = j;
        int buff_idx = 7;
        for(size_t i = 0; i < framebuffer.getHeight(); i++){
          buf[buff_idx++] = led_lut[framebuffer(j, i, 0)/2];
          buf[buff_idx++] = led_lut[framebuffer(j, i, 1)/2];
          buf[buff_idx++] = led_lut[framebuffer(j, i, 2)/2];
        }
        //memcpy(&buf[7], &framebuffer(j, 0, 0), framebuffer.getHeight()*framebuffer.getChannels());
        int sent = write(m_fd, buf, buff_size);
        
        if (sent != buff_size){
          std::cout << "Sending: " << buff_size << " acutal: " << std::endl;
        }
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