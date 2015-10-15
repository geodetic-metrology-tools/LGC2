// TRadialOffsetCnstrObservation.h
/*!
	Class for an RADI observation

	Copyright 2003 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_RADI_OBS
#define SU_RADI_OBS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;


#include "TRadialOffsetCnstr.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"



////////////////////////////////////////////////////////////////

/*! \addtogroup lgc++
	@{*/

//! Class for a constraints measurement 
class  TRadialOffsetCnstrObservation
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//! Default Constructor 
		TRadialOffsetCnstrObservation();
		/*!Constructor
		\param meas a pointer to the measurement
		\param stationedPt an iterator of the stationed points list 
		\param firstTargetedPt an iterator of the first point which define the targeted Plane 
		\param secondTargetedPt an iterator of the second point which define the targeted Plane 
		\param cte distance measurement constant*/
		TRadialOffsetCnstrObservation(PointConstIter point, RadOffCnstrConstIter meas, int id);
		//! Copy Constructor 
		TRadialOffsetCnstrObservation(const TRadialOffsetCnstrObservation &source);
		//! Destructor
		virtual  ~TRadialOffsetCnstrObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool  operator==(const TRadialOffsetCnstrObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*@name Access to observed values */
	//@{
		/*! returns the constraints width   from the measurement*/
		TLength		getRadialOffsetCnstrSigma() const;
		/*! returns the constraints bearing  from the measurement*/
		TAngle		getRadialOffsetCnstrBearing() const;
	//@}


	/*!@name Access to data on parameter point */
	//@
		/*! Returns the position vector of the stationed point */
		TPositionVector					getPosition() const;
		/*! Returns the spatial status of the stationed point */
		TSpatialStatus::ESpatialStatus	getPosStatus() const;
		/*! Returns the concatenated name of the stationed point */
		string							getPosName() const;
	//@}

	/*!@name Access to database' data */
	//@{
		/*! Returns the geode identifiant*/
		int		getId() const {return fMeasurement->getId();}
		/*! Returns the geode comment */
		string	getComment() const {return fMeasurement->getComment();}
	//@}

	/*!@name Methods converting points in 2D+H coordinate system in 3D cartesian system */ 
	//@{
		/*! Converts the stationed point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DPosVec() const;

		int								getObservationID() const { return observationID; }
	//@}


protected:

	/*!@name Access methods */
	//@{
		/*! returns a pointer to the measurement*/
		RadOffCnstrConstIter				getMeasurement() const;
		/*! returns an iterator to the stationed point of this measurement*/
		PointConstIter					getPoint() const;
	//@}

private:
	//! Copy Assignment Operator
	TRadialOffsetCnstrObservation& operator=(const TRadialOffsetCnstrObservation &right);
	
	int									observationID;
	RadOffCnstrConstIter					fMeasurement; /*!< measurement */
	PointConstIter						fPoint; /*!< point of this measurement */
	bool								fIsActive; /*< activation level of the observation based on the activation of the other attributes */

};

/*!@name Constants definition */
//@{
/*! List of constraints observations  */
typedef list< TRadialOffsetCnstrObservation >	RadOffCnstrObsContainer;
/*! Iterator of the list of constraints observations */
typedef RadOffCnstrObsContainer::iterator		RadOffCnstrObsIter;
/*! Constant iterator of the list of constraints observations */
typedef RadOffCnstrObsContainer::const_iterator	RadOffCnstrObsConstIter;
//@}

#endif  //SU_RADI_OBS

/*@}*/

inline bool TRadialOffsetCnstrObservation::isActive() const { return fIsActive; }

inline RadOffCnstrConstIter	TRadialOffsetCnstrObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter					TRadialOffsetCnstrObservation::getPoint() const { return fPoint; }
