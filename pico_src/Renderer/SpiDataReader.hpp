#pragma once


#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/spi.h"

#include "LEDController.hpp"


#define PIN_CS 17U
#define PIN_SCK 18U
#define PIN_MOSI 16U 
#define PIN_MISO 19U  
 
#define SPI_BAUD_RATE 1.953125f*1000*1000

class SpiDataReader
{
private:
    uint8_t pixel_column_buffer[N_BUFFER_SIZE + 1];
    
    SpiDataReader();
    bool checkPreamble();
public:
    ~SpiDataReader();

    static SpiDataReader& getInstance();
    //static void on_uart_rx();

    void processData(LEDController& ledController);
};
