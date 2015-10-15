//  TReferenceFrameServer.h
//
/** A class that instantiates and provides access to all available
	reference frames.

  
    Patterns:
	A singleton to ensure only one such server exists.
     
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



#ifndef SU_REFERENCE_FRAME_SERVER
#define SU_REFERENCE_FRAME_SERVER


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


////////////////////////////////////////////////////////////////
// Forward declarations
//
#include  <string>
using namespace std;
//
class  TVReferenceEllipsoid;
class  TReferenceEllipsoid;
class  TVGeoid;
class  TCernParabolicGeoid;
class  TVTopocentricDatumPosition;
class  TTopocentricDatumPosition;
class  TVReferenceFrame;
class  TEuclideanRefFrame;
class  TGeodeticRefFrame;
class  TProjectionXYHeRefFrame;
class  TProjectionXYHgRefFrame;
class  TVTransformation;
class  TReferenceFrameNetwork;
//#include  "##classname##.h"
// typedefs
//
//
////////////////////////////////////////////////////////////////



//Class definition
class  TReferenceFrameServer //: public TObject  
{
public:
	/**@name Constants */
	//@{
		/*enum  EEuclideanRF { kLocal, kCCS };
		enum  EGeodeticRF { kCGRF, kWGS84, kROMA40, kITRF97Ep98_5 };
		enum  EProjection { kSPS, kLEP, kCERNSite, kCERNProjection };*/
		enum  ERefFrame { kCCS,
							kCGRF, kWGS84, kROMA40, kITRF97Ep98_5,
							kSPS, kLEP, kCERNSite, kCERNProjection };
	//@}


	/**@name Member Functions */
	//@{
		/// Instance method to obtain a pointer to the TReferenceFrameServer instance
		static	TReferenceFrameServer*  instance();

		/// Get a pointer to a specified reference frame
		TVReferenceFrame*  getRefFrame( ERefFrame );

		/// Get the transformation between two reference frames
		TVTransformation*  getTransformation( TVReferenceFrame* from, TVReferenceFrame* to );
		
	//@}


protected:
	/**@name Protected Constructors and Destructors */
	//@{
		/// Default Constructor 
		TReferenceFrameServer();

		/// Destructor
		virtual  ~TReferenceFrameServer();
	//@}




private:

	// Private Member Functions
	// Copy Constructor 
	TReferenceFrameServer(const  TReferenceFrameServer&);

	// Copy Assignment Operator 
	TReferenceFrameServer& operator=( const TReferenceFrameServer& );
	

	// set up the network of reference frames and the transformations linking them
	void  setupNetwork();


	// Reference Frame constructors

	// Get a pointer to a Reference Frame with the CERN Coordinate System parameters
	TVReferenceFrame*  getCCS();

	// Get a pointer to a Reference Frame with the SPS System parameters
	TVReferenceFrame*  getSPS();

	// Get a pointer to a Reference Frame with the LEP System parameters
	TVReferenceFrame*  getLEP();

	// Get a pointer to a Reference Frame with the CERN Site System parameters
	TVReferenceFrame*  getCERNSite();

	// Get a pointer to a Reference Frame with the CERN Projection parameters
	TVReferenceFrame*  getCERNProjection();

	// Get a pointer to a Reference Frame with the CERN Geodetic Reference Frame parameters
	TVReferenceFrame*  getCGRF();

	// Get a pointer to a Reference Frame with the WGS84 System parameters
	TVReferenceFrame*  getWGS84();

	// Get a pointer to a Reference Frame with the ROMA40 System parameters
	TVReferenceFrame*  getROMA40();

	// Get a pointer to a Reference Frame with the ITRF97 Epoch 98.5 System parameters
	TVReferenceFrame*  getITRF97Ep98_5();

	// Get the name of a specified reference frame
	string  getName( ERefFrame );


private:
	static	TReferenceFrameServer*  fInstance;

	static  TReferenceFrameNetwork*  fNetwork;
	static  bool  fNetworkSet;

	static  TEuclideanRefFrame*  fCCS;
	static  TProjectionXYHeRefFrame*  fSPS;
	static  TProjectionXYHgRefFrame*  fLEP;
	static  TProjectionXYHgRefFrame*  fCERNSite;
	static  TProjectionXYHeRefFrame*  fCERNProjection;
	static  TGeodeticRefFrame*  fCGRF;
	static  TGeodeticRefFrame*  fWGS84;
	static  TGeodeticRefFrame*  fROMA40;
	static  TGeodeticRefFrame*  fITRF97Ep98_5;

	//ClassDef(TReferenceFrameServer, 1)
};


//////////////////////////////////////////////////////////////////////
// Inline Definitions
//////////////////////////////////////////////////////////////////////



#endif // SU_REFERENCE_FRAME_SERVER
