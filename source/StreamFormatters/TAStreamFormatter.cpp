// TAStreamFormatter.cpp: 
//
// Abstract Base Class for a QTextStream Formatter.
// Enables the output of all the standard survey objects.
//
//
// Patterns:
// Maintains a reference to the C++ Standard Library 
// the decorator pattern.
// 
//
// Copyright 2002-2008, M. Jones  CERN, TS/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations

#if _MSC_VER
#include <float.h>
#else
#include <cmath>
#endif

#include	"TADataSet.h"
#include	"TFileParameters.h"
#include	"TDataParameters.h"

#include	"TSpatialPositionFilter.h"
#include	"T3DCartesianFilter.h"
#include	"TGeodeticFilter.h"
#include	"T2DPlusHFilter.h"

#include	"TAngleFilter.h"
#include	"TDMSFilter.h"
#include	"TGonsFilter.h"
#include	"T100MicroGonsFilter.h"
#include	"TCCsFilter.h"
#include	"TRadiansFilter.h"

#include	"TLengthFilter.h"
#include	"TMillimetresFilter.h"
#include	"TMetresFilter.h"
#include	"TKilometresFilter.h"

#include	"TSpatialPoint.h"
#include	"TSpatialPointName.h"
#include	"TSpatialPosition.h"
#include	"TSpatialLineName.h"

#include	"TAStreamFormatter.h"


static inline bool isfiniteq(TReal v) {
#if _MSC_VER
	return _finite(v) != 0;
#else
	return isfinite(v);
#endif
}

//////////////////////////////////////////////////////////////////////
//ClassImp(TAStreamFormatter)




//////////////////////////////////////////////////////////////////////
//Constructor/Destructor
//////////////////////////////////////////////////////////////////////
TAStreamFormatter::TAStreamFormatter(TDataParameters& dp)
{//insertion of a string into a stream, writing in a stream

	//INSURE
	init();

	fName = "defaultName";
	fFStream=0;

	fSStream = new stringstream(ios_base::out);
	fIOStream = fSStream;

	setAngleUnits(dp.getAngleUnits());
	setLengthUnits(dp.getLengthUnits());
	setCoordSys(dp.getCoordinateSystem());
	fRefFrame = 0;
	fLineGap = true;
	fNonSpaceSeparator = "";
	fSeparator = "";
}


TAStreamFormatter::TAStreamFormatter(const string& input, TDataParameters& dp)
{//extraction of a string from a stream, reading of a stream
	init();
	
	fName = "defaultName";
	fFStream=0;

	fSStream = new stringstream(input, ios_base::in);
	fIOStream = fSStream;

	setAngleUnits(dp.getAngleUnits());
	setLengthUnits(dp.getLengthUnits());
	setCoordSys(dp.getCoordinateSystem());
	fRefFrame = 0;
	fLineGap = true;
	fNonSpaceSeparator = "";
	fSeparator = "";
}


/*!Constructors
\param io: read or write
\param fp: data file's parameters (name, path ...) 
\param ds: data parameters (unit, precision, refernce system, coord sys...)*/ 
TAStreamFormatter::TAStreamFormatter(EIOType io, TFileParameters& fp, TDataParameters& dp)
{
	init();
	fName= fp.getFileName();
	fSStream=0;
	fIOType =io;
	if (fIOType == kRead)
	{// extraction of a file from a stream, reading of a stream
		fFStream = new fstream(fName.c_str(), ios_base::in);
	}
	else if(fIOType == kWrite)
	{// insertion of a file into a stream, writing a stream
		fFStream = new fstream(fName.c_str(), ios_base::out);
	}

	if(fFStream->fail())
	{
		if(fIOType == kRead)
		{
			this->setError("No input file to open, check the path");
		}
		else
		{
			this->setError("Failed to open output file, check the path");
		}
		fFStream->clear();
	}

	fIOStream = fFStream;

	setAngleUnits(dp.getAngleUnits());
	setLengthUnits(dp.getLengthUnits());
	setCoordSys(dp.getCoordinateSystem());
	fRefFrame = dp.getRefFrame();
	fLineGap = true;
	fNonSpaceSeparator = "";
	fSeparator = "";
}


