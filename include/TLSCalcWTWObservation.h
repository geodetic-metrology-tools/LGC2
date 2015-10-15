//TLSCalcWTWObservation.h

/*! 
	Class for a wire to wire observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				vertical and horizontal offset observations, position and length parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_WTW_OBS
#define LSCALC_WTW_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class TWireToWireDistMeas;

#include "TLength.h"
#include "UEOIndices.h"
#include "TSpatialPoint.h" // à modifier avec TLSCalcPosVectorParam.h quand la classe aura été créée
//#include "TLSCalcLengthParam.h"


/*!\ingroup lgc++
	@{*/

//! Class for a wire to wire observation to be used in the least square algorithms
class TLSCalcWTWObservation 
{
public:
	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TLSCalcWTWObservation();
	/*! Constructor
	\param wtwmeas wire to wire measurement observation 
		\param w1ex1 pointer to the end point 1 of the target wire 1 
	\param w1ex2 pointer to the end point 2 of the target wire 1
	\param w2ex1 pointer to the end point 1 of the target wire 2
	\param w2ex2 pointer to the end point 1 of the target wire 2
	\param pos1  pointer to the position on the target wire 1
	\param pos2  pointer to the position on the target wire 2*/
	TLSCalcWTWObservation(TWireToWireDistMeas& wtwmeas,TSpatialPoint* w1ex1,TSpatialPoint* w1ex2,TSpatialPoint* w2ex1,TSpatialPoint* w2ex2,
		TLength* pos1,TLength* pos2);
	/*! Destructor */
	virtual ~TLSCalcWTWObservation();
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns the observed vertical offset */
	TLength		getVOffset() const;
	/*! Returns the observed vertical offset's sigma a priori */
	TLength		getVOffSigmaAPriori() const;
	/*! Returns the observed horizontal offset */
	TLength		getHOffset() const; 
	/*! Returns the observed horizontal offset's sigma a priori */
	TLength		getHOffSigmaAPriori() const; 
	/*! Returns a pointer to the wire 1's end point 1  of the wire */
	TSpatialPoint*	getWire1End1Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	/*! Returns a pointer to the wire 1's end point 2  of the wire */
	TSpatialPoint*	getWire1End2Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	/*! Returns a pointer to the wire 2's end point 1  of the wire */
	TSpatialPoint*	getWire2End1Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	/*! Returns a pointer to the wire 2's end point 1  of the wire */
	TSpatialPoint*	getWire2End2Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	/*! Returns a pointer to the position of the point on wire 1 */
	TLength*		getPosOnWire1() const; // Attention, le type devra être remplacé par TLSCalcLengthParam
	/*! Returns a pointer to the position of the point on wire 2 */
	TLength*		getPosOnWire2() const; // Attention, le type devra être remplacé par TLSCalcLengthParam
	//@}

	/*!@name Setting methods */
	//@{
	/*! Sets the sigma a posteriori for vertical offset observation */
	void	setVOffSigmaAPosteriori(TLength vsap);
		/*! Sets the sigma a posteriori for horizontal offset observation */
	void	setHOffSigmaAPosteriori(TLength hsap);
	/*! Sets the residual of the computed vertical measurement */
	void	setVResidual(TLength vres);
	/*! Sets the residual of the computed horizontal measurement */
	void	setHResidual(TLength hres);
	/*! Sets a simulated value of the vertical offset */
	void	setSimulatedVOffset(TLength sObsVO);
	/*! Sets a simulated value of the horizontal offset */
	void	setSimulatedHOffset(TLength sObsHO);
	/*!Sets observations and equations indices
	\param ueoi a UEOIndices object holding the lowest free unknown, equation and observation indices */
	UEOIndices	setIndices(UEOIndices ueoi);
	//@}

protected:
	/*!@name Protected setting method */
	//@{
	/*! Sets the estimated vertical offset (observed + residual) */
	void		setEstimatedVOffset();
	/*! Sets the estimated horizontal offset (observed + residual) */
	void		setEstimatedHOffset();

	//@}

private:

	/*!@name Private copy methods */
	//@{
	/*! Copy constructor */
	TLSCalcWTWObservation(const TLSCalcWTWObservation& source);
	/*! Copy assignment operator */
	TLSCalcWTWObservation& operator=(const TLSCalcWTWObservation& right);
	//@}

	/*!@name Observation attributes for calculation */
	//@{
	TLength		fVOffset;	/*!< observed vertical offset in calculation */
	TLength		fVOffSigmaAPriori;	/*!< sigma on observed vertical offset in calculation */
	TLength		fHOffset;	/*!< observed horizontal offset in calculation */
	TLength		fHOffSigmaAPriori;	/*!< sigma on observed horizontal offset in calculation */
	TLength		fVEstimatedOff;	/*!< compensated vertical offset after calculation */
	TLength		fHEstimatedOff;	/*!< compensated horizontal offset after calculation */
	TLength		fVOffSigmaAPosteriori;	/*!< sigma on the compensated vertical offset */
	TLength		fHOffSigmaAPosteriori;	/*!< sigma on the compensated horizontal offset */
	TLength		fVOffResidue;	/*!< vertical residue after calculation */
	TLength		fHOffResidue;	/*!< horizontal residue after calculation */
	//@}

	/*!@name Variables attributes for calculation */
	// les types de ces attributs devront être changés pour des TLSCalcParam.
	//@{
	TSpatialPoint*	fWire1End1Point; /*!< pointer to the wire 1's end point 1 */
	TSpatialPoint*	fWire1End2Point; /*!< pointer to the wire 1's end point 2 */
	TSpatialPoint*	fWire2End1Point; /*!< pointer to the wire 2's end point 1 */
	TSpatialPoint*	fWire2End2Point; /*!< pointer to the wire 2's end point 2 */
	TLength*		fPosition1; /*!< pointer to the position on wire 1 */
	TLength*		fPosition2; /*!< pointer to the position on wire 2 */
	//@}

	UEOIndices	fIndices; /*!< Unknown, Equation, observation indices of the observation for calculation */

};

/*@}*/

#endif //LSCALC_WTW_OBS


/* Constructor
\param voff observed vertical offset
\param svoff sigma a priori of the observed vertical offset
\param hoff observed horizontal offset
\param shoff sigma a priori of the observed horizontal offset
\param w1ex1 pointer to the end point 1 of the target wire 1 
\param w1ex2 pointer to the end point 2 of the target wire 1
\param w2ex1 pointer to the end point 1 of the target wire 2
\param w2ex2 pointer to the end point 1 of the target wire 2
\param pos1  pointer to the position on the target wire 1
\param pos2  pointer to the position on the target wire 2
TLSCalcWTWObservation(TLength voff,TLength svoff,TLength hoff,TLength shoff,
	TSpatialPoint* w1ex1,TSpatialPoint* w1ex2,TSpatialPoint* w2ex1,TSpatialPoint* w2ex2,
	TLength* pos1,TLength* pos2); */
