#pragma once

#include <chrono>

class Globe;


struct RpmData {
    std::chrono::duration<float, std::milli> cycle_time;
    int curr_temporal_pos;
    bool valid;
};

class RpmMeasureBase
{
public:

    RpmMeasureBase();
    virtual ~RpmMeasureBase();

    virtual void initialize(Globe& globe);
    virtual RpmData getRpmData() = 0;

protected:
    int m_temporal_resolution;

};