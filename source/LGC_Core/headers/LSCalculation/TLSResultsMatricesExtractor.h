/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SU_TLS_RESULTS_MATRICES_EXTRACTOR
#define SU_TLS_RESULTS_MATRICES_EXTRACTOR

// LGC
#include <MeasDef.h>

class TLGCData;
class TLSResultsMatrices;
class TUVD;
class TUVEC;
class TPdorObs;
class TRADI;
struct TEDM;
struct TTSTN;
struct TLEVEL;
struct TECHOROM;
struct TECVEROM;
struct TECSPROM;
struct TORIEROM;
struct TINCLYROM;
struct TROLLYROM;
struct TECWSROM;
struct TECWIROM;

/*!
	\ingroup LSCalculation
	\brief  Class responsible for extracting the results from the matrices and updating the appropriate member data of the
	adjustable objects and measurements. Specific to a least squares calculation.
*/
class TLSResultsMatricesExtractor
{
public:
	/*!@name Constructors / Destructor*/
	//@{
	/*!
		\brief Constructor

		\param[in] dataSet a pointer to a project data
	*/
	TLSResultsMatricesExtractor(TLGCData *dataSet);
	/// Destructor
	~TLSResultsMatricesExtractor();
	//@}

	/*!
		\brief Extracts the solution from the results matrices, updates the adjustable objects.

		\param[in] rm Result matrices.
		\param[in] convCrit LS convergence criterium.

		\returns TRUE if the process was successful, FALSE otherwise.
	*/
	bool extractResults(const TLSResultsMatrices &rm, TReal convCrit);

	/// Returns the boolean that indicates if a new iteration is necessary or not
	bool lastIteration() const;

	/*!
		\brief Extracts the residuals from the results matrices and update measurements with these values.

		\param[in] rm Result matrices.
	*/
	bool extractResiduals(const TLSResultsMatrices &rm);

	/// Extracts unknown variances and covariances and fill them into the adjustable objects. Returns true if successful.
	bool extractVarCovarParams(const TLSResultsMatrices &);

	/// Extract results for relative error computation, returning true if successful, false otherwise
	bool extractRelError();

	/// Extract distance sensitivities
	void computeDistSensi();

private:
	/*!@name Methods responsible for extracting corrections from result matrices and filling them into the adjustable objects.*/
	//@{
	bool extractPointParams(const TLSResultsMatrices &rm, const TReal convCrit);
	bool extractAngleParams(const TLSResultsMatrices &rm, const TReal convCrit);
	bool extractLengthParams(const TLSResultsMatrices &rm, const TReal convCrit);
	bool extractPlaneParams(const TLSResultsMatrices &rm, const TReal convCrit);
	bool extractTransformationParams(const TLSResultsMatrices &rm, const TReal convCrit);
	bool extractLineParams(const TLSResultsMatrices &rm, const TReal convCrit);
	bool extractSagParams(const TLSResultsMatrices &rm, const TReal convCrit);
	//@}

	/*!@name Methods responsible for final extraction of variances covariances in order to be filled into the adjustable objects. */
	//@{
	void extractFullCovar(const TLSResultsMatrices &rm);
	void extractPointVarCovar(const TLSResultsMatrices &rm);
	void extractAngleVar(const TLSResultsMatrices &rm);
	void extractLengthVar(const TLSResultsMatrices &rm);
	void extractPlaneVarCovar(const TLSResultsMatrices &rm);
	void extractTransformationVarCovar(const TLSResultsMatrices &rm);
	void extractLineVarCovar(const TLSResultsMatrices &rm);
	void extractSagVarCovar(const TLSResultsMatrices &rm);
	//@}

