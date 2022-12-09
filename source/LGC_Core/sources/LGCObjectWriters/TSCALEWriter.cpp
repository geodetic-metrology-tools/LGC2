#include "TSCALEWriter.h"
#include <TAMeas.h>
#include "TObservationFormat.h"
#include "TAStreamFormatter.h"
#include <Global.h>
#include "RoundOfMeasurements.h"
#include "LGCAdjustablePoint.h"

TSCALEWriter::TSCALEWriter(TAStreamFormatter& stream, bool /*hist*/) : TObservationWriter(stream)
{}

TSCALEWriter::~TSCALEWriter(){}

//------------------ Result header---------------------------------------------------------------------------
void TSCALEWriter::writeSCALEResultsHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					obsIdWidth = getObsIdWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(2);


	////////////////////////////////////////////////////////////
	//First line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); //Position of the scale
	(*stream).writeString(obsWidth,	"OBSERVE"); //mesured offset
	(*stream).writeString(obsResWidth,	"SIGMA"); //sigma DIST
	(*stream).writeString(obsWidth,	"CALCULE"); //estimated offset 
	(*stream).writeString(obsResWidth,	"RESIDU"); //residual
	(*stream).writeString(obsResWidth,	"RES/SIG");    //residual/sigma
	(*stream).writeString(nameWidth,	"SCALE ID");    //scale ID
	(*stream).writeString(obsResWidth, "OBSE"); // observation sigma for ECHO and ECVE
	(*stream).writeString(obsResWidth, "PPM"); // observation PPM error for ECHO and ECVE
	(*stream).writeString(obsResWidth, "ICSE"); // instrument centering sigma for ECHO and ECVE
	if (obsIdWidth != 0) (*stream).writeString(obsIdWidth, "ID"); // Observation identifier
	(*stream)<<endl;	

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream)<<TABs;
	(*stream).writeStringLeft(nameWidth,""); //POSITION
	(*stream).writeString(obsWidth,	"(M)"); //OBSERVED VALUE
	(*stream).writeString(obsResWidth,"(MM)"); //sigma observed value
	(*stream).writeString(obsWidth,	"(M)"); //estimated offset
	(*stream).writeString(obsResWidth,	"(MM)"); //residual
	(*stream).writeString(obsResWidth,	"");    //residual/sigma
	(*stream).writeString(nameWidth,	"");    //scale ID
	(*stream).writeString(obsResWidth, "(MM)"); // observation sigma for ECHO and ECVE
	(*stream).writeString(obsResWidth, "(MM/KM)");     // observation PPM error for ECHO and ECVE
	(*stream).writeString(obsResWidth, "(MM)"); // instrument centering sigma for ECHO and ECVE
	(*stream)<<endl;
}


