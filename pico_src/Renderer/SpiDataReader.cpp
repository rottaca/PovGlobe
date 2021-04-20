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
#define SPI_DATA_JUNK_SIZE 4096

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
    printf("Done.\n");

}

void SpiDataReader::processData(LEDController &ledController)
{
    uint8_t *input_buffer = ledController.getInputBuffer();
    size_t bytes_read = 0;
    size_t junk_nr = 0;
    while (bytes_read < N_BUFFER_SIZE)
    {
        size_t junk_size = SPI_DATA_JUNK_SIZE;

        if (bytes_read + SPI_DATA_JUNK_SIZE > N_BUFFER_SIZE)
        {
            junk_size -= (bytes_read + SPI_DATA_JUNK_SIZE) - N_BUFFER_SIZE;
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