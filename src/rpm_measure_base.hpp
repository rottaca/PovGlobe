#pragma once

class RpmMeasureBase
{
public:

	struct RpmData {
		float cycle_time;
		int curr_temporal_pos;
	};

	RpmMeasureBase();
	virtual ~RpmMeasureBase();

	virtual void initialize(int temporal_resolution);
	virtual RpmData getRpmData() = 0;

protected:
	int m_temporal_resolution;

};