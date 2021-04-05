#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <functional>
#include <algorithm>
#include <memory>

#include <CImg.h>

#include <Magick++.h>

#include "core/globe.hpp"
#include "apps/application_examples.hpp"

#ifdef SIM_AVAILABLE 
#include "sim/renderer_sim.hpp"
#endif

#ifdef HW_AVAILABLE
#include "hw/renderer_led_strip_pico.hpp"
#endif

const int default_height_leds = 55;
const float default_radius = 13.25;
const float default_spacing_top = 1.5;
const float default_spacing_bottom = 2.0;

const int default_hall_sensor_gpio_pin = 25;
const char* default_pico_port_name = "/dev/ttyACM0";

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

const EquirectangularProjection equrectProj;
const NearestNeighbourPixelInterpolation nearestNeighbour;

const std::map<std::string, AlgoFactory> algo_factory = {
    {"Test1", [](int argc, char* argv[]) {return std::make_unique<ApplicationTest1>(); }},
    {"ImageViewer", [](int argc, char* argv[]) {
        const char* file_name = getCmdOption(argv, argv + argc, "-f");
        return std::make_unique<ApplicationImageViewer>(file_name, equrectProj, nearestNeighbour);
    }},
    {"ImageRotator", [](int argc, char* argv[]) {
        const char* file_name = getCmdOption(argv, argv + argc, "-f");
        return std::make_unique<ApplicationImageRotator>(file_name, equrectProj, nearestNeighbour);
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
    const auto setterStr = [argc, argv](const char* opt, const char* fallback) {
        const char* strVal = getCmdOption(argv, argv + argc, opt);
        return strVal ? strVal : fallback;
    };

    // Parse commandline options
    const int height_leds = setterInt("-h", default_height_leds);
    const float radius = setterFloat("-r", default_radius);
    const float spacing_top = setterFloat("-t", default_spacing_top);
    const float spacing_bottom = setterFloat("-b", default_spacing_bottom);
    const char* pico_port_name = setterStr("-p", default_pico_port_name);

    const bool double_sided = cmdOptionExists(argv, argv + argc, "-d");    
    const bool use_hw = cmdOptionExists(argv, argv + argc, "-k");


    std::shared_ptr<RendererBase> renderer;

    if (use_hw) {
#ifdef HW_AVAILABLE 
    renderer = std::make_shared<RendererLedStripPico>(pico_port_name);
#else
    std::cerr << "Not build with HW support!" << std::endl;  
    exit(1);
#endif
    } else{
#ifdef SIM_AVAILABLE 
    renderer = std::make_shared<RendererSim>();
#else
    std::cerr << "Not build with simulation support!" << std::endl;  
    exit(1);
#endif
    }
        
    Globe globe(height_leds, radius, spacing_top, spacing_bottom, double_sided, *renderer);

    auto app_ptr = instantiateAlgorithms(argc, argv);

    globe.runRendererAsync();
    globe.runApplicationAsync(*app_ptr);

    while(true) {
      std::this_thread::sleep_for(1000ms);
    }
    globe.shutdown();
}