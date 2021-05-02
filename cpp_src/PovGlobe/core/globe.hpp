#pragma once

#include <thread>
#include <atomic>
#include "renderer_base.hpp"
#include "application_base.hpp"
#include "framebuffer.hpp"
#include "helper.hpp"

class Globe
{
public:
    Globe(int vertical_num_leds,
        float radius, 
        float spacing_top, float spacing_bottom, 
        bool doublesidedRendering, RendererBase& renderer);
    ~Globe();

    void runRendererAsync();
    void runApplicationAsync(ApplicationBase& app);
    void shutdown();
    void stopCurrentApp();

    int getVerticalNumPixelsWithLeds() const;
    int getTotalVerticalNumPixels() const;
    int getHorizontalNumPixels() const;
    int getHorizontalOffset() const;
    void setHorizontalOffset(int offset);

    float getRadius() const;
    float getHalfCircumference() const;

    float getSpacingTopCm() const;
    float getSpacingTopRatio() const;
    int getSpacingTopPixels() const;

    float getSpacingBottomCm() const;
    float getSpacingBottomRatio() const;
    int getSpacingBottomPixels() const;

    bool getDoubleSidedRendering() const;

    const Framebuffer& getRenderFrameBuffer() const;
    Framebuffer& getAppFrameBuffer();
    std::mutex& getDoubleBufferMutex();
    void swapFramebuffers();

protected:
    int m_vertical_num_leds;
    int m_horizontal_num_pixels;

    std::atomic<int> m_horizontal_offset;

    int m_total_height_num_leds;
    int m_spacing_top_num_leds;
    int m_spacing_bottom_num_leds;

    float m_spacing_top_ratio;
    float m_spacing_bottom_ratio;

    float m_radius_cm;
    float m_spacing_top_cm;
    float m_spacing_bottom_cm;
    float m_half_circumference_cm;
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


