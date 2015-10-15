////////////////////////////////////////////////////////////////////
// TPunchFileWriter.cpp :Implementation file
//
//Write an LGC output punch (summary) file
//Creates a file from the calculation results and sends the appropriate messages
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	<ctime>
//#include	<cmath>
#include	"TLGCProject.h"
#include	"TLGCDataSet.h"
#include	"LSCalcDataSet.h"
#include	"TSeparatedFormatTStream.h"
#include	"TPunchFileWriter.h"
#include	"TLGCApplication.h"
#include	"TDataParameters.h"
#include	"TAStreamFormatter.h"


/////////////////////////////////////////////////////////////////////

//ClassImp(TPunchFileWriter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TPunchFileWriter::TPunchFileWriter() : TAFileWriter()
{//default constructor
}


TPunchFileWriter::TPunchFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TAFileWriter(stream, project)
{//default constructor
}


TPunchFileWriter::~TPunchFileWriter()
{//destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TPunchFileWriter::writeFile(TLGCDataSet* ds, LSCalcDataSet* LSds)
{//Write file if there no error in the project
	string error = "";
	init(ds, LSds, error);

	writePoints();
	
	return;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
//TITLE
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void	TPunchFileWriter::writeTitle()
{

	TAStreamFormatter* stream =	getStream();

	//write title
	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<"RESUME DES COORDONNEES POUR :"<<endl<<endl;
	(*stream)<<(TLGCProject::getTitle())<<endl;
	(*stream)<<endl;

	//write time
    char tmpbuf[128];
    time_t ltime;
	time(&ltime);
    struct tm *today;
	_tzset();
    today = localtime( &ltime );
	string essai = ctime( &ltime );
	strftime( tmpbuf, 128,"CALCUL DU %d %B %Y %X", today );
	(*stream)<<tmpbuf<<endl;
	(*stream)<<"*********************************************************************************************************************************** "<<endl<<endl;
	TDataParameters dataParams = getDataSet()->getDataParams();
	TLGCDataSet* dataSet = getDataSet();
	(*stream)<<(TLGCApplication::getProgId())<<" "<<(*stream).getSeparator()<<"FMT:" << dataSet->getFormatType();
	if (dataSet->getPunchFileFormat() == TAStreamFormatter::kSeparatorFormat)
	{
		(*stream)<<" '" << dataSet->getPunchSeparator()<<"'"<<(*stream).getSeparator();
	}
	else
	{
		(*stream)<<" ";
	}
	(*stream)<<"PREC: " << dataParams.getCoordPrecision()<<(*stream).getSeparator()<<endl<<endl;


}


//////////////////////////////////////////////////////////////////////////////////////
//WRITE GENERAL METHOD
//////////////////////////////////////////////////////////////////////////////////////
void	TPunchFileWriter::writePoints()
{//write point

	TAStreamFormatter* stream =	getStream();
	
	//list iterator
	LSPosVecConstIter	posVecIterator, endPosVecIterator;
	posVecIterator		= getLSCalcDataSet()->beginPV();
	endPosVecIterator	= getLSCalcDataSet()->endPV();


	switch(getPuncOptions())
		{
			case TLGCOutputOptions::kDefault:
				writeTitle();
				writeXYZHeader();
				break;

			case TLGCOutputOptions::kE:
				writeTitle();
				writeXYZVarCovarDeltaHeader();
				break;

			case TLGCOutputOptions::kEE:
				writeTitle();
				writeXYZErrorEllSigZDeltaHeader();
				break;

			case TLGCOutputOptions::kH:
				writeTitle();
				writeXYHHeader();
				break;

			case TLGCOutputOptions::kZ:
				writeTitle();
				writeXYZHeader();
				break;

			case TLGCOutputOptions::kZH:
				writeTitle();
				writeXYZHHeader();
				break;

			case TLGCOutputOptions::kHN:
				writeTitle();
				writeXYHNHeader();
				break;

			case TLGCOutputOptions::kZHN:
				writeTitle();
				writeXYZHNHeader();
				break;

			case TLGCOutputOptions::kT:
				writeXYZSigmaHeader();
				break;
				
			default:
				break;
		}


	///////////////////////////////////////////////////////
	//Results
	while(posVecIterator != endPosVecIterator)
	{
		switch(getPuncOptions())
		{
			case TLGCOutputOptions::kDefault:
				writeXYZData(posVecIterator);
				break;

			case TLGCOutputOptions::kE:
				writeXYZVarCovarDeltaData(posVecIterator);
				break;

			case TLGCOutputOptions::kEE:
				writeXYZErrorEllSigZDeltaData(posVecIterator);
				break;

			case TLGCOutputOptions::kH:
				writeXYHData(posVecIterator);
				break;

			case TLGCOutputOptions::kZ:
				writeXYZData(posVecIterator);
				break;

			case TLGCOutputOptions::kZH:
				writeXYZHData(posVecIterator);
				break;

			case TLGCOutputOptions::kHN:
				writeXYHNData(posVecIterator);
				break;

			case TLGCOutputOptions::kZHN:
				writeXYZHNData(posVecIterator);
				break;

			case TLGCOutputOptions::kT:
				writeXYZSigmaData(posVecIterator);
				break;
				
			default:
				break;
		}
		posVecIterator++;
	}

	(*stream)<<endl<<endl<<endl;
	return;
}



void	TPunchFileWriter::writeXYZHeader()
{//XYZ
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}


void	TPunchFileWriter::writeXYZVarCovarDeltaHeader()
{//X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz	

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();

	if(isSAPrioriUsed())
	{
		(*stream)<<"LES VARIANCES ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)"<<endl<<endl;
	}
	else
	{
		(*stream)<<"LES VARIANCES ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI"<<endl<<endl;
	}	


	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "VX ");
	(*stream).writeString(coordResWidth, "VY ");
	(*stream).writeString(coordResWidth, "VZ ");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "COVXY");
	(*stream).writeString(coordResWidth, "COVXZ");
	(*stream).writeString(coordResWidth, "COVYZ");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "DX ");
	(*stream).writeString(coordResWidth, "DY ");
	(*stream).writeString(coordResWidth, "DZ ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).writeString(coordResWidth, "(MM2)");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream)<<endl<<endl;
	return;
}


