#include "renderer_base.hpp"
#include "renderer_base.hpp"
#include "renderer_base.hpp"
#include "globe.hpp"

#include <iostream>

RendererBase::RendererBase(RpmMeasureBase& rpm_measure)
	: m_rpmMeasure(rpm_measure)
{
}

RendererBase::~RendererBase()
{
}

void RendererBase::initialize(Globe& globe)
{
	stopAndJoinRenderThread();
	m_renderThread_running = true;
	m_rpmMeasure.initialize(globe);
}

void RendererBase::runAsync(Globe& globe)
{	
	m_renderThread = std::thread(&RendererBase::run, this, std::ref(globe));
}

void RendererBase::run(Globe& globe)
{
	std::mutex& mutex = globe.getDoubleBufferMutex();
	LoopTimer t("Render Thread");
	while (m_renderThread_running)
	{
		{
			const Framebuffer& framebuffer = globe.getRenderFrameBuffer();
			render(framebuffer);
		}
		auto loop_time = t.loopDone();
	}
}

void RendererBase::stopAndJoinRenderThread()
{
	m_renderThread_running = false;
	if (m_renderThread.joinable()) {
		m_renderThread.join();
	}
}
