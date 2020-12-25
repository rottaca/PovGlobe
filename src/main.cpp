

#ifdef _WIN32 
#include "rpm_measure_sim.hpp"
using RpmMeasure = RpmMeasureSim;
#elif __linux__
#include "rpm_measure_hall.hpp"
using RpmMeasure = RpmMeasureHall;
#endif

int main() {
	
	RpmMeasure rpm(100);


}