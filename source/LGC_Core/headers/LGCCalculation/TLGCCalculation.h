/*
© Copyright CERN 2000-2019. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLGC_CALCULATION
#define TLGC_CALCULATION


//STL
#include <memory>
//SURVEYLIB
#include <Behavior.h>
//LGC
#include <TSimulationOutputFileWriter.h>

class TLSResultsMatrices;
/*!
	\ingroup LGCCalculation
	\brief Class responsible for the LGC calculation process.
*/
class TLGCCalculation{

	public:
	/*!
		\brief Constructor.

		@param dat[in] LGC project to be processed.
	*/
	TLGCCalculation(std::shared_ptr<TLGCData> dat, int maxIterations = MAX_ITERATIONS);

	/*!
		\brief Calculates results, based on the keywords used (normal calculation, simulation (SIMU), free network adjustmen (LIBR), fixed points and frames (ALLFIXED), etc.)
	
    	\note The fact that the Writer is passed to the function is not the final state, it should be treated in the SIMULATION calculation itself! The problem comes
		      from the initialization of TAStreamFormatter in the TLGCApp. This should change when the Stream formatter SurveyLib issue is resolved!!!

		@param dat[in] fileWriter The writer is only used for SIMULATION, where we output values after each simulation run. 
	*/
	Behavior computeResults(std::shared_ptr<TSimulationOutputFileWriter> fileWriter);
	// try to find a solution with armijo stepsize regularization starting from several randomly sampled initial values
	void tryArmijoSampling();

	/// Returns the project data.
	const TLGCData& getData(){ return *fData.get();}

	/// Returns the results matrices.
	TLSResultsMatrices& getResultMtr() const { return *fResultsMtr; }

	private:
    
    // Initialise the observation summaries in measurements
		void initialiseObsSummaries();

	// contains references: not assignable
	TLGCCalculation& operator=(const TLGCCalculation&);

	/// Project data
	std::shared_ptr<TLGCData> fData;

	int			fMaxIterations; /*!< maximum of iterations that can be carried out */
	//@}

	/*!
	\brief Pointer to the result matrices
	\note  needed to be able to write a deform file
	*/
	TLSResultsMatrices* fResultsMtr;
};

#endif