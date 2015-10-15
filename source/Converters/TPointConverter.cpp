////////////////////////////////////////////////////////////////////
// TPointConverter.cpp
/*!
Write a output LGC file
Creates a file from the data read and sends the appropriate messages

Patterns:

 
Copyright 2003 CERN EST/SU. All rights reserved.
*/
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	"TPointConverter.h"
#include	"TPointFormat.h"
#include	"TAStreamFormatter.h"


/////////////////////////////////////////////////////////////////////

//ClassImp(TPointConverter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////

#if 0
TPointConverter::TPointConverter(TAStreamFormatter* stream,
								 const TRefSystemFactory::ERefFrame	refFrame):
TAConverter(stream)
{//constructor

	fRefFrame = refFrame;

	TAReferenceFrame* pointRefFrame = stream->getReferenceFrame();
	TAReferenceFrame* ccsRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
	if(pointRefFrame == ccsRefFrame)
		fLocalSys = false;
	else
		fLocalSys = true;
}
#endif

TPointConverter::TPointConverter(TAStreamFormatter* stream,
							const TLGCRefFrame::ERefs	refFrame):
TAConverter(stream)
{//constructor
	fRefFrame = refFrame;

	//This was changed, the previous version is quite weird
	if(refFrame == TLGCRefFrame::kOLOC)
		fLocalSys = true;
	else
		fLocalSys = false;
}

TPointConverter::~TPointConverter()
{//destructor
}


TPointConverter& 	TPointConverter::operator=(const TPointConverter& source)
{//! copy assignment operator
	this->TAConverter::operator =(source);
	fLocalSys = source.fLocalSys;
	fRefFrame = source.fRefFrame;
	return *this;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////////////
//Name
//////////////////////////////////////////////////////////////////////////////////
#if 0
void	TPointConverter::writeName(LSPosVecConstIter pt, int width)
{
	TAStreamFormatter*	stream = getStream();

	//Write point name
	stream->width(width);
	(*stream)<<left<<(pt->getName())<<right << this->getSeparator();
	return;
}
#endif

void	TPointConverter::writeName(string ptName, int width)
{
	TAStreamFormatter*	stream = getStream();

	//Write point name
	stream->width(width);
	(*stream)<<left<<(ptName)<<right << this->getSeparator();
	return;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//X Y Z and H
////////////////////////////////////////////////////////////////////////////////////////////////

//Write position vector in any 
void	TPointConverter::writeXYZandH(const TPositionVector& pt)
{
	TAStreamFormatter*	stream = getStream();
	string				separator = getSeparator();

	//get Coordinate as a TPositionVector
	(*stream)<<(pt);
	return;
}


void	TPointConverter::writeXYZandH(const TPositionVector& pt, TReal heightH)
{
	TAStreamFormatter*	stream = getStream();
	int					coordWidth = getCoordWidth();
	string				separator = getSeparator();

	//get Coordinate as a TPositionVector
	(*stream)<<(pt);

	stream->width(coordWidth);
	(*stream)<<heightH << separator;

	return;
}


void	TPointConverter::writeXYZandH(const TAdjustablePoint& pt)
{
	TAStreamFormatter*	stream = getStream();
	int					coordWidth = getCoordWidth();
	string				separator = getSeparator();

	//get Coordinate as a TPositionVector
	(*stream)<<(pt.getEstimatedValue());

	//write H if point's refrence frame is CCS
	if(!isInLocalSystem())
	{
		stream->width(coordWidth);
		(*stream)<<(pt.getHEstValue()) << separator;
	}

	return;
}

void	TPointConverter::writeXYZandH(	const int width,
										const int precision,
										const TLength::EUnits unit,
										const string separator,
										TAdjustablePoint pt)
{
	TAStreamFormatter*	stream = getStream();

	stream->setLengthUnits(unit);
	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);

	//write X
	(*stream) << pt.getEstimatedValue()[0] << separator;

	//write Y
	(*stream) << pt.getEstimatedValue()[1] << separator;

	//write Z
	(*stream) << pt.getEstimatedValue()[2] << separator;

	//write H if point's refrence frame is CCS
	if(!isInLocalSystem())
		(*stream) << (pt.getHEstValue()) << separator;
}

#if 0

void	TPointConverter::writeXYZorH(	const int width,
										const int precision,
										const TLength::EUnits unit,
										const string separator,
										LSPosVecConstIter pt)
{
	if(!isInLocalSystem())
	{writeXYH(width, precision, unit, separator, pt);}
	else
	{writeXYZ(width, precision, unit, separator, pt);}
	return;
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
//X Y H
////////////////////////////////////////////////////////////////////////////////////////////////
void	TPointConverter::writeXYH(	const int width,
									const int precision,
									const TLength::EUnits unit,
									const string separator,
									TAdjustablePoint pt)
{
	TAStreamFormatter*	stream = getStream();

	stream->setLengthUnits(unit);
	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);

	//write X
	(*stream)<< pt.getEstimatedValue().getX().getValue() << separator;

	//write Y
	(*stream)<< pt.getEstimatedValue().getY().getValue() << separator;

	//write H if point's refrence frame is CCS
	if(!isInLocalSystem())
		(*stream)<< pt.getHEstValue(); 
	else
		writeString(width, " ");
	
	(*stream) << separator; 
	
	return;
}

#if 0

////////////////////////////////////////////////////////////////////////////////////////////////
//X Y Z 
////////////////////////////////////////////////////////////////////////////////////////////////
void	TPointConverter::writeXYZ(	const int width,
									const int precision,
									const TLength::EUnits unit,
									const string separator,
									LSPosVecConstIter pt)
{
	TAStreamFormatter*	stream = getStream();

	stream->setLengthUnits(unit);
	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);

	//write X
	(*stream)<<pt->getXEstValue()<<separator;

	//write Y
	(*stream)<<pt->getYEstValue()<<separator;

	//write Z
	(*stream)<<pt->getZEstValue() << separator;
	return;
}
#endif

