
#ifndef MONI
#define MONI


#include <Eigen/Dense>
#include <string>
#include <memory>

class Moni
{
public:
	__declspec(dllexport) Moni(std::string inputFilePath);
	__declspec(dllexport) ~Moni();
	__declspec(dllexport) void updateMeas(std::string id, Eigen::VectorXd measurementVector);
	// triggering the adjustment claculation
	__declspec(dllexport) void adjust();
	// get estimate of parameter
	__declspec(dllexport) Eigen::VectorXd getEstimate(std::string);
	// get estimate of parameter in subframe
	__declspec(dllexport) Eigen::VectorXd getEstimate(std::string, std::string);
	// get diagonal elements of covariances of the estimated parameters
	__declspec(dllexport) Eigen::VectorXd getEstimateCovar(std::string);
	// get diagonal elements of covariances of the estimated parameters in a subframe, in a first version only Root frame is allowed
	__declspec(dllexport) Eigen::VectorXd getEstimateCovar(std::string pointName, std::string frameName);
	// get Meas IDs
	__declspec(dllexport) std::vector<std::string> getECWSMeasIds();
	// get measurement
	__declspec(dllexport) Eigen::VectorXd getMeas(std::string id);
	// get the sigma0 after adjustment
	__declspec(dllexport) double getSigma0();
	
private:
	class MoniImpl;
	std::unique_ptr<MoniImpl> pimpl_;
};



#endif
