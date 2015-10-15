////////////////////////////////////////////////////////////////////
// TSimFileWriter.cpp : implementation class
// Write an LGC "input" file with simulated values for observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TSeparatedFormatTStream.h"
#include "TLGCDataSet.h"
#include "LSCalcDataSet.h"
#include "TSimFileWriter.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TSimFileWriter::TSimFileWriter() : TAFileWriter()
{//default constructor
}


TSimFileWriter::TSimFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TAFileWriter(stream, project)
{//constructor
}


TSimFileWriter::~TSimFileWriter()
{//destructor
}

//////////////////////////
// public member functions
//////////////////////////

//////////////////////////////////////////////////////////////////////
//write the sim obs lgc file used when there's no error in the project
//////////////////////////////////////////////////////////////////////
void	TSimFileWriter::writeSimulatedObsFile(TLGCDataSet* dataSet, TFileParameters input, LSCalcDataSet* calcDS)
{
	bool simulationMessage = true;
	string error = "";
	init(dataSet, calcDS, error);

	TAStreamFormatter* stream = getStream();/*stream is define in the init method*/

	TLGCDataSet inpDS;
	inpDS.setFileParams(input);

	TSeparatedFormatTStream* inputFile = new TSeparatedFormatTStream(TAStreamFormatter::kRead, inpDS);
	int obsID = 0;

	string station;

	while(!(inputFile->atEnd()))
	{
		char C = inputFile->peek();
		if(C == '*')
		{
			// TODO: fix all missing keywords
			string keyW;
			inputFile->TAStreamFormatter::operator>>(keyW);
			if(keyW == "*SOBS")
			{
				inputFile->skipWhiteSpace();
				string simOption("NOBS");
				if (inputFile->peek() != '\n')
				{
					inputFile->TAStreamFormatter::operator>>(simOption);
				}
				if (simOption == "NOBS")
				{
					stream->setNoObsToRead();
				}
				inputFile->readLine();
			}
			else if (keyW == "*TSTN")
			{
				string read = inputFile->readLine();
				int i = read.find_first_of(' ');
				char* temp = new char[i + 1];
				sscanf(read.c_str(), "%s", temp);
				station = temp;
				delete[] temp;
				(*stream)<<(keyW + read)<<endl;
			}
			else if(keyW == "*DRDL")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimSpaDist(inputFile, obsID, calcDS);
			}
			else if(keyW == "*DSPT")
			{
				string read = inputFile->readLine();
				int i = read.find_first_of(' ');
				char* temp = new char[i + 1];
				sscanf(read.c_str(), "%s", temp);
				station = temp;
				delete[] temp;
				writeSimEDMSpaDist(inputFile, obsID, calcDS);
			}
			else if(keyW == "*PLR")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimPolar(inputFile, obsID, calcDS);
			}
			else if(keyW == "*ANGL")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimHorAng(inputFile, obsID, calcDS);
			}
			else if(keyW == "*ZEND")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimZenDist(inputFile, obsID, calcDS);
			}
			else if(keyW == "*DVER")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimVertDist(inputFile, obsID, calcDS);
			}
			else if(keyW == "*DHOR")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimHorDist(inputFile, obsID, calcDS);
			}
			else if(keyW == "*ECVE")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToVerLine(inputFile, obsID, calcDS);
			}
			else if(keyW == "*ECSP")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToSpaLine(inputFile, obsID, calcDS);
			}
			else if(keyW == "*ECHO")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToVerPlane(inputFile, obsID, calcDS);
			}
			else if(keyW == "*ECTH")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToTheoPlane(inputFile, obsID, calcDS);
			}
			else if(keyW == "*ORIE")
			{
				string read = inputFile->readLine();
				int i = read.find_first_of(' ');
				char* temp = new char[i + 1];
				sscanf(read.c_str(), "%s", temp);
				station = temp;
				delete[] temp;
				writeSimGyroOrie(inputFile, obsID, calcDS);
			}
			else if(keyW == "*TITR")
			{
				string line = keyW+ '\n';
				if(simulationMessage)
				{
					line = line + "DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !";
					simulationMessage = false;
				}
				line = line + inputFile->readLine();
				(*stream)<<(line)<<endl;
			}
			else/*title and option*/
			{
				string line = keyW + inputFile->readLine();
				(*stream)<<(line)<<endl;
			}
		}
		else
		{
			string line = inputFile->readLine();
			(*stream)<<line<<endl;
		}
	}
	
	
	return;
}


////////////////////////////////////////////////////
// write spatial distance simulated observation line
////////////////////////////////////////////////////
void	TSimFileWriter::writeSimSpaDist(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSSpaDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSSpaDist();
			iterEnd = calcDS->endLSSpaDist();

			obs = dataSet->getCalcSpaDistByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}


