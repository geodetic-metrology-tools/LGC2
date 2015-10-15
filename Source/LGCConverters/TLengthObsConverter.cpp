////////////////////////////////////////////////////////////////////
// TLengthObsConverter.cpp
/*!
Base Class for converters writing Length type Observations for an output LGC file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TLengthObsConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TLengthObsConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TLengthObsConverter::TLengthObsConverter(TAStreamFormatter& stream) :
TObservationConverter(stream)
{}

//destructor
TLengthObsConverter::~TLengthObsConverter()
{}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//FAUT FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TLengthObsConverter::writeReliabilityData(TALSCalcLengthObservation& obs,
												  string name)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();

	// get Point 1
	(*stream).writeStringLeft(nameWidth, name);

	// get Point 3
	(*stream).writeStringLeft(nameWidth, "");

	//get the observed distance
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	(*stream) << (obs.getCorrectedObsDist()) << separator;

	//get the sigma
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setWidthFormat(obsResWidth);
	stream->setPrecisionFormat(lengthResidualPrecision);
	(*stream) << obs.getSigmaAPriori() << separator;

	//get the offset (cc) after calculation
	if (obs.wToCompute())
	{
		(*stream) << obs.getResidue() << separator;
	}
	else
	{
		(*stream).writeStringLeft(obsResWidth, "");
	}

	// Test the computing: if it failed, write the corresponding error messages
	if (obs.paramsCanBeDetermined()) 
	{
		//get zi
		TReal z = obs.getZ();
		stream->setf(ios::fixed,ios::floatfield);
		// checks that z is between 25% and 60%, and if not, adds some symbols
		(*stream).writeDouble(obsResWidth,1,100*z);
				
		if (z < LITERAL(0.25)) 
		{	(*stream).writeString(3,"**");}	

		if (z > LITERAL(0.60)) 
		{	(*stream).writeString(3,"!");}	

		if (z>=LITERAL(0.25) && z<=LITERAL(0.60))
		{	(*stream).writeString(3,"");}
		
		//get wi
		if (obs.wToCompute())
		{
			(*stream).writeDouble(8,2, obs.getW());
		}
		else
		{
			(*stream).writeString(8,"");
		}
	
		//get gi	
		if (obs.gToCompute())
		{
			(*stream).writeString(3,"**");
			(*stream) << right << obs.getG() << separator;
		}
		else
		{
			(*stream).writeString(3,"");
			(*stream).writeString(obsResWidth,"");
		}
		
		// get nabla
		if (obs.deltaComputed()) 
		{ //tests that the nabla has been correctly computed
			(*stream) << right << obs.getNabla() << separator;
		}
		else
		{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

		// get T
		(*stream).writeDouble(obsResWidth,2, obs.getT());
		
		// get DELTY
		stream->width(10);
		if (obs.deltaComputed())/*tests that the delty has been correctly computed*/
		{ 
			(*stream).writeDouble(obsResWidth,2, obs.getDelty());
		}
		else
		{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

	}

	else 
	{ // write "ind" (indetermined) instead of values
		// Is Zi = LITERAL(0.0)?
		if ((obs.getResVariance().getValue() == LITERAL(0.0)) || (obs.getZ() < LITERAL(0.0001)))
		{
			(*stream).writeString(obsResWidth,"0.0");//z
			(*stream).writeString(3,"");
			(*stream).writeString(8,"INDETERMINE");//wi
			(*stream).writeString(0,"");
			(*stream).writeString(obsResWidth,"");//gi
			(*stream).writeString(obsResWidth,"");//nabla
			(*stream).writeString(obsResWidth,"INFINI");//T
			(*stream).writeString(obsResWidth,"");//DELTY
		}
		else 
		{ // Zi is undetermined
			(*stream).writeString(obsResWidth,"INFINI");//z
			(*stream).writeString(3,"");
			(*stream).writeString(8,"***");//wi
			(*stream).writeString(3,"");
			(*stream).writeString(obsResWidth,"***");//gi
			(*stream).writeString(obsResWidth,"***");//nabla
			(*stream).writeString(obsResWidth,"***");//T
			(*stream).writeString(obsResWidth,"***");//DELTY
		}
	}
	(*stream)<<endl;
	(*stream).setDataSpacing();

	return;
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////
//PUNC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TLengthObsConverter::formatMesLine(const long int id, const string namept1, const string namept2, const string namept3, const TAngle gis, const TLength obs, const TLength calc, const TLength res, const TLength sig, const TAngle v0, const string com)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = this->getNameWidth();
	int					coordResWidth = this->getCoordResWidth();
	int					obsWidth = this->getObsWidth();
	int					obsResWidth = this->getObsResWidth();
	int					anglePrecision = this->getAnglePrecision();
	int					lengthPrecision = this->getLengthPrecision();
	int					lengthResidualPrecision = this->getLengthResidualPrecision();

	//write Id
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeInteger(coordResWidth, id);
	//write pt1
	(*stream).writeString(nameWidth,namept1);
	//write pt2
	(*stream).writeString(nameWidth,namept2);
	//write pt3
	(*stream).writeString(nameWidth,namept3);
	//gis
	if(gis.isNull())
	{
		stream->writeString(obsWidth, "");
	}
	else
	{
		 writeAngle(obsWidth,anglePrecision,TAngle::kGons,gis);
		(*stream)<<getSeparator();
	}
	//write obs
	writeLength(obsWidth, lengthPrecision, TLength::kMetres, obs);
	(*stream) << getSeparator();
	//write calc
	writeLength(obsWidth, lengthPrecision, TLength::kMetres, calc);
	(*stream) << getSeparator();
	//write res
	writeLength(obsResWidth, lengthResidualPrecision, TLength::kMillimetres, res);
	(*stream) << getSeparator();	
	//write sig
	writeLength(obsResWidth, lengthResidualPrecision, TLength::kMillimetres, sig);
	(*stream) << getSeparator();
	//v0
	if(v0.isNull())
	{
		(*stream).writeString(obsWidth, "");
	}
	else
	{
		writeAngle(obsWidth, anglePrecision, TAngle::kGons, v0);
		(*stream) << getSeparator();
	}
	//com
	stream->writeStringLeft(nameWidth, com);
	(*stream) << endl;

	return;
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////
//READ FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool	TLengthObsConverter::readColSigma(TReal& colSigma, TReal& colSigmaPpm, int &i)
{
	TAStreamFormatter*	stream = getStream();
	bool noColSigToRead = false;

	//check if there is a collective sigma to read
	stream->skipWhiteSpace();
	if (!stream->atEnd() && stream->peek()!='\n')
	{
		*stream >> colSigma;
		stream->skipWhiteSpace();
		if (!stream->atEnd() && stream->peek()!='\n')
		{
			*stream >> colSigmaPpm;
		}
	}
	else
	{
		noColSigToRead = true;
	}

	stream->readLine();
	stream->skipWhiteSpace();
	i++;

	return noColSigToRead;
}


