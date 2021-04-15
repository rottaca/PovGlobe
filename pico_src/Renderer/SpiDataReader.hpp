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
 
#define SPI_BAUD_RATE 3.90625f*1000*1000


#define N_PREAMBLE_BYTES 6
#define PREAMBLE_ODD_BYTE '*'
#define PREAMBLE_EVEN_BYTE '+'
#define N_COL_BUFFER_BYTES (N_PREAMBLE_BYTES + N_BUFFER_SIZE_PER_COLUMN + 2U)

class SpiDataReader
{
private:
    uint8_t pixel_column_buffer[N_COL_BUFFER_BYTES];
    uint32_t curr_pixel_buff_index;
    uint dma_rx;
    
    SpiDataReader();
    bool checkPreamble();
public:
    ~SpiDataReader();

    static SpiDataReader& getInstance();
    //static void on_uart_rx();

    void processData(LEDController& ledController);
};
