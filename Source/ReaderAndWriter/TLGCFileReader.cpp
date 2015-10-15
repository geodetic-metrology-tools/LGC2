////////////////////////////////////////////////////////////////////////////////////
// TLGCFileReader.cpp :Implementation file
// File reader for an LGC input data file
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////




//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
/////////#include "QFile.h"
//////////#include "QString.h"
#include <list>
#include <locale>
#include <iostream>
//#include <cmath>
using namespace std;

#include "TLGCProject.h"
#include "TSeparatedFormatTStream.h"
#include "TAStreamFormatter.h"

#include  "TLGCFileReader.h"

#include "TOrientationCnstr.h"

#include  "THorAngleConverter.h"
#include  "TZenithDistConverter.h"
#include  "GyroscopeStationConverter.h"

#include  "TVertDistConverter.h"
#include  "LevelStationConverter.h"
#include  "EDMSpatialDistConverter.h"
#include  "THorDistConverter.h"
#include  "OffsetToVerticalPlaneConverter.h"
#include  "TRadialOffsetCnstrConverter.h"
#include  "TRelativeErrorConverter.h"
#include  "OffsetToVerticalLineConverter.h"
#include  "OffsetToSpatialLineConverter.h"
#include  "OffsetToVerticalLineROM.h"
#include  "OffsetToSpatialLineROM.h"

#include "InstrumentsConverter.h"
#include "TheodoliteStationConverter.h"

////////////////////////////////////////////////////////////////


//ClassImp(TLGCFileReader)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// constructor setting the link to the LGC project and to the file to be read
///////////////////////////////////////////////////////////////////////////////
TLGCFileReader::TLGCFileReader(TLGCProject* project)
{
	fRefFrameSet = false;
	fNumOfMeas = 0;
	fProject=project;
	fSeparatedStream = new TSeparatedFormatTStream(TAStreamFormatter::kRead, *(project->getDataSet()));
}


TLGCFileReader::~TLGCFileReader()
{
	if (fSeparatedStream != 0)
	{delete fSeparatedStream;}
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// reads the LGC file and does everything necessary according to what is read
/////////////////////////////////////////////////////////////////////////////
void TLGCFileReader::readFile()
{
	string streamError = fSeparatedStream->getError();
	
	if(streamError == "")
	{
		if(fProject != 0)
		{
			char firstChar;
			int i = 1;
			char lineNum[9];
			string keyword, error, iLine, line;

			fSeparatedStream->skipWhiteSpace();			

			// title 
			*fSeparatedStream >> keyword;
			if (keyword != "*TITR")
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error=error + line + "LGC Input file must start with ""*TITR"" keyword" + '\n';
				fProject->setError(error);
				fSeparatedStream->readLine();
				i++;
			}
			else
			{ 
				fSeparatedStream->skipWhiteSpace();
				if ((fSeparatedStream->peek()) !='\n')
				{
					_itoa(i, lineNum, 10);
					iLine = lineNum;
					line = "line number " + iLine + " : ";
					error=error + line + "Warning : Nothing expected after ""*TITR"" keyword" + '\n';
					fProject->setError(error);
				}
				i = readTitle(i);
			}

			i = readCommentLines(i);

			bool end = true;
			while (!(fSeparatedStream->atEndCom()) && end)
			{
				i = readCommentLines(i);

				fSeparatedStream->skipWhiteSpace();
				firstChar = fSeparatedStream->peek();
				char a = '*';
				if (firstChar == a)
				{
					*fSeparatedStream >> keyword;
					i = reactToKeyword(keyword, i);
					fSeparatedStream->skipWhiteSpace();
					if ( (keyword == "*END") || (keyword == "*FIN") ) 
					{
					/*	while (!(fSeparatedStream->atEnd()))
						{
							if (fSeparatedStream->peek() != '\n')
							{
								_itoa(i, lineNum, 10);
								iLine = lineNum;
								line = "line number " + iLine + " : ";
								error=error + line + "End of File Keyword appeared : nothing else expected" + '\n';
								fProject->setError(error);
							}
							fSeparatedStream->readLine();
							fSeparatedStream->skipWhiteSpace();
							i++;
						}*/
						end = false;
					}
					

				}
				else 
				{
					_itoa(i, lineNum, 10);
					iLine = lineNum;
					line = "line number " + iLine + " : ";
					error=fProject->getError() + line + "Keyword expected" + '\n';
					
					fProject->setError(error);
					fSeparatedStream->readLine();
					i++;
				}
				

			}
			
		}
	}

	else 
	{
		fProject->setError(streamError);
	}

	return;
}

//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////

