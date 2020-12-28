#include "application_base.hpp"
#include "globe.hpp"
#include <iostream>
void ApplicationBase::initialize(Globe& globe)
{
	//m_last_pixel = 0;
}

void ApplicationBase::process(Framebuffer& framebuffer, float time)
{	
	for (int i = 0; i < framebuffer.getSize(); i++) {
		framebuffer.values()[i] = int(i + time*100) % 255;
	}
}


std::chrono::duration<float, std::milli> ApplicationBase::getTargetCycleTime() const {
	return 33ms;
}