#pragma once

#include <thread>
#include <atomic>

#include "rpm_measure_base.hpp"
#include "framebuffer.hpp"

class Globe;

class RendererBase
{
public:
    RendererBase(
        RpmMeasureBase& rpm_measure
    );
    virtual ~RendererBase();

    virtual void runAsync(Globe& globe);
    virtual void run(Globe& globe);


    virtual void stopAndJoinRenderThread();

protected:
    RpmMeasureBase& m_rpmMeasure;
    std::thread m_renderThread;
    std::atomic_bool m_renderThread_running;

    virtual void initialize(Globe& globe);
    virtual void render(const Framebuffer&) = 0;

};
