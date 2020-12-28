#include <thread>
#include <chrono>

#include <CImg.h>

#include "globe.hpp"
#include "application_base.hpp"

#ifdef BUILD_SIM 
	#include "rpm_measure_sim.hpp"
	#include "renderer_sim.hpp"
	using RpmMeasure = RpmMeasureSim;
	using Renderer = RendererSim;
#elif BUILD_HW
	#include "rpm_measure_hall.hpp"
	#include "renderer_led_strip.hpp"
	using RpmMeasure = RpmMeasureHall;
	using Renderer = RendererLedStrip;
#endif


const int width = 120;
const int height = 57;

using namespace std::chrono_literals;

int main() {

	RpmMeasure rpm;
	Renderer renderer(rpm);
	Globe globe(height, width, renderer);

	ApplicationBase app;
	
	//cimg_library::CImg<unsigned char> img("test.jpg");

	globe.runRendererAsync();
	globe.runApplicationAsync(app);

	std::this_thread::sleep_for(50000ms);

	globe.shutdown();
}