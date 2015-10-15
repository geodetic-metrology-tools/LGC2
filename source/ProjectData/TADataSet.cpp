//  TADataSet.cpp
//
/*  A spatial point data set for geodetic transformation programs.  
  
    Patterns:
  
   
    Copyright 2002 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include "TSpatialPoint.h"
#include  "TADataSet.h"
//#include  "TCSGEOFileReader.h"
//#include  "TCSGEOFileWriter.h"


////////////////////////////////////////////////////////////////


//ClassImp(TADataSet)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TADataSet::TADataSet() 
{// default constructor
	fSaved = true;
	fPunchSeparator = " ";
	fResultsSeparator = " ";
	fResultsFileFormat = TAStreamFormatter::kColumnFormat;
	fPunchFileFormat = TAStreamFormatter::kColumnFormat;
	fLineSpacing = true;
	fFormatType = "COL";
}



TADataSet::TADataSet(const TFileParameters& fp, const TDataParameters& dp)
{
	fPunchSeparator = "";
	fResultsSeparator = "   ";
	fResultsFileFormat = TAStreamFormatter::kColumnFormat;
	fPunchFileFormat = TAStreamFormatter::kColumnFormat;
	fLineSpacing = true;
	fSaved = false;
	fFileParams = fp;
	fDataParams = dp;
	fFormatType = "COL";
}

TADataSet::TADataSet(const TDataParameters& dp)
{
	fPunchSeparator = "";
	fResultsSeparator = "   ";
	fResultsFileFormat = TAStreamFormatter::kColumnFormat;
	fPunchFileFormat = TAStreamFormatter::kColumnFormat;
	fLineSpacing = true;
	fSaved = false;
	fDataParams = dp;
	fFormatType = "COL";
}

TADataSet::TADataSet( const  TADataSet& original )
{	// copy constructor
	fSaved = original.fSaved;
	fFileParams = original.fFileParams;
	fDataParams = original.fDataParams;
	fFormatType = "COL";
}


TADataSet::~TADataSet()
{
}




//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TADataSet&  TADataSet::operator=(const TADataSet& right)
{	// Copy Assignment operator

	if (this != &right)
	{
		fSaved = right.fSaved;
		fFileParams = right.fFileParams;
		fDataParams = right.fDataParams;

	}

	return *this;
}


bool  TADataSet::fileKnown() const
{// indicates if file parameters have been set
	return !(fFileParams.getFileName()).empty();
}

/*
// clear all the data
void  TADataSet::fileDeleted()
{
	fFileParams.defaultValues();
	return;
}*/



TFileParameters  TADataSet::getFileParams() const
{// returns a copy of the file parameters
	return fFileParams;
}


void  TADataSet::setFileParams(const TFileParameters& fp)
{//set the file parameters
	fFileParams = fp;
	return;
}


TDataParameters  TADataSet::getDataParams() const
{// returns a copy of the data parameters
	return fDataParams;
}


void  TADataSet::setDataParams(const TDataParameters& dp)
{//set the data parameters
	fDataParams = dp;
	return;
}


bool	TADataSet::isSaved()
{
	return fSaved;
}


void	TADataSet::saved()
{
	fSaved=true;
	return;
}



//////////////////////////////////////////////////////////////////////////
//Wrapper TDataParameters
//////////////////////////////////////////////////////////////////////////

bool	TADataSet::isDataParamsDefined() const
{
	return fDataParams.defined();
}


bool	TADataSet::isOriginExpected() const
{
	return fDataParams.isOriginExpected();
}


bool	TADataSet::setRefFrame(TRefSystemFactory::ERefFrame ref)
{//! set the reference system identifier
	return fDataParams.setRefFrame(ref);
}


bool	TADataSet::setCoordSys(const TCoordSysFactory::ECoordSys& sys)
{//!set the coordinate system
	return fDataParams.setCoordSys(sys);
}


bool	TADataSet::setAngUnits(const TAngle::EUnits& ang)
{//!set angle umits
	return fDataParams.setAngUnits(ang);
}


bool	TADataSet::setLenUnits(const TLength::EUnits& len)
{//!setlength units
	return fDataParams.setLenUnits(len);
}


bool	TADataSet::setUnits( const TDataParameters::ECoordUnit& str)
{//! set the coordinate units
	return fDataParams.setUnits(str);
}


void	TADataSet::setAnglePrecision(const TObservationFormat::EAnglePrecision i)
{//! set the angle precision
	fDataParams.setAnglePrecision(i);
	return;
}


void	TADataSet::setLengthPrecision(const TObservationFormat::ELengthPrecision i)
{//! set the length precision 
	fDataParams.setLengthPrecision(i);
	return;
}


