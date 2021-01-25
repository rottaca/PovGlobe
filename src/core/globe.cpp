#include "globe.hpp"
#include <iostream>

Globe::Globe(int height, int width, float radius, float spacing_top, float spacing_bottom, bool doublesidedRendering,
    RendererBase& renderer)
    : m_height(height)
    , m_width(width)
    , m_radius(radius)
    , m_spacing_top(spacing_top)
    , m_spacing_bottom(spacing_bottom)
    , m_renderer(renderer)
    , m_buffer_indices({ 0, 1 })
    , m_half_circumference(pi* radius)
    , m_doublesidedRendering(doublesidedRendering)
{
    assert(m_width % 2 == 0); // We always need an equal number of cols
    m_framebuffers[0].initialize(m_height, m_width);
    m_framebuffers[1].initialize(m_height, m_width);
    
}

Globe::~Globe()
{
}

void Globe::runRendererAsync()
{
    m_renderer.initialize(*this);
    m_renderer.runAsync(*this);
}

void Globe::runApplicationAsync(ApplicationBase& app)
{
    m_applicationThread_running = false;
    if (m_applicationThread.joinable()) {
        m_applicationThread.join();
    }
    m_applicationThread_running = true;

    m_applicationThread = std::thread(&Globe::runApplication, this, std::ref(app));
}


void Globe::runApplication(ApplicationBase& app)
{
    std::cout << "Initialize Application..." << std::endl;
    app.initialize(*this);
    const auto start = std::chrono::high_resolution_clock::now();
    LoopTimer t("Application Processing");
    while (m_applicationThread_running) {
        const auto time = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float, std::ratio<1, 1>> delta = time - start;
        app.process(getAppFrameBuffer(), delta.count());

        // Lock buffer swap
        // TODO: Still not completely correct, as only the indices are locked, not the buffers itself.
        swapFramebuffers();

        const auto loop_time = t.loopDone();
        const auto err = app.getTargetCycleTimeMs() - loop_time.count();
        if (err > 0) {
            std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(5));
        }
    }
}

void Globe::shutdown()
{
    m_renderer.stopAndJoinRenderThread();
    m_applicationThread_running = false;
    if (m_applicationThread.joinable()) {
        m_applicationThread.join();
    }
}

int Globe::getHeight() const
{
    return m_height;
}

float Globe::getRadius() const
{
    return m_radius;
}

int Globe::getWidth() const
{
    return m_width;
}

float Globe::getHalfCircumference() const {
    return m_half_circumference;
}

float Globe::getSpacingTop() const
{
    return m_spacing_top;
}

float Globe::getSpacingBottom() const
{
    return m_spacing_bottom;
}


bool Globe::getDoubleSidedRendering() const {
    return m_doublesidedRendering;
}

const Framebuffer& Globe::getRenderFrameBuffer() const
{
    return m_framebuffers[m_buffer_indices.load().render_buffer_idx];
}

Framebuffer& Globe::getAppFrameBuffer()
{
    return m_framebuffers[m_buffer_indices.load().app_buffer_idx];
}

std::mutex& Globe::getDoubleBufferMutex()
{
    return m_double_buffer_mutex;
}

void Globe::swapFramebuffers() {
    const auto tmp = m_buffer_indices.load();
    m_buffer_indices.store({ tmp.app_buffer_idx , tmp.render_buffer_idx });
}