int TLGCFileReader::reactToKeyword(const string keyW, int i)
{
	// reacts to read keywords	
	bool unknownKeyword = true;
	char lineNum[9];
	string iLine, line, error;

	// calc params and output options keywords
	if (keyW == "*BD"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		fProject->getOutputOptions()->setUsingGEODEOption();
		TSpatialPointName::setNamingConvention(TSpatialPointName::kDBName);
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setPointNameWidth(13);
		fProject->getDataSet()->setDataParams(data);
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*MICR"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setCoordPrecision(TPointFormat::kMicrometre);
		data.setLengthPrecision(TObservationFormat::kMicrometre);
		data.setAnglePrecision(TObservationFormat::k10Microgons);
		fProject->getDataSet()->setDataParams(data);
		fProject->getCalcParams()->setConvCriteria(LITERAL(0.0000005));
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*CLIC"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setCoordPrecision(TPointFormat::kMicrometre);
		data.setLengthPrecision(TObservationFormat::k10Micrometres);
		data.setAnglePrecision(TObservationFormat::k10Microgons);
		fProject->getDataSet()->setDataParams(data);
		fProject->getCalcParams()->setConvCriteria(LITERAL(0.0000005));
		unknownKeyword = false;
		return ++i;
	}
	
	else if (keyW == "*PREC"){
		int prec;
		fSeparatedStream->skipWhiteSpace();
		fSeparatedStream->TAStreamFormatter::operator>>(prec);
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setCoordPrecision(getCoordPrecisionFromInt(prec));
		data.setLengthPrecision(getLengthPrecisionFromInt(prec));
		data.setAnglePrecision(getAnglePrecisionFromInt(prec));

		fProject->getDataSet()->setDataParams(data);
		fProject->getCalcParams()->setConvCriteria(5*powq(LITERAL(0.1),prec+1));
		endOfLineError(i);
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}
	
	else if (keyW == "*FMTO") {
		// Deal with setting the main output format
		string format;
		string seperator;

		TLGCDataSet* data = fProject->getDataSet();
		fSeparatedStream->skipWhiteSpace();
		fSeparatedStream->TAStreamFormatter::operator>>(format);

		if (format == "SEP")
		{
			data->setResultsFileFormat(TAStreamFormatter::kSeparatorFormat);
			fSeparatedStream->skipWhiteSpace();
			fSeparatedStream->TAStreamFormatter::operator>>(seperator);
			seperator.erase(0,1);
			seperator.erase((seperator.length()-1),1);
			data->setResultsSeparator(seperator);
			
		}
		else if (format == "COL")
		{
			data->setResultsFileFormat(TAStreamFormatter::kColumnFormat);
		}

		data->setFormatType(format);
		endOfLineError(i);
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*FMTP") {
		// Deal with setting the punch files output format
		string seperator;
		string format;
		TLGCDataSet* data = fProject->getDataSet();
		fSeparatedStream->skipWhiteSpace();
		fSeparatedStream->TAStreamFormatter::operator>>(format);

		if (format == "SEP")
		{
			data->setPunchFileFormat(TAStreamFormatter::kSeparatorFormat);
			fSeparatedStream->skipWhiteSpace();
			fSeparatedStream->TAStreamFormatter::operator>>(seperator);
			seperator.erase(0,1);
			seperator.erase((seperator.length()-1),1);
			data->setPunchSeparator(seperator);
		}
		else if (format == "COL")
		{
			data->setPunchFileFormat(TAStreamFormatter::kColumnFormat);
		}
		data->setFormatType(format);
		endOfLineError(i);
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*NOSPC") {
		TLGCDataSet* data = fProject->getDataSet();
		data->setNoSpaceBetweenData();
		endOfLineError(i);
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*DIXI"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setCoordPrecision(TPointFormat::k100Micrometres);
		data.setLengthPrecision(TObservationFormat::k10Micrometres);
		data.setAnglePrecision(TObservationFormat::k10Microgons);
		fProject->getDataSet()->setDataParams(data);
		fProject->getCalcParams()->setConvCriteria(LITERAL(0.00005));
		unknownKeyword = false;
		return ++i;
	}

	
	else if (keyW == "*APRI"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		fProject->getCalcParams()->setUsingSAPriori();
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*TOL"){
		TReal cc;
		fSeparatedStream->skipWhiteSpace();
		fSeparatedStream->TAStreamFormatter::operator>>( cc );
		fProject->getCalcParams()->setConvCriteria(cc);
		endOfLineError(i);
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*OLOC"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setRefFrame(TRefSystemFactory::kLocalRefFrame);
		fProject->getDataSet()->setDataParams(data);
		refSysError(i);
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*LEP"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		fProject->getCalcParams()->setRefSurface(TRefSystemFactory::kCernXYHg85Machine);
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setRefFrame(TRefSystemFactory::kCernXYHg85Machine);
		data.setCoordSys(TCoordSysFactory::k2DPlusH);
		fProject->getDataSet()->setDataParams(data);
		fSeparatedStream->setCoordSys(TCoordSysFactory::k2DPlusH);
		refSysError(i);
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*SPHE"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		fProject->getCalcParams()->setRefSurface(TRefSystemFactory::kCERNXYHsSphereSPS);
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setRefFrame(TRefSystemFactory::kCERNXYHsSphereSPS);
		data.setCoordSys(TCoordSysFactory::k2DPlusH);
		fProject->getDataSet()->setDataParams(data);
		fSeparatedStream->setCoordSys(TCoordSysFactory::k2DPlusH);
		refSysError(i);
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*RS2K"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		fProject->getCalcParams()->setRefSurface(TRefSystemFactory::kCernXYHg00Machine);
		TDataParameters data = fProject->getDataSet()->getDataParams();
		data.setRefFrame(TRefSystemFactory::kCernXYHg00Machine);
		data.setCoordSys(TCoordSysFactory::k2DPlusH);
		fProject->getDataSet()->setDataParams(data);
		fSeparatedStream->setCoordSys(TCoordSysFactory::k2DPlusH);
		refSysError(i);
		unknownKeyword = false;
		return ++i;
	}

	// simulation options
	else if (keyW == "*SIMU"){
		int numOfSim;
		fSeparatedStream->skipWhiteSpace();
		if(fSeparatedStream->peek() == '\n')
		{
			_itoa(i, lineNum, 10);
			iLine = lineNum;
			line = "line number " + iLine + " : ";
			error = fProject->getError();
			error=error + line + """*SIMU"" must have a number of simulations" + '\n';
			fProject->setError(error);
		}
		*fSeparatedStream >> numOfSim;
		fProject->getCalcParams()->setNumOfSim(numOfSim);
		string simOption("NOBS");
		if (fSeparatedStream->peek() != '\n')
		{			
			*fSeparatedStream >> simOption;
			endOfLineError(i);
		}
		fSeparatedStream->readLine();
		fProject->setSimulation();
		if (simOption == "NOBS")
		{
			fSeparatedStream->setNoObsToRead();
		}
		unknownKeyword = false;
		return ++i;
	}
	else if (keyW == "*ALLFIXED")
	{
		fSeparatedStream->skipWhiteSpace();
        fProject->getCalcParams()->setAllPointsFixed();
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}
	else if (keyW == "*DEFA")
	{
		fSeparatedStream->skipWhiteSpace();
        fProject->getOutputOptions()->setDeformationAnalysisOption();
		fSeparatedStream->readLine();
		string title;
		*fSeparatedStream >> title;
		if (title.length() > 8)
		{
			_itoa(i, lineNum, 10);
			iLine = lineNum;
			line = "line number " + iLine + " : ";
			error = fProject->getError();
			error=error + line + """*DEFA"": Title must be no more than 8 characters in length\n";
			fProject->setError(error);
		}
		fProject->getOutputOptions()->setDeformationAnalysisTitle(title);
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}
	else if (keyW == "*NODUP"){
		endOfLineError(i);
		fSeparatedStream->readLine();
		fProject->getDataSet()->setNoDuplicates(true);
		unknownKeyword = false;
		return ++i;
	}
	
	
	else if (keyW == "*SOBS"){
		fProject->getOutputOptions()->simInputToCreate();
		fSeparatedStream->skipWhiteSpace();
		string simOption("NOBS");
		if (fSeparatedStream->peek() != '\n')
		{
			*fSeparatedStream >> simOption;
			endOfLineError(i);
		}
		fSeparatedStream->readLine();
		if( fProject->isSimulation() != true )
		{
			fProject->setSimulation();
			fProject->getCalcParams()->setNumOfSim(1);
		}
		if (simOption == "NOBS")
		{
			fSeparatedStream->setNoObsToRead();
		}
		unknownKeyword = false;
		return ++i;
	}

	// fault detection option
	else if (keyW == "*FAUT") {
		fProject->getCalcParams()->faultDetectToCreate();
		fSeparatedStream->skipWhiteSpace();
		if (fSeparatedStream->peek() != '\n') {
			TReal alpha, beta;
			*fSeparatedStream >> alpha;
			if ((alpha > 1) || (alpha < 0)) {
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error = fProject->getError();
				error=error + line + """*FAUT"": alpha must be included between 0 and 1" + '\n';
				fProject->setError(error);
			}
			else {
				fProject->getCalcParams()->setAlpha(alpha);
				fSeparatedStream->skipWhiteSpace();
				if (fSeparatedStream->peek() != '\n') {
					*fSeparatedStream >> beta;
					if ((beta > 1) || (beta < 0)) {
						_itoa(i, lineNum, 10);
						iLine = lineNum;
						line = "line number " + iLine + " : ";
						error = fProject->getError();
						error=error + line + """*FAUT"": beta must be included between 0 and 1" + '\n';
						fProject->setError(error);
					}
					else
						fProject->getCalcParams()->setBeta(beta);
				}
			}
		}
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return ++i;
	}

	
	
	//points keywords
	else if (keyW == "*CALA"){
		i = readPoints(TSpatialStatus::kCala, i);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*POIN" || keyW == "*VXYZ"){
		i = readPoints(TSpatialStatus::kVxyz, i);
		unknownKeyword = false;
		return i;
	}


	else if (keyW == "*VXY"){
		i = readPoints(TSpatialStatus::kVxy, i);
		unknownKeyword = false;
		return i;
	}


	else if (keyW == "*VXZ"){
		i = readPoints(TSpatialStatus::kVxz, i);		
		unknownKeyword = false;
		return i;
	}


	else if (keyW == "*VYZ"){
		i = readPoints(TSpatialStatus::kVyz, i);		
		unknownKeyword = false;
		return i;
	}


	else if (keyW == "*VX"){
		i = readPoints(TSpatialStatus::kVx, i);	
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*VY"){
		i = readPoints(TSpatialStatus::kVy, i);	
		unknownKeyword = false;
		return i;
	}


	else if (keyW == "*VZ"){
		i = readPoints(TSpatialStatus::kVz, i);	
		unknownKeyword = false;
		return i;
	}

	// TODO: check for incrementing line numbers after each read line - EVERYWHERE
	else if (keyW == "*INSTR")
	{
		fSeparatedStream->readLine();		
		InstrumentsConverter conv(*fSeparatedStream);
		conv.readInstruments(++i, fProject);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*TSTN")
	{
		TheodoliteStationConverter conv(*fSeparatedStream);
		conv.readTheodoliteStation(i, fProject);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*DSPT")
	{
		EDMSpatialDistConverter conv(*fSeparatedStream);
		conv.readObservations(i, fProject);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*ECVE")
	{
		OffsetToVerticalLineConverter conv(*fSeparatedStream);
		OffsetToVerticalLineROM* rom = conv.readObservations(i, fProject);

		//fProject->getDataSet()->getWorkingROMs()->addOffsetToVerticalLineROM(rom);
		//fProject->getDataSet()->addToOffsetToVerLineNum(rom->getMeasurements().size());
		throw std::logic_error("CANOT READ ECVE AT THE MOMENT");

		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*ECHO")
	{
		OffsetToVerticalPlaneConverter conv(*fSeparatedStream);
		OffsetToLineOrPlaneROM* rom = conv.readObservations(i, fProject);
		fProject->getDataSet()->getWorkingROMs()->addOffsetToVerticalPlaneROM(rom);

		fProject->getDataSet()->addToOffsetToVerPlaneNum(rom->getMeasurements().size());
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*ECSP")
	{
		OffsetToSpatialLineConverter conv(*fSeparatedStream);
		OffsetToSpatialLineROM* rom = conv.readObservations(i, fProject);

		//fProject->getDataSet()->getWorkingROMs()->addOffsetToSpatialLineROM(rom);
		//fProject->getDataSet()->addToOffsetToSpaLineNum(rom->getMeasurements().size());
		throw std::logic_error("CANOT READ ECSP AT THE MOMENT");

		unknownKeyword = false;
		return i;
	}
	
	else if (keyW == "*ORIE"){
		//check if there's NIV0
		fSeparatedStream->skipWhiteSpace();
		string niv0 = "";
		if (!fSeparatedStream->atEnd() && fSeparatedStream->peek() != '\n' && fSeparatedStream->peek() == 'N')
		{*fSeparatedStream >> niv0;}
		
		if(niv0=="")
		{
		//	i = readGyroOrieMeas( i);
			GyroscopeStationConverter reader (*fSeparatedStream);
			reader.readObservations(i, fProject);

			unknownKeyword = false;
		}
		else if(niv0 == "NIV0") //pas disponible pour l instant 13/08/2003
		{
			_itoa(i, lineNum, 10);
			iLine = lineNum;
			line = "line number " + iLine + " : ";
			error=fProject->getError() + line + "ORIE NIV0 is not available on this version" + '\n';
			fProject->setError(error);
			fSeparatedStream->readLine(); //detruit la ligne
			while (((fSeparatedStream->peek()) != '*') && ((fSeparatedStream->peek()) != EOF))
			{fSeparatedStream->readChar();}//detruit les obs de ORIE
			unknownKeyword = false;
		}	
	
		return i;
	}

	if (keyW == "*DVER")
	{		
		TVertDistConverter conv(*fSeparatedStream);
		VerticalDistanceROM* rom = conv.readObservations(i, fProject);

		fProject->getDataSet()->getWorkingROMs()->addVerticalDistanceROM(rom);
		
		fProject->getDataSet()->addToVertDistNum(rom->getMeasurements().size());

		unknownKeyword = false;
		return i;
	}

	if (keyW == "*DLEV")
	{		
		LevelStationConverter conv(*fSeparatedStream);
		conv.readObservations(i, fProject);

		unknownKeyword = false;
		return i;
	}

	if (keyW == "*RADI")
	{
	//	i = readRadOffCnstr( i);
		TRadialOffsetCnstrConverter reader (*fSeparatedStream);
		i = reader.readObservation(i, fProject);
		unknownKeyword = false;
		return i;
	}

	if (keyW == "*EREL")
	{
		// react to key word EREL
		//i = readPointNamePairs(i);
		TRelativeErrorConverter reader (*fSeparatedStream);
		i = reader.readObservation(i, fProject);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*PDOR")
	{
		//check if there's NIV0
		fSeparatedStream->skipWhiteSpace();
		string niv0 = "";
		if (!fSeparatedStream->atEnd() && fSeparatedStream->peek() != '\n')
		{*fSeparatedStream >> niv0;}

		if(niv0=="")
		{
			i = readOrieCnstr( i);
			unknownKeyword = false;
		}
		else
		{
			if(niv0 == "NIV0") //pas disponible pour l instant 13/08/2003
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error=fProject->getError() + line + "PDOR NIV0 is not available on this version" + '\n';
				fProject->setError(error);
				fSeparatedStream->readLine(); //detruit la ligne
				while (((fSeparatedStream->peek()) != '*') && ((fSeparatedStream->peek()) != EOF))
				{fSeparatedStream->readChar();}//detruit les obs de PDOR
				unknownKeyword = false;
			}
			else
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error=fProject->getError() + line + "unknown keyword after PDOR" + '\n';
				fProject->setError(error);
				fSeparatedStream->readLine(); //detruit la ligne
				while (((fSeparatedStream->peek()) != '*') && ((fSeparatedStream->peek()) != EOF))
				{fSeparatedStream->readChar();}//detruit les obs de PDOR
				unknownKeyword = false;
			}
		}

		return i;
	}

	else if (keyW == "*PROB"){
		i = readObsPrec( i);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*PRCO"){
		i = readCoordPrec( i);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*LIBR"){
		/*react to key world LIBR*/
		i++;
		fProject->getCalcParams()->librIsDetected();
		fSeparatedStream->readLine();
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*PUNC"){
		/*react to key world PUNC*/
		i = readPunchFileType( i);
		unknownKeyword = false;
		return i;
	}

	else if (keyW == "*HIST"){
		/*react to key world HIST*/
		endOfLineError(i);
		fSeparatedStream->readLine();
		fProject->getOutputOptions()->usedHistOption();
		unknownKeyword = false;
		return ++i;
	}

	else if (keyW == "*END" || keyW == "*FIN"){
		unknownKeyword = false;
		fSeparatedStream->readLine();
		i++;
		fSeparatedStream->skipWhiteSpace();
		return i;
	}

	if (unknownKeyword)
	{
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		error = fProject->getError();
		error=error+ line + "Unknown Keyword" + '\n';
		fProject->setError(error);
		fSeparatedStream->readLine();
		i++;
	}


	return i;

}



void TLGCFileReader::endOfLineError(int i)
{
	char lineNum[9];
	string iLine, line, error;

	fSeparatedStream->skipWhiteSpace();
	if ((fSeparatedStream->peek()) !='\n')
	{
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		error = fProject->getError();
		error=error + line + "Warning : Too much element on line" + '\n';
		fProject->setError(error);
	}
	
}


void TLGCFileReader::refSysError(int i)
{
	char lineNum[9];
	string iLine, line, error;

	if ( (fProject->getDataSet()->getDataParams().getRefFrameEnumerator() == TRefSystemFactory::kLocalRefFrame 
		&& (fProject->getCalcParams()->getRefSurface() == TRefSystemFactory::kCERNXYHsSphereSPS
		|| fProject->getCalcParams()->getRefSurface() == TRefSystemFactory::kCernXYHg85Machine))
		|| (fProject->getDataSet()->getDataParams().getRefFrameEnumerator() == TRefSystemFactory::kCCS 
		&& fProject->getCalcParams()->getRefSurface() == TRefSystemFactory::kNotInGraph) )
	{
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		error = fProject->getError();
		error=error + line + "Use of incompatible keyword" + '\n';
		fProject->setError(error);
	}
	
}



int TLGCFileReader::readPoints(TSpatialStatus::ESpatialStatus status, int i)
{//reads a series of points

	if (!fRefFrameSet)
	{
		fSeparatedStream->setReferenceFrame(fProject->getDataSet()->getDataParams().getRefFrame());
		fRefFrameSet = true;
	}

	char lineNum[9];
	string iLine, line, error;

	TSpatialPoint* newPoint = new TSpatialPoint();
	string headCommentLine = "";

	endOfLineError(i);
	fSeparatedStream->readLine();
	fSeparatedStream->skipWhiteSpace();
	i++;
	fSeparatedStream->setLengthUnits(TLength::kMetres);
						
	// keyword line does not exist any more : next is point or comment, until the next '*'
	while ((fSeparatedStream->peek()) !='*')
	{
		// check if there is a comment line
		if(fSeparatedStream->peek()=='%')
		{
			// read '%' to delete it
			fSeparatedStream->readChar();
			// comment line
			while(!(fSeparatedStream->peek()=='\n'))
			{
				string com;
				fSeparatedStream->TAStreamFormatter::operator>>(com);
				com = com + " ";
				headCommentLine += com;
				fSeparatedStream->skipWhiteSpace();
			}
		}

		else
		{
			// assign the comment to the point on the next line
			newPoint->setHeaderComment(headCommentLine);
			// "clear" the comment line storage
			headCommentLine="";
			// reading of a point
			fSeparatedStream->skipWhiteSpace();
			*fSeparatedStream >> *newPoint;
			fSeparatedStream->skipWhiteSpace();
			char C = fSeparatedStream->peek();
			if (fProject->getCalcParams()->areAllPointsFixed())
			{
				status = TSpatialStatus::kCala;
			}
			(newPoint->getPosition()).setObjectStatus(status);
			bool insert = fProject->getDataSet()->getWorkingPoints()->addPoint(newPoint);
			if (insert)
			{
				fProject->getDataSet()->addToPointNum(status);
			}

			if (newPoint->getName().getNamingConvention() == TSpatialPointName::kName)
			{
				TDataParameters data = fProject->getDataSet()->getDataParams();
				int newPointSize = newPoint->getName().getName().size();
				if (newPointSize > data.getPointNameWidth())
				{
					data.setPointNameWidth(newPoint->getName().getName().size());
					fProject->getDataSet()->setDataParams(data);
				}
			}

			if (fSeparatedStream->getError() != "")
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error = fProject->getError();
				error = error + line + fSeparatedStream->getError() + '\n';
				fProject->setError(error);
				fSeparatedStream->setError("");
			}

			if (fSeparatedStream->getWarning() != "")
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				string warning = fProject->getWarning();
				warning = line + fSeparatedStream->getWarning() + '\n';
				fProject->addWarning(warning);
				fSeparatedStream->initWarning();
			}

			if (C != '\n' && C != EOF)
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error = fProject->getError();
				error=error+ line + "Too much information to read" + '\n';
				fProject->setError(error);			
			}

		}

		// to go to the next line to read
		fSeparatedStream->readLine();
		fSeparatedStream->skipWhiteSpace();
		i++;

	}

	delete newPoint;
	return i;
}



