////////////////////////////////////////////////////////////////////
// TFautFileWriter.cpp :Implementation file
// Write an LGC fault detection file, from the use of the keyword *FAUT
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TFautFileWriter.h"

// other forward declarations
#include	<ctime>
#include	"TLGCApplication.h"
#include	"TLGCDataSet.h"
#include	"TSeparatedFormatTStream.h"
#include    "TLGCCalcParams.h"

#include    "THorAngleConverter.h"
#include    "TZenithDistConverter.h"
#include    "GyroscopeStationConverter.h"
#include    "THorDistConverter.h"
#include    "TVertDistConverter.h"
#include    "TSpatialDistConverter.h"
#include    "OffsetToVerticalPlaneConverter.h"
#include    "TOffsetToTheoPlaneConverter.h"
#include    "TRadialOffsetCnstrConverter.h"
#include	"PolarConverter.h"
#include	"OffsetToVerticalLineConverter.h"
#include	"OffsetToSpatialLineConverter.h"



/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TFautFileWriter::TFautFileWriter() : TAFileWriter()
{// default constructor
	fAlpha = LITERAL(0.01);
	fBeta = LITERAL(0.1);
}

TFautFileWriter::TFautFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TAFileWriter(stream, project)
{//constructor
	fAlpha = project->getCalcParams()->getAlpha();
	fBeta = project->getCalcParams()->getBeta();
}

TFautFileWriter::~TFautFileWriter()
{// Destructor
}


//////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC: WRITING RESULTS
//////////////////////////////////////////////////////////////////////////////////////////

void	TFautFileWriter::writeResults(LSCalcDataSet &data, TLGCProject& project)
{
	// initialisation
	init(project.getDataSet(), project.getLSCalcDataSet(), "");
	// write headers
	writeTitle();
	writeDataSummary();

	writeTheodoliteStations(data, project);
	writeEDMStations(data, project);
	
	// ORIE
	if( data.numGyroOrieObs() !=0)
		writeGyroOrieData();

	// ECHO
	if( data.numOffsetToVerPlaneObs() !=0)
		writeOffToVertPlaneObs();

	// ECSP
	if( data.numOffsetToSpaLineObs() !=0)
		writeOffToSpaLineObs();

	// ECVE
	if( data.numOffsetToVerLineObs() !=0)
		writeOffToVertLineObs();

	// RADI
	if( data.numRadOffCnstrObs() !=0)
		writeAllRadOffCnstrObs();

	writeOverallReliability();
}

void	TFautFileWriter::writeEDMStations(LSCalcDataSet &data, TLGCProject& project)
{
	list<EDMStation*> l = project.getDataSet()->getWorkingStations()->getEDMStations();
	list<EDMStation*>::iterator iter = l.begin();

	while (iter != l.end())
	{
		writeEDMStationHeader(*iter);

		writeEDMSpaDistObs((*iter)->getSpatialDistanceROM(), data);

		iter++;
	}
}

void	TFautFileWriter::writeTheoStationHeader(TheodoliteStation* station)
{
	// TODO: implement
}

void	TFautFileWriter::writeEDMStationHeader(EDMStation* station)
{
	// TODO: implement
}

void	TFautFileWriter::writeTheoStationROMHeader(TheodoliteStationROM* rom)
{
	// TODO: implement
}