TAStreamFormatter::TAStreamFormatter(EIOType io, TADataSet& ds)
{

	init();
	fName=ds.getFileName();
	fSStream=0;
	fIOType =io;
	if (fIOType == kRead)
	{// extraction of a file from a stream, reading of a stream
		fFStream = new fstream(fName.c_str(), ios_base::in);
	}
	else if(fIOType == kWrite)
	{// insertion of a file into a stream, writing a stream
		fFStream = new fstream(fName.c_str(), ios_base::out);
	}

	if(fFStream->fail())
	{
		if(fIOType == kRead)
		{
			this->setError("No input file to open, check the path");
		}
		else
		{
			this->setError("No output file to open, check the path");
		}
		fFStream->clear();
	}

	fIOStream = fFStream;

	setAngleUnits(ds.getAngleUnits());
	setLengthUnits(ds.getLengthUnits());
	setCoordSys(ds.getCoordinateSystem());
	fRefFrame = 0;
	fLineGap = true;
	fNonSpaceSeparator = "";
	fSeparator = "";
}


TAStreamFormatter::TAStreamFormatter(EIOType io, TADataSet& ds, TPointFormat& pf)
{
	init();
	fName=ds.getFileName();
	fSStream=0;
	fIOType =io;
	if (fIOType == kRead)
	{// extraction of a file from a stream, reading of a stream
		fFStream = new fstream(fName.c_str(), ios_base::in);
	}
	else if(fIOType == kWrite)
	{// insertion of a file into a stream, writing a stream
		fFStream = new fstream(fName.c_str(), ios_base::out);
		fPointFormat =pf;
		setPrecisionFormat(fPointFormat.getCoordPrecision());
		setWidthFormat(fPointFormat.getCoordWidth());
	}
	

	if(fFStream->fail())
	{
		if(fIOType == kRead)
		{
			this->setError("No input file to open, check the path");
		}
		else
		{
			this->setError("No output file to open, check the path");
		}
		fFStream->clear();
	}

	fIOStream = fFStream;
	
	setAngleUnits(ds.getAngleUnits());
	setLengthUnits(ds.getLengthUnits());
	setCoordSys(ds.getCoordinateSystem());
	fRefFrame = 0;
	fLineGap = true;
	fNonSpaceSeparator = "";
	fSeparator = "";
}


TAStreamFormatter::TAStreamFormatter(const EIOType io, TADataSet& ds, const TPointFormat& pf, const TObservationFormat& obsFor)
{
	init();
	fName=ds.getFileName();
	fSStream=0;
	fIOType =io;
	if (fIOType == kRead)
	{// extraction of a file from a stream, reading of a stream
		fFStream = new fstream(fName.c_str(), ios_base::in);
	}
	else if(fIOType == kWrite)
	{// insertion of a file into a stream, writing a stream
		fFStream = new fstream(fName.c_str(), ios_base::out);
		fPointFormat =pf;
		fObservationFormat = obsFor;
		setPrecisionFormat(fPointFormat.getCoordPrecision());
		setWidthFormat(fPointFormat.getCoordWidth());

	}
	

	if(fFStream->fail())
	{
		if(fIOType == kRead)
		{
			this->setError("No input file to open, check the path");
		}
		else
		{
			this->setError("No output file to open, check the path");
		}
		fFStream->clear();
	}

	fIOStream = fFStream;
	
	setAngleUnits(ds.getAngleUnits());
	setLengthUnits(ds.getLengthUnits());
	setCoordSys(ds.getCoordinateSystem());
	fRefFrame = 0;
	fLineGap = true;
	fNonSpaceSeparator = "";
	fSeparator = "";
}



TAStreamFormatter::~TAStreamFormatter()
{//Destructor
delete fIOStream;
}

void TAStreamFormatter::init()
{
	//INSURE
	fAngles = TAngle::kGons;
	fLengths = TLength::kMetres;
	fCoordSys = TCoordSysFactory::k3DCartesian;
	fPrecision = 6;
	fWidth = 13;
	

	fSStream = 0;//stringstream*
	fFStream = 0;//fstream*
	fIOStream = 0;//iostream*
	fName ="";//const char*	
	fError="";//string	

	fAngFilter =0;//TAngleFilter*
	fLenFilter=0;//	TLengthFilter*	
	fPosFilter = 0;//TSpatialPositionFilter*
	fRefFrame=0;//TVReferenceFrame*
	fPointFormat = TPointFormat();
	fObservationFormat = TObservationFormat();
	fNofSpaces = 0; // LEVEL 
}

