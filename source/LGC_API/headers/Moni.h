
#ifndef MONI
#define MONI


#include <Eigen/Dense>
#include <string>
#include <memory>

#ifndef __linux__

#define DECLSPEC __declspec(dllexport)
	#define DLLAPI __cdecl
#else
	#define DECLSPEC
#endif

class Moni
{
public:
	DECLSPEC Moni(std::string inputFilePath);
	DECLSPEC ~Moni();
	DECLSPEC void updateMeas(std::string id, Eigen::VectorXd measurementVector);
	// triggering the adjustment claculation
	DECLSPEC bool adjust();
	// for checking the estimation status
	DECLSPEC bool getStatus();
	// get estimate of parameter
	DECLSPEC Eigen::VectorXd getEstimate(std::string);
	// get estimate of parameter in subframe
	DECLSPEC Eigen::VectorXd getEstimate(std::string, std::string);
	// get diagonal elements of covariances of the estimated parameters
	DECLSPEC Eigen::VectorXd getEstimatePrec(std::string);
	// get diagonal elements of covariances of the estimated parameters in a subframe, in a first version only Root frame is allowed
	DECLSPEC Eigen::VectorXd getEstimatePrec(std::string pointName, std::string frameName);
	// get estimated residual
	DECLSPEC Eigen::VectorXd getEstimateResidual(std::string obsName);
	// get Meas IDs
	DECLSPEC std::vector<std::string> getECWSMeasIds();
	// get measurement
	DECLSPEC Eigen::VectorXd getMeas(std::string id);
	// get the sigma0 after adjustment
	DECLSPEC double getSigma0();
	
private:
	class MoniImpl;
	std::unique_ptr<MoniImpl> pimpl_;
};



#endif
