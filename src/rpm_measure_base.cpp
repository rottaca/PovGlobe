

#include "rpm_measure_base.hpp"

#include "globe.hpp"

RpmMeasureBase::RpmMeasureBase() {
	m_temporal_resolution = -1;
}

RpmMeasureBase::~RpmMeasureBase() {


}
void RpmMeasureBase::initialize(Globe& globe) {
	m_temporal_resolution = globe.getWidth();
}