//////////////////////////////////////////////////////////////////////
// Member Functions : set
//////////////////////////////////////////////////////////////////////
TAngle::EUnits TAStreamFormatter::setAngleUnits(const TAngle::EUnits units )
{//set the angle units and get the associated filter
	
	TAngle::EUnits oldAng;// = TAngle::kGons;
	// change the angle filter if different to the one already defined
	if ( fAngles != units )
	{
		oldAng = fAngles; 
		fAngles = units; 
		//fAngFilter = getAngleFilter(units);
	}
	else
	{
		oldAng = units;
	}
	// Assertion terminates the program if an unknown filter is requested
	fAngFilter = getAngleFilter(units); 
	return oldAng; 
}


TLength::EUnits TAStreamFormatter::setLengthUnits(const TLength::EUnits units )
{//set the length units and get the associated filter

	TLength::EUnits oldLen; //= TLength::kMetres; 
	// change the length filter if different to the one already defined
	if( fLengths != units )
	{
		oldLen = fLengths; 
		fLengths = units; 
	//	fLenFilter = getLengthFilter(units);
	}
	else
	{
		oldLen = units;
		
	}

	fLenFilter = getLengthFilter(units); //suite INSURE

	return oldLen; 
}


TCoordSysFactory::ECoordSys  TAStreamFormatter::setCoordSys(const TCoordSysFactory::ECoordSys coorSys)
{//set the coordinate system and get the associated filter

	TCoordSysFactory::ECoordSys  oldSys;// = TCoordSysFactory::k3DCartesian; 
	// change the position filter if different to the one already defined
	if( fCoordSys != coorSys )
	{
		oldSys = fCoordSys; 
		fCoordSys = coorSys; 
	//	setPosFilter(coorSys);
	}
	else
	{
		oldSys = coorSys;
	}
	setPosFilter(coorSys);//INSURE
	return oldSys; 
}


TAReferenceFrame*  TAStreamFormatter::setReferenceFrame(TAReferenceFrame* frame )
{//set the reference frame
	TAReferenceFrame* oldFrame = fRefFrame; 
	fRefFrame = frame; 
	return oldFrame;
}


TSpatialPositionFilter*	TAStreamFormatter::setPosFilter(const TCoordSysFactory::ECoordSys cs)
{
	TSpatialPositionFilter* oldPos = fPosFilter;
	
	switch(cs)
	{
	case TCoordSysFactory::k3DCartesian:
		fPosFilter= T3DCartesianFilter::instance();
		break;
	case TCoordSysFactory::kGeodetic:
		fPosFilter= TGeodeticFilter::instance();
		break;
	case TCoordSysFactory::k2DPlusH:
		fPosFilter= T2DPlusHFilter::instance();
		break;
	default:
		// undefined position filter requested
		assert( fPosFilter != 0 );
		break;
	}
	return oldPos;

}

int	TAStreamFormatter::setWidthFormat(const int width)
{/*!set a width for stream*/
	int oldWidth = fWidth;
	fWidth = width;
	return oldWidth;
}

	
int	TAStreamFormatter::setPrecisionFormat(const int precision)
{/*!set a precision for stream*/
	int oldPrecision = fPrecision;
	fPrecision = precision;
	return oldPrecision;
}

	
TSpatialStatus::ESpatialStatus	TAStreamFormatter::setPointFormatStatus(const TSpatialStatus::ESpatialStatus status)
{/*!set a spatial status for TPointFormat*/
	TSpatialStatus::ESpatialStatus oldStatus = fPointFormat.getSpatialStatus();
	fPointFormat.setSpatialStatus(status);
	return oldStatus;
}



void TAStreamFormatter::setPointFormat(const TPointFormat ptFormat)
{fPointFormat = ptFormat;}


void TAStreamFormatter::setObsFormat(const TObservationFormat obsFormat)
{fObservationFormat = obsFormat;}



/////////////////////////////////////////////////////////////////////////////////////
//Menber Function get
/////////////////////////////////////////////////////////////////////////////////////
TAngle::EUnits TAStreamFormatter::getAngleUnits() const
{return fAngles;}


TLength::EUnits TAStreamFormatter::getLengthUnits() const
{return fLengths;}


TCoordSysFactory::ECoordSys	TAStreamFormatter::getCoordSys() const
{return fCoordSys;}


TAReferenceFrame*  TAStreamFormatter::getReferenceFrame() const
{return fRefFrame;}


