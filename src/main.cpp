#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <functional>
#include <algorithm>

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

const int default_width = 120;
const int default_height = 55;
const float default_radius = 13.25;
const float default_spacing_top = 1.0;
const float default_spacing_bottom = 2.0;

const int default_hall_sensor_gpio_pin = 25;
const int default_led_strip_gpio_pin = 18;

using namespace std::chrono_literals;

using AlgoFactory = std::function<std::unique_ptr<ApplicationBase>(int, char* [])>;

char* getCmdOption(char** begin, char** end, const std::string option)
{
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string option)
{
    return std::find(begin, end, option) != end;
}

const std::map<std::string, AlgoFactory> algo_factory = {
    {"Test1", [](int argc, char* argv[]) {return std::make_unique<ApplicationTest1>(); }},
    {"ImageViewer", [](int argc, char* argv[]) {
        const char* file_name = getCmdOption(argv, argv + argc, "-f");
        return std::make_unique<ApplicationImageViwewer>(file_name, equirectangularProjection, interpolateNearestNeighbour);
    }},
    {"ImageRotator", [](int argc, char* argv[]) {
        const char* file_name = getCmdOption(argv, argv + argc, "-f");
        return std::make_unique<ApplicationImageRotator>(file_name, equirectangularProjection, interpolateNearestNeighbour);
    }},
};

std::unique_ptr<ApplicationBase> instantiateAlgorithms(int argc, char* argv[]) {
    const char* algo_name = getCmdOption(argv, argv + argc, "-a");

    if (!algo_name) {
        std::cout << "No algo specified. Running default algo ApplicationTest1." << std::endl;
        return std::make_unique<ApplicationTest1>();
    }
    else {
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
    const auto setterInt = [argc, argv](const char* opt, int fallback) {
        const char* strVal = getCmdOption(argv, argv + argc, opt);
        return strVal ? atoi(strVal) : fallback;
    };
    const auto setterFloat = [argc, argv](const char* opt, float fallback) {
        const char* strVal = getCmdOption(argv, argv + argc, opt);
        return strVal ? atof(strVal) : fallback;
    };
    // Parse commandline options
    const int width = setterInt("-w", default_width);
    const int height = setterInt("-h", default_height);
    const float radius = setterFloat("-r", default_radius);
    const float spacing_top = setterFloat("-t", default_spacing_top);
    const float spacing_bottom = setterFloat("-b", default_spacing_bottom);
    const int hall_sensor_gpio_pin = setterInt("-h", default_hall_sensor_gpio_pin);
    const int led_strip_gpio_pin = setterInt("-l", default_led_strip_gpio_pin);

    const bool double_sided = cmdOptionExists(argv, argv + argc, "-d");

#ifdef BUILD_SIM 
    RpmMeasureSim rpm;
    RendererSim renderer(rpm);
#elif BUILD_HW
    RpmMeasureHall rpm(hall_sensor_gpio_pin);
    RendererLedStrip renderer(rpm);
#endif

    Globe globe(height, width, radius, spacing_top, spacing_bottom, double_sided, renderer);

    auto app_ptr = instantiateAlgorithms(argc, argv);

    globe.runRendererAsync();
    globe.runApplicationAsync(*app_ptr);

    std::this_thread::sleep_for(60000ms);

    globe.shutdown();
}