#pragma once


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/pio.h"
#include "hardware/gpio.h"


#define PIN_CLK 8
#define PIN_DIN 9

#define N_VERTICAL_RESOLUTION 55
#define N_HORIZONTAL_RESOLUTION (2*N_VERTICAL_RESOLUTION) 

#define N_CHANNELS_PER_PIXEL 3
#define N_LEDS (2*N_VERTICAL_RESOLUTION)
#define N_PIXEL (N_VERTICAL_RESOLUTION*N_HORIZONTAL_RESOLUTION)

#define N_BUFFER_SIZE_PER_COLUMN (N_VERTICAL_RESOLUTION*N_CHANNELS_PER_PIXEL)
#define N_BUFFER_SIZE (N_BUFFER_SIZE_PER_COLUMN*N_HORIZONTAL_RESOLUTION)

#define SERIAL_FREQ (15 * 1000 * 1000)

// Global brightness value 0->31
#define BRIGHTNESS 31

class LEDController
{
private:
    static PIO pio;
    static uint sm;
    static uint8_t pixel_buffer[N_BUFFER_SIZE];

    LEDController();

    void put_start_frame(PIO pio, uint sm);
    void put_end_frame(PIO pio, uint sm);
    void put_rgb888(PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b);
public:
    ~LEDController();

    static LEDController& getInstance();
    static void core1_write_pixels();

    uint8_t* getPixelBuffer();

};