int TLGCFileReader::readTitle(int i)
{/*read the title */

	fSeparatedStream->readLine();
	fSeparatedStream->skipWhiteSpace();
	i++;

	string title = "";
	
	while (fSeparatedStream->peek() != '*')
	{
		string str =fSeparatedStream->readLine();
		title = title + str + '\n';
		fSeparatedStream->skipWhiteSpace();
		i++;
	}
	if (title == "")
	{
		title="No Title";
	}

	fProject->setTitle(title);

	return i;


}

/*read the Coordinate Precision */
int TLGCFileReader::readCoordPrec(int i)
{
	i++;
	int coordPrec = 0;

	//read coord precision
	fSeparatedStream->skipWhiteSpace();
	if (!fSeparatedStream->atEnd() && fSeparatedStream->peek() != '\n')
	{*fSeparatedStream >> coordPrec;}
	else	{coordPrec = 6;}

	TDataParameters data = fProject->getDataSet()->getDataParams();
	data.setCoordPrecision(getCoordPrecisionFromInt(coordPrec));
	fProject->getDataSet()->setDataParams(data);

	fSeparatedStream->readLine();
	return i;
}


int TLGCFileReader::readObsPrec(int i)
{/*read the observation precision */

	i++;
	int lenPrec = 0;
	int angPrec = 0;

	//read coord precision
	fSeparatedStream->skipWhiteSpace();
	if (!fSeparatedStream->atEnd() && fSeparatedStream->peek() != '\n')
	{*fSeparatedStream >> lenPrec;}
	else	{lenPrec = 6;}

	fSeparatedStream->skipWhiteSpace();
	if (!fSeparatedStream->atEnd() && fSeparatedStream->peek() != '\n')
	{*fSeparatedStream >> angPrec;}
	else	{angPrec = 6;}

	TDataParameters data = fProject->getDataSet()->getDataParams();
	data.setLengthPrecision(getLengthPrecisionFromInt(lenPrec));
	data.setAnglePrecision(getAnglePrecisionFromInt(angPrec));
	fProject->getDataSet()->setDataParams(data);

	fSeparatedStream->readLine();
	return i;
}


