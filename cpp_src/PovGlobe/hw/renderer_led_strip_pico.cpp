#include "renderer_led_strip_pico.hpp"
#include <iostream>
#include "core/globe.hpp"


#include <errno.h>
#include <fcntl.h> 
#include <string.h>
//#include <termios.h>
#include <asm/termios.h>
#include <stropts.h> /* Oddly, for ioctl, because ioctl.h causes include dramas */
#include <unistd.h>

RendererLedStripPico::RendererLedStripPico()
:m_fd(-1)
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
        std::cout << "error " << errno << " opening /dev/serial0: " << strerror (errno) << std::endl;
        return;
    }else{
        std::cout << "Connected to serial device" << std::endl;
    }
    // check this : https://gist.github.com/kennethryerson/f7d1abcf2633b7c03cf0
    
    int baudrate = 3000000;

   // struct termios options;
  	//tcgetattr(m_fd, &options);
  	//options.c_cflag = B3000000 | CS8 | CLOCAL | CREAD;		//<Set baud rate
  	//options.c_iflag = IGNPAR;
  	//options.c_oflag = 0;
  	//options.c_lflag = 0;

    struct termios2 tio;
    if (ioctl(m_fd, TCGETS2, &tio) != 0) {
        std::cout << "error " << errno << " from TCGETS2" << std::endl;
    }
    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= CS8 | CLOCAL | CREAD | BOTHER;
    tio.c_iflag = IGNPAR;
    tio.c_ispeed = baudrate;
    tio.c_ospeed = baudrate;
    /* do other miscellaneous setup options with the flags here */
    if (ioctl(m_fd, TCSETS2, &tio) != 0)
    {
        std::cout << "error " << errno << " from TCSETS2" << std::endl;
    }
    if (ioctl(m_fd, TCGETS2, &tio) != 0) {
        std::cout << "error " << errno << " from TCGETS2" << std::endl;
    }
    std::cout << "Custom baudrate: " << tio.c_ospeed << std::endl;

  	/*if (tcflush(m_fd, TCIFLUSH) != 0)
    {
        std::cout << "error " << errno << " from tcflush" << std::endl;
    }
  	if (tcsetattr(m_fd, TCSANOW, &options) != 0)
    {
        std::cout << "error " << errno << " from tcsetattr" << std::endl;
    }*/
}

void RendererLedStripPico::render(const Framebuffer& framebuffer)
{
    // Only RGB framebuffer supported
    assert(framebuffer.getChannels() == 3U);
    
    if (buf.size() == 0){
      int buff_size = 6 + 1 + framebuffer.getWidth()*framebuffer.getHeight()*framebuffer.getChannels();
  
      buf.resize(buff_size);
      buf[0] = '+';
      buf[1] = '*';
      buf[2] = '+';
      buf[3] = '*';
      buf[4] = '+';
      buf[5] = '*';
      buf[buff_size - 1] = 42;
    }
  
    int buff_idx = 6;  
    for (size_t j = 0; j < framebuffer.getWidth(); j++)
    {
        for(size_t i = 0; i < framebuffer.getHeight(); i++){
          buf[buff_idx++] = led_lut[framebuffer(j, i, 0)/2];
          buf[buff_idx++] = led_lut[framebuffer(j, i, 1)/2];
          buf[buff_idx++] = led_lut[framebuffer(j, i, 2)/2];
        }
    }
    
    int sent = write(m_fd, buf.data(), buf.size());
    if (sent != buf.size()){
      std::cout << "Sending: " << buf.size() << " acutal: " << std::endl;
    }
}
