#include <Global.h>
#include <TAMeas.h>

#include "LGCAdjustablePoint.h"
#include "RoundOfMeasurements.h"
#include "TAStreamFormatter.h"
#include "TWPSRWriter.h"
#include "TObservationFormat.h"

TWPSRWriter::TWPSRWriter(TAStreamFormatter &stream, bool /*hist*/) : TObservationWriter(stream)
{
}

TWPSRWriter::~TWPSRWriter()
{
}

//------------------ Result header---------------------------------------------------------------------------
void TWPSRWriter::writeWPSRResultsHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the wpsr
	(*stream).writeString(obsWidth, "X:OBSERVE"); // mesured ws distance
	(*stream).writeString(obsResWidth, "X:SIGMA"); // sigma DIST
	(*stream).writeString(obsWidth, "X:CALCULE"); // estimated offset
	(*stream).writeString(obsResWidth, "X:RESIDU"); // residual
	(*stream).writeString(obsResWidth, "X:RES/SIG"); // residual/sigma
	(*stream).writeString(obsWidth, "Z:OBSERVE"); // mesured ws distance
	(*stream).writeString(obsResWidth, "Z:SIGMA"); // sigma DIST
	(*stream).writeString(obsWidth, "Z:CALCULE"); // estimated offset
	(*stream).writeString(obsResWidth, "Z:RESIDU"); // residual
	(*stream).writeString(obsResWidth, "Z:RES/SIG"); // residual/sigma
	(*stream).writeString(nameWidth, "WPSR ID"); // wpsr ID
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // POSITION
	(*stream).writeString(obsWidth, "(M)"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(MM)"); // sigma observed value
	(*stream).writeString(obsWidth, "(M)"); // estimated offset
	(*stream).writeString(obsResWidth, "(MM)"); // residual
	(*stream).writeString(obsResWidth, ""); // residual/sigma
	(*stream).writeString(obsWidth, "(M)"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(MM)"); // sigma observed value
	(*stream).writeString(obsWidth, "(M)"); // estimated offset
	(*stream).writeString(obsResWidth, "(MM)"); // residual
	(*stream).writeString(obsResWidth, ""); // residual/sigma
	(*stream).writeString(nameWidth, ""); // wpsr ID

	(*stream) << endl;
}

//------------------ Result data---------------------------------------------------------------------------
void TWPSRWriter::writeECWIResults(const TECWIROM &ecwirom)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// write header
	writeWPSRHeader(ecwirom);

	// data summury
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kECWI), (int)ecwirom.measECWI.size());
	writeWPSRResultsHeader(); // write the title line for the observations

	for (auto const &ItECWI : ecwirom.measECWI)
	{
		(*stream) << TABs;
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItECWI.targetPos->getName());

		// write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kX)); // Output value in meters [m], stored in [m]

		// write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaCombinedX.getMMetresValue()); // Output value in milimeters [mm], stored in [m]

		auto test = ItECWI.getDistanceResidual(EECWIDistances::kX);
		// write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kX) + ItECWI.getDistanceResidual(EECWIDistances::kX)); // Output value in meters [m], stored in [m]

		// write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.getDistanceResidual(EECWIDistances::kX).getMMetresValue()); // Output value in milimeters [mm], stored in [m]

		// write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision,ItECWI.getDistanceResidual(EECWIDistances::kX) / ItECWI.target.sigmaCombinedX); // Output value in meters [m], stored in [m]

		// write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kZ)); // Output value in meters [m], stored in [m]

		// write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaCombinedZ.getMMetresValue()); // Output value in milimeters [mm], stored in [m]

		// write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kZ) + ItECWI.getDistanceResidual(EECWIDistances::kZ)); // Output value in meters [m], stored in [m]

		// write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.getDistanceResidual(EECWIDistances::kZ).getMMetresValue()); // Output value in milimeters [mm], stored in [m]

		// write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.getDistanceResidual(EECWIDistances::kZ) / ItECWI.target.sigmaCombinedZ); // Output value in meters [m], stored in [m]

		// write the scale ID
		(*stream).writeString(nameWidth, ItECWI.target.ID);

		(*stream) << endl;
	}
	(*stream) << endl;
}
//------------------ Simu data--------------------------------------------------------------------------

void TWPSRWriter::writeECWISIMUResults(const TECWIROM &ecwirom)
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpaceExtended(1);

	writeWPSRHeader(ecwirom);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kECWI), (int)ecwirom.measECWI.size());
	(*stream) << TABs << "ECWI" << endl;

	/* to activate when simu dev
	if (ecwirom.measECWI.size() > 0)
		writeDistanceResultsSummary(ecwirom.getECWIObsSummary(), stream->getCurrSpaceExtended(2));
	*/
}

