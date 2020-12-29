#include "application_base.hpp"
#include "globe.hpp"
#include <iostream>

void ApplicationBase::initialize(Globe& globe)
{
	m_last_pixel = 0;
}

void ApplicationBase::process(Framebuffer& framebuffer, float time)
{
	//for (int i = 0; i < framebuffer.getSize(); i++) {
	//	framebuffer[i] = 0;
	//}

	for (int i = 0; i < framebuffer.getHeight(); i++) {
		framebuffer(i, 0, 0) = 255;
		framebuffer(i, 1, 1) = 255;
	}


	framebuffer(framebuffer.getHeight() / 2, framebuffer.getWidth() / 2, 0) = 255;
	framebuffer(framebuffer.getHeight() / 2, framebuffer.getWidth() / 2, 1) = 255;
	framebuffer(framebuffer.getHeight() / 2, framebuffer.getWidth() / 2, 2) = 255;

	framebuffer(15, framebuffer.getWidth() / 2, 0) = 255;
	framebuffer(15, framebuffer.getWidth() / 2, 1) = 255;
	framebuffer(15, framebuffer.getWidth() / 2, 2) = 255;


	framebuffer(m_last_pixel, framebuffer.getWidth() / 2 - 2, 0) = static_cast<int>(5*time) % 255;
	framebuffer(m_last_pixel, framebuffer.getWidth() / 2 - 2, 1) = static_cast<int>(5 * time) % 255;
	m_last_pixel = (m_last_pixel + 1) % framebuffer.getHeight();
}


std::chrono::duration<float, std::milli> ApplicationBase::getTargetCycleTime() const {
	return 33ms;
}