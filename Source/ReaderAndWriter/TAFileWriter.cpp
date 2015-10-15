////////////////////////////////////////////////////////////////////
// TAFileWriter.cpp :Implementation file
// Abstract base class for LGC output file writers
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TAFileWriter.h"
#include	"TLGCProject.h"
#include	"TLGCDataSet.h"
#include	"LSCalcDataSet.h"
#include	"TSeparatedFormatTStream.h"


/////////////////////////////////////////////////////////////////////

//ClassImp(TAFileWriter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
//default constructor
TAFileWriter::TAFileWriter() : TALGCConverter()
{
	fDataSet=0;
	fRefFrame = TRefSystemFactory::kCCS;

	fLSCalcDataSet=0;

	fUsingSAPriori = false;

	fHistOption = false;

	fError = "";
	fPuncOptions = TLGCOutputOptions::kDefault;		
}

//constructor
TAFileWriter::TAFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TALGCConverter(*stream)
{
	fDataSet=0;
	fRefFrame = TRefSystemFactory::kCCS;

	fLSCalcDataSet=0;
		
	TLGCOutputOptions* outOptions = project->getOutputOptions();
	TLGCCalcParams* calcParams = project->getCalcParams();
	setOutputOptions(outOptions);
	setCalcParams(calcParams);

	fError = "";
}


