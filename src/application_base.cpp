#include "application_base.hpp"
#include "globe.hpp"

void ApplicationBase::initialize(Globe& globe)
{
}

void ApplicationBase::process(Framebuffer& framebuffer, float time)
{
	framebuffer(0, 0, 0) = static_cast<int>(time) % 255;
	framebuffer(0, 0, 1) = 255;
}
