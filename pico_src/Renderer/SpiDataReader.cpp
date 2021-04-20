#include "SpiDataReader.hpp"

#include <string.h>

#include "pico/binary_info.h"

#define SPI_DEV spi0

#define PIN_CS 17U
#define PIN_SCK 18U
#define PIN_MOSI 16U 
#define PIN_MISO 19U  
 
#define SPI_BAUD_RATE 1.953125*1000*1000
 
#define SPI_MASTER_END_BYTE 42
#define SPI_SLAVE_END_BYTE 255

SpiDataReader::SpiDataReader()
{
    // Enable SPI at 1 MHz and connect to GPIOs
    spi_init(SPI_DEV, SPI_BAUD_RATE);
    uint baudrate = spi_set_baudrate(SPI_DEV, SPI_BAUD_RATE);
    spi_set_slave(SPI_DEV, true);
    spi_set_format(SPI_DEV, 8U, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PIN_MOSI, PIN_MISO, PIN_SCK, PIN_CS, GPIO_FUNC_SPI));

    /*dma_rx = dma_claim_unused_channel(true);
    printf("Configure RX DMA\n");
    // We set the inbound DMA to transfer from the SPI receive FIFO to a memory buffer paced by the SPI RX FIFO DREQ
    // We coinfigure the read address to remain unchanged for each element, but the write
    // address to increment (so data is written throughout the buffer)
    dma_channel_config c = dma_channel_get_default_config(dma_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, spi_get_index(SPI_DEV) ? DREQ_SPI1_RX : DREQ_SPI_DEV_RX);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    dma_channel_configure(dma_rx, &c,
                          pixel_column_buffer, // write address
                          &spi_get_hw(SPI_DEV)->dr, // read address
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

    printf("SPI interface initialized!\n");
    printf("--------------\n");
    printf("Baudrate used: %u Hz\n", baudrate);
    printf("Expected Bytes per Transmission: %d\n", N_BUFFER_SIZE + 1);
    printf("--------------\n");

    syncWithMaster();
}

SpiDataReader::~SpiDataReader()
{
    //dma_channel_unclaim(dma_rx);
}

SpiDataReader &SpiDataReader::getInstance()
{
    static SpiDataReader instance;
    return instance;
}

void SpiDataReader::syncWithMaster(){
    uint8_t values[100];
    for(int i = 0; i < 100; i++){
        values[i] = i;
    }
    printf("Waiting for master...\n");
    spi_write_read_blocking(SPI_DEV, values, values, 100);

}

void SpiDataReader::processData(LEDController &ledController)
{
    uint8_t *input_buffer = ledController.getInputBuffer();
    size_t bytes_read = 0;
    size_t default_junk_size = 2048;
    size_t junk_nr = 0;
    while (bytes_read < N_BUFFER_SIZE)
    {
        size_t junk_size = default_junk_size;

        if (bytes_read + default_junk_size > N_BUFFER_SIZE)
        {
            junk_size -= (bytes_read + default_junk_size) - N_BUFFER_SIZE;
        }
        spi_read_blocking(SPI_DEV, junk_nr, input_buffer + bytes_read, junk_size);
        bytes_read += junk_size;
        junk_nr++;
    }
    uint8_t end_value;
    spi_read_blocking(SPI_DEV, SPI_SLAVE_END_BYTE, &end_value, 1);

    if (end_value == SPI_MASTER_END_BYTE)
    {
        ledController.swapBuffers();
    }
    else
    {
        printf("Invalid data %d\n", (int)end_value);
    }

    //syncWithMaster();
}