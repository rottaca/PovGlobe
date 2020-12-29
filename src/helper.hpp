#pragma once

#include <vector>
#include <mutex>
#include <assert.h>
#include <chrono>

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void initialize(uint32_t height, uint32_t width, uint32_t channels = 3);

	uint8_t & operator() (uint32_t row, uint32_t col, uint32_t channel);
	uint8_t operator() (uint32_t row, uint32_t col, uint32_t channel) const;

	uint8_t operator[](uint32_t idx) const;
	uint8_t& operator[](uint32_t idx);

	std::mutex& getMutex() const;
	const uint8_t* values() const;
	uint8_t* values();

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


class LoopTimer
{
public:
	LoopTimer(std::string tag);
	~LoopTimer();

	std::chrono::duration<float, std::milli> loopDone();
	float getAvgDuration();
	void resetLoopStartTime();

private:
	std::chrono::time_point<std::chrono::steady_clock> m_last_time;
	std::chrono::time_point<std::chrono::steady_clock> m_last_time_print;
	float m_avgLoopTime;
	std::string m_tag;
};
