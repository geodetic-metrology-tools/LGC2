////////////////////////////////////////////////////////////////////
// TAConverter.cpp
/*!
Write Observations for an output LGC file
sends the appropriate messages

Patterns:

 
Copyright 2003 CERN EST/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TAConverter.h"
#include	"TAStreamFormatter.h"

/////////////////////////////////////////////////////////////////////

//ClassImp(TAConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

inline bool isDelim(const char c, const char* delims, int ndelims) {
	for (int i = 0; i < ndelims; i++) {
		if (c == delims[i]) return true;
	}
	return false;
}


const vector<string> TAConverter::tokenizeString(const string& str, const char* delims) {
	enum {
		STATE_TOKEN,
		STATE_DELIM
	};

	size_t delimlen(strlen(delims));
	vector<string> result(0);
	
	size_t start(0);
	size_t end(0);
	int state(STATE_DELIM);

	do {
		switch (state) {
			case STATE_DELIM:
				if (! isDelim(str[end], delims, delimlen+1)) {
					start = end;
					state = STATE_TOKEN;
				}
			break;
			case STATE_TOKEN:
				if (isDelim(str[end], delims, delimlen+1)) {
					result.push_back(std::move(str.substr(start, end-start)));
					state = STATE_DELIM;
				}
			break;
		}
		end++;
	} while (str[end-1] != 0);

	return result;
}

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TAConverter::TAConverter()
{//default constructor
	fStream = 0;
}

TAConverter::TAConverter(TAStreamFormatter& stream)
{
	fStream = &stream;
	fNameWidth = fStream->getPointFormat()->getNameWidth();
	fObsWidth = fStream->getObsFormat()->getObsWidth();
	fObsResWidth = fStream->getObsFormat()->getObsResidualWidth();
	fAnglePrecision = fStream->getObsFormat()->getAnglePrecision();
	fAngleResidualPrecision = fStream->getObsFormat()->getAngleResidualPrecision();
	fLengthPrecision = fStream->getObsFormat()->getLengthPrecision();
	fLengthResidualPrecision = fStream->getObsFormat()->getLengthResidualPrecision();
	fSeparator = fStream->getSeparator();
	fCoordWidth = fStream->getPointFormat()->getCoordWidth();
	fCoordResWidth= fStream->getPointFormat()->getCoordErrorWidth();
	fCoordPrecision= fStream->getPointFormat()->getCoordPrecision();
	fCoordErrorPrecision = fStream->getPointFormat()->getCoordErrorPrecision();
}

TAConverter::TAConverter(TAStreamFormatter* stream)
{
	fStream = stream;
	fNameWidth = fStream->getPointFormat()->getNameWidth();
	fObsWidth = fStream->getObsFormat()->getObsWidth();
	fObsResWidth = fStream->getObsFormat()->getObsResidualWidth();
	fAnglePrecision = fStream->getObsFormat()->getAnglePrecision();
	fAngleResidualPrecision = fStream->getObsFormat()->getAngleResidualPrecision();
	fLengthPrecision = fStream->getObsFormat()->getLengthPrecision();
	fLengthResidualPrecision = fStream->getObsFormat()->getLengthResidualPrecision();
	fSeparator = fStream->getSeparator();
	fCoordWidth = fStream->getPointFormat()->getCoordWidth();
	fCoordResWidth= fStream->getPointFormat()->getCoordErrorWidth();
	fCoordPrecision= fStream->getPointFormat()->getCoordPrecision();
	fCoordErrorPrecision = fStream->getPointFormat()->getCoordErrorPrecision();
}


TAConverter::~TAConverter()
{//destructor
}


		
TAConverter& 	TAConverter::operator=(const TAConverter& source)
{//! copy assignment operator
	fStream  = source.fStream;
	fNameWidth = source.fNameWidth;
	fObsWidth = source.fObsWidth;
	fObsResWidth = source.fObsResWidth;
	fAnglePrecision = source.fAnglePrecision;
	fAngleResidualPrecision = source.fAngleResidualPrecision;
	fLengthPrecision = source.fLengthPrecision;
	fLengthResidualPrecision = source.fLengthResidualPrecision;
	fSeparator = source.fSeparator;
	fCoordWidth = source.fCoordWidth;
	fCoordResWidth= source.fCoordResWidth;
	fCoordPrecision= source.fCoordPrecision;
	fCoordErrorPrecision= source.fCoordErrorPrecision;

	return *this;
}



void	TAConverter::setStream(TAStreamFormatter* stream) 
{
	fStream = stream;
	fNameWidth = fStream->getPointFormat()->getNameWidth();
	fObsWidth = fStream->getObsFormat()->getObsWidth();
	fObsResWidth = fStream->getObsFormat()->getObsResidualWidth();
	fAnglePrecision = fStream->getObsFormat()->getAnglePrecision();
	fAngleResidualPrecision = fStream->getObsFormat()->getAngleResidualPrecision();
	fLengthPrecision = fStream->getObsFormat()->getLengthPrecision();
	fLengthResidualPrecision = fStream->getObsFormat()->getLengthResidualPrecision();
	fSeparator = fStream->getSeparator();
	fCoordWidth = fStream->getPointFormat()->getCoordWidth();
	fCoordResWidth= fStream->getPointFormat()->getCoordErrorWidth();
	fCoordPrecision= fStream->getPointFormat()->getCoordPrecision();
	fCoordErrorPrecision = fStream->getPointFormat()->getCoordErrorPrecision();
	return;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTECTED MEMBER FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void	TAConverter::writeString(const int width, const string data)
{
	//(*fStream)<<" ";
	fStream->width(width);
	(*fStream)<<right<<data;
	return;
}


void	TAConverter::writeStringSep(const int width, const string data)
{
	//writeString(1, "");
	writeString(width, data);
	(*fStream)<<fSeparator;
	return;
}

//void	TAConverter::writeStringNoSpaceSep(const int width, const string 


void	TAConverter::writeStringLeft(const int width, const string data)
{
	fStream->width(width);
	(*fStream)<<left<<data<<right;
	return;
}


void	TAConverter::writeStringLeftSep(const int width, const string data)
{
	writeStringLeft(width, data);
	(*fStream)<<fSeparator;
	return;
}

void	TAConverter::writeDouble(const int width, const int pres, const TReal data)
{
	fStream->width(width);
	fStream->precision(pres);
	(*fStream)<<right<<data;
	return;
}


void	TAConverter::writeDoubleSep(const int width, const int pres, const TReal data)
{
	writeDouble(width, pres, data);
	(*fStream)<<fSeparator;
	return;
}

void	TAConverter::writeInteger(const int width, const int data)
{
	fStream->width(width);
	(*fStream)<<right<<data;
	return;
}

void	TAConverter::writeLength(const int width, const int pres, const TLength::EUnits unit, const TLength data)
{
	fStream->setLengthUnits(unit);
	fStream->setWidthFormat(width);
	fStream->setPrecisionFormat(pres);
	(*fStream)<<(data);
	return;
}

void	TAConverter::writeAngle(const int width, const int pres, const TAngle::EUnits unit, const TAngle data)
{
	fStream->setAngleUnits(unit);
	fStream->setWidthFormat(width);
	fStream->setPrecisionFormat(pres);
	(*fStream)<<(data);
	return;
}

void	TAConverter::readP100Comment(string comments)
{

	//end line comments
	fStream->readChar(); //read the char %

	string comment = "";
	fStream->skipWhiteSpace();
	while(fStream->peek() != EOF && fStream->peek() != '\n')
	{
		fStream->TAStreamFormatter::operator>>(comment);
		comments += comment+ " ";
		fStream->skipWhiteSpace();
	}	

	return;
}


string TAConverter::readOptional(const string& keyword)
{
	fStream->skipWhiteSpace();

	if (fStream->peek() != '\n')
	{
		string word;
		*fStream >> word;
		
		if (word != keyword)
		{
			fStream->seekg(-static_cast<std::streamoff>(word.length()), ios_base::cur);
			return "";
		}

		*fStream >> word;
		return word;
	}
	return "";
}

bool TAConverter::readOptionalExists(const string& keyword)
{
	fStream->skipWhiteSpace();

	if (fStream->peek() != '\n')
	{
		string word;
		*fStream >> word;
		
		if (word == keyword)
		{
			return true;
		}

		fStream->seekg(-static_cast<std::streamoff>(word.length()), ios_base::cur);
	}
	return false;
}



///////////////////////////////////////////////////////////////////////////////////////////////
//END
///////////////////////////////////////////////////////////////////////////////////////////////