void	TFautFileWriter::writeTheodoliteStations(LSCalcDataSet &data, TLGCProject& project)
{
	list<TheodoliteStation*> l = project.getDataSet()->getWorkingStations()->getTheodoliteStations();
	list<TheodoliteStation*>::iterator iter = l.begin();

	while (iter != l.end())
	{
		writeTheoStationHeader(*iter);

		list<TheodoliteStationROM*> roms = (*iter)->getTheodoliteStationROMs();
		list<TheodoliteStationROM*>::iterator romsIterator = roms.begin();

		while (romsIterator != roms.end())
		{
			writeTheoStationROMHeader(*romsIterator);

			list<const PolarROM*> p3dROMs = (*romsIterator)->getPolarROMs();
			list<const PolarROM*>::iterator p3dROM = p3dROMs.begin();

			while (p3dROM != p3dROMs.end())
			{
				writePolarData(*p3dROM, data);

				p3dROM++;
			}

			list<const HorizontalAngleROM*> haROMs = (*romsIterator)->getHorizontalAngleROMs();
			list<const HorizontalAngleROM*>::iterator haROM = haROMs.begin();

			while (haROM != haROMs.end())
			{
				writeHorAngData(*haROM, data);

				haROM++;
			}

			list<const ZenithDistanceROM*> zdROMs = (*romsIterator)->getZenithDistanceROMs();
			list<const ZenithDistanceROM*>::iterator zdROM = zdROMs.begin();

			while (zdROM != zdROMs.end())
			{
				writeZenDistData(*zdROM, data);

				zdROM++;
			}

			list<const SpatialDistanceROM*> sdROMs = (*romsIterator)->getSpatialDistanceROMs();
			list<const SpatialDistanceROM*>::iterator sdROM = sdROMs.begin();

			while (sdROM != sdROMs.end())
			{
				writeSpaDistObs(*sdROM, data);

				sdROM++;
			}

			list<const HorizontalDistanceROM*> hdROMs = (*romsIterator)->getHorizontalDistanceROMs();
			list<const HorizontalDistanceROM*>::iterator hdROM = hdROMs.begin();

			while (hdROM != hdROMs.end())
			{
				writeHorDistObs(*hdROM, data);

				hdROM++;
			}

			list<const OffsetToTheodolitePlaneROM*> otpROMs = (*romsIterator)->getOffsetToTheodolitePlaneROMs();
			list<const OffsetToTheodolitePlaneROM*>::iterator otpROM = otpROMs.begin();

			while (otpROM != otpROMs.end())
			{
				writeOffToTheoPlaneObs(*otpROM, data);

				otpROM++;
			}

			romsIterator++;
		}

		iter++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE: TITLE
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeTitle()
{
	TAStreamFormatter* stream =	getStream();

	//write main title
	//write software id.
	(*stream)<<(TLGCApplication::getProgId())<<endl;

	//write software copyright
	(*stream)<<(TLGCApplication::getCopyright())<<endl;

	//last compilation
	(*stream)<<"Last compilation : "<<__DATE__<<endl<<endl<<endl;

	//write title
	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<(TLGCProject::getTitle())<<endl;
	(*stream)<<"RELIABILITY PARAMETERS"<<endl<<endl;
	(*stream)<<endl;

	//write time
    char tmpbuf[128];
    time_t ltime;
	time(&ltime);
    struct tm *today;
	_tzset();
    today = localtime( &ltime );
	string essai = ctime( &ltime );
	strftime( tmpbuf, 128,"CALCULATED %d %B %Y %X", today );
	(*stream)<<tmpbuf<<endl;
	(*stream)<<"*********************************************************************************************************************************** "<<endl<<endl<<endl<<endl;
}

void	TFautFileWriter::writeDataSummary()
{
	TAStreamFormatter* stream =	getStream();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();
	string separator = getSeparator();
	
	//SIGMA ZERO A POSTERIORI
	(*stream) << "A POSTERIORI SIGMA ZERO USED = ";
	stream->setWidthFormat(stream->getObsFormat()->getObsResidualWidth());
	stream->setPrecisionFormat((stream->getObsFormat()->getLengthResidualPrecision())/1000);
	TReal s0(LITERAL(1.0));
	if (isSAPrioriUsed()
		|| 
			(
				(lsCalcDataSet->getS0APosteriori().getValue() < lsCalcDataSet->getChiUpLimit())
				&& (lsCalcDataSet->getS0APosteriori().getValue()>lsCalcDataSet->getChiLoLimit())
			)
		)
	{
		(*stream)<<s0;
		(*stream)<<endl<<endl;
	}
	else
	{
		s0 = lsCalcDataSet->getS0APosteriori().getValue();
		(*stream)<<s0;
		(*stream)<<endl<<endl;
	}

	//SIGNIFICANCE LEVEL
	(*stream) << "SIGNIFICANCE LEVEL FOR TESTING WI, ALPHA =";
	stream->setf(ios::fixed,ios::floatfield);
	stream->width(5);
	stream->precision(1);
	(*stream) << right << LITERAL(100.0)*fAlpha << " %" << separator;
	(*stream) << "OR EXPRESSED AS CONFIDENCE LEVEL, (1-ALPHA) = ";
	stream->width(5);
	stream->precision(1);
	(*stream) << right << LITERAL(100.0)*(1-fAlpha) << " %";
	(*stream)<< endl << endl;
	
	// POWER OF TEST (1-beta)
	(*stream) << "POWER OF TEST TO DETERMINE NABLA AND DELTY, (1-BETA) = ";
	stream->width(5);
	stream->precision(1);
	(*stream) << right << 100*(1-fBeta) << " %";
	(*stream)<< endl << endl << endl << endl;

}


/////////////////////////////////////////////////////////////////////////////////////////////////
//POLAR 3D
/////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writePolarData(const PolarROM* rom, LSCalcDataSet &data)
{
	TAStreamFormatter& stream =	getStreamRef();

	PolarConverter converter (getStreamRef());
	list<const PolarMeasurement*> l = rom->getMeasurements();
	list<const PolarMeasurement*>::iterator obsIt = l.begin();
	list<const PolarMeasurement*>::iterator end = l.end();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kPLR) << " : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	while (obsIt != end)
	{
		converter.writeReliabilityData(*(data.getCalcPolarByID((*obsIt)->getObservationID())));
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//HORIZONTAL ANGLE
/////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeHorAngData(const HorizontalAngleROM* rom, LSCalcDataSet &data) 
{
	TAStreamFormatter& stream =	getStreamRef();

	THorAngleConverter converter (getStreamRef());
	list<const THorAngleMeasurement*> l = rom->getMeasurements();
	list<const THorAngleMeasurement*>::iterator obsIt = l.begin();
	list<const THorAngleMeasurement*>::iterator end = l.end();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kANGL) << " : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	while (obsIt != end)
	{
		converter.writeReliabilityData(*(data.getCalcHorAngByID((*obsIt)->getObservationID())));
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//HORIZONTAL DISTANCE
/////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeHorDistObs(const HorizontalDistanceROM* rom, LSCalcDataSet &data)
{
	TAStreamFormatter& stream =	getStreamRef();

	THorDistConverter converter (stream);
	list<const THorizontalDistMeas<TheodoliteTarget>*> l = rom->getMeasurements();
	list<const THorizontalDistMeas<TheodoliteTarget>*>::iterator obsIt = l.begin();
	list<const THorizontalDistMeas<TheodoliteTarget>*>::iterator end = l.end();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kDHOR) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	while (obsIt != end)
	{
		converter.writeReliabilityData(*(data.getCalcHorDistByID((*obsIt)->getObservationID())));
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
} 

/////////////////////////////////////////////////////////////////////////////////////////////////
//VERTICAL DISTANCE
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeVertDistObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TVertDistConverter converter (stream);
	LSVertDistIter	obsIt = lsCalcDataSet->beginLSVertDist();
	LSVertDistIter	end = lsCalcDataSet->endLSVertDist();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kDVER) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	while (obsIt != end)
	{
		converter.writeReliabilityData(*obsIt);
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//ZENITHAL DISTANCE
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeZenDistData(const ZenithDistanceROM* rom, LSCalcDataSet &data)
{
	TAStreamFormatter& stream =	getStreamRef();

	TZenithDistConverter converter (stream);

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kZEND) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	list<const TZenithDistMeasurement*> l = rom->getMeasurements();
	list<const TZenithDistMeasurement*>::iterator obsIt = l.begin();
	list<const TZenithDistMeasurement*>::iterator end = l.end();

	while (obsIt != end)
	{
		converter.writeReliabilityData(*(data.getCalcZenDistByID((*obsIt)->getObservationID())));
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}


 
/////////////////////////////////////////////////////////////////////////////////////////////////
//SPATIAL DISTANCE
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeSpaDistObs(const SpatialDistanceROM* rom, LSCalcDataSet &data)
{
	TAStreamFormatter& stream =	getStreamRef();

	TSpatialDistConverter converter (stream);

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kDRDL) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write values
	list<const TSpatialDistMeasurement<TheodoliteTarget>*> l = rom->getMeasurements();
	list<const TSpatialDistMeasurement<TheodoliteTarget>*>::iterator obsIt = l.begin();
	list<const TSpatialDistMeasurement<TheodoliteTarget>*>::iterator end = l.end();
			
	while (obsIt != end)
	{
		converter.writeReliabilityData<TheodoliteTarget>(*(data.getCalcSpaDistByID((*obsIt)->getObservationID())));
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}

void TFautFileWriter::writeEDMSpaDistObs(const EDMSpatialDistanceROM* rom, LSCalcDataSet &data)
{
	TAStreamFormatter& stream =	getStreamRef();

	TSpatialDistConverter converter (stream);

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kDSPT) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write values
	list<const TSpatialDistMeasurement<EDMTarget>*> l = rom->getMeasurements();
	list<const TSpatialDistMeasurement<EDMTarget>*>::iterator obsIt = l.begin();
	list<const TSpatialDistMeasurement<EDMTarget>*>::iterator end = l.end();
	
	while (obsIt != end)
	{
		converter.writeReliabilityData<EDMTarget>(*(data.getCalcEDMSpaDistByID((*obsIt)->getObservationID())));
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//OFFSET TO VERTICAL LINE (ECVE)
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeOffToVertLineObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	OffsetToVerticalLineConverter converter (stream);
	LSOffsetToVerLineIter	obsIt = lsCalcDataSet->beginLSOffsetToVerLine();
	LSOffsetToVerLineIter	end = lsCalcDataSet->endLSOffsetToVerLine();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kECVE) <<" : " << endl;
	converter.writeReliabilityHeader();
		
	while (obsIt != end)
	{
		converter.writeReliabilityData(*obsIt);
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//OFFSET TO SPATIAL LINE (ECSP)
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeOffToSpaLineObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	OffsetToSpatialLineConverter converter (stream);
	LSOffsetToSpaLineIter	obsIt = lsCalcDataSet->beginLSOffsetToSpaLine();
	LSOffsetToSpaLineIter	end = lsCalcDataSet->endLSOffsetToSpaLine();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kECSP) <<" : " << endl;
	converter.writeReliabilityHeader();
		
	while (obsIt != end)
	{
		converter.writeReliabilityData(*obsIt);
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//OFFSET TO VERTICAL PLANE (ECHO)
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeOffToVertPlaneObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	OffsetToVerticalPlaneConverter converter (stream);
	LSOffsetToVerPlaneIter	obsIt = lsCalcDataSet->beginLSOffsetToVerPlane();
	LSOffsetToVerPlaneIter	end = lsCalcDataSet->endLSOffsetToVerPlane();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kECHO) <<" : " << endl;
	converter.writeReliabilityHeader();
		
	while (obsIt != end)
	{
		converter.writeReliabilityData(*obsIt);
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//OFFSET TO THEO.  LINE (ECTH)
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeOffToTheoPlaneObs(const OffsetToTheodolitePlaneROM* rom, LSCalcDataSet &data)
{
	TAStreamFormatter& stream =	getStreamRef();

	TOffsetToTheoPlaneConverter converter (stream);
	list<const TOffsetToTheoPlaneMeasurement*> l = rom->getMeasurements();
	list<const TOffsetToTheoPlaneMeasurement*>::iterator obsIt = l.begin();
	list<const TOffsetToTheoPlaneMeasurement*>::iterator end = l.end();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kECTH) <<" : " << endl;
	converter.writeReliabilityHeader();

	while (obsIt != end)
	{
		converter.writeReliabilityData(*(data.getOffTheoPlaneByID((*obsIt)->getObservationID())));
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//GYRO ORIENTATION
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeGyroOrieData()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	GyroscopeStationConverter converter (stream);
	LSGyroOrieIter	obsIt = lsCalcDataSet->beginLSGyroOrie();
	LSGyroOrieIter	end = lsCalcDataSet->endLSGyroOrie();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kORIE) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	while (obsIt != end)
	{
		converter.writeReliabilityData(*obsIt);
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}
	


/////////////////////////////////////////////////////////////////////////////////////////////////
//RADIAL OFFSET CONSTRAINT
/////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeAllRadOffCnstrObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TRadialOffsetCnstrConverter converter (stream);
	LSRadOffCnstrIter obsIt = lsCalcDataSet->beginLSRadOffCnstr();
	LSRadOffCnstrIter end = lsCalcDataSet->endLSRadOffCnstr();
		
	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kRADI) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	while (obsIt != end)
	{
		converter.writeReliabilityData(*obsIt);
		obsIt++;
	}
	if (!fDataSet->isSpaceBetweenData())
		(stream)<< endl;
	(stream)<<endl;
	return;
}



void TFautFileWriter::writeOverallReliability()
{
	TAStreamFormatter* stream =	getStream();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();
	string separator = getSeparator();

	TDouble F(lsCalcDataSet->computeOverallNetworkReliability());

	if (F.getStatus()!=TVNumericValue::kNull)
	{
		(*stream) << "* * * OVERALL NETWORK RELIABILITY FACTOR: " << separator;
		stream->width(stream->getObsFormat()->getObsResidualWidth());
		stream->precision(4);
		(*stream) << right << F.getValue() << " * * *";
	}
	else 
	{
		(*stream) << "* * * INDETERMINATE OVERALL NETWORK RELIABILITY FACTOR * * *";
	}
	(*stream) << endl << endl;
}