//------------------ Result data---------------------------------------------------------------------------
void TSCALEWriter::writeECHOResults(const  TECHOROM& echorom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					obsIdWidth = getObsIdWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);
	int					lengthPrecision =	getLengthPrecision();
	std::string         TABs1 = stream->getCurrSpaceExtended(1);
	std::string         TABs3 = stream->getCurrSpaceExtended(2);

	//write header
	(*stream) << endl ;
	(*stream)<<TABs1<<"ECHO"<<endl;

	///////////////////////////////////////////////////////////////////////////////////
	(*stream) << TABs1;
	(*stream).writeStringLeft(nameWidth, "REF POINT");
	(*stream).writeStringLeft(nameWidth, echorom.fMeasuredPlane->getReferencePoint()->getName());
	(*stream).writeStringLeft(nameWidth, "X (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(0));
	(*stream).writeStringLeft(nameWidth, "Y (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(1));
	(*stream).writeStringLeft(nameWidth, "Z (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream) << endl;
	(*stream) << TABs1;
	(*stream).writeStringLeft(nameWidth, "PARAMETRE DU FIL");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "X (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(0) + echorom.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue() * sin(echorom.fMeasuredPlane->getThetaEstimatedValue().getRadiansValue()+M_PI_2));
	(*stream).writeStringLeft(nameWidth, "Y (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(1) + echorom.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue() * cos(echorom.fMeasuredPlane->getThetaEstimatedValue().getRadiansValue() + M_PI_2));
	// Z is not relevant
	//(*stream).writeStringLeft(nameWidth, "Z (M)");
	//(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream) << endl;
	(*stream) << TABs1;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "ORIENTATION (GON)");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getThetaEstimatedValue().getGonsValue());
	(*stream).writeStringLeft(nameWidth, "SORIENTATION (CC)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, echorom.fMeasuredPlane->getThetaEstimatedPrecision().getSignedCCValue());
	(*stream).writeStringLeft(nameWidth, "SNORMALE (MM)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, echorom.fMeasuredPlane->getRefPDistEstimatedPrecision().getMMetresValue());
	(*stream) << endl;
	
	//data summury
   this->writeObsTitle(TABs3 + this->getObsDescriptionFR(TALGCObjectWriter::kECHO), (int)echorom.measECHO.size());
	writeSCALEResultsHeader(); // write the title line for the observations

	for(auto const& ItECHO : echorom.measECHO)
	{
		(*stream)<<TABs3;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItECHO.targetPos->getName());

		//write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECHO.getDistance());//Output value in meters [m], stored in [m]

		//write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.target.sigmaCombinedDist.getMMetresValue());//Output value in milimeters [mm], stored in [m]
		
		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECHO.getDistance() + ItECHO.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.getDistanceResidual().getMMetresValue());//Output value in milimeters [mm], stored in [m]

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.getDistanceResidual()/ ItECHO.target.sigmaCombinedDist);//Output value in meters [m], stored in [m]

		//write the scale ID
		(*stream).writeString(nameWidth, ItECHO.target.ID);

		//Write the sigma of the distance observation (OBSE)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.target.sigmaD.getMMetresValue());

		//Write the ppm of the angle observation (PPM)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.target.ppmD.getMMetresValue());

		//Write the sigma of the target centering (ICSE)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECHO.target.sigmaInstrCentering.getMMetresValue());

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItECHO.obsID);

		(*stream)<<endl;
	}
	(*stream)<<endl;

}

