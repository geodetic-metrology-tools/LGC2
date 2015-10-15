// TObservationFormat.cpp
//
/** Class used by a text stream to store and get all necessary formatting information for observations*/
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TObservationFormat.h"
////////////////////////////////////////////////////////////////


//ClassImp(TObservationFormat)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////
// default constructor
//////////////////////
TObservationFormat::TObservationFormat(){

	fNameWidth = 10;
	fObsWidth = 12;
	fObsResidualWidth = 8;
	fLengthPrecision = k10Micrometres;
	fAnglePrecision = k100Microgons;
	fLengthResidualPrecision = k10Micrometres;
	fAngleResidualPrecision = k100Microgons;
	fShowSign = false;

}
//////////////
// constructor
//////////////
TObservationFormat::TObservationFormat(int nameWidth, 
									   int obsWidth, ELengthPrecision lengthPrecision, EAnglePrecision anglePrecision,
									   int obsResWidth, ELengthPrecision lengthResPrec, EAnglePrecision angleResPrec, 
									   bool showSign):
fNameWidth(nameWidth), fObsWidth(obsWidth), 
fLengthPrecision(lengthPrecision),fAnglePrecision(anglePrecision), 
fObsResidualWidth(obsResWidth), 
fLengthResidualPrecision(lengthResPrec), fAngleResidualPrecision(angleResPrec), 
fShowSign(showSign)
{	

}

///////////////////
// copy constructor
///////////////////
TObservationFormat::TObservationFormat( const  TObservationFormat& source ):
fNameWidth(source.getNameWidth()), fObsWidth(source.getObsWidth()), 
fLengthPrecision(source.getLengthPrecision()), fAnglePrecision(source.getAnglePrecision()), 
fObsResidualWidth(source.getObsResidualWidth()),
fLengthResidualPrecision(source.getLengthResidualPrecision()), 
fAngleResidualPrecision(source.getAngleResidualPrecision()),
fShowSign(source.getShowSign())
{	

}

/////////////
// destructor
/////////////
TObservationFormat::~TObservationFormat()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
////////////////////////////
// copy assignement operator
////////////////////////////
TObservationFormat&  TObservationFormat::operator=(const TObservationFormat& source){	

	if (this != &source)
	{
		fNameWidth = source.getNameWidth();
		fObsWidth = source.getObsWidth();
		fObsResidualWidth = source.getObsResidualWidth();
		fLengthPrecision = source.getLengthPrecision();
		fAnglePrecision = source.getAnglePrecision();
		fLengthResidualPrecision = source.getLengthResidualPrecision();
		fAngleResidualPrecision = source.getAngleResidualPrecision();
		fShowSign = source.getShowSign();
	}
	return *this;
}


////////////////////////////////
// returns the lengths precision
////////////////////////////////
TObservationFormat::ELengthPrecision TObservationFormat::getLengthPrecision() const{

	return fLengthPrecision;
}

////////////////////////////////
// returns the Angles precision
////////////////////////////////
TObservationFormat::EAnglePrecision TObservationFormat::getAnglePrecision() const{

	return fAnglePrecision;
}

/////////////////////////////////////////
// returns the length residuals precision
/////////////////////////////////////////
TObservationFormat::ELengthPrecision TObservationFormat::getLengthResidualPrecision() const{

	return fLengthResidualPrecision;
}

////////////////////////////////
// returns the Angles precision
////////////////////////////////
TObservationFormat::EAnglePrecision TObservationFormat::getAngleResidualPrecision() const{

	return fAngleResidualPrecision;
}





