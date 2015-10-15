//TLSCalcHorAngleObservation.h

/*! 
	Class for a horizontal angle observation, used in the least square algorithms.

	Pattern: facade (for the components involved in least-square compensation of zenithal measurements :
				angle observations, position and angle parameters)

	Copyright 2002 CERN EST/SU. All rights reserved.

*/

#ifndef LSCALC_HORANG_OBS
#define LSCALC_HORANG_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <list>
#include <string>
using namespace std;

#include "THorAngleObservation.h"
#include "TLSCalcOrientationParam.h"
#include "TLSCalcPosVectorParam.h" 

#include "TALSCalcAngleObservation.h"

/*!\ingroup lgc++
	@{*/

//! Class for a horizontal angle observation to be used in the least square algorithms
class TLSCalcHorAngleObservation : public TALSCalcAngleObservation
{
public:
	/*!@name Constructors / Destructors */
	//@{
		/*! Default constructor */
		TLSCalcHorAngleObservation();

		/*! Constructor
		\param haObs iterator of the horizontal angle observation 
		\param v0 iterator of the orientation's station
		\param st iterator of the stationned point
		\param tg iterator of the target point */
		TLSCalcHorAngleObservation(HorAngObsConstIter haObs,LSOrientIter v0,
			LSPosVecIter st,LSPosVecIter tg,LSOrientIter cte);

		/*! Copy constructor */
		TLSCalcHorAngleObservation(const TLSCalcHorAngleObservation& source);

		/*! Destructor */
		virtual ~TLSCalcHorAngleObservation();
	//@}


	/*! Overloaded equality operator */
	bool	operator==(const TLSCalcHorAngleObservation& right);


	/*!@name Access methods : iterator*/
	//@{
		/*! Returns a iterator to the corresponding facade hor. angle observation */
		HorAngObsConstIter	getHorAngleObs() const {return fHorAngObs;};

		/*! Returns a pointer to the v0 angle */
		LSOrientConstIter	getV0() const {return fV0;}; 

		/*! Returns a pointer to the stationned point */
		LSPosVecIter		getStPoint() const {return fStPoint;}; 

		/*! Returns a pointer to  the target point */
		LSPosVecIter		getTgPoint() const {return fTgPoint;}; 

		/*!Returns an iterator of the angle cste */
		LSOrientConstIter	getAngleConst() const{return fAngleConst;};
	//@}

	
	/*!@name Access methods : Facade */
	//@{
		/*!Return the corrected observation (obsvervation + constant)*/
		TAngle		getCorrectedObsAngle() const {return (getObsAngle() + getAngleConst()->getProvisionalValue().kappa);}

		/*! Returns the estimated value of the angle constant as a TAngle */
		TAngle		getEstConstAngle() const {return getAngleConst()->getEstimatedValue().kappa;};

		/*! Returns the compensated angle after calculation  add to V0*/
		TAngle		getCorrectedEstimatedAngle() const {return (getEstimatedAngle() + fV0->getEstimatedValue().kappa);}

		/*!Return the stationned point Name*/
		string		getStPointName() const {return fStPoint->getName();};

		/*!Return the target point Name*/
		string		getTgPointName() const {return fTgPoint->getName();};

		/*!return the V0 as a TAngle*/
		TAngle		getEstimatedV0() const {return hasOwnV0() ? getAllPointsFixedV0() : fV0->getEstimatedValue().kappa;}

		/*!return the distance between the station and the target*/
		TLength		getStTgDistance() const;

		/*!return the movement of the target (length)*/
		TLength		getTgMovement() const;

		/*! Returns the geode identifiant*/
		int			getId() const {return fHorAngObs->getId();}

		/*! Returns the geode comment */
		string		getComment() const {return fHorAngObs->getComment();}

		TPositionVector	getStEstCoordinate() const {return fStPoint->getEstimatedValue();}

		TPositionVector	getTgEstCoordinate() const {return fTgPoint->getEstimatedValue();}

		MatrixIndex		getStXIndex() const {return fStPoint->getXIndex();}

		MatrixIndex		getStYIndex() const {return fStPoint->getYIndex();}

		MatrixIndex		getStZIndex() const {return fStPoint->getZIndex();}

		MatrixIndex		getTgXIndex() const {return fTgPoint->getXIndex();}

		MatrixIndex		getTgYIndex() const {return fTgPoint->getYIndex();}

		MatrixIndex		getTgZIndex() const {return fTgPoint->getZIndex();}

		MatrixIndex		getV0Index() const { return fV0->getKappaIndex();}


		TALSCalcParameter::ELSStatus  getStXStatus() const { return fStPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getStYStatus() const { return fStPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getStZStatus() const { return fStPoint->getZStatus();}

		TALSCalcParameter::ELSStatus  getTgXStatus() const { return fTgPoint->getXStatus();}

		TALSCalcParameter::ELSStatus  getTgYStatus() const { return fTgPoint->getYStatus();}

		TALSCalcParameter::ELSStatus  getTgZStatus() const { return fTgPoint->getZStatus();}

		TALSCalcParameter::ELSStatus	getV0Status() const { return fV0->getKappaStatus();}

		int	getObservationID() const { return fHorAngObs->getObservationID(); }


	//@}

		void setHasOwnV0() { fHasOwnV0 = true; }
		bool hasOwnV0() const { return fHasOwnV0; }
		TAngle getAllPointsFixedV0() const { return fAllPointsFixedV0; }
		void setAllPointsFixedV0(TAngle newV0) { fAllPointsFixedV0 = newV0; }


private:

	/*! Copy assignment operator */
	TLSCalcHorAngleObservation& operator=(const TLSCalcHorAngleObservation& right);

	/*!@name Observation attribute for calculation */
	//@{
		HorAngObsConstIter fHorAngObs; /*!< iterator of the corresponding horizontal angle observation (facade to spatial measurements) */
	//@}

	/*!@name Variables attributes for calculation */
	//@{
		bool			fHasOwnV0;
		TAngle			fAllPointsFixedV0; // V0 for the observation when all the points are fixed
		LSOrientIter	fV0; /*!< iterator of the station's orientation */
		LSPosVecIter	fTgPoint; /*!< iterator of the target point */
		LSPosVecIter	fStPoint; /*!< iterator of the stationned point */
		LSOrientIter	fAngleConst; /*! Iterator for the angle constant */
	//@}


};

/*@}*/

/*!@name Constants definition */
//@{
	/*! List of ls calc horizontal angle observations  */
	typedef list< TLSCalcHorAngleObservation >		LSHorAngContainer;
	/*! Iterator of the list of ls calc horizontal angle observations */
	typedef LSHorAngContainer::iterator		LSHorAngIter;
	/*! Constant iterator of the list of ls calc horizontal angle observations */
	typedef LSHorAngContainer::const_iterator	LSHorAngConstIter;
//@}


#endif //LSCALC_HORANG_OBS





