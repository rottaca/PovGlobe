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
 
#define SPI_BAUD_RATE 15.625f*1000*1000
 
#define SPI_BUFF_SIZE (N_BUFFER_SIZE + 1)
#define SPI_BUFF_LAST_IDX (SPI_BUFF_SIZE - 1)
#define SPI_END_BYTE 42
class SpiDataReader
{
private:
    uint8_t pixel_column_buffer[SPI_BUFF_SIZE];
    
    SpiDataReader();
    bool checkPreamble();
public:
    ~SpiDataReader();

    static SpiDataReader& getInstance();

    void processData(LEDController& ledController);
};
