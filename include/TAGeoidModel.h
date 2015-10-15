// TAGeoidModel.h: 
/*!
// Purely Abstract Base Class (Interface) for a TGeoid.
// 
// 
// Copyright 1999,2000, Mark Jones, EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////

#ifndef SU_A_GEOID
#define SU_A_GEOID


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
//using namespace std;
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Purely Abstract Base Class (Interface) for a TGeoid
class TAGeoidModel //: public TObject  
{
public:
	/**@name Constructors and Destructors */
	//@{
		/// default constructor
		TAGeoidModel();

		/// Destructor
		virtual  ~TAGeoidModel();
	//@}

	//!@name Member functions
	//@{
		/*! \Returns the geoidal undulation (geoidal height) at the given Point's position*/
		virtual TLength getN(const TSpatialPosition&) const {TLength null; return null;}
		
		/*! \Returns the deflection of the vertical in the prime vertical
		at the given Point's position*/
		virtual TAngle getEta(const TSpatialPosition&) const {TAngle null; return null;}

		/*!\Returns the deflection of the vertical in the meridian
		at the given Point's position*/	
		virtual TAngle getXi(const TSpatialPosition&) const {TAngle null; return null;}
		
		/*! \Returns the Laplace correction at the given Point's position*/	
		virtual TAngle getDAlpha(const TSpatialPosition&) const {TAngle null; return null;}

		/*! \Returns the Laplace correction at the given Point's position
		(phi is given)*/
		virtual TAngle getDAlpha(const TSpatialPosition&, const TAngle&) const {TAngle null; return null;}

		/*! \Returns the name of the geoid*/
		virtual string	getName() const = 0;

		/*! \Returns the definition reference frame*/
		virtual TAReferenceFrame* getDefRefFrame() const {return 0;}

		/*! \Returns the definition reference ellipsoid*/
		virtual TReferenceEllipsoid* getDefRefEll() const {return 0;}

		/*! \Returns the calculation reference frame*/
		virtual TAReferenceFrame* getCalcRefFrame() const {return 0;}

		/*! Set the geoid identifier as an enum type*/
		virtual void setGeoidId(const TRefSystemFactory::EGeoid geoidId) = 0;

		/*! \Returns the geoid identifier*/
		virtual TRefSystemFactory::EGeoid  getGeoidId() const{return TRefSystemFactory::kNoGeoid;}
	//@}
};

/*@}*/



#endif // SU_A_GEOID
