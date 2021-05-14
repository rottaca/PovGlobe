#pragma once

#define LED_PIN_CLK 8
#define LED_PIN_DIN 9
#define LED_SERIAL_FREQ (19 * 1000 * 1000)
// Global brightness value 0-31
// Limit to 50% as we don't have enough power
#define BRIGHTNESS 10

#define N_VERTICAL_RESOLUTION 55
#define N_HORIZONTAL_RESOLUTION 120
#define N_CHANNELS_PER_PIXEL 3

#define PIN_HALL_SENSOR 5
#define N_MAGNETS 8

#define SPI_DEV spi0
#define SPI_PIN_CS 17U
#define SPI_PIN_SCK 18U
#define SPI_PIN_MOSI 16U 
#define SPI_PIN_MISO 19U  
#define SPI_BAUD_RATE 1.953125*1000*1000
#define SPI_MASTER_END_BYTE 42
#define SPI_SLAVE_END_BYTE 255 
#define SPI_COM_START_BYTE 48
#define SPI_DATA_JUNK_SIZE 8192


// Computed parameters based on the previously set values
#define N_PIXEL (N_VERTICAL_RESOLUTION*N_HORIZONTAL_RESOLUTION)
#define N_BUFFER_SIZE_PER_COLUMN (N_VERTICAL_RESOLUTION*N_CHANNELS_PER_PIXEL)
#define N_BUFFER_SIZE (N_PIXEL*N_CHANNELS_PER_PIXEL)
