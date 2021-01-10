#include "application_base.hpp"
#include "globe.hpp"
#include <iostream>

using namespace std::chrono_literals;

void ApplicationBase::initialize(Globe& globe)
{
}

std::chrono::duration<float, std::milli> ApplicationBase::getTargetCycleTime() const {
    return 33ms;
}