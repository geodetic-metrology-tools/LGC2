// TWPSMeasurement.h : header file
/*! 
	Class for a 2D WPS measurement

	Pattern: 

	Copyright 2002 CERN EST/SU. All rights reserved.
*/



#ifndef SU_WPS_MEASUREMENT
#define SU_WPS_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
class TWPSStation;

#include "TAObjectMeasurement.h"
#include "TLength.h"

/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class Definition
class TWPSMeasurement:public TAObjectMeasurement{

public :
	/*!@name Constructors and Destructors */
	//@{
	/*! Default constructor */
	TWPSMeasurement();
	/*! Constructor
	\param wireName name of the target wire
	\param setupName Name of the measurement's station's setup point
	\param obsTOffset transversal offset measurement
	\param tSigma transversal offset measurement sigma a priori
	\param obsVOffset vertical offset measurement
	\param vSigma vertical offset measurement sigma a priori*/
	TWPSMeasurement(string wireName, string setupName, TLength obsTOffset, TLength tSigma, TLength obsVOffset, TLength vSigma /*TSpatialPointName end1Name, TSpatialPointName end2Name, TSpatialPointName setupName,*/);
	/*! Destructor */
	virtual ~TWPSMeasurement();
	//@}
	
	/*!@name Overloaded operators*/
	//@{
	/*! Copy assignment operator */
	TWPSMeasurement&						operator=(const TWPSMeasurement& source);
	//to be used carefully : shallow copy (see NetworkSurveyMeas)
	/*! Less than operator */
	virtual bool								operator<(const TWPSMeasurement&) const;
	/*! Equality operator */
	virtual bool								operator==(const TWPSMeasurement&) const;
	//@}

	/*!@name Access methods */
	//@{
	/*! Return a string indicating this measurement is a WPS measurement */
	virtual string								getMeasKind() const;
	/*! return the observed vertical offset measurement */
	virtual TLength								getVertOffset() const;
	/*! return the vertical offset sigma a priori */
	virtual TLength								getVertSigma() const;
	/*! return the observed transversal offset */
	virtual TLength								getTransOffset() const;
	/*! return the transversal offset sigma a priori */
	virtual TLength								getTransSigma() const;
	/*! return the target wire's name */
	virtual string								getTgWireName() const;
	/*! return the setup point's name */
	virtual TSpatialPointName					getSetupPointName() const;
	//@}

	/*!@name Set methods */
	/*! Sets the transverse sigma a priori if it isn't done yet */
	virtual void								ifNotDoneSetTSigma(TLength sigma);
	/*! Sets the vertical sigma a priori if it isn't done yet */
	virtual void								ifNotDoneSetVSigma(TLength sigma);
	/*! Sets the target wire's name */
	virtual void								setTgWireName(string twn);
	//@}

private:

	//alternative to the TWorkingTargetObject line
	string									fTgWireName; /*!< name of the target wire */

//	TSpatialPointName						fWireEnd1Name; /*!< name of the wire's ending point 1 */
//	TSpatialPointName						fWireEnd2Name; /*!< name of the wire's ending point 2 */
	TSpatialPointName						fSetupPtName; /*!< setup point's name */
	TLength									fVertOffset; /*!< observed vertical offset */
	TLength									fVertSigma; /*!< vertical offset's error */
	TLength									fTransOffset; /*!< observed transversal offset */
	TLength									fTransSigma; /*!< transversal offset's error */

};
/*@}*/


	/*!@name Type definitions */
	//@{
	typedef	TWPSMeasurement* WPSMeasIterator;
	typedef	const TWPSMeasurement* WPSMeasConstIter;
	//@}


#endif



















//class TWorkingStations;
//class TWorkingPoints;
//#include "TWire.h"
//#include "TWorkingTargetObjects.h"
//#include "TVMeasurableEntityLstnr.h"
//#include "UEOIndices.h"
//	TWPSMeasurement(TWPSStation& parent, TWorkingTargetObjects::WireIterator iterW, TLength vertMeas, TLength vertSigma, TLength transMeas, TLength transSigma);

//	virtual TWorkingTargetObjects::WireIterator	getTargetWire() const;

	/**Sets the target wire from the two end point names
	@param wto a pointer to the TWorkingTargetObjects object*/
//	virtual bool								setTargetWire(TWorkingPoints* wp, TWorkingTargetObjects* wto);

	/**Creates the relevant WPS station, using the latter's setup point's name
	@param wp a pointer to the TWorkingPoints object
	@param ws a pointer to the TWorkingStations object*/
//	virtual bool								addToStation(TWorkingPoints* wp, TWorkingStations* ws);

/*	virtual void								sendDisactivatedToParent();
	virtual void								sendActivatedToParent();
*/	/*! disactivates observed transversal offset for calculation */
//	virtual void								disactivateTOffset();
	/*! activates observed transversal offset for calculation */
//	virtual void								activateTOffset();
	/*! tells if the transverse offset meas is active or not */
//	virtual bool								tActive() const;
	/*! disactivates observed vertical offset for calculation */
//	virtual void								disactivateVOffset();
	/*! activates observed vertical offset for calculation */
//	virtual void								activateVOffset();
	/*! tells if the vertical offset meas is active or not */
//	virtual bool								vActive() const;


//	TWorkingTargetObjects::WireIterator		fTargetWire;
//	TWPSStation*							fParentStation;
//	bool									fVUsedInCalc; /*!< indicator for vertical offset's activation in calculation */
//	bool									fTUsedInCalc; /*!< indicator for transversal offset's activation in calculation */
//	int										fActiveMeasCount; /*!< storage value for the number of active measurements */
//	int										fActiveEqCount; /*!< storage value fot the number of active equations */
//	int										fActiveUnkCount; /*!< storage value fot the number of active unknowns */
