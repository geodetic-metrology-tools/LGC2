// TOffsetToVerPlaneObservation.h
/*!
	Class for an ECHO observation

	Copyright 2003 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_ECHO_OBS
#define SU_ECHO_OBS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;


#include "TDistMeasStation.h"
#include "TAPlaneMeasurement.h"
#include "TOffsetToVerPlaneMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"



////////////////////////////////////////////////////////////////

/*! \addtogroup lgc++
	@{*/

//! Class for a horizontal angle measurement 
class  TOffsetToVerPlaneObservation
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//! Default Constructor 
		TOffsetToVerPlaneObservation();
		/*!Constructor
		\param meas a pointer to the measurement
		\param stationedPt an iterator of the stationed points list 
		\param firstTargetedPt an iterator of the first point which define the targeted Plane 
		\param secondTargetedPt an iterator of the second point which define the targeted Plane 
		\param cte distance measurement constant*/
		TOffsetToVerPlaneObservation(DistStConstIter station, OffsetToVerPlaneMeasConstIter meas,
			PointConstIter stationedPt,
			PointConstIter firstTargetPt, PointConstIter secondTargetPt, DistConstantConstIter cte, int id);
		//! Copy Constructor 
		TOffsetToVerPlaneObservation(const TOffsetToVerPlaneObservation &source);
		//! Destructor
		virtual  ~TOffsetToVerPlaneObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool  operator==(const TOffsetToVerPlaneObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*@name Access to observed values */
	//@{
		/*! returns the horizontal angle value  from the measurement added to the constant angle (if there is one) */
		TLength		getOffsetToVerPlaneValue() const;
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
		TPositionVector						getFirstTargetPosition() const;
		/*! Returns the spatial status of the first point used to define the target Plane*/
		TSpatialStatus::ESpatialStatus		getFirstTargetPosStatus() const;
		/*! Returns the concatenated name of the first point used to define the target Plane*/
		string								getFirstTargetPosName() const;
		/*! Returns the position vector of the second  point used to define the target Plane*/
		TPositionVector						getSecondTargetPosition() const;
		/*! Returns the spatial status of the second point used to define the target Plane*/
		TSpatialStatus::ESpatialStatus		getSecondTargetPosStatus() const;
		/*! Returns the concatenated name of the second point used to define the target Plane*/
		string								getSecondTargetPosName() const;
		/*! Returns the concatenated name of the target Plane*/
		string								getTargetPlaneName() const;
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
		/*! Converts the first target point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DFirstTargetPosVec() const;
		/*! Converts the second target point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DSecondTargetPosVec() const;

		int								getObservationID() const { return observationID; }
	//@}

	
protected:

	/*!@name Access methods */
	//@{
		/*! Returns a pointer to the dist measurement station */
		DistStConstIter					getStation() const;
		/*! returns a pointer to the measurement*/
		OffsetToVerPlaneMeasConstIter	getMeasurement() const;
		/*! returns an iterator to the stationed point of this measurement*/
		PointConstIter					getStationedPoint() const;
		/*! returns an iterator to the first point used to define the targeted Plane of this measurement*/
		PointConstIter					getFirstTargetPoint() const;
		/*! returns an iterator to the second point used to define the  targeted Plane of this measurement*/
		PointConstIter					getSecondTargetPoint() const;
	//@}

private:
	//! Copy Assignment Operator
	TOffsetToVerPlaneObservation& operator=(const TOffsetToVerPlaneObservation &right);
	
	int									observationID;
	DistStConstIter						fStation; /*!< pointer to the dist measurements station */
	OffsetToVerPlaneMeasConstIter		fMeasurement; /*!< measurement */
	PointConstIter						fStationedPoint; /*!< stationed point of this measurement */
	PointConstIter						fFirstTargetPoint; /*!< first targeted Plane of this measurement */
	PointConstIter						fSecondTargetPoint; /*!< second targeted Plane of this measurement */
	DistConstantConstIter				fMeasConst; /*!< measurement constant of this observation */
	bool								fIsActive; /*< activation level of the observation based on the activation of the other attributes */

};

/*!@name Constants definition */
//@{
/*! List of horizontal angle observations  */
typedef list< TOffsetToVerPlaneObservation >	OffsetToVerPlaneObsContainer;
/*! Iterator of the list of horizontal angle observations */
typedef OffsetToVerPlaneObsContainer::iterator		OffsetToVerPlaneObsIter;
/*! Constant iterator of the list of horizontal angle observations */
typedef OffsetToVerPlaneObsContainer::const_iterator	OffsetToVerPlaneObsConstIter;
//@}

#endif  //SU_ECHO_OBS

/*@}*/

inline bool TOffsetToVerPlaneObservation::isActive() const { return fIsActive; }

inline DistStConstIter					TOffsetToVerPlaneObservation::getStation() const { return fStation; }
inline OffsetToVerPlaneMeasConstIter		TOffsetToVerPlaneObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter					TOffsetToVerPlaneObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter					TOffsetToVerPlaneObservation::getFirstTargetPoint() const { return fFirstTargetPoint; }
inline PointConstIter					TOffsetToVerPlaneObservation::getSecondTargetPoint() const { return fSecondTargetPoint; }
