//TLSCalc2DOffsetObservation.h

/*! 
	Class for a 2D offset observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				horizontal and vertical offset observations, position parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_2DOFF_OBS
#define LSCALC_2DOFF_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class T2DOffsetMeasurement;

#include "TLength.h"
#include "UEOIndices.h"
#include "TSpatialPoint.h" // à modifier avec TLSCalcPosVectorParam.h quand la classe aura été créée
//#include "TLSCalcLengthParam.h"


/*!\ingroup lgc++
	@{*/

//! Class for a 2D offset observation to be used in the least square algorithms
class TLSCalc2DOffsetObservation 
{
public:
	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TLSCalc2DOffsetObservation();
	/*! Constructor
	\param offmeas 2D offset measurement 
	\param st stationed point
	\param ex1 pointer to the end point 1 of the target wire  
	\param ex2 pointer to the end point 2 of the target wire */
	TLSCalc2DOffsetObservation(T2DOffsetMeasurement& offmeas,TSpatialPoint* st,
		TSpatialPoint* ex1,TSpatialPoint* ex2);
	/*! Destructor */
	virtual ~TLSCalc2DOffsetObservation();
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
	/*! Returns a pointer to the stationned point */
	TSpatialPoint*	getStPoint() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	/*! Returns a pointer to the end point 1  of the wire */
	TSpatialPoint*	getEnd1Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
	/*! Returns a pointer to the end point 1  of the wire */
	TSpatialPoint*	getEnd2Point() const; // Attention, le type devra être remplacé par TLSCalcPosVectParam
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
	TLSCalc2DOffsetObservation(const TLSCalc2DOffsetObservation& source);
	/*! Copy assignment operator */
	TLSCalc2DOffsetObservation& operator=(const TLSCalc2DOffsetObservation& right);
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
	TSpatialPoint*	fEnd1Point; /*!< pointer to the wire's end point 1 */
	TSpatialPoint*	fEnd2Point; /*!< pointer to the wire's end point 2 */
	TSpatialPoint*	fStPoint; /*!< pointer to the stationned point */
	//@}

	UEOIndices	fIndices; /*!< Unknown, Equation, observation indices of the observation for calculation */

};

/*@}*/

#endif //LSCALC_2DOFF_OBS



/* Constructor
\param voff observed vertical offset
\param svoff sigma a priori of the observed vertical offset
\param hoff observed horizontal offset
\param shoff sigma a priori of the observed horizontal offset
\param st pointer to the stationned point
\param ex1 pointer to the end point 1 of the target wire  
\param ex2 pointer to the end point 2 of the target wire 
TLSCalc2DOffsetObservation(TLength voff,TLength svoff,TLength hoff,TLength shoff,
	TSpatialPoint* st,TSpatialPoint* ex1,TSpatialPoint* ex2); */
