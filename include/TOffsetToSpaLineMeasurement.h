// TOffsetToSpaLineMeasurement
/*!
	Class for a distance between a point and a spatial line
	
	Patterns:
  
	Copyright 2003, CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_ECSP_MEASUREMENT
#define SU_ECSP_MEASUREMENT

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



/////////////////////////////////////////////////////
// Forward declarations
//
#include "TALineMeasurement.h"
/////////////////////////////////////////////////////

/*! \ingroup spatialmeasurements
	@{*/ 

//! Class for a horizontal distance measurement
class	TOffsetToSpaLineMeasurement : public TALineMeasurement{

public :



	/*!@name Constructors/Destructor */
	//@{
		/*! Default constructor */
		TOffsetToSpaLineMeasurement();

		/*! Constructor 
		\param setupPtName the measurement's station's setup point's name
		\param targetname  the measurement's target point's name
		\param obsDist the observed distance as a TLength
		\param sigma the observed distance's precision as a TLength*/
		TOffsetToSpaLineMeasurement(const TSpatialLineName targetName, const TLength obsDist, const TLength sigma);
	
		/*! Copy constructor */
		TOffsetToSpaLineMeasurement(const TOffsetToSpaLineMeasurement &source);

		/*! Destructor */
		~TOffsetToSpaLineMeasurement();
	//@}

	/*!@Member functions */
	//@{
		/*! Copy assignment operator */
		TOffsetToSpaLineMeasurement&	operator=(const TOffsetToSpaLineMeasurement& source);
		//DANGER : shallow copy (see NetworkSurveyMeas)

		/*! return a string indicating this measurement is a TECSP measurement */
		virtual string				getMeasKind() const;

};
/*@}*/

	/*!@name Typedefs*/
	//@{
	/*!Type of the container used to store the spatial distances */
	typedef list< TOffsetToSpaLineMeasurement > OffsetToSpaLineContainer;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToSpaLineContainer::iterator OffsetToSpaLineMeasIterator;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToSpaLineContainer::const_iterator OffsetToSpaLineMeasConstIter;
	//@}

#endif

