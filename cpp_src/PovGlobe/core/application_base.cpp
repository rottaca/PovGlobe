#include "application_base.hpp"
#include "globe.hpp"
#include <iostream>

using namespace std::chrono_literals;

bool ApplicationBase::initialize(Globe& globe)
{
    return true;
}

float ApplicationBase::getTargetCycleTimeMs() const {
    return 33.f;
}