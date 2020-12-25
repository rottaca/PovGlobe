#pragma once

#include "renderer_base.hpp"
#include "rpm_measure_base.hpp"

class Globe
{
public:
	Globe(RendererBase& renderer, 
		  RpmMeasureBase& rpmMeasure);
	~Globe();

private:
	RendererBase& m_renderer;
	RpmMeasureBase& m_rpmMeasure;
};
