//TWTWObservation.h
/*!
	Class for a wire to wire observation

	Patterns: façade (for the components of a wire to wire measurement :
				station, measurement, spatial line)

	Copyright 2002 CERN EST/SU. All rights reserved.
	
*/

#ifndef WTW_OBS
#define WTW_OBS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class TWTWDistStation;
class TWireToWireDistMeas;
class TSpatialLine;

/*!\ingroup lgc++
	@{*/

//! Class for a wire to wire observation
class TWTWObservation 
{
public:

	/*!@name Constructors / Destructors */
	//@{
	/*! Default constructor */
	TWTWObservation();
	/*! Constructor
	\param station pointer to the wire to wire measurements station
	\param meas pointer to the wire to wire measurement
	\param tg pointer to the target spatial line */
	TWTWObservation(TWTWDistStation* station, TWireToWireDistMeas* meas,
		TSpatialLine* tg);
	/*! Destructor */
	virtual ~TWTWObservation();
	//@}

	/*!@name Access methods */
	//@{
	/*! Returns a pointer to the wire to wire measurements station */
	TWTWDistStation*			getStation() const;
	/*! Returns a pointer to the wire to wire measurement */
	TWireToWireDistMeas*		getMeasurement() const;
	/*! Returns a pointer to the target line */
	TSpatialLine*				getTargetLine() const;
	//@}

private:

	/*! Copy Constructor */
	TWTWObservation(const TWTWObservation &source);
	
	/*! Copy Assignment Operator */
	TWTWObservation& operator=(const TWTWObservation &right);


	TWTWDistStation*			fStation; /*!< pointer to the wire to wire measurements station */
	TWireToWireDistMeas*		fMeasurement; /*!< pointer to the wire to wire measurement */
	TSpatialLine*				fTargetLine; /*!< pointer to the target line */
};
/*@}*/

#endif //WTW_OBS



inline TWTWDistStation*	TWTWObservation::getStation() const { return fStation; }
inline TWireToWireDistMeas*	TWTWObservation::getMeasurement() const { return fMeasurement; }
inline TSpatialLine*	TWTWObservation::getTargetLine() const { return fTargetLine; }
