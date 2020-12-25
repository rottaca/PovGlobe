#pragma once

#include "rpm_measure_base.hpp"

class RpmMeasureSim : public RpmMeasureBase
{
public:
	RpmMeasureSim();
	~RpmMeasureSim();

	virtual void initialize(int temporal_resolution);
	virtual RpmData getRpmData();

private:

	float m_cycle_time;
	int m_curr_tick;
	int m_max_tick;
};