void	TPointConverter::writeXYZ(	const int width,
									const int precision,
									const TLength::EUnits unit,
									const string separator,
									const TPositionVector pt)
{
	TAStreamFormatter*	stream = getStream();

	stream->setLengthUnits(unit);
	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);

	//write X
	(*stream)<<pt.getX()<<separator;

	//write Y
	(*stream)<<pt.getY()<<separator;

	//write Z
	(*stream)<<pt.getZ() << separator;
	return;
}


void	TPointConverter::write3Coordinates(	const int width,
											const int precision,
											const string separator,
											const TPositionVector pt)
{
	TAStreamFormatter*	stream = getStream();

	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);

	//write X
	(*stream)<<pt.getX()<<separator;

	//write Y
	(*stream)<<pt.getY()<<separator;

	//write Z
	if(pt.getCoordSys() == TCoordSysFactory::k3DCartesian)
		(*stream)<<pt.getZ() << separator;
	
	if(pt.getCoordSys() == TCoordSysFactory::k2DPlusH)
		(*stream)<<pt.getH()<< separator;

	return;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//N
////////////////////////////////////////////////////////////////////////////////////////////////


void	TPointConverter::writeN( const int width,
								 const int precision,
								 const TLength::EUnits unit,
								 TAdjustablePoint pt)
{
	TRefSystemFactory::EGeoid geoid(TRefSystemFactory::kNoGeoid);
	
	// Set geoeid depending on the ref frame
	if(fRefFrame ==	TRefSystemFactory::kCERNXYHsSphereSPS)
		geoid = TRefSystemFactory::kCGSphere;
	if(fRefFrame ==	TRefSystemFactory::kCernXYHg85Machine)
		geoid = TRefSystemFactory::kCG1985Machine;
	if(fRefFrame ==	TRefSystemFactory::kCernXYHg00Machine)
		geoid = TRefSystemFactory::kCG2000Machine;
	
	//writeLength(width, precision ,unit , pt.getNEstValue(geoid) );
}


