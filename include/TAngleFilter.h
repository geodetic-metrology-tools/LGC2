// TAngleFilter.h: 
/*!
Purely Abstract Base Class (Interface) for an TAngleFilter.
Handles the IO of an TAngle object for all defined
angular units. A different concrete class exists for 
each angular unit.
 
Patterns:
Works as a Strategy of a TQtStreamFormatter object. 

Copyright 2002, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_ANGLE_FILTER
#define SU_ANGLE_FILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//using namespace std;
class	TAngle;
class	TPointFormat;
class	TObservationFormat;
class	TAStreamFormatter;
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup projectframework
@{*/

//Interface definition
class TAngleFilter //: public TObject  
{
public:

	virtual ~TAngleFilter() { }
	/*!name Public Member Function*/
	//@!{
	/*!Get angle value from a TAStreamFormatter object
	and set the values in a TAngle object
	\param stream a TAStreamFormatter object
	\param ang a TAngle object*/
	virtual void input(TAStreamFormatter& stream, TAngle& ang) const = 0;

	/*!Get the values from a TAngle object
	and put the values in a TAStreamFormatter object
	\param stream a TAStreamFormatter object
	\param ang a TAngle object*/
	virtual void output(TAStreamFormatter& stream, const TAngle& ang) const = 0;
	//@}

private:
	// member attributes

	//ClassDef(TAngleFilter, 1)
};

/*@}*/

#endif // SU_ANGLE_FILTER
