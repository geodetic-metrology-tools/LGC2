// TFreeFormatTStream.h
//
//
// Concrete class defining the free format file syntax,
// for example the file as used in CSGeo, and Geotran.
//
// Patterns:
// A concrete decorator class of a QTextStream.
// 
// Copyright 1999,2000 CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include  "TAngleFilter.h"
#include  "TLengthFilter.h"
#include  "TSpatialPositionFilter.h"
#include  "TFreeFormatTStream.h"
#include  "TSpatialPoint.h"
#include  "TSpatialPointName.h"
#include  "TSpatialPosition.h"
//#include  "QTextStream.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TFreeFormatTStream)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TFreeFormatTStream::TFreeFormatTStream(TDataParameters& dp) : TAStreamFormatter (dp)
{
	//fCoordToken;
	//fCoorSys;
}


TFreeFormatTStream::TFreeFormatTStream(const string& str, TDataParameters& dp) : TAStreamFormatter (str, dp)
{
	//fCoordToken;
	//fCoorSys;
}


TFreeFormatTStream::TFreeFormatTStream(EIOType io, TADataSet& ds) : TAStreamFormatter (io, ds)
{
	//fCoordToken;
	//fCoorSys;
}

TFreeFormatTStream::~TFreeFormatTStream()
{
	delete fIOStream;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TAStreamFormatter  &TFreeFormatTStream::operator>>( TSpatialPoint &point )
{	

	TSpatialPointName  name;
	TSpatialPosition*	position = new TSpatialPosition(fRefFrame);
	
	this->skipCommentLines();

	// get the point name
	this->TAStreamFormatter::operator>>(name);

	// get the point posiiton
	this->TAStreamFormatter::operator>>(*position);
	
	point.setName( name );
	point.setPosition( position );
	
	return *this;
}



TAStreamFormatter  &TFreeFormatTStream::operator<<( const TSpatialPosition &position )
{	//input a spatial position from the text stream
	
	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, position);

	return *this;
}


TAStreamFormatter  &TFreeFormatTStream::operator<<( const TSpatialPoint &point )
{	
	// Get the width and precision for the point output
	// The assigned width is divided evenly between the point name and the
	// three point coordinates
	int  width = this->width() - 3; // reduce width by 3 to account for spaces
	int  precision = this->precision();
	int  remainder = width % 4;

	//divide the width by four to determine the amount to assign to the name,
	// and the three coordinates
	width /= 4;
	
	
	// output the point name
	// assign the additional width to the point name
	this->setf(ios::left );
	this->width( width + remainder ); 
	this->TAStreamFormatter::operator<<(point.getName()); 
	this->TAStreamFormatter::operator<< ( ' ' );
	this->unsetf(ios::left );

	// output the position
	//if ( point.positionKnown() )
	{
		this->width( 3*width );
		this->precision( precision );
		*this << point.getPosition(getReferenceFrame());
	}
	
	return *this;
}


void  TFreeFormatTStream::skipCommentLines()
{// method to skip over the white space and comment lines from the input stream

	// skip over the white space in the file
	this->skipWhiteSpace();
	
	// loop through all unrequired text
	while( '#' == this->peek() ) // # character indicates a comment line
	{
		// read the comment line and skip any more white space
		this->readLine();
		this->skipWhiteSpace();
	}

	
	return;
}


string TFreeFormatTStream::separator() const
{//returns the string used to separate fields
	string sep(" ");
	return sep;
}

//////////////////////////////////////////////////////////////////////
//end
//////////////////////////////////////////////////////////////////////
