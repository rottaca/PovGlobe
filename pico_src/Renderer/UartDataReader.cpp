#include "UartDataReader.hpp"

#include <string.h>

UartDataReader::UartDataReader()
{
    curr_pixel_buff_index = 0;
    memset(pixel_column_buffer, 0, N_VERTICAL_RESOLUTION*N_CHANNELS_PER_PIXEL);

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

bool UartDataReader::checkPreamble(){

    // Check for preamble
    bool preamble_found = true;
    if (curr_pixel_buff_index >= N_PREAMBLE_BYTES-1){
        for (size_t i = 0; i < N_PREAMBLE_BYTES && preamble_found; i++)
        {
            const size_t idx = curr_pixel_buff_index - N_PREAMBLE_BYTES + 1 + i;
            if (i % 2 == 0)
                preamble_found &= pixel_column_buffer[idx] == PREAMBLE_EVEN_BYTE;
            else
                preamble_found &= pixel_column_buffer[idx] == PREAMBLE_ODD_BYTE;
        }
    }else{
        preamble_found = false;
    }

    return preamble_found;
}

void UartDataReader::processUart(LEDController& ledController){
    int c = getchar_timeout_us(0U);
    int max_chars_to_read = 100U;
    int chars_read = 0U;

    while(c != PICO_ERROR_TIMEOUT) {
        chars_read++;

        pixel_column_buffer[curr_pixel_buff_index] = (uint8_t)c;
        //printf("Received %c\n", c);
        if (checkPreamble()){
            //printf("New preamble detected. Restarting.\n");
            curr_pixel_buff_index = N_PREAMBLE_BYTES;
        }else{
            curr_pixel_buff_index++;

            // Column completely received
            if (curr_pixel_buff_index == N_COL_BUFFER_BYTES){
                if(c == '\n' || c == '\r'){
                    // Last byte is column 
                    // TODO 2 bytes
                    uint32_t current_input_column = pixel_column_buffer[N_PREAMBLE_BYTES];
                    
                    if (current_input_column < N_HORIZONTAL_RESOLUTION){
                        //printf("Updating pixel buffer for column %d\n", current_input_column);

                        uint8_t* const pixelBuffer = ledController.getPixelBuffer();
                        uint8_t* const curr_column_output_buffer = pixelBuffer + current_input_column*N_BUFFER_SIZE_PER_COLUMN;
                        uint8_t* const curr_column_input_buffer = pixel_column_buffer+N_PREAMBLE_BYTES+1;
                        memcpy(curr_column_output_buffer, curr_column_input_buffer, N_BUFFER_SIZE_PER_COLUMN);
                    }else{
                        printf("Error, invalid column recieved: %d\n", current_input_column);
                    }
                }else{
                    printf("Error, recieved invalid data (last: %d, total %u bytes)! Restarting\n", (int)c, curr_pixel_buff_index);
                }
                curr_pixel_buff_index = 0U;
            }
        }

        if(chars_read == max_chars_to_read)
            break;

        c = getchar_timeout_us(0U);
    }
}