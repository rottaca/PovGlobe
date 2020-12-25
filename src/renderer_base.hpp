
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

inline RendererBase::RendererBase(RpmMeasureBase& rpm_measure_base)
	: m_rpm_measure_base(rpm_measure_base)
{
}

RendererBase::~RendererBase()
{
}