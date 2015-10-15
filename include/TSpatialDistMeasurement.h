// TSpatialDistMeasurement
/*!
	Class for a spatial distance measurement

	Patterns:
 
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_SPATIAL_DIST_MEASUREMENT
#define SU_SPATIAL_DIST_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
//
#define  PrismNb  string

class TSpatialDistROM;

#include "TAPointMeasurement.h"
#include "TLength.h"
#include "TDistConstants.h"
/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/

//! Class for a spatial distance measurement
class	TSpatialDistMeasurement : public TAPointMeasurement{

public :

	/*!@name Constructors/Destructor */
	//@{
	/*! Default constructor */
	TSpatialDistMeasurement();
	/*! Constructor
	\param targetname  the measurement target point's name
	\param obsDist the observed horizontal distance as a TLength
	\param sigma the observed distance's precision as a TLength
	\param pn the prism number*/
	TSpatialDistMeasurement(TSpatialPointName targetName, TLength obsDist, TLength sigma/*, PrismNb pn*/);
	/*! Constructor
	\param targetname  the measurement target point's name
	\param obsDist the observed horizontal distance as a TLength
	\param sigma the observed distance's precision as a TLength
	\param height a TLength object representing the prism height, supposed to be fixed 
	\param pn the prism number*/
	TSpatialDistMeasurement(TSpatialPointName targetName, TLength obsDist, TLength sigma, TLength height, PrismNb pn);
	/*! Copy constructor */
	TSpatialDistMeasurement(const TSpatialDistMeasurement &source);
	/*! Destructor */
	~TSpatialDistMeasurement();
	//@}

	/*! Copy assignment operator */
	TSpatialDistMeasurement&		operator=(const TSpatialDistMeasurement& source);
	//DANGER : shallow copy (see NetworkSurveyMeas)

	/*!@name Access Methods */
	//@{
	/*! return the observed distance's value as a TLength */
	virtual TLength							getDistValue() const;
	/*!return the observed distance's precision as a TLength */
	virtual TLength							getSigma() const;
	/*! return the target point's height value as a TLength instance */
	virtual TLength							getPrismHeight() const;
	/*! return the prism height status */
	virtual ECalcStatus						getPrismHeightStatus() const;

	/*! Return a string indicating this measurement is a spatial dist measurement */
	virtual string							getMeasKind() const;
	/*! return the prism number */
	virtual PrismNb							getPrismNumber() const;
	/*!return the observed distance's precision in ppm as a TLength */
	virtual TLength							getSigmaPpm() const;
	/*!@return the observed angle's constante (if there is one) as a TAngle instance*/
	virtual TDistConstants					getMeasConst() const {return fMeasConstant; }
	//@}
	
	/*!@name Set Methods */
	//@{
	/*! set the target point's height value as a TLength instance */
	virtual void							setPrismHeight(TLength hp);
	/*! set the prism height status */
	virtual void							setPrismHeightStatus(ECalcStatus status);
	/*! sets the sigma a priori if it isn't done yet */
	virtual void							ifNotDoneSetSigma(TLength sigma);
	/*! sets the sigma a priori if it isn't done yet */
	virtual void							ifNotDoneSetSigmaPpm(TLength sigma);
	/*!Sets the measurement constant */
	virtual void							ifNotDoneSetMeasConst(const TDistConstants& constant);
	//@}

protected:

private:

	TLength							fObservedDist; /*!< observed spatial distance */
	TLength							fSigmaAPriori; /*!< distance's "a priori" error */
	TLength							fSigmaPpm; /*!< error "a priori" on the observed distance in ppm*/

	TDistConstants					fMeasConstant; /*!< measurement constant if there is one */
	TLength							fPrismHeight; /*!< prism height */
	PrismNb							fPrismNumber; /*!< prism number */
	ECalcStatus						fHeightStatus; /*!< prism height status : variable or fixed */

};
/*@}*/


	/*!@name Typedefs*/
	//@{
	/*!Type of the container used to store the spatial distances */
	typedef list< TSpatialDistMeasurement > SpatialDistContainer;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef SpatialDistContainer::iterator SpatialDistMeasIterator;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef SpatialDistContainer::const_iterator SpatialDistMeasConstIter;
	//@}
	
#endif
















//class TWorkingStations;
//#include "UEOIndices.h"



	/* Constructor (comments to be updated) */
/*	\param parent a reference to the TSpatialDistROM from which this measurement is part
	\param iterTg an iterator pointing at the target TSpatialPoint
	\param obsDist a TLength object representing the measured distance
	\param sigma a TLength object representing the measurement's a priori precision
	\param height a TLength object representing the prism height
	TSpatialDistMeasurement(TSpatialDistROM& parent, TWorkingPoints::PointIterator iterTg, TLength obsDist, TLength sigma, TLength height, PrismNb pn);
*/	
	/* add this measurement to the relevant station (to be updated) */
/*	\param wp a pointer to the working points object
	\param ws a pointer to the working stations object */
//	virtual bool							addToStation(TWorkingPoints* wp, TWorkingStations* ws);
	
	/*! tells the parent ROM this measurement has been disactivated */
//	virtual void							sendDisactivatedToParent();

	/*! tells the parent ROM this measurement has been activated */
//	virtual void							sendActivatedToParent();
//	TSpatialDistROM*				fParentROM;






















//	TSpatialPointName				fStationPtName; /*!< name of the stationed point */