TSpatialStatus::ESpatialStatus	TAStreamFormatter::getPointFormatStatus() const
{/*!get the TPointFormat's spatial status for */
	return fPointFormat.getSpatialStatus();
}


	
int	TAStreamFormatter::getWidthFormat() const
{/*!get the stream's width*/
	return fWidth;
}

	
int	TAStreamFormatter::getPrecisionFormat() const
{/*!get the stream's precision*/
	return fPrecision;
}


TPointFormat* TAStreamFormatter::getPointFormat()
{return &fPointFormat;}


TObservationFormat* TAStreamFormatter::getObsFormat()
{return &fObservationFormat;}




/////////////////////////////////////////////////////////////////////////////////
//Input spatial object Operator
/////////////////////////////////////////////////////////////////////////////////
TAStreamFormatter&	TAStreamFormatter::operator>>( TAngle &angle )
{//input an angle from the text stream

	// pass on the request to the angles filter Strategy
	fAngFilter->input(*this, angle);
	return *this;
}


TAStreamFormatter&	TAStreamFormatter::operator>>( TLength &length )
{//input a length from the text stream
	
	// pass on the request to the lengths filter Strategy
	fLenFilter->input(*this, length);
	return *this;
}


TAStreamFormatter  &TAStreamFormatter::operator>>( TSpatialPointName &ptName )
{//input a spatial point name from the text stream
	
	//char  cStr[20];
	string  stringName = "";

	this->skipWhiteSpace();
	if(this->peek()!='\n')
	{
	// get the name of the point as a single string from the text stream
	// and set the spatial point name
	this->TAStreamFormatter::operator>>(stringName);
	ptName.setName(stringName );
	}

	return *this;
}


TAStreamFormatter  &TAStreamFormatter::operator>>( TSpatialPosition &position )
{//input a spatial position from the text stream
	
	// pass on the request to the coordinate system filter Strategy
	fPosFilter->input(*this, position);

	return *this;
}


TAStreamFormatter  &TAStreamFormatter::operator>>(TSpatialPoint&)
{//Default function
	return *this;
}



/////////////////////////////////////////////////////////////////////////////////
//Output  spatial object Operator
/////////////////////////////////////////////////////////////////////////////////
TAStreamFormatter  &TAStreamFormatter::operator<<( const TAngle &angle )
{//output an angle to the text stream	

	// pass on the request to the angles filter Strategy
	fAngFilter->output(*this, angle);
	return *this;
}


TAStreamFormatter  &TAStreamFormatter::operator<<( const TLength &length )
{//output a length to the text stream	
	
	// pass on the request to the lengths filter Strategy
	fLenFilter->output(*this, length);
	return *this;
}


TAStreamFormatter  &TAStreamFormatter::operator<<( const TSpatialPointName &ptName )
{//output a spatial point name to the text stream	

	// extract the name of the point as a single string and output to the text stream
	this->width(fPointFormat.getNameWidth());
	(*this)<<(right);

	(*this)<<(ptName.getName());

	return *this;
}


TAStreamFormatter&	TAStreamFormatter::operator<<(const TSpatialPosition& SpPos)
{//Default function
	ignoring(SpPos);
	return *this;
}


TAStreamFormatter&	TAStreamFormatter::operator<<(const TPositionVector& pos)
{//Default function	
	ignoring(pos);
	return *this;
}


TAStreamFormatter  &TAStreamFormatter::operator<<( const TDouble& db )
{//output a TReal object to the text stream	

	// extract the TReal object as a TReal value and output to the text stream
	this->width(fWidth);
	this->precision(fPrecision);
	(*this)<<right;
	if(db.getStatus() != TVNumericValue::kNull)
	{
		TReal d = db.getValue();
		(*this)<<d;
	}

	return *this;
}

#if 0
TAStreamFormatter  &TAStreamFormatter::operator<<( const std::string& string )
{//output a TReal object to the text stream	

	// extract the TReal object as a TReal value and output to the text stream
	this->width(fWidth);
	(*this)<<right;

	(*this)<<string;

	return *this;
}
#endif

TAStreamFormatter  &TAStreamFormatter::operator<<( const TScalar& db )
{//output a scalar object to the text stream	

	// extract the scalar object as a TReal value and output to the text stream
	this->width(fWidth);
	this->precision(fPrecision);
	(*this)<<right;
	if(db.getStatus() != TVNumericValue::kNull)
	{
		TReal d = db.getValue();
		(*this)<<d;
	}

	return *this;
}

TAStreamFormatter&	TAStreamFormatter::operator<<(const TFreeVector& pos)
{//Default function
	ignoring(pos);
	return *this;
}