void	TPointConverter::writeCoordinateParam(	const TSpatialStatus::ESpatialStatus status,
												const int width,
												const int precision,
												const string separator,
												const TScalar Xparam,
												const TScalar Yparam,
												const TScalar Zparam,
												const string Dparam,
												bool isCovar)
{
	TAStreamFormatter*	stream = getStream();

	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);
	
	//write X
	if((isCovar &&  (status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy))  || 
		!isCovar && (status == TSpatialStatus::kVx || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy  || status == TSpatialStatus::kVxz))
		(*stream)<<(Xparam);	
	else
		writeString(width, Dparam);

	(*stream)<<(separator);


	//write Y
	if((isCovar &&  (status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxz))  || 
		!isCovar && (status == TSpatialStatus::kVy || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy  || status == TSpatialStatus::kVyz))
		(*stream)<<(Yparam);
	else
		writeString(width, Dparam);

	(*stream)<<(separator);


	//write Z
	if((isCovar &&  (status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVyz))  || 
		!isCovar && (status == TSpatialStatus::kVz || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxz  || status == TSpatialStatus::kVyz))
		(*stream)<<(Zparam);
	else
		writeString(width, Dparam);

	(*stream)<<(separator);

	return;
}


void	TPointConverter::writeCoordinateParam(	const TSpatialStatus::ESpatialStatus status,
												const int width,
												const int precision,
												const TLength::EUnits unit,
												const string separator,
												const TLength Xparam,
												const TLength Yparam,
												const TLength Zparam,
												const string Dparam,
												bool isCovar)
{
	TAStreamFormatter*	stream = getStream();

	stream->setLengthUnits(unit);
	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);
	
	//write X
	if((isCovar &&  (status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy))  || 
		!isCovar && (status == TSpatialStatus::kVx || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy  || status == TSpatialStatus::kVxz))
		(*stream)<<(Xparam);	
	else
		writeString(width, Dparam);

	(*stream)<<(separator);


	//write Y
	if((isCovar &&  (status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxz))  || 
		!isCovar && (status == TSpatialStatus::kVy || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy  || status == TSpatialStatus::kVyz))
		(*stream)<<(Yparam);
	else
		writeString(width, Dparam);

	(*stream)<<(separator);


	//write Z
	if((isCovar &&  (status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVyz))  || 
		!isCovar && (status == TSpatialStatus::kVz || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxz  || status == TSpatialStatus::kVyz))
		(*stream)<<(Zparam);
	else
		writeString(width, Dparam);

	(*stream)<<(separator);

	return;
}

void	TPointConverter::writeCoordinateParam(	const TSpatialStatus::ESpatialStatus status,
												const int width,
												const int precision,
												const string separator,
												const TDouble Xparam,
												const TDouble Yparam,
												const TDouble Zparam,
												const string Dparam)
{
	TAStreamFormatter*	stream = getStream();

	stream->setWidthFormat(width);
	stream->setPrecisionFormat(precision);

	//write X
	if(status == TSpatialStatus::kVx || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy  || status == TSpatialStatus::kVxz)
		(*stream)<<(Xparam);	
	else
		writeString(width, Dparam);

	(*stream)<<(separator);


	//write Y
	if(status == TSpatialStatus::kVy || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxy  || status == TSpatialStatus::kVyz)
		(*stream)<<(Yparam);
	else
		writeString(width, Dparam);

	(*stream)<<(separator);


	//write Z
	if(status == TSpatialStatus::kVz || status == TSpatialStatus::kVxyz || status == TSpatialStatus::kVxz  || status == TSpatialStatus::kVyz)
		(*stream)<<(Zparam);
	else
		writeString(width, Dparam);

	(*stream)<<(separator);

	return;
}