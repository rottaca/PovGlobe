#include "renderer_base.hpp"


inline RendererBase::RendererBase(RpmMeasureBase& rpm_measure_base)
	: m_rpm_measure_base(rpm_measure_base)
{
}

RendererBase::~RendererBase()
{
}