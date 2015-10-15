// TDataParameters.cpp
//
// A class defining the parameters of a data file.
//       filename, filetype, reference frame, coordinate system
//
// Patterns:
//
// 
// Copyright 2000 CERN EST/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include  "TDataParameters.h"
#include  "TAReferenceFrame.h"
#include  "TRefFrameInfo.h"
#include  "TGeodeticRefFrame.h"
#include  "TModifiedLocalAstronomicalRF.h"
#include  "T3DLocalRefFrame.h"

////////////////////////////////////////////////////////////////


//ClassImp(TDataParameters)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TDataParameters::TDataParameters()
{// default constructor
	/*
	punchSeparator = ";";
	resultsSeparator = ";";
	resultsFileFormat = TAStreamFormatter::kColumnFormat;
	punchFileFormat = TAStreamFormatter::kColumnFormat;
	*/
	fRefFrame =0;
	//fLSO.origin = 0;
    fRefFrameEnum = TRefSystemFactory::kNotInGraph;
	fCoordUnit = TDataParameters::kNotDefined;
	fCoordSys = TCoordSysFactory::k3DCartesian;
	fAngleUnits =TAngle::kGons;
	fLengthUnits =TLength::kMetres ;
	fAnglePrecision = TObservationFormat::k10Microgons;
	fLengthPrecision = TObservationFormat::k10Micrometres;
	fCoordPrecision = TPointFormat::kMillimetre;
	fPointNameWidth=7;
}

TDataParameters::TDataParameters(const TDataParameters& original )
: fRefFrame(original.fRefFrame)
, fRefFrameEnum(original.fRefFrameEnum)
, fCoordUnit(original.fCoordUnit)
, fLSO(original.fLSO.get() ? new TLocalSystemOrigin( *original.fLSO.get()) : 0)
, fCoordSys(original.fCoordSys)
, fAngleUnits(original.fAngleUnits)
, fLengthUnits(original.fLengthUnits)
, fAnglePrecision(original.fAnglePrecision)
, fLengthPrecision(original.fLengthPrecision)
, fCoordPrecision(original.fCoordPrecision)
, fPointNameWidth(original.fPointNameWidth)
{
}


TDataParameters::~TDataParameters()
{
}


//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
TDataParameters&  TDataParameters::operator=(TDataParameters rhs )
{
    // rhs is a copy of the source; hard work already done
    swap(rhs); // trade our resources for rhs'es
    return *this;    // our (old) resources get destroyed with rhs (copy)
}

void TDataParameters::swap(TDataParameters & other) throw()
{
    std::swap(fRefFrame,other.fRefFrame);
    std::swap(fRefFrameEnum,other.fRefFrameEnum);
    std::swap(fCoordUnit,other.fCoordUnit);
    std::swap(fLSO,other.fLSO);
    std::swap(fCoordSys,other.fCoordSys);
    std::swap(fAngleUnits,other.fAngleUnits);
    std::swap(fLengthUnits,other.fLengthUnits);
    std::swap(fAnglePrecision,other.fAnglePrecision);
    std::swap(fLengthPrecision,other.fLengthPrecision);
    std::swap(fCoordPrecision,other.fCoordPrecision);
    std::swap(fPointNameWidth,other.fPointNameWidth);
}

bool  TDataParameters::operator==(const TDataParameters& rhs )
{//Equivalence operator
	return	fRefFrameEnum == rhs.getRefFrameEnumerator() && 
		fCoordUnit == rhs.fCoordUnit &&
		//getLocalSystemOrigin().gisement == rhs.getLocalSystemOrigin().gisement &&
		//getLocalSystemOrigin().slope == rhs.getLocalSystemOrigin().slope &&
		//getLocalSystemOrigin().origin == rhs.getLocalSystemOrigin().origin &&
        fLSO == rhs.fLSO &&
		fCoordSys == rhs.getCoordinateSystem() &&
		fAngleUnits == rhs.getAngleUnits() && 
		fLengthUnits == rhs.getLengthUnits() && 
		fAnglePrecision == rhs.getAnglePrecision() && 
		fLengthPrecision == rhs.getLengthPrecision() && 
		fCoordPrecision == rhs.getCoordPrecision() &&
		fPointNameWidth==rhs.fPointNameWidth;
}


