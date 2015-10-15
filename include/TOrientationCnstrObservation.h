// TOrientationCnstrObservation.h
/*!
	Class for an PDOR observation

	Copyright 2003 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_PDOR_OBS
#define SU_PDOR_OBS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;


#include "TOrientationCnstr.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"



////////////////////////////////////////////////////////////////

/*! \addtogroup lgc++
	@{*/

//! Class for a constraints measurement 
class  TOrientationCnstrObservation
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//! Default Constructor 
		TOrientationCnstrObservation();
		/*!Constructor*/
		TOrientationCnstrObservation(PointConstIter fixedPoint, PointConstIter refPoint, OrieCnstrConstIter meas, int id);
		//! Copy Constructor 
		TOrientationCnstrObservation(const TOrientationCnstrObservation &source);
		//! Destructor
		virtual  ~TOrientationCnstrObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool  operator==(const TOrientationCnstrObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*@name Access to observed values */
	//@{
		/*! returns the constraints bearing  from the measurement*/
		TAngle		getOrieCnstrBearing() const;
	//@}

	/*! Returns the horizontal distance sigma as a TLength from the measurement */
	TAngle						getSigmaValue() const;

	/*!@name Access to data on parameter fixed point */
	//@
		/*! Returns the position vector of the stationed point */
		TPositionVector					getFixedPosition() const;
		/*! Returns the spatial status of the stationed point */
		TSpatialStatus::ESpatialStatus	getFixedPosStatus() const;
		/*! Returns the concatenated name of the stationed point */
		string							getFixedPosName() const;
	//@}

	/*!@name Access to data on parameter ref point */
	//@
		/*! Returns the position vector of the stationed point */
		TPositionVector					getRefPosition() const;
		/*! Returns the spatial status of the stationed point */
		TSpatialStatus::ESpatialStatus	getRefPosStatus() const;
		/*! Returns the concatenated name of the stationed point */
		string							getRefPosName() const;
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
		TPositionVector					get3DRefPosVec() const;
		/*! Converts the stationed point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DFixedPosVec() const;

		int								getObservationID() const { return observationID; }
	//@}


protected:

	/*!@name Access methods */
	//@{
		/*! returns a pointer to the measurement*/
		OrieCnstrConstIter				getMeasurement() const;
		/*! returns an iterator to the fixed point of this measurement*/
		PointConstIter					getFixedPoint() const;
		/*! returns an iterator to the ref point of this measurement*/
		PointConstIter					getRefPoint() const;

	//@}

private:
	//! Copy Assignment Operator
	TOrientationCnstrObservation& operator=(const TOrientationCnstrObservation &right);
	
	int									observationID;
	OrieCnstrConstIter					fMeasurement; /*!< measurement */
	PointConstIter						fFixedPoint; /*!< fixed point of this measurement */
	PointConstIter						fRefPoint; /*!< ref point of this measurement */
	bool								fIsActive; /*< activation level of the observation based on the activation of the other attributes */

};

/*!@name Constants definition */
//@{
/*! List of constraints observations  */
typedef list< TOrientationCnstrObservation >	OrieCnstrObsContainer;
/*! Iterator of the list of constraints observations */
typedef OrieCnstrObsContainer::iterator			OrieCnstrObsIter;
/*! Constant iterator of the list of constraints observations */
typedef OrieCnstrObsContainer::const_iterator	OrieCnstrObsConstIter;
//@}

#endif  //SU_PDOR_OBS

/*@}*/

inline bool TOrientationCnstrObservation::isActive() const { return fIsActive; }

inline OrieCnstrConstIter	TOrientationCnstrObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter		TOrientationCnstrObservation::getFixedPoint() const { return fFixedPoint; }
inline PointConstIter		TOrientationCnstrObservation::getRefPoint() const { return fRefPoint; }
