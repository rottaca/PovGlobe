#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <functional>

#include <CImg.h>

#include <Magick++.h>

#include "core/globe.hpp"
#include "apps/application_examples.hpp"

#ifdef BUILD_SIM 
#include "sim/rpm_measure_sim.hpp"
#include "sim/renderer_sim.hpp"
#elif BUILD_HW
#include "hw/rpm_measure_hall.hpp"
#include "hw/renderer_led_strip.hpp"
#endif

const int width = 120;
const int height = 55;
const float radius = 13.25;
const float spacing_top = 1.0;
const float spacing_bottom = 2.0;

const int HALL_SENSOR_GPIO_PIN = 25;
const int LED_STRIP_GPIO_PIN = 18;

using namespace std::chrono_literals;


using AlgoFactory = std::function<std::unique_ptr<ApplicationBase>(int, char* [])>;

const std::map<std::string, AlgoFactory> algo_factory = {
    {"Test1", [](int argc, char* argv[]) {return std::make_unique<ApplicationTest1>(); }},
    {"ImageViewer", [](int argc, char* argv[]) {return std::make_unique<ApplicationImageViwewer>(argv[2], equirectangularProjection, interpolateNearestNeighbour); }},
    {"ImageRotator", [](int argc, char* argv[]) {return std::make_unique<ApplicationImageRotator>(argv[2], equirectangularProjection, interpolateNearestNeighbour); }},
};

std::unique_ptr<ApplicationBase> instantiateAlgorithms(int argc, char* argv[]) {

    for (size_t i = 0; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }
    if (argc <= 1) {
        std::cout << "No algo specified. Running default algo ApplicationTest1." << std::endl;
        return std::make_unique<ApplicationTest1>();
    }
    else {
        const std::string algo_name{ argv[1] };
        if (algo_factory.find(algo_name) != algo_factory.end()) {
            return algo_factory.at(algo_name)(argc, argv);
        }
        else {
            std::cerr << "Invalid algo name " << algo_name << std::endl;
            return nullptr;
        }
    }
}

int main(int argc, char* argv[]) {

#ifdef BUILD_SIM 
    RpmMeasureSim rpm;
    RendererSim renderer(rpm);
#elif BUILD_HW
    RpmMeasureHall rpm(HALL_SENSOR_GPIO_PIN);
    RendererLedStrip renderer(rpm, LED_STRIP_GPIO_PIN);
#endif
    Globe globe(height, width, radius, spacing_top, spacing_bottom, true, renderer);

    auto app_ptr = instantiateAlgorithms(argc, argv);

    globe.runRendererAsync();
    globe.runApplicationAsync(*app_ptr);

    std::this_thread::sleep_for(60000ms);

    globe.shutdown();
}