int	TLGCFileReader::readPunchFileType(int i)
{
	i++;

	
	string ops = "";
	TLGCOutputOptions::EPunc option = TLGCOutputOptions::kDefault;
	fSeparatedStream->skipWhiteSpace();

	while (!fSeparatedStream->atEnd() && fSeparatedStream->peek() != '\n')
	{// si il y a plusieurs options PUNCH, seul la derniere sera retenue.
		fSeparatedStream->skipWhiteSpace();
		*fSeparatedStream >> ops;

		if(option == TLGCOutputOptions::kDefault)
		{//option *PUNC MES ou *PUNC COO ou combinaison des deux
			option = TLGCOutputOptions::kNo;
		}
		//else : cas *PUNC T MES par ex

		if(ops=="E")
		{
			option = TLGCOutputOptions::kE;
			fProject->getCalcParams()->setIsPunchEOrEE();
		}

		else if(ops=="EE")
		{
			option = TLGCOutputOptions::kEE;
			fProject->getCalcParams()->setIsPunchEOrEE();
		}

		else if(ops=="H")
		{option = TLGCOutputOptions::kH;}

		else if(ops=="Z")
		{option = TLGCOutputOptions::kZ;}

		else if(ops=="ZH" || ops == "HZ")
		{option = TLGCOutputOptions::kZH;}

		else if(ops=="HN")
		{option = TLGCOutputOptions::kHN;}

		else if(ops=="ZHN")
		{option = TLGCOutputOptions::kZHN;}

		else if(ops=="T")
		{option = TLGCOutputOptions::kT;}

		else if(ops=="OUT1" || ops=="COO")
		{
			fProject->getOutputOptions()->setCooOption();
		}

		else if(ops=="OUT3" || ops=="MES")
		{
			fProject->getOutputOptions()->setMesOption();
		}
	
	}

	if(fProject->getOutputOptions()->isPunchfileToBeSaved() == TLGCOutputOptions::kNo)
	{
		fProject->getOutputOptions()->setPuncOption(option);
	}
	fSeparatedStream->readLine();
	return i;
}









