#pragma once

#include "rpm_measure_base.hpp"

class RpmMeasureHall : public RpmMeasureBase
{
public:
	RpmMeasureHall(int temporal_resolution);
	~RpmMeasureHall();

	virtual void initialize();
	virtual RpmData getRpmData();

private:

};