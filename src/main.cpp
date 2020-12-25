
#include "globe.hpp"

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

int main() {

	Renderer renderer;
	RpmMeasure rpm;
	Globe globe(renderer, rpm);


}