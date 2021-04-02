#pragma once


#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/uart.h"
#include "hardware/irq.h"

#include "LEDController.hpp"


#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE
#define UART_TX_PIN 16
#define UART_RX_PIN 17


#define N_PREAMBLE_BYTES 6
#define PREAMBLE_ODD_BYTE '*'
#define PREAMBLE_EVEN_BYTE '+'
#define N_COL_BUFFER_BYTES (N_PREAMBLE_BYTES + N_BUFFER_SIZE_PER_COLUMN + 1U)

class UartDataReader
{
private:
    uint8_t pixel_column_buffer[N_COL_BUFFER_BYTES];
    uint32_t curr_pixel_buff_index;
    uint32_t current_input_column;

    UartDataReader();
    bool checkPreamble();
public:
    ~UartDataReader();

    static UartDataReader& getInstance();
    //static void on_uart_rx();

    void processUart(LEDController& ledController);
};
