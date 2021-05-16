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

    ledController.put_start_frame(pio, sm);
    for (int i = 0; i < 2*N_BUFFER_SIZE_PER_COLUMN; i += N_CHANNELS_PER_PIXEL)
    {
        ledController.put_rgb888(pio, sm,
                                    0,  //render_buff[column * N_BUFFER_SIZE_PER_COLUMN + i],
                                    0,//render_buff[column * N_BUFFER_SIZE_PER_COLUMN + i + 1],
                                    0//render_buff[column * N_BUFFER_SIZE_PER_COLUMN + i + 2]
                                );
    }
    ledController.put_end_frame(pio, sm);
    
    uint32_t last_column = 0xFFFFFF;
    uint32_t column, opposite_column;
    while (true)
    {
        const absolute_time_t curr_time = get_absolute_time();
        rttMeasure.getCurrentColumn(curr_time, column, opposite_column);  
        if (column != last_column){
            mutex_enter_blocking(&mutex);
            const uint8_t *render_buff = ledController.getRenderBuffer();

            ledController.put_start_frame(pio, sm);
            for (int i = 0; i < N_BUFFER_SIZE_PER_COLUMN; i += N_CHANNELS_PER_PIXEL)
            {
                ledController.put_rgb888(pio, sm,
                                            render_buff[column * N_BUFFER_SIZE_PER_COLUMN + i],
                                            render_buff[column * N_BUFFER_SIZE_PER_COLUMN + i + 1],
                                            render_buff[column * N_BUFFER_SIZE_PER_COLUMN + i + 2]
                                        );
            }
            for (int i = (N_VERTICAL_RESOLUTION - 1) *N_CHANNELS_PER_PIXEL; i >= 0; i -= N_CHANNELS_PER_PIXEL)
            {
                ledController.put_rgb888(pio, sm,
                                            render_buff[opposite_column * N_BUFFER_SIZE_PER_COLUMN+i ],
                                            render_buff[opposite_column * N_BUFFER_SIZE_PER_COLUMN+i + 1],
                                            render_buff[opposite_column * N_BUFFER_SIZE_PER_COLUMN+i + 2]
                                        );
            }
            ledController.put_end_frame(pio, sm);
            mutex_exit(&mutex);
            
            last_column = column;
            sleep_us(100);
        }
        else if (SHOW_DOT_MATRIX_STYLE) {
            ledController.put_start_frame(pio, sm);
            for (int i = 0; i < 2*N_BUFFER_SIZE_PER_COLUMN; i += N_CHANNELS_PER_PIXEL)
            {
                ledController.put_rgb888(pio, sm, 0, 0, 0);
            }
            ledController.put_end_frame(pio, sm);
        }
    }
}