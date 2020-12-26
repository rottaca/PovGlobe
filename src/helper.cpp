#pragma once

#include "helper.hpp"
#include <iostream>


LoopTimer::LoopTimer(std::string tag)
	: m_avgLoopTime(-1)
	, m_last_time_print(std::chrono::high_resolution_clock::now())
	, m_last_time(std::chrono::high_resolution_clock::now())
	, m_tag(tag)
{
}

LoopTimer::~LoopTimer()
{
}

std::chrono::duration<float, std::milli> LoopTimer::loopDone()
{
	auto elapsed_time = std::chrono::high_resolution_clock::now();
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

float LoopTimer::getAvgDuration()
{
	return m_avgLoopTime;
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


void Framebuffer::initialize(int height, int width, int channels)
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


uint8_t& Framebuffer::operator()(int row, int col, int channel)
{
	// TODO assert
	uint32_t idx = row * (m_width * m_channels) + col * m_channels + channel;
	return m_values[idx];
}


std::mutex& Framebuffer::getMutex() const {
	return m_accessMutex;
}


int Framebuffer::getWidth() const
{
	return m_width;
}


int Framebuffer::getHeight() const
{
	return m_height;
}


int Framebuffer::getChannels() const
{
	return m_channels;
}

