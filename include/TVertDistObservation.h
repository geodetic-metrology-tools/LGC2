//TVertDistObservation.h
/*!
	Class for a vertical dist observation (levelling)

	Patterns: façade (for the components of a vertical dist measurement :
				station, round of meas., measurement, spatial point)

	Copyright 2002 CERN EST/SU. All rights reserved.
	
*/

#ifndef VERT_DIST_OBS
#define VERT_DIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class TVerticalDistROM;

#include "TLevelStation.h"
#include "TAMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"

#include "TLength.h"
#include "TVerticalDistMeasurement.h"

#include <string>
using namespace std;

/*!\ingroup lgc++
	@{*/

//! Class for a vertical dist observation (levelling)
class TVertDistObservation 
{
public:

	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TVertDistObservation();
	/*! Constructor
	\param station pointer to the vertical dist measurements station (level)
	\param rom pointer to the vertical dist measurements ROM 
	\param meas pointer to the vertical dist measurement
	\param ref iterator to the reference spatial point 
	\param cte distance measurement constant*/
	TVertDistObservation(LevelStConstIter station, VertDistROMConstIter rom,
		VertDistMeasConstIter meas, PointConstIter ref, PointConstIter tg, DistConstantConstIter cte, int id);
	/*! Copy Constructor */
	TVertDistObservation(const TVertDistObservation &source);
	/*! Destructor */
	virtual ~TVertDistObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TVertDistObservation& right) const;
	
	/*! returns true if the observation is active */
	bool	isActive() const;

	/*!@name Access to observed values */
	//@{
	/*! Returns the vertical distance value as a TLength from the measurement */
	TLength						getVertDistValue() const;
	/*! Returns the vertical distance sigma as a TLength from the measurement */
	TLength						getSigmaValue() const;
	//@}

	/*!@name Access to data on parameter stationed point */
	//@
		/*! Returns the position vector of the reference point */
		TPositionVector						getRefPosition() const;
		/*! Returns the spatial status of the reference point */
		TSpatialStatus::ESpatialStatus		getRefPosStatus() const;
		/*! Returns the concatenated name of the reference point */
		string								getRefPosName() const;
	//@}

	/*!@name Access to data on parameter target point */
	//@{
		/*! Returns the position vector of the target point */
		TPositionVector						getTargetPosition() const;
		/*! Returns the spatial status of the target point */
		TSpatialStatus::ESpatialStatus		getTargetPosStatus() const;
		/*! Returns the concatenated name of the target point */
		string								getTargetPosName() const;
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
		TPositionVector					get3DRefPosVec() const;
		/*! Converts the stationed point from 2D+H to 3D cartesian coordinate system, if necessary */
		TPositionVector					get3DTargetPosVec() const;

		int								getObservationID() const { return observationID; }
	//@}

private:
	
	/*!@name Access methods */
	//@{
	/*! Returns a pointer to the vertical dist measurements station */
	LevelStConstIter				getStation() const;
	/*! Returns a pointer to the vertical dist measurements ROM */
	VertDistROMConstIter			getRoundOfMeas() const;
	/*! Returns a pointer to the vertical dist measurement */
	VertDistMeasConstIter	getMeasurement() const;
	/*! Returns a pointer to the reference point */
	PointConstIter				getReferencePoint() const;
	/*! Returns an iterator of the target points container */
	PointConstIter				getTargetPoint() const;
	//@}
	
	/*! Copy Assignment Operator */
	TVertDistObservation& operator=(const TVertDistObservation &right);


	int							observationID;
	LevelStConstIter			fStation; /*!< pointer to the vert. dist. measurements station */
	VertDistROMConstIter		fROM;	/*!< pointer to the vert. dist. measurements ROM */
	VertDistMeasConstIter		fMeasurement; /*!< pointer to the vert. dist. measurement */
	PointConstIter				fRefPoint; /*!< pointer to the reference point */
	PointConstIter				fTargetPoint; /*!< pointer to the target point */
	DistConstantConstIter		fMeasConst;

	bool						fIsActive; /*< activation level of the observation based on the activation of the other attributes */
};
/*@}*/

/*!@name Constants definition */
//@{
/*! List of horizontal distance observations  */
typedef list< TVertDistObservation >			VertDistObsContainer;
/*! Iterator of the list of horizontal distance observations */
typedef VertDistObsContainer::iterator				VertDistObsIter;
/*! Constant iterator of the list of horizontal distance observations */
typedef VertDistObsContainer::const_iterator		VertDistObsConstIter;
//@}

#endif //VERT_DIST_OBS

inline bool TVertDistObservation::isActive() const { return fIsActive; }

inline LevelStConstIter			TVertDistObservation::getStation() const { return fStation; }
inline VertDistROMConstIter		TVertDistObservation::getRoundOfMeas() const { return fROM; }
inline VertDistMeasConstIter		TVertDistObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter			TVertDistObservation::getReferencePoint() const { return fRefPoint; }
inline PointConstIter			TVertDistObservation::getTargetPoint() const { return fTargetPoint; }
