#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <math.h>
#include <mutex>
#include <assert.h>
#include <chrono>

constexpr double pi = 3.14159265358979323846;

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void initialize(uint32_t height, uint32_t width, uint32_t channels = 3);

	inline uint8_t & operator() (uint32_t row, uint32_t col, uint32_t channel);
	inline const uint8_t& operator() (uint32_t row, uint32_t col, uint32_t channel) const;

	inline const uint8_t& operator[](uint32_t idx) const;
	inline uint8_t& operator[](uint32_t idx);

	inline std::mutex& getMutex() const;
	inline const uint8_t* values() const;
	inline uint8_t* values();

	inline uint32_t getSize() const;
	inline uint32_t getWidth() const;
	inline uint32_t getHeight() const;
	inline uint32_t getChannels() const;

private:
	std::vector<uint8_t> m_values;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_channels;

	mutable std::mutex m_accessMutex;
};


class LoopTimer
{
public:
	LoopTimer(std::string tag);
	~LoopTimer();

	std::chrono::duration<float, std::milli> loopDone();
	inline float getAvgDuration();
	inline void resetLoopStartTime();

private:
	std::chrono::time_point<std::chrono::steady_clock> m_last_time;
	std::chrono::time_point<std::chrono::steady_clock> m_last_time_print;
	float m_avgLoopTime;
	std::string m_tag;
};

float LoopTimer::getAvgDuration()
{
	return m_avgLoopTime;
}

void LoopTimer::resetLoopStartTime()
{
	m_last_time = std::chrono::steady_clock::now();
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


// https://en.wikipedia.org/wiki/Miller_cylindrical_projection
// lat = 0 at equator
inline void invCylindricalProjection(float lon, float lat, float& x, float& y) {

	x = lon;
	y = 5.0f / 4.0f * asinh(tan(4.0f * lat / 5.0f));
}

inline void cylindricalProjection(float x, float y, float& lon, float& lat) {
	lon = x;
	lat = 5.0f / 4.0f * atan(sinh(4.0f * y / 5.0f));
}


inline std::vector<std::pair<float, float>> project(std::vector<std::pair<float, float>> coordinates,
	const std::function<void(float, float, float&, float&)>& projection) {
	std::vector<std::pair<float, float>> res(coordinates.size());

	for (size_t i = 0; i < coordinates.size(); i++)
	{
		projection(coordinates[i].first, coordinates[i].second, res[i].first, res[i].second);
	}
	return res;
}
