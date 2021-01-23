#include "framebuffer.hpp"
#include <iostream>

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


uint8_t& Framebuffer::operator()(uint32_t col, uint32_t row, uint32_t channel)
{
    assert(row < m_height);
    assert(col < m_width);
    assert(channel < m_channels);
    const uint32_t idx = col * (m_height * m_channels) + row * m_channels + channel;
    return m_values[idx];
}

const uint8_t& Framebuffer::operator()(uint32_t col, uint32_t row, uint32_t channel) const
{
    assert(row < m_height);
    assert(col < m_width);
    assert(channel < m_channels);
    const uint32_t idx = col * (m_height * m_channels) + row * m_channels + channel;
    return m_values[idx];
}

const uint8_t& Framebuffer::operator[](uint32_t idx) const {
    assert(idx < m_values.size());
    return m_values[idx];
}
uint8_t& Framebuffer::operator[](uint32_t idx) {
    assert(idx < m_values.size());
    return m_values[idx];
}

std::mutex& Framebuffer::getMutex() const {
    return m_accessMutex;
}

const uint8_t* Framebuffer::values() const
{
    return m_values.data();
}

uint8_t* Framebuffer::values()
{
    return m_values.data();
}

uint32_t Framebuffer::getWidth() const
{
    return m_width;
}

uint32_t Framebuffer::getSize() const
{
    return m_values.size();
}


uint32_t Framebuffer::getHeight() const
{
    return m_height;
}


uint32_t Framebuffer::getChannels() const
{
    return m_channels;
}
