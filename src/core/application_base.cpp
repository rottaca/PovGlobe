#include "application_base.hpp"
#include "globe.hpp"
#include <iostream>

void ApplicationBase::initialize(Globe& globe)
{
}

std::chrono::duration<float, std::milli> ApplicationBase::getTargetCycleTime() const {
	return 33ms;
}