bool	TLengthObsConverter::readColSigma(TReal& colSigma, int &i)
{
	TAStreamFormatter*	stream = getStream();
	bool noColSigToRead = false;

	//check if there is a collective sigma to read
	stream->skipWhiteSpace();
	if (!stream->atEnd() && stream->peek()!='\n')
	{
		*stream >> colSigma;
	}
	else
	{
		noColSigToRead = true;
	}


	stream->readLine();
	stream->skipWhiteSpace();
	i++;
	return noColSigToRead;
}



void	TLengthObsConverter::addConstant2List(TDistConstants& cte, 
										 	 TLGCProject* project)
{
	// if necessary, add the constant to the Working Constants
	if (project->getDataSet()->getWorkingConstants()->numberOfDistConstants() == 0)
	{
		project->getDataSet()->getWorkingConstants()->addDistConstant(&cte);
	}
	else
	{
		DistConstantIterator constIter = --(project->getDataSet()->getWorkingConstants()->getDistConstantsEndIterator());
		if (!(cte.getValue() == constIter->getValue()) || !(cte.getStatus() == constIter->getStatus()))
		{
			cte.setConstantName("");
			project->getDataSet()->getWorkingConstants()->addDistConstant(&cte);
		}
		else
		{
			cte.setConstantName(constIter->getConstantName());
		}
	}
	
	return;
}






void	TLengthObsConverter::readMeasurement(TLength &obsDist)
{
	TAStreamFormatter*	stream = getStream();

	if (stream->hasNoObsToRead() == false)
	{// get the observed distance value
		stream->TAStreamFormatter::operator>>(obsDist);
	}
	stream->TAStreamFormatter::skipWhiteSpace();

	return;
}



void	TLengthObsConverter::readSigma(TLength &sigma, TLength &sigmaPpm)
{
	TAStreamFormatter*	stream = getStream();

	//possibly read the measurement's precision
	list<int>::iterator	charIt = getKeyCharListBegin();
	list<int>::iterator	charItEnd =	getKeyCharListEnd();

	bool isAKeyChar = false;
	while(charIt != charItEnd && isAKeyChar == false)
	{
		isAKeyChar = ((*charIt) == stream->peek());
		charIt++;
	}

	if(!isAKeyChar)
	{
		stream->TAStreamFormatter::operator>>(sigma);
		sigma *= LITERAL(0.001);
		stream->skipWhiteSpace();

		charIt = getKeyCharListBegin();
		charItEnd =	getKeyCharListEnd();

		while(charIt != charItEnd && isAKeyChar == false)
		{
			isAKeyChar = ((*charIt) == stream->peek());
			charIt++;
		}

		if(!isAKeyChar)
		{
			stream->TAStreamFormatter::operator>>(sigmaPpm);
			sigmaPpm *= LITERAL(0.001);
		}
	}
	
	stream->skipWhiteSpace();

	return;
}



void	TLengthObsConverter::readSigma(TLength &sigma)
{
	TAStreamFormatter*	stream = getStream();

	//possibly read the measurement's precision
	list<int>::iterator	charIt = getKeyCharListBegin();
	list<int>::iterator	charItEnd =	getKeyCharListEnd();

	bool isAKeyChar = false;
	while(charIt != charItEnd && isAKeyChar == false)
	{
		isAKeyChar = ((*charIt) == stream->peek());
		charIt++;
	}

	if(!isAKeyChar)
	{
		stream->TAStreamFormatter::operator>>(sigma);
		sigma *= LITERAL(0.001);
	}
	
	stream->skipWhiteSpace();

	return;
}


void  TLengthObsConverter::readConstant( TDistConstants &cte )
{//output a spatial position to the text stream
	TAStreamFormatter*	stream = getStream();

	stream->readChar();// '/'

	TLength value;
	stream->TAStreamFormatter::operator>>(value);
	
	cte.setValue(value);
	cte.setStatus(TAMeasurement::kFixed);

	stream->skipWhiteSpace();

	return;
}
