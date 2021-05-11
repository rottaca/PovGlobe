#pragma once


#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"

#include "LEDController.hpp"

class SpiDataReader
{
private:    
    SpiDataReader();
    bool checkPreamble();

    uint dma_tx;
    uint dma_rx;

    dma_channel_config dma_c_tx;
    dma_channel_config dma_c_rx;
public:
    ~SpiDataReader();

    static SpiDataReader& getInstance();

    void syncWithMaster();
    void processData(LEDController& ledController);
};
