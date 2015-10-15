#if 0
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

	while(!(inputFile->atEnd()))
	{
		char C = inputFile->peek();
		if(C == '*')
		{
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
			else if(keyW == "*DMES" || keyW == "*DTHE")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimSpaDist(inputFile);
			}
			else if(keyW == "*ANGL")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimHorAng(inputFile);
			}
			else if(keyW == "*ZENI" || keyW == "*ZENH")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimZenDist(inputFile);
			}
			else if(keyW == "*DVER")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimVertDist(inputFile, false);
			}
			else if(keyW == "*DLEV")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimVertDist(inputFile, true);
			}
			else if(keyW == "*DHOR")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimHorDist(inputFile);
			}
			else if(keyW == "*ECVE")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToVerLine(inputFile);
			}
			else if(keyW == "*ECSP")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToSpaLine(inputFile);
			}
			else if(keyW == "*ECHO")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToVerPlane(inputFile);
			}
			else if(keyW == "*ECTH")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimOffsetToTheoPlane(inputFile);
			}
			else if(keyW == "*ORIE")
			{
				(*stream)<<(keyW + inputFile->readLine())<<endl;
				writeSimGyroOrie(inputFile);
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
void	TSimFileWriter::writeSimSpaDist(TAStreamFormatter* inputFile)
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
			
			LSSpaDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSSpaDist();
			iterEnd = calcDS->endLSSpaDist();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station && iter->getTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<station<<separator<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}


////////////////////////////////////////////////////
// write horizontal angle simulated observation line
////////////////////////////////////////////////////
void	TSimFileWriter::writeSimHorAng(TAStreamFormatter* inputFile)
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
			
			LSHorAngIter iter, iterEnd, obs;
			iter = calcDS->beginLSHorAng();
			iterEnd = calcDS->endLSHorAng();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station && iter->getTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<station<<separator<<target<<separator
					<<obs->getObsAngle()<<separator<<end<<endl;
			}
		}
	}


	return;
}

///////////////////////////////////////////////////////
// write horizontal distance simulated observation line
///////////////////////////////////////////////////////
void	TSimFileWriter::writeSimHorDist(TAStreamFormatter* inputFile)
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
			
			LSHorDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSHorDist();
			iterEnd = calcDS->endLSHorDist();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station && iter->getTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<station<<separator<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

/////////////////////////////////////////////////////
// write vertical distance simulated observation line
/////////////////////////////////////////////////////
void	TSimFileWriter::writeSimVertDist(TAStreamFormatter* inputFile, bool isDLEV)
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
			iter = calcDS->beginLSVertDist(isDLEV);
			iterEnd = calcDS->endLSVertDist(isDLEV);
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getRefPoint()->getName() == station && iter->getTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<station<<separator<<target<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

////////////////////////////////////////////////////
// write zenital distance simulated observation line
////////////////////////////////////////////////////
void	TSimFileWriter::writeSimZenDist(TAStreamFormatter* inputFile)
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
			
			LSZenDistIter iter, iterEnd, obs;
			iter = calcDS->beginLSZenDist();
			iterEnd = calcDS->endLSZenDist();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station && iter->getTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<station<<separator<<target<<separator
					<<obs->getObsAngle()<<separator<<end<<endl;
			}
		}
	}


	return;
}


//////////////////////////////////////////
// write Offset simulated observation line
//////////////////////////////////////////
void	TSimFileWriter::writeSimOffsetToSpaLine(TAStreamFormatter* inputFile)
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
			string station, firstTarget, secondTarget, end;
			(*inputFile)>>firstTarget;
			(*inputFile)>>station;
			(*inputFile)>>secondTarget;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToSpaLineIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToSpaLine();
			iterEnd = calcDS->endLSOffsetToSpaLine();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station 
					&& iter->getFirstTgPoint()->getName() == firstTarget
					&& iter->getSecondTgPoint()->getName() == secondTarget
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<firstTarget<<separator<<station<<separator
					<<secondTarget<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}


void	TSimFileWriter::writeSimOffsetToVerPlane(TAStreamFormatter* inputFile)
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
			string station, firstTarget, secondTarget, end;
			(*inputFile)>>firstTarget;
			(*inputFile)>>station;
			(*inputFile)>>secondTarget;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToVerPlaneIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToVerPlane();
			iterEnd = calcDS->endLSOffsetToVerPlane();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station 
					&& iter->getFirstTgPoint()->getName() == firstTarget
					&& iter->getSecondTgPoint()->getName() == secondTarget
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<firstTarget<<separator<<station<<separator
					<<secondTarget<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

void	TSimFileWriter::writeSimOffsetToVerLine(TAStreamFormatter* inputFile)
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
			(*inputFile)>>target;
			(*inputFile)>>station;
			if (!(stream->hasNoObsToRead()))
			{
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToVerLineIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToVerLine();
			iterEnd = calcDS->endLSOffsetToVerLine();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station && iter->getFirstTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator<<station<<separator
					<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

void	TSimFileWriter::writeSimOffsetToTheoPlane(TAStreamFormatter* inputFile)
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
			TAngle obsAng(LITERAL(0.0));
			(*inputFile)>>target;
			(*inputFile)>>station;
			(*inputFile)>>obsAng;
			
			if (!(stream->hasNoObsToRead()))
			{				
				TLength obsLen;
				*inputFile>>obsLen;
			}
			end = inputFile->readLine();
			
			LSOffsetToTheoPlaneIter iter, iterEnd, obs;
			iter = calcDS->beginLSOffsetToTheoPlane();
			iterEnd = calcDS->endLSOffsetToTheoPlane();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station && iter->getFirstTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<target<<separator<<station<<separator
					<<obsAng<<separator<<obs->getObsDist()<<separator<<end<<endl;
			}
		}
	}


	return;
}

///////////////////////////////////////////////
// write orientation simulated observation line
///////////////////////////////////////////////
void	TSimFileWriter::writeSimGyroOrie(TAStreamFormatter* inputFile)
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
				TAngle obsAng;
				*inputFile>>obsAng;
			}
			end = inputFile->readLine();
			
			LSGyroOrieIter iter, iterEnd, obs;
			iter = calcDS->beginLSGyroOrie();
			iterEnd = calcDS->endLSGyroOrie();
			obs = iterEnd;

			bool stop = false;

			while(iter != iterEnd && stop == false)
			{
				if(iter->getStPoint()->getName() == station && iter->getTgPoint()->getName() == target
					&& iter->isSimValueWritten() == false)
				{
					obs = iter;
					iter->simValueWritten();
					stop = true;
				}
				iter++;
			}

			if (obs != iterEnd)
			{
				(*stream)<<station<<separator<<target<<separator
					<<obs->getObsAngle()<<separator<<end<<endl;
			}
		}
	}


	return;
}

#endif


