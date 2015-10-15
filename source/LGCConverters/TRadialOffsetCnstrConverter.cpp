////////////////////////////////////////////////////////////////////
// TRadialOffsetCnstrConverter.cpp
/*!
Write Radial Offset Constraints for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations

#include	"TRadialOffsetCnstrConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TRadialOffsetCnstrConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TRadialOffsetCnstrConverter::TRadialOffsetCnstrConverter(TAStreamFormatter& stream) :
TObservationConverter(stream)
{
	insertKeyChar(EOF);
	insertKeyChar('\n');
	insertKeyChar('%');
}

//destructor
TRadialOffsetCnstrConverter::~TRadialOffsetCnstrConverter()
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TRadialOffsetCnstrConverter::writeResultsHeader(const int )
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();

	////////////////////////////////////////////////////////////
	//first line
	(*stream).writeStringLeft(nameWidth,	"POINT"); //first point's Name
	(*stream).writeString(obsWidth,	"GISEMENT"); //direction
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma
	(*stream).writeString(obsResWidth,	"RESIDU"); //offset (mm)
	(*stream).writeString(obsResWidth,	"RES"); //offset/sigma
	(*stream)<<endl;	

	////////////////////////////////////////////////////////////
	//second line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(obsWidth,	"IMPOSE"); //direction
	(*stream).writeString(obsResWidth,	"(MM)"); //sigma
	(*stream).writeString(obsResWidth,	"(MM)"); //offset (mm)
	(*stream).writeString(obsResWidth,	"/SIG"); //offset/sigma
	(*stream)<<endl;	

	////////////////////////////////////////////////////////////
	//third line
	(*stream).writeString(nameWidth,	""); //first point's Name
	(*stream).writeString(obsWidth,	"(GRAD)"); //direction
	(*stream)<<endl<<endl;	
	
	return;
}



void	TRadialOffsetCnstrConverter::writeResults(LSRadOffCnstrConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	int					anglePrecision = getAnglePrecision();
	string				separator = getSeparator();

	//write Point 1
	(*stream).writeStringLeft(nameWidth, obsIt->getPointName());
	//write gisement
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream)<<(obsIt->getBearing())<<separator;
	
	//write the sigma
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setWidthFormat(obsResWidth);
	stream->setPrecisionFormat(lengthResidualPrecision);
	(*stream)<<(obsIt->getSigmaAPriori())<<(separator);
	
	//write the offset (mm) after calculation
	TLength residue = obsIt->getResidue();
	(*stream) << (residue) << separator;


	//write the offset / sigma
	stream->setPrecisionFormat(2);
	(*stream)<< obsIt->getResDivSig() << separator << endl;
	(*stream).setDataSpacing();

	return;
}



/////////////////////////////////////////////////////////////////////////
//RELIABILITY
/////////////////////////////////////////////////////////////////////////
void	TRadialOffsetCnstrConverter::writeReliabilityData(TLSCalcRadialOffsetCnstrObservation& obs)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					anglePrecision = this->getAnglePrecision();
	int					lengthResidualPrecision = getLengthResidualPrecision();
	string				separator = getSeparator();

	// get Point 1
	(*stream).writeStringLeft(nameWidth, obs.getPointName());

	//get Point 2
	(*stream).writeStringLeft(nameWidth, "");

	// get Point 3
	(*stream).writeStringLeft(nameWidth, "");

	//get bearing
	stream->setAngleUnits(TAngle::kGons);
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(anglePrecision);
	(*stream) << (obs.getBearing()) << separator;

	//get the sigma
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setPrecisionFormat(lengthResidualPrecision);
	stream->setWidthFormat(obsResWidth);
	(*stream) << obs.getSigmaAPriori() << separator;

	//get the offset (mm) after calculation
	if (obs.wToCompute())
	{
		(*stream) << obs.getResidue() << separator;
	}
	else
	{
		(*stream).writeString(obsResWidth, "");
	}

	// Test the computing: if it failed, write the corresponding error messages
	if (obs.paramsCanBeDetermined()) 
	{

		// sets the values format:
		//stream->setLengthUnits(TLength::kMillimetres);

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
		stream->width(obsResWidth);
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
	(*stream) <<endl ;
	(*stream).setDataSpacing();
	

	return;
}


void	TRadialOffsetCnstrConverter::writeReliabilityHeader()
{
	this->TObservationConverter::writeReliabilityHeader("POINT",
														"",
														"",
														"GIS",
														"G",
														"MM");
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////
//FORMAT CONSTRAINT LINE
/////////////////////////////////////////////////////////////////////////////////////////
void	TRadialOffsetCnstrConverter::writeRadData(LSRadOffCnstrConstIter cnstrIt)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = this->getNameWidth();
	int					obsWidth = this->getObsWidth();
	int					anglePrecision = this->getAnglePrecision();
	int					obsResWidth = this->getObsResWidth();
	int					lengthResidualPrecision = this->getLengthResidualPrecision();
		
	//name
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeStringLeft(nameWidth, cnstrIt->getPointName() );

	//res
	writeLength(obsResWidth, lengthResidualPrecision, TLength::kMillimetres, cnstrIt->getResidue() );
	(*stream)<<getSeparator();
	//bearing
	writeAngle(obsWidth, anglePrecision, TAngle::kGons, cnstrIt->getBearing());
	(*stream)<<getSeparator();
	//sigma
	writeLength(obsResWidth, lengthResidualPrecision, TLength::kMillimetres,cnstrIt->getSigmaAPriori() );
	(*stream)<<getSeparator();
	(*stream)<<endl;

	return;
}



void	TRadialOffsetCnstrConverter::writeMesData(LSRadOffCnstrConstIter cnstrIt)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = this->getNameWidth();
	int					coordResWidth = this->getCoordResWidth();
	int					obsWidth = this->getObsWidth();
	int					obsResWidth = this->getObsResWidth();
	int					anglePrecision = this->getAnglePrecision();
	int					lengthResidualPrecision = this->getLengthResidualPrecision();

	//write Id
	(*stream).width(1);
	(*stream)<<"";
	(*stream).writeInteger(coordResWidth, cnstrIt->getId());
	//write pt1
	(*stream).writeString(nameWidth, cnstrIt->getPointName());
	//write pt2
	(*stream).writeString(nameWidth, "");
	//write pt3
	(*stream).writeString(nameWidth, "");
	//gis
	(*stream).writeString(obsWidth, "");
	//write obs
	writeAngle(obsWidth, anglePrecision, TAngle::kGons, cnstrIt->getBearing());
	(*stream)<<getSeparator();
	//write calc
	(*stream).writeString(obsWidth, "");
	//write res
	writeLength(obsResWidth, lengthResidualPrecision, TLength::kMillimetres, cnstrIt->getResidue() );
	(*stream)<<getSeparator();
	//write sig
	writeLength(obsResWidth, lengthResidualPrecision, TLength::kMillimetres,cnstrIt->getSigmaAPriori() );
	(*stream)<<getSeparator();
	//v0
	(*stream).writeString(obsWidth, "");
	//com
	stream->writeStringLeft(nameWidth, "");
	(*stream) << endl;

	return;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//READ FUNCTION
////////////////////////////////////////////////////////////////////////////////////////////////////
int TRadialOffsetCnstrConverter::readObservation(int i, int& , TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, error;

//	TRadialOffsetCnstr* newCnstr = new TRadialOffsetCnstr;
			
	string headCommentLine = "";
	

	TReal colSigma = LITERAL(1.0);
	bool noColSig = readColSigma(colSigma, i);
	if(noColSig)
	{
		project->getDataSet()->setDefaultRadOffCnstrSig(true);
	}

	//read the offset measurements
	while ((stream->peek()) != '*')
	{
		stream->skipWhiteSpace();
		if(stream->peek()=='%')
		{
			readP100Comment(headCommentLine);
		}

		else
		{

			TSpatialPointName ptName;
			TAngle cBearing(LITERAL(0.0));
			TLength cWidth(LITERAL(0.0));
			TLength cSigma;

			
			readPtName(ptName);

			//read the bearing
			stream->TAStreamFormatter::operator>>(cBearing);
			stream->skipWhiteSpace();

			readSigma(cWidth);
	
			//create the radial offset constraint
			TRadialOffsetCnstr* newCnstr = new TRadialOffsetCnstr(ptName, cWidth, cBearing);

			// set of the global sigma, if necessary
			cSigma.setMMetresValue(colSigma);
			newCnstr->ifNotDoneSetSigma(cSigma);

			// add to fError errors from the read of measurement
			if (stream->getError() != "")
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				string error = project->getError();
				error=error+ line + stream->getError() + '\n';
				project->setError(error);
				stream->setError("");
			}

	
			//read the comment, id. and constant
			string EOLComments="";
			string comdb="";

			while (!(stream->peek()=='\n'))
			{

				switch (stream->peek())
				{
				/*	case '$':
						readDBComments(	lineNum, iLine, line,
										i,
										id, comdb, EOLComments,
										project);
						break;*/

					case '#':
						readP100Comment(EOLComments);
						break;

					case '%':
						readP100Comment(EOLComments);
						break;

					default:
						// other characters to read on this line correspond to an error
						_itoa(i, lineNum, 10);
						iLine = lineNum;
						line = "line number " + iLine + " : ";
						string error = project->getError();
						error=error+line+ "Too much informations to read" + '\n';
						project->setError(error);
						while (!(stream->peek()=='\n')){
							stream->readChar();}
						break;
				}
				stream->skipWhiteSpace();
			}

	//		setId(newCnstr, id, numOfMeas);
			newCnstr->setHeaderComment(headCommentLine);
			// "clear" the comment line storage
			headCommentLine="";

			project->getDataSet()->getWorkingConstraints()->addRadOffCnstr(newCnstr);
			project->getDataSet()->addToRadOffCnstrNum();

			delete newCnstr;
		}
		stream->readLine();
		stream->skipWhiteSpace();
		i++;
	}

	if (stream->peek() == EOF)
	{
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		string error = project->getError();
		error=error+line+ """*END"" keyword expected" + '\n';
		project->setError(error);
	}
	
	return i;
}


bool	TRadialOffsetCnstrConverter::readColSigma(TReal& colSigma, int &i)
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

void	TRadialOffsetCnstrConverter::readSigma(TLength &sigma)
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
/////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////



