#include "rpm_measure_hall.hpp"

RpmMeasureHall::RpmMeasureHall()
{
}

RpmMeasureHall::~RpmMeasureHall()
{
}

void RpmMeasureHall::initialize(int temporal_resolution)
{
	RpmMeasureBase::initialize(temporal_resolution);
}

RpmMeasureHall::RpmData RpmMeasureHall::getRpmData()
{
	return {};
}
