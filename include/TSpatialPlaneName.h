///TSpatialPlaneName.h
/*!	  
	Class modelising a Vertical Plane	

	Copyright 2003, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////

#ifndef SU_SPATIAL_PlANE_NAME
#define SU_SPATIAL_PlANE_NAME

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//////////////////////////////////////////////////////////
// forward declariations and includes
#include <list>
#include <sstream>
//using namespace std;

#include "TAngle.h"
#include "TSpatialPointName.h"
//////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/


class TSpatialPlaneName //: public 
{

public:

	/*!@name Constructors / Destructor*/
	//@{
		/*!Default constructor*/
		TSpatialPlaneName();

		/*!Constructor for a vertical plane
		\param firstPointName the name of the Plane's first point
		\param secondPointName the name of the Plane's second point*/
		TSpatialPlaneName(const TSpatialPointName firstPointName, const TSpatialPointName secondPointName);

		/*!Constructor for a vertical plane
		\param firstPointName the name of the Plane's first point
		\param angle the line'orientation*/
		TSpatialPlaneName(const TSpatialPointName firstPointName, const TAngle angle);
	
		//!Copy constructor
		TSpatialPlaneName(const TSpatialPlaneName& source);

		//!Destructor
		~TSpatialPlaneName();
	//@}

	/*!@name Member function*/
	//@{
		//!Equality operator
		virtual bool					operator==(const TSpatialPlaneName& right) const;
		//!operator =
		virtual void					operator=(const TSpatialPlaneName& right);
		//!Less than operator
		bool							operator<( const TSpatialPlaneName & ) const;

		/*!\return true if the plane is define with the new arguments*/
		virtual bool					ifNotDoneDefinePlane(const TSpatialPointName firstPointName, const TSpatialPointName secondPointName);
		/*!\return true if the plane is define with the new arguments*/
		virtual bool					ifNotDoneDefinePlane(const TSpatialPointName firstPointName, const TAngle angle);

		/*!\return a const iterator to the Plane's first end point*/
		virtual	TSpatialPointName		getFirstPointName() const;
		/*!\return a const iterator to the Plane's second end point*/
		virtual	TSpatialPointName		getSecondPointName() const;
		/*!\return a TAngle defined the plane orientation*/
		virtual TAngle					getAngle() const;
		/*!\return the Plane's name*/
		virtual std::string				getName() const;

		/*!build the name (used only for TWorkingPlanes
		if the plane which is added to the list is not defined)*/
		virtual void					buildErrorName();
	//@}




private:

	/*!@name Member function*/
	//@{
	/*!\return true if the plane is define*/
		bool								isDefine() const;
	//@}

	TSpatialPointName					fFirstPointName;
	TSpatialPointName					fSecondPointName;
	TAngle								fAngle;
	
	std::string								fName;
									
};
/*@}*/


/*!@name Typedefs*/
//@{
/*! Type of the container used to store points */
typedef std::list<TSpatialPlaneName> SpatialPlaneContainer;
/*! Type of an iterator pointing to an element of the container */
typedef SpatialPlaneContainer::iterator PlaneIterator;
/*! Type of a const iterator pointing to an element of the container */
typedef SpatialPlaneContainer::const_iterator PlaneConstIter;
//@}


#endif