////////////////////////////////////////////////////
// write EDM spatial distance simulated observation line
////////////////////////////////////////////////////
void	TSimFileWriter::writeSimEDMSpaDist(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSEDMSpaDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSEDMSpaDist();
			iterEnd = calcDS->endLSEDMSpaDist();

			obs = dataSet->getCalcEDMSpaDistByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}


////////////////////////////////////////////////////
// write polar 3d simulated observation line
////////////////////////////////////////////////////
void	TSimFileWriter::writeSimPolar(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSPolarIter iter, iterEnd, obs;
			iter = calcDS->beginLSPolar();
			iterEnd = calcDS->endLSPolar();

			obs = dataSet->getCalcPolarByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getHorizontalAngleObservation()->getObsAngle()<<separator
					<< obs->getZenithDistanceObservation()->getObsAngle() << separator
					<< obs->getSpatialDistanceObservation()->getObsDist() <<end<<endl;
			}
		}
	}


	return;
}


////////////////////////////////////////////////////
// write horizontal angle simulated observation line
////////////////////////////////////////////////////
void	TSimFileWriter::writeSimHorAng(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSHorAngIter iter, iterEnd, obs;
			iter = calcDS->beginLSHorAng();
			iterEnd = calcDS->endLSHorAng();

			obs = dataSet->getCalcHorAngByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsAngle()<<separator<<end<<endl;
			}
		}
	}


	return;
}

///////////////////////////////////////////////////////
// write horizontal distance simulated observation line
///////////////////////////////////////////////////////
void	TSimFileWriter::writeSimHorDist(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSHorDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSHorDist();
			iterEnd = calcDS->endLSHorDist();

			obs = dataSet->getCalcHorDistByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

/////////////////////////////////////////////////////
// write vertical distance simulated observation line
/////////////////////////////////////////////////////
void	TSimFileWriter::writeSimVertDist(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string station, target, end;
			(*inputFile)>>station;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSVertDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSVertDist();
			iterEnd = calcDS->endLSVertDist();
			obs = iterEnd;

			obs = dataSet->getVertDistByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

////////////////////////////////////////////////////
// write zenital distance simulated observation line
////////////////////////////////////////////////////
void	TSimFileWriter::writeSimZenDist(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSZenDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSZenDist();
			iterEnd = calcDS->endLSZenDist();

			obs = dataSet->getCalcZenDistByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsAngle()<<separator<<end<<endl;
			}
		}
	}


	return;
}


//////////////////////////////////////////
// write Offset simulated observation line
//////////////////////////////////////////
void	TSimFileWriter::writeSimOffsetToSpaLine(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToSpaLineIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToSpaLine();
			iterEnd = calcDS->endLSOffsetToSpaLine();

			obs = dataSet->getOffSpaLineByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}


void	TSimFileWriter::writeSimOffsetToVerPlane(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToVerPlaneIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToVerPlane();
			iterEnd = calcDS->endLSOffsetToVerPlane();

			obs = dataSet->getOffVerPlaneByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

void	TSimFileWriter::writeSimOffsetToVerLine(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToVerLineIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToVerLine();
			iterEnd = calcDS->endLSOffsetToVerLine();

			obs = dataSet->getOffVerLineByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

void	TSimFileWriter::writeSimOffsetToTheoPlane(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			TAngle obsAng(LITERAL(0.0));
			(*inputFile)>>target;
			
			if (!(stream->hasNoObsToRead()))
			{				
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToTheoPlaneIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToTheoPlane();
			iterEnd = calcDS->endLSOffsetToTheoPlane();

			obs = dataSet->getOffTheoPlaneByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

///////////////////////////////////////////////
// write orientation simulated observation line
///////////////////////////////////////////////
void	TSimFileWriter::writeSimGyroOrie(TAStreamFormatter* inputFile, int& obsID, const LSCalcDataSet* dataSet)
{
	TAStreamFormatter* stream = getStream();
	LSCalcDataSet* calcDS = getLSCalcDataSet();
	string separator = getSeparator();

	while(inputFile->peek() != '*')
	{
		if(inputFile->peek() == '%')
		{
			string comment = inputFile->readLine();
			(*stream)<<comment<<endl;
		}
		else
		{
			string target, end;
			(*inputFile)>>target;
			if (!(stream->hasNoObsToRead()))
			{
				TAngle obsAng;
				*inputFile>>obsAng;
			}
			end = inputFile->readLine();
			
			LSGyroOrieIter iter, iterEnd, obs;
			iter = calcDS->beginLSGyroOrie();
			iterEnd = calcDS->endLSGyroOrie();

			obs = dataSet->getGyroOrieByID(obsID++);

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator
					<<obs->getObsAngle()<<separator<<end<<endl;
			}
		}
	}


	return;
}



