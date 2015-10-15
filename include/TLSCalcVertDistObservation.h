//TLSCalcVertDistObservation.h

/*! 
	Class for a vertical distance observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_VERTDIST_OBS
#define LSCALC_VERTDIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "TVerticalDistMeasurement.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "lsalgo/TLSCalcLengthParam.h" 

#include "TALSCalcLengthObservation.h"

/*!\ingroup lgc++
	@{*/

//!	Class for a vertical distance observation to be used in the least square algorithms.
class TLSCalcVertDistObservation : public TALSCalcLengthObservation
{
public:

	TLSCalcVertDistObservation(const TVerticalDistMeasurement* vdmeas,LSPosVecIter tg1,LSPosVecIter tg2, LSLengthIter cte);

	~TLSCalcVertDistObservation()
	{
	}
	/*!@name Access methods */
	//@{
		/*! Returns a pointer to the corresponding facade vert. dist observation */
		const TVerticalDistMeasurement*	getVertDistObs() const { return fVertDistObs; }

		/*! Returns the reference point */
		LSPosVecIter			getTg1Point() const {return fTg1Point;} 

		/*! Returns the target point */
		LSPosVecIter			getTg2Point() const {return fTg2Point;}

		/*!Returns an iterator of the distance cste */
		LSLengthConstIter		getDistConst() const {return fDistConst;}

		/*! Returns the estimated value of the dist constant as a TLenght */
		TLength					getDistConstValue() const {return getDistConst()->getEstimatedValue();}

	//@}


	/*!@name Access methods : Facade */
	//@{
		/*!Return the corrected observation (obsvervation + constant)*/
		TLength		getCorrectedObsDist() const {return getObsDist() + getProvDistConst();}

		/*! Returns the provisional value of the distance constant as a TAngle */
		TLength		getProvDistConst() const {return fDistConst->getProvisionalValue();}

		/*! Returns the estimated value of the distance constant as a TAngle */
		TLength		getEstDistConst() const {return fDistConst->getEstimatedValue();}

		/*! Returns the compensated angle after calculation  add to constante*/
		TLength		getCorrectedEstDist() const {return getEstDist()+ getEstDistConst();}

		/*!Return the stationned point Name*/
		string		getTg1PointName() const {return fTg1Point->getName();}

		/*!Return the target point Name*/
		string		getTg2PointName() const {return fTg2Point->getName();}

		/*! Returns the geode identifiant*/
		int		getId() const {return fVertDistObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fVertDistObs->getComment();}


		TPositionVector	getTg1EstCoordinate() const {return fTg1Point->getEstimatedValue();}

		TPositionVector	getTg2EstCoordinate() const {return fTg2Point->getEstimatedValue();}

		MatrixIndex		getTg1XIndex() const {return fTg1Point->getXIndex();}

		MatrixIndex		getTg1YIndex() const {return fTg1Point->getYIndex();}

		MatrixIndex		getTg1ZIndex() const {return fTg1Point->getZIndex();}

		MatrixIndex		getTg2XIndex() const {return fTg2Point->getXIndex();}

		MatrixIndex		getTg2YIndex() const {return fTg2Point->getYIndex();}

		MatrixIndex		getTg2ZIndex() const {return fTg2Point->getZIndex();}


		TALSCalcParameter::ELSStatus  getTg1XStatus() const { return fTg1Point->getXStatus();}

		TALSCalcParameter::ELSStatus  getTg1YStatus() const { return fTg1Point->getYStatus();}

		TALSCalcParameter::ELSStatus  getTg1ZStatus() const { return fTg1Point->getZStatus();}

		TALSCalcParameter::ELSStatus  getTg2XStatus() const { return fTg2Point->getXStatus();}

		TALSCalcParameter::ELSStatus  getTg2YStatus() const { return fTg2Point->getYStatus();}

		TALSCalcParameter::ELSStatus  getTg2ZStatus() const { return fTg2Point->getZStatus();}

		int	getObservationID() const { return fVertDistObs->getObservationID(); }


	//@}

private:

	/*!@name Observation attribute for calculation */
	//@{
	const TVerticalDistMeasurement* fVertDistObs; /*!< pointer of the corresponding vertical distance observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
	LSPosVecIter	fTg1Point; /*!< a pointer to the reference point */
	LSPosVecIter	fTg2Point; /*!< a pointer to the target point */
	LSLengthIter	fDistConst; /*! Iterator for the distance constant */
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc vertical dist observations  */
typedef list< TLSCalcVertDistObservation >			LSVertDistContainer;
/*! Iterator of the list of LSCalc vertical dist observations */
typedef LSVertDistContainer::iterator				LSVertDistIter;
/*! Constant iterator of the list of LSCalc vertical dist observations */
typedef LSVertDistContainer::const_iterator			LSVertDistConstIter;
//@}

#endif //LSCALC_VERTDIST_OBS

