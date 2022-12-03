
#ifndef MONI
#define MONI


#include <Eigen/Dense>
#include <string>

class Moni
{
public:
	Moni(std::string inputFilePath);
	~Moni();
	void updateMeas(std::string id, Eigen::VectorXd measurementVector);
	// triggering the adjustment claculation
	void adjust();
	// get estimate of parameter
	Eigen::VectorXd getEstimate(std::string);
	// get diagonal elements of covariances of the estimated parameters
	Eigen::VectorXd getEstimateCovar(std::string);
	// get Meas IDs
	std::vector<std::string> getECWSMeasIds();
	// get measurement
	Eigen::VectorXd getMeas(std::string id);
	// get the sigma0 after adjustment
	double getSigma0();
	
private:
	class MoniImpl;
	std::unique_ptr<MoniImpl> pimpl_;
};



#endif
