#pragma once

#include "rpm_measure_base.hpp"

class RpmMeasureHall : public RpmMeasureBase
{
public:
	RpmMeasureHall();
	~RpmMeasureHall();

	virtual void initialize(Globe& globe);
	virtual RpmData getRpmData();

private:

};