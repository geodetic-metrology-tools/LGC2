
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

// structures for water and wire round of measurements results.
struct waterRom
{
	std::string romName = "";
	double estimate = 0;
	double prec = 0;
	// basic constuctor
	waterRom(std::string name, double level, double sigma) : romName(name), estimate(level), prec(sigma) {}
};

struct wireRom
{
	std::string romName = "";
	// containing values of Dx,Dz,bearing,slope,sag in this order
	Eigen::VectorXd estimate = Eigen::VectorXd::Zero(5);
	// sigmas in the same order
	Eigen::VectorXd prec = Eigen::VectorXd::Zero(5);
	// basic constructor
	wireRom(std::string name, Eigen::VectorXd vals, Eigen::VectorXd sigmas) : romName(name), estimate(vals), prec(sigmas) {}
};


class Moni
{
public:
	DECLSPEC Moni(std::string inputFilePath);
	DECLSPEC ~Moni();
	//DECLSPEC void Moni::writeJsonFile(TLGCData const *const dat, const std::string &outputFileLocation);
	// write the results from the current estimation in a json file with timestamp

#if USE_SERIALIZER
	DECLSPEC void writeResultFile();
#endif
	// write lgc input file with current measurements with timestamp
	DECLSPEC void writeLGCInputFile();
	// set activtaionStatus
	DECLSPEC void setActivationStatus(std::string id, bool status);
	// updating observations
	DECLSPEC void updateMeas(std::string id, Eigen::VectorXd measurementVector);
	// changing observation Sigma
	DECLSPEC void setObsSigma(std::string id, Eigen::VectorXd sigma);
	// changing the value of a fixed frame parameter
	DECLSPEC void setFixedFrameParameter(std::string frameName, int idx, double val);
	DECLSPEC void setFixedPointParameter(std::string pointName, int idx, double val);
	// "freezing" of free parameters
	DECLSPEC void freezeFrameParameter(std::string frameName, int idx, double val);
	// "unfreezing" of free parameters
	DECLSPEC void unfreezeFrameParameter(std::string frameName, int idx);
	// "freezing" of free parameters
	DECLSPEC void freezePointParameter(std::string pointName, int idx, double val);
	// "unfreezing" of free parameters
	DECLSPEC void unfreezePointParameter(std::string pointName, int idx);

	// triggering the adjustment calculation
	DECLSPEC bool adjust();
	// for checking the estimation status
	DECLSPEC bool getStatus();
	// get estimate of point
	DECLSPEC Eigen::VectorXd getPointEstimate(std::string);
	// get estimate of point in subframe
	DECLSPEC Eigen::VectorXd getPointEstimate(std::string, std::string);
	// get estimate of frame
	DECLSPEC Eigen::VectorXd getFrameEstimate(std::string);
	DECLSPEC Eigen::VectorXd getFrameEstimatePrec(std::string);
	// get diagonal elements of covariances of the estimated parameters
	DECLSPEC Eigen::VectorXd getPointEstimatePrec(std::string);
	// get diagonal elements of covariances of the estimated parameters in a subframe, in a first version only Root frame is allowed
	DECLSPEC Eigen::VectorXd getPointEstimatePrec(std::string pointName, std::string frameName);
	// get Meas IDs
	DECLSPEC std::vector<std::string> getECWSMeasIds();
	// get measurement
	DECLSPEC Eigen::VectorXd getMeas(std::string id);
	// get estimated residual
	DECLSPEC Eigen::VectorXd getEstimateResidual(std::string obsName);
	// get "calculated" measurement = obs+residual
	DECLSPEC Eigen::VectorXd getCalcMeas(std::string obsname);
	// get observation sigma
	DECLSPEC Eigen::VectorXd getObsSigma(std::string obsname);
	// get the sigma0 after adjustment
	DECLSPEC double getSigma0();
	DECLSPEC waterRom getECWSData(std::string ecwsRomName);
	DECLSPEC wireRom getECWIData(std::string ecwiRomName);
	
private:
	class MoniImpl;
	std::unique_ptr<MoniImpl> pimpl_;
};



#endif