TAStreamFormatter&	TAStreamFormatter::operator<<(const TSpatialPoint& SpPoint)
{//Default function
	ignoring(SpPoint);
	return *this;
}


TAStreamFormatter& TAStreamFormatter::operator<<(const TPointFormat& ptFormat)
{//sets the stream's point format
	fPointFormat = ptFormat;
	return *this;
}


TAStreamFormatter& TAStreamFormatter::operator<<(const TObservationFormat& obsFormat)
{//sets the stream's observation format
	fObservationFormat = obsFormat;
	return *this;
}


//////////////////////////////////////////////////////////////////////
// Overloaded input operator
//////////////////////////////////////////////////////////////////////
TAStreamFormatter &TAStreamFormatter::operator>>( signed short &i )
{   (*fIOStream)>>( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>( unsigned short &i )
{   (*fIOStream)>>( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>( signed int &i )
{   (*fIOStream)>>( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>( unsigned int &i )
{   (*fIOStream)>>( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>( signed long &i )
{   (*fIOStream)>>( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>( unsigned long &i )
{   (*fIOStream)>>( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>( float &f )
{   (*fIOStream)>>( f ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>( TReal &d )
{
#if __INTEL_COMPILER && USE_QUAD

	string str;
	(*fIOStream) >> str;
	
	_Quad wholePart = 0, fractionPart = 0;
	int strLen = str.length();

	int sign = 0, i = 0;

	if (str[0] == '-')
	{
		sign = -1;
		i++;
	}
	else
	{
		sign = 1;
	}

	while (str[i] != '.' && i < strLen)
	{
		wholePart *= 10;
		wholePart += str[i++] - '0';
	}
	if (i != strLen)
	{
		i++;
	}
	int len = strLen - i;
	while (i < strLen)
	{
		fractionPart *= 10;
		fractionPart += str[i++] - '0';
	}
	_Quad fraction = fractionPart / powq(10.0q, len);
	d = sign * (wholePart + fraction);

#else

	(*fIOStream) >> d;

#endif
	return *this;
}


TAStreamFormatter &TAStreamFormatter::operator>>( char *s )
{   (*fIOStream)>> s ; return *this; }


TAStreamFormatter &TAStreamFormatter::operator>>(string& str)
{
(*fIOStream) >> str;
return *this;
}


TAStreamFormatter &TAStreamFormatter::operator>>( TSFFUNC func )
{	(*func)( *(this) ); return *this; }



//////////////////////////////////////////////////////////////////////
//Overloaded output operator
//////////////////////////////////////////////////////////////////////
TAStreamFormatter &TAStreamFormatter::operator<<( const char c )
{   (*fIOStream)<<( c ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( signed short i )
{   (*fIOStream)<<( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( unsigned short i )
{   (*fIOStream)<<( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( signed int i )
{   (*fIOStream)<<( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( unsigned int i )
{   fIOStream->operator<<( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( signed long i )
{   (*fIOStream)<<( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( unsigned long i )
{   (*fIOStream)<<( i ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( float f )
{   (*fIOStream)<<( f ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( TReal d )
{   
	string s("IND");
	if((size_t)fWidth > s.size())
    	s.insert(0, fWidth - s.size(), ' ');

	if (isfiniteq(d))
		(*fIOStream)<<( (double) d );
	else
		(*fIOStream)<<( s );
	 return *this; 
}


TAStreamFormatter &TAStreamFormatter::operator<<( const char *s )
{   (*fIOStream)<<( s ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( const string &str )
{   
	(*fIOStream)<<( str ); 
	return *this; 
}


TAStreamFormatter &TAStreamFormatter::operator<<( void *v )
{   (*fIOStream)<<( v ); return *this; }


TAStreamFormatter &TAStreamFormatter::operator<<( TSFFUNC func )
{	(*func)( *(this) ); 
return *this; }




string operator*(const string& s, unsigned int n) {
        stringstream out;
		while (n--)
			out << s;
		return out.str();
	}

string operator*(unsigned int n, const string& s) { return s * n; }

//////////////////////////////////////////////////////////////////////
//Member Function
/////////////////////////////////////////////////////////////////////
fstream*	TAStreamFormatter::device() const
{
return fFStream;
}

/*comentaireSTL
inline void TAStreamFormatter::setDevice(QIODevice *device)
{	fStream->setDevice(device); return; }

inline void TAStreamFormatter::unsetDevice()
{	fStream->unsetDevice(); return; }
*/

/*
inline void TAStreamFormatter::setEncoding(TAStreamFormatter::Encoding e)
{
//fStream->setEncoding( e );
	locale loc (std::locale(e));
	//fIOStream->locale::global(e);
return;
}

*/
/*
inline void TAStreamFormatter::setCodec( QTextCodec *codec )
{	fStream->setCodec( codec ); return; }
*/


bool TAStreamFormatter::atEnd()
{//The atEnd method returns true if the end of file was hit 
this->skipCommentLines();
return fIOStream->eof();
}


bool TAStreamFormatter::atEndCom()
{//The atEnd method returns true if the end of file was hit 
return fIOStream->eof();
}

TAStreamFormatter& TAStreamFormatter::seekg(streamoff off, ios_base::seekdir dir)
{
	fIOStream->seekg(off, dir);
	return *this;
}

std::streampos TAStreamFormatter::tellg()
{
	return fIOStream->tellg();
}

string TAStreamFormatter::readLine() 
{/*
  Reads a line from the stream and returns a string containing the text.
  The returned string does not contain any trailing newline or carriage
  return.
  On EOF you will get a string that is null. On reading an empty line the
  returned string is empty but not null.
*/

	
	
	string result ="";
	const int buf_size=256;
	char c[buf_size];
	int pos=0;
	//int EndChar = fIOStream->eof();

////////////////////////////////////

	c[pos]=(char) fIOStream->get();

	if(c[pos]==EOF)
	{//check if there is a line
		result = "end of file";
	}
	else
	{		
		while( c[pos]!=EOF && (c[pos]!=*("\n")) && pos < buf_size)
		{//read character by character
			result +=c[pos];
			pos++;
			c[pos]=(char) fIOStream->get();
		}
		//substract the last char (eof or \n)
	}
		

	//////////////////////////////////////////////////////////////////////////

/*
	if(c[pos]==EOF)
	{//check if there is a line
		result = "end of file";
	}

	else
		{
			//while(c[pos] != EndChar && (c[pos]!=*("\n")))
			while(!(fIOStream->eof()) && (c[pos]!=*("\n")))
			{//read character by character
				pos++;
				if (pos>=buf_size)
				{//!return the line even if it's not the end when pos>=256
					result+=string(c,pos);
					//pos=0;
					c[pos]=*("\n");
				}
				c[pos]=fIOStream->get();
				result +=c[pos];
			}
		//result +=string(c, pos);
	result=result.substr(0,pos-1); //substract the last char (eof or \n)
		}
*/


	return result;


}


/*fonction non utilisee?
inline string TAStreamFormatter::read() 
{}
*/

char TAStreamFormatter::readChar()
{return (char) fIOStream->get();
}

void TAStreamFormatter::skipWhiteSpace() 
{//positionthe read pointer at the first non-whitespace character

	char c=this->peek();

	if (c == ' ' || c == '\t')
	{
		//skips whitespace until c = first non-whitespace character

		//while (c!=EOF && isspace(c)) // DO NOT USE ISSPACE: TREATS '\n' as space!
		while((c==' '||c== '\t') && c!=EOF)
		{
			c=(char) fIOStream->get();
		}
		fIOStream->putback(c);
	}
	return;
}


void TAStreamFormatter::skipBOM() {
	static const int NUM_BOMs(3);
	static const int BOM_LEN(3);

	// all widespread byte order marks
	static const char boms[NUM_BOMs][BOM_LEN] = {
		{239, 187, 191},
		{254, 255,   0},
		{255, 254,   0}
	};
	
	char y;
	for (int b=0; b < NUM_BOMs; b++) {
		for (int c=0; c < BOM_LEN; c++) {
			char x = peek();
			// if the current character is a BOM character: remove it.
			if (x == boms[b][c])
				get(y);
		}	
	}
}

TAStreamFormatter&	TAStreamFormatter::get(char& c)
{//reads and returns a character from the stream and puts it back
	fIOStream->get(c);
	return (*this);
}


char	TAStreamFormatter::peek()
{//reads and returns a character from the stream and puts it back
return (char) fIOStream->peek();
}


TAStreamFormatter &TAStreamFormatter::readRawBytes(char *s, streamsize n)
{//extracts up to n elements and stores them in the array beginning at s
	fIOStream->read(s, n);
	return *this;
}

bool TAStreamFormatter::fail() const
{//return true if an error has occured (failbit or badbit is set)
return  fIOStream->fail();
}

void TAStreamFormatter::clear() const
{//return true if an error has occured (failbit or badbit is set)
	fIOStream->clear(fIOStream->rdstate() & ~std::ios::failbit);
	return;
}


int TAStreamFormatter::flags() const
{//returns all set format flags
return fIOStream->flags();
}


int TAStreamFormatter::flags( int f )
{//sets flags as the new format flags and returns the prvious state of all flags
return fIOStream->flags( ios_base::fmtflags(f) );
}


int TAStreamFormatter::setf( int bits )
{//sets flags "bits" as additional format flags and return the previous state of all flags
return fIOStream->setf( ios_base::fmtflags(bits) );
}


int TAStreamFormatter::setf( int bits, int mask )
{//sets flags as the new format flags of the group identified by mask and returns the previous state of all flags
return fIOStream->setf( ios_base::fmtflags(bits), ios_base::fmtflags(mask) );
}


void TAStreamFormatter::unsetf( int bits )
{//clears flags
fIOStream->unsetf( ios_base::fmtflags(bits) );
return;
}


void TAStreamFormatter::reset()
{
	const char* c=" ";

	fIOStream->unsetf(fIOStream->flags());
	fIOStream->width(0);
	fIOStream->fill(*c);
	fIOStream->precision(6);
	return;
}


 int TAStreamFormatter::width() const
{//Returns the field width
	return (int)fIOStream->width();
}


 int  TAStreamFormatter::width(int w)
{//sets the field width to w and returns the previous field width
	return (int)fIOStream->width(w);
}


int TAStreamFormatter::fill() const
{//return the current fill character
return fIOStream->fill();
}


int TAStreamFormatter::fill(int f)
{//defines f as the fill character and returns the previous fill character
return fIOStream->fill((char)f);
}


int TAStreamFormatter::precision() const
{//return the current precision of floating-point values
	return (int)fIOStream->precision();
}

int TAStreamFormatter::precision(int p)
{//sets p as the new precision of floating-point values and returns the old
	(*fIOStream)<<std::fixed; /*modif du 28/10/03 pour chaba*/
	return (int)fIOStream->precision(p);
}


TAStreamFormatter &TAStreamFormatter::writeRawBytes(const char* s, streamsize n)
{//inserts the sequence of n elements beginning at s
	fIOStream->write(s, n);
	return *this;
}


const string&	TAStreamFormatter::getError() const
{
	return fError;
}


void TAStreamFormatter::setError(const string error)
{
	fError = error;
	return;
}


string	TAStreamFormatter::getWarning() const
{
	return fWarning;
}


void	TAStreamFormatter::initWarning()
{
	fWarning = "";
	return;
}


void TAStreamFormatter::addWarning(const string warning)
{
	if (fWarning != "")
		fWarning += "\n                    ";
	fWarning += warning;
	return;
}

void  TAStreamFormatter::setNoObsToRead()
{return;}

bool TAStreamFormatter::hasNoObsToRead() const
{return true;}




////////////////////////////////////////////////////////////////////////
//Default Function
////////////////////////////////////////////////////////////////////////
string TAStreamFormatter::separator() const
{//default
	string sep("default");
	return sep;
}


void  TAStreamFormatter::skipCommentLines()
{//default	
	return;
}


//////////////////////////////////////////////////////////////////////
//Open & close Function
//////////////////////////////////////////////////////////////////////
void TAStreamFormatter::openIn()
{
	if (fIOType != kRead && fSStream==0)
	{fFStream ->open(fName.c_str());}
return;
}


void TAStreamFormatter::openOutApp()
{
	if (fIOType != kWrite && fSStream==0)
	{fFStream->open(fName.c_str(), ios::out| ios::app);}
return;
}


bool TAStreamFormatter::isOpen()
{
	if (fSStream==0)
	{return	fFStream->is_open();}
	else
	{return false;}
}


void TAStreamFormatter::close()
{
	if (fSStream==0)
	{	fFStream->close();}
return;
}


//////////////////////////////////////////////////////////////////////
//Manipulators
//////////////////////////////////////////////////////////////////////


TAStreamFormatter &bin( TAStreamFormatter &s )
{
    s.setf(ios::binary /*,ios::basefield*/);
    return s;
}


TAStreamFormatter &oct(TAStreamFormatter &s)
{
    s.setf(ios::oct,ios::basefield);
    return s;
}

TAStreamFormatter &dec(TAStreamFormatter &s)
{
    s.setf(ios::dec,ios::basefield);
    return s;
}

TAStreamFormatter &hex(TAStreamFormatter &s)
{
    s.setf(ios::hex,ios::basefield);
    return s;
}

TAStreamFormatter &endl(TAStreamFormatter &s)
{
	s.getIOStream().put('\n');
	s.getIOStream().flush();
	return s;

}


TAStreamFormatter &flush(TAStreamFormatter &s)
{
    if (s.device())
	s.device()->flush();
    return s;
}


TAStreamFormatter &ws(TAStreamFormatter &s)
{
    //s.skipWhiteSpace();
	s.setf(ios::skipws/*,ios::basefield*/);
    return s;
}


TAStreamFormatter &reset(TAStreamFormatter &s)
{
s.reset();
return s;
}


TAStreamFormatter &left(TAStreamFormatter &s)
{
	s.setf(ios::left,ios::adjustfield);
	return s;
}


TAStreamFormatter &right(TAStreamFormatter &s)
{
	s.setf(ios::right,ios::adjustfield);
	return s;
}

//////////////////////////////////////////////////////////////////////
//Protected
//////////////////////////////////////////////////////////////////////
TAngleFilter *TAStreamFormatter::getAngleFilter( TAngle::EUnits units ) 
{	// Function to obtain a pointer instance of the requested angle filter
	TAngleFilter *filter = 0;

	switch( units )
	{
	case TAngle::kRadians:
		filter = TRadiansFilter::instance();
		break;
	case TAngle::kGons:
		filter = TGonsFilter::instance();
		break;
	case TAngle::k100MicroGons:
		filter = T100MicroGonsFilter::instance();
		break;
	case TAngle::kDMS:
		filter = TDMSFilter::instance();
		break;
	case TAngle::kCCs:
		filter = TCCsFilter::instance();
		break;
	default:
		// undefined angle filter requested
		assert( filter != 0 );
		break;
	}
	return filter;
}


TLengthFilter *TAStreamFormatter::getLengthFilter( TLength::EUnits units ) 
{//Function to obtain a pointer instance of the requested length filter
	TLengthFilter *filter = 0;

	switch( units )
	{
	case TLength::kKilometres:
		filter = TKilometresFilter::instance();
		break;
	case TLength::kMetres:
		filter = TMetresFilter::instance();
		break;
	case TLength::kMillimetres:
		filter = TMillimetresFilter::instance();
		break;
	default:
		// undefined length filter requested
		assert( filter != 0 );
		break;
	}

	return filter;
}


TSpatialPositionFilter *TAStreamFormatter::getPositionFilter(TCoordSysFactory::ECoordSys coordSys ) 
{//Function to obtain a pointer instance of the requested spatial position filter
	TSpatialPositionFilter *filter = 0;

	switch( coordSys )
	{
	case TCoordSysFactory::k3DCartesian:
		filter = T3DCartesianFilter::instance();
		break;
	default:
		// undefined position filter requested
		assert( filter != 0 );
		break;
	}

	return filter;
}


string TAStreamFormatter::getSeparator() const
{//returns the string used to separate fields
	return fSeparator;
}


void TAStreamFormatter::setSeparator(const string& sep)
{//sets the string used to separate fields
	fSeparator = sep;
	return;
}
//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////


//test
string TAStreamFormatter::getString()
{string s;
s=fSStream->str();
return s;}


void TAStreamFormatter::setDataSpacing()
{
	if (fLineGap)
	 (*fIOStream)<<endl;
	
}
		
void TAStreamFormatter::setNoGapBetweenData()
{
	fLineGap = false;
}

void TAStreamFormatter::setGapBetweenData()
{
	fLineGap = true;
}

void	TAStreamFormatter::writeString(const int width, const string data)
{
	//(*fStream)<<" ";

	this->width(width);
	(*this) << right << data;
	(*this) << getSeparator();
	return;

}

void	TAStreamFormatter::writeStringLeft(const int width, const string data)
{
	this->width(width);
	(*this) << left << data << right << getSeparator();
	return;
}

void	TAStreamFormatter::writeDouble(const int width, const int pres, const TReal data)
{
	this->width(width);
	this->precision(pres);
	(*this) << right << data << getSeparator();
	return;
}

void	TAStreamFormatter::writeInteger(const int width, const int data)
{
	this->width(width);
	(*this) << right << data << getSeparator();
	return;
}


