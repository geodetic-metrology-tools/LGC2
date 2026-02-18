/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
#include <TContributionsGenerator.h>
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
	TContributionsGenerator	fCGenerator; 

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

	void updatePLR3DSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void updateANGLSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void updateZENDSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void	updateDISTSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void	updateDHORSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void	updateECTHSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void	updateECDIRSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	//for a TSTN defined in a subframe
	void	updateANGLSimValuesInFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void	updateZENDSimValuesInFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);
	void	updateDISTSimValuesInFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station);

	void	updateDLEVSimValues(const TLEVEL& levelST, std::list<TDLEV>& dlev);
	void	updateDVERSimValues(std::list<TDVER>& dver);
	void	updateDSPTSimValues(const TEDM& edmST, std::list<TDSPT>& dspt);
	void    updateECHOSimValues(const TECHOROM& echoROM, std::list<TECHO>& echo);
	void    updateECVESimValues(const TECVEROM& ecveROM, std::list<TECVE>& ecve);
	void	updateECSPSimValues(const TECSPROM& ecspROM, std::list<TECSP>& ecsp);
	void    updateORIESimValues(const TORIEROM& orieROM, std::list<TORIE>& orie);
	/*DHOR made in DLEV measurement, different from the DHOR obs.*/
	void	updateHorDistSimValues(const LGCAdjustablePoint* referencePoint, TDLEV::TDHOR& dhorlevel);

	void	updateUVDSimValues(TCAM& camera);
	void	updateUVECSimValues(TCAM& camera);

	void    updateINCLYSimValues(const TINCLYROM& inclyROM, std::list<TINCLY>& incly);
	void    updateROLLYSimValues(const TROLLYROM& rollyROM, std::list<TROLLY>& rolly);

	void    updateECWSSimValues(const TECWSROM& ecwsROM, std::list<TECWS>& ecws);
	void    updateOBSXYZSimValues(std::list<TOBSXYZ>& obsxyz);

	void    updateECWISimValues(const TECWIROM& ecwiROM, std::list<TECWI>& ecws);

	/// Return a biased value (correct measurement + small error)
	TReal   getSimulatedValue(const TReal val, const TReal sigma);

	/// Reinitialised simulated observation for the next iteration/ simulation
	void updateResValues();
	//@}
};

#endif
