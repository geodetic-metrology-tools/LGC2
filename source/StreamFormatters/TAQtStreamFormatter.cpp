// TAQtStreamFormatter.cpp: 
//
// Abstract Base Class for a QTextStream Formatter.
// Enables the output of all the standard survey objects.
//
//
// Patterns:
// Maintains a reference to a QTextStream following 
// the decorator pattern.
// 
//
// Copyright 1999,2000, CERN, EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// Other forward declarations
#include "setformat.h"

#include  "TAngleFilter.h"
#include  "TDMSFilter.h"
#include  "TGonsFilter.h"
#include  "TRadiansFilter.h"
#include  "TLengthFilter.h"
#include  "TMillimetresFilter.h"
#include  "TMetresFilter.h"
#include  "TKilometresFilter.h"
#include  "TSpatialPointName.h"
#include  "TSpatialPosition.h"
#include  "TSpatialPoint.h"
#include  "TSpatialPositionFilter.h"
#include  "T3DCartesianFilter.h"
#include  "TAQtStreamFormatter.h"
//////////////////////////////////////////////////////////////////////


//ClassImp(TAQtStreamFormatter)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*TAQtStreamFormatter::TAQtStreamFormatter()
{
}*/


TAQtStreamFormatter::TAQtStreamFormatter(QTextStream *stream)
{
	fStream = stream;
	fStream->flags( QTextStream::fixed );

	fAngles = TAngle::kGons;
	fAngFilter = getAngleFilter(fAngles);

	fLengths = TLength::kMetres;
	fLenFilter = getLengthFilter(fLengths);

	fCoordToken = TSpatialPosition::kNull;
	fCoorSys = TSpatialPosition::k3DCartesian;
	fPosFilter = T3DCartesianFilter::instance();
}

TAQtStreamFormatter::~TAQtStreamFormatter()
{
}



//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

TAngle::EUnits TAQtStreamFormatter::setAngleUnits( TAngle::EUnits units )
{	// set the angle units and get the associated filter
	
	TAngle::EUnits oldAng;

	// change the angle filter if different to the one already defined
	if ( fAngles != units )
	{
		oldAng = fAngles; 
		fAngles = units; 
		
		fAngFilter = getAngleFilter(units);
	}
	else
	{
		oldAng = units;
	}

	return oldAng; 
}


TLength::EUnits TAQtStreamFormatter::setLengthUnits( TLength::EUnits units )
{	// set the length units and get the associated filter

	TLength::EUnits oldLen; 

	// change the length filter if different to the one already defined
	if( fLengths != units )
	{
		oldLen = fLengths; 
		fLengths = units; 
		
		fLenFilter = getLengthFilter(units);
	}
	else
	{
		oldLen = units;
	}

	return oldLen; 
}


TSpatialPosition::ECoordinateSystem  TAQtStreamFormatter::setCoordSys( TSpatialPosition::ECoordinateSystem coorSys )
{	// set the coordinate system and get the associated filter

	TSpatialPosition::ECoordinateSystem  oldSys; 

	// change the position filter if different to the one already defined
	if( fCoorSys != coorSys )
	{
		oldSys = fCoorSys; 
		fCoorSys = coorSys; 
		
		fPosFilter = getPositionFilter(coorSys);
	}
	else
	{
		oldSys = coorSys;
	}

	return oldSys; 
}


TAngleFilter *TAQtStreamFormatter::getAngleFilter( TAngle::EUnits units ) 
{	// Function to obtain a pointer instance of the requested
	// angle filter
	
	TAngleFilter *filter = 0;

	switch( units )
	{
	case TAngle::kRadians:
		filter = TRadiansFilter::instance();
		break;

	case TAngle::kGons:
		filter = TGonsFilter::instance();
		break;

	case TAngle::kDMS:
		filter = TDMSFilter::instance();
		break;

	default:
		// undefined angle filter requested
		assert( filter != 0 );
		break;
	}

	return filter;
}


