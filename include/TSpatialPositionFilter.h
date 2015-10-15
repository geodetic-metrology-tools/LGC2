// TSpatialPositionFilter.h: 
//
// Purely Abstract Base Class (Interface) for a Spatial Position Filter.
// Handles the IO of an TSpatialPosition object for all defined
// coordinate systems. A different concrete class exists for 
// each coordinate system.
//
// Also used by a TReferenceFrame to identify valid coordinate systems
// for a given reference frame.
// 
// Patterns:
// Works as a Strategy of a TQtStreamFormatter object. 
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



#ifndef SU_SPATIAL_POSITION_FILTER
#define SU_SPATIAL_POSITION_FILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
using namespace std;
//
class	TAStreamFormatter;
class	TSpatialPosition;
class	TPositionVector;
//
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Interface definition
class TSpatialPositionFilter //: public TObject  
{
public:
	//constants

	// constructors and destructors

	virtual ~TSpatialPositionFilter() { }

	//member functions
	virtual void input( TAStreamFormatter &, TSpatialPosition & ) const = 0;
	virtual void output( TAStreamFormatter &, const TSpatialPosition & ) const = 0;

	//member functions
	virtual void input( TAStreamFormatter &, TPositionVector&) const = 0;
	virtual void output( TAStreamFormatter &, const TPositionVector&) const = 0;

protected:

private:
	// member attributes

	//ClassDef(TSpatialPositionFilter, 1)
};




#endif // SU_SPATIAL_POSITION_FILTER


