////////////////////////////////////////////////////////////////////
// TCooFileWriter.cpp :Implementation file
// Write a *.coo LGC output file
// Creates the summary file from the data read and sends the appropriate messages
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations

#include	"TLGCDataSet.h"
#include	"LSCalcDataSet.h"
#include	"TSeparatedFormatTStream.h"
#include	"TCooFileWriter.h"

#include	"TPointConverter.h"
#include	"TLGCApplication.h"


/////////////////////////////////////////////////////////////////////

//ClassImp(TCooFileWriter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TCooFileWriter::TCooFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TAFileWriter(stream, project)
{//constructor
}


TCooFileWriter::TCooFileWriter() : TAFileWriter()
{//default constructor
}


TCooFileWriter::~TCooFileWriter()
{//destructor
}

void	TCooFileWriter::writeTitle()
{
	TAStreamFormatter* stream =	getStream();
	//write software id.
	TDataParameters dataParams = getDataSet()->getDataParams();
	TLGCDataSet* dataSet = getDataSet();
	(*stream)<<(TLGCApplication::getProgId()) << "  " << "FMT:";
	if (dataSet->getPunchFileFormat() == TAStreamFormatter::kSeparatorFormat)
	{
		(*stream)<<"SEP '" << dataSet->getPunchSeparator()<<"' ";
	}
	else
	{
		(*stream)<<"COL ";
	}
	(*stream)<<"PREC: " << dataParams.getCoordPrecision()<<endl<<endl;
	
	//if (dataSet->getPunchFileFormat() != TAStreamFormatter::ETextFormat::kSeparatorFormat)
	//	(*stream) << endl;

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TCooFileWriter::writeFile(TLGCDataSet* ds, LSCalcDataSet* LSds)
{//Write file if there no error in the project
	string error = "";
	init(ds, LSds, error);
	writeTitle();
	writePointsHeader();
	writePoints();
	
	return;
}


void	TCooFileWriter::writePointsHeader()
{
	int					nameWidth = this->getNameWidth();
	int					coordWidth = this->getCoordWidth();
	int					coordResWidth = this->getCoordResWidth();
	
	TAStreamFormatter* stream =	this->getStream();
	string			   separator = stream->getSeparator();
	TPointConverter converter (stream, getRefFrameForH());

	//Point;X;Y;ZorH;ID;DX;DY;DZ;DCUM;OPTION
	(*stream).writeString(nameWidth,		"POINT");//Nom du Point;

	(*stream).writeString( coordWidth,		"X");//X

	(*stream).writeString( coordWidth,		"Y");//Y

	//write header for H or Z coordinate as appropriate
	if(converter.isInLocalSystem())
		(*stream).writeString( coordWidth,		"Z");//Z
	else
		(*stream).writeString( coordWidth,		"H(G)");//H

	(*stream).writeString( coordResWidth,		"ID");// dB ID

	(*stream).writeString( coordResWidth,	"DX");//X offset 

	(*stream).writeString( coordResWidth,	"DY");//Y offset

	(*stream).writeString(	coordResWidth,	"DZ");//Z offset 

	(*stream).writeString( coordWidth,		"DCUM");// cumulative distance;

	(*stream).writeString( coordResWidth,		"OPTION");// type of point in calculation;

	(*stream)<<endl;
}

//////////////////////////////////////////////////////////////////////////////////////
//WRITE GENERAL METHOD
//////////////////////////////////////////////////////////////////////////////////////
void	TCooFileWriter::writePoints()
{//write point

	TAStreamFormatter* stream =	getStream();
	
	//list iterator
	PointConstIter	pointIterator, endPointIterator;
	pointIterator		= getDataSet()->getWorkingPoints()->getPointsBeginIterator();
	endPointIterator	= getDataSet()->getWorkingPoints()->getPointsEndIterator();



	//write the point's list
	while(pointIterator!=endPointIterator)
	{
		writeCooData(getLSCalcDataSet()->getPV(pointIterator->getPtName()), pointIterator);
		pointIterator++;
	}

	(*stream)<<endl<<endl<<endl;
	return;
}



//////////////////////////////////////////////////////////////////////////////////////
//WRITE DATA
//////////////////////////////////////////////////////////////////////////////////////
void	TCooFileWriter::writeCooData( LSPosVecConstIter pt,
									  PointConstIter pointIterator)
{

	int	nameWidth = this->getNameWidth();
	int	coordWidth = this->getCoordWidth();
	int	coordResWidth = this->getCoordResWidth();
	TAStreamFormatter* stream =	getStream();
	TPointConverter converter (stream, getRefFrameForH());
	string separator = stream->getSeparator();
	TLength thirdCoord;
		
	//write name
	converter.writeName(pt, nameWidth);

	//write coordinate
	converter.writeXYZorH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, pt);

	//write database Id
	(*stream).writeInteger(coordResWidth, pointIterator->getName().getId());

	// get Z or H coordinate as required
	if(converter.isInLocalSystem())
	{
		thirdCoord = pt->getDZValue();
	}
	else
	{
		thirdCoord = pt->getDHValue( getRefFrameForH());
	}

	// write the coordinate displacements
	converter.writeCoordinateParam(pt->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								pt->getDXValue(),
								pt->getDYValue(),
								thirdCoord,
								"0.0");
	
	//write the point's cumulative distance
	(*stream).writeDouble(coordWidth, getCoordPrecision(), pointIterator->getDist());

	//Point's type
	(*stream).writeString(coordResWidth, pt->getPosVecStatus());

	(*stream)<<endl;

	return;
}

////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////
