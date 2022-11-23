
#ifndef MONITOR
#define MONITOR


#include <Eigen/Dense>
#include <string>

class Monitor
{
public:
	Monitor(std::string inputFilePath);
	~Monitor();
	// access to measurements, for updating observations
	void updateMeas(std::string id, Eigen::VectorXd meas) { mMonitor->updateMeas(id, meas); }
	// get estimate of parameter
	Eigen::VectorXd getEstimate(std::string id) { return mMonitor->getEstimate(id); }
	// get measurement
	Eigen::VectorXd getMeas(std::string id) { return mMonitor->getMeas(id); }
	// get Meas IDs
	std::vector<std::string> getECWSMeasIds() { return mMonitor->getECWSMeasIds(); }
	// triggering the adjustment claculation
	void adjust();


	
private:
	class Monitor::TMonitorImpl;
	std::unique_ptr<Monitor::TMonitorImpl> mMonitor;
};



#endif