void	TPunchFileWriter::writeXYZErrorEllSigZDeltaHeader()
{//X Y Z gist_gd_axe Gd_axe Pt_axe Sz Cyz Dx Dy Dz	

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	int					obsWidth = max(getObsWidth(),11);

	if(isSAPrioriUsed())
	{
		(*stream)<<"LES ELLIPSES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)"<<endl<<endl;
	}
	else
	{
		(*stream)<<"LES ELLIPSES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI"<<endl<<endl;
	}



	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line	
	(*stream).writeString(nameWidth, "NOM  ");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(obsWidth, "GIS Gd_AXE");
	(*stream).writeString(11, "1/2 Gd_AXE");
	(*stream).writeString(11, "1/2 Pt_AXE");
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream).writeString(coordResWidth, "DX ");
	(*stream).writeString(coordResWidth, "DY ");
	(*stream).writeString(coordResWidth, "DZ ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(obsWidth, "(GON)");
	(*stream).writeString(11, "(MM)");
	(*stream).writeString(11, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream)<<endl;

	return;
}


void	TPunchFileWriter::writeXYHHeader()
{//XYH

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}


void	TPunchFileWriter::writeXYZHHeader()
{//XYZH

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}


void	TPunchFileWriter::writeXYHNHeader()
{//XYHN

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	/////////////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream)<<endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}


void	TPunchFileWriter::writeXYZHNHeader()
{
	TAStreamFormatter* stream =	getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	
	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream)<<endl;

	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream)<<endl<<endl;
	return;
}

void	TPunchFileWriter::writeXYZSigmaHeader()
{
	TAStreamFormatter* stream =	getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();

	//First line
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordResWidth, "SX ");
	(*stream).writeString(coordResWidth, "SY ");
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream)<<endl;

	//second line : units
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream)<<endl<<endl;
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//DATA
///////////////////////////////////////////////////////////////////////////////////////////////////////
void	TPunchFileWriter::writeXYZData(LSPosVecConstIter pt)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, getRefFrameForH());
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(pt, nameWidth);
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, pt);
	(*stream)<<endl;
	return;
}


void	TPunchFileWriter::writeXYHData(LSPosVecConstIter pt)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, getRefFrameForH());
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(pt, nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, pt);
	(*stream)<<endl;
	return;
}


void	TPunchFileWriter::writeXYZVarCovarDeltaData(LSPosVecConstIter posVecIterator)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, getRefFrameForH());
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	//Name
	converter.writeName(posVecIterator, nameWidth);

	//Coordinate
	converter.writeXYZ(coordWidth, getCoordPrecision(),TLength::kMetres, separator,posVecIterator);

	//Variance
	(*stream).width(1);
	(*stream)<<"";
	int  pre = getCoordPrecision();
	if(pre >=3)
	{pre = pre -3;}
	else
	{pre = 0;}
	converter.writeCoordinateParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								pre,
								separator,
								posVecIterator->getXVar(),
								posVecIterator->getYVar(),
								posVecIterator->getZVar(),
								"XXX");
	(*stream).width(1);
	(*stream)<<"";
	//Covariance
	converter.writeCoordinateParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								posVecIterator->getXYCovar(),
								posVecIterator->getYZCovar(),
								posVecIterator->getXZCovar(),
								"XXX");
	(*stream).width(1);
	(*stream)<<"";
	//Delta
	converter.writeCoordinateParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								posVecIterator->getDXValue(),
								posVecIterator->getDYValue(),
								posVecIterator->getDZValue(),
								"XXX");
	(*stream)<<endl;
	return;
}


