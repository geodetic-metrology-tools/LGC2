#ifndef SU_TLS_RESULTS_MATRICES_EXTRACTOR
#define SU_TLS_RESULTS_MATRICES_EXTRACTOR

#include "MeasDef.h"
class TLGCData;
class TLSResultsMatrices;
class TUVD;
class TUVEC;
struct TLEVEL;
struct TECHOROM;
/*!
	\ingroup LSCalculation
	\brief  Class responsible for extracting the results from the matrices and updating the appropriate member data of the 
	adjustable objects and measurements. Specific to a least squares calculation.
*/
class TLSResultsMatricesExtractor{

public:

	/*!@name Constructors / Destructor*/
	//@{
		/*!
			\brief Constructor

			\param[in] dataSet a pointer to a project data
		*/
		TLSResultsMatricesExtractor(TLGCData* dataSet);
		/// Destructor
		~TLSResultsMatricesExtractor();
	//@}

	/*!
		\brief Extracts the solution from the results matrices, updates the adjustable objects.
		
		\param[in] rm Result matrices.
		\param[in] convCrit LS convergence criterium.

		\returns TRUE if the process was successful, FALSE otherwise. 
	*/
	bool	extractResults(const TLSResultsMatrices& rm, TReal convCrit);

	/// Returns the boolean that indicates if a new iteration is necessary or not 
	bool	lastIteration() const;

	/*! 
		\brief Extracts the residuals from the results matrices and update measurements with these values.
		
		\param[in] rm Result matrices.
	*/
	bool	extractResiduals(const TLSResultsMatrices& rm);

	/// Extracts unknown variances and covariances and fill them into the adjustable objects.
	void	extractVarCovarParams(const TLSResultsMatrices&);

private:
	/*!@name Methods responsible for extracting corrections from result matrices and filling them into the adjustable objects.*/
	//@{
		bool extractPointParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractAngleParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractScalarParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractPlaneParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractTransformationParams(const TLSResultsMatrices& rm, const TReal convCrit);
		bool extractLineParams(const TLSResultsMatrices& rm, const TReal convCrit);  //Not yet implemented, LINE not used so far
	//@}

	/*!@name Methods responsible for final extraction of variances covariances in order to be filled into the adjustable objects. */
	//@{
		void extractPointVarCovar(const TLSResultsMatrices& rm);
		void extractAngleVar(const TLSResultsMatrices& rm);
		void extractScalarVar(const TLSResultsMatrices& rm);
		void extractPlaneVarCovar(const TLSResultsMatrices& rm);
		void extractTransformationVarCovar(const TLSResultsMatrices& rm);
		void extractLineVarCovar(const TLSResultsMatrices& rm);  //Not yet implemented
	//@}

	/*!@name Methods relative to the measurements, responsible for extracting residuals.*/
	//@{
		/*TSTN*/
		/// Sets the angle measurement residual (ANGL or ZEND).
		void extractAngleObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1>& anglMeas);

		/// Sets the distance measurement residual (DIST, DHOR).
		void extractDistObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget>& distanceMeas);

		/// Sets the PLR3D measurement residual.
		void extractPLR3DObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ESingleValue, 1, EPLR3DAngles,  2>& plr3DMeas);


		/// Sets the UVD measurement residual.
		void extractUVDObs(const TLSResultsMatrices& rm, TUVD& uvdMeas);
		/// Sets the UVEC measurement residual.
		void extractUVECObs(const TLSResultsMatrices& rm, TUVEC& uvecMeas);


		/*OTHERS*/

		/// Sets the TDLEV measurement residual.
		void extractLEVELObs(const TLSResultsMatrices& rm, TLEVEL& levelMeas);

		/// Sets the ECHO measurement residual.
		void extractECHOROMObs(const TLSResultsMatrices& rm, TECHOROM& levelMeas);

		/// Sets the DSPT measurement residual.
		void extractDSPTObs(const TLSResultsMatrices& rm, TAScalarMeas<TInstrumentData::TEDM::TTarget>& distanceMeas);

		/// Sets the DVER measurement residual.
		void extractDVERObs(const TLSResultsMatrices& rm, std::vector<TDVER>& dver);
	//@}


	TLGCData*				fDataSet;			/*!< pointer to a project data*/
	bool					fLastIteration;		/*!< indicates if the convergence criteria has been exceeded and if a new iteration is necessary (default value = false)*/
};
#endif

