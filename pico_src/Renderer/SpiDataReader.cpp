#include "SpiDataReader.hpp"

#include <string.h>

#include "pico/binary_info.h"


SpiDataReader::SpiDataReader()
{
    curr_pixel_buff_index = 0;
    memset(pixel_column_buffer, 0, N_COL_BUFFER_BYTES);   
    
     // Enable SPI at 1 MHz and connect to GPIOs
    spi_init(spi0, SPI_BAUD_RATE);
    spi_set_slave(spi0, true);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PIN_MISO, PIN_MOSI, PIN_SCK, GPIO_FUNC_SPI));
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PIN_CS, "SPI CS"));


    /*dma_rx = dma_claim_unused_channel(true);
    printf("Configure RX DMA\n");
    // We set the inbound DMA to transfer from the SPI receive FIFO to a memory buffer paced by the SPI RX FIFO DREQ
    // We coinfigure the read address to remain unchanged for each element, but the write
    // address to increment (so data is written throughout the buffer)
    dma_channel_config c = dma_channel_get_default_config(dma_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_index(spi0) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    dma_channel_configure(dma_rx, &c,
                          pixel_column_buffer, // write address
                          &spi_get_hw(spi0)->dr, // read address
                          N_COL_BUFFER_BYTES, // element count (each element is of size transfer_data_size)
                          false); // don't start yet

    printf("Starting DMAs...\n");
    // start them exactly simultaneously to avoid races (in extreme cases the FIFO could overflow)
    dma_start_channel_mask((1u << dma_rx));
    printf("Wait for RX complete...\n");
    dma_channel_wait_for_finish_blocking(dma_rx);

    printf("Done. Checking...");
    for (uint i = 0; i < N_COL_BUFFER_BYTES; ++i) {
        pixel_column_buffer[i];
    }*/
    
    while(spi_is_readable(spi0)){
      printf("Clear buffer by reading\n");
      spi_read_blocking(spi0, 0, pixel_column_buffer, 1);
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

void SpiDataReader::processData(LEDController& ledController){
    if(spi_is_readable(spi0)){
      printf("start reading...\n");
      spi_read_blocking(spi0, 42, pixel_column_buffer, N_COL_BUFFER_BYTES);
      for(int i = 0; i < N_COL_BUFFER_BYTES; i++)
        printf("%d", pixel_column_buffer[i]);
      printf("\n");
    }else{
      printf("Nothing to read!\n");
    }
}