#pragma once


#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/irq.h"
#include "hardware/dma.h"
#include "hardware/spi.h"

#include "LEDController.hpp"

class SpiDataReader
{
private:    
    SpiDataReader();
    bool checkPreamble();
public:
    ~SpiDataReader();

    static SpiDataReader& getInstance();

    void syncWithMaster();
    void processData(LEDController& ledController);
};
