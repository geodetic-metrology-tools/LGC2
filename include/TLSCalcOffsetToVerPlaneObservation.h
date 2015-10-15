//TLSCalcOffsetToVerPlaneObservation.h
/*! 
	Class for a offset (ECHO) observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_ECHO_OBS
#define LSCALC_OFFSET_ECHO_OBS


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
#define TLSCalcOffsetToVerPlaneObservation TLSCECHOOBS
#endif

#include <list>
#include <string>
using namespace std;

#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "lsalgo/TLSCalcFreeVectorParam.h" 
#include "lsalgo/TLSCalcLengthParam.h"

#include "TALSCalcLengthObservation.h"
#include "TOffsetToLineOrPlaneMeasurement.h"

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical Plane observation to be used in the least square algorithms
class TLSCalcOffsetToVerPlaneObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{

		TLSCalcOffsetToVerPlaneObservation(const TOffsetToLineOrPlaneMeasurement* obs,
			LSPosVecIter measuredPoint,
			LSPosVecIter origin,
			LSFreeVecIter planeUnitVector,
			LSLengthIter cte,
			LSLengthIter distanceFromOrigin);

		~TLSCalcOffsetToVerPlaneObservation()
		{
		}


	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcOffsetToVerPlaneObservation& right);

	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		const TOffsetToLineOrPlaneMeasurement*	getOffsetToVerPlaneObs() const { return fOffsetToVerPlaneObs; }
			
		/*! Returns a pointer to the stationned point */
		LSPosVecIter		getMeasuredPoint() const {return fMeasuredPoint;}
				
		/*!Returns an iterator of the distance cste */
		LSLengthConstIter	getDistConst() const {	return fDistConst;}
	//@}


	/*!@name Access methods : facade */
	//@{
		/*! Returns the estimated value of the dist constant as a TLenght */
		virtual TLength		getDistConstValue() const {return getDistConst()->getEstimatedValue();}


		/*! Returns the estimated value of the dist constant as a TLength */
		TLength		getEstDistConst() const {return getDistConst()->getEstimatedValue();}

		/*! Returns the provisional value of the distance constant as a TLength */
		TLength		getProvDistConst() const {return getDistConst()->getProvisionalValue();}

		/*!Return the corrected observation (obsvervation + constant)*/
		TLength		getCorrectedObsDist() const {return getObsDist() + getProvDistConst();}

		/*! Returns the compensated angle after calculation  add to constante*/
		TLength		getCorrectedEstDist() const {return getEstDist()+ getEstDistConst();}

		/*! Returns  the stationned point name */
		string		getMeasuredPointName() const {return fMeasuredPoint->getName();} 



		/*! Returns the geode identifiant*/
		int		getId() const {return fOffsetToVerPlaneObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fOffsetToVerPlaneObs->getComment();}


		TPositionVector	getMeasuredPointEstCoordinate() const {return fMeasuredPoint->getEstimatedValue();}

		TPositionVector	getOriginPointCoordinates() const {return originPosition->getProvisionalValue();}

		TLength	getOriginPointDistanceFromPlane() const {return fPlaneDistanceFromOrigin->getEstimatedValue();}

		LSLengthIter	getOriginPointDistanceFromPlaneUnknown() const {return fPlaneDistanceFromOrigin;}

		MatrixIndex		getMeasuredPointXIndex() const {return fMeasuredPoint->getXIndex();}

		MatrixIndex		getMeasuredPointYIndex() const {return fMeasuredPoint->getYIndex();}

		MatrixIndex		getMeasuredPointZIndex() const {return fMeasuredPoint->getZIndex();}


		TALSCalcParameter::ELSStatus  getMeasuredPointXStatus() const { return fMeasuredPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getMeasuredPointYStatus() const { return fMeasuredPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getMeasuredPointZStatus() const { return fMeasuredPoint->getZStatus();}

		int	getObservationID() const { return fOffsetToVerPlaneObs->getObservationID(); }

		LSFreeVecIter getPlaneNormalVector() const { return planeNormalVector; }



private:

	void calculateSigma();

	/*!@name Observation attribute for calculation */
	//@{
		const TOffsetToLineOrPlaneMeasurement* fOffsetToVerPlaneObs; /*!< iterator of the corresponding offset observation */

		LSFreeVecIter planeNormalVector;
		LSPosVecIter originPosition;
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		LSPosVecIter		fMeasuredPoint; /*!< iterator for the measured point used to define the target Plane*/
		LSLengthIter		fDistConst; /*! Iterator for the distance constant */
		LSLengthIter		fPlaneDistanceFromOrigin; /*! Iterator for the distance constant */
	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToVerPlaneObservation >		LSOffsetToVerPlaneContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerPlaneContainer::iterator			LSOffsetToVerPlaneIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToVerPlaneContainer::const_iterator		LSOffsetToVerPlaneConstIter;
//@}

#endif //LSCALC_OFFSET_ECHO_OBS



