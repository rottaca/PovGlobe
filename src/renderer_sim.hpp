#pragma once	
#include <CImg.h>

#include "renderer_base.hpp"

class RendererSim: public RendererBase
{
public:
    RendererSim(
        RpmMeasureBase& rpm_measure_base);
    ~RendererSim();


    virtual void initialize(Globe& globe);
    
protected:

    cimg_library::CImgDisplay m_draw_display;

    virtual void render(const Framebuffer&);

};