////////////////////////////////////////////////////////////
// reads an orientation constraints
////////////////////////////////////////////////////////////
int TLGCFileReader::readOrieCnstr(int i)
{
	char lineNum[9];
	string iLine, line, error;
			
	string headCommentLine = "";
	
	//read keyworld's line 
	fSeparatedStream->readLine();
	fSeparatedStream->skipWhiteSpace();
	i++;

	//read the orientation Cnstr
	while ((fSeparatedStream->peek()) != '*')
	{
		fSeparatedStream->skipWhiteSpace();
		if(fSeparatedStream->peek()=='%')
		{
			fSeparatedStream->readChar(); //read the char %

			string comment = "";
			while(fSeparatedStream->peek() != EOF && fSeparatedStream->peek() != '\n')
			{
				fSeparatedStream->TAStreamFormatter::operator>>(comment);
				headCommentLine += comment+ " ";
				fSeparatedStream->skipWhiteSpace();
			}	
		}
		else
		{
			//read the constraint
			TSpatialPointName ptName;
			TAngle cBearing(LITERAL(0.0));


			// read the Point
			(*fSeparatedStream)>>(ptName);
			fSeparatedStream->skipWhiteSpace();
			//read the bearing
			(*fSeparatedStream)>>(cBearing);
			if(fSeparatedStream->getError()=="No angle value to read")
			{
				cBearing.setRadiansValue(LITERAL(0.0));
				string null = "";
				fSeparatedStream->setError(null);
				fProject->getCalcParams()->pdorBearIsNotDefined();
			}

			fSeparatedStream->skipWhiteSpace();

			//create the radial offset constraint
			TOrientationCnstr* newCnstr = new TOrientationCnstr(fProject->getDataSet()->getObservationsCount()++, ptName, cBearing);

			// assign the comment to the point on the next line
			newCnstr->setHeaderComment(headCommentLine);
			// "clear" the comment line storage
			headCommentLine="";
			
			fSeparatedStream->skipWhiteSpace();

			// add to fError errors from the read of measurement
			if (fSeparatedStream->getError() != "")
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				error = fProject->getError();
				error=error+ line + fSeparatedStream->getError() + '\n';
				fProject->setError(error);
				fSeparatedStream->setError("");
			}

	
			//read the comment, id. and constant
			string EOLComments="";
			string comment = "";

			while (!(fSeparatedStream->peek()=='\n'))
			{
				switch (fSeparatedStream->peek())
				{

					case '%':
						fSeparatedStream->readChar(); //read the char %

						
						while(fSeparatedStream->peek() != EOF && fSeparatedStream->peek() != '\n')
						{
							fSeparatedStream->TAStreamFormatter::operator>>(comment);
							EOLComments += comment+ " ";
							fSeparatedStream->skipWhiteSpace();
						}	
						
						break;

					case '#':
						fSeparatedStream->readChar(); //read the char #

						
						while(fSeparatedStream->peek() != EOF && fSeparatedStream->peek() != '\n')
						{
							fSeparatedStream->TAStreamFormatter::operator>>(comment);
							EOLComments += comment+ " ";
							fSeparatedStream->skipWhiteSpace();
						}	
						
						break;

					default:
						// other characters to read on this line correspond to an error
						_itoa(i, lineNum, 10);
						iLine = lineNum;
						line = "line number " + iLine + " : ";
						error = fProject->getError();
						error=error+line+ "Too much informations to read" + '\n';
						fProject->setError(error);
						while (!(fSeparatedStream->peek()=='\n')){
							fSeparatedStream->readChar();}
						break;
				}
				fSeparatedStream->skipWhiteSpace();
			}

			fProject->getDataSet()->getWorkingConstraints()->addOrieCnstr(newCnstr);
			fProject->getDataSet()->addToOrieCnstrNum();

			delete newCnstr;
		}
		fSeparatedStream->readLine();
		fSeparatedStream->skipWhiteSpace();
		i++;
	}

	
	return i;
}








