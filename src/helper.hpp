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


class LoopTimer
{
public:
	LoopTimer(std::string tag);
	~LoopTimer();

	std::chrono::duration<float, std::milli> loopDone();
	float getAvgDuration();

private:
	std::chrono::time_point<std::chrono::steady_clock> m_last_time;
	std::chrono::time_point<std::chrono::steady_clock> m_last_time_print;
	float m_avgLoopTime;
	std::string m_tag;
};
