#include "globe.hpp"
#include <iostream>
#include <chrono>

Globe::Globe(int vertical_num_leds,
    float radius_cm, 
    float spacing_top_cm, float spacing_bottom_cm,
    bool doublesidedRendering,
    RendererBase& renderer)
    : m_vertical_num_leds(vertical_num_leds)
    , m_radius_cm(radius_cm)
    , m_spacing_top_cm(spacing_top_cm)
    , m_spacing_bottom_cm(spacing_bottom_cm)
    , m_renderer(renderer)
    , m_buffer_indices({ 0, 1 })
    , m_half_circumference_cm(pi* radius_cm)
    , m_doublesidedRendering(doublesidedRendering)
{
    m_spacing_top_ratio = m_spacing_top_cm / m_half_circumference_cm;
    m_spacing_bottom_ratio = m_spacing_bottom_cm / m_half_circumference_cm;
    const float num_leds_ratio = 1.F - (m_spacing_bottom_cm + m_spacing_top_cm) / m_half_circumference_cm;

    m_total_height_num_leds = std::round(m_vertical_num_leds / num_leds_ratio);
    m_spacing_top_num_leds = std::round(m_total_height_num_leds * m_spacing_top_ratio);
    m_spacing_bottom_num_leds = std::round(m_total_height_num_leds * m_spacing_bottom_ratio);
    // The surface of a sphere has a 2:1 aspect ratio
    m_horizontal_num_pixels = 2 * m_total_height_num_leds;
    assert(m_horizontal_num_pixels % 2 == 0); // We always need an equal number of cols

    m_framebuffers[0].initialize(m_vertical_num_leds, m_horizontal_num_pixels);
    m_framebuffers[1].initialize(m_vertical_num_leds, m_horizontal_num_pixels);

    std::cout << "--------------------- Globe Initialized --------------------" << std::endl;
    std::cout << "Globe Radius:                     " << m_radius_cm << " cm" << std::endl;
    std::cout << "Num LEDs per side:                " << m_vertical_num_leds << std::endl;
    std::cout << "Num Pixels for half circle:       " << m_total_height_num_leds << std::endl;
    std::cout << "Num Pixels horizontal:            " << m_horizontal_num_pixels << std::endl;
    std::cout << "Leds are attached on both sides:  " << (m_doublesidedRendering ? "yes" : "no") << std::endl;
    std::cout << "Spacing top    (cm / ratio in %): " << m_spacing_top_cm << " cm (" << m_spacing_top_ratio * 100 << " %)" << std::endl;
    std::cout << "Spacing bottom (cm / ratio in %): " << m_spacing_bottom_cm << " cm (" << m_spacing_bottom_ratio * 100 << " %)" << std::endl;
}

Globe::~Globe()
{
}

void Globe::runRendererAsync()
{
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
    const bool appInitSuccessful = app.initialize(*this);

    if (!appInitSuccessful) {
        std::cerr << "App initialization failed." << std::endl;
        return;
    }

    const auto target_cycle_time = std::chrono::milliseconds((int)app.getTargetCycleTimeMs());

    const auto start = std::chrono::high_resolution_clock::now();
    LoopTimer t("Application Processing");
    while (m_applicationThread_running) {
        const auto cycle_start_time = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<float, std::ratio<1, 1>> delta = cycle_start_time - start;
        app.process(getAppFrameBuffer(), delta.count());

        // Lock buffer swap
        // TODO: Still not completely correct, as only the indices are locked, not the buffers itself.
        swapFramebuffers();
        
        const auto cycle_end_time = cycle_start_time + target_cycle_time;
        const auto err = cycle_end_time - std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(err);
        t.loopDone();
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

void Globe::stopCurrentApp()
{
    m_applicationThread_running = false;
    if (m_applicationThread.joinable()) {
        m_applicationThread.join();
    }
}

int Globe::getVerticalNumPixelsWithLeds() const {
    return m_vertical_num_leds;
}

int Globe::getTotalVerticalNumPixels() const
{
    return m_total_height_num_leds;
}

int Globe::getHorizontalNumPixels() const
{
    return m_horizontal_num_pixels;
}

float Globe::getRadius() const
{
    return m_radius_cm;
}

float Globe::getHalfCircumference() const {
    return m_half_circumference_cm;
}

float Globe::getSpacingTopRatio() const
{
    return m_spacing_top_ratio;
}

float Globe::getSpacingBottomRatio() const
{
    return m_spacing_bottom_ratio;
}

float Globe::getSpacingTopCm() const
{
    return m_spacing_top_cm;
}

float Globe::getSpacingBottomCm() const
{
    return m_spacing_bottom_cm;
}

int Globe::getSpacingTopPixels() const {
    return m_spacing_top_num_leds;
}

int Globe::getSpacingBottomPixels() const {
    return m_spacing_bottom_num_leds;
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
