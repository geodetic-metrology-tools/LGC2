// TLengthFilter.h: 
/*!
Purely Abstract Base Class (Interface) for aa TLengthFilter.
Handles the IO of an TLength object for all defined
angular units. A different concrete class exists for 
each length unit.
 
Patterns:
Works as a Strategy of a TQtStreamFormatter object. 

Copyright 2002, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_LENGTH_FILTER
#define SU_LENGTH_FILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//
//
//using namespace std;
class	TAStreamFormatter;
class	TLength;
class	TPointFormat;
class	TObservationFormat;
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup projectframework
@{*/

//Interface definition
class TLengthFilter //: public TObject  
{
public:

	virtual ~TLengthFilter() { }

	/*!name Public Member Function*/
	//@!{
	/*!Get angle value from a TAStreamFormatter object
	and set the values in a TLength object
	\param stream a TAStreamFormatter object
	\param len a TLength object*/
	virtual void input(TAStreamFormatter& stream, TLength& len) const = 0;

	/*!Get the values from a TLength object
	and put the values in a TAStreamFormatter object
	\param stream a TAStreamFormatter object
	\param len a TLength object*/
	virtual void output(TAStreamFormatter& stream, const TLength& len) const = 0;
	//@}

private:
	// member attributes

	//ClassDef(TLengthFilter, 1)
};

/*@}*/

#endif // SU_LENGTH_FILTER
