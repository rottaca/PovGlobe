#include "helper.hpp"

#include <iostream>


LoopTimer::LoopTimer(std::string tag)
    : m_avgLoopTime(-1)
    , m_last_time_print(std::chrono::steady_clock::now())
    , m_last_time(std::chrono::steady_clock::now())
    , m_tag(tag)
{
}

LoopTimer::~LoopTimer()
{
}

std::chrono::duration<float, std::milli> LoopTimer::loopDone()
{
    auto elapsed_time = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> delta = elapsed_time - m_last_time;
    std::chrono::duration<float, std::milli> delta_print = elapsed_time - m_last_time_print;

    if (m_avgLoopTime < 0) {
        m_avgLoopTime = delta.count();
    }
    else {
        m_last_time = elapsed_time;
        m_avgLoopTime = 0.9f * m_avgLoopTime + 0.1f * delta.count();
    }

    if (delta_print.count() > 1000) {
        m_last_time_print = elapsed_time;
        std::cout << "[" << m_tag << "]: Loop Time is " << delta.count() << " ms (avg " << m_avgLoopTime << " ms)." << std::endl;
    }

    return delta;
}


Framebuffer::Framebuffer()
    : m_width(0)
    , m_height(0)
    , m_channels(0)
{
}


Framebuffer::~Framebuffer()
{
}

void Framebuffer::initialize(uint32_t height, uint32_t width, uint32_t channels)
{
    assert(width > 0);
    assert(height > 0);
    assert(channels > 0);
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_values.resize(m_width * m_height * m_channels);
    std::fill(m_values.begin(), m_values.end(), 0);
}
