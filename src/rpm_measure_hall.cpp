#include "rpm_measure_hall.hpp"

RpmMeasureHall::RpmMeasureHall(int temporal_resolution)
	: RpmMeasureBase(temporal_resolution)
{
}

RpmMeasureHall::~RpmMeasureHall()
{
}

void RpmMeasureHall::initialize()
{
}

RpmMeasureHall::RpmData RpmMeasureHall::getRpmData()
{
	return {};
}
