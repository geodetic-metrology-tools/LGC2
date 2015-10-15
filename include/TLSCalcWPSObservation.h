//TLSCalcWPSObservation.h

/*! 
	Class for a wps observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				vertical and horizontal offset observations, position parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_WPS_OBS
#define LSCALC_WPS_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class TWPSMeasurement;

#include "TLength.h"
#include "UEOIndices.h"
#include "TSpatialPoint.h" // à modifier avec TLSCalcPosVectorParam.h quand la classe aura été créée
//#include "TLSCalcLengthParam.h"

/*!\ingroup lgc++
	@{*/

//! Class for a wps observation to be used in the least square algorithms
class TLSCalcWPSObservation 
{
public:
	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TLSCalcWPSObservation();
	/*! Constructor
	\param wpsmeas wps measurement observation 
	\param ex1 pointer to the end point 1 of the target wire  
	\param ex2 pointer to the end point 2 of the target wire */
	TLSCalcWPSObservation(TWPSMeasurement& wpsmeas,TSpatialPoint* ex1,TSpatialPoint* ex2);
	/*! Destructor */
	virtual ~TLSCalcWPSObservation();
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns the observed vertical offset */
	TLength			getVOffset() const;
	/*! Returns the observed vertical offset's sigma a priori */
	TLength			getVOffSigmaAPriori() const;
	/*! Returns the observed horizontal offset */
	TLength			getTOffset() const; 
	/*! Returns the observed horizontal offset's sigma a priori */
	TLength			getTOffSigmaAPriori() const; 
	/*! Returns a pointer to the end point 1  of the wire */
	TSpatialPoint*	getEnd1Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	/*! Returns a pointer to the end point 1  of the wire */
	TSpatialPoint*	getEnd2Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	//@}

	/*!@name Setting methods */
	//@{
	/*! Sets the sigma a posteriori for vertical offset observation */
	void	setVOffSigmaAPosteriori(TLength vsap);
		/*! Sets the sigma a posteriori for transversal offset observation */
	void	setTOffSigmaAPosteriori(TLength tsap);
	/*! Sets the residual of the computed vertical measurement */
	void	setVResidual(TLength vres);
	/*! Sets the residual of the computed transversal measurement */
	void	setTResidual(TLength tres);
	/*! Sets a simulated value of the vertical offset */
	void	setSimulatedVOffset(TLength sObsVO);
	/*! Sets a simulated value of the transversal offset */
	void	setSimulatedTOffset(TLength sObsTO);
	/*!Sets observations and equations indices
	\param ueoi a UEOIndices object holding the lowest free unknown, equation and observation indices */
	UEOIndices	setIndices(UEOIndices ueoi);
	//@}

protected:
	/*!@name Protected setting method */
	//@{
	/*! Sets the estimated vertical offset (observed + residual) */
	void		setEstimatedVOffset();
	/*! Sets the estimated transversal offset (observed + residual) */
	void		setEstimatedTOffset();

	//@}

private:

	/*!@name Private copy methods */
	//@{
	/*! Copy constructor */
	TLSCalcWPSObservation(const TLSCalcWPSObservation& source);
	/*! Copy assignment operator */
	TLSCalcWPSObservation& operator=(const TLSCalcWPSObservation& right);
	//@}

	/*!@name Observation attributes for calculation */
	//@{
	TLength		fVOffset;	/*!< observed vertical offset in calculation */
	TLength		fVOffSigmaAPriori;	/*!< sigma on observed vertical offset in calculation */
	TLength		fTOffset;	/*!< observed transversal offset in calculation */
	TLength		fTOffSigmaAPriori;	/*!< sigma on observed transversal offset in calculation */
	TLength		fVEstimatedOff;	/*!< compensated vertical offset after calculation */
	TLength		fTEstimatedOff;	/*!< compensated transversal offset after calculation */
	TLength		fVOffSigmaAPosteriori;	/*!< sigma on the compensated vertical offset */
	TLength		fTOffSigmaAPosteriori;	/*!< sigma on the compensated transversal offset */
	TLength		fVOffResidue;	/*!< vertical residue after calculation */
	TLength		fTOffResidue;	/*!< transversal residue after calculation */
	//@}

	/*!@name Variables attributes for calculation */
	// les types de ces attributs devront être changés pour des TLSCalcParam.
	//@{
	TSpatialPoint*	fEnd1Point; /*!< pointer to the wire's end point 1 */
	TSpatialPoint*	fEnd2Point; /*!< pointer to the wire's end point 2 */
	//@}

	UEOIndices fIndices; /*!< Unknown, Equation, observation indices of the observation for calculation */

};

/*@}*/

#endif //LSCALC_WPS_OBS


/*! Constructor
\param voff observed vertical offset
\param svoff sigma a priori of the observed vertical offset
\param hoff observed transversal offset
\param shoff sigma a priori of the observed transversal offset
\param ex1 pointer to the end point 1 of the target wire  
\param ex2 pointer to the end point 2 of the target wire 
TLSCalcWPSObservation(TLength voff,TLength svoff,TLength toff,TLength stoff,
	TSpatialPoint* ex1,TSpatialPoint* ex2); */
