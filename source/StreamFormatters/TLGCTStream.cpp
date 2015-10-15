// TLGCTStream.cpp
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
#include <stdlib.h>

#include  "TAngleFilter.h"
#include  "TLengthFilter.h"
/*include "TWPSFilter.h"
#include "TWPSTFilter.h"
#include "TWPSVFilter.h"
#include "TWPS2DFilter.h"
#include "TWTWFilter.h"
#include "TWTWHFilter.h"
#include "TWTWVFilter.h"
#include "TWTW2DFilter.h"*/
#include  "TSpatialPositionFilter.h"
#include "TAngle.h"
#include "TLength.h"
#include "TSpatialPointName.h"
#include "TSpatialPosition.h"
//#include "TSpatialLine.h"
#include "TLSCalcSpPoint.h"
/////#include "TCoordinateSystem.h"
/////#include "QTextStream.h"

#include "TPointFormat.h"
#include "TObservationFormat.h"

#include "THorizontalDistMeas.h"
#include "TVerticalDistMeasurement.h"
#include "THorAngleMeasurement.h"
#include "TZenithDistMeasurement.h"
#include "T2DOffsetMeasurement.h"

//#include "TRadialOffsetCnstr.h"

#include "TGeodeticRefFrame.h"

#include  "TLGCTStream.h"
////////////////////////////////////////////////////////////////

//ClassImp(TLGCTStream)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////
TLGCTStream::TLGCTStream() : TAStreamFormatter ()
{
		TGeodeticRefFrame* geo = new TGeodeticRefFrame("LGCTStreamFrame");
	setReferenceFrame(geo);
/*	TEuclideanRefFrame* erf = new TEuclideanRefFrame("LGCTStreamFrame");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}


TLGCTStream::TLGCTStream(const string& str) : TAStreamFormatter (str)
{
		TGeodeticRefFrame* geo = new TGeodeticRefFrame("LGCTStreamFrame");
	setReferenceFrame(geo);
/*	TEuclideanRefFrame* erf = new TEuclideanRefFrame("LGCTStreamFrame");
	setReferenceFrame(erf);*/
	//fCoordToken;
	//fCoorSys;
}


TLGCTStream::TLGCTStream(TAStreamFormatter::EIOType io, const char* filename) : TAStreamFormatter (io, filename)
{
	TGeodeticRefFrame* geo = new TGeodeticRefFrame("LGCTStreamFrame");
	setReferenceFrame(geo);
	//fCoordToken;
	//fCoorSys;
}


