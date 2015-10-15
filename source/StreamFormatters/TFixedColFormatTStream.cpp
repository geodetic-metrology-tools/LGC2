// TFixedColFormatTStream.cpp
//
// Concrete class defining the old LGC file syntax,
//
// Patterns:
// A concrete decorator class of a QTextStream.
// 
// Copyright 1999,2000 CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
//#include <stdlib.h>

#include  "TAngleFilter.h"
#include  "TLengthFilter.h"
#include  "TSpatialPositionFilter.h"
#include "TAngle.h"
#include "TLength.h"
#include "TSpatialPointName.h"
#include "TSpatialPosition.h"
#include "TSpatialPoint.h"


#include "TPointFormat.h"
#include "TObservationFormat.h"

#include "TGeodeticRefFrame.h"

#include  "TFixedColFormatTStream.h"
////////////////////////////////////////////////////////////////

//ClassImp(TFixedColFormatTStream)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////
TFixedColFormatTStream::TFixedColFormatTStream(TDataParameters& dp) : TAStreamFormatter(dp)
{
		fSeparator ="";
		fNoObsToRead = false;
}


TFixedColFormatTStream::TFixedColFormatTStream(const string& str, TDataParameters& dp) : TAStreamFormatter (str, dp)
{

		fSeparator ="";
		fNoObsToRead = false;

}


TFixedColFormatTStream::TFixedColFormatTStream(TAStreamFormatter::EIOType io, TADataSet& ds) : TAStreamFormatter (io, ds)
{
		fSeparator ="";
		fNoObsToRead = false;
	
}

TFixedColFormatTStream::TFixedColFormatTStream(TAStreamFormatter::EIOType io, TADataSet& ds, TPointFormat& pf) : TAStreamFormatter (io, ds, pf)
{
	fSeparator ="";
	fNoObsToRead = false;
}


TFixedColFormatTStream::TFixedColFormatTStream(const TAStreamFormatter::EIOType io, TADataSet& ds, const TPointFormat& pf, const TObservationFormat& obsFor) : TAStreamFormatter (io, ds, pf, obsFor)
{
	fSeparator ="";
	fNoObsToRead = false;
}


TFixedColFormatTStream::~TFixedColFormatTStream()
{
	//delete fStream;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TAStreamFormatter  &TFixedColFormatTStream::operator>>( TSpatialPoint &point )
{//input a spatial point from the text stream
	
	TSpatialPointName  name;
	TSpatialPosition*	position = new TSpatialPosition(fRefFrame);
	
	this->skipCommentLines();

	// get the point name
	this->TAStreamFormatter::operator>>(name);

	// get the point position
	if (this->getError() == "")
	{
		this->TAStreamFormatter::operator>>(*position);
	}
	
	TReal dist=-1;
	int id=-1;
	string comment="";
	string comeol="";
	string comdb="";
	//crashes here !?!?!

	this->skipWhiteSpace();

	if(this->getError() == "" && !this->atEndCom() && this->peek()=='$')
	{
		
		this->TAStreamFormatter::readChar();//read the char $

		if(this->getError() == "" && this->peek() != ' ')
		{//read distance just after $
			this->TAStreamFormatter::operator>>(dist);
			
			if (this->fail() == true)
				this->setError("Failed Reading dist value.");
		}
				
		if (this->getError() == "")
		{	
			this->skipWhiteSpace();
			this->TAStreamFormatter::operator>>(id);
			if (this->fail() == true)
				this->setError("Failed Reading ID value.");
		}
	
		while(!this->atEndCom() && this->getError() == "" && !(this->TAStreamFormatter::peek()=='\n') && !(this->TAStreamFormatter::peek()=='%'))
		{//read data base comments until the end of line or an end line comments
			
			this->TAStreamFormatter::operator>>(comment);
			comdb += comment+ " ";
			if (this->fail() == true)
				this->setError("Failed reading comment 1 at " + comdb);

			this->skipWhiteSpace();
		}
	
	}

	if(this->getError() == "" && (this->peek()=='%' || this->peek()=='#'))
	{
		
		this->TAStreamFormatter::readChar();

		while(this->getError() == "" && !(this->TAStreamFormatter::peek()=='\n'))
		{
			this->TAStreamFormatter::operator>>(comment);
			comeol += comment+ " ";
					
			if (this->fail() == true)
				this->setError("Failed reading comment 2.");

			this->skipWhiteSpace();
		}
	}

	if (this->getError() == "")
	{
		name.setId(id);
		name.setComment(comdb);

		point.setName( name );
		point.setPosition( position );
		point.setDist(dist);
		point.setEOLComment(comeol);
	}
	else
	{
		delete position;
	}

	return *this;

}


/////////////////////////////////////////////////////////////////////////////////////////////
//OUTPUT
/////////////////////////////////////////////////////////////////////////////////////////////
TAStreamFormatter  &TFixedColFormatTStream::operator<<( const TSpatialPosition &position )
{//output a spatial position to the text stream	

	//format the stream with the information from the PointFormat
//	this->setWidthFormat(fPointFormat.getCoordWidth());
//	this->setPrecisionFormat(fPointFormat.getCoordPrecision());
	
	//width and precision are defined before (writer)
	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, position);

	return *this;
}


