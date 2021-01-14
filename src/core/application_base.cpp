#include "application_base.hpp"
#include "globe.hpp"
#include <iostream>

using namespace std::chrono_literals;

void ApplicationBase::initialize(Globe& globe)
{
}

float ApplicationBase::getTargetCycleTimeMs() const {
    return 33.f;
}