TLGCTStream::~TLGCTStream()
{
	//delete fStream;
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////


TAStreamFormatter  &TLGCTStream::operator>>( TSpatialPoint &point )
{//input a spatial point from the text stream
	
	
	TSpatialPointName  name;
	TSpatialPosition*	position = new TSpatialPosition( fRefFrame );
	
	this->skipCommentLines();

	// get the point name
	this->TAStreamFormatter::operator>>(name);

	// get the point posiiton
	this->TAStreamFormatter::operator>>(*position);
	
	point.setName( name );
	point.setPosition( position );

	this->skipWhiteSpace();

	double dist=-1;
	int id=-1;
	string comment="";
	string comeol="";
	string comdb="";

	if(this->peek()=='$')
	{
		this->TAStreamFormatter::readChar();

		this->TAStreamFormatter::operator>>(dist);
		this->TAStreamFormatter::operator>>(id);
		
			while(!(this->atEnd()))
			{
			this->TAStreamFormatter::operator>>(comment);
			comdb += comment+ " ";
			}
	}

	else if(this->peek()=='%')
	{
		this->TAStreamFormatter::readChar();

			while(!(this->atEnd()))
			{
			this->TAStreamFormatter::operator>>(comment);
			comeol += comment+ " ";
			}
	}

	point.setDist(dist);
	point.setId(id);
	point.setComment(comdb);
	point.setEOLComment(comeol);

	return *this;
}

/*
TAStreamFormatter& TLGCTStream::operator>>( TSpatialLine& line)
{//input a spatial line from the text stream

	TSpatialPointName end1, end2;

	this->skipCommentLines();

	string lLine = this->readLine();
	/*QTextStream* lineTStream = new QTextStream(&lLine, IO_ReadOnly);
	TLGCTStream lgcLine(lineTStream);*/
/*	TLGCTStream lgcLine(lLine);

	//get the first end's name
	lgcLine.TAStreamFormatter::operator>>(end1);

	//get the second end's name
	lgcLine.TAStreamFormatter::operator>>(end2);

	//possibly get the wire's name
	if (!lgcLine.atEnd()){

		char lineName[30];
		(&lgcLine)->TAStreamFormatter::operator>>(lineName);
		string lName(lineName);

		TSpatialLine* spl = new TSpatialLine(end1, end2, lName);
		line = *spl;
	}
	else{
		TSpatialLine* spl = new TSpatialLine(end1, end2);
		line = *spl;
	}

	//delete lineTStream;

	return *this;
}
*/

TAStreamFormatter& TLGCTStream::operator>>( THorizontalDistMeas& hDist)
{//input an horizontal distance measurement from the text stream

	TSpatialPointName stName, tgName;
	TLength obsDist(0.0), sigma(0.0);

	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
	TLGCTStream lgcLine(obsLine);

	//get the station name
	lgcLine.TAStreamFormatter::operator>>(stName);

	//get the target name
	lgcLine.TAStreamFormatter::operator>>(tgName);

	//possibly read the observed distance
	if (!lgcLine.atEnd()){
		//lgcLine >> obsDist;
		lgcLine.TAStreamFormatter::operator>>(obsDist);
	}

	//possibly read the measurement's precision
	if (!lgcLine.atEnd()){
		//lgcLine >> sigma;
		lgcLine.TAStreamFormatter::operator>>(sigma);
		sigma *= 0.001;
	}

	/////////////delete obsTStream;

	//create the horizontal distance measurement
	THorizontalDistMeas* hD = new THorizontalDistMeas(/*stName,*/ tgName, obsDist, sigma);

	hDist = *hD;

	delete hD;

	return *this;
}


TAStreamFormatter& TLGCTStream::operator>>( TVerticalDistMeasurement& vDist)
{//inputs a vertical distance measurement from the text stream

	TSpatialPointName refName, tgName;
	TLength obsDist(0.0), sigma(0.0);

	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
	TLGCTStream lgcLine(obsLine);

	//get the reference name
	lgcLine.TAStreamFormatter::operator>>(refName);

	//get the target name
	lgcLine.TAStreamFormatter::operator>>(tgName);

	//possibly read the observed distance
	if (!lgcLine.atEnd()){
		//lgcLine >> obsDist;
		lgcLine.TAStreamFormatter::operator>>(obsDist);
	}

	//possibly read the measurement's precision
	if (!lgcLine.atEnd()){
		//lgcLine >> sigma;
		lgcLine.TAStreamFormatter::operator>>(sigma);
		sigma *= 0.001;
	}

	///////delete obsTStream;

	//create the vertical distance measurement
	TVerticalDistMeasurement* vD = new TVerticalDistMeasurement(/*refName,*/ tgName, obsDist, sigma);

	vDist = *vD;

	delete vD;

	return *this;
}


TAStreamFormatter& TLGCTStream::operator>>( THorAngleMeasurement& hAng)
{//inputs a horizontal angle measurement from the text stream

	TSpatialPointName stName, tgName;
	TAngle obsAngle(0.0), sigma(0.0);

	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
	TLGCTStream lgcLine(obsLine);

	//get the station name
	lgcLine.TAStreamFormatter::operator>>(stName);

	//get the target name
	lgcLine.TAStreamFormatter::operator>>(tgName);

	//possibly read the observed angle
	if (!lgcLine.atEnd()){
		//lgcLine >> obsAngle;
		lgcLine.TAStreamFormatter::operator>>(obsAngle);
	}

	//possibly read the measurement's precision
	if (!lgcLine.atEnd()){
		//lgcLine >> sigma;
		lgcLine.TAStreamFormatter::operator>>(sigma);
		sigma *= 0.001;
	}

	/////////delete obsTStream;

	//create the vertical distance measurement
	THorAngleMeasurement* ha = new THorAngleMeasurement(/*stName,*/ tgName, obsAngle, sigma);

	hAng = *ha;

	delete ha;

	return *this;
}


TAStreamFormatter& TLGCTStream::operator>>( TZenithDistMeasurement& zDist)
{//inputs a zenithal distance measurement from the text stream

	TSpatialPointName stName, tgName;
	TAngle obsAngle(0.0), sigma(0.0);

	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
	TLGCTStream lgcLine(obsLine);

	//get the station name
	lgcLine.TAStreamFormatter::operator>>(stName);

	//get the target name
	lgcLine.TAStreamFormatter::operator>>(tgName);

	//possibly read the observed angle
	if (!lgcLine.atEnd()){
		//lgcLine >> obsAngle;
		lgcLine.TAStreamFormatter::operator>>(obsAngle);
	}

	//possibly read the measurement's precision
	if (!lgcLine.atEnd()){
		//lgcLine >> sigma;
		lgcLine.TAStreamFormatter::operator>>(sigma);
		sigma *= 0.001;
	}

	////////////////delete obsTStream;

	//create the zenithal distance measurement
	TZenithDistMeasurement* zd = new TZenithDistMeasurement(/*stName,*/ tgName, obsAngle, sigma);

	zDist = *zd;

	delete zd;

	return *this;
}


TAStreamFormatter& TLGCTStream::operator>>( T2DOffsetMeasurement& off2D)
{//inputs a 2D offset measurement from the stream

	TSpatialPointName stName;
	char lineName[50];

	string token;
	TLength vOffset(0.0), hOffset(0.0), vSigma(0.0), hSigma(0.0);

	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
	TLGCTStream lgcLine(obsLine);

	//get the station name
	lgcLine.TAStreamFormatter::operator>>(stName);

	//get the observed spatial line name
	(&lgcLine)->TAStreamFormatter::operator>>(lineName);
	string sLineName(lineName);

	while (!lgcLine.atEnd()){

		bool knownToken = false;
		//read the token identifying the meaning of the next numerical value to be read
		(&lgcLine)->TAStreamFormatter::operator>>(token);

		if (token == string("DH")){
			//lgcLine >> hOffset;
			lgcLine.TAStreamFormatter::operator>>(hOffset);
			knownToken = true;
		}
		if (token == string("DV")){
			//lgcLine >> vOffset;
			lgcLine.TAStreamFormatter::operator>>(vOffset);
			knownToken = true;
		}
		if (token == string("SH")){
			//lgcLine >> hSigma;
			lgcLine.TAStreamFormatter::operator>>(hSigma);
			hSigma *= 0.001;
			knownToken = true;
		}
		if (token == string("SV")){
			//lgcLine >> vSigma;
			lgcLine.TAStreamFormatter::operator>>(vSigma);
			vSigma *= 0.001;
			knownToken = true;
		}
		if (!knownToken){
			double ignored;
			(&lgcLine)->TAStreamFormatter::operator>>(ignored);
		}
	}


	////////////////////delete obsTStream;

	//create the 2D offset measurement
	T2DOffsetMeasurement* offset2D =
		new T2DOffsetMeasurement(/*stName,*/ sLineName, hOffset, hSigma, vOffset, vSigma);

	//temporary
	//offset2D->disactivateVOffset();

	off2D = *offset2D;

	delete offset2D;

	return *this;
}



/*

void TLGCTStream::setWPSFilter(TWPSFilter::EWPSMeasType measType)
{//sets the type of WPS measurements filter to be used

	fWPSFilter = 0;

	switch(measType){

		case TWPSFilter::kTrans:
			fWPSFilter = TWPSTFilter::instance();
			break;

		case TWPSFilter::kVert:
			fWPSFilter = TWPSVFilter::instance();
			break;

		case TWPSFilter::k2D:
			fWPSFilter = TWPS2DFilter::instance();
			break;

		default:
			// undefined wps filter requested
			assert( fWPSFilter != 0 );
			break;
	}
}
*/
/*
TAStreamFormatter& TLGCTStream::operator>>( TWPSMeasurement& wps)
{//inputs a WPS measurement from the text stream

	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
/*	TLGCTStream lgcLine(obsLine);

	// pass on the request to the wps filter Strategy
	fWPSFilter->input(lgcLine, wps);

	return *this;
}
*/
/*
void TLGCTStream::setWTWFilter(TWTWFilter::EWTWMeasType measType)
{//sets the type of WTW measurements filter to be used

	fWTWFilter = 0;

	switch(measType){

		case TWTWFilter::kHoriz:
			fWTWFilter = TWTWHFilter::instance();
			break;

		case TWTWFilter::kVert:
			fWTWFilter = TWTWVFilter::instance();
			break;

		case TWTWFilter::k2D:
			fWTWFilter = TWTW2DFilter::instance();
			break;

		default:
			// undefined wire to wire filter requested
			assert( fWTWFilter != 0 );
			break;
	}
}
*/
/*
TAStreamFormatter& TLGCTStream::operator>>( TWireToWireDistMeas& wtw)
{//inputs a wire to wire distance measurement from the text stream
	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
/*	TLGCTStream lgcLine(obsLine);

	// pass on the request to the WTW filter Strategy
	fWTWFilter->input(lgcLine, wtw);

	return *this;
}
*/
/*
TAStreamFormatter& TLGCTStream::operator>>( TRadialOffsetCnstr& roc)
{//inputs a radial offset constraint from the text stream

	TSpatialPointName ptName;
	TAngle cBearing(0.0);
	TLength cWidth(0.0);

	this->skipCommentLines();

	string obsLine = this->readLine();
	/*QTextStream* obsTStream = new QTextStream(&obsLine, IO_ReadOnly);
	TLGCTStream lgcLine(obsTStream);*/
/*	TLGCTStream lgcLine(obsLine);

	//get the constrained point's name
	lgcLine.TAStreamFormatter::operator>>(ptName);

	//get the constraint's bearing
	//lgcLine >> cBearing;
	lgcLine.TAStreamFormatter::operator>>(cBearing);

	//possibly read the constraint's width
	if (!lgcLine.atEnd()){
		//lgcLine >> cWidth;
		lgcLine.TAStreamFormatter::operator>>( cWidth);

		cWidth *= 0.001;
	}

	///////////delete obsTStream;

	//create the radial offset constraint
	TRadialOffsetCnstr* rocn = new TRadialOffsetCnstr(ptName, cWidth, cBearing);

	roc = *rocn;

	delete rocn;

	return *this;
}
*/

TAStreamFormatter  &TLGCTStream::operator<<( const TSpatialPosition &position )
{//output a spatial position to the text stream	

	//format the stream with the information from the PointFormat
	fIOStream->width(fPointFormat.getCoordWidth());
	fIOStream->precision(fPointFormat.getCoordPrecision());
	

	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, position);

	return *this;
}