void	TADataSet::setCoordPrecision(const TPointFormat::ECoordPrecision prec)
{//!set coordinate precision
	fDataParams.setCoordPrecision(prec);
	return;
}




//! set point name's width precision 
void  TADataSet::setPointNameWidth(const int width )  
{
	fDataParams.setPointNameWidth(width ); 
	return; 
}




bool	TADataSet::setLocalSystemOrigin(std::shared_ptr<TLocalSystemOrigin> lso)
{
	return fDataParams.setLocalSystemOrigin(lso);
}


TAReferenceFrame*	TADataSet::getRefFrame()
{//! get the reference system identifier
	return fDataParams.getRefFrame();
}


TRefSystemFactory::ERefFrame	TADataSet::getRefFrameEnumerator() const
{//! get the reference system identifier
	return fDataParams.getRefFrameEnumerator();
}


TCoordSysFactory::ECoordSys		TADataSet::getCoordinateSystem()  const
{//! get the coordinate system for the RefSystem data
	return fDataParams.getCoordinateSystem();
}


TAngle::EUnits		TADataSet::getAngleUnits() const
{//! get the angle units
	return fDataParams.getAngleUnits();
}


TLength::EUnits		TADataSet::getLengthUnits() const
{//! get the length units
	return fDataParams.getLengthUnits();
}


TObservationFormat::EAnglePrecision		TADataSet::getAnglePrecision() const
{//! get the angle precision
	return fDataParams.getAnglePrecision();
}


TObservationFormat::ELengthPrecision		TADataSet::getLengthPrecision() const
{//! get the length precision 
	return fDataParams.getLengthPrecision();
}


TPointFormat::ECoordPrecision  TADataSet::getCoordPrecision() const
{//! get the coord precision 
	return fDataParams.getCoordPrecision();
}


int  TADataSet::getPointNameWidth() const
{//! get the coord precision 
	return fDataParams.getPointNameWidth();
}


std::shared_ptr<TLocalSystemOrigin> TADataSet::getLocalSystemOrigin() const
{
	return fDataParams.getLocalSystemOrigin();
}

//////////////////////////////////////////////////////////////////////////
//Wrapper TFileParameters
//////////////////////////////////////////////////////////////////////////
bool  TADataSet::setFileName( const string& name)
{//! set the full filename, both path and name
	return fFileParams.setFileName(name);
}


bool  TADataSet::setTypeFormat( const string& format)
{//! set the file type
	return fFileParams.setTypeFormat(format);
}


void  TADataSet::defaultValues()
{//! clear the file parameters and set the default values 
	fFileParams.defaultValues();
	return;
}


string  TADataSet::getFileName()  const 
{//! get the full filename, both path and name
	return fFileParams.getFileName();
}


string  TADataSet::getName()  const
{//! get the name of the file
	return fFileParams.getName();
}


string  TADataSet::getPath()  const
{//! get the path for the file
	return fFileParams.getPath();
}


TFileParameters::EFileType  TADataSet::getType()  const
{//! get the file type
	return fFileParams.getType();
}


TFileParameters::ETextFormat  TADataSet::getFormat() const
{//! get the text stream's format
	return fFileParams.getFormat();
}


bool  TADataSet::isFileParamsDefined()  const
{//! indicates if the file name, type and format have been set
	return fFileParams.defined();
}

void TADataSet::setResultsFileFormat(TAStreamFormatter::ETextFormat resFileFrmt)
{
	fResultsFileFormat = resFileFrmt;
}


TAStreamFormatter::ETextFormat TADataSet::getResultsFileFormat()
{
	return fResultsFileFormat;
}

void TADataSet::setPunchFileFormat(TAStreamFormatter::ETextFormat puncFileFrmt)
{
	fPunchFileFormat = puncFileFrmt;
}

TAStreamFormatter::ETextFormat TADataSet::getPunchFileFormat()
{
	return fPunchFileFormat;
}

void TADataSet::setPunchSeparator(string seperator)
{
	fPunchSeparator = seperator;
}

void TADataSet::setResultsSeparator(string seperator)
{
	fResultsSeparator = seperator;
}

string TADataSet::getResultsSeparator()
{
	return fResultsSeparator;
}

string TADataSet::getPunchSeparator()
{
	return fPunchSeparator;
}

bool TADataSet::isSpaceBetweenData()
{
	return fLineSpacing;
}

void TADataSet::setSpaceBetweenData()
{
	fLineSpacing = true;
}

void TADataSet::setNoSpaceBetweenData()
{
	fLineSpacing = false;
}

string TADataSet::getFormatType()
{
	return fFormatType;
}

void TADataSet::setFormatType(string fmtType)
{
	fFormatType = fmtType;
}


//////////////////////////////////////////////////////////////////////////
//end
//////////////////////////////////////////////////////////////////////////