	/*!@name Methods relative to the measurements, responsible for extracting residuals.*/
	//@{
	/*TSTN*/
	/// Sets the angle measurement residual (ANGL or ZEND).
	void extractAngleObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget, ENoValues, 0, ESingleValue, 1> &anglMeas);

	/// Sets the distance measurement residual for a TSTN (DIST, DHOR).
	void extractDistObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TPOLAR::TTarget> &distanceMeas);

	/// Sets the PLR3D measurement residual.
	void extractPLR3DObs(const TLSResultsMatrices &rm, TPLR3D &plr3DMeas);

	/// Sets the UVD measurement residual.
	void extractUVDObs(const TLSResultsMatrices &rm, TUVD &uvdMeas);
	/// Sets the UVEC measurement residual.
	void extractUVECObs(const TLSResultsMatrices &rm, TUVEC &uvecMeas);

	/*OTHERS*/

	/// Sets the TDLEV measurement residual.
	void extractLEVELObs(const TLSResultsMatrices &rm, TLEVEL &levelMeas);

	/// Sets the ECHO measurement residual.
	void extractECHOROMObs(const TLSResultsMatrices &rm, TECHOROM &echoMeas);

	/// Sets the ECVE measurement residual.
	void extractECVEROMObs(const TLSResultsMatrices &rm, TECVEROM &ecveMeas);

	/// Sets the ECSP measurement residual.
	void extractECSPROMObs(const TLSResultsMatrices &rm, TECSPROM &ecspMeas);

	/// Sets the ORIE measurement residual.
	void extractORIEROMObs(const TLSResultsMatrices &rm, TORIEROM &orieMeas);

	/// Sets the DSPT measurement residual.
	void extractDSPTObs(const TLSResultsMatrices &rm, TDSPT &distanceMeas);

	/// Sets the DVER measurement residual.
	void extractDVERObs(const TLSResultsMatrices &rm, std::list<TDVER> &dver);

	/// Sets the distance measurement residual for a SCALE (ECTH, ECVE,ECSP, ECDIR).
	void extractDistObs(const TLSResultsMatrices &rm, TAScalarMeas<TInstrumentData::TSCALE> &scaleMeas);

	/// Sets the PDOR measurement residual.
	void extractPDORObs(const TLSResultsMatrices &rm, TPdorObs &pdorObs);

	/// Sets the RADI measurement residual.
	void extractRADIObs(const TLSResultsMatrices &rm, std::list<TRADI> &radi);

	/// Sets the OBSXYZ measurement residual.
	void extractOBSXYZObs(const TLSResultsMatrices &rm, std::list<TOBSXYZ> &obsxyz);

	/// Sets the INCLY measurement residual.
	void extractINCLYROMObs(const TLSResultsMatrices &rm, TINCLYROM &inclyMeas);

	/// Sets the ROLLY measurement residual.
	void extractROLLYROMObs(const TLSResultsMatrices &rm, TROLLYROM &rollyMeas);

	/// Sets the ECWS measurement residual.
	void extractECWSROMObs(const TLSResultsMatrices &rm, TECWSROM &ecwsMeas);

	/// Sets the ECWI measurement residual.
	void extractECWIROMObs(const TLSResultsMatrices &rm, TECWIROM &ecwiMeas);
	//@}
	// calculates and sets the point sigma offset
	void extractPointSigmaOffset(LGCAdjustablePoint &point);

	TLGCData *fDataSet; /*!< pointer to a project data*/
	bool fLastIteration; /*!< indicates if the convergence criteria has been exceeded and if a new iteration is necessary (default value = false)*/
};

// TMEAS must have distance observation and residual, and its target a distCorrectionAdjustable
// TINSTR must have property instrHeight
template<class TMEAS, class TINSTR>
TReal computeDistanceSensibility(TMEAS const *const meas, LGCAdjustablePoint const *const instrPos, const TINSTR &instr)
{
	// computes the distance sensibility (unitless)
	TReal dz = meas->targetPos->getEstValue(2) + meas->target.targetHt - instrPos->getEstValue(2) - instr.instrHeight;
	return meas->target.distCorrectionUnknown ? dz / (meas->getDistance() + meas->getDistanceResidual() + meas->target.distCorrectionAdjustable->getEstimatedValue())
											  : dz / (meas->getDistance() + meas->getDistanceResidual());
}

#endif
