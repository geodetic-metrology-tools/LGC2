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
#include "Measurements\TLGCObsSummary.h"

//#include	"TAMeasurement.h"
//#include	"TLGCProject.h"
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

// write an observation table title
void	TObservationConverter::writeObsTitle(const string& description, const int numObs)
{
	TAStreamFormatter& stream = getStreamRef();
	//Title
	stream<<description<<getSeparator()<<"(NB. = "<<numObs<<" )"<<getSeparator()<<endl<<endl;

	return;
}

void TObservationConverter::writeAngleResultsSummary(TLGCObsSummary summary, std::string TABs){
	int					obsResWidth = getObsResWidth();
	int					angleResidualPrecision = max(getAngleResidualPrecision()-3, 0);
	TAStreamFormatter	&stream = getStreamRef();

	//Write statistic
	// mean residual
	stream << TABs;
	stream << "RESIDU MOYEN =  "; 
	stream.width( obsResWidth );
	stream.precision(angleResidualPrecision);
	stream << summary.getMean();
	stream << getSeparator();

	// mean residual statistical test limits
	stream<<" CC :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(angleResidualPrecision);
	stream << summary.getMeanLoLimit()<<", ";
	stream.precision(angleResidualPrecision);
	stream << summary.getMeanHiLimit()<<")";
	stream << endl;

	// variance of residuals
	stream << TABs;
	stream<<"ECART-TYPE   =  ";
	stream.width( obsResWidth );
	stream.precision(angleResidualPrecision);
	stream << summary.getVariance() ;
	stream << getSeparator();

	//variance of residuals statistical test limits
	stream<<" CC :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(angleResidualPrecision);
	stream << summary.getVarLoLimit()<<", ";
	stream.precision(angleResidualPrecision);
	stream << summary.getVarHiLimit()<<")";
	stream << getSeparator();
	stream << endl << endl;

	// write histograms for the observation residuals
	//writeResidualsHistogram(summary, description);
//	stream << endl;

}


void TObservationConverter::writeDistanceResultsSummary(TLGCObsSummary summary, std::string TABs){

	int					obsResWidth = getObsResWidth();
	int					lengthResidualPrecision = max(getLengthResidualPrecision()-2, 0);
	TAStreamFormatter	&stream = getStreamRef();
	//Write statistic
	stream << TABs;

	// mean residual
	stream<<"RESIDU MOYEN =  "; 
	stream.width( obsResWidth );
	stream.precision(lengthResidualPrecision);
	stream << summary.getMean();
	stream<<getSeparator();

	// mean residual statistical test limits
	stream<<" MM :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(lengthResidualPrecision);
	stream << summary.getMeanLoLimit()<<", ";
	stream.precision(lengthResidualPrecision);
	stream << summary.getMeanHiLimit()<<")";
	stream<<endl;

	stream << TABs;
	// variance of residuals
	stream<<"ECART-TYPE   =  ";
	stream.width( obsResWidth );
	stream.precision(lengthResidualPrecision);
	stream << summary.getVariance() ;
	stream<<getSeparator();

	//variance of residuals statistical test limits
	stream<<" MM :  LIMITES DE CONFIANCE A 95.0 = (";
	stream.precision(lengthResidualPrecision);
	stream << summary.getVarLoLimit()<<", ";
	stream.precision(lengthResidualPrecision);
	stream << summary.getVarHiLimit()<<")";
	stream<<getSeparator();
	stream<<endl<<endl;

	// write histograms for the observation residuals
	//writeResidualsHistogram(summary, description);
	//stream << endl;
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TObservationConverter::writeReliabilityHeader(	string name1,
														string name2,
														string name3,
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
	(*stream).writeStringLeft(nameWidth,	name1);//first point's Name
	(*stream).writeStringLeft(nameWidth,	name2);//second point's Name
	(*stream).writeStringLeft(nameWidth,	name3);//third point's Name
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
	(*stream).writeString(nameWidth,	"");//second point's Name
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
// PROTECTED MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*void	TObservationConverter::writeKeyWord()
{

	(*getStream())<<fTextData.keyWordText;
	(*getStream())<<getSeparator();
	(*getStream())<<endl;
	return;
}*/


// Get the French version of the observation description
// returns a string with the description
string	TObservationConverter::getObsDescriptionFR(TALGCConverter::ELGCObservations key)
{
	static const char* descriptions[] = {
		"POLAR3D",
		"Unit Vector Measurment (UVEC)",
		"Unit Vector Measurment + Distance (UVD)"
		"ANGLES HORIZONTAUX",
		"DISTANCES ZENITHALES",
		"DISTANCES MESUREES (DIST)",
		"DHOR",
		"DLEV",
		"NoObs",
	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCConverter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
}


// Get the English version of the observation description
// returns a string with the description
string	TObservationConverter::getObsDescriptionEN(TALGCConverter::ELGCObservations key)
{
	static const char* descriptions[] = {
		"POLAR3D",
		"Unit Vector Measurment (UVEC)",
		"Unit Vector Measurment + Distance (UVD)"
		"HORIZONTAL ANGLE OBSERVATIONS",
		"ZENITHAL DISTANCE OBSERVATIONS",
		"OBSERVED SPATIAL DISTANCES (DIST)",
		"DHOR",
		"DLEV",
		"NoObs",
	};

	static_assert((sizeof(descriptions)/sizeof(descriptions[0])) == TALGCConverter::ALWAYS_LAST, "Not enough description strings for enums");

	return descriptions[key];
}



///////////////////////////////////////////////////////////////////////////////////////////////
//READER PROTECTED FUNCTION
///////////////////////////////////////////////////////////////////////////////////////////////
#if 0
void	TObservationConverter::setComments(TAMeasurement* meas, string headCommentLine, string dbCom)
{

	meas->setHeaderComment(headCommentLine);// assign the comment to the point on the next line
	headCommentLine="";// "clear" the comment line storage
	meas->setComment(dbCom);
	return;
}


void	TObservationConverter::setId(TAMeasurement* meas, int ID, int&)
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
												string&,
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
#endif
///////////////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////////////


