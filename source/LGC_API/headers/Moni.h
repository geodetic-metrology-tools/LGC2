
#ifndef MONI
#define MONI

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

#ifndef __linux__

#	define DECLSPEC __declspec(dllexport)
#	define DLLAPI __cdecl
#else
#	define DECLSPEC
#endif

// structures for water and wire round of measurements results.
struct waterRom
{
	std::string romName = "";
	double estimate = 0;
	double prec = 0;
	// basic constuctor
	waterRom(const std::string &name, double level, double sigma) : romName(name), estimate(level), prec(sigma) {}
};

struct wireRom
{
	std::string romName = "";
	// containing values of Dx,Dz,bearing,slope,sag in this order
	Eigen::VectorXd estimate = Eigen::VectorXd::Zero(5);
	// sigmas in the same order
	Eigen::VectorXd prec = Eigen::VectorXd::Zero(5);
	// basic constructor
	wireRom(const std::string &name, const Eigen::VectorXd &vals, const Eigen::VectorXd &sigmas) : romName(name), estimate(vals), prec(sigmas) {}
};

class Moni
{
public:
	DECLSPEC Moni(const std::string &inputFilePath);
	DECLSPEC ~Moni();
	// reset the Monitoring object to a freshly constructed object
	DECLSPEC void reset(); 

	// DECLSPEC void Moni::writeJsonFile(TLGCData const *const dat, const std::string &outputFileLocation);
	//  write the results from the current estimation in a json file with timestamp

#if USE_SERIALIZER
	DECLSPEC void writeResultFile();
#endif
	// write lgc input file with current measurements with timestamp
	DECLSPEC void writeLGCInputFile();
	// set activtaionStatus
	DECLSPEC void setActivationStatus(const std::string &id, bool status);
	// updating observations
	DECLSPEC void updateMeas(const std::string &id, const Eigen::VectorXd &measurementVector);
	// changing observation Sigma
	DECLSPEC void setObsSigma(const std::string &id, const Eigen::VectorXd &sigma);
	// changing the value of a fixed frame parameter
	DECLSPEC void setFixedFrameParameter(const std::string &frameName, int idx, double val);
	DECLSPEC void setFixedPointParameter(const std::string &pointName, int idx, double val);
	DECLSPEC void setFixedSagParameter(const std::string &sagName, int idx, double val);
	// freeze methods
	DECLSPEC void freezeFrameParameter(const std::string &frameName, int idx, double val);
	DECLSPEC void freezePointParameter(const std::string &pointName, int idx, double val);
	DECLSPEC void freezeSagParameter(const std::string &sagName, int idx, double val);
	// unfreeze methods
	DECLSPEC void unfreezeFrameParameter(const std::string &frameName, int idx);
	DECLSPEC void unfreezePointParameter(const std::string &pointName, int idx);
	DECLSPEC void unfreezeSagParameter(const std::string &sagName, int idx);

	// triggering the adjustment calculation
	DECLSPEC bool adjust();
	// for checking the estimation status
	DECLSPEC bool getStatus();
	// get estimates
	DECLSPEC Eigen::VectorXd getPointEstimate(const std::string &);
	DECLSPEC Eigen::VectorXd getPointEstimate(const std::string &, const std::string &);
	DECLSPEC Eigen::VectorXd getFrameEstimate(const std::string &);
	DECLSPEC Eigen::VectorXd getSagEstimate(const std::string &);
	// get precisions of estimations
	DECLSPEC Eigen::VectorXd getPointEstimatePrec(const std::string &);
	DECLSPEC Eigen::VectorXd getPointEstimatePrec(const std::string &pointName, const std::string &frameName);
	DECLSPEC Eigen::VectorXd getFrameEstimatePrec(const std::string &);
	DECLSPEC Eigen::VectorXd getSagEstimatePrec(const std::string &);
	// get Meas IDs
	DECLSPEC std::vector<std::string> getECWSMeasIds();
	// get measurement
	DECLSPEC Eigen::VectorXd getMeas(const std::string &id);
	// get estimated residual
	DECLSPEC Eigen::VectorXd getEstimateResidual(const std::string &obsName);
	// get "calculated" measurement = obs+residual
	DECLSPEC Eigen::VectorXd getCalcMeas(const std::string &obsname);
	// get observation sigma
	DECLSPEC Eigen::VectorXd getObsSigma(const std::string &obsname);
	// get the sigma0 after adjustment
	DECLSPEC double getSigma0();
	// transform coordinates/directions
	DECLSPEC Eigen::Vector3d transformCoordinates(const Eigen::Vector3d& coord, const std::string& from, const std::string& to);
	DECLSPEC Eigen::Vector3d transformDirection(const Eigen::Vector3d& dir, const std::string& from, const std::string& to);
	DECLSPEC waterRom getECWSData(const std::string &ecwsRomName);
	DECLSPEC wireRom getECWIData(const std::string &ecwiRomName);

private:
	class MoniImpl;
	std::string fFilePath; // needed for the reset method
	std::unique_ptr<MoniImpl> pimpl_;
};

#endif
