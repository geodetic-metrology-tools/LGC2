#ifndef TLS_SIMULATION
#define TLS_SIMULATION

#include <random>
#include <limits>
#include "TLSAlgorithm.h"
#include "TSimulationOutputFileWriter.h"
#include "TPointTransformer.h"
#include "TObservationGenerator.h"
#include "TLGCStatistic.h"


/*!
\ingroup LGCCalculation

\brief Class responsible for running the calculation in the simulation mode. It controls the whole process, distributing work into related classes.
*/
class TLSSimulation : public TLSAlgorithm{
public:

	/// Constructor
	TLSSimulation(TLGCData& data, int maxIter, std::shared_ptr<TSimulationOutputFileWriter> fResFileWriter);

	/*
	\brief Treat the SIMU option.

	One SIMU run treated here consists of:
	Calculating the 'measured values', runnning the algorithm to find the solutions (so far only LS),
	writing out results, reinitialising for next SIMU.
	*/
	virtual bool run(TLGCData& data, int fMaxIterations);

private:

	/*!@name Data*/
	//@{
	/// Contribution generator used for calculation of the 'measured values' 
	TObservationGenerator fSimObs; 

	/// Allow usual transformations between reference frame
	TPointTransformer fPointTransformer; 

	/// Reference to the data
	TLGCData& fData;

	/// Pointer to the specific simulated output file writer
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter;

	/// Random numbers generator 
	std::default_random_engine engine;  // FRK 16/11 Remis comme avant!!!
//	std::tr1::ranlux3_01 engine;

	/// Convergence criterion
	TReal  fConvCriteria; 

	/// Maximum number of iteration of LS
	int    fMaxIterations; 

	/// Summaries of points across the simulations
	std::list<TSimPointSummary> fPointSummaries; 
	/// Summaries of frame across the simulations
	std::list<TSimFrameSummary> fFrameSummaries; 

	//@}

	/*!@name Methods*/
	//@{
	/// Simulate all measurements
	void	simulateValues();

	/// Run the simulation process
	bool	processSimCalculation();

	void	getPLR3DSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TPLR3D>& plr3D);
	void	getANGLSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TANGL>& angl);
	void	getZENDSimValues(const TTSTN& station, std::vector<TZEND>& zend);
	void	getDISTSimValues(const TTSTN& station, std::vector<TLINE>& dist);
	void	getDHORSimValues(const TTSTN& station, std::vector<TLINE>& dhor);
	void	getECTHSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TECTH>& ecth);
	void	getECDIRSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TECDIR>& ecdir);

	void	getDLEVSimValues(const TLEVEL& levelST, std::vector<TDLEV>& dlev);
	void	getDVERSimValues(std::vector<TDVER>& dver);
	void	getDSPTSimValues(const TEDM& edmST, std::vector<TDSPT>& dspt);
	void    getECHOSimValues(const TECHOROM& echoROM, std::vector<TECHO>& echo);
	void    getECVESimValues(const TECVEROM& ecveROM, std::vector<TECVE>& ecve);
	void	getECSPSimValues(const TECSPROM& ecspROM, std::vector<TECSP>& ecsp);
	void    getORIESimValues(const TORIEROM& orieROM, std::vector<TORIE>& orie);
	/*DHOR made in DLEV measurement, different from the DHOR obs.*/
	void	getHorDistSimValues(const LGCAdjustablePoint* referencePoint, TDLEV::TDHOR& dhorlevel);

	void	getUVDSimValues(TCAM& camera);
	void	getUVECSimValues(TCAM& camera);

	/// Return a biased value (correct measurement + small error)
	TReal   getSimulatedValue(const TReal val, const TReal sigma);

	/// Reinitialised simulated observation for the next iteration/ simulation
	void updateResValues();
	//@}
};

#endif
