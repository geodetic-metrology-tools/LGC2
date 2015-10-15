////////////////////////////////////////////////////////////////////
// TObservationConverter.cpp
/*!
Base Class for converters writing Observations for an output LGC file

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TObservationConverter.h"
#include	"TObservationFormat.h"
#include	"TAStreamFormatter.h"

#include	"TAMeasurement.h"
#include	"TLGCProject.h"
/////////////////////////////////////////////////////////////////////

//ClassImp(TObservationConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
/*TObservationConverter::TObservationConverter()
{//default constructor
}*/

/*
TObservationConverter::TObservationConverter(TAStreamFormatter* stream):
TAConverter(stream)
{
}*/

TObservationConverter::TObservationConverter(TAStreamFormatter& stream):
TALGCConverter(stream)
{insertKeyChar('#');
}



TObservationConverter::~TObservationConverter()
{//destructor
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TObservationConverter::writeReliabilityHeader(	string name,
														string Observation,
														string unit1,
														string unit2)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();


	////////////////////////////////////////////////////////////////////////////////////
	//First line
	(*stream).writeStringLeft(nameWidth,	name);//first point's Name
	(*stream).writeStringLeft(nameWidth,	"");//third point's Name
	(*stream).writeString(obsWidth,	Observation);//mesured
	(*stream).writeString(obsResWidth,	"SIGMA");//sigma
	(*stream).writeString(obsResWidth,	"RESIDU");//residue 
	(*stream).writeString(obsResWidth,			"ZI");//local reliability statistic zi
	(*stream).writeString(3,			"");//zi flag
	(*stream).writeString(8,			"WI");//Gross-error detection statistics wi
	(*stream).writeString(3,			"");//wi flag
	(*stream).writeString(obsResWidth,			"GI");//Estimation of the gross-error gi
	(*stream).writeString(obsResWidth,			"NABLA");// Greatest undetected error NABLA
	(*stream).writeString(obsResWidth,			"TI");//Level of ease a gross error of size NABLA can be detected: Ti
	(*stream).writeString(obsResWidth,			"DELTY");//Maximum effect of an undetected gross-error: DELTY
	(*stream)<<endl;


	////////////////////////////////////////////////////////////////////////////////////
	//Second line
	(*stream).writeString(nameWidth,	"");//first point's Name
	(*stream).writeString(nameWidth,	"");//third point's Name
	(*stream).writeString(obsWidth,	unit1);//mesured
	(*stream).writeString(obsResWidth,	unit2);//sigma
	(*stream).writeString(obsResWidth,	unit2);//residue 
	(*stream).writeString(obsResWidth,			"(%)");//local reliability statistic zi
	(*stream).writeString(3,			"");//zi flag
	(*stream).writeString(8,			"");//Gross-error detection statistics wi
	(*stream).writeString(3,			"");//wi flag
	(*stream).writeString(obsResWidth,			unit2);//Estimation of the gross-error gi
	(*stream).writeString(obsResWidth,			unit2);// Greatest undetected error NABLA
	(*stream).writeString(obsResWidth,			"");//Maximum effect of an undetected gross-error: DELTY
	(*stream)<<endl<<endl;

	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//READ FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TObservationConverter::readPtName(TSpatialPointName &tgName)
{
	TAStreamFormatter*	stream = getStream();

	//get the target name
	stream->TAStreamFormatter::operator>>(tgName);
	stream->skipWhiteSpace();

	return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTECTED MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*void	TObservationConverter::writeKeyWord()
{

	(*getStream())<<fTextData.keyWordText;
	(*getStream())<<getSeparator();
	(*getStream())<<endl;
	return;
}*/







///////////////////////////////////////////////////////////////////////////////////////////////
//READER PROTECTED FUNCTION
///////////////////////////////////////////////////////////////////////////////////////////////
void	TObservationConverter::setComments(TAMeasurement* meas, string headCommentLine, string dbCom)
{

	meas->setHeaderComment(headCommentLine);// assign the comment to the point on the next line
	headCommentLine="";// "clear" the comment line storage
	meas->setComment(dbCom);
	return;
}


void	TObservationConverter::setId(TAMeasurement* meas, int ID)
{
	if (ID == -1)
	{
		meas->setId(0);//il faudra enlever tout les num of meas des converter, ca ne sert plus
	}
	else
	{
		meas->setId(ID);
	}

	return;
}


void	TObservationConverter::readDBComments(	char lineNum[9],
												string& iLine,
												string& line,
												int& i,
												int& id,
												string& comdb,
												string& EOLComments,
												TLGCProject* project)
{
	TAStreamFormatter*	stream = getStream();

	//data base comments
	stream->readChar(); //read the char $

	stream->TAStreamFormatter::operator>>(id);
	if(stream->fail())
	{
		stream->clear(); // a remplacer par good() lors du nouveau converter
		_itoa(i, lineNum, 10);
		iLine = lineNum;
		line = "line number " + iLine + " : ";
		string error = project->getError();
		error=error+line+ "after $, you have to indicate an identifiant" + '\n';
		project->setError(error);
		stream->skipWhiteSpace();
	}

	if (stream->peek() == '#')
	{
		stream->readChar(); //read the char #

		string comment = "";
		while(stream->peek() != EOF && stream->peek() != '\n' && stream->peek() != '%')
		{
			stream->TAStreamFormatter::operator>>(comment);
			comdb += comment+ " ";
			stream->skipWhiteSpace();
		}	
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////////////


