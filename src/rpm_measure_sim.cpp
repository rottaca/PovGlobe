#include "rpm_measure_sim.hpp"
#include <cmath>

RpmMeasureSim::RpmMeasureSim(int temporal_resolution)
	: RpmMeasureBase(temporal_resolution)
	, m_cycle_time(1/10.f)
	, m_curr_tick(0)
	, m_max_tick(200)
{

}

RpmMeasureSim::~RpmMeasureSim()
{
}

void RpmMeasureSim::initialize()
{
	m_curr_tick = 0;
}

RpmMeasureSim::RpmData RpmMeasureSim::getRpmData()
{
	m_curr_tick = (m_curr_tick + 1) % m_max_tick;
	int curr_pos = std::round(static_cast<float>(m_curr_tick) * temporal_resolution / (m_max_tick - 1));
	return {m_cycle_time, curr_pos};

}