////////////////////////////////////////////////////////////////////////
//CONVERT METHODS
////////////////////////////////////////////////////////////////////////
TPointFormat::ECoordPrecision	TLGCFileReader::getCoordPrecisionFromInt(const int i)
{//!convert integer to Coordinate Precision
	switch (i)
	{
		case 0:
			return TPointFormat::kMetre;
			break;

		case 1:
			return TPointFormat::k100Millimetres;
			break;

		case 2:
			return TPointFormat::k10Millimetres;
			break;

		case 3:
			return TPointFormat::kMillimetre;
			break;

		case 4:
			return TPointFormat::k100Micrometres;
			break;

		case 5:
			return TPointFormat::k10Micrometres;
			break;

		case 6:
			return TPointFormat::kMicrometre;
			break;

		case 7:
			return TPointFormat::k100Nanometres;
			break;


		default:
			return TPointFormat::k10Micrometres;
			break;
	}
}



TObservationFormat::ELengthPrecision	TLGCFileReader::getLengthPrecisionFromInt(const int i)
{//!convert integer to length observation Precision
	switch (i)
	{
		case 0:
			return TObservationFormat::kMetre;
			break;

		case 1:
			return TObservationFormat::k100Millimetres;
			break;

		case 2:
			return TObservationFormat::k10Millimetres;
			break;

		case 3:
			return TObservationFormat::kMillimetre;
			break;

		case 4:
			return TObservationFormat::k100Micrometres;
			break;

		case 5:
			return TObservationFormat::k10Micrometres;
			break;

		case 6:
			return TObservationFormat::kMicrometre;
			break;

		case 7:
			return TObservationFormat::k100Nanometres;
			break;


		default:
			return TObservationFormat::k10Micrometres;
			break;
	}
}




TObservationFormat::EAnglePrecision	TLGCFileReader::getAnglePrecisionFromInt(const int i)
{//!convert integer to angle observation Precision
	switch (i)
	{
		case 0:
		case 1:
		case 2:
			return TObservationFormat::k10Milligons;
			break;

		case 3:
		case 4:
			return TObservationFormat::k100Microgons;
			break;

		case 5:
			return TObservationFormat::k10Microgons;
			break;
		case 6:
			return TObservationFormat::kMicrogon;
			break;
		case 7:
			return TObservationFormat::k100Nanogons;
			break;

		default:
			return TObservationFormat::k10Microgons;
			break;
	}
}


int  TLGCFileReader::readCommentLines(int i)
{
	fSeparatedStream->skipWhiteSpace();
	while( '%' == fSeparatedStream->peek() ) // % character indicates a comment line
	{
		// read the comment line and skip any more white space
		fSeparatedStream->readLine();
		fSeparatedStream->skipWhiteSpace();
		i++;
	}
	return i;
}


/////////////////////////////////////////////////////////////////////////
//END
//////////////////////////////////////////////////////////////////////////