void TSCALEWriter::writeECVEResults(const  TECVEROM& ecverom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					obsIdWidth = getObsIdWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int					lengthPrecision = getLengthPrecision();
	std::string         TABs1 = stream->getCurrSpaceExtended(1);
	std::string         TABs3 = stream->getCurrSpaceExtended(2);

	(*stream) << endl << endl;
	(*stream) << TABs1 << "ECVE" << endl;

	///////////////////////////////////////////////////////////////////////////////////
	(*stream) << TABs1;
	(*stream).writeStringLeft(nameWidth, "POINT ON THE LINE"); //Reference point
	(*stream).writeStringLeft(nameWidth, ecverom.fMeasuredLine->getLinePoint()->getName()); //Reference point
	(*stream).writeString(3, "X");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecverom.fMeasuredLine->getLinePoint()->getEstValue(0));
	(*stream).writeString(3, "Y");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecverom.fMeasuredLine->getLinePoint()->getEstValue(1));
	(*stream).writeString(3, "Z");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecverom.fMeasuredLine->getLinePoint()->getEstValue(2));
	(*stream) << endl << endl;
	///////////////////////////////////////////////////////////////////////////////////

	//data summury
	this->writeObsTitle(TABs3 + this->getObsDescriptionFR(TALGCObjectWriter::kECVE), (int)ecverom.measECVE.size());
	writeSCALEResultsHeader(); // write the title line for the observations

	for (auto const& ItECVE : ecverom.measECVE)
	{
		(*stream) << TABs3;
		//write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItECVE.targetPos->getName());

		//write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECVE.getDistance());//Output value in meters [m], stored in [m]

		//write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECVE.target.sigmaCombinedDist.getMMetresValue());//Output value in milimeters [mm], stored in [m]

		//write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECVE.getDistance() + ItECVE.getDistanceResidual());//Output value in meters [m], stored in [m]

		//write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECVE.getDistanceResidual().getMMetresValue());//Output value in milimeters [mm], stored in [m]

		//write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECVE.getDistanceResidual().getMetresValue() / ItECVE.target.sigmaCombinedDist.getMetresValue());//Output value in meters [m], stored in [m]

		//write the scale ID
		(*stream).writeString(nameWidth, ItECVE.target.ID);

		//Write the sigma of the distance observation (OBSE)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECVE.target.sigmaD.getMMetresValue());

		//Write the ppm of the angle observation (PPM)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECVE.target.ppmD.getMMetresValue());

		//Write the sigma of the target centering (ICSE)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECVE.target.sigmaInstrCentering.getMMetresValue());

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItECVE.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

void TSCALEWriter::writeECSPResults(const TECSPROM& ecsprom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					obsIdWidth = getObsIdWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int					lengthPrecision = getLengthPrecision();
	std::string         TABs1 = stream->getCurrSpaceExtended(1);
	std::string         TABs2 = stream->getCurrSpaceExtended(2);


	(*stream) << endl << endl;
	(*stream) << TABs1 << "ECSP MEASUREMENTS" << endl;
	(*stream) << TABs1 << "LINE " << ecsprom.p1->getName() << " - " << ecsprom.p2->getName() << endl << endl;

	////////////////////////////////////////////////////////////
	(*stream) << TABs2;
	(*stream).writeStringLeft(nameWidth, "POINT"); //second point's Name
	(*stream).writeString(obsWidth, "OBSERVE"); //mesured ECSP
	(*stream).writeString(obsResWidth, "SIGMA"); //sigma ECSP
	(*stream).writeString(obsWidth, "CALCULE"); //estimated ECSP
	(*stream).writeString(obsResWidth, "RESIDU"); //offset ECSP
	(*stream).writeString(obsResWidth, "RES/SIG"); //offset/sigma
	(*stream).writeString(nameWidth, "SCALE ID");    //scale ID
	(*stream).writeString(obsResWidth, "OBSE"); // observation sigma ECSP
	(*stream).writeString(obsResWidth, "PPM"); // observation PPM error ECSP
	(*stream).writeString(obsResWidth, "ICSE"); // instrument centering sigma
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	(*stream) << TABs2;
	(*stream).writeStringLeft(nameWidth, ""); //station
	(*stream).writeString(obsWidth, "(M)"); //mesured
	(*stream).writeString(obsResWidth, "(MM)"); //sigma 
	(*stream).writeString(obsWidth, "(M)"); //estimated 
	(*stream).writeString(obsResWidth, "(MM)"); //offset 
	(*stream).writeString(obsResWidth, ""); //offset/sigma	
	(*stream).writeString(nameWidth, "");    //scale ID
	(*stream).writeString(obsResWidth, "(MM)"); // observation sigma ECSP
	(*stream).writeString(obsResWidth, "(MM/KM)"); // observation PPM error ECSP
	(*stream).writeString(obsResWidth, "(MM)"); // instrument centering sigma
	(*stream) << endl;

		//For each measurement of the station
		for (auto const& ItECSP : ecsprom.measECSP)
		{
			(*stream) << TABs2;

			//write Point
			(*stream).writeStringLeft(nameWidth, ItECSP.targetPos->getName());

			//write the observed offset
			(*stream).writeDouble(obsWidth, lengthPrecision, ItECSP.getDistance());

			//write the sigma
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECSP.target.sigmaCombinedDist.getMMetresValue());

			//write the estimated offset
			(*stream).writeDouble(obsWidth, lengthPrecision, (ItECSP.getDistance() + ItECSP.getDistanceResidual()));

			//write the offset (mm) after calculation
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECSP.getDistanceResidual().getMMetresValue());

			//write the offset / sigma (TDouble (MM))
			(*stream).writeDouble(obsResWidth, 2, (ItECSP.getDistanceResidual().getMetresValue() / ItECSP.target.sigmaCombinedDist.getMetresValue()));

			//write the scale ID
			(*stream).writeString(nameWidth, ItECSP.target.ID);

			//Write the sigma of the distance observation (OBSE)
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECSP.target.sigmaD.getMMetresValue());

			//Write the ppm of the angle observation (PPM)
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECSP.target.ppmD.getMMetresValue());

			//Write the sigma of the target centering (ICSE)
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECSP.target.sigmaInstrCentering.getMMetresValue());

			// Write the observation identifier
			(*stream).writeString(obsIdWidth, ItECSP.obsID);

			(*stream) << endl;

		}
		(*stream) << endl;

}

