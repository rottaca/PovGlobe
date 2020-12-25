#pragma once	

#include "renderer_base.hpp"

class RendererLedStrip: public RendererBase
{
public:
	RendererLedStrip(
		RendererBase& rpm_measure_base);
	~RendererLedStrip();

private:

};

RendererLedStrip::RendererLedStrip(RendererBase& rpm_measure_base)
	:RendererBase(rpm_measure_base)
{
}

RendererLedStrip::~RendererLedStrip()
{
}