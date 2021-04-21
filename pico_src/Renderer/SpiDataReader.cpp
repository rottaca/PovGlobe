#include "SpiDataReader.hpp"

#include <string.h>

#include "pico/binary_info.h"

#include "constants.hpp"

SpiDataReader::SpiDataReader()
{
    // Enable SPI at 1 MHz and connect to GPIOs
    spi_init(SPI_DEV, SPI_BAUD_RATE);
    uint baudrate = spi_set_baudrate(SPI_DEV, SPI_BAUD_RATE);
    spi_set_slave(SPI_DEV, true);
    spi_set_format(SPI_DEV, 8U, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(SPI_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SPI_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(SPI_PIN_MISO, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(SPI_PIN_MOSI, SPI_PIN_MISO, SPI_PIN_SCK, SPI_PIN_CS, GPIO_FUNC_SPI));

    printf("SPI interface initialized!\n");
    printf("SPI Baudrate: %u Hz\n", baudrate);
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
    uint8_t values[6];
    
    values[0] = SPI_COM_START_BYTE;
    values[1] = N_VERTICAL_RESOLUTION;
    values[2] = N_HORIZONTAL_RESOLUTION;
    values[3] = N_CHANNELS_PER_PIXEL;
    values[4] = (uint8_t)((SPI_DATA_JUNK_SIZE) >> 8) & 0xff;   // high
    values[5] = (uint8_t)(SPI_DATA_JUNK_SIZE & 0xff);          // low

    printf("Syncing with master...\n");
    spi_write_read_blocking(SPI_DEV, values, values, 6);

    if (values[1] != N_VERTICAL_RESOLUTION){
        printf("Vertical resolution does not match.\n");
    }
    if (values[2] != N_HORIZONTAL_RESOLUTION){
        printf("Horizontal resolution does not match.\n");
    }
    if (values[3] != N_CHANNELS_PER_PIXEL){
        printf("Number of channels per pixel does not match.\n");
    }
}

void SpiDataReader::processData(LEDController &ledController)
{
    uint8_t end_value = 0;
    size_t junk_size = 0;

    while(true){
        uint8_t *input_buffer = ledController.getInputBuffer();
        size_t bytes_read = 0;
        size_t junk_nr = 0;
        while (bytes_read < N_BUFFER_SIZE)
        {
            if (bytes_read + SPI_DATA_JUNK_SIZE > N_BUFFER_SIZE)
            {
                junk_size = N_BUFFER_SIZE - bytes_read;
            } else {
                junk_size = SPI_DATA_JUNK_SIZE;
            }
            spi_read_blocking(SPI_DEV, junk_nr, input_buffer + bytes_read, junk_size);
            bytes_read += junk_size;
            junk_nr++;
        }
        
        spi_read_blocking(SPI_DEV, SPI_SLAVE_END_BYTE, &end_value, 1);
        if (end_value == SPI_MASTER_END_BYTE)
        {
            ledController.swapBuffers();
        }
        else
        {
            printf("Invalid data %d\n", (int)end_value);
        }
    }
}