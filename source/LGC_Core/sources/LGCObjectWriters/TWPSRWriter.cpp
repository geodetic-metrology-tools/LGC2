// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TWPSRWriter.h"

#include <Global.h>
#include <TAMeas.h>

#include "LGCAdjustablePoint.h"
#include "RoundOfMeasurements.h"
#include "TAStreamFormatter.h"
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
	int obsIdWidth = getObsIdWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the wpsr
	(*stream).writeString(obsWidth, "OBSX"); // mesured ws distance
	(*stream).writeString(obsResWidth, "SX"); // sigma DIST
	(*stream).writeString(obsWidth, "CALCX"); // estimated offset
	(*stream).writeString(obsResWidth, "RESX"); // residual
	(*stream).writeString(obsResWidth, "RESX/SX"); // residual/sigma
	(*stream).writeString(obsWidth, "OBSZ"); // mesured ws distance
	(*stream).writeString(obsResWidth, "SZ"); // sigma DIST
	(*stream).writeString(obsWidth, "CALCZ"); // estimated offset
	(*stream).writeString(obsResWidth, "RESZ"); // residual
	(*stream).writeString(obsResWidth, "RESZ/SZ"); // residual/sigma
	(*stream).writeString(obsResWidth, "XSE"); // OBSE value for X
	(*stream).writeString(obsResWidth, "XICSE"); // instrument centering X sigma
	(*stream).writeString(obsResWidth, "ZSE"); // OBSE value for Z
	(*stream).writeString(obsResWidth, "ZICSE"); // instrument centering Z sigma
	(*stream).writeString(obsResWidth, "WISE"); // wire sigma
	(*stream).writeString(nameWidth, "WPSR ID"); // wpsr ID
	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // Observation identifier
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
	(*stream).writeString(obsResWidth, "(MM)"); // OBSE value for X
	(*stream).writeString(obsResWidth, "(MM)"); // instrument centering X sigma
	(*stream).writeString(obsResWidth, "(MM)"); // OBSE value for Z
	(*stream).writeString(obsResWidth, "(MM)"); // instrument centering Z sigma
	(*stream).writeString(obsResWidth, "(MM)"); // Wire sigma
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
	int obsIdWidth = getObsIdWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// write header
	writeWPSRHeader(ecwirom);

	// data summury
	this->writeObsTitle(TABs + this->getObsDescription(TALGCObjectWriter::kECWI), (int)ecwirom.measECWI.size());
	writeWPSRResultsHeader(); // write the title line for the observations

	for (auto const &ItECWI : ecwirom.measECWI)
	{
		(*stream) << TABs;
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItECWI.targetPos->getName());

		// write the observed X Distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kX)); 

		// write the sigma X Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaCombinedX.getMMetresValue()); 

		// write the estimated X Distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kX) + ItECWI.getDistanceResidual(EECWIDistances::kX)); 

		// write the residual for X Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.getDistanceResidual(EECWIDistances::kX).getMMetresValue()); 

		// write the residual/sigma for X Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.getDistanceResidual(EECWIDistances::kX) / ItECWI.target.sigmaCombinedX); 

		// write the observed Z Distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kZ)); 

		// write the sigma Z Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaCombinedZ.getMMetresValue()); 

		// write the estimated Z Distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItECWI.getDistance(EECWIDistances::kZ) + ItECWI.getDistanceResidual(EECWIDistances::kZ)); 

		// write the residual for Z Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.getDistanceResidual(EECWIDistances::kZ).getMMetresValue()); 

		// write the residual/sigma for Z Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.getDistanceResidual(EECWIDistances::kZ) / ItECWI.target.sigmaCombinedZ);

		// write the OBSE for X Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaX.getMMetresValue()); 

		// write the Instrument Centering for X Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaInstrCenteringX.getMMetresValue()); 

		// write the OBSE for Z Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaZ.getMMetresValue());

		// write the Instrument Centering for Z Distance
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaInstrCenteringZ.getMMetresValue()); 

		// write the precision of the wire
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItECWI.target.sigmaWire.getMMetresValue()); 

		// write the scale ID
		(*stream).writeString(nameWidth, ItECWI.target.ID);

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItECWI.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

