//TCCsFilter.h: 
/*!
Concrete Class of the TAngleFilter PABC.
Handles the IO of an TAngle object angles in a text stream with 
cc units. 

Patterns:
This class is a Singleton.
Works as a Strategy of a TTStreamFormatter object. 

Copyright 2002, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_CC_FILTER
#define SU_CC_FILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//using namespace std;
//
class  TAngle;
#include  "TAStreamFormatter.h"
#include "TAngleFilter.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup projectframework
@{*/

//Class definition
class TCCsFilter : public TAngleFilter //: public TObject  
{

public:

	/*!name Public Member Function*/
	//@!{
		/*!return the unique class component*/
		static	TCCsFilter *instance();

		/*!Get angle value from a TAStreamFormatter object
		and set the values in a TAngle object
		\param stream a TAStreamFormatter object
		\param ang a TAngle object*/
		virtual void input(TAStreamFormatter&, TAngle &) const;

		/*!Get the values from a TAngle object
		and put the values in a TAStreamFormatter object
		\param stream a TAStreamFormatter object
		\param ang a TAngle object*/
		virtual void output(TAStreamFormatter&, const TAngle &) const;

		static void	deleteFilter() {if(fFilter!=0) {delete fFilter;} return;}
	//@}

protected:

	/*!name Constructors / Destructors*/
	//@!{
		/*!Constructors*/
		TCCsFilter();

		/*!Destructors*/
		virtual ~TCCsFilter();
	//@}

private:

	/*!static pointer to TCCsFilter*/
	static TCCsFilter	*fFilter;

	/*!copy constructor declaration*/
	TCCsFilter(const TCCsFilter&);
	
	//ClassDef(TCCsFilter, 1)
};


/*@}*/

#endif // SU_GONS_FILTER
