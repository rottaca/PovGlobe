#include <iostream>
#include <chrono>

#include "core/globe.hpp"

#ifdef BUILD_SIM 
	#include "sim/rpm_measure_sim.hpp"
	#include "sim/renderer_sim.hpp"
#elif BUILD_HW
	#include "hw/rpm_measure_hall.hpp"
	#include "hw/renderer_led_strip.hpp"
#endif

const int width = 120;
const int height = 57;
const float radius = 13.25;
const float spacing_top = 1.0;
const float spacing_bottom = 2.0;

const int HALL_SENSOR_GPIO_PIN = 25;
const int LED_STRIP_GPIO_PIN = 18;

int main(int argc, char* argv[]) {
#ifdef BUILD_SIM 
	RpmMeasureSim rpm;
	RendererSim renderer(rpm);
#elif BUILD_HW
	RpmMeasureHall rpm(HALL_SENSOR_GPIO_PIN);
	RendererLedStrip renderer(rpm, LED_STRIP_GPIO_PIN);
#endif

	Globe globe(height, width, radius, spacing_top, spacing_bottom, false, renderer);

	rpm.initialize(globe);

	auto last_time_print = std::chrono::steady_clock::now();
	while (true) {

		const std::chrono::duration<float, std::ratio<1,1>> dt = std::chrono::steady_clock::now() - last_time_print;
		if (dt.count() > 1.0) {
			last_time_print = std::chrono::steady_clock::now();

			const auto rpmData = rpm.getRpmData();
			std::cout << rpmData.cycle_time.count() << " ms, "<< 1000.0f/rpmData.cycle_time.count() << " Hz, position " << rpmData.curr_temporal_pos << std::endl;
		}
	}

}