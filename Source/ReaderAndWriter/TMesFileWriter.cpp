////////////////////////////////////////////////////////////////////
// TMesFileWriter.cpp :Implementation file
// Writes an LGC measurements file
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TMesFileWriter.h"
#include	"TLGCProject.h"
#include	"TLGCDataSet.h"
#include	"LSCalcDataSet.h"
#include	"TSeparatedFormatTStream.h"
#include	"TPointFormat.h"
#include	"TSpatialStatus.h"


#include	"THorAngleConverter.h"
#include	"TZenithDistConverter.h"
#include	"TGyroOrientationConverter.h"
#include	"TVertDistConverter.h"
#include	"THorDistConverter.h"
#include	"TSpatialDistConverter.h"
#include	"TOffsetToVerLineConverter.h"
#include	"TOffsetToVerPlaneConverter.h"
#include	"TOffsetToSpaLineConverter.h"
#include	"TOffsetToTheoPlaneConverter.h"
#include	"TRadialOffsetCnstrConverter.h"

/////////////////////////////////////////////////////////////////////

//ClassImp(TMesFileWriter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TMesFileWriter::TMesFileWriter() : TAFileWriter()
{//default constructor
}


TMesFileWriter::TMesFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TAFileWriter(stream, project)
{//constructor
}


TMesFileWriter::~TMesFileWriter()
{//destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeFile(TLGCDataSet* ds, LSCalcDataSet* LSds)
{//Write file if there no error in the project
	string error = "";
	init(ds, LSds, error);

	if( (ds->getHADimension()) != 0 )
	{writeHorAng();}

	if( (ds->getZDDimension(TAMeasurement::kFixed)) != 0 )
	{writeZenDist(TALSCalcParameter::kFixed);}

	if( (ds->getZDDimension(TAMeasurement::kVariable)) != 0 )
	{writeZenDist(TALSCalcParameter::kVariable);}	

	if( (ds->getSDDimension(TAMeasurement::kFixed)) != 0 )
	{writeSpaDist(TALSCalcParameter::kFixed);}

	if( (ds->getSDDimension(TAMeasurement::kVariable)) != 0 )
	{writeSpaDist(TALSCalcParameter::kVariable);}
	
	if( (ds->getGyroOrieDimension()) != 0 )
	{writeGyroOrie();}

	// cover DVER and DLEV
	for (int i = 0; i < 2; i++)
		if( (ds->getVDDimension(i==1)) != 0 )
			writeVertDist(i==1);

	if( (ds->getHDDimension()) != 0 )
	{writeHorDist();}

	if( (ds->getOffsetToVerPlaneDimension()) != 0 )
	{writeOffsetToVerPlane();}

	if( (ds->getOffsetToSpaLineDimension()) != 0 )
	{writeOffsetToSpaLine();}
	
	if( (ds->getOffsetToTheoPlaneDimension()) != 0 )
	{writeOffsetToTheoPlane();}

	if( (ds->getOffsetToVerLineDimension()) != 0 )
	{writeOffsetToVerLine();}

	if( (ds->getRadOffCnstrDimension()) != 0 )
	{writeRadOffCnstr();}

	return;
}



//////////////////////////////////////////////////////////////////////////////////
//SPATIAL DISTANCE
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeSpaDist(const TALSCalcParameter::ELSStatus status)
{//write spatial distance

	TSpatialDistConverter converter (getStreamRef(), status);
	//List
	LSSpaDistConstIter	obsIt, endObsIt;
	obsIt			= getLSCalcDataSet()->beginLSSpaDist();
	endObsIt		= getLSCalcDataSet()->endLSSpaDist();

	//Title
	if( status == TALSCalcParameter::kFixed )
		this->writeKeyWord(this->getKeyWord(TALGCConverter::kDMES));
	else
		this->writeKeyWord(this->getKeyWord(TALGCConverter::kDTHE));

	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}
	return;
}


