//TZenithDistObservation.h
/*!
	Class for a zenithal observation

	Patterns: façade (for the components of a zenithal dist measurement :
				station, ROM, measurement, spatial points)

	Copyright 2002 CERN EST/SU. All rights reserved.
	
*/

#ifndef SU_ZEN_DIST_OBS
#define SU_ZEN_DIST_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class TZenithDistROM;

#include "TTheodoliteStation.h"
#include "TAMeasurement.h"
#include "TSpatialPoint.h"
#include "TSpatialPosition.h"

#include "TLength.h"
#include "TAngle.h"
#include "TZenithDistMeasurement.h"

#include <string>
using namespace std;

/*!\ingroup lgc++
	@{*/

//! Class for a zenith dist observation
class TZenithDistObservation 
{
public:

	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TZenithDistObservation();
	/*! Constructor
	\param station pointer to the zenith measurements station
	\param rom pointer to the zenith round of measurements
	\param meas pointer to the zenith measurement
	\param st iterator of the stationed spatial points container
	\param tg iterator of the target spatial points container*/
	TZenithDistObservation(TheodStConstIter station, ZenithDistROMConstIter rom, ZenithDistMeasConstIter meas,
		PointConstIter st, PointConstIter tg, int id);
	/*! Copy Constructor */
	TZenithDistObservation(const TZenithDistObservation &source);
	/*! Destructor */
	virtual ~TZenithDistObservation();
	//@}

	/*! Overloaded equality operator */
	virtual bool operator==(const TZenithDistObservation& right) const;

	/*! returns true if the observation is active */
	bool	isActive() const;

	/*!@name Access to data */
	//@{
	/*! Returns the zenithal angle value as a TAngle from the measurement */
	TAngle		getZenAngleValue() const;
	/*! Returns the sigma as a TAngle from the measurement */
	TAngle		getSigmaValue() const;
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
	
	/*! Copy Assignment Operator */
	TZenithDistObservation& operator=(const TZenithDistObservation &right);
	
	/*!@name Access methods */
	//@{
	/*! Returns a pointer to the zenith measurement station */
	TheodStConstIter				getStation() const;
	/*! Returns a pointer to the zenith round od measurements */
	ZenithDistROMConstIter		getRoundOfMeas() const;
	/*! Returns a pointer to the zenith measurement */
	ZenithDistMeasConstIter		getMeasurement() const;
	/*! Returns an iterator of the stationed points container*/
	PointConstIter				getStationedPoint() const;
	/*! Returns an iterator of the target points container */
	PointConstIter				getTargetPoint() const;
	//@}

	int							observationID;
	TheodStConstIter			fStation; /*!< pointer to the zenith measurements station */
	ZenithDistROMConstIter		fROM; /*!< pointer to the zenith round of measurements */
	ZenithDistMeasConstIter		fMeasurement; /*!< pointer to the zenith measurement */
	PointConstIter				fStationedPoint; /*!< pointer to the stationed point */
	PointConstIter				fTargetPoint; /*!< pointer to the target point */
	bool						fIsActive; /*< activation level of the observation based on the activation of the other attributes */
};
/*@}*/

/*!@name Constants definition */
//@{
/*! List of zenithal angle observations  */
typedef list< TZenithDistObservation >			ZenDistObsContainer;
/*! Iterator of the list of zenithal angle observations */
typedef ZenDistObsContainer::iterator			ZenDistObsIter;
/*! Constant iterator of the list of zenithal angle observations */
typedef ZenDistObsContainer::const_iterator		ZenDistObsConstIter;
//@}



#endif //SU_ZEN_DIST_OBS

inline bool TZenithDistObservation::isActive() const { return fIsActive; }

inline TheodStConstIter			TZenithDistObservation::getStation() const { return fStation; }
inline ZenithDistROMConstIter	TZenithDistObservation::getRoundOfMeas() const { return fROM; }
inline ZenithDistMeasConstIter	TZenithDistObservation::getMeasurement() const { return fMeasurement; }
inline PointConstIter			TZenithDistObservation::getStationedPoint() const { return fStationedPoint; }
inline PointConstIter			TZenithDistObservation::getTargetPoint() const {return fTargetPoint; }
	
