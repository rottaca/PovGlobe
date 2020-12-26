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
	~RendererBase();

	void initialize(Globe& globe);
	void runAsync(Globe& globe);
	void run(Globe& globe);

	void stopAndJoinRenderThread();

protected:
	RpmMeasureBase& m_rpmMeasure;
	std::thread m_renderThread;
	std::atomic_bool m_renderThread_running;
};
