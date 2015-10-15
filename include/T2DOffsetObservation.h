//T2DOffsetObservation.h
/*!
	Class for a 2D offset observation

	Patterns: façade (for the components of a 2D offset measurement :
				station, ROM, measurement, spatial points)

	Copyright 2002 CERN EST/SU. All rights reserved.
	
*/

#ifndef SU_2D_OFF_OBS
#define SU_2D_OFF_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class T2DOffsetROM;
class T2DOffsetMeasurement;
class TSpatialPoint;
class TSpatialLine;

#include "TDistMeasStation.h"
#include "TLength.h"

/*!\ingroup lgc++
	@{*/

//! Class for a 2D offset observation
class T2DOffsetObservation 
{
public:

	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	T2DOffsetObservation();
	/*! Constructor
	\param station pointer to the dist measurements station
	\param rom pointer to the 2D offset round of measurements
	\param meas pointer to the 2D offset measurement
	\param st pointer to the stationed spatial point
	\param tg pointer to the target spatial line */
	T2DOffsetObservation(DistStIterator station, Off2DROMIterator rom, Off2DMeasIterator meas,
		TSpatialPoint* st, TSpatialLine* tg);
	/*! Destructor */
	virtual ~T2DOffsetObservation();
	//@}


	/*!@name Access to data */
	//@{
	/*! Returns the horizontal offset value from the measurement */
	TLength		getHorOffsetValue() const;
	/*! Returns the horizontal sigma value from the measurement */
	TLength		getHorSigmaValue() const;
	/*! Returns the vertical offset value from the measurement */
	TLength		getVerOffsetValue() const;
	/*! Returns the vertical sigma value from the measurement */
	TLength		getVerSigmaValue() const;

	/*!@name Access methods */
	//@{
	/*! Returns a pointer to the dist measurement station */
	DistStIterator				getStation() const;
	/*! Returns a pointer to the horizontal dist round od measurements */
	Off2DROMIterator			getRoundOfMeas() const;
	/*! Returns a pointer to the horizontal dist measurement */
	Off2DMeasIterator			getMeasurement() const;
	/*! Returns a pointer to the stationed point */
	TSpatialPoint*				getStationedPoint() const;
	/*! Returns a pointer to the target line */
	TSpatialLine*				getTargetLine() const;
	//@}

private:

	/*! Copy Constructor */
	T2DOffsetObservation(const T2DOffsetObservation &source);
	
	/*! Copy Assignment Operator */
	T2DOffsetObservation& operator=(const T2DOffsetObservation &right);


	DistStIterator				fStation; /*!< pointer to the dist measurements station */
	Off2DROMIterator			fROM; /*!< pointer to the horizontal dist round of measurements */
	Off2DMeasIterator			fMeasurement; /*!< pointer to the horizontal dist measurement */
	TSpatialPoint*				fStationedPoint; /*!< pointer to the stationed point */
	TSpatialLine*				fTargetLine; /*!< pointer to the target line */
};
/*@}*/

#endif //SU_2D_OFF_OBS



inline DistStIterator		T2DOffsetObservation::getStation() const { return fStation; }
inline Off2DROMIterator		T2DOffsetObservation::getRoundOfMeas() const { return fROM; }
inline Off2DMeasIterator	T2DOffsetObservation::getMeasurement() const { return fMeasurement; }
inline TSpatialPoint*		T2DOffsetObservation::getStationedPoint() const { return fStationedPoint; }
inline TSpatialLine*		T2DOffsetObservation::getTargetLine() const { return fTargetLine; }
