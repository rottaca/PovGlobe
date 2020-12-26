#include "globe.hpp"
#include "globe.hpp"
#include <iostream>

Globe::Globe(int height, int width,
	RendererBase& renderer,
	RpmMeasureBase& rpmMeasure)
	: m_height(height)
	, m_width(width)
	, m_renderer(renderer)
	, m_rpmMeasure(rpmMeasure)
	, m_render_buffer_idx(1)
	, m_app_buffer_idx(0)
{
	m_framebuffers[m_app_buffer_idx].initialize(m_height, m_width );
	m_framebuffers[m_render_buffer_idx].initialize(m_height, m_width);
	//std::lock_guard<std::mutex> guard(m_framebuffer.getMutex());
}

Globe::~Globe()
{
}

void Globe::runRendererAsync()
{
	m_renderer.initialize(*this);
	m_rpmMeasure.initialize(*this);

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
	app.initialize(*this);

	auto start = std::chrono::high_resolution_clock::now();
	while (m_applicationThread_running) {
		auto time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::milli> delta = time - start;
		app.process(getAppFrameBuffer(), delta.count());

		// Lock buffer swap
		swapFramebuffers();

		std::cout << "App thread running at time" << delta.count() << std::endl;
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

int Globe::getWidth() const
{
	return m_width;
}

const Framebuffer& Globe::getRenderFrameBuffer() const
{
	return m_framebuffers[m_render_buffer_idx];
}

Framebuffer& Globe::getAppFrameBuffer()
{
	return m_framebuffers[m_app_buffer_idx];
}

std::mutex& Globe::getDoubleBufferMutex()
{
	return m_double_buffer_mutex;
}

void Globe::swapFramebuffers() {
	std::lock_guard<std::mutex> guard(m_double_buffer_mutex);
	std::swap(m_render_buffer_idx, m_app_buffer_idx);
}