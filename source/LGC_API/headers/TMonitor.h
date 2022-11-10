// STL
#include <TLGCData.h>

#include "TVAbstractAlgorithm.h"

class TMonitor
{
public:
	// constructor
	TMonitor();
	// access to measurements, for updating observations
	void updateMeas(std::string, double);
	// get Meas IDs
	std::vector<std::string> getMeasIds();
	// triggering the adjustment claculation
	void adjust();
	// containing maps to measurement object references
	// should probably be private. is public to make the previous measurements available in the mockup
	std::unordered_map<std::string, TECWS &> ecws;

private:
	// LGC adjustment algorithm used by adjust method
	std::unique_ptr<TVAbstractAlgorithm> algorithm;
	// containing measurement configuration, observations, estimates
	std::shared_ptr<TLGCData> project;
	void initialize();
	// create a map for easy access to references to measurement objects without the need to go thrugh the frame tree
	void createMeasurementReferences();
};