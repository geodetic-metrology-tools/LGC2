// TMillimetresFilter.h: 
/*
Concrete Class of the TLengthFilter PABC.
Handles the IO of an TLength object lengths in a text stream with 
millimetre units. 
 
Patterns:
This class is a Singleton.
Works as a Strategy of a TTStreamFormatter object. 

Copyright 2002, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_MILLIMETRES_FILTER
#define SU_MILLIMETRES_FILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//using namespace std;
//
class  TLength;
#include  "TAStreamFormatter.h"
#include  "TLengthFilter.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup projectframework
@{*/

//Class definition
class TMillimetresFilter : public TLengthFilter //: public TObject  
{

public:

	/*!name Public Member Function*/
	//@!{
		/*!return the unique class component*/
		static	TMillimetresFilter *instance();

		/*!Get length value from a TAStreamFormatter object
		and set the values in a TLength object
		\param stream a TAStreamFormatter object
		\param len a TLength object*/
		virtual void input(TAStreamFormatter& stream, TLength& len) const;

		/*!Get the values from a TLength object
		and put the values in a TAStreamFormatter object
		\param stream a TAStreamFormatter object
		\param len a TLengthe object*/
		virtual void output(TAStreamFormatter& stream, const TLength& len) const;

		static void	deleteFilter() {if(fFilter!=0) {delete fFilter;} return;}
	//@}


protected:

	/*!name Constructors / Destructors*/
	//@!{
		/*!Constructors*/
		TMillimetresFilter();

		/*!Destructors*/
		virtual ~TMillimetresFilter();
	//@}


private:

	/*!static pointer to TMillimetresFilter*/
	static TMillimetresFilter	*fFilter;

	/*!copy constructor declaration*/
	TMillimetresFilter(const TMillimetresFilter&);

	//ClassDef(TMetresFilter, 1)
};

/*@}*/

#endif // SU_MILLIMETRES_FILTER
