#pragma once

class RpmMeasureBase
{
public:

	struct RpmData {
		float cycle_time;
		int curr_temporal_pos;
	};

	inline RpmMeasureBase(int temporal_resolution) {
		this->temporal_resolution = temporal_resolution;
	};

	inline virtual ~RpmMeasureBase() {};

	virtual void initialize() = 0;
	virtual RpmData getRpmData() = 0;

protected:
	int temporal_resolution;

};
