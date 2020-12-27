#include "renderer_sim.hpp"
#include "globe.hpp"

RendererSim::RendererSim(
	RpmMeasureBase& rpm_measure_base)
	:RendererBase(rpm_measure_base)
{
}

RendererSim::~RendererSim()
{
}

void RendererSim::initialize(Globe& globe)
{
	RendererBase::initialize(globe);
	m_draw_display.assign(3*globe.getWidth(), 3 * globe.getHeight(), "Visualization");

	m_draw_display.move(0, 0);

}

void RendererSim::render(const Framebuffer& buffer)
{
	//using namespace std::chrono_literals;
	//std::this_thread::sleep_for(1000ms);
	if (!m_draw_display.is_closed()) {
		cimg_library::CImg<unsigned char> img(buffer.values(), buffer.getWidth(),
											  buffer.getHeight(), 1,
											  buffer.getChannels(), true);
		m_draw_display.display(img.get_resize_tripleXY());
	}
}
