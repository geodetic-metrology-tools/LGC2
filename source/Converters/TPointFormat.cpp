// TPointFormat.cpp
//
/** Class used by a text stream to store and get all necessary formatting information*/
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
#include  "TPointFormat.h"

////////////////////////////////////////////////////////////////


//ClassImp(TPointFormat)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TPointFormat::TPointFormat()
{//default constructor

	fNameWidth = 10;
	fCoordWidth = 12;
	fCoordPrecision = k100Micrometres;
	fCoordErrorWidth = 6;
	fCoordErrorPrecision =  k100Micrometres;
	fShowSign = false;
	fStatus = TSpatialStatus::kCala;
}


TPointFormat::TPointFormat(int nameWidth, int coordWidth, ECoordPrecision coordPrecision, 
						   int coordErrorWidth, ECoordPrecision coordErrorPrecision,
						   bool showSign, TSpatialStatus::ESpatialStatus status):
fNameWidth(nameWidth), fCoordWidth(coordWidth), fCoordPrecision(coordPrecision),
fCoordErrorWidth(coordErrorWidth), fCoordErrorPrecision(coordErrorPrecision),
fShowSign(showSign), fStatus(status)
{//constructor	

}


TPointFormat::TPointFormat( const  TPointFormat& source )
{//copy constructor	
	(*this) = source;
}


TPointFormat::~TPointFormat()
{//destructor
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TPointFormat&  TPointFormat::operator=(const TPointFormat& source)
{// copy assignement operator	

	if (this != &source)
	{
		fNameWidth = source.fNameWidth;
		fCoordWidth = source.fCoordWidth;
		fCoordPrecision = source.fCoordPrecision;
		fCoordErrorWidth = source.fCoordErrorWidth;
		fCoordErrorPrecision = source.fCoordErrorPrecision;
		fShowSign = source.fShowSign;
		fStatus = source.fStatus;
	}
	return *this;
}

void  TPointFormat::setSpatialStatus(const TSpatialStatus::ESpatialStatus status)
{// copy assignement operator	

	fStatus = status;

	return;
}

///////////////////////////////////
// returns the coordinate precision
///////////////////////////////////
TPointFormat::ECoordPrecision TPointFormat::getCoordPrecision() const{

	return fCoordPrecision;
}

/////////////////////////////////////////
// returns the coordinate error precision
/////////////////////////////////////////
TPointFormat::ECoordPrecision TPointFormat::getCoordErrorPrecision() const{

	return fCoordErrorPrecision;
}

/////////////////////////////////////////
// returns the double value of the precision
/////////////////////////////////////////
double TPointFormat::getPrecision(ECoordPrecision currPrec)
{
	switch(currPrec){
	
		case TPointFormat::kMetre:
			return 0.1;
		case TPointFormat::k100Millimetres:
			return 0.01;
		case TPointFormat::k10Millimetres:
			return 0.001 ;
		case TPointFormat::kMillimetre:
			return 0.0001;
		case  TPointFormat::k100Micrometres:
			return 0.00001;
		case TPointFormat::k10Micrometres:
			return 0.000001;
		case TPointFormat::kMicrometre:
			return 0.0000001;
		case TPointFormat::k100Nanometres:
			return 0.00000001;
		default:
			return 0.000001;
		}
}

TPointFormat::ECoordPrecision TPointFormat::getPrecisionFromNumber(int prec)
{

	switch(prec){
	
		case 0:
			return TPointFormat::kMetre;
		case 1:
			return TPointFormat::k100Millimetres;
		case 2:
			return TPointFormat::k10Millimetres;
		case 3:
			return TPointFormat::kMillimetre;
		case 4:
			return TPointFormat::k100Micrometres;
		case 5:
			return TPointFormat::k10Micrometres;
		case 6:
			return TPointFormat::kMicrometre;
		case 7:
			return TPointFormat::k100Nanometres;
		default:
			return TPointFormat::k10Micrometres;

	}
}



