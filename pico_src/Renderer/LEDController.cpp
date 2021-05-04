#include "LEDController.hpp"

#include "pico/time.h"

#include "apa102.pio.h"
#include "RTTMeasure.hpp"

PIO LEDController::pio{};
uint LEDController::sm = 0;
uint8_t LEDController::pixel_buffer[2][N_BUFFER_SIZE];
mutex_t LEDController::mutex = {};
uint8_t LEDController::render_buffer_idx = 0;

LEDController::LEDController()
{
    mutex_init(&mutex);
    multicore_launch_core1(&LEDController::core1_write_pixels);
}

LEDController::~LEDController()
{
}

LEDController &LEDController::getInstance()
{
    static LEDController instance;
    return instance;
}

inline void LEDController::put_start_frame(PIO pio, uint sm)
{
    pio_sm_put_blocking(pio, sm, 0u);
}

inline void LEDController::put_end_frame(PIO pio, uint sm)
{
    pio_sm_put_blocking(pio, sm, ~0u);
}

inline void LEDController::put_rgb888(PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b)
{
    pio_sm_put_blocking(pio, sm,
                        0x7 << 29 |                     // magic
                            (BRIGHTNESS & 0x1f) << 24 | // global brightness parameter
                            (uint32_t)b << 16 |
                            (uint32_t)g << 8 |
                            (uint32_t)r << 0);
}

void LEDController::core1_write_pixels()
{
    pio = pio0;
    sm = 0;
    uint offset = pio_add_program(pio, &apa102_mini_program);
    apa102_mini_program_init(pio, sm, offset, LED_SERIAL_FREQ, LED_PIN_CLK, LED_PIN_DIN);

    RTTMeasure &rttMeasure = RTTMeasure::getInstance();
    LEDController &ledController = getInstance();

    uint32_t column = 0;
    uint32_t last_column = 0xFFFFFF;
    while (true)
    {
        if (rttMeasure.rotationDetected())
        {            
            ledController.put_start_frame(pio, sm);

            mutex_enter_blocking(&mutex);
            const uint8_t *render_buff = ledController.getRenderBuffer();
            const uint32_t column = rttMeasure.getCurrentColumn();
            {
                const uint8_t *pixel_buffer_column = render_buff + column * N_VERTICAL_RESOLUTION * N_CHANNELS_PER_PIXEL;
                for (int i = 0; i < N_VERTICAL_RESOLUTION * N_CHANNELS_PER_PIXEL; i += N_CHANNELS_PER_PIXEL)
                {
                    ledController.put_rgb888(pio, sm,
                                                pixel_buffer_column[i],
                                                pixel_buffer_column[i + 1],
                                                pixel_buffer_column[i + 2]);
                }
            }
            {
                // Double sided globe
                const uint32_t opposite_column = (column + N_HORIZONTAL_RESOLUTION / 2) % N_HORIZONTAL_RESOLUTION;
                const uint8_t *pixel_buffer_opposite_column = render_buff + opposite_column * N_VERTICAL_RESOLUTION * N_CHANNELS_PER_PIXEL;
                for (int i = (N_VERTICAL_RESOLUTION - 1) *N_CHANNELS_PER_PIXEL; i >= 0; i -= N_CHANNELS_PER_PIXEL)
                {
                    ledController.put_rgb888(pio, sm,
                                                pixel_buffer_opposite_column[i ],
                                                pixel_buffer_opposite_column[i + 1],
                                                pixel_buffer_opposite_column[i + 2]);
                }
            }
            mutex_exit(&mutex);

            ledController.put_end_frame(pio, sm);
        }
    }
}