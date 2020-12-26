#pragma once

class Globe;

class RpmMeasureBase
{
public:

	struct RpmData {
		float cycle_time;
		int curr_temporal_pos;
	};

	RpmMeasureBase();
	virtual ~RpmMeasureBase();

	virtual void initialize(Globe &globe);
	virtual RpmData getRpmData() = 0;

protected:
	int m_temporal_resolution;

};