TLengthFilter *TAQtStreamFormatter::getLengthFilter( TLength::EUnits units ) 
{	// Function to obtain a pointer instance of the requested
	// length filter
	
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


TSpatialPositionFilter *TAQtStreamFormatter::getPositionFilter( TSpatialPosition::ECoordinateSystem coordSys ) 
{	// Function to obtain a pointer instance of the requested
	// spatial position filter
	
	TSpatialPositionFilter *filter = 0;

	switch( coordSys )
	{
	case TSpatialPosition::k3DCartesian:
		filter = T3DCartesianFilter::instance();
		break;

	default:
		// undefined position filter requested
		assert( filter != 0 );
		break;
	}

	return filter;
}




//////////////////////////////////////////////////////////////////////
// Overloaded input operator
//////////////////////////////////////////////////////////////////////

TAQtStreamFormatter &TAQtStreamFormatter::operator>>( QChar &c )
{   fStream->operator>>( c ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( char &c )
{   fStream->operator>>( c ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( signed short &i )
{   fStream->operator>>( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( unsigned short &i )
{   fStream->operator>>( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( signed int &i )
{   fStream->operator>>( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( unsigned int &i )
{   fStream->operator>>( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( signed long &i )
{   fStream->operator>>( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( unsigned long &i )
{   fStream->operator>>( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( float &f )
{   fStream->operator>>( f ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( double &d )
{   fStream->operator>>( d ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( char *s )
{   fStream->operator>>( s ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( string word)
{
	QString  str;

	// get the name of the point as a single string from the text stream
	// and set the spatial point name
	this->TAQtStreamFormatter::operator>>( str );
	word = ( str.latin1() );
	return *this;
}


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( QString &str )
{   fStream->operator>>( str ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator>>( QCString &str )
{   fStream->operator>>( str ); return *this; }

    
TAQtStreamFormatter &TAQtStreamFormatter::operator>>( TSFFUNC func )
{	(*func)( *(this) ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::readRawBytes( char *s, 
													uint len )
{	fStream->readRawBytes(s, len); return *this; }



//////////////////////////////////////////////////////////////////////
// Overloaded output operator
//////////////////////////////////////////////////////////////////////

TAQtStreamFormatter &TAQtStreamFormatter::operator<<( char c )
{   fStream->operator<<( c ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( signed short i )
{   fStream->operator<<( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( unsigned short i )
{   fStream->operator<<( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( signed int i )
{   fStream->operator<<( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( unsigned int i )
{   fStream->operator<<( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( signed long i )
{   fStream->operator<<( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( unsigned long i )
{   fStream->operator<<( i ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( float f )
{   fStream->operator<<( f ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( double d )
{   fStream->operator<<( d ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( const char *s )
{   fStream->operator<<( s ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( string str)
{
	fStream->operator<<( str.c_str()); return *this;
}


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( const QString &str )
{   fStream->operator<<( str ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( const QCString &str )
{   fStream->operator<<( str ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( QTSManip &manip )
{   manip.exec( *(this->fStream) ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( TSFFUNC func )
{	(*func)( *(this) ); 
return *this; }


//TAQtStreamFormatter &TAQtStreamFormatter::operator<<( QTSFUNC func )
//{	(*func)( *(this) ); 
//return *this; }


//TAQtStreamFormatter &TAQtStreamFormatter::operator<<( TSFFUNC func )
//{	*(this->fStream) << "\n"; return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::operator<<( void *v )
{   fStream->operator<<( v ); return *this; }


TAQtStreamFormatter &TAQtStreamFormatter::writeRawBytes( const char* s, 
													 uint len )
{	fStream->writeRawBytes(s, len); return *this; }

////////////////////////////////
// sets the stream's point format
////////////////////////////////
TAQtStreamFormatter& TAQtStreamFormatter::operator<<(TPointFormat& ptFormat){

	fPointFormat = ptFormat;
	return *this;
}

///////////////////////////////////////
// sets the stream's observation format
///////////////////////////////////////
TAQtStreamFormatter& TAQtStreamFormatter::operator<<(TObservationFormat& obsFormat){

	fObservationFormat = obsFormat;
	return *this;
}


/////////////////////////////////////////////////////////////////
// reads and returns a character from the stream and puts it back
/////////////////////////////////////////////////////////////////
QChar TAQtStreamFormatter::peek(){

	return fStream->peek();
}


//////////////////////////////////////////////////////////////////////
// Utility Member Functions
//////////////////////////////////////////////////////////////////////


	
//////////////////////////////////////////////////////////////////////
// QTextStream manipulators
//////////////////////////////////////////////////////////////////////
 

TAQtStreamFormatter &fbin( TAQtStreamFormatter &s )
{
    s.setf(QTS::bin,QTS::basefield);
    return s;
}

TAQtStreamFormatter &foct( TAQtStreamFormatter &s )
{
    s.setf(QTS::oct,QTS::basefield);
    return s;
}

TAQtStreamFormatter &fdec( TAQtStreamFormatter &s )
{
    s.setf(QTS::dec,QTS::basefield);
    return s;
}

TAQtStreamFormatter &fhex( TAQtStreamFormatter &s )
{
    s.setf(QTS::hex,QTS::basefield);
    return s;
}

TAQtStreamFormatter &fendl( TAQtStreamFormatter &s )
{
    return s << '\n';
}

TAQtStreamFormatter &fflush( TAQtStreamFormatter &s )
{
    if ( s.device() )
	s.device()->flush();
    return s;
}

TAQtStreamFormatter &fws( TAQtStreamFormatter &s )
{
    s.skipWhiteSpace();
    return s;
}

TAQtStreamFormatter &freset( TAQtStreamFormatter &s )
{
    s.reset();
    return s;
}



//////////////////////////////////////////////////////////////////////
// End 
//////////////////////////////////////////////////////////////////////