TAStreamFormatter  &TFixedColFormatTStream::operator<<( const TSpatialPoint &point )
{//output a spatial point to the text stream	

	// output the point name
	TSpatialPointName name;
	name = point.getName();
	this->TAStreamFormatter::operator<<(name); 

	(*fIOStream) << fSeparator;

	// output the position
	//if ( (point.getPosition(getReferenceFrame()).getPositionStatus()) != TPositionStatus::kPosNull )
	TPositionVector pv (this->getCoordSys());
	pv=point.getPosition().getCoordinates(this->getCoordSys());

	if (pv.getStatus()!= TANumericValue::kNull)
	{
		this->TAStreamFormatter::setWidthFormat(fPointFormat.getCoordWidth());
		this->TAStreamFormatter::setPrecisionFormat(fPointFormat.getCoordPrecision());
		
		*this << point.getPosition();
	}
	
	return *this;
}


TAStreamFormatter& TFixedColFormatTStream::operator<<(const TPositionVector& posVec)
{//output a TPositionVector

	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, posVec);

	return *this;
}



TAStreamFormatter&	TFixedColFormatTStream::operator<<(const TFreeVector& fv )
{// ouputs a TFreeVector
	this->TAStreamFormatter::operator<<(fv.getX())<<(fSeparator)<<(fv.getY())<<fSeparator<<(fv.getZ());
	return *this;
}


//////////////////////////////////////////////////////////////////////////////////////////
//Member Functions
//////////////////////////////////////////////////////////////////////////////////////////
string TFixedColFormatTStream::getSeparator() const
{//returns the string used to separate fields
	return fSeparator;
}


void TFixedColFormatTStream::setSeparator(const string& sep)
{//returns the string used to separate fields
	fSeparator = sep;
	return;
}


void  TFixedColFormatTStream::skipCommentLines()
{//skips over the white space and comment lines from the input stream	
	//char c = this->peek();
	// skip over the white space in the file
	this->skipWhiteSpace();
	//char c = this->peek();
	
	// loop through all unrequired text
	while( '%' == this->peek() ) // % character indicates a comment line
	{
		// read the comment line and skip any more white space
		this->readLine();
		this->skipWhiteSpace();
	}
	//c = this->peek();

	return;
}


void  TFixedColFormatTStream::setNoObsToRead()
{
	fNoObsToRead = true;
}


int TFixedColFormatTStream::ajustAnglePrecTo100MicroGons(const TObservationFormat::EAnglePrecision angle)
{
	int i = 0;

	switch (angle)
	{
		case TObservationFormat::kGons:
			i = 0;
			break;

		case TObservationFormat::k100Milligons:
			i = 0 ;
			break;

		case TObservationFormat::k10Milligons:
			i = 0 ;
			break;

		case TObservationFormat::kMilligons:
			i = 0 ;
			break;

		case TObservationFormat::k100Microgons:
			i = 0 ;
			break;

		case TObservationFormat::k10Microgons:
			i = 1 ;
			break;

		case TObservationFormat::kMicrogon:
			i = 2 ;
			break;

		default:
			i = 2;
			break;
	}

	return i;
}


