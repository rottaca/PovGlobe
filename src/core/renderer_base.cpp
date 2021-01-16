#include "renderer_base.hpp"
#include "globe.hpp"

#include <iostream>

#if defined(__unix__) || defined(__unix) 
#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include <sched.h>
#endif

RendererBase::RendererBase(RpmMeasureBase& rpm_measure)
    : m_rpmMeasure(rpm_measure)
{
}

RendererBase::~RendererBase()
{
}

void RendererBase::initialize(Globe& globe)
{
    stopAndJoinRenderThread();
    std::cout << "Initialize Renderer..." << std::endl;
    m_renderThread_running = true;
    m_rpmMeasure.initialize(globe);
}

void RendererBase::runAsync(Globe& globe)
{
    m_renderThread = std::thread(&RendererBase::run, this, std::ref(globe));

}

void RendererBase::run(Globe& globe)
{
    LoopTimer t("Render Thread");
    while (m_renderThread_running)
    {
        const Framebuffer& framebuffer = globe.getRenderFrameBuffer();
        render(framebuffer);
        const auto loop_time = t.loopDone();
        // This is necessary if thread priority is set to realtime.
        std::this_thread::sleep_for(std::chrono::duration<float, std::nano>(1.0f));
    }
}

void RendererBase::stopAndJoinRenderThread()
{
    m_renderThread_running = false;
    if (m_renderThread.joinable()) {
        m_renderThread.join();
    }
}
