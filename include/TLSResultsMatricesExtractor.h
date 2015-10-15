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
class TLSResultsMatrices;
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
	
bool TLSResultsMatricesExtractor::extractAllPointsFixedUnknowns(const TLSResultsMatrices& rm, TLGCCalcParams& calcParams);

	/*!Extracts the solution from the results matrices, updates the LSCalc dataset elements
	\param fstDsgn first design matrix used if the sigma a posteriori has to be calculated. If not, a default matrix is inserted
	\param calcParams parameters used during the extraction to indicate if a new iteration is necessary, and if the calculation of sigma a posteriori has to be done */
	virtual bool	extractResults(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

	/*!Extracts the results from the results matrices, updates the LSCalc dataset elements
	\param fstDsgn first design matrix used if the sigma a posteriori has to be calculated. If not, a default matrix is inserted
	\param calcParams parameters used during the extraction to indicate if a new iteration is necessary, and if the calculation of sigma a posteriori has to be done */
	virtual bool	extractResiduals(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

	/*! Extracts unknowns variances and covariances, updates the LSCalc dataset elements */
	virtual void	extractVarCovarParams(const TLSResultsMatrices&);

	/*! Extract error detection parameters */
	virtual	void	computeAndExtractReliability(const TVector & obsVar, TLGCCalcParams&);

	/*! Extract results for relative error computation */
	virtual void	extractRelError(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

private:

	/*!@name Helper methods relative to the ls calc parameters */
	//@{
		/*! Extract the results relative to the ls calc position vectors parameters 
		\param convergenceCriteria criteria the criteria tested during the extraction to determine if an additional iteration will be necessary*/
		virtual bool extractPosVecPara(const TLSResultsMatrices&, TReal convergenceCriteria);

		/*! Extract the results relative to the ls calc orientations parameters 
		\param convergenceCriteria criteria the criteria tested during the extraction to determine if an additional iteration will be necessary*/
		virtual bool extractOrientPara(const TLSResultsMatrices&, TReal convergenceCriteria);

		/*! Extract the results relative to the ls calc lengths parameters 
		\param convergenceCriteria criteria the criteria tested during the extraction to determine if an additional iteration will be necessary*/
		virtual bool extractLengthPara(const TLSResultsMatrices&, TReal convergenceCriteria);
	//@}

	
	/*!@name Methods for unknows var. covar. extractions */
	//@{
		/*! Extracts position vector params var. and covar. */
		virtual void extractPosVecVar(const TLSResultsMatrices&);

		/*! Extracts orientation params var. and covar. */
		virtual void extractOrientVar(const TLSResultsMatrices&);

		/*! Extracts length params var. and covar. */
		virtual void extractLengthVar(const TLSResultsMatrices&);
	//@}

	/*!@name Helper methods relative to the ls calc observations*/
	//@{
		/*!Extract the results relative to the horizontal angle observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractHorAngleObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

		/*!Extract the results relative to the zenith. distance observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractZenDistObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

		/*!Extract the results relative to the spatial distance observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractSpaDistObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

		/*!Extract the results relative to the horizontal distance observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractHorDistObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

		/*!Extract the results relative to the vertical distance observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractVertDistObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams, bool isDLEV);
		
		/*!Extract the results relative to the offset to vertical line observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractOffsetToVerLineObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);

		/*!Extract the results relative to the offset to spatial line observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractOffsetToSpaLineObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);
		
		/*!Extract the results relative to the offset to vertical plane observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractOffsetToVerPlaneObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);
		
		/*!Extract the results relative to the offset to theodolite plane observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractOffsetToTheoPlaneObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);
		
		/*!Extract the results relative to the gyro. orientation observations and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractGyroOrieObs(const TLSResultsMatrices&, TLGCCalcParams& calcParams);
		
		/*!Extract the results relative to the radial constraint and updates the latter
		@param calcParams parameters used for error detection if the calculation is enabled*/
		virtual void extractRadOffCnstr(const TLSResultsMatrices&, TLGCCalcParams& calcParams);	
	//@}


	/*!@name Methods for error detection parameters calculation and extraction */
		/*!Compute the fault for horizontal angle observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorHorAngle(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for the zenith. distance observations and updates the latter
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorZenDist(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for the spatial distance observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorSpaDist(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for horizontal distance observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorHorDist(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for vertical distance observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorVertDist(const TVector &, TLGCCalcParams&, bool isDLEV);
	
		/*!Compute the fault for offset to vertical line observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorOffsetToVerLine(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for offset to spatial line observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorOffsetToSpaLine(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for offset to vertical plane observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorOffsetToVerPlane(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for offset to theodolite plane observations and updates the latter 
		@param TLGCCalcParams	used for alpha and beta parameters */
		virtual void computeErrorOffsetToTheoPlane(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for gyro. orientation observations and updates the latter */
		virtual void computeErrorGyroOrie(const TVector &, TLGCCalcParams&);
		
		/*!Compute the fault for radial constraint and updates the latter */
		virtual void computeErrorRadOff(const TVector &, TLGCCalcParams&);	
	//@}

	/*! Extract the square sigma zero from the results matrices in ordre to update the sigma zero in LSCalcDataSet */
	virtual void extractS0APosteriori(const TLSResultsMatrices&);
	
	LSCalcDataSet*							fDataSet;			/*!< data from ls calc observations and ls calc parameters */
	bool									fLastIteration;		/*!< indicates if the convergence criteria has been exceeded and if a new iteration is necessary (default value = false)*/
	string									fError;				/*!< errors during execution of data extraction */
	TReal									fS0;				/*!< s0 a  posteriori to apply */
	
	bool									fIsSimulation;      /*!< indicates if the extraction is carried out in a simulation framework */
	TRefSystemFactory::ERefFrame fRefSurface;		/*!< reference surface for the project (from the TLGCCalcParameters) */
};
#endif

