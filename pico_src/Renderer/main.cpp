/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"

#include "hardware/gpio.h"

#include "LEDController.hpp"
#include "RTTMeasure.hpp"
#include "UartDataReader.hpp"

int main() {
    stdio_init_all();
    UartDataReader& reader = UartDataReader::getInstance();
    RTTMeasure& rttMeasure = RTTMeasure::getInstance();
    LEDController& ledController = LEDController::getInstance();

    uint8_t * pixels = ledController.getPixelBuffer();

    for (size_t i = 0; i < N_BUFFER_SIZE; i++)
    {
        pixels[i] = 128;
    }
    

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while(true){
        reader.processUart(ledController);

        if (rttMeasure.rotationDetected()){
            const int64_t time_since_hall_sensor_event = rttMeasure.getDeltaTimeSinceLastEvent();
            const int64_t rtt = rttMeasure.getRtt();
            uint32_t curr_column = time_since_hall_sensor_event * (N_HORIZONTAL_RESOLUTION - 1U) / rtt;
            if (curr_column > (N_HORIZONTAL_RESOLUTION - 1U)){
                curr_column = (N_HORIZONTAL_RESOLUTION - 1U);
            }

            multicore_fifo_push_blocking(curr_column);

            printf("-----------\n");
            printf("RTT             : %lld us\n", rtt);
            printf("Time Since Event: %lld us\n", time_since_hall_sensor_event);
            printf("Curr Column     : %lu\n", curr_column);
            printf("-----------\n");
        }else{
            printf("No rotation detected.\n");
        }
        
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
}
