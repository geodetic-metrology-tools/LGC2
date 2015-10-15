// TOffsetToTheoPlaneObservation.h
/*!
	Class for an ECTH observation

	Copyright 2003 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_ECTH_OBS
#define SU_ECTH_OBS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;


#include "TDistMeasStation.h"
#include "TTheodoliteStation.h"
#include "TAPlaneMeasurement.h"
#include "TOffsetToTheoPlaneMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"



////////////////////////////////////////////////////////////////

/*! \addtogroup lgc++
	@{*/

//! Class for a horizontal angle measurement 
class  TOffsetToTheoPlaneObservation
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//! Default Constructor 
		TOffsetToTheoPlaneObservation();
		/*!Constructor
		\param meas a pointer to the measurement
		\param stationedPt an iterator of the stationed points list 
		\param firstTargetedPt an iterator of the first point which define the targeted Plane 
		\param secondTargetedPt an iterator of the second point which define the targeted Plane 
		\param cte distance measurement constant*/
		TOffsetToTheoPlaneObservation(DistStConstIter station, OffsetToTheoPlaneMeasConstIter meas,
			PointConstIter stationedPt,
			PointConstIter targetPt, TheodStConstIter targetSt, TAngle angle, DistConstantConstIter cte, int id);
		//! Copy Constructor 
		TOffsetToTheoPlaneObservation(const TOffsetToTheoPlaneObservation &source);
		//! Destructor
		virtual  ~TOffsetToTheoPlaneObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool  operator==(const TOffsetToTheoPlaneObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*@name Access to observed values */
	//@{
		/*! returns the horizontal angle value  from the measurement added to the constant angle (if there is one) */
		TLength		getOffsetToTheoPlaneValue() const;
		/*! returns the horizontal angle sigma value from the measurement */
		TLength		getSigmaValue() const;
	//@}


	/*!@name Access to data on parameter stationed point */
	//@
		/*! Returns the position vector of the stationed point */
		TPositionVector					getStationedPosition() const;
		/*! Returns the spatial status of the stationed point */
		TSpatialStatus::ESpatialStatus	getStationedPosStatus() const;
		/*! Returns the concatenated name of the stationed point */
		string							getStationedPosName() const;
	//@}

	/*!@name Access to data on parameter target point */
	//@{
		/*! Returns the position vector of the first point used to define the target Plane*/
		TPositionVector						getTargetTheoPosition() const;
		/*! Returns the spatial status of the first point used to define the target Plane*/
		TSpatialStatus::ESpatialStatus		getTargetTheoPosStatus() const;
		/*! Returns the concatenated name of the first point used to define the target Plane*/
		string								getTargetTheoPosName() const;
		/*! returns the angle used to define the  targeted Plane of this measurement*/
		TAngle								getTargetAngle() const;
		/*! Returns the concatenated name of the target Plane*/
		string								getTargetPlaneName() const;
	//@}


	/*!@name Access to data on target parameter V0 */
	//@{
		/*! returns the V0 from the rom */
		struct Angles				getTheoV0() const;
		/*! returns V0's status from the rom */
		TAMeasurement::ECalcStatus	getTheoV0Status() const;
		/*! returns V0's identifier */
		string						getTheoV0Name() const;
	//@}

	/*!@name Access to data on parameter constant distance */
	//@{
		/*! Returns the constant distance */
		TLength		getConstantValue() const;
		/*! Returns the constant distance'status */
		TAMeasurement::ECalcStatus		getConstantStatus() const;
		/*! Returns the constant distance's identifier */
		string		getConstantName() const;
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
		TPositionVector					get3DStationPosVec() const;
		/*! Converts the stationed point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DTargetStPosVec() const;

		int								getObservationID() const { return observationID; }
	//@}

protected:

	/*!@name Access methods */
	//@{
		/*! Returns a pointer to the dist measurement station */
		DistStConstIter					getStation() const;
		/*! returns a pointer to the measurement*/
		OffsetToTheoPlaneMeasConstIter	getMeasurement() const;
		/*! returns an iterator to the stationed point of this measurement*/
		PointConstIter					getStationedPoint() const;
		/*! returns an iterator to the point used to define the targeted Plane of this measurement*/
		PointConstIter					getTargetStationedPoint() const;
		/*! returns an iterator to the station used to define the targeted Plane of this measurement*/
		TheodStConstIter					getTargetStation() const;
	//@}


	

private:
	//! Copy Assignment Operator
	TOffsetToTheoPlaneObservation& operator=(const TOffsetToTheoPlaneObservation &right);
	
	int									observationID;
	DistStConstIter						fStation; /*!< pointer to the dist measurements station */
	OffsetToTheoPlaneMeasConstIter		fMeasurement; /*!< measurement */
	PointConstIter						fStationedPoint; /*!< stationed point of this measurement */
	PointConstIter						fTargetStationedPoint; /*!< stationed point of the targeted Plane of this measurement */
	TheodStConstIter					fTargetStation;/*!<station of the targeted Plane of this measurement */
	TAngle								fAngle; /*!< angle of the target Plane of this measurement */
	DistConstantConstIter				fMeasConst; /*!< measurement constant of this observation */
	bool								fIsActive; /*< activation level of the observation based on the activation of the other attributes */

};

/*!@name Constants definition */
//@{
/*! List of horizontal angle observations  */
typedef list< TOffsetToTheoPlaneObservation >	OffsetToTheoPlaneObsContainer;
/*! Iterator of the list of horizontal angle observations */
typedef OffsetToTheoPlaneObsContainer::iterator		OffsetToTheoPlaneObsIter;
/*! Constant iterator of the list of horizontal angle observations */
typedef OffsetToTheoPlaneObsContainer::const_iterator	OffsetToTheoPlaneObsConstIter;
//@}

#endif  //SU_ECTH_OBS

/*@}*/

inline bool TOffsetToTheoPlaneObservation::isActive() const { return fIsActive; }

inline DistStConstIter					TOffsetToTheoPlaneObservation::getStation() const { return fStation; }
inline OffsetToTheoPlaneMeasConstIter	TOffsetToTheoPlaneObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter					TOffsetToTheoPlaneObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter					TOffsetToTheoPlaneObservation::getTargetStationedPoint() const { return fTargetStationedPoint; }
inline TheodStConstIter					TOffsetToTheoPlaneObservation::getTargetStation() const { return fTargetStation; }
