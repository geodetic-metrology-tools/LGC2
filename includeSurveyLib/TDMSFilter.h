// TDMSFilter.h: 
/*!
Concrete Class of the TAngleFilter PABC.
Handles the IO of an TAngle object angles in a text stream with 
sexagesimal units (DMS: Degrees Minutes Seconds). 

Patterns:
This class is a Singleton.
Works as a Strategy of a TTStreamFormatter object. 

Copyright 2002, CERN, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_DMS_FILTER
#define SU_DMS_FILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//using namespace std;
class		TAngle;
#include	"TAStreamFormatter.h"
#include	"TAngleFilter.h"
//
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup FiltersAndFormats
@{*/

//Class definition
class TDMSFilter : public TAngleFilter //: public TObject  
{

public:

	/*!name Public Member Function*/
	//@!{
		/*!return the unique class component*/
		static	TDMSFilter* instance();

		/*!Get angle value from a TAStreamFormatter object
		and set the values in a TAngle object
		\param stream a TAStreamFormatter object
		\param ang a TAngle object*/
		virtual void input(TAStreamFormatter& stream, TAngle& ang) const;

		/*!Get the values from a TAngle object
		and put the values in a TAStreamFormatter object
		\param stream a TAStreamFormatter object
		\param ang a TAngle object*/
		virtual void output(TAStreamFormatter& stream, const TAngle& ang) const;

		static void	deleteFilter() {if(fFilter!=0) {delete fFilter;} return;}
	//@}

protected:

	/*!name Constructors / Destructors*/
	//@!{
		/*!Constructors*/
		TDMSFilter();

		/*!Destructors*/
		virtual ~TDMSFilter();
	//@}

private:
	
	/*!static pointer to TDMSFilter*/
	static TDMSFilter	*fFilter;

	/*!copy constructor declaration*/
	TDMSFilter(const TDMSFilter&);
	
	//ClassDef(TDMSFilter, 1)
};

/*@}*/

#endif // SU_DMS_FILTER
