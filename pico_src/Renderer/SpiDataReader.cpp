#include "SpiDataReader.hpp"

#include <string.h>

SpiDataReader::SpiDataReader()
{
    curr_pixel_buff_index = 0;
    memset(pixel_column_buffer, 0, N_COL_BUFFER_BYTES);   
    
    static uint8_t rxbuf[TEST_SIZE];

     // Enable SPI at 1 MHz and connect to GPIOs
    spi_init(spi_default, SPI_BAUD_RATE);
    spi_set_slave(spi_default, true);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PIN_MOSI, PICO_DEFAULT_SPI_TX_PIN, PIN_SCK, GPIO_FUNC_SPI));
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PIN_CS, "SPI CS"));


    const uint dma_rx = dma_claim_unused_channel(true);
    printf("Configure RX DMA\n");
    // We set the inbound DMA to transfer from the SPI receive FIFO to a memory buffer paced by the SPI RX FIFO DREQ
    // We coinfigure the read address to remain unchanged for each element, but the write
    // address to increment (so data is written throughout the buffer)
    c = dma_channel_get_default_config(dma_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_index(spi_default) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    dma_channel_configure(dma_rx, &c,
                          rxbuf, // write address
                          &spi_get_hw(spi_default)->dr, // read address
                          TEST_SIZE, // element count (each element is of size transfer_data_size)
                          false); // don't start yet

    printf("Starting DMAs...\n");
    // start them exactly simultaneously to avoid races (in extreme cases the FIFO could overflow)
    dma_start_channel_mask((1u << dma_rx);
    printf("Wait for RX complete...\n");
    dma_channel_wait_for_finish_blocking(dma_rx);

    printf("Done. Checking...");
    for (uint i = 0; i < TEST_SIZE; ++i) {
        rxbuf[i];
    }

}

SpiDataReader::~SpiDataReader()
{
    dma_channel_unclaim(dma_rx);
}

SpiDataReader& SpiDataReader::getInstance() {
    static SpiDataReader instance;
    return instance;
}

bool SpiDataReader::checkPreamble(){

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

void SpiDataReader::processUart(LEDController& ledController){
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