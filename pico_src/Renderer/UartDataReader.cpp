#include "UartDataReader.hpp"

#include <string.h>

UartDataReader::UartDataReader()
{
    next_pixel_buff_value = 0;
    current_input_column = 0;
    memset(pixel_column_buffer, 0, N_VERTICAL_RESOLUTION*N_PIXELS_PER_CHANNEL);

    // // Set up our UART with a basic baud rate.
    // int actual = uart_init(UART_ID, BAUD_RATE);
    // // Set the TX and RX pins by using the function select on the GPIO
    // // Set datasheet for more information on function select
    // gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    // gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    // // Set UART flow control CTS/RTS, we don't want these, so turn them off
    // uart_set_hw_flow(UART_ID, false, false);
    // // Set our data format
    // uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);
    // // Turn off FIFO's - we want to do this character by character
    // uart_set_fifo_enabled(UART_ID, false); 
    // // Set up a RX interrupt
    // // We need to set up the handler first
    // // Select correct interrupt for the UART we are using
    // int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    // // Now enable the UART to send interrupts - RX only
    // uart_set_irq_enables(UART_ID, true, false);

    // // And set up and enable the interrupt handlers
    // irq_set_exclusive_handler(UART_IRQ, &UartDataReader::on_uart_rx);
    // irq_set_enabled(UART_IRQ, true);
}

UartDataReader::~UartDataReader()
{
}

UartDataReader& UartDataReader::getInstance() {
    static UartDataReader instance;
    return instance;
}

// void UartDataReader::on_uart_rx() {
//     while (uart_is_readable(UART_ID)) {
//         uint8_t ch = uart_getc(UART_ID);
//         // Can we send it back?
//         if (uart_is_writable(UART_ID)) {
//             // Change it slightly first!
//             ch++;
//             uart_putc(UART_ID, ch);
//         }
//     }
// }

void UartDataReader::processUart(LEDController& ledController){

    current_input_column = 0;
    int c = getchar_timeout_us(0U);
    int max_chars_to_read = 100U;
    int chars_read = 0U;

    while(c != PICO_ERROR_TIMEOUT) {
        pixel_column_buffer[next_pixel_buff_value++] = (uint8_t)c;
        chars_read++;

        // Column completely received
        if (next_pixel_buff_value == N_BUFFER_SIZE_PER_COLUMN){
            printf("Received %d values.\n", next_pixel_buff_value);
            printf("Updating pixel buffer for column %d\n", current_input_column);

            uint8_t* const pixelBuffer = ledController.getPixelBuffer();
            uint8_t* const curr_column_output_buffer = pixelBuffer + current_input_column*N_BUFFER_SIZE_PER_COLUMN;
            memcpy(pixel_column_buffer, curr_column_output_buffer, N_BUFFER_SIZE_PER_COLUMN);
            next_pixel_buff_value = 0U;

            printf("Done pixel update.\n", current_input_column);
        }

        if(chars_read == max_chars_to_read)
            break;

        c = getchar_timeout_us(0U);
    }
}