//------------------ Simu data--------------------------------------------------------------------------
void TSCALEWriter::writeECHOSIMUResults(const  TECHOROM& echorom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					lengthPrecision = getLengthPrecision();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kECHO), (int)echorom.measECHO.size());

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "REF POINT"); //Reference point
	(*stream).writeStringLeft(nameWidth, echorom.fMeasuredPlane->getReferencePoint()->getName());
	(*stream).writeString(3, "X");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(0));
	(*stream).writeString(3, "Y");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(1));
	(*stream).writeString(3, "Z");
	(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream) << endl << endl;


	writeDistanceResultsSummary(echorom.getECHOObsSummary(), stream->getCurrSpaceExtended(2));
}

void TSCALEWriter::writeECVESIMUResults(const  TECVEROM& ecverom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					lengthPrecision = getLengthPrecision();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kECVE), (int)ecverom.measECVE.size());

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POINT ON THE LINE"); //Reference point
	(*stream).writeStringLeft(nameWidth, ecverom.fMeasuredLine->getLinePoint()->getName()); //Reference point
	(*stream).writeString(3, "X");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecverom.fMeasuredLine->getLinePoint()->getEstValue(0));
	(*stream).writeString(3, "Y");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecverom.fMeasuredLine->getLinePoint()->getEstValue(1));
	(*stream).writeString(3, "Z");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecverom.fMeasuredLine->getLinePoint()->getEstValue(2));
	(*stream) << endl << endl;


	writeDistanceResultsSummary(ecverom.getECVEObsSummary(), stream->getCurrSpaceExtended(2));
}

void TSCALEWriter::writeECSPSIMUResults(const  TECSPROM& ecsprom)
{
	TAStreamFormatter*	stream = getStream();
    // int				nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);
	std::string         TAB2 = stream->getCurrSpaceExtended(2);

	(*stream) << TABs;
	(*stream) << "LINE: " << ecsprom.p1->getName() << ", " << ecsprom.p2->getName() << endl;
	(*stream) << endl;
	this->writeObsTitle(TAB2 + this->getObsDescriptionFR(TALGCObjectWriter::kECSP), (int)ecsprom.measECSP.size());

	writeDistanceResultsSummary(ecsprom.getECSPObsSummary(), stream->getCurrSpaceExtended(2));
}


//------------------ Reliability header----------------------------------------------------------------------
void	TSCALEWriter::writeECHOReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PT REF","STATION", "", "OBSERVATION", "M", "MM");
	return;
}

void	TSCALEWriter::writeECVEReliabilityHeader()
{
this->TObservationWriter::writeReliabilityHeader("PT LINE","STATION", "", "OBSERVATION", "M", "MM");
	return;
}

void	TSCALEWriter::writeECSPReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("LINE NAME", "STATION", "", "OBSERVATION", "M", "MM");
	return;
}


//------------------ Reliability data----------------------------------------------------------------------
void	TSCALEWriter::writeECHOReliabilityData(const  TECHOROM& echorom, const TLGCStatistic& stat, const std::list<TECHO> measECHO)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision()-3, 0);


	//For each ECHO measurement of the station
	for(auto const& ItEcho : measECHO)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItEcho.getFirstObservationIndex();

		// get Ref  Point 
		(*stream).writeStringLeft(nameWidth, echorom.fMeasuredPlane->getReferencePoint()->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItEcho.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision,ItEcho.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcho.target.sigmaCombinedDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcho.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

void	TSCALEWriter::writeECVEReliabilityData(const  TECVEROM& ecverom, const TLGCStatistic& stat, const std::list<TECVE> measECVE)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);


	//For each ECHO measurement of the station
	for (auto const& ItEcve : measECVE)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItEcve.getFirstObservationIndex();

		// get Ref  Point 
		(*stream).writeStringLeft(nameWidth, ecverom.fMeasuredLine->getLinePoint()->getName());
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItEcve.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItEcve.getDistance());
		//get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcve.target.sigmaCombinedDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcve.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

