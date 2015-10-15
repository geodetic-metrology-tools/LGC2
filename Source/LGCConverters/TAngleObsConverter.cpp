////////////////////////////////////////////////////////////////////
// TAngleObsConverter.cpp
/*!
Base Class for converters writing Angle type Observations for an output LGC file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TAngleObsConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"

/////////////////////////////////////////////////////////////////////

//ClassImp(TAngleObsConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TAngleObsConverter::TAngleObsConverter(TAStreamFormatter& stream) :
TObservationConverter(stream)
{}

//destructor
TAngleObsConverter::~TAngleObsConverter()
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TAngleObsConverter::writeReliabilityData(TALSCalcAngleObservation& absAng)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = getAnglePrecision();
	int					angleResidualPrecision = getAngleResidualPrecision();
	string				separator = getSeparator();

	//get Point 2
	(*stream).writeStringLeft(nameWidth, absAng.getTgPointName());

	// get Point 3
	(*stream).writeStringLeft(nameWidth, "");

	//get the observed angle
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream) << (absAng.getCorrectedObsAngle()) << separator;

	//get the sigma
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setWidthFormat(obsResWidth);
	stream->setPrecisionFormat(angleResidualPrecision);
	(*stream) << absAng.getSigmaAPriori() << separator;

	//get the offset (cc) after calculation
	if (absAng.wToCompute()) 
	{
		(*stream) << absAng.getResidue() << separator;
	}
	else
	{
		(*stream).writeString(obsResWidth, "");
	}

	//get Parameters
	if (absAng.paramsCanBeDetermined())
	{
		stream->setPrecisionFormat(angleResidualPrecision);
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setWidthFormat(obsResWidth);

		// get z
		TReal z = absAng.getZ();
		stream->setf(ios::fixed,ios::floatfield);
		(*stream).writeDouble(obsResWidth,1,100*z);
		//(*stream)<<stream->getNonSpaceSeparator();
		if (z < LITERAL(0.25))
		{	(*stream).writeString(3,"**"); }		 

		if (z > LITERAL(0.60)) 
		{	(*stream).writeString(3,"!");}	

		if (z>=LITERAL(0.25) && z<=LITERAL(0.60))
		{	(*stream).writeString(3,"");}

		//get wi
		if (absAng.wToCompute()) 
		{
			(*stream).writeDouble(8,2, absAng.getW());
		}
		else
		{
			(*stream).writeString(8,"");
		}

		//get gi
		if (absAng.gToCompute())
		{
			(*stream).writeString(3,"**");
			(*stream) << right << absAng.getG() << separator;		
		}
		else 
		{
			(*stream).writeString(3,"");
			(*stream).writeString(obsResWidth,"");
		}

		// get nabla
		if (absAng.deltaComputed())/*tests that the nabla has been correctly computed*/
		{ 
			(*stream) << right << absAng.getNabla() << separator;
		}
		else
		{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}

		// get T
		(*stream).writeDouble(obsResWidth,2, absAng.getT());
	

		// get DELTY
		if (absAng.deltaComputed())/*tests that the delty has been correctly computed*/
		{ 
			(*stream).writeDouble(obsResWidth,2, absAng.getDelty());
		}
		else
		{
			(*stream).writeString(obsResWidth,"INDTMNE");
		}
	}
	else
	{ // write "ind" (indetermined) instead of values
		// Is Zi = LITERAL(0.0)?
		if ((absAng.getResVariance().getValue() == LITERAL(0.0)) || (absAng.getZ() < LITERAL(0.0001)))
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
		// Zi is undetermined
		else
		{ 
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



void	TAngleObsConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("TARGET",
														"OBSERVE",
														"G",
														"CC");
	return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//PUNC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TAngleObsConverter::formatMesLine(const long int id, const string namept1, const string namept2, const string namept3,  const TAngle obs, const TAngle calc, const TAngle res, const TAngle sig, const TAngle v0, const string com)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = this->getNameWidth();
	int					coordResWidth = this->getCoordResWidth();
	int					obsWidth = this->getObsWidth();
	int					obsResWidth = this->getObsResWidth();
	int					anglePrecision = this->getAnglePrecision();
	int					angleResidualPrecision = this->getAngleResidualPrecision();

	//write Id
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeInteger(coordResWidth, id);
	//write pt1
	(*stream).writeString(nameWidth, namept1);
	//write pt2
	(*stream).writeString(nameWidth, namept2);
	//write pt3
	(*stream).writeString(nameWidth, namept3);
	// Gis element
	stream->writeString(obsWidth, "");
	//write obs
	writeAngle(obsWidth, anglePrecision, TAngle::kGons, obs);
	(*stream)<<getSeparator();
	//write calc
	writeAngle(obsWidth, anglePrecision, TAngle::kGons, calc);
	(*stream)<<getSeparator();
	//write res
	writeAngle(obsResWidth, angleResidualPrecision, TAngle::k100MicroGons, res);
	(*stream)<<getSeparator();
	//write sig
	writeAngle(obsResWidth, angleResidualPrecision, TAngle::k100MicroGons, sig);
	(*stream)<<getSeparator();
	//v0
	if(v0.isNull())
	{
		stream->writeString(obsWidth, "");
	}
	else
	{
		writeAngle(obsWidth, anglePrecision, TAngle::kGons, v0);
		(*stream)<<getSeparator();
	}
	//com
	stream->writeStringLeft(nameWidth, com);
	(*stream) << endl;

	return;
}












//////////////////////////////////////////////////////////////////////////////////////////////////////////
//READER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
TAStreamFormatter& TAngleObsConverter::readMeasurement(TSpatialPointName& tgName,
													   TAngle &obsAngle, TAngle &sigma)
{//inputs a horizontal angle measurement from the text stream

	TAStreamFormatter*	stream = getStream();

	//get the target name
	stream->TAStreamFormatter::operator>>(tgName);

	if (stream->hasNoObsToRead() == false)
	{// get the observed angle value
		stream->TAStreamFormatter::operator>>(obsAngle);
	}

	stream->TAStreamFormatter::skipWhiteSpace();

	
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
		sigma *= LITERAL(0.0001);
	}
	
	stream->skipWhiteSpace();

	return *stream;
}


bool	TAngleObsConverter::readColSigma(TReal &colSigma, int& i)
{
	TAStreamFormatter*	stream = getStream();

	bool res = false;
	//check if there is a collective sigma to read
	stream->skipWhiteSpace();
	if (!stream->atEnd() && stream->peek() != '\n')
	{
		*stream >> colSigma;
		colSigma *= LITERAL(0.0001);
	}
	else
	{
		res = true;
	}

	stream->readLine();
	stream->skipWhiteSpace();
	i++;
	return res;
}




void	TAngleObsConverter::readConstant( TAngleConstants &cte )
{//output a spatial position to the text stream

	TAStreamFormatter*	stream = getStream();

	stream->readChar();// '/'

	TAngle value;
	stream->TAStreamFormatter::operator>>(value);
	
	cte.setValue(value);
	cte.setStatus(TAMeasurement::kFixed);

	stream->skipWhiteSpace();

	return;
}