//////////////////////////////////////////////////////////////////////////////////
//HORIZONTAL DISTANCE
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeHorDist()
{//write spatial distance

	THorDistConverter converter (getStreamRef());
	//List
	LSHorDistConstIter	obsIt, endObsIt;
	obsIt			= getLSCalcDataSet()->beginLSHorDist();
	endObsIt		= getLSCalcDataSet()->endLSHorDist();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kDHOR));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//GYRO. ORIENTATION
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeGyroOrie()
{//write spatial distance

	TGyroOrientationConverter converter (getStreamRef());
	//List
	LSGyroOrieConstIter	obsIt, endObsIt;
	obsIt			= getLSCalcDataSet()->beginLSGyroOrie();
	endObsIt		= getLSCalcDataSet()->endLSGyroOrie();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kORIE));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//VERTICAL DISTANCE
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeVertDist(bool isDLEV)
{//write spatial distance

	TVertDistConverter converter (getStreamRef());
	//List
	LSVertDistConstIter	obsIt, endObsIt;
	obsIt			= getLSCalcDataSet()->beginLSVertDist(isDLEV);
	endObsIt		= getLSCalcDataSet()->endLSVertDist(isDLEV);

	//Title
	TALGCConverter::ELGCObservations key = isDLEV ? TALGCConverter::kDLEV : TALGCConverter::kDVER;
	this->writeKeyWord(this->getKeyWord(key));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//HORIZONTAL ANGLE
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeHorAng()
{//write spatial distance

	THorAngleConverter converter (getStreamRef());
	//List
	LSHorAngConstIter	obsIt, endObsIt;
	obsIt			= getLSCalcDataSet()->beginLSHorAng();
	endObsIt		= getLSCalcDataSet()->endLSHorAng();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kANGL));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//ZENITHAL DISTANCE
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeZenDist(const TALSCalcParameter::ELSStatus status)
{//write spatial distance

	TZenithDistConverter converter (getStreamRef(), status);
	//List
	LSZenDistConstIter	obsIt, endObsIt;
	obsIt			= getLSCalcDataSet()->beginLSZenDist();
	endObsIt		= getLSCalcDataSet()->endLSZenDist();

	//Title
	if( status == TALSCalcParameter::kFixed )
		this->writeKeyWord(this->getKeyWord(TALGCConverter::kZENH));
	else
		this->writeKeyWord(this->getKeyWord(TALGCConverter::kZENI));

	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//ECVE OFFSET
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeOffsetToVerLine()
{
	TOffsetToVerLineConverter converter (getStreamRef());
	//List
	LSOffsetToVerLineConstIter	obsIt, endObsIt;
	obsIt					= getLSCalcDataSet()->beginLSOffsetToVerLine();
	endObsIt				= getLSCalcDataSet()->endLSOffsetToVerLine();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kECVE));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//ECSP OFFSET
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeOffsetToSpaLine()
{
	TOffsetToSpaLineConverter converter (getStreamRef());
	//List
	LSOffsetToSpaLineConstIter	obsIt, endObsIt;
	obsIt					= getLSCalcDataSet()->beginLSOffsetToSpaLine();
	endObsIt				= getLSCalcDataSet()->endLSOffsetToSpaLine();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kECSP));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//ECHO OFFSET
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeOffsetToVerPlane()
{

	TOffsetToVerPlaneConverter converter (getStreamRef());
	//List
	LSOffsetToVerPlaneConstIter	obsIt, endObsIt;
	obsIt					= getLSCalcDataSet()->beginLSOffsetToVerPlane();
	endObsIt				= getLSCalcDataSet()->endLSOffsetToVerPlane();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kECHO));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//ECTH OFFSET
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeOffsetToTheoPlane()
{
	TOffsetToTheoPlaneConverter converter (getStreamRef());
	//List
	LSOffsetToTheoPlaneConstIter	obsIt, endObsIt;
	obsIt					= getLSCalcDataSet()->beginLSOffsetToTheoPlane();
	endObsIt				= getLSCalcDataSet()->endLSOffsetToTheoPlane();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kECTH));
	//write results
	while(obsIt != endObsIt)
	{
		converter.writeMesData(obsIt);	
		obsIt++;
	}

	return;
}


//////////////////////////////////////////////////////////////////////////////////
//RADIAL CONSTRAINT
//////////////////////////////////////////////////////////////////////////////////
void	TMesFileWriter::writeRadOffCnstr()
{

	TRadialOffsetCnstrConverter converter (getStreamRef());
	//List
	LSRadOffCnstrConstIter	cnstrIt, endCnstrIt;
	cnstrIt					= getLSCalcDataSet()->beginLSRadOffCnstr();
	endCnstrIt				= getLSCalcDataSet()->endLSRadOffCnstr();

	//Title
	this->writeKeyWord(this->getKeyWord(TALGCConverter::kRADI));

	//write results
	while(cnstrIt != endCnstrIt)
	{
		converter.writeMesData(cnstrIt);	
		cnstrIt++;
	}

	return;
}


////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////