// TZenithDistMeasurement.h
/*!
	Class for a zenithal distance measurement

	Pattern:

	Copyright 2002 CERN EST/SU. All rights reserved.
*/

////////////////////////////////////////////////////////////////

#ifndef SU_ZENITH_DIST_MEASUREMENT
#define SU_ZENITH_DIST_MEASUREMENT


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000




class  TZenithDistROM;

#include  "TAPointMeasurement.h"
#include  "TAngle.h"
#include  "TLength.h"

////////////////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a zenithal distance measurement 
class  TZenithDistMeasurement : public TAPointMeasurement  
{
public:

	/*!@name Constructors and Destructors */
	//@{
	/*! Default Constructor */
	TZenithDistMeasurement();
	/*!Constructor
	\param stName the measurement's station's setup point's name
	\param tgName the measurement's target point's name
	\param obsAng the observed zenithal distance as a TAngle
	\param sigma the observed angle's precision as a TAngle*/
	TZenithDistMeasurement(TSpatialPointName tgName, TAngle obsAng, TAngle sigma);
	/*!Constructor
	\param stName the measurement's station's setup point's name
	\param tgName the measurement's target point's name
	\param obsAng the observed zenithal distance as a TAngle
	\param sigma the observed angle's precision as a TAngle
	\param hp the height of the prisme, supposed to be fixed*/
	TZenithDistMeasurement(TSpatialPointName tgName, TAngle obsAng, TAngle sigma, TLength hp);
	/*! Copy Constructor */
	TZenithDistMeasurement(const TZenithDistMeasurement &source);
	/*! Destructor */
	virtual  ~TZenithDistMeasurement();
	//@}
	
	/*!@name methods used in the facade ZenithDistObservation */
	//@{
	/*! returns the observed angle's value as a TAngle instance*/
	virtual TAngle							getAngleValue() const;
	/*! returns the observed angle's precision as a TAngle instance*/
	virtual TAngle							getSigma() const;
	/*! returns the prisme height as a TLength instance*/
	virtual TLength							getPrismeHeight() const;
	/*! returns the prisme height's status*/
	virtual ECalcStatus						getPrismeHeightStatus() const;
	//@}

	/*! sets the prisme height as a TLength instance*/
	virtual void							setPrismeHeight(TLength hp);
	/*! sets the prisme height's status*/
	virtual void							setPrismeHeightStatus(ECalcStatus status);

	//! Copy Assignment Operator 
	TZenithDistMeasurement& operator=( const TZenithDistMeasurement& source);

	/*! returns a pointer to this TZenithDistMeasurement */
	virtual TZenithDistMeasurement*			getPointer() const;

	/*! returns a string indicating this measurement is a zenithal distance measurement*/
	virtual string							getMeasKind() const;
	
	//!Sets the sigma a priori if it isn't done yet
	virtual void				ifNotDoneSetSigma(TAngle sigma);
	

protected:

	

private:

	TAngle								fMeasuredAngle; /*!< measured angle */
	TAngle								fSigmaAPriori;	/*!< angle measurement error */

	TLength								fPrismeHeight;	/*!< prisme height */
	ECalcStatus							fHeightStatus;	/*!< prisme height status : variable or fixed*/


};
/*@}*/


	/*!@name Typedef */
	//@{
	/*! Type of the container used to store the zenithal distance measurements */
	typedef list<TZenithDistMeasurement> ZenithDistContainer;
	/*! Type of the iterator pointing to an element of the zenithal distances container */
	typedef ZenithDistContainer::iterator ZenithDistMeasIterator;
	/*! Type of the const_iterator pointing to an element of the zenithal distances container */
	typedef ZenithDistContainer::const_iterator ZenithDistMeasConstIter;
	//@}

#endif  //SU_ZENITH_DIST_MEASUREMENT























//class TWorkingStations;
//#include  "UEOIndices.h"

	/*!Constructor
	@param parent a reference to the TZenithDistROM from which this measurement is part
	@param iterTg an iterator pointing to the target TSpatialPoint
	@param obsAngle a TAngle object representing the measured angle
	@param sigma a TAngle object representing the measurement's a priori precision*/
//	TZenithDistMeasurement(TZenithDistROM& parent, TWorkingPoints::PointIterator iterTg, TAngle obsAngle, TAngle sigma);

	/*!adds this measurement to the relevant station
	@param	wp a pointer to the working points object
	@param	ws a pointer to the working stations object*/
//	virtual bool				addToStation(TWorkingPoints* wp, TWorkingStations* ws);

//	TZenithDistROM*						fParentROM;




//protected:

	//!Tells the parent ROM this measurement has been disactivated
//	virtual void							sendDisactivatedToParent();
	//!Tells the parent ROM this measurement has been activated
//	virtual void							sendActivatedToParent();




//	TSpatialPointName					fStPtName; /*< stationed point name */
