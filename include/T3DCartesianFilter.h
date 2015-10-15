// T3DCartesianFilter.h
/*!
Concrete class of a TSpatialPositionFilter.
Handles the IO of an TSpatialPosition object for a
3D coordinate system. 

Patterns:
This is a Singleton.
Works as a Strategy of a TAStreamFormatter object. 

Copyright 2002, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


#ifndef SU_3D_CARTESIAN_FILTER
#define SU_3D_CARTESIAN_FILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//using namespace std;
//
class  TAStreamFormatter;
class  TSpatialPosition;
#include  "TSpatialPositionFilter.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup projectframework
@{*/

//Class definition
class T3DCartesianFilter : public TSpatialPositionFilter //: public TObject  
{
public:

	/*!name Public Member Function*/
	//@!{
		/*!return the unique class component*/
		static	T3DCartesianFilter *instance();

		/*!Get coordinates from a TAStreamFormatter object
		and set the values in a TSpatialPosition object
		\param stream a TAStreamFormatter object
		\param pos a TSpatialPosition object*/
		virtual void input(TAStreamFormatter& stream, TSpatialPosition& pos) const;

		/*!Get coordinates from a TSpatialPosition object
		and put the values in a TAStreamFormatter object
		\param stream a TAStreamFormatter object
		\param pos a TSpatialPosition object*/
		virtual void output(TAStreamFormatter& stream, const TSpatialPosition& pos) const;

		/*!Get coordinates from a TAStreamFormatter object
		and set the values in a TPositionVector object
		\param stream a TAStreamFormatter object
		\param pos a TPositionVector object*/
		virtual void input(TAStreamFormatter& stream, TPositionVector& pos) const;

		/*!Get coordinates from a TPositionVector object
		and put the values in a TAStreamFormatter object
		\param stream a TAStreamFormatter object
		\param pos a TPositionVector object*/
		virtual void output(TAStreamFormatter& stream, const TPositionVector& pos) const;

		static void	deleteFilter() {if(fCoorSys!=0) {delete fCoorSys;} return;}
	//@}

protected:
	/*!name Constructors / Destructors*/
	//@!{
		/*!Constructors*/
		T3DCartesianFilter();

		/*!Destructors*/
		virtual ~T3DCartesianFilter();
	//@}

private:
	/*! static pointor to T3DCartesianHFilter*/
	static T3DCartesianFilter	*fCoorSys;

	//ClassDef(T3DCartesianFilter, 1)
};

/*@}*/

#endif // SU_3D_CARTESIAN_FILTER