bool  TDataParameters::defined()  const  
{ 
	// returns "true" if the parameters are defined

	bool retVal = true;

    if(fRefFrameEnum == TRefSystemFactory::kNotInGraph || fCoordUnit == kNotDefined)
	{
		retVal = false;
	}

	return retVal; 
}


bool	TDataParameters::isOriginExpected() const
{
	return TRefFrameInfo::isLocalRefFrame(fRefFrameEnum);
}



////////////////////////////////////////////////////////////////////////
//function set
////////////////////////////////////////////////////////////////////////

bool  TDataParameters::setRefFrame(TRefSystemFactory::ERefFrame rf)
{//! set the reference system identifier
	// returns "true" if the parameters are set correctly
	bool retVal = true;
	
    if( rf != TRefSystemFactory::kNotInGraph && fRefFrame == 0)
	{
		fRefFrameEnum = rf;
		fLSO.reset();
		
		if(	fRefFrameEnum != TRefSystemFactory::kCGRF && fRefFrameEnum != TRefSystemFactory::kWGS84 && 
			fRefFrameEnum != TRefSystemFactory::kROMA40 && fRefFrameEnum != TRefSystemFactory::kITRF97 &&
			fRefFrameEnum != TRefSystemFactory::kETRF93)
		{//set automatically metric
			setUnits(TDataParameters::kMetric );
		}
	}
	else
	{
		retVal = false;
	}

	return retVal;
}

	
bool TDataParameters::setCoordSys(const TCoordSysFactory::ECoordSys& idNum)
{//!set the coordinate system
	bool retVal = true;

	if( idNum != TCoordSysFactory::k2DCartesian )
	{
		fCoordSys = idNum;
	}
	else
	{
		retVal = false;
	}

	return retVal;
}


bool  TDataParameters::setUnits( const TDataParameters::ECoordUnit& units )
{//! set the coordinate units
	// returns "true" if the parameters are set correctly
	bool  retVal = true;
	
	fCoordUnit= units;

	if(	fRefFrameEnum == TRefSystemFactory::kCGRF || fRefFrameEnum == TRefSystemFactory::kWGS84 || 
		fRefFrameEnum == TRefSystemFactory::kROMA40 || fRefFrameEnum == TRefSystemFactory::kITRF97 ||
		fRefFrameEnum == TRefSystemFactory::kETRF93 || fRefFrameEnum == TRefSystemFactory::kCH1903plus)
	{
		if ( units == kDMS )
		{
			fAngleUnits = TAngle::kDMS;  
			setCoordSys(TCoordSysFactory::kGeodetic);
		}
		else if (units == kGons )
		{
			fAngleUnits = TAngle::kGons;  
			setCoordSys(TCoordSysFactory::kGeodetic);
		}
		else if (units == kMetric )
		{
			fAngleUnits = TAngle::kGons;  
			//setCoordSys(TCoordSysFactory::kGeodetic);
		}
		else
		{
			retVal = false;
		}
	}
	else
	{//only metric system with this referance frame
		if (units == kMetric )
		{
			fAngleUnits = TAngle::kGons;  
		}
		else
		{
			fCoordUnit = kNotDefined;
			retVal = false;
		}
	}
	return  retVal;
}


bool  TDataParameters::setAngUnits( const TAngle::EUnits& un)
{//! set the angle units
	fAngleUnits = un;  
    return true;
	
}


bool  TDataParameters::setLenUnits( const TLength::EUnits& un)
{//! set the length units
	// returns "true" if the parameters are set correctly
	bool  retVal = true;

	if ( un > 0  &&  un < 3 )
	{
		fLengthUnits = un;  
	}
	else
	{
		retVal = false;
	}

	return  retVal;
}

//! set the coordinate precision 
void  TDataParameters::setCoordPrecision( const TPointFormat::ECoordPrecision precision )  
{
	fCoordPrecision = precision; 
	return; 
}

//! set angle precision 
void  TDataParameters::setAnglePrecision(const TObservationFormat::EAnglePrecision precision )  
{
	fAnglePrecision = precision; 
	return; 
}

//! set length precision 
void  TDataParameters::setLengthPrecision(const TObservationFormat::ELengthPrecision precision )  
{
	fLengthPrecision = precision; 
	return; 
}

