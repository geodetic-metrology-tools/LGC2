// TSeparatedFormatTStream.cpp
//
// Concrete class defining a separated stream text stream formatter
//
// Patterns:
// A concrete decorator class of a TAStreamFormatter.
// 
// Copyright 1999-2008 M.Jones, CERN, EST/SU. All rights reserved.
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

#include  "TSeparatedFormatTStream.h"
////////////////////////////////////////////////////////////////

//ClassImp(TSeparatedFormatTStream)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////
TSeparatedFormatTStream::TSeparatedFormatTStream(TDataParameters& dp) : TAStreamFormatter(dp)
{
		fSeparator ="   ";
		fNoObsToRead = false;
}


TSeparatedFormatTStream::TSeparatedFormatTStream(const string& str, TDataParameters& dp) : TAStreamFormatter (str, dp)
{
		fSeparator ="   ";
		fNoObsToRead = false;
}


/*!Constructors
\param io: read or write
\param fp: data file's parameters (name, path ...) 
\param ds: data parameters (unit, precision, refernce system, coord sys...)*/ 
TSeparatedFormatTStream::TSeparatedFormatTStream(EIOType io, TFileParameters& fp, TDataParameters& dp):
TAStreamFormatter(io, fp, dp)
{
		fSeparator ="   ";
		fNoObsToRead = false;
}

TSeparatedFormatTStream::TSeparatedFormatTStream(TAStreamFormatter::EIOType io, TADataSet& ds) : TAStreamFormatter (io, ds)
{
		fSeparator ="   ";
		fNoObsToRead = false;
}

TSeparatedFormatTStream::TSeparatedFormatTStream(TAStreamFormatter::EIOType io, TADataSet& ds, TPointFormat& pf) : TAStreamFormatter (io, ds, pf)
{
	fSeparator ="   ";
	fNoObsToRead = false;
}


TSeparatedFormatTStream::TSeparatedFormatTStream(const TAStreamFormatter::EIOType io, TADataSet& ds, const TPointFormat& pf, const TObservationFormat& obsFor) : TAStreamFormatter (io, ds, pf, obsFor)
{
	fSeparator ="   ";
	fNoObsToRead = false;
}


TSeparatedFormatTStream::~TSeparatedFormatTStream()
{
	//delete fStream;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TAStreamFormatter  &TSeparatedFormatTStream::operator>>( TSpatialPoint &point )
{//input a spatial point from the text stream
	
	TSpatialPointName  name;
	TSpatialPosition*	position = new TSpatialPosition(fRefFrame);
	TReal dist=-1;
	int id=-1;
	string comment="";
	string comeol="";
	string comdb="";

	
	// make sure that the spatial position object was created ok
	// if not (position = 0), set an error message and return
	if( position == 0 )
	{
		this->setError("Failed to create Spatial Position object");
		return *this;
	}

	this->skipCommentLines();

	// get the point name
	this->TAStreamFormatter::operator>>(name);

	// get the point position
	if (this->getError() == "")
	{
		this->TAStreamFormatter::operator>>(*position);
	}
	
	this->skipWhiteSpace();

	if(this->getError() == "" && !this->atEndCom() && this->peek()=='$')
	{	
		this->TAStreamFormatter::readChar();//read the char $

		//if no error encountered and the next character is not a blank
		// read the cumulative distance for the point
		if(this->getError() == "")
		{
			// see if the next character is a blank
			if( this->peek() != ' ' )
				//read distance just after $
				this->TAStreamFormatter::operator>>(dist);
			else
				// set warning that the cumulative distance is missing
				this->addWarning("Point Cumulative Distance value missing");

			// if there is a problem reading the cumulative distance set a warning message
			if (this->fail() == true)
			{
				this->addWarning("Failed to read Cumulative Distance value");
				// clear the failbit
				this->clear();
			}	
		}
		
		// read the point dB ID
		if (this->getError() == "")
		{	
			this->skipWhiteSpace();
			this->TAStreamFormatter::operator>>(id);
			if (this->fail() == true)
			{
				this->addWarning("Failed to read database ID");
				// clear the failbit
				this->clear();
			}
		}
	
		this->skipWhiteSpace();

		while(!this->atEndCom() && this->getError() == "" && !(this->TAStreamFormatter::peek()=='\n') 
				&& !(this->TAStreamFormatter::peek()=='%' || this->peek()=='#'))
		{//read data base comments until the end of line or an end line comments
			
			this->TAStreamFormatter::operator>>(comment);
			comdb += comment+ " ";

			this->skipWhiteSpace();
		}
	
	}

	// read any additional comments at the end of the line
	if(this->getError() == "" && (this->peek()=='%' || this->peek()=='#'))
	{
		
		this->TAStreamFormatter::readChar();

		while(this->getError() == "" && !(this->TAStreamFormatter::peek()=='\n'))
		{
			this->TAStreamFormatter::operator>>(comment);
			comeol += comment+ " ";
					
			this->skipWhiteSpace();
		}
	}

	//if (this->getError() == "")
	{
		name.setId(id);
		name.setComment(comdb);

		point.setName( name );
		point.setPosition( position );
		point.setDist(dist);
		point.setEOLComment(comeol);
	}
	/*else
	{
		delete position;
	}*/

	return *this;

}


/////////////////////////////////////////////////////////////////////////////////////////////
//OUTPUT
/////////////////////////////////////////////////////////////////////////////////////////////
TAStreamFormatter  &TSeparatedFormatTStream::operator<<( const TSpatialPosition &position )
{//output a spatial position to the text stream	

	//format the stream with the information from the PointFormat
//	this->setWidthFormat(fPointFormat.getCoordWidth());
//	this->setPrecisionFormat(fPointFormat.getCoordPrecision());
	
	//width and precision are defined before (writer)
	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, position);

	return *this;
}


TAStreamFormatter  &TSeparatedFormatTStream::operator<<( const TSpatialPoint &point )
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


TAStreamFormatter& TSeparatedFormatTStream::operator<<(const TPositionVector& posVec)
{//output a TPositionVector

	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, posVec);

	return *this;
}



TAStreamFormatter&	TSeparatedFormatTStream::operator<<(const TFreeVector& fv )
{// ouputs a TFreeVector
	this->TAStreamFormatter::operator<<(fv.getX())<<(fSeparator)<<(fv.getY())<<fSeparator<<(fv.getZ());
	return *this;
}


//////////////////////////////////////////////////////////////////////////////////////////
//Member Functions
//////////////////////////////////////////////////////////////////////////////////////////
string TSeparatedFormatTStream::getSeparator() const
{//returns the string used to separate fields
	return fSeparator;
}


void TSeparatedFormatTStream::setSeparator(const string& sep)
{//returns the string used to separate fields
	fSeparator = sep;
	return;
}


void  TSeparatedFormatTStream::skipCommentLines()
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


void  TSeparatedFormatTStream::setNoObsToRead()
{
	fNoObsToRead = true;
}


int TSeparatedFormatTStream::ajustAnglePrecTo100MicroGons(const TObservationFormat::EAnglePrecision angle)
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

int	TSeparatedFormatTStream::width() const
{
	return (int)fIOStream->width();
	//return 0;
}

int	TSeparatedFormatTStream::width(int w)
{
	return (int)fIOStream->width(w);
	//return 0;
}

int	TSeparatedFormatTStream::setWidthFormat(const int width)
{
	int oldWidth = fWidth;
	fWidth = width;
	return oldWidth;
}



