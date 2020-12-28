#include "globe.hpp"
#include "globe.hpp"
#include <iostream>

Globe::Globe(int height, int width,
	RendererBase& renderer)
	: m_height(height)
	, m_width(width)
	, m_renderer(renderer)
	, m_buffer_indices({0, 1})
{
	m_framebuffers[0].initialize(m_height, m_width );
	m_framebuffers[1].initialize(m_height, m_width);
	//std::lock_guard<std::mutex> guard(m_framebuffer.getMutex());
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

	auto start = std::chrono::high_resolution_clock::now();
	LoopTimer t("Application Processing"), t2("Application Total");
	while (m_applicationThread_running) {
		auto time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float, std::ratio<1,1>> delta = time - start;
		app.process(getAppFrameBuffer(), delta.count());

		// Lock buffer swap
		swapFramebuffers();

		auto total_loop_time = t2.loopDone();
		auto loop_time = t.loopDone();
		auto err = app.getTargetCycleTime() - loop_time;
		if (err.count() > 0) {
			std::this_thread::sleep_for(err);
		}
		t.resetLoopStartTime();
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
	auto tmp = m_buffer_indices.load();
	m_buffer_indices.store({ tmp.app_buffer_idx , tmp.render_buffer_idx});
}