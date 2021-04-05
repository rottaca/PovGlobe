/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>
#include <cstring>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"

#include "hardware/gpio.h"

#include "LEDController.hpp"
#include "UartDataReader.hpp"


#define N_INIT_FRAMES 10
void showNextInitFrame(int frameNumber, uint8_t * pixels){
    printf("Init frame %d\n", frameNumber);
    for (size_t i = 0; i < N_VERTICAL_RESOLUTION*N_CHANNELS_PER_PIXEL; i+=N_CHANNELS_PER_PIXEL)
    {
        pixels[i] = 128;
        pixels[i+1] = 0;
        pixels[i+2] = 0;
    }
}

int main() {
    stdio_init_all();
    UartDataReader& reader = UartDataReader::getInstance();
    LEDController& ledController = LEDController::getInstance();

    uint8_t * pixels = ledController.getPixelBuffer();
    memset(pixels,0, N_BUFFER_SIZE);

    for (size_t i = 0; i < N_INIT_FRAMES; i++)
    {
        showNextInitFrame(i, pixels);
        sleep_ms(1000);
    }
    
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    while(true){
        reader.processUart(ledController);
    }
}
