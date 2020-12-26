#pragma once

#include <vector>
#include <mutex>
#include <assert.h>

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void initialize(int height, int width, int channels = 3);

	uint8_t & operator() (int row, int col, int channel);

	std::mutex& getMutex() const;

	int getWidth() const;
	int getHeight() const;
	int getChannels() const;

private:
	std::vector<uint8_t> m_values;
	int m_width;
	int m_height;
	int m_channels;

	mutable std::mutex m_accessMutex;
};


inline Framebuffer::Framebuffer()
	: m_width(0)
	, m_height(0)
	, m_channels(0)
{
}


inline Framebuffer::~Framebuffer()
{
}


inline void Framebuffer::initialize(int height, int width, int channels)
{
	if (width > 0 && height > 0 && channels > 0) {
		m_width = width;
		m_height = height;
		m_channels = channels;
		m_values.resize(m_width * m_height * m_channels);
	}
	else {
		assert(false && "Invalid matrix dimension specified!");
		m_width = 0;
		m_height = 0;
		m_channels = 0;
	}
}


inline uint8_t& Framebuffer::operator()(int row, int col, int channel)
{
	// TODO assert
	uint32_t idx = row * (m_width * m_channels) + col * m_channels + channel;
	return m_values[idx];
}


inline std::mutex& Framebuffer::getMutex() const {
	return m_accessMutex;
}


inline int Framebuffer::getWidth() const
{
	return m_width;
}


inline int Framebuffer::getHeight() const
{
	return m_height;
}


inline int Framebuffer::getChannels() const
{
	return m_channels;
}

