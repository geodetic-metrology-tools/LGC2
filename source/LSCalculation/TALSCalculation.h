#ifndef ALS_CALCULATION
#define ALS_CALCULATION

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
///////////////////////////////////////////////////// 
class TLSResultsMatricesExtractor;

#include <iostream>
using namespace std;

#include "lsalgo/UEOIndices.h"
#include "lsalgo/TALSComputer.h"
#include "lsalgo/TLSInputMatrices.h"
#include "lsalgo/TLSResultsMatrices.h"
#include "TLSInputMatricesFiller.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLGCCalcparams.h"
#include "TLGCData.h"
/* Replace by TLGCData
#include "TLGCDataSet.h"
#include "LSCalcDataSet.h"
#include "ObservationSet.h"
#include "TLSCalcObservationMaker.h"
*/

#include <string>
#include <random>

struct limits{TReal s0PostUpLimit; TReal s0PostLoLimit;};


class TALSCalculation
{
public:
    /*!@name Constructors / Destructors*/
    //@{
        /*!Default Constructor*/
	    TALSCalculation();
        /*!Constructor
        \param calcParams Calculation parameters used to compute a solution */
	    //explicit TALSCalculation(const TLGCCalcParams& calcParams);
		explicit TALSCalculation(const TLGCCalcParams& calcParams);

        //!Destructor
	    virtual ~TALSCalculation();
    //@}

	/*! Compute the Least Squares solution of a survey measurement network.
    Identifies the type of computation to carry out from the input data and options.
    @return a true boolean if a solution was found */
	//virtual bool computeLSResults(const ObservationSet& obsSet, LSCalcDataSet& dataSet, TLGCDataSet& data, TLGCCalcParams* params);
		virtual bool computeLSResults(TLGCData data, TLGCCalcParams* params);
	
	//Compute the solution for a survey network from data in LSCalcDataSet and store the results
    bool processCalculation( /*LSCalcDataSet& calcDS*/ TLGCData& calcDS );

	/* iterate to the solution of the least square calculation */
	bool iterate2Solution(/*LSCalcDataSet& calcDS,*/
									TLGCData& calcDS,
									TLSInputMatricesFiller* inpMtrFiller,
									TLSInputMatrices* inputMtr,
									TALSCalculation* computer,
									TLSResultsMatricesExtractor* extractor);

	/* Calculate the least squares residuals and the variance covariance matrix */
	void calcResiduAndVarCovMatrice(/*LSCalcDataSet& calcDS,*/
							  TLGCData calcDS,
							  const TLSInputMatrices* inputMtr, 
							  TLSResultsMatricesExtractor* /*,
							  TLSParametricMtdComputer* */) = 0;

	/* Calculate the least squares residuals and the variance covariance matrix */
	void calcResiduAndVarCovMatrice(/*LSCalcDataSet& calcDS,*/ 
							  TLGCData calcDS,
							  const TLSInputMatrices* inputMtr,
							  TLSResultsMatrices* rm,
							  TLSResultsMatricesExtractor* /*,
							  TLSParametricMtdComputer* */) = 0;

	//! Compute the results matrices
	virtual bool computeResultsMtrs(TLSInputMatrices*, TLSResultsMatrices*) = 0;

	virtual string getError() const {return fError;}

protected:


	//! invert the variance covariance matrix
	void invertMatriceCoVar(TLGCData& calcDS);

	//! extract and store the variance covariance values for the calculated parameters
	void extractVarCovarParams(TLSResultsMatricesExtractor* extractor);

	//! calculate the fault detection parameters for the observations
	void calcFautParams( TLSInputMatrices* inputMtr, TLSResultsMatricesExtractor* extractor);

	//! calculate relative errors between pairs of points
	void calcRelErrorParams( TLSResultsMatricesExtractor* extractor);

	//! calculate the hypothesis testing limits for the sigma zero a posteriori
	limits	calcSigmaZeroLimits(const int nbObs, const int nbUnk, const TReal sigmaZero2);

	int						count;
	TLSResultsMatrices*		fResultsMatrices; /*!< pointer to the results matrices */ 
	int						fMaxIterations; /*!< maximum of iterations that can be carried out */
	TReal					fConvCriteria; /*!< convergence criteria that has to be exceeded to stop the iterative process */
	int						fNumberOfMadeIterations; /*!< indicates how many iteration have been carried out */
	string					fError; /*!< error from the calculation */
	
	TLGCCalcParams			fCalcParams;

	bool					fGeodSys; /*!< boolean indicating if the used coordinate system is geodetic or not */
	bool					fS0APosterioriVariances;
	bool					fIsSimulation;

    std::tr1::ranlux3_01 fRndGenerator;

};
#endif