//  TReferenceFrameServer.cpp
//
/** A class that instantiates and provides access to all available
	reference frames.

  
    Patterns:
	A singleton to ensure only one such server exists.
     
    Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TAngle.h"
#include  "TLength.h"
#include  "TVReferenceEllipsoid.h"
#include  "TVGeoid.h"
#include  "TVTopocentricDatumPosition.h"
#include  "TDatumServer.h"
//
#include  "TVReferenceFrame.h"
#include  "TEuclideanRefFrame.h"
#include  "TGeodeticRefFrame.h"
//#include  "TProjectionXYHRefFrame.h"
#include  "TReferenceFrameNetwork.h"
#include  "TReferenceFrameServer.h"
////////////////////////////////////////////////////////////////


//ClassImp(TReferenceFrameServer)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

TReferenceFrameServer*  TReferenceFrameServer::fInstance = 0;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TReferenceFrameServer::TReferenceFrameServer()
{	// default constructor
	
	fNetwork = 0;
	fNetworkSet = false;

	// initialise all the pointers to null
	// the reference frames
	fCCS = 0;
	fSPS = 0;
	fLEP = 0;
	fCERNSite = 0;
	fCERNProjection = 0;
	fCGRF = 0;
	fWGS84 = 0;
	fROMA40 = 0;
	fITRF97Ep98_5 = 0;

}


// **Deliberately not implemented**
//TReferenceFrameServer::TReferenceFrameServer( const  TReferenceFrameServer& original )
//{	// copy constructor
//}


TReferenceFrameServer::~TReferenceFrameServer()
{
	// delete all the objects that have been made
	if( fCCS != 0) delete fCCS;
	//if( fSPS != 0) delete fSPS;
	//if( fLEP != 0) delete fLEP;
	//if( fCERNSite != 0) delete fCERNSite;
	//if( fCERNProjection != 0) delete fCERNProjection;
	if( fCGRF != 0) delete fCGRF;
	if( fWGS84 != 0) delete fWGS84;
	if( fROMA40 != 0) delete fROMA40;
	if( fITRF97Ep98_5 != 0) delete fITRF97Ep98_5;

}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


// **Deliberately not implemented**
//TReferenceFrameServer&  TReferenceFrameServer::operator=(const TReferenceFrameServer& right)
//{	// Copy Assignment operator
//
//	if (this != &right)
//	{
//	}
//	return *this;
//}


TReferenceFrameServer*  TReferenceFrameServer::instance()
{	// return a pointer to the instance of the TReferenceFrameServer
	
	if( fInstance == 0 )
	{
		fInstance = new TReferenceFrameServer();
	}

	return fInstance;
}


//
// Return a pointer to the specified reference frame
//
TVReferenceFrame*  TReferenceFrameServer::getRefFrame( TReferenceFrameServer::ERefFrame  nameEnum )
{	// if the ellipsoid instance doesn't yet exist, make it
	
	TVReferenceFrame*  frame = 0;

	switch( nameEnum )
	{
	case kCCS:
		frame = getCCS();
		break;

	case kSPS:
		frame = getSPS();
		break;

	case kLEP:
		frame = getLEP();
		break;

	case kCERNSite:
		frame = getCERNSite();
		break;

	case kCERNProjection:
		frame = getCERNProjection();
		break;

	case kCGRF:
		frame = getCGRF();
		break;

	case kWGS84:
		frame = getWGS84();
		break;

	case kROMA40:
		frame = getROMA40();
		break;

	case kITRF97Ep98_5:
		frame = getITRF97Ep98_5();
		break;


	default:
		break;
	}

	// return a pointer to the reference frame
	return frame;
}


//
// Return a pointer to the instance of the CERN Cartesian Coordinate System
//
TVReferenceFrame*  TReferenceFrameServer::getCCS()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "CERN Cartesian Coordinate System";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{
		frame = fNetwork->insert( new TEuclideanRefFrame( name ) );
	}

	return frame;
}


//
// Return a pointer to the instance of the SPS Projection
//
TVReferenceFrame*  TReferenceFrameServer::getSPS()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "CERN Orthographic Projection (HSPS)";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{

	}

	return frame;
}


//
// Return a pointer to the instance of the LEP Projection
//
TVReferenceFrame*  TReferenceFrameServer::getLEP()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "CERN Orthographic Projection (HLEP)";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{

	}

	return frame;
}


//
// Return a pointer to the instance of the CERN Site Projection
//
TVReferenceFrame*  TReferenceFrameServer::getCERNSite()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "CERN Orthographic Projection (HSURF)";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{

	}

	return frame;
}


//
// Return a pointer to the instance of the CERN Projection
//
TVReferenceFrame*  TReferenceFrameServer::getCERNProjection()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "CERN Orthographic Projection (LevZero)";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{

	}

	return frame;
}


//
// Return a pointer to the instance of the CERN Geodetic Reference Frame
//
TVReferenceFrame*  TReferenceFrameServer::getCGRF()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "CERN Geodetic Reference Frame";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{
		// get a pointer to the GRS80 ellipsoid
		TDatumServer*  server = TDatumServer::instance();
		TVReferenceEllipsoid*  ellipsoid = server->getRefEllipsoid( TDatumServer::kGRS80 );

		frame = fNetwork->insert( new TGeodeticRefFrame( name, ellipsoid ) );
	}

	return frame;
}


//
// Return a pointer to the instance of the WGS84 Reference Frame
//
TVReferenceFrame*  TReferenceFrameServer::getWGS84()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "WGS84 Reference Frame";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{
		// get a pointer to the WGS84 ellipsoid
		TDatumServer*  server = TDatumServer::instance();
		TVReferenceEllipsoid*  ellipsoid = server->getRefEllipsoid( TDatumServer::kWGS84Ell );

		frame = fNetwork->insert( new TGeodeticRefFrame( name, ellipsoid ) );
	}

	return frame;
}


//
// Return a pointer to the instance of the ROMA40 Reference Frame
//
TVReferenceFrame*  TReferenceFrameServer::getROMA40()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "ROMA40 Reference Frame";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{
		// get a pointer to the GRS80 ellipsoid
		TDatumServer*  server = TDatumServer::instance();
		TVReferenceEllipsoid*  ellipsoid = server->getRefEllipsoid( TDatumServer::kInternational );

		frame = fNetwork->insert( new TGeodeticRefFrame( name, ellipsoid ) );
	}

	return frame;
}


//
// Return a pointer to the instance of the ITRF97 (epoch 98.5)
//
TVReferenceFrame*  TReferenceFrameServer::getITRF97Ep98_5()
{	// if the reference frame instance doesn't yet exist, make it
	
	// search the network for the requested reference frame
	string name = "ITRF97 (epoch 98.5)";
	TVReferenceFrame* frame = fNetwork->find( name );

	// if requested reference frame not found create it and add it to the network
	if( 0 == frame )
	{
		// get a pointer to the GRS80 ellipsoid
		TDatumServer*  server = TDatumServer::instance();
		TVReferenceEllipsoid*  ellipsoid = server->getRefEllipsoid( TDatumServer::kGRS80 );

		frame = fNetwork->insert( new TGeodeticRefFrame( name, ellipsoid ) );
	}

	return frame;
}


//
// Get the transformation between two reference frames
//
TVTransformation*  TReferenceFrameServer::getTransformation( TVReferenceFrame* from, 
															 TVReferenceFrame* to )
{
	TVTransformation* transformer = 0;

	// If the reference frame network is not yet initialised
	// ensure it is complete
	if( !fNetworkSet )
	{
		setupNetwork();
	}

	
	// find the transformation from the network
	transformer = fNetwork->getTransformation( from, to );

	return transformer;
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

