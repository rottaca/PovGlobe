#pragma once

#include "renderer_base.hpp"
#include <thread>
#include <atomic>
#include "rpm_measure_base.hpp"
#include "application_base.hpp"
#include "helper.hpp"

class Globe
{
public:
    Globe(int height, int width, float radius, float spacing_top, float spacing_bottom, bool doublesidedRendering, RendererBase& renderer);
    ~Globe();

    void runRendererAsync();
    void runApplicationAsync(ApplicationBase& app);
    void shutdown();

    int getHeight() const;
    int getWidth() const;
    float getRadius() const;
    float getSpacingTop() const;
    float getSpacingBottom() const;
    float getHalfCircumference() const;
    bool getDoubleSidedRendering() const;

    const Framebuffer& getRenderFrameBuffer() const;
    Framebuffer& getAppFrameBuffer();
    std::mutex& getDoubleBufferMutex();
    void swapFramebuffers();

protected:
    int m_height;
    int m_width;
    float m_radius;
    float m_spacing_top;
    float m_spacing_bottom;
    float m_half_circumference;
    bool m_doublesidedRendering;

    RendererBase& m_renderer;

    std::mutex m_double_buffer_mutex;
    Framebuffer m_framebuffers[2];
    struct BufferIndices {
        int render_buffer_idx;
        int app_buffer_idx;
    };

    std::atomic<BufferIndices> m_buffer_indices;

    std::thread m_applicationThread;
    std::atomic_bool m_applicationThread_running;



    void runApplication(ApplicationBase& app);
};


