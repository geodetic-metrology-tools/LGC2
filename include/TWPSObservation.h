//TWPSObservation.h
/*!
	Class for a wps observation

	Patterns: façade (for the components of a wps measurement :
				station, measurement, spatial points)

	Copyright 2002 CERN EST/SU. All rights reserved.
	
*/

#ifndef WPS_OBS
#define WPS_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class TWPSStation;
class TWPSMeasurement;
class TSpatialPoint;
class TSpatialLine;

/*!\ingroup lgc++
	@{*/

//! Class for a wps observation
class TWPSObservation 
{
public:

	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TWPSObservation();
	/*! Constructor
	\param station pointer to the wps measurements station
	\param meas pointer to the wps measurement
	\param st pointer to the stationed spatial point
	\param tg pointer to the target spatial line */
	TWPSObservation(TWPSStation* station, TWPSMeasurement* meas,
		TSpatialPoint* st, TSpatialLine* tg);
	/*! Destructor */
	virtual ~TWPSObservation();
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns a pointer to the wps measurements station */
	TWPSStation*			getStation() const;
	/*! Returns a pointer to the wps measurement */
	TWPSMeasurement*		getMeasurement() const;
	/*! Returns a pointer to the stationed point */
	TSpatialPoint*			getStationedPoint() const;
	/*! Returns a pointer to the target line */
	TSpatialLine*			getTargetLine() const;
	//@}

private:

	/*! Copy Constructor */
	TWPSObservation(const TWPSObservation &source);
	
	/*! Copy Assignment Operator */
	TWPSObservation& operator=(const TWPSObservation &right);


	TWPSStation*			fStation; /*!< pointer to the wps measurements station */
	TWPSMeasurement*		fMeasurement; /*!< pointer to the wps measurement */
	TSpatialPoint*			fStationedPoint; /*!< pointer to the stationed point */
	TSpatialLine*			fTargetLine; /*!< pointer to the target line */
};
/*@}*/

#endif //WPS_OBS



inline TWPSStation*	TWPSObservation::getStation() const { return fStation; }
inline TWPSMeasurement*	TWPSObservation::getMeasurement() const { return fMeasurement; }
inline TSpatialPoint*	TWPSObservation::getStationedPoint() const { return fStationedPoint; }
inline TSpatialLine*	TWPSObservation::getTargetLine() const { return fTargetLine; }
