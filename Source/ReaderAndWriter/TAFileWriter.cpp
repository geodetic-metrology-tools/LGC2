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
	static const char* descriptions[] = {
		"ANGLES HORIZONTAUX",
		"DISTANCES ZENITHALES : HAUTEUR D'INSTRUMENT CONNUE",
		"DISTANCES ZENITHALES",
		"DISTANCES MESUREES",
		"DISTANCES D'UN THEODOLITE",
		"ORIENTATIONS",
		"DISTANCES VERTICALES",
		"DISTANCES VERTICALES (DIGITAL LEVELING)",
		"DISTANCES REDUITES AU PLAN XY",
		"ECARTS DANS LE PLAN HORIZONTAL",
		"ECARTS PERPENDICULAIRES SPACIAUX",
		"ECARTS A UNE DIRECTION THEODOLITE",
		"ECARTS A UNE VERTICALE",
		"OBSERVATIONS RADIALES"
	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCConverter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
}


// Get the English version of the observation description
// returns a string with the description
string	TAFileWriter::getObsDescriptionEN(TALGCConverter::ELGCObservations key)
{
	static const char* descriptions[] = {
		"HORIZONTAL ANGLE OBSERVATIONS",
		"ZENITHAL DISTANCE OBSERVATIONS WITH FIXED INSTRUMENT HEIGHT",
		"ZENITHAL DISTANCE OBSERVATIONS WITH VARIABLE INSTRUMENT HEIGHT",
		"OBSERVED SPATIAL DISTANCES",
		"SPATIAL DISTANCES FROM A THEODOLITE",
		"ORIENTATION OBSERVATIONS",
		"VERTICAL DISTANCE OBSERVATIONS",
		"VERTICAL DISTANCE OBSERVATIONS (DIGITAL LEVELING)",
		"HORIZONTAL DISTANCE OBSERVATIONS",
		"ECHO OBSERVATIONS",
		"ECSP OBSERVATIONS",
		"ECTH OBSERVATIONS",
		"ECVE OBSERVATIONS",
		"RADI OBSERVATIONS"
	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCConverter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
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
