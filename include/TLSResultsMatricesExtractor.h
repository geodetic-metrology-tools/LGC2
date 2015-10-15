////////////////////////////////////////////////////////////////////
// TLSResultsMatricesExtractor.h
/*!
 Class responsible for extracting the results from the matrices 
 and updating the appropriate member data of the LS Calc objects
 Specific to a least squares calculation

Patterns:
 
Copyright 2003-2008 M.Jones, CERN, TS/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////


#ifndef SU_TLS_RESULTS_MATRICES_EXTRACTOR
#define SU_TLS_RESULTS_MATRICES_EXTRACTOR


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
class TLSResultsMatricesAdapter;
class TMatrix;
class LSCalcDataSet;
class TLGCCalcParams;

#include "TDataParameters.h"
#include "TRefSystemFactory.h"

//!Class responsible for extracting the results from the matrices and updating the appropriate member data of the LS Calc objects
/*!Specific to a least squares calculation*/
class TLSResultsMatricesExtractor{

public:

	/*!@name Constructors / Destructor*/
	//@{
		/*!Constructor
		@param lscds a pointer to the LS calc dataset*/
		explicit TLSResultsMatricesExtractor(LSCalcDataSet* lscds);
		//!Destructor
		virtual ~TLSResultsMatricesExtractor();
	//@}

	/*! Returns the boolean that indicates if a new iteration is necessary or not */
	bool		lastIteration() const;

	/*! Returns the eventual error */
	string		getError() const { return fError; }

	/*! Indicates if it is a simulated case or not */
	bool		isSimulation() const {return fIsSimulation;}


	/*! Initiates the simulation case */
	void		simulation();

	/*! Initiates the case of observations variances a posteriori calculation*/
	void		setComputeS2APost(bool compute);

	/*! Initiates the reference surface*/
	void		setReferenceSurface(TRefSystemFactory::ERefFrame refSurf) { fRefSurface = refSurf; return; }
	
bool TLSResultsMatricesExtractor::extractAllPointsFixedUnknowns();

	/*!Extracts the solution from the results matrices, updates the LSCalc dataset elements
	\param fstDsgn first design matrix used if the sigma a posteriori has to be calculated. If not, a default matrix is inserted
	\param calcParams parameters used during the extraction to indicate if a new iteration is necessary, and if the calculation of sigma a posteriori has to be done */
	virtual bool	extractResults(const TLSResultsMatricesAdapter&, TLGCCalcParams& calcParams);

	/*!Extracts the results from the results matrices, updates the LSCalc dataset elements
	\param fstDsgn first design matrix used if the sigma a posteriori has to be calculated. If not, a default matrix is inserted
	\param calcParams parameters used during the extraction to indicate if a new iteration is necessary, and if the calculation of sigma a posteriori has to be done */
	virtual bool	extractResiduals(const TLSResultsMatricesAdapter&);

	/*! Extracts unknowns variances and covariances, updates the LSCalc dataset elements */
	virtual void	extractVarCovarParams(const TLSResultsMatricesAdapter&);

	/*! Extract error detection parameters */
	virtual	void	computeAndExtractReliability(TVector* obsVar, TLGCCalcParams&);

	/*! Extract results for relative error computation */
	virtual void	extractRelError(const TLSResultsMatricesAdapter&, TLGCCalcParams& calcParams);

private:

	/*!@name Helper methods relative to the ls calc parameters */
	//@{
		/*! Extract the results relative to the ls calc position vectors parameters 
		\param convergenceCriteria criteria the criteria tested during the extraction to determine if an additional iteration will be necessary*/
		virtual bool extractPosVecPara(const TLSResultsMatricesAdapter&, TReal convergenceCriteria);
		
		/*! Extract the results relative to the ls calc free vectors parameters 
		\param convergenceCriteria criteria the criteria tested during the extraction to determine if an additional iteration will be necessary*/
		virtual bool extractFreeVecPara(const TLSResultsMatricesAdapter&, TReal convergenceCriteria);

		/*! Extract the results relative to the ls calc orientations parameters 
		\param convergenceCriteria criteria the criteria tested during the extraction to determine if an additional iteration will be necessary*/
		virtual bool extractOrientPara(const TLSResultsMatricesAdapter&, TReal convergenceCriteria);

		/*! Extract the results relative to the ls calc lengths parameters 
		\param convergenceCriteria criteria the criteria tested during the extraction to determine if an additional iteration will be necessary*/
		virtual bool extractLengthPara(const TLSResultsMatricesAdapter&, TReal convergenceCriteria);
	//@}

	
	/*!@name Methods for unknows var. covar. extractions */
	//@{
		/*! Extracts position vector params var. and covar. */
		virtual void extractPosVecVar(const TLSResultsMatricesAdapter&);
		
		/*! Extracts free vector params var. and covar. */
		virtual void extractFreeVecVar(const TLSResultsMatricesAdapter&);

		/*! Extracts orientation params var. and covar. */
		virtual void extractOrientVar(const TLSResultsMatricesAdapter&);

		/*! Extracts length params var. and covar. */
		virtual void extractLengthVar(const TLSResultsMatricesAdapter&);
	//@}

	/*!@name Helper methods relative to the ls calc observations*/
	//@{

		template <typename T>
		void extractAngleObsResidual(typename list<T>::iterator iterB, typename list<T>::iterator iterE, const TLSResultsMatricesAdapter& rm);

		template <typename T>
		void extractLengthObsResidual(typename list<T>::iterator iterB, typename list<T>::iterator iterE, const TLSResultsMatricesAdapter& rm);
		
		void extractPolarObsResiduals(const TLSResultsMatricesAdapter& rm);
	//@}


	/*!@name Methods for error detection parameters calculation and extraction */
		template <typename T>
		void computeErrorObservation(typename list<T>::iterator iterB, typename list<T>::iterator iterE, TVector* obsVar, TLGCCalcParams& calcParams);
	//@}

	/*! Extract the square sigma zero from the results matrices in ordre to update the sigma zero in LSCalcDataSet */
	virtual void extractS0APosteriori(const TLSResultsMatricesAdapter&);
	
	LSCalcDataSet*							fDataSet;			/*!< data from ls calc observations and ls calc parameters */
	bool									fLastIteration;		/*!< indicates if the convergence criteria has been exceeded and if a new iteration is necessary (default value = false)*/
	string									fError;				/*!< errors during execution of data extraction */
	TReal									fS0;				/*!< s0 a  posteriori to apply */
	
	bool									fIsSimulation;      /*!< indicates if the extraction is carried out in a simulation framework */
	TRefSystemFactory::ERefFrame				fRefSurface;		/*!< reference surface for the project (from the TLGCCalcParameters) */
};
#endif

