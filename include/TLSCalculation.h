////////////////////////////////////////////////////////////////////
// LSCalculation.h
/*!
 Class managing the least squares calculation of an LGC project
 from input matrices preparation to final results extraction

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////


#ifndef SU_LS_CALCULATION
#define SU_LS_CALCULATION

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
class TLSParametricMtdComputerAdapter;
class TLSInputMatricesAdapter;
class TLSResultsMatricesAdapter;

#include "TLGCDataSet.h"
#include "LSCalcDataSet.h"
#include "TLSInputMatricesFiller.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLGCCalcparams.h"

#include <random>


struct limits{TReal s0PostUpLimit; TReal s0PostLoLimit;};

//!Class managing the ls calculation from input matrices preparation to the final results extraction*/
class TLSCalculation{ 


	
public:

	/*!@name Constructors / Destructor*/
	//@{
		/*!Default Constructor*/
		TLSCalculation();

		/*! Constructor
		\param calcParams Calculation parameters used to compute a solution */
		explicit TLSCalculation(const TLGCCalcParams& calcParams);

		//!Destructor
		virtual ~TLSCalculation();
	//@}

	
	/*! Computes the Least Squares solution of a survey measurement network.
		Identifies the type of computation to carry out from the input data and
		options. 
		@return a true boolean if a solution was found*/
	virtual bool	computeLSResults(LSCalcDataSet& dataSet,  TLGCDataSet& data, TLGCCalcParams* params);


	/*!@return the error*/
	virtual string	getError() const;

	/*!turn to true the boolean indicating that a geodetic system is used*/
	virtual void  useGeodSys() {fGeodSys = true;}



private:

	/*! Computes the solution for a survey network from data in the LSCalcDataSet 
	    and stores the results
	\return a boolean identifying if a solution was found)*/
	bool	processCalculation(	LSCalcDataSet& calcDS);
	
	// Computes the solution for an all-poinst fixed situation
	bool computeAllPointsFixedResults(LSCalcDataSet& calcDS);

	/*! Computes the a series of simulated Least Squares solutions of a survey measurement network.
		@return a true boolean if computations ran correctly */
	bool	computeSimulatedLSResults(LSCalcDataSet& dataSet,  TLGCDataSet& data, TLGCCalcParams* params);

	/*! Computes the solution for a simulated survey network from data in the LSCalcDataSet 
	    and stores the results
	\return a true boolean if a solution was found*/
	bool	processSimCalculation(LSCalcDataSet& calcDS,  TLGCDataSet& data);

	/*! Iterate to the solution of the least squares calculation */
	bool	iterate2Solution(LSCalcDataSet& calcDS,
							 TLSInputMatricesFiller* inpMtrFiller,
							 TLSInputMatricesAdapter* inputMtr,
							 TLSParametricMtdComputerAdapter* computer,
							 TLSResultsMatricesExtractor* extractor);

	/* Calculate the least squares residuals and the Variance/Covariance matrix */
	void	calcResiduAndVarCovMatrice(	LSCalcDataSet& calcDS,
										const TLSInputMatricesAdapter* inputMtr);

	//! invert the variance convariance matrix 
	void	invertMatriceVarCoVar(LSCalcDataSet& calcDS);

	//! extract and store the variance covariance values for the calculated parameters
	void	extractVarCovarParams(TLSResultsMatricesExtractor* extractor);

	//! calculate the fault detection parameters for the observations
	void	calcFautParams(	TLSResultsMatricesAdapter* inputMtr,
							TLSResultsMatricesExtractor* extractor);

	//! calculate the relative errors between pairs of points
	void	calcRelErrorParams(	TLSResultsMatricesExtractor* extractor);

	//! calculate the hypothesis testing limits for the sigma zero a posteriori
	struct limits	calcSigmaZeroLimits(const int nbObs, const int nbUnk, const TReal sigmaZero2);

	/*!@name Methods for the simulation of measurement values for the observations */
	//@{
		/*! Generate simulated values for the calculation data set*/
		virtual void		simulateValues(LSCalcDataSet&);


		/*! Simulated Values for the polar 3d observations */
		void		getPolarSimValues(LSCalcDataSet&);
		/*! Simulated Values for the hor. angle observations */
		void		getHorAngSimValues(LSCalcDataSet&);
		/*! Simulated Values for the zen. dist. observations */
		void		getZenDistSimValues(LSCalcDataSet&);

		template <typename T>
		void		getSpaDistSimValues(typename list<TLSCalcSpatialDistObservation<T> >::iterator iter,
										typename list<TLSCalcSpatialDistObservation<T> >::iterator iterEnd);

		/*! Simulated Values for the hor. dist. observations */
		void		getHorDistSimValues(LSCalcDataSet&);
		/*! Simulated Values for the vert. dist. observations */
		void		getVertDistSimValues(LSCalcDataSet&);
		/*! Simulated Values for the vert. dist. observations */
		void		getLevelObsSimValues(LSCalcDataSet&);

		/*! Simulated Values for the offset to ver. line observations */
		void		getOffsetToVerLineSimValues(LSCalcDataSet&);
		/*! Simulated Values for the offset to spa. line observations */
		void		getOffsetToSpaLineSimValues(LSCalcDataSet&);
		/*! Simulated Values for the offset to ver. plane observations */
		void		getOffsetToVerPlaneSimValues(LSCalcDataSet&);
		/*! Simulated Values for the offset to theo. plane observations */
		void		getOffsetToTheoPlaneSimValues(LSCalcDataSet&);

		/*! Simulated Values for the gyro orientation observations */
		void		getGyroOrieSimValues(LSCalcDataSet&);

		/*! Returns the simulated value for a value and its sigma */
		TReal		getSimulatedValue(const TReal, const TReal);
	//@}



private:

	TLSResultsMatricesAdapter*		fResultsMatrices; /*!< pointer to the results matrices */ 
	int						fMaxIterations; /*!< maximum of iterations that can be carried out */
	TReal					fConvCriteria; /*!< convergence criteria that has to be exceeded to stop the iterative process */
	int						fNumberOfMadeIterations; /*!< indicates how many iteration have been carried out */
	string					fError; /*!< error from the calculation */
	
	TLGCCalcParams			fCalcParams;

	bool					fGeodSys; /*!< boolean indicating if the used coordinate system is geodetic or not */

	bool					fS0APosterioriVariances;

	//TReal					fUniformLastValue;
	bool				fIsSimulation;
	//GSLRandomEngine*	rndMM;
    std::tr1::ranlux3_01 fRndGenerator;    


};
#endif
