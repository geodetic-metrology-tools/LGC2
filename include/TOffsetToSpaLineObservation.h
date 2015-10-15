// TOffsetToSpaLineObservation.h
/*!
	Class for an ECSP observation

	Copyright 2003 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_ECSP_OBS
#define SU_ECSP_OBS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;


#include "TDistMeasStation.h"
#include "TALineMeasurement.h"
#include "TOffsetToSpaLineMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"



////////////////////////////////////////////////////////////////

/*! \addtogroup lgc++
	@{*/

//! Class for a horizontal angle measurement 
class  TOffsetToSpaLineObservation
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//! Default Constructor 
		TOffsetToSpaLineObservation();
		/*!Constructor
		\param meas a pointer to the measurement
		\param stationedPt an iterator of the stationed points list 
		\param firstTargetedPt an iterator of the first point which define the targeted Line 
		\param secondTargetedPt an iterator of the second point which define the targeted Line 
		\param cte distance measurement constant*/
		TOffsetToSpaLineObservation(DistStConstIter station, OffsetToSpaLineMeasConstIter meas,
			PointConstIter stationedPt,
			PointConstIter firstTargetPt, PointConstIter secondTargetPt, DistConstantConstIter cte, int id);
		//! Copy Constructor 
		TOffsetToSpaLineObservation(const TOffsetToSpaLineObservation &source);
		//! Destructor
		virtual  ~TOffsetToSpaLineObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool  operator==(const TOffsetToSpaLineObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*@name Access to observed values */
	//@{
		/*! returns the horizontal angle value  from the measurement added to the constant angle (if there is one) */
		TLength		getOffsetToSpaLineValue() const;
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
		/*! Returns the position vector of the first point used to define the target line*/
		TPositionVector						getFirstTargetPosition() const;
		/*! Returns the spatial status of the first point used to define the target line*/
		TSpatialStatus::ESpatialStatus		getFirstTargetPosStatus() const;
		/*! Returns the concatenated name of the first point used to define the target line*/
		string								getFirstTargetPosName() const;
		/*! Returns the position vector of the second  point used to define the target line*/
		TPositionVector						getSecondTargetPosition() const;
		/*! Returns the spatial status of the second point used to define the target line*/
		TSpatialStatus::ESpatialStatus		getSecondTargetPosStatus() const;
		/*! Returns the concatenated name of the second point used to define the target line*/
		string								getSecondTargetPosName() const;
		/*! Returns the concatenated name of the target line*/
		string								getTargetLineName() const;
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
		OffsetToSpaLineMeasConstIter		getMeasurement() const;
		/*! returns an iterator to the stationed point of this measurement*/
		PointConstIter					getStationedPoint() const;
		/*! returns an iterator to the first point used to define the targeted line of this measurement*/
		PointConstIter					getFirstTargetPoint() const;
		/*! returns an iterator to the second point used to define the  targeted line of this measurement*/
		PointConstIter					getSecondTargetPoint() const;
	//@}

private:
	//! Copy Assignment Operator
	TOffsetToSpaLineObservation& operator=(const TOffsetToSpaLineObservation &right);
	
	int									observationID;
	DistStConstIter						fStation; /*!< pointer to the dist measurements station */
	OffsetToSpaLineMeasConstIter			fMeasurement; /*!< measurement */
	PointConstIter						fStationedPoint; /*!< stationed point of this measurement */
	PointConstIter						fFirstTargetPoint; /*!< first targeted line of this measurement */
	PointConstIter						fSecondTargetPoint; /*!< second targeted line of this measurement */
	DistConstantConstIter				fMeasConst; /*!< measurement constant of this observation */
	bool								fIsActive; /*< activation level of the observation based on the activation of the other attributes */

};

/*!@name Constants definition */
//@{
/*! List of horizontal angle observations  */
typedef list< TOffsetToSpaLineObservation >	OffsetToSpaLineObsContainer;
/*! Iterator of the list of horizontal angle observations */
typedef OffsetToSpaLineObsContainer::iterator		OffsetToSpaLineObsIter;
/*! Constant iterator of the list of horizontal angle observations */
typedef OffsetToSpaLineObsContainer::const_iterator	OffsetToSpaLineObsConstIter;
//@}

#endif  //SU_ECSP_OBS

/*@}*/

inline bool TOffsetToSpaLineObservation::isActive() const { return fIsActive; }

inline DistStConstIter					TOffsetToSpaLineObservation::getStation() const { return fStation; }
inline OffsetToSpaLineMeasConstIter		TOffsetToSpaLineObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter					TOffsetToSpaLineObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter					TOffsetToSpaLineObservation::getFirstTargetPoint() const { return fFirstTargetPoint; }
inline PointConstIter					TOffsetToSpaLineObservation::getSecondTargetPoint() const { return fSecondTargetPoint; }
