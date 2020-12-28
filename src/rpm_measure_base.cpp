

#include "rpm_measure_base.hpp"

#include "globe.hpp"
#include <iostream>

RpmMeasureBase::RpmMeasureBase() {
	m_temporal_resolution = -1;
}

RpmMeasureBase::~RpmMeasureBase() {


}
void RpmMeasureBase::initialize(Globe& globe) {
	std::cout << "Initialize RpmMeasurment..." << std::endl;
	m_temporal_resolution = globe.getWidth();
}