//------------------ Simu data--------------------------------------------------------------------------
void TWPSRWriter::writeECWISIMUResults(const TECWIROM &ecwirom)
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpaceExtended(1);

	this->writeObsTitle(TABs + this->getObsDescription(TALGCObjectWriter::kECWI), (int)ecwirom.measECWI.size());

	writeWPSRHeader(ecwirom);

	if (ecwirom.measECWI.size() > 0)
	{
		const auto &summary = ecwirom.getECWIObsSummary();
		this->writeObsTitle(TABs + this->getObsDescription(TALGCObjectWriter::kECWI) + ": X", (int)ecwirom.measECWI.size());
		writeDistanceResultsSummary(summary.xObsSum, TABs);
		this->writeObsTitle(TABs + this->getObsDescription(TALGCObjectWriter::kECWI) + ": Z", (int)ecwirom.measECWI.size());
		writeDistanceResultsSummary(summary.zObsSum, TABs);
	}
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
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "WIRE NAME");
	(*stream).writeStringLeft(nameWidth, ecwirom.romName);
	(*stream) << "\n";
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "WIRE PARAMETERS");
	(*stream).writeStringLeft(nameWidth, "X (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.referencePoint.getX().getMetresValue());
	(*stream).writeStringLeft(nameWidth, "Y (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.referencePoint.getY().getMetresValue());
	(*stream).writeStringLeft(nameWidth, "Z (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.referencePoint.getZ().getMetresValue());
	(*stream) << "\n";
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "BEARING (GON)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fWireBearing->getEstimatedValue().getGonsValue());
	(*stream).writeStringLeft(nameWidth, "SBEARING (CC)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, ecwirom.fWireBearing->getEstimatedPrecision().getSignedCCValue());
	(*stream).writeStringLeft(nameWidth, "SLOPE (GON)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fWireSlope->getEstimatedValue().getGonsValue());
	(*stream).writeStringLeft(nameWidth, "SSLOPE (CC)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fWireSlope->getEstimatedPrecision().getSignedCCValue());
	(*stream) << "\n";
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "DX (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fWireDx->getEstimatedValue().getMetresValue());
	(*stream).writeStringLeft(nameWidth, "SDX (MM)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, ecwirom.fWireDx->getEstimatedPrecision().getMMetresValue());
	(*stream).writeStringLeft(nameWidth, "DZ (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.fWireDz->getEstimatedValue().getMetresValue());
	(*stream).writeStringLeft(nameWidth, "SDZ (MM)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, ecwirom.fWireDz->getEstimatedPrecision().getMMetresValue());
	(*stream) << "\n";
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "SAGFIX");
	TReal sigmaSag;
	if (ecwirom.sagfix)
	{
		(*stream).writeString(obsWidth, "TRUE");
		sigmaSag = ecwirom.instrument.sigmaSagWire.getMMetresValue();
	}
	else
	{
		(*stream).writeString(obsWidth, "FALSE");
		sigmaSag = ecwirom.sagAdjustable->getEstimatedPrecision().getMMetresValue();
	}
	(*stream).writeStringLeft(nameWidth, "SAG (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, ecwirom.sagAdjustable->getEstimatedValue().getMetresValue());
	(*stream).writeStringLeft(nameWidth, "SSAG (MM)");
	(*stream).writeDouble(obsWidth, lengthPrecision - 3, sigmaSag);
	(*stream) << endl;
}

//------------------ Synthesis header------------------------------------------------------------------------
void TWPSRWriter::writeWPSRSynthesisHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "REF. POINT"); // plane name
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MEAN"); // residu mean
	(*stream).writeString(obsResWidth, "STD_DEV"); // ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// Second line
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
		(*stream).writeDouble(obsResWidth, ResPrecision, ItMEAS->getStdev()); // ecart type
		(*stream) << endl;
	}
}

//------------------ Reliability header----------------------------------------------------------------------
void TWPSRWriter::writeECWIReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("WIRE", "STATION", "", "OBS_X", "M", "MM");
	this->TObservationWriter::writeReliabilityHeader("WIRE", "STATION", "", "OBS_Z", "M", "MM");
	return;
}

//------------------ Reliability data----------------------------------------------------------------------
void TWPSRWriter::writeECWIReliabilityData(const TECWIROM &ecwirom, const TLGCStatistic &stat, const std::list<TECWI> &measECWI)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each ECWS measurement of the station
	for (auto const &itEcwi : measECWI)
	{
		// Observation index to take the right value in the statistic vector
		int index = itEcwi.getFirstObservationIndex();

		// get water surface reference
		(*stream).writeStringLeft(nameWidth, ecwirom.romName);

		// get Station point
		(*stream).writeStringLeft(nameWidth, itEcwi.targetPos->getName());

		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		//------------------- 1rst obs----------------------------------------------------//
		// get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, itEcwi.getDistance(EECWIDistances::kX));
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itEcwi.target.sigmaCombinedX.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itEcwi.getDistanceResidual(EECWIDistances::kX).getMMetresValue());

		writeReliabilityMM(index, stat);

		//------------------- 2nd obs----------------------------------------------------//
		index = index + 1;

		// indentation to not repeat the station / wire names
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, itEcwi.getDistance(EECWIDistances::kZ));
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itEcwi.target.sigmaCombinedZ.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itEcwi.getDistanceResidual(EECWIDistances::kZ).getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}
