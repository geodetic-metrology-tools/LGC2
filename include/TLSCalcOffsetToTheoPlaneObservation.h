//TLSCalcOffsetToTheoPlaneObservation.h
/*! 
	Class for a offset (ECTH) observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				distance observations, position and length parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.
*/


#ifndef LSCALC_OFFSET_ECTH_OBS
#define LSCALC_OFFSET_ECTH_OBS


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
#define TLSCalcOffsetToTheoPlaneObservation TLSCECTHOBS
#endif

#include <list>
#include <string>
using namespace std;

#include "TOffsetToTheoPlaneMeasurement.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 
#include "THorAngleMeasurement.h"
#include "lsalgo/TLSCalcOrientationParam.h"
#include "lsalgo/TLSCalcLengthParam.h" 

#include "TALSCalcLengthObservation.h"
#include "TheodoliteStation.h"

/*!\ingroup lgc++
	@{*/

//! Class for a offset to vertical Plane observation to be used in the least square algorithms
class TLSCalcOffsetToTheoPlaneObservation : public TALSCalcLengthObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{

		/*! Constructor
		\param sdObs iterator of the spatial dist observations container  
		\param stPoint iterator for the stationned point
		\param firstPlaneTargetPoint iterator for the first point used to define the target Plane
		\param angle iterator for the angle used to define the target Plane
		\param v0 iterator for the v0 used to define the target Plane*/
		TLSCalcOffsetToTheoPlaneObservation(const TOffsetToTheoPlaneMeasurement* obs,
			LSPosVecIter stPoint,
			LSPosVecIter firstPlaneTargetPoint, LSOrientIter v0,
			LSLengthIter cte, const TheodoliteStation* station);

		/*! Copy constructor */
		TLSCalcOffsetToTheoPlaneObservation(const TLSCalcOffsetToTheoPlaneObservation& source);

		/*! Destructor */
		virtual ~TLSCalcOffsetToTheoPlaneObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcOffsetToTheoPlaneObservation& right);

	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade spatial dist observation */
		const TOffsetToTheoPlaneMeasurement*	getOffsetToTheoPlaneObs() const { return fOffsetToTheoPlaneObs;}

		/*! Returns a pointer to the stationned point */
		LSPosVecIter		getStPoint() const {return fStPoint;} 

		/*! Returns a pointer to the first target point used to defined the target Plane */
		LSPosVecIter		getTgPoint() const {return fTarget;} 

		/*! Returns a pointer to the v0 angle */
		LSOrientConstIter	getTgV0() const {return fV0;} 

		/*!Returns an iterator of the distance cste */
		LSLengthConstIter	getDistConst() const {return fDistConst;}

	//@}
	

	/*!@name Access methods : facade */
	//@{
		/*! Returns the estimated value of the dist constant as a TLenght */
		virtual TLength		getDistConstValue() const {return getDistConst()->getEstimatedValue();}

		/*! Returns a pointer to the angle used to defined the target Plane */
		TAngle				getTgAngle() const {return *(fOffsetToTheoPlaneObs->getObservedAngle());} 


		/*! Returns the estimated value of the dist constant as a TLength */
		TLength		getEstDistConst() const {return getDistConst()->getEstimatedValue();}

		/*! Returns the provisional value of the distance constant as a TLength */
		TLength		getProvDistConst() const {return getDistConst()->getProvisionalValue();}

		/*!Return the corrected observation (obsvervation + constant)*/
		TLength		getCorrectedObsDist() const {return getObsDist() + getProvDistConst();}

		/*! Returns the compensated angle after calculation  add to constante*/
		TLength		getCorrectedEstDist() const {return getEstDist()+ getEstDistConst();}

		/*! Returns  the stationned point name */
		string		getStPointName() const {return fStPoint->getName();} 

		/*! Returns the first target point name */
		string		getTgPointName() const {return fTarget->getName();} 

		/*! Returns the estimated V0 of the station*/
		TAngle		getEstV0() const { return hasOwnV0() ? getAllPointsFixedV0() : getTgV0()->getEstimatedValue().kappa; }

		TAngle		getEstBearing() const {return getEstV0() + getTgAngle();}

		/*! Returns the geode identifiant*/
		int		getId() const {return fOffsetToTheoPlaneObs->getId();}

		/*! Returns the geode comment */
		string	getComment() const {return fOffsetToTheoPlaneObs->getComment();}

		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getTgEstCoordinate() const {return fTarget->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getTgXIndex() const {return fTarget->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTarget->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTarget->getZIndex();}

		MatrixIndex		getV0Index() const {return getTgV0()->getKappaIndex();}

		TALSCalcParameter::ELSStatus	getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus	getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus	getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus	getTgXStatus() const { return fTarget->getXStatus();}

		TALSCalcParameter::ELSStatus	getTgYStatus() const { return fTarget->getYStatus();}

		TALSCalcParameter::ELSStatus	getTgZStatus() const { return fTarget->getZStatus();}

		TALSCalcParameter::ELSStatus	getV0Status() const {return getTgV0()->getKappaStatus();}

		int	getObservationID() const { return fOffsetToTheoPlaneObs->getObservationID(); }


	//@}

		void setHasOwnV0() { fHasOwnV0 = true; }
		bool hasOwnV0() const { return fHasOwnV0; }
		TAngle getAllPointsFixedV0() const { return fAllPointsFixedV0; }
		void setAllPointsFixedV0(TAngle newV0) { fAllPointsFixedV0 = newV0; }

private:

	const TheodoliteStation* tStation;

	void calculateSigma();

	/*!@name Private copy methods */
	//@{
		/*! Copy assignment operator */
		TLSCalcOffsetToTheoPlaneObservation& operator=(const TLSCalcOffsetToTheoPlaneObservation& right);
	//@}

	/*!@name Observation attribute for calculation */
	//@{
		const TOffsetToTheoPlaneMeasurement*	fOffsetToTheoPlaneObs; /*!< iterator of the corresponding offset observation */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		bool			fHasOwnV0;
		TAngle			fAllPointsFixedV0; // V0 for the observation when all the points are fixed
		LSPosVecIter		fTarget;
		LSOrientIter		fV0; /*!< iterator of the station's orientation */
		LSPosVecIter		fStPoint; /*!< iterator for the stationned point */
		LSLengthIter		fDistConst; /*! Iterator for the distance constant */
	//@}

};

/*@}*/

/*!@name Constants definition */
//@{
/*! List of LSCalc spatial dist observations  */
typedef list< TLSCalcOffsetToTheoPlaneObservation >		LSOffsetToTheoPlaneContainer;
/*! Iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToTheoPlaneContainer::iterator			LSOffsetToTheoPlaneIter;
/*! Constant iterator of the list of LSCalc spatial dist observations */
typedef LSOffsetToTheoPlaneContainer::const_iterator	LSOffsetToTheoPlaneConstIter;
//@}

#endif //LSCALC_OFFSET_ECTH_OBS



