#pragma once	

#include "renderer_base.hpp"

class RendererLedStrip: public RendererBase
{
public:
	RendererLedStrip(
		RpmMeasureBase& rpm_measure_base);
	~RendererLedStrip();

protected:

	virtual void render(const Framebuffer&);
};
