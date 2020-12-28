#pragma once

#include "rpm_measure_base.hpp"
#include "globe.hpp"

class RpmMeasureSim : public RpmMeasureBase
{
public:
	RpmMeasureSim();
	~RpmMeasureSim();

	virtual void initialize(Globe& globe);
	virtual RpmData getRpmData();

private:

	std::chrono::duration<float, std::milli> m_cycle_time;
	int m_curr_tick;
	int m_max_tick;
};

