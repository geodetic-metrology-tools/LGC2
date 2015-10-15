////////////////////////////////////////////////////////////////////
// TRelativeErrorConverter.cpp
/*!
Write Relative Erros for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TRelativeErrorConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TRelativeErrorConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////
//default constructor
TRelativeErrorConverter::TRelativeErrorConverter(TAStreamFormatter& stream) :
TAConverter(stream)
{}

//destructor
TRelativeErrorConverter::~TRelativeErrorConverter()
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TRelativeErrorConverter::writeResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsResWidth = max(getObsResWidth(), 9);

	// write header
	(*stream) << endl << endl << endl;
	(*stream) << "ERREURS RELATIVES " << endl;
	(*stream) << "*******************" << endl << endl;

	//////////////////////////////////////////////////////////////
	//line1
	// point 1 & 2
	(*stream).writeStringLeft(nameWidth, "POINT 1");
	(*stream).writeStringLeft(nameWidth ,"POINT 2");
	// Sigmas
	(*stream).writeString(obsResWidth,"SIGMA L");
	(*stream).writeString(obsResWidth,"SIGMA G");
	(*stream).writeString(obsResWidth,"SIGMA R");
	(*stream).writeString(obsResWidth,"SIGMA Z");
	(*stream).writeString(obsResWidth,"SIGMA V");
	(*stream)<<endl;

	//////////////////////////////////////////////////////////////
	//line2
	// units
	(*stream).writeString(nameWidth, " ");
	(*stream).writeString(nameWidth ," ");
	(*stream).writeString(obsResWidth,"(MM)");
	(*stream).writeString(obsResWidth,"(CC)");
	(*stream).writeString(obsResWidth,"(MM)");
	(*stream).writeString(obsResWidth,"(MM)");
	(*stream).writeString(obsResWidth,"(CC)");
	(*stream)<<endl<<endl;	

	return;
}



void	TRelativeErrorConverter::writeResults(LSRelErrorConstIter obsIt)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	string				separator = getSeparator();

	// write points name

	(*stream).writeStringLeft(nameWidth, obsIt->getPoint1Name());
	(*stream).writeStringLeft(nameWidth ,obsIt->getPoint2Name());
	// sets the values format:
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setAngleUnits(TAngle::k100MicroGons);
	stream->setPrecisionFormat(getLengthPrecision());
	stream->setWidthFormat(max(getObsResWidth(), 9));

	//sigma L
	(*stream) << right << obsIt->getSigmaL() << separator;

	//sigma G			
	stream->setPrecisionFormat(getAnglePrecision());
	(*stream) << right << obsIt->getSigmaG() << separator;			
				
	//sigma R
	stream->setPrecisionFormat(getLengthPrecision());
	(*stream) << right << obsIt->getSigmaR() << separator;
				
	//sigma Z
	(*stream) << right << obsIt->getSigmaZ() << separator;

	//sigma V
	stream->setPrecisionFormat(getAnglePrecision());
	(*stream) << right << obsIt->getSigmaV() << separator << endl;
	(*stream).setDataSpacing();

	return;
}


/////////////////////////////////////////////////////////////////////////
//READ FUNCTION
/////////////////////////////////////////////////////////////////////////
int		TRelativeErrorConverter::readObservation(int i, TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	char lineNum[9];
	string iLine, line, warn;

	//read keyword
	stream->readLine();
	i++;
	
	stream->skipWhiteSpace();


	while ((stream->peek() != '*') && (stream->peek() != EOF))
	{
	
		stream->skipWhiteSpace();

		// check if there is a comment line
		if(stream->peek()=='%')
		{
			// read line to delete it
			stream->readLine();
		}
		else
		{//read point Pair

			TSpatialPointName ptA, ptB;
			if (stream->peek() != '\n')
			{
				stream->TAStreamFormatter::operator>>(ptA);
				stream->skipWhiteSpace();
				if (stream->peek() != '\n')
				{
					stream->TAStreamFormatter::operator>>(ptB);
					stream->skipWhiteSpace(); 
					//TPointNamePair np(ptA, ptB);
					project->getCalcParams()->push_backPtNamePairs(TPointNamePair (ptA, ptB));

					if (stream->peek() != '\n' && stream->peek() != '%' && stream->peek() != '#')
					{
						_itoa(i, lineNum, 10);
						iLine = lineNum;
						line = "line number " + iLine + " : ";
						warn =warn + line + "Trop de donnees" + '\n';
						project->getCalcParams()->setWarning(warn);
					}
				}
				else
				{
					_itoa(i, lineNum, 10);
					iLine = lineNum;
					line = "line number " + iLine + " : ";
					warn =warn + line + "Declaration de paire de points incomplete" + '\n';
					project->getCalcParams()->setWarning(warn);
				}
			}
			else
			{
				_itoa(i, lineNum, 10);
				iLine = lineNum;
				line = "line number " + iLine + " : ";
				warn =warn + line + "Pas de declaration de paire de points" + '\n';
				project->getCalcParams()->setWarning(warn);
			}

			stream->readLine();
		}
		i++;
	}


	if(project->getCalcParams()->getPtNamePairsSize()== 0)
	{
		warn = warn + "aucune paire de point déclarée";
		warn = warn + '\n';//encode sur deux ligne car sinon le message d erreur ne s ecrit pas entierement???
		project->getCalcParams()->setWarning(warn);
	}

	return i;
}





/////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////