void	TPunchFileWriter::writeXYZErrorEllSigZDeltaData(LSPosVecConstIter posVecIterator)
{	
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, getRefFrameForH());
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	int					obsWidth = max(getObsWidth(),11);
	string separator = stream->getSeparator();

	//Name
	converter.writeName(posVecIterator, nameWidth);

	//Coordinate
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, posVecIterator);

	//Error ellipse
	if( (posVecIterator->getXStatus() == TALSCalcParameter::kVariable) && (posVecIterator->getYStatus() == TALSCalcParameter::kVariable) )
	{
		writeAngle(obsWidth, getAngleResidualPrecision(), TAngle::kGons, posVecIterator->getErrorEllGis());
		(*stream)<<separator;
		writeLength(11, getLengthResidualPrecision(), TLength::kMillimetres, posVecIterator->getErrorEllMajorAxis());
		(*stream)<<separator;
		writeLength(11, getLengthResidualPrecision(), TLength::kMillimetres, posVecIterator->getErrorEllMinorAxis());
		(*stream)<<separator;
 	}
	else
	{
		(*stream).width(obsWidth);
		(*stream)<<"" << separator;
		(*stream).width(11);
		(*stream)<<"" << separator;
		(*stream).width(11);
		(*stream)<<"" << separator;
	}

	//sigma Z
	if(posVecIterator->getZStatus() == TALSCalcParameter::kVariable)
		writeLength(coordResWidth, getCoordPrecision(),  TLength::kMillimetres, posVecIterator->getZSigma());
	else
	{
		(*stream).width(coordResWidth);
		(*stream)<<"";
	}
	(*stream)<<separator;

	//delta
	
	converter.writeCoordinateParam(posVecIterator->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								posVecIterator->getDXValue(),
								posVecIterator->getDYValue(),
								posVecIterator->getDZValue(),
								"");

	(*stream)<<endl;
	return;
}


void	TPunchFileWriter::writeXYZHData(LSPosVecConstIter posVecIterator)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";

	(*stream).writeString(nameWidth, (posVecIterator->getName()));

	writeLength(coordWidth, getCoordPrecision(), TLength::kMetres , posVecIterator->getXEstValue());
	(*stream)<<separator;
	writeLength(coordWidth, getCoordPrecision(), TLength::kMetres ,posVecIterator->getYEstValue());
	(*stream)<<separator;
	writeLength(coordWidth, getCoordPrecision(), TLength::kMetres ,posVecIterator->getZEstValue());
	(*stream)<<separator;
	writeLength(coordWidth, getCoordPrecision(), TLength::kMetres ,posVecIterator->getHEstValue(getRefFrameForH()) );
	(*stream)<<separator;
	(*stream)<<endl;
	return;
}



void	TPunchFileWriter::writeXYHNData(LSPosVecConstIter pt)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, getRefFrameForH());
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(pt, nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, pt);
	converter.writeN( coordWidth, getCoordPrecision(), TLength::kMetres, pt);
	(*stream)<<endl;
	
	return;
}


void	TPunchFileWriter::writeXYZHNData(LSPosVecConstIter pt)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, getRefFrameForH());
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	converter.writeName(pt, nameWidth);
	converter.writeXYZandH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, pt);
	converter.writeN( coordWidth, getCoordPrecision(), TLength::kMetres, pt);
	(*stream)<<endl;

	return;
}


void	TPunchFileWriter::writeXYZSigmaData(LSPosVecConstIter pt)
{
	TAStreamFormatter* stream = getStream();
	TPointConverter converter (stream, getRefFrameForH());
	int					nameWidth = getNameWidth();
	int					coordWidth = getCoordWidth();
	int					coordResWidth = getCoordResWidth();
	string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream)<<"";
	//Name
	string name = pt->getName();
	//name = name.substr(0, 10);
	(*stream).writeString(nameWidth, name );
	//Coordinate
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, pt);

	//Sigma
	converter.writeCoordinateParam(pt->getGlobalStatus(),
								coordResWidth,
								getCoordPrecision(),
								TLength::kMillimetres,
								separator,
								pt->getXSigma(),
								pt->getYSigma(),
								pt->getZSigma(),
								"XXX");/*sigma*/

	(*stream)<<endl;

	return;
}




////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////
