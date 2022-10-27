// STL
#include <random>

#include <TLGCData.h>

#include "TVAbstractAlgorithm.h"

class TMonitor
{
public:
	// constructor
	TMonitor();
	// access to measurements, for updating observations
	void manipulate_ECWS_measurements();
	// triggering the adjustment claculation
	void adjust();

private:
	// LGC adjustment algorithm used by adjust method
	std::unique_ptr<TVAbstractAlgorithm> algorithm;
	// containing measurement configuration, observations, estimates
	std::shared_ptr<TLGCData> project;
	/// Random numbers generator
	std::ranlux48 engine;
	void initialize();
};