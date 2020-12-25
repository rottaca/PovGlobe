

#include "rpm_measure_base.hpp"


RpmMeasureBase::RpmMeasureBase() {
	m_temporal_resolution = -1;
}

RpmMeasureBase::~RpmMeasureBase() {


}
void RpmMeasureBase::initialize(int temporal_resolution) {
	m_temporal_resolution = temporal_resolution;
}