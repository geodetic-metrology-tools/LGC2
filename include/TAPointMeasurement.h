//
// TAPointMeasurement.h : header file
// abstract base class holding the behaviour common to all
// targeted measurements
//
//  Patterns:
//  
//  
//  Copyright 2002 CERN EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////


#ifndef SU_PT_MEAS
#define SU_PT_MEAS

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//include files
#include <list>
using namespace std;

#include "TAMeasurement.h"
#include "TSpatialPointName.h"

//typedefs
typedef string Series;

/*! \ingroup spatialmeasurements
	@{*/

//! Abstract Base Class. Defines the methods common to all survey measurements having a target point
class TAPointMeasurement : public TAMeasurement 
{
public:

	
	/*!@name Overloaded operators*/
	//@{
	//! Less than operator
	virtual bool		operator<(const TAPointMeasurement& right) const;
	//! Equality operator
	virtual bool		operator==(const TAPointMeasurement& right) const;
	//@}

	
	/*!@return the name the target TSpatialPoint*/
	virtual TSpatialPointName	getTargetPoint() const;
	/*!sets the name of the target point from the latter's name
	@param wp a pointer to the working points object*/
	//virtual bool				setTargetPoint(TSpatialPointName wp);


protected:

	/*!@name Constructors / Destructor*/
	//@{
	//!Default constructor
	TAPointMeasurement();
	/*!Constructor
	@param parent a pointer to the parent meas tree node
	@param target an iterator pointing to the target TSpatialPoint*/
	//TAPointMeasurement(TVParentMeasTreeNode* parent, TWorkingPoints::PointIterator target);
	/*! Constructor setting the target point's name
	@param tgName the target point's name */
	explicit TAPointMeasurement(TSpatialPointName tgName);
	/*! Copy constructor */
	TAPointMeasurement(const TAPointMeasurement& source);
	/*! Destructor */
	~TAPointMeasurement();
	//@}

	
	
	
	TSpatialPointName				fTargetPtName; /*!< name of the target point */


};
#endif

/*@}*/






