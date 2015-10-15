////////////////////////////////////////////////////////////////////
// TALGCConverter.cpp
/*!
Base Class for converters writing to an LGC output file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
//#include	<nag.h>
//#include	<nagg01.h>
#include	"TALGCConverter.h"
#include	"TAStreamFormatter.h"

/////////////////////////////////////////////////////////////////////

//ClassImp(TObservationConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
//default constructor
TALGCConverter::TALGCConverter(): TAConverter()
{}

/*
TALGCConverter::TALGCConverter(TAStreamFormatter* stream):
TAConverter(stream)
{
}*/

TALGCConverter::TALGCConverter(TAStreamFormatter& stream):TAConverter(stream)
{
}


TALGCConverter::~TALGCConverter()
{//destructor
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
string	TALGCConverter::getKeyWord(TALGCConverter::ELGCObservations key)
{
	// return the keyword string corresponding to the key
	//
	switch(key)
	{
	// Polar
	case TALGCConverter::kPLR:
		return "PLR";
		break;
	// Horizontal Angles
	case TALGCConverter::kANGL:
		return "ANGL";
		break;
	// Horizontal distances
	case TALGCConverter::kDHOR:
		return "DHOR";
		break;
	// Azimuth measurements
	case TALGCConverter::kORIE:
		return "ORIE";
		break;
	// Vertical Distances
	case TALGCConverter::kDVER:
		return "DVER";
		break;
	// Zenith Distances -unknown instrument height
	case TALGCConverter::kZEND:
		return "ZEND";
		break;
	// Spatial Distances
	case TALGCConverter::kDRDL:
		return "DRDL";
		break;
	case TALGCConverter::kDSPT:
		return "DSPT";
		break;
	case TALGCConverter::kECVE:
		return "ECVE";
		break;
	// ECSP observations
	case TALGCConverter::kECSP:
		return "ECSP";
		break;
	// ECHO observations
	case TALGCConverter::kECHO:
		return "ECHO";
		break;
	// ECTH observations
	case TALGCConverter::kECTH:
		return "ECTH";
		break;
	// radial constraints
	case TALGCConverter::kRADI:
		return "RADI";
		break;
	default:
		return "";
		break;
	}
}


void	TALGCConverter::writeKeyWord(const string &keyword)
{
	TAStreamFormatter& stream = this->getStreamRef();

	// Ouput the keyword to the stream
	//
	stream << keyword;
	stream << this->getSeparator();
	stream << endl;

	return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTECTED MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void TALGCConverter::setUnits(TLength::EUnits un)
{
	fStream->setLengthUnits(un);
}

void TALGCConverter::setUnits(TAngle::EUnits un)
{
	fStream->setAngleUnits(un);
}

bool TALGCConverter::isComment()
{
	fStream->skipWhiteSpace();
	char c = fStream->peek();
	return c == '%' || c == '#' || c == '$';
}

///////////////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////////////


