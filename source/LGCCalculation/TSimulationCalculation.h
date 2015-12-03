#ifndef TSIMULATION_CALCULATION
#define TSIMULATION_CALCULATION

#include <random>
#include <limits>
#include "TALSComputer.h"
#include "TResSimFileWriter.h"
#include "TContributionsGenerator.h"
#include "TLGCStatistic.h"

class TLSResultsMatricesExtractor;
class TLSInputMatricesFiller;
class TLSInputMatrices;
class TLGCData;

/*! 
	\ingroup LGCCalculation

	\brief Class responsible for running the calculation in the simulation mode. It controls the whole process, distributing work into related classes.
*/
class TSimulationCalculation{
	public:

		/// Constructor
		TSimulationCalculation(TLGCData& data, int maxIter, TReal convCrit);
		/*
			\brief Treat the SIMU option.

			One SIMU run treated here consists of:
			Calculating the 'measured values', runnning the algorithm to find the solutions (so far only LS),
			writing out results, reinitialising for next SIMU.
		*/
		bool	computeSimulatedResults(std::shared_ptr<TResSimFileWriter> fileWriter);

private:

		/*!@name Data*/
		//@{
			TContributionsGenerator fCg; //< Contribution generator used for calculation of the 'measured values' 
			TLGCData& fData;

			std::default_random_engine engine; //< Random numbers generator 

			TReal  fConvCriteria; //< Convergence criterion
			int    fMaxIterations; //< Maximum number of iteration of LS

			std::list<TSimPointSummary> fPointSummaries; //< Summaries of points across the simulations
			std::list<TSimFrameSummary> fFrameSummaries; //< Summaries of points across the simulations

		//@}

		/*!@name Methods*/
		//@{
			void	simulateValues();

			bool	processSimCalculation();

			void	getPLR3DSimValues(const TTSTN& station,const TTSTN::TROM& rom,std::vector<TPLR3D>& plr3D);
			void	getANGLSimValues(const TTSTN& station,const TTSTN::TROM& rom,std::vector<TANGL>& angl);
			void	getZENDSimValues(const TTSTN& station,std::vector<TZEND>& zend);
			void	getDISTSimValues(const TTSTN& station,std::vector<TLINE>& dist);
			void	getDHORSimValues(const TTSTN& station, std::vector<TLINE>& dhor);
			void	getECTHSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TECTH>& ecth);
			void	getECSPSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TECSP>& ecsp);

			void	getDLEVSimValues(const TLEVEL& levelST, std::vector<TDLEV>& dlev);
			void	getDVERSimValues(std::vector<TDVER>& dver);
			void	getDSPTSimValues(const TEDM& edmST, std::vector<TDSPT>& dspt);
			void    getECHOSimValues(const TECHOROM& echoROM, std::vector<TECHO>& echo);
			void    getECVESimValues(const TECVEROM& ecveROM, std::vector<TECVE>& ecve);
			void    getORIESimValues(const TORIEROM& orieROM, std::vector<TORIE>& orie);
			/*DHOR made in DLEV measurement, different from the DHOR obs.*/
			void	getHorDistSimValues(const TAdjustablePoint* referencePoint, TDLEV::TDHOR& dhorlevel);

			void	getUVDSimValues(TCAM& camera);
			void	getUVECSimValues(TCAM& camera);

			TReal   getSimulatedValue(const TReal val, const TReal sigma);

			void updateResValues();
		//@}
};

#endif
