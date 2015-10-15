// TOffsetToVerPlaneMeasurement
/*!
	Class for a horizontal distance between a point and a line
	
	Patterns:
  
	Copyright 2003, CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_ECHO_MEASUREMENT
#define SU_ECHO_MEASUREMENT

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
class	TOffsetToVerPlaneMeasurement : public TAPlaneMeasurement{

public :



	/*!@name Constructors/Destructor */
	//@{
		/*! Default constructor */
		TOffsetToVerPlaneMeasurement();

		/*! Constructor 
		\param setupPtName the measurement's station's setup point's name
		\param targetname  the measurement's target point's name
		\param obsDist the observed distance as a TLength
		\param sigma the observed distance's precision as a TLength*/
		TOffsetToVerPlaneMeasurement(const TSpatialPlaneName targetName, const TLength obsDist, const TLength sigma);
	
		/*! Copy constructor */
		TOffsetToVerPlaneMeasurement(const TOffsetToVerPlaneMeasurement &source);

		/*! Destructor */
		~TOffsetToVerPlaneMeasurement();
	//@}

	/*!@Member functions */
	//@{
		/*! Copy assignment operator */
		TOffsetToVerPlaneMeasurement&	operator=(const TOffsetToVerPlaneMeasurement& source);
		//DANGER : shallow copy (see NetworkSurveyMeas)

		/*! return a string indicating this measurement is a TECHO measurement */
		virtual string				getMeasKind() const;

};
/*@}*/

	/*!@name Typedefs*/
	//@{
	/*!Type of the container used to store the spatial distances */
	typedef list< TOffsetToVerPlaneMeasurement > OffsetToVerPlaneContainer;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToVerPlaneContainer::iterator OffsetToVerPlaneMeasIterator;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToVerPlaneContainer::const_iterator OffsetToVerPlaneMeasConstIter;
	//@}

#endif

