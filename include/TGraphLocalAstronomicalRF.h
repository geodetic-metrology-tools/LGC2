//  TGraphLocalAstronomicalRF.h
//
/** Patterns:
    A concrete STATE of a Spatial Position
	Used for the definition of the LA intermediate system of CCS 
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_GLA_REFERENCE_FRAME
#define SU_GLA_REFERENCE_FRAME


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//

#include  "TAModifiedLocalAstronomicalRF.h"
#include  "TModifiedLocalGeodeticRF.h"


//using namespace std;
//#include  "TLocalAstronomicalRefFrame.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for the definition of the LA intermediate system of CCSClass definition
class  TGraphLocalAstronomicalRF : public TAModifiedLocalAstronomicalRF //: public TObject  
{
public:
	
	/**@name Constructors and Destructors */
	//@{
		
		/// Constructor for a local astronomical reference frame
		/// taking the name of the reference frame, the geoid id., and the origin
		TGraphLocalAstronomicalRF( const string& name, const TAngle eta,
			const TAngle xsi, const TAngle dAlpha, TModifiedLocalGeodeticRF* locGeodSys);

		/// Destructor
		virtual  ~TGraphLocalAstronomicalRF();
	//@}


	/**@name Member Functions */
	//@{
		using TAModifiedLocalAstronomicalRF::getOrientation;

		/// return the associated local geodetic reference frame
		virtual TModifiedLocalGeodeticRF* getLocalGeodeticRF() const;

		/// return the geodetic reference frame
		virtual TGeodeticRefFrame* getGeodeticRF() const;

		/// Returns the origin of the local RF
		virtual TSpatialPosition getOrigin() const { return fOrigin; }

		/// Returns the orientation of the system
		virtual TSpatialOrientation getOrientation() const { return fOrientationMatrix; }


		/// get the eta value
		TAngle getEta() const { return fEta; }

		/// get the xsi value
		TAngle getXsi() const { return fXsi; }

		/// get the dAlpha value
		TAngle getDAlpha() const { return fDAlpha; }

		
		
		/// set the eta value
		void setEta(const TAngle eta) { fEta = eta; return; }

		/// set the xsi value
		void setXsi(const TAngle xsi) { fXsi = xsi; return; }

		/// set the dAlpha value
		void setDAlpha(const TAngle dAlpha) { fDAlpha = dAlpha; return; }
		
		

	//@}


protected:


private:
	// We don't want these accessible
	// Copy Constructor 
	TGraphLocalAstronomicalRF( const TGraphLocalAstronomicalRF& );

	// Copy Assignment Operator 
	TGraphLocalAstronomicalRF& operator=( const TGraphLocalAstronomicalRF& );
	


	
private:
	
	TSpatialPosition			fOrigin;
	TModifiedLocalGeodeticRF*	fLocalGeodeticSys;
	TSpatialOrientation			fOrientationMatrix;

	TAngle						fEta;
	TAngle						fXsi;
	TAngle						fDAlpha;



	
	
};

/*@}*/
//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_GLA_REFERENCE_FRAME
