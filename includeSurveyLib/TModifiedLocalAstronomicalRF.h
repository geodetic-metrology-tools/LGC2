//  TModifiedLocalAstronomicalRF.h
//
/** Patterns:
    A concrete STATE of a Spatial Position
   
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_MLA_REFERENCE_FRAME
#define SU_MLA_REFERENCE_FRAME


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
//


#include  "TAModifiedLocalAstronomicalRF.h"

//using namespace std;
// typedefs
//
//
////////////////////////////////////////////////////////////////

/*! \ingroup spatialobjects
	@{*/

//! Class for a modified local astronomical ref. frame
class  TModifiedLocalAstronomicalRF : public TAModifiedLocalAstronomicalRF //: public TObject  
{
public:
	
	/**@name Constructors and Destructors */
	//@{
		/// Constructor taking the name of the reference frame, the geoid id.,
		/// the origin and the false origin in CCS and the beam gis. and slope
		TModifiedLocalAstronomicalRF( const string& name, TRefSystemFactory::EGeoid geoid, 
			TSpatialPosition origin, TFreeVector falseOrigin,
			const TAngle gis, const TAngle slope);

		/// Constructor taking the name of the reference frame, the geoid id.,
			TModifiedLocalAstronomicalRF( const string& name, TRefSystemFactory::EGeoid geoid);

		/// Constructor for a local astronomical reference frame
		/// taking the name of the reference frame, the geoid id., and the origin
		TModifiedLocalAstronomicalRF( const string& name, TRefSystemFactory::EGeoid geoid, TSpatialPosition origin);

		/// Constructor for a local astronomical reference frame
		/// taking the name of the reference frame, the ellipsoid id., and the origin
		TModifiedLocalAstronomicalRF( const string& name, TRefSystemFactory::ERefEll ell, TSpatialPosition origin);


		/// Destructor
		virtual  ~TModifiedLocalAstronomicalRF();
	//@}


	/**@name Member Functions */
	//@{
		using TAModifiedLocalAstronomicalRF::getOrientation;
		
		/// Returns a pointer to the Local Geodetic RF
		TGeodeticRefFrame* getGeodeticRF() const;

		/// Returns the geoid
		TRefSystemFactory::EGeoid getGeoid() const { return fGeoid; }

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

		/*! get the gisement of the reference frame */
		virtual TAngle getGisement() const { return fGis; }

		/*! get the slope of the reference frame */
		virtual TAngle getSlope() const { return fSlope; }

		//! transformation from this reference frame to CGRF (in case of a reference frame not included in TGraph)
		virtual TARefFrameTransformation*   getRFTransfo2CGRF();

		//!set the origin of the MLA
		bool	setOrigin(const TSpatialPosition&);

		//!return true if the origin is set
		bool	isOriginSet() const;

		//! transform a position from a reference frame to another
		bool	transform(TSpatialPosition* sp, TAReferenceFrame* rf);
		
		//! transform a position from a reference frame to another
		bool	transform(TSpatialVector* sv, TAReferenceFrame* rf);

		/*!return true if the refernec frame is in graph,
		so return false only for MLA*/
		virtual bool isInGraph() const {return false; };


		void initialiseMLA(TSpatialPosition origin);

		void initialiseLA(TSpatialPosition origin);



	//@}


protected:


private:
	// We don't want these accessible
	// Copy Constructor 
	TModifiedLocalAstronomicalRF( const TModifiedLocalAstronomicalRF& );

	// Copy Assignment Operator 
	TModifiedLocalAstronomicalRF& operator=( const TModifiedLocalAstronomicalRF& );


	/// set the eta value
	void setEta(const TAngle eta) { fEta = eta; return; }

	/// set the xsi value
	void setXsi(const TAngle xsi) { fXsi = xsi; return; }

	/// set the dAlpha value
	void setDAlpha(const TAngle dAlpha) { fDAlpha = dAlpha; return; }
	
		
	
private:
	
	TSpatialPosition			fOrigin;
	TRefSystemFactory::EGeoid	fGeoid;
	TSpatialOrientation			fOrientationMatrix;
	
	TGeodeticRefFrame*			fGeodeticSys;

	TAngle						fEta;
	TAngle						fXsi;
	TAngle						fDAlpha;

	TAngle						fGis;
	TAngle						fSlope;

	bool						fOriginDefined; /*!<true if the origin is set*/

	TARefFrameTransformation*	fTransformation;
};
/*@}*/

//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////






#endif // SU_GEODETIC_REFERENCE_FRAME