void	TSCALEWriter::writeECSPReliabilityData(const TECSPROM& ecsprom, const TLGCStatistic& stat, const std::list<TECSP> measECSP)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsWidth = getObsWidth();
	int					obsResWidth = getObsResWidth();
	int					lengthPrecision = getLengthPrecision();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	for (auto const& ItEcsp : measECSP)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItEcsp.getFirstObservationIndex();

		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, ecsprom.romName);
		//get Tg point
		(*stream).writeStringLeft(nameWidth, ItEcsp.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//get the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItEcsp.getDistance());
		//get the sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcsp.target.sigmaCombinedDist.getMMetresValue());
		//get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItEcsp.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}


//------------------ Synthesis header------------------------------------------------------------------------
void TSCALEWriter::writeSCALESynthesisHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	std::string				separator = getSeparator();
	std::string         TABs = stream->getCurrSpaceExtended(1);


	////////////////////////////////////////////////////////////
	//First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "LIGNE"); //plane name
	(*stream).writeString(obsResWidth, "RES_MAX"); //residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); //residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); //residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); //ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	//second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)"); 

	(*stream) << endl;
}


//------------------ Synthesis data--------------------------------------------------------------------------
void TSCALEWriter::writeDefResultsSynthesis(std::list<const TLGCObsSummary*> &meassum, int obsResWidth, int ResPrecision) {
	TAStreamFormatter* stream = getStream();
	int					nameWidth = getNameWidth();
	std::string         TABs = stream->getCurrSpaceExtended(1);

	for (auto const& ItMEAS : meassum) {
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, ItMEAS->getObsText()); //Reference point
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getResMax());//residu max
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getResMin());//residu min
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getMean());//residu moy
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getVariance());//ecart type
		(*stream) << endl;
	}
}


void TSCALEWriter::writeECHOResultsSynthesis(const  TECHOROM& echorom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
    // int				lengthPrecision = getLengthPrecision();
    std::string         TABs = stream->getCurrSpaceExtended(1);

    const auto &echoSummary = echorom.getECHOObsSummary();

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, echorom.fMeasuredPlane->getReferencePoint()->getName()); //Reference point
	(*stream).writeDouble(obsResWidth, lengthResPrecision, echoSummary.getResMax());//residu max
	(*stream).writeDouble(obsResWidth, lengthResPrecision, echoSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, echoSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, echoSummary.getVariance());//ecart type
	(*stream) << endl;


}

void TSCALEWriter::writeECVEResultsSynthesis(const  TECVEROM& ecrom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
    // int				lengthPrecision = getLengthPrecision();
    std::string         TABs = stream->getCurrSpaceExtended(1);

    const auto &ecveSummary = ecrom.getECVEObsSummary();

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ecrom.fMeasuredLine->getName()); //Reference point
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecveSummary.getResMax());//residu max
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecveSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecveSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecveSummary.getVariance());//ecart type
	(*stream) << endl;


}

void TSCALEWriter::writeECSPResultsSynthesis(const  TECSPROM& ecrom)
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
    // int				obsWidth = getObsWidth();
    int					obsResWidth = getObsResWidth();
	int					lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
    // int				lengthPrecision = getLengthPrecision();
    std::string         TABs = stream->getCurrSpaceExtended(1);

    const auto &ecspSummary = ecrom.getECSPObsSummary();

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ecrom.romName); //line
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecspSummary.getResMax());//residu max
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecspSummary.getResMin());//residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecspSummary.getMean());//residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecspSummary.getVariance());//ecart type
	(*stream) << endl;


}

