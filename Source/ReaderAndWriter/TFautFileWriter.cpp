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
#include    "TGyroOrientationConverter.h"
#include    "THorDistConverter.h"
#include    "TVertDistConverter.h"
#include    "TSpatialDistConverter.h"
#include    "TOffsetToVerLineConverter.h"
#include    "TOffsetToVerPlaneConverter.h"
#include    "TOffsetToSpaLineConverter.h"
#include    "TOffsetToTheoPlaneConverter.h"
#include    "TRadialOffsetCnstrConverter.h"



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
void	TFautFileWriter::writeResults(TLGCDataSet* ds,LSCalcDataSet* LSds)
{
	// initialisation
	init(ds, LSds, "");
	// write headers
	writeTitle();
	writeDataSummary();

	TLGCDataSet* dataSet = getDataSet();

	// Write observations if lists are not empty
	if (dataSet->getHADimension() != 0)
		writeHorAngData();

	if (dataSet->getZDDimension(TAMeasurement::kFixed))
		writeZenDistData(TALSCalcParameter::kFixed);

	if (dataSet->getZDDimension(TAMeasurement::kVariable))
		writeZenDistData(TALSCalcParameter::kVariable);

	if (dataSet->getSDDimension(TAMeasurement::kFixed))
		writeSpaDistObs(TALSCalcParameter::kFixed);

	if (dataSet->getSDDimension(TAMeasurement::kVariable))
		writeSpaDistObs(TALSCalcParameter::kVariable);

	if (dataSet->getGyroOrieDimension())
		writeGyroOrieData();

	// Cover DLEV and DVER observations
	for (int i = 0; i < 2; i++)
		if (dataSet->getVDDimension(i==1) != 0)
			writeVertDistObs(i==1);	

	if (dataSet->getHDDimension() != 0)
		writeHorDistObs();

	if (dataSet->getOffsetToVerPlaneDimension())
		writeOffToVertPlaneObs();

	if (dataSet->getOffsetToSpaLineDimension())
		writeOffToSpaLineObs();

	if (dataSet->getOffsetToTheoPlaneDimension())
		writeOffToTheoPlaneObs();

	if (dataSet->getOffsetToVerLineDimension())
		writeOffToVertLineObs();

	if (dataSet->getRadOffCnstrDimension())
		writeAllRadOffCnstrObs();

	writeOverallReliability();
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
//HORIZONTAL ANGLE
/////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeHorAngData() 
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	THorAngleConverter converter (getStreamRef());
	LSHorAngIter	obsIt = lsCalcDataSet->beginLSHorAng();
	LSHorAngIter	end = lsCalcDataSet->endLSHorAng();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kANGL) << " : " << endl;
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
//HORIZONTAL DISTANCE
/////////////////////////////////////////////////////////////////////////////////////////////////
void	TFautFileWriter::writeHorDistObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	THorDistConverter converter (stream);
	LSHorDistIter	obsIt = lsCalcDataSet->beginLSHorDist();
	LSHorDistIter	end = lsCalcDataSet->endLSHorDist(); 

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kDHOR) <<" : " << endl;
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
//VERTICAL DISTANCE
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeVertDistObs(bool isDLEV)
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TVertDistConverter converter (stream);
	LSVertDistIter	obsIt = lsCalcDataSet->beginLSVertDist(isDLEV);
	LSVertDistIter	end = lsCalcDataSet->endLSVertDist(isDLEV);

	//write header
	TALGCConverter::ELGCObservations key = isDLEV ? TALGCConverter::kDLEV : TALGCConverter::kDVER;
	(stream) << this->getObsDescriptionEN(key) <<" : " << endl;
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
void TFautFileWriter::writeZenDistData(const TALSCalcParameter::ELSStatus status)
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TZenithDistConverter converter (stream, status);

	//write header
	if( status == TALSCalcParameter::kFixed )
		(stream) << this->getObsDescriptionEN(TALGCConverter::kZENH) <<" : " << endl;
	else
		(stream) << this->getObsDescriptionEN(TALGCConverter::kZENI) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write data
	LSZenDistIter	obsIt = lsCalcDataSet->beginLSZenDist();
	LSZenDistIter	end = lsCalcDataSet->endLSZenDist();

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
//SPATIAL DISTANCE
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeSpaDistObs(const TALSCalcParameter::ELSStatus status)
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TSpatialDistConverter converter (stream, status);

	//write header
	if( status == TALSCalcParameter::kFixed )
		(stream) << this->getObsDescriptionEN(TALGCConverter::kDMES) <<" : " << endl;
	else
		(stream) << this->getObsDescriptionEN(TALGCConverter::kDTHE) <<" : " << endl;
	converter.writeReliabilityHeader();
	
	// write values
	LSSpaDistIter	obsIt = lsCalcDataSet->beginLSSpaDist();
	LSSpaDistIter	end = lsCalcDataSet->endLSSpaDist();
			
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
//OFFSET TO VERTICAL LINE (ECVE)
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeOffToVertLineObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TOffsetToVerLineConverter converter (stream);
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

	TOffsetToSpaLineConverter converter (stream);
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

	TOffsetToVerPlaneConverter converter (stream);
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
void TFautFileWriter::writeOffToTheoPlaneObs()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TOffsetToTheoPlaneConverter converter (stream);
	LSOffsetToTheoPlaneIter	obsIt = lsCalcDataSet->beginLSOffsetToTheoPlane();
	LSOffsetToTheoPlaneIter	end =lsCalcDataSet->endLSOffsetToTheoPlane();

	//write header
	(stream) << this->getObsDescriptionEN(TALGCConverter::kECTH) <<" : " << endl;
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
//GYRO ORIENTATION
/////////////////////////////////////////////////////////////////////////////////////////////////
void TFautFileWriter::writeGyroOrieData()
{
	TAStreamFormatter& stream =	getStreamRef();
	LSCalcDataSet* lsCalcDataSet = getLSCalcDataSet();

	TGyroOrientationConverter converter (stream);
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
