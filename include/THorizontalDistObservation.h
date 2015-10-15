//THorizontalDistObservation.h
/*!
	Class for a horizontal dist observation

	Patterns: façade (for the components of an horizontal dist measurement :
				station, ROM, measurement, spatial points)

	Copyright 2002 CERN EST/SU. All rights reserved.
	
*/

#ifndef SU_HOR_DIST_OBS
#define SU_HOR_DIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class THorizontalDistROM;

#include "TDistMeasStation.h"
#include "TAMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"

#include "TLength.h"
#include "THorizontalDistMeas.h"

#include <string>
using namespace std;

/*!\ingroup lgc++
	@{*/

//! Class for a horizontal dist observation
class THorizontalDistObservation 
{
public:

	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	THorizontalDistObservation();
	/*! Constructor
	\param station pointer to the dist measurements station
	\param rom pointer to the horizontal dist round of measurements
	\param meas pointer to the horizontal dist measurement
	\param st pointer to the stationed spatial point
	\param tg pointer to the target spatial point 
	\param cte distance measurement constant*/
	THorizontalDistObservation(DistStConstIter station, HorDistROMConstIter rom, HorDistMeasConstIter meas,
		PointConstIter st, PointConstIter tg, DistConstantConstIter cte, int id);
	/*! Copy Constructor */
	THorizontalDistObservation(const THorizontalDistObservation &source);
	/*! Destructor */
	virtual ~THorizontalDistObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const THorizontalDistObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*!@name Access to observed values */
	//@{
	/*! Returns the horizontal distance value as a TLength from the measurement */
	TLength						getHorDistValue() const;
	/*! Returns the horizontal distance sigma as a TLength from the measurement */
	TLength						getSigmaValue() const;
	/*! Returns the horizontal distance sigma Ppm as a TLength from the measurement */
	TLength						getSigmaPpmValue() const;
	//@}

	/*!@name Access to data on parameter stationed point */
	//@
	/*! Returns the position vector of the stationed point */
	TPositionVector		getStationedPosition() const;
	/*! Returns the spatial status of the stationed point */
	TSpatialStatus::ESpatialStatus		getStationedPosStatus() const;
	/*! Returns the concatenated name of the stationed point */
	string		getStationedPosName() const;
	//@}

	/*!@name Access to data on parameter target point */
	//@{
	/*! Returns the position vector of the target point */
	TPositionVector		getTargetPosition() const;
	/*! Returns the spatial status of the target point */
	TSpatialStatus::ESpatialStatus		getTargetPosStatus() const;
	/*! Returns the concatenated name of the target point */
	string		getTargetPosName() const;
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



private:

	/*!@name Access methods */
	//@{
	/*! Returns a pointer to the dist measurement station */
	DistStConstIter				getStation() const;
	/*! Returns a pointer to the horizontal dist round od measurements */
	HorDistROMConstIter			getRoundOfMeas() const;
	/*! Returns a pointer to the horizontal dist measurement */
	HorDistMeasConstIter			getMeasurement() const;
	/*! Returns a pointer to the stationed point */
	PointConstIter				getStationedPoint() const;
	/*! Returns a pointer to the target point */
	PointConstIter				getTargetPoint() const;
	//@}
	
	/*! Copy Assignment Operator */
	THorizontalDistObservation& operator=(const THorizontalDistObservation &right);


	int							observationID;
	DistStConstIter				fStation; /*!< pointer to the dist measurements station */
	HorDistROMConstIter			fROM; /*!< pointer to the horizontal dist round of measurements */
	HorDistMeasConstIter			fMeasurement; /*!< pointer to the horizontal dist measurement */
	PointConstIter				fStationedPoint; /*!< pointer to the stationed point */
	PointConstIter				fTargetPoint; /*!< pointer to the target point */
	DistConstantConstIter		fMeasConst; /*!< distance measurement constant */
	bool						fIsActive;	/*< activation level of the observation based on the activation of the other attributes */ 
};
/*@}*/

/*!@name Constants definition */
//@{
/*! List of horizontal distance observations  */
typedef list< THorizontalDistObservation >		HorDistObsContainer;
/*! Iterator of the list of horizontal distance observations */
typedef HorDistObsContainer::iterator				HorDistObsIter;
/*! Constant iterator of the list of horizontal distance observations */
typedef HorDistObsContainer::const_iterator		HorDistObsConstIter;
//@}


#endif //SU_HOR_DIST_OBS

inline bool THorizontalDistObservation::isActive() const { return fIsActive; }

inline DistStConstIter		THorizontalDistObservation::getStation() const { return fStation; }
inline HorDistROMConstIter	THorizontalDistObservation::getRoundOfMeas() const { return fROM; }
inline HorDistMeasConstIter	THorizontalDistObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter		THorizontalDistObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter		THorizontalDistObservation::getTargetPoint() const {return fTargetPoint; }
