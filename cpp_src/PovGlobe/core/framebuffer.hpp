#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <math.h>
#include <mutex>
#include <assert.h>
#include <chrono>

#include <iostream>

#include <CImg.h>

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    void initialize(uint32_t height, uint32_t width, uint32_t channels = 3);

    void fill(uint8_t v);

    uint8_t& operator() (uint32_t row, uint32_t col, uint32_t channel);
    const uint8_t& operator() (uint32_t row, uint32_t col, uint32_t channel) const;

    uint8_t& operator[](uint32_t idx);
    const uint8_t& operator[](uint32_t idx) const;

    std::mutex& getMutex() const;

    uint8_t* values();
    const uint8_t* values() const;

    uint32_t getSize() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getChannels() const;

private:
    std::vector<uint8_t> m_values;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_channels;

    mutable std::mutex m_accessMutex;
};


inline void Framebuffer::fill(const uint8_t v){
    std::fill(m_values.begin(), m_values.end(), v);
}

inline const uint8_t* Framebuffer::values() const
{
    return m_values.data();
}

inline uint8_t* Framebuffer::values() 
{
    return m_values.data();
}

inline uint32_t Framebuffer::getWidth() const
{
    return m_width;
}

inline uint32_t Framebuffer::getSize() const
{
    return m_values.size();
}


inline uint32_t Framebuffer::getHeight() const
{
    return m_height;
}


inline uint32_t Framebuffer::getChannels() const
{
    return m_channels;
}
