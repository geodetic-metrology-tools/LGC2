// 
// TWPSStation.h : Header File
/*!
	Class for a WPS instrument station

	Pattern:

	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////// 

#ifndef SU_WPS_STATION
#define SU_WPS_STATION

#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
#include <list>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include "TWPSMeasurement.h"
#include "TAInstrumentStation.h"
#include "TRotationMatrix.h"

//typedefs
typedef string InstNb;

/*! \ingroup spatialmeasurements
	@{*/

//! Class WPS instrument station
class TWPSStation : public TAInstrumentStation{ 

public:

	/*!@name Constructors and destructors */
	//@{
	/*! Constructor
	\param setupPt station's setup point's name */
	TWPSStation(TSpatialPointName setupPt);
	/*! Copy constructor */
	TWPSStation(const TWPSStation& source);
	/*! Destructor */
	~TWPSStation();
	//@}

	/*!@name Overloaded operators */
	//@{
	/*! Copy assignment operator */
	TWPSStation& operator=(const TWPSStation& source);
	//DANGER : shallow copy 
	/*! Equality operator */
	virtual bool							operator==(const TWPSStation& right) const;
	//@}

	/*!@name Access methods */
	//@{
	/*! return the list of WPS measurement */
	virtual WPSMeasIterator					getWPSMeas();
	/*! return the const. list of WPS measurement */
	virtual WPSMeasConstIter				getWPSMeas() const;
	/*! return the omega angle */
	virtual TAngle							getOmega() const;
	/*! return the phi angle */
	virtual TAngle							getPhi() const;
	/*! return the kappa angle */
	virtual TAngle							getKappa() const;
	/*! return the omega's status */
//	virtual TUnknownAngle::EUnknownStatus	getOmegaStatus() const;
	/*! return the phi's status */
//	virtual TUnknownAngle::EUnknownStatus	getPhiStatus() const;
	/*! return the kappa's status */
//	virtual TUnknownAngle::EUnknownStatus	getKappaStatus() const;
	//@}

	/*!@name Set methods*/
	//@{
	/*! set a new wps measurement 
	\return a pointer to this new wps measurement */
	virtual TWPSMeasurement*				setWPSMeas(TWPSMeasurement*);
	/*! set omega's status for calculation */
//	virtual	void							setOmegaStatus(TUnknownAngle::EUnknownStatus os);
	/*! set phi's status for calculation */
//	virtual	void							setPhiStatus(TUnknownAngle::EUnknownStatus os);
	/*! set kappa's status for calculation */
//	virtual	void							setKappaStatus(TUnknownAngle::EUnknownStatus os); */
	//@}

private :
	TWPSMeasurement					fWPSMeas; /*!< wps measurements */

	TAngle							fOmega; /*!< omega angle */
	TAngle							fPhi; /*!< phi angle */
	TAngle							fKappa; /*!< kappa angle */
//	TUnknownAngle::EUnknownStatus	fOmegaStatus; /*!< omega's status */
//	TUnknownAngle::EUnknownStatus	fPhiStatus; /*!< phi's status */
//	TUnknownAngle::EUnknownStatus	fKappaStatus; /*!< kappa's status */



};

#endif

/*@}*/


/*!@name Typedefs*/
//@{
//!Type of the container used to store WPS stations
typedef list< TWPSStation > WPSStContainer;
//!Type of an iterator pointing to an element of the WPS stations container
typedef WPSStContainer::iterator WPSStIterator;
//!Type of a const iterator pointing to an element of the WPS stations container
typedef WPSStContainer::const_iterator WPSStConstIter;	
//@}











//class TWorkingStations;

//#include "TUnknownLength.h"
//#include "TUnknownAngle.h"
//#include "UEOIndices.h"

//	TWPSStation(TWorkingStations* parent, TWorkingPoints::PointIterator iterSetupPt, TAngle omega, TAngle phi, TAngle kappa);

/*	
	virtual void						sendDisactivatedToChildren();
	virtual void						sendActivatedToChildren();

	virtual void						sendEnoughTotalEqns();
	virtual void						childReadyToActivate(int, int, int);
	virtual void						activateReadyChildren();
*/
