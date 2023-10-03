#include "TINCLWriter.h"

#include <Global.h>
#include <TAMeas.h>

#include "LGCAdjustablePoint.h"
#include "RoundOfMeasurements.h"
#include "TAStreamFormatter.h"
#include "TObservationFormat.h"

TINCLWriter::TINCLWriter(TAStreamFormatter &stream, bool /*hist*/) : TObservationWriter(stream)
{
}

TINCLWriter::~TINCLWriter()
{
}

//------------------ Result header---------------------------------------------------------------------------
void TINCLWriter::writeINCLResultsHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the scale
	(*stream).writeString(obsWidth, "OBSERVE"); // mesured offset
	(*stream).writeString(obsResWidth, "SIGMA"); // sigma DIST
	(*stream).writeString(obsWidth, "CALCULE"); // estimated offset
	(*stream).writeString(obsResWidth, "RESIDU"); // residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // residual/sigma
	(*stream).writeString(nameWidth, "INCL ID"); // inclinometer ID
	(*stream).writeString(obsResWidth, "OBSE"); // OBSE value
	(*stream).writeString(obsResWidth, "PPM"); // PPM value
	(*stream).writeString(obsWidth, "AC"); // AC value
	(*stream).writeString(obsResWidth, "ACSE"); // ACSE Value
	(*stream).writeString(obsWidth, "RF"); // RF value
	(*stream).writeString(obsResWidth, "RFSE"); // RFSE value
	if (obsIdWidth != 0) (*stream).writeString(obsIdWidth, "ID"); // Observation identifier
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // POSITION
	(*stream).writeString(obsWidth, "(GON)"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(CC)"); // sigma observed value
	(*stream).writeString(obsWidth, "(GON)"); // estimated offset
	(*stream).writeString(obsResWidth, "(CC)"); // residual
	(*stream).writeString(obsResWidth, ""); // residual/sigma
	(*stream).writeString(nameWidth, ""); // inclinometer ID
	(*stream).writeString(obsResWidth, "(CC)"); // OBSE
	(*stream).writeString(obsResWidth, ""); // PPM
	(*stream).writeString(obsWidth, "(GON)"); // AC
	(*stream).writeString(obsResWidth, "(CC)"); // ACSE
	(*stream).writeString(obsWidth, "(GON)"); // RF
	(*stream).writeString(obsResWidth, "(CC)"); // RFSE
	(*stream) << endl;
}

//------------------ Result data---------------------------------------------------------------------------
void TINCLWriter::writeINCLYResults(const TINCLYROM &inclyrom)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	int anglePrecision = getAnglePrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// write header
	(*stream) << endl;
	(*stream) << TABs << "INCLY" << endl;

	// data summury
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kINCLY), (int)inclyrom.measINCLY.size());
	writeINCLResultsHeader(); // write the title line for the observations

	for (auto const &ItINCLY : inclyrom.measINCLY)
	{
		(*stream) << TABs;
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItINCLY.targetPos->getName());

		// write the observed angle
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.getAngle().getGonsValue()); // Output value in gradians [gon], stored in [rad]

		// write the sigma angle
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCombinedAngle.getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the estimated angle
		(*stream).writeDouble(obsWidth, anglePrecision, (ItINCLY.getAngle() + ItINCLY.getAngleResidual()).getGonsValue()); // Output value in gradians [gon], stored in [rad]

		// write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the residual/sigma
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getRadiansValue() / ItINCLY.target.sigmaCombinedAngle.getRadiansValue()); // Output value unitless

		// write the scale ID
		(*stream).writeString(nameWidth, ItINCLY.target.ID);

		// write the OBSE
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaAngl.getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the PPM
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaPpm.getMicroRadiansValue()); // Output value in microradians [urad], stored in [rad]

		// write the AC
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.target.angleCorrectionValue.getGonsValue()); // Output value in gon [gon], stored in [rad]

		// write the ACSE
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCorrectionValue.getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the RF
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.target.refAngleCorrectionValue.getGonsValue()); // Output value in gon [gon], stored in [rad]

		// write the RFSE
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.refSigmaCorrectionValue.getSignedCCValue()); // Output value in cc [cc], stored in [rad]
	
		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItINCLY.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

//------------------ Simu data--------------------------------------------------------------------------
void TINCLWriter::writeINCLYSIMUResults(const TINCLYROM &inclyrom)
{
	TAStreamFormatter *stream = getStream();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kINCLY), (int)inclyrom.measINCLY.size());
	(*stream) << TABs << "INCLY" << endl;

	writeAngleResultsSummary(inclyrom.getINCLYObsSummary(), stream->getCurrSpaceExtended(2));
}

//------------------ Reliability header----------------------------------------------------------------------
void TINCLWriter::writeINCLYReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("FRAME", "STATION", "", "OBSERVATION", "GON", "CC");
	return;
}

//------------------ Reliability data----------------------------------------------------------------------
void TINCLWriter::writeINCLYReliabilityData(const TINCLYROM &inclyrom, const TLGCStatistic &stat, const std::list<TINCLY> &measINCLY)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int anglePrecision = getAnglePrecision();
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);

	// For each INCLY measurement of the station
	for (auto const &ItINCLY : measINCLY)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItINCLY.getFirstObservationIndex();

		// Get the frame name
		(*stream).writeStringLeft(nameWidth, inclyrom.positionInTree.node->data->frame.getName());

		// get Station point
		(*stream).writeStringLeft(nameWidth, ItINCLY.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed angle
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.getAngle().getGonsValue());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCombinedAngle.getSignedCCValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getSignedCCValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

//------------------ Synthesis header------------------------------------------------------------------------
void TINCLWriter::writeINCLSynthesisHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	// int				obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "STATION"); // plane name
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");

	(*stream) << endl;
}

//------------------ Synthesis data--------------------------------------------------------------------------

void TINCLWriter::writeINCLYResultsSynthesis(std::list<const TLGCObsSummary *> &inclysum)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	std::string TABs = stream->getCurrSpaceExtended(1);

	for (auto const &ItINCLY : inclysum)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, ItINCLY->getObsText()); // Reference point
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getResMax()); // residu max
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getResMin()); // residu min
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getMean()); // residu moy
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getStdev()); // ecart type
		(*stream) << endl;
	}
}