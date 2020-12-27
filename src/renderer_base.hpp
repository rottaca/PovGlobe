#pragma once

#include <thread>

#include "rpm_measure_base.hpp"
#include "helper.hpp"

class Globe;

class RendererBase
{
public:
	RendererBase(
		RpmMeasureBase& rpm_measure
	);
	virtual ~RendererBase();

	virtual void initialize(Globe& globe);
	virtual void runAsync(Globe& globe);
	virtual void run(Globe& globe);

	
	virtual void stopAndJoinRenderThread();

protected:
	RpmMeasureBase& m_rpmMeasure;
	std::thread m_renderThread;
	std::atomic_bool m_renderThread_running;

	virtual void render(const Framebuffer&) = 0;

};
