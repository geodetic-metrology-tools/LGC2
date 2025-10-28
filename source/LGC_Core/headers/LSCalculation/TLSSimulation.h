/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_SIMULATION
#define TLS_SIMULATION


//STL
#include <random>
#include <limits>
//SURVEYLIB
#include <Behavior.h>
//LGC
#include <TLSAlgorithm.h>
#include <TSimulationOutputFileWriter.h>
#include <TPointTransformer.h>
#include <TObservationGenerator.h>
#include <TLGCStatistic.h>


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
	virtual Behavior run(TLGCData& data, int fMaxIterations);

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
	std::ranlux48 engine;

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

	void	getPLR3DSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::list<TPLR3D>& plr3D);
	void	getANGLSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::list<TANGL>& angl);
	void	getZENDSimValues(const TTSTN& station, std::list<TZEND>& zend);
	void	getDISTSimValues(const TTSTN& station, std::list<TLINE>& dist);
	void	getDHORSimValues(const TTSTN& station, std::list<TLINE>& dhor);
	void	getECTHSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::list<TECTH>& ecth);
	void	getECDIRSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::list<TECDIR>& ecdir);
	//for a TSTN defined in a subframe
	void	getANGLSimValuesInFrame(const TTSTN& station, const TTSTN::TROM& rom, std::list<TANGL>& angl);
	void	getZENDSimValuesInFrame(const TTSTN& station, std::list<TZEND>& zend);
	void	getDISTSimValuesInFrame(const TTSTN& station, std::list<TLINE>& dist);

	void	getDLEVSimValues(const TLEVEL& levelST, std::list<TDLEV>& dlev);
	void	getDVERSimValues(std::list<TDVER>& dver);
	void	getDSPTSimValues(const TEDM& edmST, std::list<TDSPT>& dspt);
	void    getECHOSimValues(const TECHOROM& echoROM, std::list<TECHO>& echo);
	void    getECVESimValues(const TECVEROM& ecveROM, std::list<TECVE>& ecve);
	void	getECSPSimValues(const TECSPROM& ecspROM, std::list<TECSP>& ecsp);
	void    getORIESimValues(const TORIEROM& orieROM, std::list<TORIE>& orie);
	/*DHOR made in DLEV measurement, different from the DHOR obs.*/
	void	getHorDistSimValues(const LGCAdjustablePoint* referencePoint, TDLEV::TDHOR& dhorlevel);

	void	getUVDSimValues(TCAM& camera);
	void	getUVECSimValues(TCAM& camera);

	void    getINCLYSimValues(const TINCLYROM& inclyROM, std::list<TINCLY>& incly);
	void    getROLLYSimValues(const TROLLYROM& rollyROM, std::list<TROLLY>& rolly);

	void    getECWSSimValues(const TECWSROM& ecwsROM, std::list<TECWS>& ecws);
	void    getOBSXYZSimValues(std::list<TOBSXYZ>& obsxyz);

	void    getECWISimValues(const TECWIROM& ecwiROM, std::list<TECWI>& ecws);

	/// Return a biased value (correct measurement + small error)
	TReal   getSimulatedValue(const TReal val, const TReal sigma);

	/// Reinitialised simulated observation for the next iteration/ simulation
	void updateResValues();
	//@}
};

#endif
