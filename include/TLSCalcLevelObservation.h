//TLSCalcLevelObservation.h

/*! 
	Class for a vertical distance observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_LEVEL_OBS
#define LSCALC_LEVEL_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "LevelMeasurement.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "lsalgo/TLSCalcLengthParam.h" 

#include "TALSCalcLengthObservation.h"

/*!\ingroup lgc++
	@{*/

//!	Class for a vertical distance observation to be used in the least square algorithms.
class TLSCalcLevelObservation : public TALSCalcLengthObservation
{
public:

	TLSCalcLevelObservation(const LevelMeasurement* vdmeas,LSPosVecIter st, LSPosVecIter tg, LSLengthIter cte)
		: TALSCalcLengthObservation(*(vdmeas->getObservedValue()), *(vdmeas->getSigma()))
	{
		fLevelObs = vdmeas;
		fStPoint = st;
		fTgPoint = tg;
		fDistConst = cte;
	}

	/*!@name Access methods */
	//@{
		/*! Returns a pointer to the corresponding facade vert. dist observation */
		const LevelMeasurement*	getLevelObs() const { return fLevelObs; }

		/*! Returns the reference point */
		LSPosVecIter			getStPoint() const {return fStPoint;} 

		/*! Returns the target point */
		LSPosVecIter			getTgPoint() const {return fTgPoint;}

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
		string		getStPointName() const {return fStPoint->getName();}

		/*!Return the target point Name*/
		string		getTgPointName() const {return fTgPoint->getName();}

		/*! Returns the geode identifiant*/
		int		getId() const {return fLevelObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fLevelObs->getComment();}


		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTgPoint->getZIndex();}


		TALSCalcParameter::ELSStatus  getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getTgZStatus() const { return fTgPoint->getZStatus();}

		int	getObservationID() const { return fLevelObs->getObservationID(); }


	//@}

private:

	/*!@name Observation attribute for calculation */
	//@{
	const LevelMeasurement* fLevelObs; /*!< pointer of the corresponding vertical distance observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
	LSPosVecIter	fStPoint; /*!< a pointer to the reference point */
	LSPosVecIter	fTgPoint; /*!< a pointer to the target point */
	LSLengthIter	fDistConst; /*! Iterator for the distance constant */
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc vertical dist observations  */
typedef list< TLSCalcLevelObservation >			LSLevelContainer;
/*! Iterator of the list of LSCalc vertical dist observations */
typedef LSLevelContainer::iterator				LSLevelIter;
/*! Constant iterator of the list of LSCalc vertical dist observations */
typedef LSLevelContainer::const_iterator		LSLevelConstIter;
//@}

#endif //LSCALC_LEVEL_OBS

