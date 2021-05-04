#pragma once


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/sync.h"

#include "hardware/pio.h"
#include "hardware/gpio.h"

#include "constants.hpp"


class LEDController
{
private:
    static PIO pio;
    static uint sm;
    static uint8_t pixel_buffer[2U][N_BUFFER_SIZE];
    static uint8_t render_buffer_idx;
    static mutex_t mutex;

    LEDController();

    void put_start_frame(PIO pio, uint sm);
    void put_end_frame(PIO pio, uint sm);
    void put_rgb888(PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b);
public:
    ~LEDController();

    static LEDController& getInstance();
    static void core1_write_pixels();

    void swapBuffers();
    uint8_t* getRenderBuffer();
    uint8_t* getInputBuffer();

};


inline uint8_t *LEDController::getRenderBuffer()
{
    return pixel_buffer[render_buffer_idx];
}
inline uint8_t *LEDController::getInputBuffer()
{
    return pixel_buffer[(render_buffer_idx + 1) % 2];
}

inline void LEDController::swapBuffers()
{
    mutex_enter_blocking(&mutex);          
    render_buffer_idx = (render_buffer_idx + 1) % 2;
    mutex_exit(&mutex);
}