//------------------ Result header---------------------------------------------------------------------------

void TWPSRWriter::writeWPSRHeader(const TECWIROM &ecwirom)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// write header
	(*stream) << "\n";
	(*stream) << TABs << "ECWI"
			  << "\n";

		///////////////////////////////////////////////////////////////////////////////////
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "REF POINT");
	(*stream).writeStringLeft(nameWidth, ecwirom.fMeasuredPlane->getReferencePoint()->getName());
	(*stream).writeStringLeft(nameWidth, "X (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredPlane->getReferencePoint()->getEstValue(0));
	(*stream).writeStringLeft(nameWidth, "Y (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredPlane->getReferencePoint()->getEstValue(1));
	(*stream).writeStringLeft(nameWidth, "Z (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream) << endl;
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "PARAMETRE DU FIL");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "X (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision,
		ecwirom.fMeasuredPlane->getReferencePoint()->getEstValue(0)
			+ ecwirom.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue() * sin(ecwirom.fMeasuredPlane->getThetaEstimatedValue().getRadiansValue() + M_PI_2));
	(*stream).writeStringLeft(nameWidth, "Y (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision,
		ecwirom.fMeasuredPlane->getReferencePoint()->getEstValue(1)
			+ ecwirom.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue() * cos(ecwirom.fMeasuredPlane->getThetaEstimatedValue().getRadiansValue() + M_PI_2));
	// Z is not relevant
	//(*stream).writeStringLeft(nameWidth, "Z (M)");
	//(*stream).writeDouble(obsWidth, lengthPrecision, echorom.fMeasuredPlane->getReferencePoint()->getEstValue(2));
	(*stream) << endl;
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "ORIENTATION (GON)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredPlane->getThetaEstimatedValue().getGonsValue());
	(*stream).writeStringLeft(nameWidth, "SORIENTATION (CC)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, ecwirom.fMeasuredPlane->getThetaEstimatedPrecision().getSignedCCValue());
	(*stream).writeStringLeft(nameWidth, "NORMALE (MM)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, ecwirom.fMeasuredPlane->getRefPtDistEstimatedValue().getMMetresValue());
	(*stream).writeStringLeft(nameWidth, "SNORMALE (MM)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, ecwirom.fMeasuredPlane->getRefPDistEstimatedPrecision().getMMetresValue());
	(*stream) << endl;
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "SAG (mm)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredSAG->getEstimatedValue().getMMetresValue());
	(*stream).writeStringLeft(nameWidth, "SIGMA SAG (mm)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredSAG->getEstimatedPrecision().getMMetresValue());
	(*stream) << endl;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "SLOPE (GON)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredPitch->getEstimatedValue().getGonsValue());
	(*stream).writeStringLeft(nameWidth, "SIGMA SLOPE (CC)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fMeasuredPitch->getEstimatedPrecision().getSignedCCValue());
}

//------------------ Synthesis header------------------------------------------------------------------------
void TWPSRWriter::writeWPSRSynthesisHeader()
{
	// TO DO
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	// int				obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "REF. POINT"); // plane name
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");

	(*stream) << endl;
}
void TWPSRWriter::writeDefResultsSynthesis(std::list<const TLGCObsSummary *> &meassum, int obsResWidth, int ResPrecision)
{
	// TO DO
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	std::string TABs = stream->getCurrSpaceExtended(1);

	for (auto const &ItMEAS : meassum)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, ItMEAS->getObsText()); // Reference point
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getResMax()); // residu max
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getResMin()); // residu min
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getMean()); // residu moy
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getVariance()); // ecart type
		(*stream) << endl;
	}
}

void TWPSRWriter::writeECWIResultsSynthesis(const TECWIROM &ecwirom)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string TABs = stream->getCurrSpaceExtended(1);

	const auto &ecwiSummary = ecwirom.getECWIObsSummary();
	/* TO DO
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ecwiSummary.getObsText()); // Reference point
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwiSummary.getResMax()); // residu max
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwiSummary.getResMin()); // residu min
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwiSummary.getMean()); // residu moy
	(*stream).writeDouble(obsResWidth, lengthResPrecision, ecwiSummary.getVariance()); // ecart type
	(*stream) << endl;
	*/
}

//------------------ Reliability header----------------------------------------------------------------------
void TWPSRWriter::writeECWIReliabilityHeader()
{
	/*TO DO*/
	this->TObservationWriter::writeReliabilityHeader("PT REF", "STATION", "", "OBSERVATION", "M", "MM");
	return;
}

//------------------ Reliability data----------------------------------------------------------------------
void TWPSRWriter::writeECWIReliabilityData(const TECWIROM &ecwirom, const TLGCStatistic &stat, const std::list<TECWI> &measECWI)
{
	/*TO DO*/
	return;
}
