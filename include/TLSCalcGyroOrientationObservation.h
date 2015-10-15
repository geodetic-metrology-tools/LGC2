//TLSCalcGyroOrientationObservation.h
/*! 
	Class for a orientation observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				angle observations, position and angle parameters)

	Copyright 2003 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_GYRO_ORIE_OBS
#define LSCALC_GYRO_ORIE_OBS

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)
#endif // _MSC_VER >= 1000

#ifdef _DEBUG
#define TLSCalcGyroOrientationObservation TLSCORIEOBS
#endif

#include <list>
#include <string>
using namespace std;

#include "lsalgo/UEOIndices.h"
#include "THorAngleMeasurement.h"
#include "lsalgo/TLSCalcOrientationParam.h"
#include "lsalgo/TLSCalcPosVectorParam.h" 

#include "TALSCalcAngleObservation.h"
#include "GyroscopeStation.h"


/*!\ingroup lgc++
	@{*/

//! Class for a orientation observation to be used in the least square algorithms
class TLSCalcGyroOrientationObservation : public TALSCalcAngleObservation 
{
public:
	/*!@name Constructors / Destructors */
	//@{

		/*! Constructor
		\param haObs iterator of the orientation observation 
		\param st iterator of the stationned point
		\param tg iterator of the target point */
		TLSCalcGyroOrientationObservation(const THorAngleMeasurement* haObs,
			LSPosVecIter st,LSPosVecIter tg, LSOrientIter cte, const GyroscopeStation* station);

		/*! Copy constructor */
		TLSCalcGyroOrientationObservation(const TLSCalcGyroOrientationObservation& source);

		/*! Destructor */
		virtual ~TLSCalcGyroOrientationObservation();
	//@}

	void calculateSigma();


	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcGyroOrientationObservation& right);


	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade hor. angle observation */
		const THorAngleMeasurement*	getGyroOrientationObs() const {return fGyroOrieObs;}

		/*! Returns a pointer to the stationned point */
		LSPosVecIter			getStPoint() const {return fStPoint;}

		/*! Returns a pointer to  the target point */
		LSPosVecIter			getTgPoint() const {return fTgPoint;}

 		/*!Returns an iterator of the angle cste */
		LSOrientConstIter		getAngleConst() const {	return fAngleConst;}
	//@}


	/*!@name Access methods : Facade */
	//@{
		/*!Return the corrected observation (obsvervation + constant)*/
		TAngle		getCorrectedObsAngle() const {return ( getObsAngle() + getAngleConst()->getProvisionalValue().kappa);}

		/*! Returns the estimated value of the angle constant as a TAngle */
		TAngle		getEstConstAngle() const {return getAngleConst()->getEstimatedValue().kappa;}

		/*! Returns the corrected compensated angle after calculation (angle +cte)*/
		TAngle		getCorrectedEstimatedAngle() const {return ( getEstimatedAngle() + getAngleConst()->getEstimatedValue().kappa);};

		/*!Return the stationned point Name*/
		string		getStPointName() const {return fStPoint->getName();};

		/*!Return the target point Name*/
		string		getTgPointName() const {return fTgPoint->getName();};

		/*!return the distance between the station and the target*/
		TLength		getStTgDistance() const;

		/*!return the movement of the target (length)*/
		TLength		getTgMovement() const;

		/*! Returns the geode identifiant*/
		int			getId() const {return fGyroOrieObs->getId();}

		/*! Returns the geode comment */
		string		getComment() const {return fGyroOrieObs->getComment();}

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

		int	getObservationID() const { return fGyroOrieObs->getObservationID(); }

		
	//@}


private:

	const GyroscopeStation* gStation;

	/*! Copy assignment operator */
	TLSCalcGyroOrientationObservation& operator=(const TLSCalcGyroOrientationObservation& right);

	/*!@name Observation attribute for calculation */
	//@{
		const THorAngleMeasurement*	fGyroOrieObs; /*!< iterator of the corresponding orientation observation (facade to spatial measurements) */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		LSPosVecIter	fTgPoint; /*!< iterator of the target point */
		LSPosVecIter	fStPoint; /*!< iterator of the stationned point */
		LSOrientIter	fAngleConst; /*! Iterator for the angle constant */

	//@}
};

/*@}*/

/*!@name Constants definition */
//@{
	/*! List of ls calc orientation observations  */
	typedef list< TLSCalcGyroOrientationObservation >	LSGyroOrieContainer;
	/*! Iterator of the list of ls calc orientation observations */
	typedef LSGyroOrieContainer::iterator				LSGyroOrieIter;
	/*! Constant iterator of the list of ls calc orientation observations */
	typedef LSGyroOrieContainer::const_iterator			LSGyroOrieConstIter;
//@}


#endif //LSCALC_GYRO_ORIE_OBS





