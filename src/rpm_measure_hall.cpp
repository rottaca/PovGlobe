#include "rpm_measure_hall.hpp"

RpmMeasureHall::RpmMeasureHall()
{
}

RpmMeasureHall::~RpmMeasureHall()
{
}

void RpmMeasureHall::initialize(Globe& globe)
{
	RpmMeasureBase::initialize(globe);
}

RpmMeasureHall::RpmData RpmMeasureHall::getRpmData()
{
	return {};
}
