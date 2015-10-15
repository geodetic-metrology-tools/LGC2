// TAFixedColFormatTStream.cpp
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
#include <locale>
using namespace std;


#include  "TAngleFilter.h"
#include  "TLengthFilter.h"
#include  "TSpatialPositionFilter.h"
#include  "TAFixedColFormatTStream.h"
#include  "TSpatialPoint.h"
#include  "TSpatialPointName.h"
#include  "TSpatialPosition.h"
///////////////////#include  "TEuclideanRefFrame.h"
#include  "TPointFormat.h"
#include  "TAStreamFormatter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TAFixedColFormatTStream)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TAFixedColFormatTStream::TAFixedColFormatTStream(TDataParameters& dp) : TAStreamFormatter(dp)
{
	/*TEuclideanRefFrame* erf = new TEuclideanRefFrame("LocalTransform");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}

TAFixedColFormatTStream::TAFixedColFormatTStream(const string& str,TDataParameters& dp) : TAStreamFormatter(str, dp)
{
	/*TEuclideanRefFrame* erf = new TEuclideanRefFrame("LocalTransform");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}

TAFixedColFormatTStream::TAFixedColFormatTStream(EIOType io, TADataSet& ds) : TAStreamFormatter(io, ds)
{
	/*TEuclideanRefFrame* erf = new TEuclideanRefFrame("LocalTransform");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}


TAFixedColFormatTStream::~TAFixedColFormatTStream()
{
//	delete fStream;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TAStreamFormatter  &TAFixedColFormatTStream::operator>>(TSpatialPoint& point)
{	
	TSpatialPointName  name;
	TSpatialPosition*	position = new TSpatialPosition(fRefFrame);
	
	this->skipCommentLines();

	// get the point name
	this->TAStreamFormatter::operator>>(name);

	// get the point position
	this->TAStreamFormatter::operator>>(*position);

	position->setObjectStatus( TSpatialStatus::kVxyz);

	point.setName(name);
	point.setPosition( position );

	return *this;
}



TAStreamFormatter  &TAFixedColFormatTStream::operator<<( const TSpatialPointName &ptName )
{//output a spatial point name to the text stream
	
	int  width = fPointFormat.getNameWidth(); // the width assigned to the output

	// extract the name of the point as a single string and output to the
	// text stream
	this->width( width );
	this->setf(ios::left);
	this->TAStreamFormatter::operator<<( (ptName.getName()).c_str() );
	this->unsetf(ios::left);

	return *this;
}


TAStreamFormatter  &TAFixedColFormatTStream::operator<<( const TSpatialPosition &position )
{//input a spatial position from the text stream
	
	//format the stream with the information from the PointFormat
	fIOStream->width(fPointFormat.getCoordWidth());
	fIOStream->precision(fPointFormat.getCoordPrecision());
	

	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, position);

	return *this;

}


TAStreamFormatter  &TAFixedColFormatTStream::operator<<( const TSpatialPoint &point )
{// output the point name

	*this << point.getName(); 
	(*fIOStream) << separator();

	// output the position
	//if ( (point.getPosition(getReferenceFrame()).getPositionStatus()) != TPositionStatus::kPosNull )
	TPositionVector pv (TCoordSysFactory::k3DCartesian);
	pv=point.getPosition(getReferenceFrame()).getCoordinates(TCoordSysFactory::k3DCartesian);
	
	if (pv.getStatus()!= TANumericValue::kNull)
	{
		*this << point.getPosition(getReferenceFrame());
	}
	
	return *this;
}


void  TAFixedColFormatTStream::skipCommentLines()
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


string TAFixedColFormatTStream::separator() const
{//returns the string used to separate fields

	string sep(" ");
	return sep;
}


//////////////////////////////////////////////////////////////////////
//end
//////////////////////////////////////////////////////////////////////
