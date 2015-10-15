// TBDFixedColFormatTStream.cpp
//
//
// Concrete class defining the normal "fixed column size" format file syntax,
// for example the file as used in Transform.
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
#include  "TBDFixedColFormatTStream.h"
#include  "TSpatialPoint.h"
#include  "TSpatialPointName.h"
#include  "TSpatialPosition.h"
//#include  "TEuclideanRefFrame.h"
#include  "TPointFormat.h"
//#include  "QTextStream.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TBDFixedColFormatTStream)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TBDFixedColFormatTStream::TBDFixedColFormatTStream(TDataParameters& dp) : TAFixedColFormatTStream(dp)
{
/*	TEuclideanRefFrame* erf = new TEuclideanRefFrame("LocalTransform");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}

TBDFixedColFormatTStream::TBDFixedColFormatTStream(const string& str, TDataParameters& dp) : TAFixedColFormatTStream(str, dp)
{
/*	TEuclideanRefFrame* erf = new TEuclideanRefFrame("LocalTransform");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}

TBDFixedColFormatTStream::TBDFixedColFormatTStream(EIOType io, TADataSet& ds) : TAFixedColFormatTStream(io, ds)
{
/*	TEuclideanRefFrame* erf = new TEuclideanRefFrame("LocalTransform");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}

TBDFixedColFormatTStream::~TBDFixedColFormatTStream()
{
//	delete fStream;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////



TAStreamFormatter  &TBDFixedColFormatTStream::operator>>( TSpatialPoint &point )
{	
	TSpatialPointName  name;
	TSpatialPosition*	position = new TSpatialPosition(fRefFrame);
	int id;
	
	this->skipCommentLines();

	// get the point name
	this->TAStreamFormatter::operator >> (name);

	// get the point position
	this->TAStreamFormatter::operator >> (*position);

	position->setObjectStatus(TSpatialStatus::kVxyz);

	// get the point position
	this->TAStreamFormatter::operator>> (id);
	
	point.setName(name);
	point.setPosition( position );
	point.setId(id);

	return *this;
}



TAStreamFormatter  &TBDFixedColFormatTStream::operator<<( const TSpatialPoint &point )
{	
// output the point name;
	TPointFormat format(13,11,TPointFormat::k10Micrometres,7,TPointFormat::k10Millimetres, true);
	this->TAStreamFormatter::width(13);
	this->TAFixedColFormatTStream::operator << (point.getName()); 
	
	(*fIOStream) << separator();

	// output the position
	//if ( (point.getPosition(getReferenceFrame()).getPositionStatus()) != TPositionStatus::kPosNull )
	TPositionVector pv (TCoordSysFactory::k3DCartesian);
	pv=point.getPosition(getReferenceFrame()).getCoordinates(TCoordSysFactory::k3DCartesian);
	
	if (pv.getStatus()!= TANumericValue::kNull)
	{
		this->TAStreamFormatter::setPointFormat(format);
		this->TAFixedColFormatTStream::operator << (point.getPosition(getReferenceFrame()));
	}

	(*fIOStream) << separator();

	this->TAStreamFormatter::width(6);
	this->TAStreamFormatter::operator<< (point.getId());
	
	return *this;
}

//////////////////////////////////////////////////////////////////////


