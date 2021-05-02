#pragma once

#include <thread>
#include <atomic>

#include "framebuffer.hpp"

class Globe;

class RendererBase
{
public:
    RendererBase();
    virtual ~RendererBase();

    virtual void runAsync(Globe& globe);
    virtual void run(Globe& globe);


    virtual void stopAndJoinRenderThread();

protected:
    std::thread m_renderThread;
    std::atomic_bool m_renderThread_running;

    virtual void initialize(Globe& globe);
    virtual void render(const Framebuffer&, int32_t horizontal_offset) = 0;

};