void TDataParameters::setPrecision(int precision)
{

		TObservationFormat::ELengthPrecision observationPrecision;
		TPointFormat::ECoordPrecision pointPrecision;

		//Should be an integer between 0 (m) and 7 (100nm)
		switch(precision){
	
		case 0:
			observationPrecision = TObservationFormat::kMetre;
			pointPrecision = TPointFormat::kMetre; break;
		case 1:
			observationPrecision = TObservationFormat::k100Millimetres; 
			pointPrecision = TPointFormat::k100Millimetres; break;
		case 2:
			observationPrecision = TObservationFormat::k10Millimetres; 
			pointPrecision = TPointFormat::k10Millimetres; break;
		case 3:
			observationPrecision = TObservationFormat::kMillimetre; 
			pointPrecision = TPointFormat::kMillimetre; break;
		case 4:
			observationPrecision = TObservationFormat::k100Micrometres; 
			pointPrecision = TPointFormat::k100Micrometres; break;
		case 5:
			observationPrecision = TObservationFormat::k10Micrometres;
			pointPrecision = TPointFormat::k10Micrometres; break;
		case 6:
			observationPrecision = TObservationFormat::kMicrometre;
			pointPrecision = TPointFormat::kMicrometre; break;
		case 7:
			observationPrecision = TObservationFormat::k100Nanometres;
			pointPrecision = TPointFormat::k100Nanometres; break;
		default:
			observationPrecision = TObservationFormat::k10Micrometres; 
			pointPrecision = TPointFormat::k10Micrometres; break;

		}

		setCoordPrecision(pointPrecision); 
		setLengthPrecision(observationPrecision);
		setAnglePrecision(TObservationFormat::k10Microgons);
}

//! set point name's width precision 
void  TDataParameters::setPointNameWidth(const int width )  
{
	fPointNameWidth = width; 
	return; 
}


bool TDataParameters::setLocalSystemOrigin(const TLocalSystemOrigin & LSO)
{
    if(TRefFrameInfo::isLocalRefFrame(fRefFrameEnum)
		&& fRefFrame == 0)
	{
		//fLSO = LSO;
        fLSO.reset(new TLocalSystemOrigin(LSO));
		return true;
	}
	return false;
}
bool TDataParameters::setLocalSystemOrigin(std::shared_ptr<TLocalSystemOrigin> lso)
{
    if(TRefFrameInfo::isLocalRefFrame(fRefFrameEnum)
		&& fRefFrame == 0)
	{
        fLSO = lso;
        return true;
    }
    return false;
}


