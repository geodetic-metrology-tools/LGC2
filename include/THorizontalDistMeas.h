// THorizontalDistMeas
/*!
	Class for a horizontal distance measurement
	
	Patterns:
  
	Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_HOR_DIST_MEASUREMENT
#define SU_HOR_DIST_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////
// Forward declarations
//

//! 
class THorizontalDistROM;

#include "TAPointMeasurement.h"
#include "TLength.h"
#include "TDistConstants.h"
/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/ 

//! Class for a horizontal distance measurement
class	THorizontalDistMeas : public TAPointMeasurement{

public :

	/*!@name Constructors/Destructor */
	//@{
	/*! Default constructor */
	THorizontalDistMeas();
	/*! Constructor 
	\param setupPtName the measurement's station's setup point's name
	\param targetname  the measurement's target point's name
	\param obsDist the observed horizontal distance as a TLength
	\param sigma the observed angle's precision as a TAngle */
	THorizontalDistMeas(/*TSpatialPointName setupPtName,*/ TSpatialPointName targetName, TLength obsDist, TLength sigma);
	/*! Copy constructor */
	THorizontalDistMeas(const THorizontalDistMeas &source);
	/*! Destructor */
	~THorizontalDistMeas();
	//@}

	/*! Copy assignment operator */
	THorizontalDistMeas&		operator=(const THorizontalDistMeas& source);
	//DANGER : shallow copy (see NetworkSurveyMeas)

	/*!@name Member functions */
	//@{
	/*! return a string indicating this measurement is a hor. dist measurement */
	virtual string				getMeasKind() const;
	/*! return the observed distance's value as a TLength */
	virtual TLength				getDistValue() const;
	/*!return the observed distance's precision as a TLength */
	virtual TLength				getSigma() const;
	/*!return the observed distance's precision in ppm as a TLength */
	virtual TLength				getSigmaPpm() const;
	/*!@return the observed angle's constante (if there is one) as a TAngle instance*/
	virtual TDistConstants		getMeasConst() const {return fMeasConstant; }

	/*! sets the sigma a priori if it isn't done yet */
	virtual void				ifNotDoneSetSigma(TLength sigma);
	/*! sets the sigma a priori if it isn't done yet */
	virtual void				ifNotDoneSetSigmaPpm(TLength sigma);
	/*!Sets the measurement constant */
	virtual void				ifNotDoneSetMeasConst(const TDistConstants& constant);

	//@}

private:

	TLength							fObservedDist; /*!< observed distance */
	TLength							fSigmaAPriori; /*!< error "a priori" on the observed distance*/
	TLength							fSigmaPpm; /*!< error "a priori" on the observed distance in ppm*/

	TDistConstants					fMeasConstant; /*!< measurement constant if there is one */

//	TSpatialPointName				fStationPtName; /*< stationated point's name */

};
/*@}*/

	/*!@name Typedefs*/
	//@{
	/*!Type of the container used to store the horizontal distances */
	typedef list< THorizontalDistMeas > HorDistContainer;
	/*!Type of an iterator pointing to an element of the hor. dist. container */
	typedef HorDistContainer::iterator HorDistMeasIterator;
	/*!Type of a const iterator pointing to an element of the hor. dist. container */
	typedef HorDistContainer::const_iterator HorDistMeasConstIter;
	//@}

#endif













//class TWorkingStations;
//#include "UEOIndices.h"


	/*! Constructor */
/*	\param parent a reference to the THorizontalDistROM from which this measurement is part
	\param iterTg an iterator pointing at the target TSpatialPoint
	\param obsDist a TLength object representing the measured distance
	\param sigma a TLength objct representing the measurement's a priori precision*/
//	THorizontalDistMeas(THorizontalDistROM& parent, TWorkingPoints::PointIterator iterTg, TLength obsDist, TLength sigma);

	/*! add this measurement to the relevant station */
/*	param wp a pointer to the working points object
	param ws a pointer to the working stations object*/
//	virtual bool							addToStation(TWorkingPoints* wp, TWorkingStations* ws);

	/*! tells the parent ROM this measurement has been disactivated */
//	virtual void							sendDisactivatedToParent();

	/*! tells the parent ROM this measurement has been activated */
//	virtual void							sendActivatedToParent();

//	THorizontalDistROM*				fParentROM;
