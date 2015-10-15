//  TGraphMLARF.h
//
/** Patterns:
    A concrete STATE of a Spatial Position
	Used for the CCS definition
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_GMLA_REFERENCE_FRAME
#define SU_GMLA_REFERENCE_FRAME


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//

#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TGraphLocalAstronomicalRF.h"


//using namespace std;
//#include  "TLocalAstronomicalRefFrame.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*!\ingroup spatialobjects
	@{*/

//! Class for the CCS definition  
class  TGraphMLARF : public TAModifiedLocalAstronomicalRF //: public TObject  
{
public:
	
	/**@name Constructors and Destructors */
	//@{
		/// Constructor taking the name of the reference frame, the origin, the geoid parameters,
		/// the false origin,the geodetic reference frame and the orientation angles
		TGraphMLARF( const string& name, TFreeVector falseOrigin, TGraphLocalAstronomicalRF* RF,
		const TAngle omega, const TAngle phi, const TAngle kappa);

		
		/// Destructor
		virtual  ~TGraphMLARF();
	//@}


	/**@name Member Functions */
	//@{
		using TAModifiedLocalAstronomicalRF::getOrientation;

		/// return the associated local astronomical reference frame
		TGraphLocalAstronomicalRF* getLocalAstronomicalRF() const;

		/// return the geodetic reference frame
		virtual TGeodeticRefFrame* getGeodeticRF() const;

		/// return the local geodetic reference frame
		virtual TModifiedLocalGeodeticRF* getLocalGeodeticRF() const;

		/// Returns the origin of the local RF
		virtual TSpatialPosition getOrigin() const { return fOrigin; }

		/// Returns the orientation of the system
		virtual TSpatialOrientation getOrientation() const { return fOrientationMatrix; }


		/// get the eta value
		virtual TAngle getEta() const;

		/// get the xsi value
		virtual TAngle getXsi() const;

		/// get the dAlpha value
		virtual TAngle getDAlpha() const;



	//@}


protected:


private:
	// We don't want these accessible
	// Copy Constructor 
	TGraphMLARF( const TGraphMLARF& );

	// Copy Assignment Operator 
	TGraphMLARF& operator=( const TGraphMLARF& );


	/// set the eta value
	void setEta(const TAngle eta);

	/// set the xsi value
	void setXsi(const TAngle xsi);

	/// set the dAlpha value
	void setDAlpha(const TAngle dAlpha);
	
		
	
private:
	
	TSpatialPosition					fOrigin;	/*!< origin */
	TGraphLocalAstronomicalRF*			fLocAstroSys; /*!< loc astronomical ref. frame */
	TSpatialOrientation					fOrientationMatrix; /*!< orientation matrix */

	
	
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_GMLA_REFERENCE_FRAME
