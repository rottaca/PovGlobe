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
    
#if defined(__unix__) || defined(__unix) 
    // Set thread priority to max for optimized rendering performance.
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);

    if(pthread_setaffinity_np(m_renderThread.native_handle(), sizeof(cpu_set_t), &cpuset)){
        std::cout << "pthread_setaffinity_np failed: " << std::strerror(errno) << std::endl;
        exit(1);
    }else{
        std::cout << "CPU affinity of render thread changed to CPU0."<< std::endl;
    }
    
    sched_param sch;
    sch.sched_priority = 1;
    if (pthread_setschedparam(m_renderThread.native_handle(), SCHED_FIFO, &sch)) {
        std::cout << "setschedparam failed: " << std::strerror(errno) << std::endl;
        exit(1);
    }else{
        std::cout << "Render thread policy changed to FIFO and priority increased."<< std::endl;
    }
    
#endif
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
        //std::this_thread::sleep_for(std::chrono::duration<float, std::nano>(1.0f));
    }
}

void RendererBase::stopAndJoinRenderThread()
{
    m_renderThread_running = false;
    if (m_renderThread.joinable()) {
        m_renderThread.join();
    }
}