TAFileWriter::~TAFileWriter()
{//destructor
	getStream()->close();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TAFileWriter::writeFile(TLGCDataSet* ds, const string error)
{//write error messages from project
	LSCalcDataSet* LSds = 0;
	init(ds, LSds, error);

	writeError();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//INITIALISE ATTRIBUTS
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TAFileWriter::init(TLGCDataSet* ds , LSCalcDataSet* LSds, const string error)
{
	//ERROR
	fError = error;

	//DATA SET
	fDataSet = ds;
	fLSCalcDataSet = LSds;

	//REFERENCE FRAME
	if(ds->getDataParams().getRefFrameEnumerator() == TRefSystemFactory::kCernXYHg85Machine)
	{
		fRefFrame = TRefSystemFactory::kCernXYHg85Machine;
	}
	
	if(ds->getDataParams().getRefFrameEnumerator() ==  TRefSystemFactory::kCERNXYHsSphereSPS)
	{
		fRefFrame = TRefSystemFactory::kCERNXYHsSphereSPS;
	}

	if(ds->getDataParams().getRefFrameEnumerator() ==  TRefSystemFactory::kCernXYHg00Machine)
	{
		fRefFrame = TRefSystemFactory::kCernXYHg00Machine;
	}


	return;
}


// Get the French version of the observation description
// returns a string with the description
string	TAFileWriter::getObsDescriptionFR(TALGCConverter::ELGCObservations key)
{
	// return the keyword string corresponding to the key
	//
	switch(key)
	{
	// Polar 3D
	case TALGCConverter::kPLR:
		return "POLAR 3D OBSERVATIONS";
		break;
	// Horizontal Angles
	case TALGCConverter::kANGL:
		return "ANGLES HORIZONTAUX";
		break;
	// Horizontal distances
	case TALGCConverter::kDHOR:
		return "DISTANCES REDUITES AU PLAN XY";
		break;
	// Azimuth measurements
	case TALGCConverter::kORIE:
		return "ORIENTATIONS";
		break;
	// Vertical Distances
	case TALGCConverter::kDVER:
		return "DISTANCES VERTICALES";
		break;
	// Level Observation
	case TALGCConverter::kDLEV:
		return "DISTANCES VERTICALES";
		break;
	// Zenith Distances
	case TALGCConverter::kZEND:
		return "DISTANCES ZENITHALES";
		break;
	// Spatial Distances
	case TALGCConverter::kDRDL:
		return "DISTANCES MESUREES";
		break;
	// EDM Spatial Distances
	case TALGCConverter::kDSPT:
		return "OBSERVED EDM SPATIAL DISTANCES";
		break;
	// ECVE observations
	case TALGCConverter::kECVE:
		return "ECARTS A UNE VERTICALE";
		break;
	// ECSP observations
	case TALGCConverter::kECSP:
		return "ECARTS PERPENDICULAIRES SPACIAUX";
		break;
	// ECHO observations
	case TALGCConverter::kECHO:
		return "ECARTS DANS LE PLAN HORIZONTAL";
		break;
	// ECTH observations
	case TALGCConverter::kECTH:
		return "ECARTS A UNE DIRECTION THEODOLITE";
		break;
	// radial constraints
	case TALGCConverter::kRADI:
		return "OBSERVATIONS RADIALES";
		break;
	default:
		break;
	}
	return "";
}


// Get the English version of the observation description
// returns a string with the description
string	TAFileWriter::getObsDescriptionEN(TALGCConverter::ELGCObservations key)
{
	// return the keyword string corresponding to the key
	//
	switch(key)
	{
	// Polar 3D
	case TALGCConverter::kPLR:
		return "POLAR 3D OBSERVATIONS";
		break;
	// Horizontal Angles
	case TALGCConverter::kANGL:
		return "HORIZONTAL ANGLE OBSERVATIONS";
		break;
	// Horizontal distances
	case TALGCConverter::kDHOR:
		return "HORIZONTAL DISTANCE OBSERVATIONS";
		break;
	// Azimuth measurements
	case TALGCConverter::kORIE:
		return "ORIE OBSERVATIONS";
		break;
	// Vertical Distances
	case TALGCConverter::kDVER:
		return "VERTICAL DISTANCE OBSERVATIONS";
		break;
	// Level Observation
	case TALGCConverter::kDLEV:
		return "VERTICAL DISTANCE OBSERVATIONS";
		break;
	// Zenith Distances -fixed instrument height
	case TALGCConverter::kZEND:
		return "ZENITHAL DISTANCE OBSERVATIONS";
		break;
	// Spatial Distances
	case TALGCConverter::kDRDL:
		return "OBSERVED SPATIAL DISTANCES";
		break;
	// EDM Spatial Distances
	case TALGCConverter::kDSPT:
		return "OBSERVED EDM SPATIAL DISTANCES";
		break;
	// ECVE observations
	case TALGCConverter::kECVE:
		return "ECVE OBSERVATIONS";
		break;
	// ECSP observations
	case TALGCConverter::kECSP:
		return "ECSP OBSERVATIONS";
		break;
	// ECHO observations
	case TALGCConverter::kECHO:
		return "ECHO OBSERVATIONS";
		break;
	// ECTH observations
	case TALGCConverter::kECTH:
		return "ECTH OBSERVATIONS";
		break;
	// radial constraints
	case TALGCConverter::kRADI:
		return "RADI OBSERVATIONS";
		break;
	default:
		break;
	}
	return "";
}



///////////////////////////////////////////////////////////
//ERROR
///////////////////////////////////////////////////////////
string	TAFileWriter::getError() const
{//ERROR
	return fError;
}


void	TAFileWriter::setError(const string error)
{
	fError =fError + error +'\n';
	return;
}


void	TAFileWriter::writeError()
{
	(*getStream())<<fError;
	return;
}


//////////////////////////////////////////////////////////////////////
//OUPUT OPTIONS & CALC OPTIONS
//////////////////////////////////////////////////////////////////////
void	TAFileWriter::setOutputOptions(TLGCOutputOptions* outOptions)
{
	fPuncOptions = outOptions->isPunchfileToBeSaved();	
	fHistOption = outOptions->isHistOptionUsed();
	return;
}


void	TAFileWriter::setCalcParams(const TLGCCalcParams* param)
{
	fUsingSAPriori = param->usingSigmaAPriori();
	fRefSurface = param ->getRefSurface();
	return;
}


////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////
