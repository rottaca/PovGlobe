#include "UartDataReader.hpp"

#include <string.h>

UartDataReader::UartDataReader()
{
    m_curr_pixel_buff_index = 0;
    m_preamble_found = false;
    memset(m_pixel_column_buffer, 0, N_VERTICAL_RESOLUTION*N_CHANNELS_PER_PIXEL);

    // Set up our UART with a basic baud rate.
    int actual = uart_init(UART_ID, BAUD_RATE);
    printf("Uart baud is %d..\n", actual);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);
    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    uart_set_translate_crlf(UART_ID, false);
    // Turn off FIFO's - we want to do this character by character
    //uart_set_fifo_enabled(UART_ID, false); 
    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    //int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    // Now enable the UART to send interrupts - RX only
    //uart_set_irq_enables(UART_ID, true, false);

    // And set up and enable the interrupt handlers
    //irq_set_exclusive_handler(UART_IRQ, &UartDataReader::on_uart_rx);
    //irq_set_enabled(UART_IRQ, true);
}

UartDataReader::~UartDataReader()
{
}

UartDataReader& UartDataReader::getInstance() {
    static UartDataReader instance;
    return instance;
}

bool UartDataReader::checkPreamble(){
    // Check for preamble
    m_preamble_found = true;
    if (m_curr_pixel_buff_index >= N_PREAMBLE_BYTES){
        for (size_t i = 0; i < N_PREAMBLE_BYTES && m_preamble_found; i++)
        {
            const size_t idx = m_curr_pixel_buff_index - N_PREAMBLE_BYTES + i;
            if (i % 2 == 0)
                m_preamble_found &= m_pixel_column_buffer[idx] == PREAMBLE_EVEN_BYTE;
            else
                m_preamble_found &= m_pixel_column_buffer[idx] == PREAMBLE_ODD_BYTE;
        }
    }else{
        m_preamble_found = false;
    }

    return m_preamble_found;
}

void UartDataReader::processUart(LEDController& ledController){
    uint8_t* const pixelBuffer = ledController.getPixelBuffer();
    critical_section_t* crit = ledController.getCriticalSection();

    int c = uart_getc(UART_ID);
    while(c != PICO_ERROR_TIMEOUT) {
        m_pixel_column_buffer[m_curr_pixel_buff_index++] = (uint8_t)c;

        if (checkPreamble()) {
            m_curr_pixel_buff_index = N_PREAMBLE_BYTES;
        }
        else {
            // Column completely received
            if (m_curr_pixel_buff_index == N_UART_BUFFER_BYTES) {
                if (c == 42) {
                    critical_section_enter_blocking(crit);
                    memcpy(pixelBuffer, m_pixel_column_buffer + N_PREAMBLE_BYTES, N_BUFFER_SIZE);
                    critical_section_exit(crit);
                }
                else {
                    printf("Invalid data (last: %d, total %u bytes)! Restarting\n", (int)c, m_curr_pixel_buff_index);
                }
                m_curr_pixel_buff_index = 0U;
                m_preamble_found = false;
            }
        }
        c = uart_getc(UART_ID);
    }
}