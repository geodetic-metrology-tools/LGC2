// T2DOffsetMeasurement.h : header file
/*! 
	Class for a 2D Offset measurement

	Copyright 2000 CERN EST/SU. All rights reserved.
*/
// 


#ifndef SU_2DOFFSET_MEASUREMENT
#define SU_2DOFFSET_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
class T2DOffsetROM;

#include "TAObjectMeasurement.h"
#include "TLength.h"
/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//!Class for a bidimensional(horizontal + vertical) offset to a line measurement
class T2DOffsetMeasurement :public TAObjectMeasurement{
public :
	/*!@name Constructors / Destructor*/
	//@{
	/*!Default constructor */
	T2DOffsetMeasurement();
	/*! Constructor 
	\param setupName the name of the station's setup point
	\param lineName the name of the measured spatial line
	\param obsHOffset the observed horizontal offset
	\param hSigma the precision of the horizontal offset measurement
	\param obsVOffset the observed vertical offset
	\param vSigma the precision of the vertical offset measurement
	*/
	T2DOffsetMeasurement(/*TSpatialPointName setupName,*/ string lineName, TLength obsHOffset, TLength hSigma, TLength obsVOffset, TLength vSigma);
	/*!Copy constructor */
	T2DOffsetMeasurement(const T2DOffsetMeasurement &source);
	/*!Destructor */
	virtual ~T2DOffsetMeasurement();
	//@}

	//!Copy assignement operator
	T2DOffsetMeasurement&						operator=(const T2DOffsetMeasurement& source);
	

	/*!@name Overloaded operators*/
	//@{
	/*!Less than operator */
	virtual bool								operator<(const T2DOffsetMeasurement&) const;
	/*!Equality operator */
	virtual bool								operator==(const T2DOffsetMeasurement&) const;
	//@}

	/*!@return a string indicating this measurement is a 2D offset measurement*/
	virtual string								getMeasKind() const;

	/*!@name Access methods*/
	//@{
	/*!@return the observed horizontal offset*/
	virtual TLength								getHorOffset() const;
	/*!@return the observed horizontal offset's precision*/
	virtual TLength								getHorSigma() const;
	/*!@return the observed vertical offset*/
	virtual TLength								getVertOffset() const;
	/*!@return the observed vertical offset's precision*/
	virtual TLength								getVertSigma() const;
	// Temporary
	string	getLineName() const;
	/* return an iterator pointing to the measured spatial line */
//	virtual TWorkingTargetObjects::SpLineIterator	getTargetLine() const;
	//@}

	/*!@name Set methods*/
	//@{
	/*!Sets the horizontal sigma a priori if it isn't done yet */
	virtual void								ifNotDoneSetHSigma(TLength sigma);
	/*!Sets the vertical sigma a priori if it isn't done yet */
	virtual void								ifNotDoneSetVSigma(TLength sigma);
	// Temporary
	void	setLineName(string ln);
	/* Sets the target spatial line from the two end point names
	\param wp a pointer to the TWorkingPoints object
	\param wto a pointer to the TWorkingTargetObjects object*/
//	virtual bool								setTargetLine(TWorkingTargetObjects* wto);
	//@}


private:


//	TSpatialPointName						fSetupPtName; /*!< setup point's name */
	string									fLineName; /*!< target line's name */
	TLength									fVertOffset; /*!< observed vertical offset */
	TLength									fVertSigma; /*!< vertical offset's error */
	TLength									fHorOffset; /*!< observed horizontal offset */
	TLength									fHorSigma; /*!< horizontal offset's error */
//	int										fActiveMeasCount; /*!< storage value for the number of active measurements */
//	int										fActiveEqCount; /*!< storage value for the number of active equations */
//	int										fActiveUnkCount;/*!< storage value for the number of active unknowns */


//	TWorkingTargetObjects::SpLineIterator		fTargetLine;
//	bool									fVUsedInCalc;
//	bool									fHUsedInCalc;

};

/*@}*/

	/*!@name Typedef */
	//@{
	/*! Type of the container used to store the 2D offset measurements */
	typedef list<T2DOffsetMeasurement> Off2DContainer;
	/*!Type of the iterator pointing to an element of the 2D offset container */
	typedef Off2DContainer::iterator Off2DMeasIterator;
	/*!Type of the const iterator pointing to an element of the 2D offset container */
	typedef Off2DContainer::const_iterator Off2DMeasConstIter;
	//@}


#endif














/*@}*/

//class TWorkingStations;
//class TWorkingPoints;
//#include "TSpatialLine.h"
//#include "TWorkingTargetObjects.h"
//#include "TVMeasurableEntityLstnr.h"

//#include "UEOIndices.h"


	/*Constructor */
/*	\param parent a reference to the station from which the measurement is made
	\param iterSpl an iterator pointing to the measured spatial line
	\param obsHOffset the observed horizontal offset
	\param hSigma the precision of the horizontal offset measurement
	\param obsVOffset the observed vertical offset
	\param vSigma the precision of the vertical offset measurement
	*/
//	T2DOffsetMeasurement(T2DOffsetROM& parent, TWorkingTargetObjects::SpLineIterator iterSpl, TLength obsHOffset, TLength hSigma, TLength obsVOffset, TLength vSigma);

	/*Adds to or creates the relevant dist. meas. station, using the latter's setup point's name
	\param wp a pointer to the TWorkingPoints object
	\param ws a pointer to the TWorkingStations object*/
//	virtual bool								addToStation(TWorkingPoints* wp, TWorkingStations* ws);

	/*!@name Active/Inactive related*/
	//@{
	/*!Disactivates the horizontal offset meas. */
//	virtual void								disactivateHOffset();
	/*!Activates the horizontal offset meas. */
//	virtual void								activateHOffset();
	/*! return a true boolean if the horizontal offset is active */
//	virtual bool								hActive() const;
	/*!Disactivates the vertical offset meas. */
//	virtual void								disactivateVOffset();
	/*!Activates the vertical offset meas. */
//	virtual void								activateVOffset();
	/*! return a true boolean if the vertical offset is active */
//	virtual bool								vActive() const;
	/*!Tells the parent ROM that this measurement is disactivated */
//	virtual void								sendDisactivatedToParent();
	/*!Tells the parent ROM that this measurement is activated */
//	virtual void								sendActivatedToParent();
	//@}

	//	T2DOffsetROM*							fParentROM;
