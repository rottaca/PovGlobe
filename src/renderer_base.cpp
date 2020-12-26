#include "renderer_base.hpp"
#include "renderer_base.hpp"
#include "renderer_base.hpp"
#include "globe.hpp"

#include <iostream>

RendererBase::RendererBase(RpmMeasureBase& rpm_measure_base)
	: m_rpm_measure_base(rpm_measure_base)
{
}

RendererBase::~RendererBase()
{
}

void RendererBase::initialize(Globe& globe)
{
	stopAndJoinRenderThread();
	m_renderThread_running = true;
}

void RendererBase::runAsync(Globe& globe)
{	
	m_renderThread = std::thread(&RendererBase::run, this, std::ref(globe));
}

void RendererBase::run(Globe& globe)
{
	std::mutex& mutex = globe.getDoubleBufferMutex();
	while (m_renderThread_running)
	{
		std::lock_guard<std::mutex> guard(mutex);
		const Framebuffer& framebuffer = globe.getRenderFrameBuffer();

		std::cout << "Render thread running" << std::endl;
	}
}

void RendererBase::stopAndJoinRenderThread()
{
	m_renderThread_running = false;
	if (m_renderThread.joinable()) {
		m_renderThread.join();
	}
}