/*
/////////////////////////////////////////////////////
//outputs a spatial position error to the text stream
/////////////////////////////////////////////////////
TAStreamFormatter  &TLGCTStream::operator<<( const TSpatialPositionError &posEr )
{	

	//format the stream with the information from the PointFormat
	fIOStream->width(fPointFormat.getCoordErrorWidth());
	fIOStream->precision(fPointFormat.getCoordErrorPrecision());

	//build a position with sigmaxx, sigmayy and sigmazz
	TLength sigmaxx = posEr.getSigmaXX();
	TLength sigmayy = posEr.getSigmaYY();
	TLength sigmazz = posEr.getSigmaZZ();

	TSpatialPosition position(posEr.getReferenceFrame());
	TPositionVector posVec(sigmaxx, sigmayy, sigmazz);
	position.setXYZ(posVec);

	position.setXCoordStatus(sigmaxx.getStatus());
	position.setYCoordStatus(sigmayy.getStatus());
	position.setZCoordStatus(sigmazz.getStatus());
	

	// pass on the request to the coordinate system filter Strategy
	fPosFilter->output(*this, position);

	return *this;
}
*/


TAStreamFormatter  &TLGCTStream::operator<<( const TSpatialPoint &point )
{//output a spatial point to the text stream	
	
	// output the point name
	this->TAStreamFormatter::operator<<(point.getName()); 
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


TAStreamFormatter  &TLGCTStream::operator<<( TLSCalcSpPoint &lscpt )
{//outputs a TLSCalcSpPoint (results output) to the text stream

	// output the point name
	this->TAStreamFormatter::operator<<((lscpt.getNtPoint())->getName()); 
	(*fIOStream) << separator();

	// output the estimated position
	setLengthUnits(TLength::kMetres);
	//if ( (lscpt.getEstimPosition()->getPositionStatus()) != TPositionStatus::kPosNull )
	TPositionVector pv (TCoordSysFactory::k3DCartesian);
	pv=lscpt.getEstimPosition()->getCoordinates(TCoordSysFactory::k3DCartesian);

	if (pv.getStatus()!= TANumericValue::kNull)
	{
		*this << *(lscpt.getEstimPosition());
	}
	(*fIOStream) << separator();

	/*
	// output the precision of the estimated coordinates
	setLengthUnits(TLength::kMillimetres);
		*this << *(lscpt.getSigmaPos());

	(*fStream) << separator();
	*/


	// output the variation between provisional and estimated coordinates
		*this << *(lscpt.getDeltaPos());

	(*fIOStream) << separator();


	return *this;

}

//
// ouputs a TFreeVector. Temporarily empty (not used yet elsewhere)
TAStreamFormatter  &TLGCTStream::operator<<( TFreeVector &fv )
{

	return *this;
}

//
// ouputs a T3DMatrix. Temporarily empty (not used yet elsewhere)
TAStreamFormatter  &TLGCTStream::operator<<( T3DMatrix &m )
{

	return *this;
}


string TLGCTStream::separator() const
{//returns the string used to separate fields
	string sep("   ");
	return sep;
}


void  TLGCTStream::skipCommentLines()
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

