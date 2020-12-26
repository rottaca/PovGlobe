#include <thread>
#include <chrono>

#include <CImg.h>

#include "globe.hpp"
#include "application_base.hpp"

#ifdef _WIN32 
	#include "rpm_measure_sim.hpp"
	#include "renderer_sim.hpp"
	using RpmMeasure = RpmMeasureSim;
	using Renderer = RendererSim;
#elif __linux__
	#include "rpm_measure_hall.hpp"
	#include "renderer_led_strip.hpp"
	using RpmMeasure = RpmMeasureHall;
	using Renderer = RendererLedStrip;
#endif


const int WIDTH = 120;
const int HEIGHT = 57;

using namespace std::chrono_literals;

int main() {

	RpmMeasure rpm;
	Renderer renderer(rpm);
	Globe globe(HEIGHT, WIDTH, renderer, rpm);

	ApplicationBase app;
	ApplicationBase app2;
	
	//cimg_library::CImg<unsigned char> img("test.jpg");

	globe.runRendererAsync();
	globe.runApplicationAsync(app);

	std::this_thread::sleep_for(2000ms);

	globe.runApplicationAsync(app2);

	std::this_thread::sleep_for(2000ms);
	globe.shutdown();
}