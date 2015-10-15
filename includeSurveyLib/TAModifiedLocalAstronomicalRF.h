//  TAModifiedLocalAstronomicalRF.h
//
/** Patterns:
    An abstract base classe for the modified local astronomical reference frame
	and local astronomical reference frame
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_AMLA_REFERENCE_FRAME
#define SU_AMLA_REFERENCE_FRAME


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//
#include  <string>
#include  <vector>

/*! \ingroup spatialobjects
	@{*/

//! abstract base classe for the modified local astronomical reference frame
class	TGraph;

#include  "TLength.h"
#include  "TA3DEuclideanRefFrame.h"
#include  "TGeodeticRefFrame.h"
#include  "TAGeoidModel.h"
#include  "TSpatialOrientation.h"


//using namespace std;



//Class definition
class  TAModifiedLocalAstronomicalRF : public TA3DEuclideanRefFrame //: public TObject  
{
public:
	
	/**@name Constructors and Destructors */
	//@{
		/// Default constructor
		TAModifiedLocalAstronomicalRF( const string& name );
		/// Destructor
		virtual  ~TAModifiedLocalAstronomicalRF();
	//@}


	/**@name Member Functions */
	//@{
		using TA3DEuclideanRefFrame::getOrientation;

		/// Returns a pointer to the Geodetic RF
		virtual TGeodeticRefFrame* getGeodeticRF() const;

		/// Returns the false origin of the reference frame
		virtual TFreeVector getFalseOrigin() const { return fFalseOrigin; }

		/// Returns the orientation of the system
		virtual TSpatialOrientation getOrientation() const;

		/// get the eta value
		virtual TAngle getEta() const;

		/// get the xsi value
		virtual TAngle getXsi() const;

		/// get the dAlpha value
		virtual TAngle getDAlpha() const;
	//@}


protected:

	/**@name Protected Functions */
	//@{
		/// set the false origin of the reference frame
		void setFalseOrigin(const TFreeVector falseOrigin) { fFalseOrigin = falseOrigin; return; }
	//@}

private:
	
	/**@name Private Constructors*/
	//@{
		// We don't want these accessible
		/*! Copy Constructor */
		TAModifiedLocalAstronomicalRF( const TAModifiedLocalAstronomicalRF& );

		/*! Copy Assignment Operator */
		TAModifiedLocalAstronomicalRF& operator=( const TAModifiedLocalAstronomicalRF& );
	//@}

	
private:
	
	/**@name Private Attributes*/
	//@{	
		TFreeVector					fFalseOrigin; /*!< false origin */
	//@}
};
/*@}*/


#endif // SU_AMLA_REFERENCE_FRAME
