#include "LEDController.hpp"

#include "apa102.pio.h"

PIO LEDController::pio{};
uint LEDController::sm = 0;
uint8_t LEDController::pixel_buffer[N_BUFFER_SIZE];

LEDController::LEDController()
{
    multicore_launch_core1(&LEDController::core1_write_pixels);   
}

LEDController::~LEDController()
{
}

LEDController& LEDController::getInstance() {
    static LEDController instance;
    return instance;
}
uint8_t* LEDController::getPixelBuffer(){
    return pixel_buffer;
}

void LEDController::put_start_frame(PIO pio, uint sm) {
    pio_sm_put_blocking(pio, sm, 0u);
}

void LEDController::put_end_frame(PIO pio, uint sm) {
    pio_sm_put_blocking(pio, sm, ~0u);
}

void LEDController::put_rgb888(PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b) {
    pio_sm_put_blocking(pio, sm,
                        0x7 << 29 |                   // magic
                        (BRIGHTNESS & 0x1f) << 24 |   // global brightness parameter
                        (uint32_t) b << 16 |
                        (uint32_t) g << 8 |
                        (uint32_t) r << 0
    );
}

void LEDController::core1_write_pixels(){
    pio = pio0;
    sm = 0;
    uint offset = pio_add_program(pio, &apa102_mini_program);
    apa102_mini_program_init(pio, sm, offset, SERIAL_FREQ, PIN_CLK, PIN_DIN);

    LEDController& ledController = getInstance();
    while (true) {
        const uint32_t column = multicore_fifo_pop_blocking();
        ledController.put_start_frame(pio, sm);
        for (int i = 0; i < N_VERTICAL_RESOLUTION*N_CHANNELS_PER_PIXEL; i+=N_CHANNELS_PER_PIXEL) {
            ledController.put_rgb888(pio, sm,
                    pixel_buffer[i + column*N_CHANNELS_PER_PIXEL],
                    pixel_buffer[i+1 + column*N_CHANNELS_PER_PIXEL],
                    pixel_buffer[i+2 + column*N_CHANNELS_PER_PIXEL]
            );
        }
        // Double sided globe
        const uint32_t opposite_column = (column + N_HORIZONTAL_RESOLUTION/2) % N_HORIZONTAL_RESOLUTION;
        for (int i = N_VERTICAL_RESOLUTION*N_CHANNELS_PER_PIXEL-1; i >=0; i-=N_CHANNELS_PER_PIXEL) {
            ledController.put_rgb888(pio, sm,
                    pixel_buffer[i + opposite_column*N_CHANNELS_PER_PIXEL],
                    pixel_buffer[i+1 + opposite_column*N_CHANNELS_PER_PIXEL],
                    pixel_buffer[i+2 + opposite_column*N_CHANNELS_PER_PIXEL]
            );
        }
        ledController.put_end_frame(pio, sm);
    }
}