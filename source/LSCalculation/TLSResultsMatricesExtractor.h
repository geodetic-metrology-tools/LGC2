////////////////////////////////////////////////////////////////////
// TLSResultsMatricesExtractor.h
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
class TLGCData;
class TLSResultsMatrices;
class TUVD;
class TUVEC;
struct TLEVEL;
#include "Measurements\MeasDef.h"
/*!
 Class responsible for extracting the results from the matrices 
 and updating the appropriate member data of the adjustable objects and measurements (observations respectively).
 Specific to a least squares calculation.
*/
class TLSResultsMatricesExtractor{

public:

	/*!@name Constructors / Destructor*/
	//@{
		/*!Constructor
		@param dataSet a pointer to a project data*/
		explicit TLSResultsMatricesExtractor(TLGCData* dataSet);
		//!Destructor
		virtual ~TLSResultsMatricesExtractor();
	//@}

	/*!Extracts the solution from the results matrices, updates the adjustable objects.
		\param[in] rm Result matrices.
		\param[in] convCrit LS convergence criterium.

		\returns TRUE if the process was successful, FALSE otherwise. 
	*/
	bool	extractResults(const TLSResultsMatrices& rm, TReal convCrit);

	/*! \returns the boolean that indicates if a new iteration is necessary or not */
	bool	lastIteration() const;

	/*! 
		Extracts the residuals from the results matrices and update measurements (observations) with these values.
		\param[in] rm Result matrices.
	*/
	bool	extractResiduals(const TLSResultsMatrices& rm);

	/*! Extracts unknowns variances and covariances, updates the adjustbale objects with these values.*/
	void	extractVarCovarParams(const TLSResultsMatrices&);

	/*! Indicates if it is a simulated case or not */
	bool		isSimulation() const {return fIsSimulation;}


#if 0
		bool extractAllPointsFixedUnknowns(const TLSResultsMatrices& rm, TLGCCalcParams& calcParams);


		/*! Extract error detection parameters */
		virtual	void	computeAndExtractReliability(const TVector & obsVar, TLGCCalcParams&);

		/*! Extract results for relative error computation */
		virtual void	extractRelError(const TLSResultsMatrices&, TLGCCalcParams& calcParams);
#endif

private:

#if 0
		/*!@name Methods for error detection parameters calculation and extraction */
			/*!Compute the fault for horizontal angle observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorHorAngle(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for the zenith. distance observations and updates the latter
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorZenDist(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for the spatial distance observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorSpaDist(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for horizontal distance observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorHorDist(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for vertical distance observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorVertDist(const TVector &, TLGCCalcParams&, bool isDLEV);
	
			/*!Compute the fault for offset to vertical line observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorOffsetToVerLine(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for offset to spatial line observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorOffsetToSpaLine(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for offset to vertical plane observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorOffsetToVerPlane(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for offset to theodolite plane observations and updates the latter 
			@param TLGCCalcParams	used for alpha and beta parameters */
	//		virtual void computeErrorOffsetToTheoPlane(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for gyro. orientation observations and updates the latter */
	//		virtual void computeErrorGyroOrie(const TVector &, TLGCCalcParams&);
		
			/*!Compute the fault for radial constraint and updates the latter */
	//		virtual void computeErrorRadOff(const TVector &, TLGCCalcParams&);	
		//@}

		/*! Extract the square sigma zero from the results matrices in ordre to update the sigma zero in LSCalcDataSet */
		virtual void extractS0APosteriori(const TLSResultsMatrices&);
#endif	

	/*!@name Methods responsible for extracting corrections from result matrices and filling them into adjustable objects.*/
	//@{
		bool extractPointParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractAngleParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractScalarParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractPlaneParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractTransformationParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractLineParams(const TLSResultsMatrices& rm, const TReal convCrit);  //Not yet implemented
	//@}

	/*!@name Methods responsible for unknows var. covar. extractions and filling them into adjustable objects*/
	//@{
		void extractPointVarCovar(const TLSResultsMatrices& rm);
		void extractAngleVar(const TLSResultsMatrices& rm);
		void extractScalarVar(const TLSResultsMatrices& rm);
		void extractPlaneVarCovar(const TLSResultsMatrices& rm);
		void extractTransformationVarCovar(const TLSResultsMatrices& rm);
		void extractLineVarCovar(const TLSResultsMatrices& rm);  //Not yet implemented
	//@}

	/*!@name Methods relative to the measurements (observations), responsible for extracting residuals for measured values and setting them up to measurements.*/
	//@{

		/*TSTN*/
		/*!Extract the results relative to any angle measurement (ANGL, ZEND) observation and set the calculated residual*/
		void extractAngleObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>& anglMeas);

		/*!Extract the results relative to any distance measurement (DIST, DHOR) observation and set the calculated residual*/
		void extractDistObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget>& distanceMeas);

		/*!Extract the results relative to a PLR3D measurement observation and set the calculated residual*/
		void extractPLR3DObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles,  2>& plr3DMeas);

		/*!Extract the results relative to a DIR3D measurement observation and set the calculated residual*/
		void extractDIR3DObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 0, EPLR3DAngles,  2>& dir3DMeas);

		/* CAMERA*/
		void extractUVDObs(const TLSResultsMatrices& rm, TUVD& uvdMeas);

		void extractUVECObs(const TLSResultsMatrices& rm, TUVEC& uvecMeas);
		/*OTHERS*/

		/*!Extract the results relative to a LEVEL measurement observation and set the calculated residuals*/
		void extractLEVELObs(const TLSResultsMatrices& rm, TLEVEL& levelMeas);


		void extractDSPTObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TEDM::TTarget>& distanceMeas);
	//@}


	TLGCData*				fDataSet;			/*!< pointer to a project data*/
	bool					fLastIteration;		/*!< indicates if the convergence criteria has been exceeded and if a new iteration is necessary (default value = false)*/
	TReal					fS0;				/*!< s0 a  posteriori to apply */
	bool					fIsSimulation;      /*!< indicates if the extraction is carried out in a simulation framework */
};
#endif

