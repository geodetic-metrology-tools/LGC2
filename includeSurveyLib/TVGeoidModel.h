// TVGeoidModel.h: 
/*!
// Purely Abstract Base Class (Interface) for a TGeoid.
// 
// 
// Copyright 1999,2000, Mark Jones, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_GEOID
#define SU_GEOID


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>

class  TSpatialPosition;

#include "TGeoidValues.h"
#include "TLength.h"
#include "TAngle.h"
#include "TRefSystemFactory.h"
using namespace std;
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Purely Abstract Base Class (Interface) for a TGeoid
class TVGeoidModel //: public TObject  
{
public:

	//!@name Member functions
	//@{
		/*! \Returns the geoidal undulation (geoidal height) at the given Point's position*/
		virtual TLength getN(const TSpatialPosition&) const = 0;
		
		/*! \Returns the deflection of the vertical in the prime vertical
		at the given Point's position*/
		virtual TAngle getEta(const TSpatialPosition&) const = 0;

		/*!\Returns the deflection of the vertical in the meridian
		at the given Point's position*/	
		virtual TAngle getXi(const TSpatialPosition&) const = 0;
		
		/*! \Returns the Laplace correction at the given Point's position*/	
		virtual TAngle getDAlpha(const TSpatialPosition&) const = 0;

		/*! \Returns the Laplace correction at the given Point's position
		(phi is given)*/
		virtual TAngle getDAlpha(const TSpatialPosition&, const TAngle&) const = 0;

		/*! \Returns the name of the geoid*/
		virtual string	getName() const = 0;

		/*! \Returns the definition reference frame*/
		virtual TAReferenceFrame* getDefRefFrame() const = 0;

		/*! \Returns the definition reference ellipsoid*/
		virtual TReferenceEllipsoid* getDefRefEll() const = 0;

		/*! \Returns the calculation reference frame*/
		virtual TAReferenceFrame* getCalcRefFrame() const = 0;

		/*! Set the geoid identifier as an enum type*/
		virtual void setGeoidId(const TRefSystemFactory::EGeoid geoidId) = 0;

		/*! \Returns the geoid identifier*/
		virtual TRefSystemFactory::EGeoid  getGeoidId() const = 0;
	//@}
};

/*@}*/



#endif // SU_GEOID
