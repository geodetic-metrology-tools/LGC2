//TSpatialDistObservation.h
/*!
	Class for a spatial dist observation

	Patterns: façade (for the components of a spatial dist measurement :
				station, ROM, measurement, spatial points)

	Copyright 2002 CERN EST/SU. All rights reserved.
	
*/

#ifndef SU_SPA_DIST_OBS
#define SU_SPA_DIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class TSpatialDistROM;

#include "TDistMeasStation.h"
#include "TTheodoliteStation.h"
#include "TAMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"

#include "TLength.h"
#include "TSpatialDistMeasurement.h"

#include <string>
using namespace std;


/*!\ingroup lgc++
	@{*/

//! Class for a spatial dist observation
class TSpatialDistObservation 
{
public:

	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TSpatialDistObservation();
	/*! Constructor
	\param station pointer to the dist measurements station
	\param rom pointer to the spatial dist round of measurements
	\param meas pointer to the spatial dist measurement
	\param st iterator to the stationed spatial point
	\param tg iterator to the target spatial point 
	\param cte iterator to a distance constant */
	TSpatialDistObservation(DistStConstIter station, SpatialDistROMConstIter rom, SpatialDistMeasConstIter meas,
		PointConstIter st, PointConstIter tg, DistConstantConstIter cte, int id);
	/*! Copy Constructor */
	TSpatialDistObservation(const TSpatialDistObservation &source);
	/*! Destructor */
	virtual ~TSpatialDistObservation();
	//@}

	/*! Overloaded equality operator */
	bool	operator==(const TSpatialDistObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*!@name Access to observed values */
	//@{
	/*! Returns the spatial distance value as a TLength from the measurement */
	TLength						getSpatialDistValue() const;
	/*! Returns the spatial distance sigma as a TLength from the measurement */
	TLength						getSigmaValue() const;
	/*! Returns the spatial distance sigma Ppm as a TLength from the measurement */
	TLength						getSigmaPpmValue() const;
	//@}

	/*!@name Access to data on parameter prism's height */
	//@{
	/*! Returns the prism's height from the measurement */
	TLength		getPrismHeight() const;
	/*!Returns the prism's status from measurement */
	TAMeasurement::ECalcStatus		getPrismHeightStatus() const;
	/*! Returns the prism's height identifier */
	string		getPHeightName() const;
	//@}

	/*!@name Access to data on parameter instrument's height */
	//@{
	/*! Returns the instrument's height from the station */
	TLength		getInstrumentHeight() const;
	/*! Returns the instrument's status from station */
	TAMeasurement::ECalcStatus		getInstHeightStatus() const;
	/*! Returns the instrument's height identifier */
	string		getIHeightName() const;
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

	/*!@name Set method */
	//@{
	/*! sets the corresponding theodolite station, if necessary (variable instrumnet height) */
	void		setTheodStation(TheodStConstIter iter );
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
	
	/*! Returns a pointer to the dist measurement station */
	DistStConstIter				getStation() const;
	/*! Returns a pointer to the spatial dist round od measurements */
	SpatialDistROMConstIter		getRoundOfMeas() const;
	/*! Returns a pointer to the spatial dist measurement */
	SpatialDistMeasConstIter	getMeasurement() const;
	/*! Returns an iterator to the stationed point */
	PointConstIter				getStationedPoint() const;
	/*! Returns an iterator to the target point */
	PointConstIter				getTargetPoint() const;

	/*! Copy Assignment Operator */
	TSpatialDistObservation& operator=(const TSpatialDistObservation &right);


	int							observationID;
	DistStConstIter				fStation; /*!< iterator to the dist measurements station container */
	SpatialDistROMConstIter		fROM; /*!< iterator to the spatial dist round of measurements container */
	SpatialDistMeasConstIter	fMeasurement; /*!< iterator to the spatial dist measurement container */
	PointConstIter				fStationedPoint; /*!< iterator to the stationed point container */
	PointConstIter				fTargetPoint; /*!< iterator to the target point container */
	DistConstantConstIter		fMeasConst; /*!< iterator the the distance constant of this observation */
	bool						fIsActive;	/*< activation level of the observation based on the activation of the other attributes */ 

	TheodStConstIter			fTheodStation; /*!< iterator to the corresponding theodolite station (instrument height) */

};
/*@}*/

/*!@name Constants definition */
//@{
/*! List of spatial distance observations  */
typedef list< TSpatialDistObservation >		SpaDistObsContainer;
/*! Iterator of the list of spatial distance observations */
typedef SpaDistObsContainer::iterator			SpaDistObsIter;
/*! Constant iterator of the list of spatial distance observations */
typedef SpaDistObsContainer::const_iterator	SpaDistObsConstIter;
//@}


#endif //SU_SPA_DIST_OBS

inline bool TSpatialDistObservation::isActive() const { return fIsActive; }

inline DistStConstIter			TSpatialDistObservation::getStation() const { return fStation; }
inline SpatialDistROMConstIter	TSpatialDistObservation::getRoundOfMeas() const { return fROM; }
inline SpatialDistMeasConstIter	TSpatialDistObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter			TSpatialDistObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter			TSpatialDistObservation::getTargetPoint() const {return fTargetPoint; }
