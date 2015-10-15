// TOffsetToVerLineObservation.h
/*!
	Class for an ECVE observation

	Copyright 2003 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_ECVE_OBS
#define SU_ECVE_OBS


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <string>
using namespace std;


#include "TDistMeasStation.h"
#include "TALineMeasurement.h"
#include "TOffsetToVerLineMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"



////////////////////////////////////////////////////////////////

/*! \addtogroup lgc++
	@{*/

//! Class for a horizontal angle measurement 
class  TOffsetToVerLineObservation
{
public:

	/*!@name Constructors and Destructors */
	//@{
		//! Default Constructor 
		TOffsetToVerLineObservation();
		/*!Constructor
		\param meas a pointer to the measurement
		\param stationedPt an iterator of the stationed points list 
		\param targetedLi an iterator of the point which define the targeted Line 
		\param cte distance measurement constant*/
		TOffsetToVerLineObservation(DistStConstIter station, OffsetToVerLineMeasConstIter meas, PointConstIter stationedPt, PointConstIter targetPt, DistConstantConstIter cte, int id);
		//! Copy Constructor 
		TOffsetToVerLineObservation(const TOffsetToVerLineObservation &source);
		//! Destructor
		virtual  ~TOffsetToVerLineObservation();
	//@}


	/*! Overloaded equality operator */
	virtual bool  operator==(const TOffsetToVerLineObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*@name Access to observed values */
	//@{
		/*! returns the horizontal angle value  from the measurement added to the constant angle (if there is one) */
		TLength		getOffsetToVerLineValue() const;
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
		/*! Returns the position vector of the target point */
		TPositionVector						getTargetPosition() const;
		/*! Returns the spatial status of the target point */
		TSpatialStatus::ESpatialStatus		getTargetPosStatus() const;
		/*! Returns the concatenated name of the target point */
		string								getTargetPosName() const;
		/*! Returns the concatenated name of the target Line*/
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
		/*! Converts the stationed point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DTargetPosVec() const;

		int								getObservationID() const { return observationID; }
	//@}


protected:

	/*!@name Access methods */
	//@{
		/*! Returns a pointer to the dist measurement station */
		DistStConstIter					getStation() const;
		/*! returns a pointer to the measurement*/
		OffsetToVerLineMeasConstIter		getMeasurement() const;
		/*! returns an iterator to the stationed point of this measurement*/
		PointConstIter					getStationedPoint() const;
		/*! returns an iterator to the targeted line of this measurement*/
		PointConstIter					getTargetPoint() const;
	//@}

private:
	//! Copy Assignment Operator
	TOffsetToVerLineObservation& operator=(const TOffsetToVerLineObservation &right);
	
	int									observationID;
	DistStConstIter						fStation; /*!< pointer to the dist measurements station */
	OffsetToVerLineMeasConstIter			fMeasurement; /*!< measurement */
	PointConstIter						fStationedPoint; /*!< stationed point of this measurement */
	PointConstIter						fTargetPoint; /*!< targeted line of this measurement */
	DistConstantConstIter				fMeasConst; /*!< measurement constant of this observation */
	bool								fIsActive; /*< activation level of the observation based on the activation of the other attributes */

};

/*!@name Constants definition */
//@{
/*! List of horizontal angle observations  */
typedef list< TOffsetToVerLineObservation >	OffsetToVerLineObsContainer;
/*! Iterator of the list of horizontal angle observations */
typedef OffsetToVerLineObsContainer::iterator		OffsetToVerLineObsIter;
/*! Constant iterator of the list of horizontal angle observations */
typedef OffsetToVerLineObsContainer::const_iterator	OffsetToVerLineObsConstIter;
//@}

#endif  //SU_ECVE_OBS

/*@}*/

inline bool TOffsetToVerLineObservation::isActive() const { return fIsActive; }

inline DistStConstIter					TOffsetToVerLineObservation::getStation() const { return fStation; }
inline OffsetToVerLineMeasConstIter		TOffsetToVerLineObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter					TOffsetToVerLineObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter					TOffsetToVerLineObservation::getTargetPoint() const { return fTargetPoint; }
