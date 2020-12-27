#pragma once

#include "helper.hpp"

#include <chrono>
using namespace std::chrono_literals;

class Globe;

class ApplicationBase {


public:
	void initialize(Globe& globe);

	void process(Framebuffer& framebuffer, float time);

	std::chrono::duration<float, std::milli> getTargetCycleTIme() const;

private:
	int m_last_pixel;
};
