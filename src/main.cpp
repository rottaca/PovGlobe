#include <thread>
#include <chrono>

#include <CImg.h>

#include "globe.hpp"
#include "application_base.hpp"

#ifdef BUILD_SIM 
	#include "rpm_measure_sim.hpp"
	#include "renderer_sim.hpp"
#elif BUILD_HW
	#include "rpm_measure_hall.hpp"
	#include "renderer_led_strip.hpp"
#endif

const int width = 120;
const int height = 57;
const float radius = 13.25;
const float spacing_top = 1.0;
const float spacing_bottom = 2.0;

const int HALL_SENSOR_GPIO_PIN = 25;
const int LED_STRIP_GPIO_PIN = 18;

using namespace std::chrono_literals;

int main() {

#ifdef BUILD_SIM 
	RpmMeasureSim rpm;
	RendererSim renderer(rpm);
#elif BUILD_HW
	RpmMeasureHall rpm(HALL_SENSOR_GPIO_PIN);
	RendererLedStrip renderer(rpm, LED_STRIP_GPIO_PIN);
#endif
	Globe globe(height, width, radius, spacing_top, spacing_bottom, renderer);

	ApplicationBase app;
	
	//cimg_library::CImg<unsigned char> img("test.jpg");

	globe.runRendererAsync();
	globe.runApplicationAsync(app);

	std::this_thread::sleep_for(50000ms);

	globe.shutdown();
}