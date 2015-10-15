// TOffsetToTheoPlaneMeasurement
/*!
	Class for a horizontal distance between a point and a theodolite's
	sighting line
	
	Patterns:
  
	Copyright 2003, CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_ECTH_MEASUREMENT
#define SU_ECTH_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////
// Forward declarations
//
#include "TAPlaneMeasurement.h"
/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/ 

//! Class for a horizontal distance measurement
class	TOffsetToTheoPlaneMeasurement : public TAPlaneMeasurement{

public :



	/*!@name Constructors/Destructor */
	//@{
		/*! Default constructor */
		TOffsetToTheoPlaneMeasurement();

		/*! Constructor 
		\param setupPtName the measurement's station's setup point's name
		\param targetname  the measurement's target point's name
		\param obsDist the observed distance as a TLength
		\param sigma the observed distance's precision as a TLength*/
		TOffsetToTheoPlaneMeasurement(const TSpatialPlaneName targetName, const TLength obsDist, const TLength sigma);
	
		/*! Copy constructor */
		TOffsetToTheoPlaneMeasurement(const TOffsetToTheoPlaneMeasurement &source);

		/*! Destructor */
		~TOffsetToTheoPlaneMeasurement();
	//@}

	/*!@Member functions */
	//@{
		/*! Copy assignment operator */
		TOffsetToTheoPlaneMeasurement&	operator=(const TOffsetToTheoPlaneMeasurement& source);
		//DANGER : shallow copy (see NetworkSurveyMeas)

		/*! return a string indicating this measurement is a TECTH measurement */
		virtual string				getMeasKind() const;

};
/*@}*/

	/*!@name Typedefs*/
	//@{
	/*!Type of the container used to store the spatial distances */
	typedef list< TOffsetToTheoPlaneMeasurement > OffsetToTheoPlaneContainer;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToTheoPlaneContainer::iterator OffsetToTheoPlaneMeasIterator;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToTheoPlaneContainer::const_iterator OffsetToTheoPlaneMeasConstIter;
	//@}

#endif

