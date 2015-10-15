#ifndef TLGC_CALCULATION
#define TLGC_CALCULATION


#include "TDataAnalyzer.h"
#include "lsalgo\TALSComputer.h"
struct TCalcType;

class TLSResultsMatricesExtractor;
class TLSInputMatricesFiller;
class TLSInputMatrices;
class TLGCData;



class TLGCCalculation{
	public:
	/*!
			Manages the LGC calculation process.

			@param dat[in] LGC project to be processed.
			@param dat[calculationType] Type of the LGC calculation, describing the specific options found, constraints, etc.

	*/
	TLGCCalculation(TLGCData* dat);

	bool computeLSResults();


	const TLGCData& getData(){ return fData;}

	private:

	// contains references: not assignable
	TLGCCalculation& operator=(const TLGCCalculation&);
	//Information about the calculation
	TCalcType fCalculationType;

	// LGC data set
	TLGCData& fData;

//	std::unique_ptr<TALSComputer> fComputer;
	std::unique_ptr<TLSResultsMatrices>		fResultsMatrices; /*!< pointer to the results matrices */ 

	int			fMaxIterations; /*!< maximum of iterations that can be carried out */
	TReal		fConvCriteria; /*!< convergence criteria that has to be exceeded to stop the iterative process */
	int			fNumberOfMadeIterations; /*!< indicates how many iteration have been carried out */
	bool		fS0APosterioriVariances;


	bool				fIsSimulation;

	//
	/*!@name Private methods -- preparation*/
	//@{



	//@}


	/*!@name Private methods -- calculation*/
	//@{
	/*! Iterate to the solution of the least squares calculation */
	bool	iterate2Solution(TLGCData& fData,
							 TLSInputMatricesFiller* inpMtrFiller,
							 TLSInputMatrices* inputMtr,
							 std::unique_ptr<TALSComputer> computer,
							 TLSResultsMatricesExtractor* extractor);


	//@}

};

#endif