//
// THorAngleMeasurement.h : header file
//
// Class for a survey measurement as defined for survey purposes
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_HOR_ANGLE_MEASUREMENT
#define SU_HOR_ANGLE_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////
// Forward declarations
//
class THorAngleROM;

#include "TAPointMeasurement.h"
#include "TAngle.h"

/////////////////////////////////
/*! \ingroup spatialmeasurements
	@{*/

//! Class for a horizontal angle measurement
class	THorAngleMeasurement : public TAPointMeasurement{

public :

	/*!@name Constructors / Destructor*/
	//@{
	//!Default constructor
	THorAngleMeasurement();
	/*!Constructor
	@param parent a reference to the THorAngleROM from which this measurement is part
	@param iterTg an iterator pointing to the target TSpatialPoint
	@param obsAngle a TAngle object representing the measured angle
	@param sigma a TAngle object representing the measurement's a priori precision*/
	THorAngleMeasurement(/*THorAngleROM& parent,*/ TSpatialPointName iterTg, TAngle obsAngle, TAngle sigma);
	/*!Constructor
	@param refName the measurement's station's setup point's name
	@param tgfName the measurement's target point's name
	@param obsDist the observed horizontal direction as a TAngle
	@param sigma the observed angle's precision as a TAngle*/
//	THorAngleMeasurement(/*TSpatialPointName stName, */TSpatialPointName tgName, TAngle obsAng, TAngle sigma);
	/*!Destructor */
	virtual ~THorAngleMeasurement();
	//@}

	/*!@name Overloaded operators */
	//@{
	/*! Copy assignement operator */
	THorAngleMeasurement&		operator=(const THorAngleMeasurement& source);
	//to be used carefully : shallow copy (see NetworkSurveyMeas)
	/*! Equality operator */
	virtual bool				operator==(const THorAngleMeasurement& right) const;
	//@}

	/*! returns a pointer to this THorAngleMeasurement */
	virtual THorAngleMeasurement*			getPointer() const;

	/*!@return a string indicating this measurement is a hor. angle measurement*/
	virtual string							getMeasKind() const;

	/*!@return the observed angle's value as a TAngle instance*/
	virtual TAngle							getAngleValue() const;
	/*!@return the observed angle's precision as a TAngle instance*/
	virtual TAngle							getSigma() const;
	
	/*!Sets the sigma a priori if it isn't done yet */
	virtual void				ifNotDoneSetSigma(TAngle sigma);

	
protected:


private:

	TAngle								fMeasuredAngle; /*!< measured angle */
	TAngle								fSigmaAPriori; /*!< estimated error on the angle measurement*/

//	TSpatialPointName					fStPtName;


};
/*@}*/

	/*!@name Typedefs*/
	//@{
	//! Type of the container used to store the hor. angle measurements
	typedef list< THorAngleMeasurement > HorAngleContainer;
	//! Type of the iterator pointing to an element of the hor. angles container
	typedef HorAngleContainer::iterator HorAngMeasIterator;
	//! Type of the const iterator pointing to an element of the hor. angles container
	typedef HorAngleContainer::const_iterator HorAngMeasConstIter;
	//@}

#endif









//class TWorkingStations;
//#include "UEOIndices.h"


/*!adds this measurement to the relevant station
	@param	wp a pointer to the working points object
	@param	ws a pointer to the working stations object*/
	//virtual bool				addToStation(TSpatialPointName wp, TWorkingStations* ws);


//protected: 

	//!Tells the parent ROM this measurement has been disactivated
	//virtual void							sendDisactivatedToParent();
	//!Tells the parent ROM this measurement has been activated
	//virtual void							sendActivatedToParent();



	//THorAngleROM*						fParentROM;





