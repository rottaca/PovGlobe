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


class UartDataReader
{
private:
    uint8_t pixel_column_buffer[N_BUFFER_SIZE_PER_COLUMN];
    uint32_t next_pixel_buff_value;
    uint32_t current_input_column;

    UartDataReader();
public:
    ~UartDataReader();

    static UartDataReader& getInstance();
    //static void on_uart_rx();

    void processUart(LEDController& ledController);
};
