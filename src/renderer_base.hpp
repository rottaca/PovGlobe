#pragma once

#include "rpm_measure_base.hpp"

class RendererBase
{
public:
	RendererBase(
		RpmMeasureBase& rpm_measure_base
	);
	~RendererBase();

protected:
	RpmMeasureBase& m_rpm_measure_base;
};