//////////////////////////////////////////////////////////////////////
//get Functions
//////////////////////////////////////////////////////////////////////
TAReferenceFrame*  TDataParameters::getRefFrame() const
{//! get the reference system identifier
    return TRefFrameInfo::getReferenceFrame(fRefFrameEnum, fLSO.get());;
//		switch (fRefFrameEnum)
//		{
//			case TDataParameters::kCCS:
//				//CERN XYZ (kCCS)
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
//				break;
//
//			case TDataParameters::kCernXYHg00Machine:
//				//CERN XYHg LHC (kCernXYHg00Machine)
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg00Machine));
//				break;
//
//			case TDataParameters::kCernXYHg85Machine:
//				//CERN XYHg LHC (kCernXYHg85Machine)
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg85Machine));
//				break;
//
//			case TDataParameters::kCERNXYHsSphereSPS:
//				//CERN XYHs SPS (kCERNXYHsSphereSPS)
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCERNXYHsSphereSPS));
//				break;
//
//			case TDataParameters::kCernXYHg85:
//				//CERN XYHg topo (kCernXYHg85Topo) kCernXYHg85
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernXYHg85));
//				break;
//
//			case TDataParameters::kCernX0Y0He:
//				//CERN X0Y0He MapTransfert (kCernX0Y0He)
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCernX0Y0He));
//				break;
//
///*			case TDataParameters::kMLA1985Machine:
//				//CERN Local Astronomical (kCG1985Machine)
//				{
//					if (fRefFrame == 0)
//					{
//						if(fLSO.origin != 0)
//						{
//							TSpatialPosition* originPointer = fLSO.origin;
//							TSpatialPosition origin (*originPointer);
//							TAngle gis = fLSO.gisement;
//							TAngle slope = fLSO.slope;
//
//							TFreeVector falseOrigin (0,0,0, TCoordSysFactory::k3DCartesian);
//
//							TModifiedLocalAstronomicalRF* MLA = new TModifiedLocalAstronomicalRF("mla", TRefSystemFactory::kCG1985Machine, origin,falseOrigin, gis, slope);
//							fRefFrame = MLA;
//						}
//						else
//						{
//							fRefFrame =  0;
//						}
//					}
//					
//				}
//				break;*/
//
//			case TDataParameters::kMLA2000Machine:
//				//CERN Local Astronomical (kCG2000Machine)
//				{
//					fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getNewLocalRefFrame(fLSO, TRefSystemFactory::kCG2000Machine));
//				}
//				break;
//
//			case TDataParameters::kMLA1985Machine:
//				//CERN Local Astronomical (kCG1985Machine)
//				{
//					fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getNewLocalRefFrame(fLSO, TRefSystemFactory::kCG1985Machine));
//				}
//				break;
//
//
//			case TDataParameters::kROMA40:
//				//ROMA40
//				{
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kROMA40));
//				}
//				break;
//
//			case TDataParameters::kWGS84:
//				//WGS84
//				{
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kWGS84));
//				}
//				break;
//
//			case TDataParameters::kITRF97:
//				//CGRF
//				{
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kITRF97));
//				}
//				break;
//
//			case TDataParameters::kCGRF:
//				//CGRF
//				{
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCGRF));
//				}
//				break;
//
//			case TDataParameters::kCernLGatP0:
//				//LGp0
//				{
//				fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kLGp0));
//				}
//				break;
//
//			case kLocalRefFrame:
//				{
//					fRefFrame = (TRefSystemFactory::getRefSystemFactory()->getNewLocalRefFrame());
//				}
//				break;
//
//			case TDataParameters::kUndefined:
//				{
//					fRefFrame =   0;
//				}
//				break;
//
//			default:
//				//Default Value CERN XYZ (kCCS)
//				{
//					fRefFrame =   0;
//				}
//				break;
//		}
//	
//	
//	return fRefFrame;
}


TRefSystemFactory::ERefFrame TDataParameters::getRefFrameEnumerator() const 
{//! get the reference system identifier
	return fRefFrameEnum;
}

TCoordSysFactory::ECoordSys  TDataParameters::getCoordinateSystem()  const
{//! get the coordinate system for the RefSystem data
	return fCoordSys;
}
		

TAngle::EUnits  TDataParameters::getAngleUnits() const
{//! get the angle units
	return fAngleUnits;
}
	

TLength::EUnits  TDataParameters::getLengthUnits() const
{//! get the length units
	return fLengthUnits;
}
	

TObservationFormat::EAnglePrecision  TDataParameters::getAnglePrecision() const
{//! get the angle precision
	return fAnglePrecision;
}

 
TObservationFormat::ELengthPrecision  TDataParameters::getLengthPrecision() const
{//! get the length precision
	return fLengthPrecision;
}



TPointFormat::ECoordPrecision  TDataParameters::getCoordPrecision() const
{//! get the coord precision 
	return fCoordPrecision;
}



int  TDataParameters::getPointNameWidth() const
{//! get the coord precision 
	return fPointNameWidth;
}


std::shared_ptr<TLocalSystemOrigin> TDataParameters::getLocalSystemOrigin() const
{
	return fLSO;
}
/*
void TDataParameters::setResultsFileFormat(TAStreamFormatter::ETextFormat resFileFrmt)
{
	resultsFileFormat = resFileFrmt;
}


TAStreamFormatter::ETextFormat TDataParameters::getResultsFileFormat()
{
	return resultsFileFormat;
}

void TDataParameters::setPunchFileFormat(TAStreamFormatter::ETextFormat puncFileFrmt)
{
	punchFileFormat = puncFileFrmt;
}

TAStreamFormatter::ETextFormat TDataParameters::getPunchFileFormat()
{
	return punchFileFormat;
}

void TDataParameters::setPunchSeparator(string seperator)
{
	punchSeparator = seperator;
}

void TDataParameters::setResultsSeparator(string seperator)
{
	resultsSeparator = seperator;
}

string TDataParameters::getResultsSeparator()
{
	return resultsSeparator;
}

string TDataParameters::getPunchSeparator()
{
	return punchSeparator;
}
*/

/////////////////////////////////////////////////////////////////////////////
//end
/////////////////////////////////////////////////////////////////////////////









