// TOffsetToVerLineMeasurement
/*!
	Class for a horizontal distance between a point and a spatial line
	
	Patterns:
  
	Copyright 2003, CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_ECVE_MEASUREMENT
#define SU_ECVE_MEASUREMENT

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
class	TOffsetToVerLineMeasurement : public TALineMeasurement{

public :



	/*!@name Constructors/Destructor */
	//@{
		/*! Default constructor */
		TOffsetToVerLineMeasurement();

		/*! Constructor 
		\param setupPtName the measurement's station's setup point's name
		\param targetname  the measurement's target point's name
		\param obsDist the observed distance as a TLength
		\param sigma the observed distance's precision as a TLength*/
		TOffsetToVerLineMeasurement(const TSpatialLineName targetName, const TLength obsDist, const TLength sigma);
	
		/*! Copy constructor */
		TOffsetToVerLineMeasurement(const TOffsetToVerLineMeasurement &source);

		/*! Destructor */
		~TOffsetToVerLineMeasurement();
	//@}

	/*!@Member functions */
	//@{
		/*! Copy assignment operator */
		TOffsetToVerLineMeasurement&	operator=(const TOffsetToVerLineMeasurement& source);
		//DANGER : shallow copy (see NetworkSurveyMeas)

		/*! return a string indicating this measurement is a TECVE measurement */
		virtual string				getMeasKind() const;

	//@}

};
/*@}*/

	/*!@name Typedefs*/
	//@{
	/*!Type of the container used to store the spatial distances */
	typedef list< TOffsetToVerLineMeasurement > OffsetToVerLineContainer;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToVerLineContainer::iterator OffsetToVerLineMeasIterator;
	/*!Type of an iterator pointing to an element of the spatial dist. container */
	typedef OffsetToVerLineContainer::const_iterator OffsetToVerLineMeasConstIter;
	//@}

#endif

