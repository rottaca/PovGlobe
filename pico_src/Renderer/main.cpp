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
#include "RTTMeasure.hpp"
#include "UartDataReader.hpp"


#define N_INIT_FRAMES 10
void showNextInitFrame(int frameNumber, uint8_t * pixels){
    printf("Init frame %d\n", frameNumber);
    for (size_t i = 0; i < N_BUFFER_SIZE; i+=N_CHANNELS_PER_PIXEL)
    {
        if((i+frameNumber) % 4 == 0){
            pixels[i] = 128;
            pixels[i+1] = 0;
            pixels[i+2] = 0;
        }
        else if((i+frameNumber) % 4 == 1){
            pixels[i] = 0;
            pixels[i+1] = 128;
            pixels[i+2] = 0;
        }
        else if((i+frameNumber) % 4 == 2){
            pixels[i] = 0;
            pixels[i+1] = 0;
            pixels[i+2] = 128;
        }
        else if((i+frameNumber) % 4 == 3){
            pixels[i] = 128;
            pixels[i+1] = 128;
            pixels[i+2] = 128;
        }
    }
    multicore_fifo_push_blocking(0U);
}

int main() {
    stdio_init_all();
    UartDataReader& reader = UartDataReader::getInstance();
    RTTMeasure& rttMeasure = RTTMeasure::getInstance();
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

    uint32_t last_column = 0xFFFFFF;
    bool last_cycle_rotation_detected = false;
    while(true){
        reader.processUart(ledController);

        if (rttMeasure.rotationDetected()){
            last_cycle_rotation_detected = false;
            const int64_t time_since_hall_sensor_event = rttMeasure.getDeltaTimeSinceLastEvent();
            const int64_t rtt = rttMeasure.getRtt();
            uint32_t curr_column = time_since_hall_sensor_event * (N_HORIZONTAL_RESOLUTION - 1U) / rtt;
            if (curr_column > (N_HORIZONTAL_RESOLUTION - 1U)){
                curr_column = (N_HORIZONTAL_RESOLUTION - 1U);
            }
            if (last_column != curr_column){
                multicore_fifo_push_blocking(curr_column);
                last_column = curr_column;

                // printf("-----------\n");
                // printf("RTT             : %lld us\n", rtt);
                // printf("Time Since Event: %lld us\n", time_since_hall_sensor_event);
                //printf("Curr Column     : %lu\n", curr_column);
                // printf("-----------\n");
            }
        }else if (!last_cycle_rotation_detected) {
            printf("No rotation detected.\n");
            memset(pixels,0, N_BUFFER_SIZE);
            multicore_fifo_push_blocking(0);
            last_cycle_rotation_detected = true;
        }
    }
}
