// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLEVELWriter.h"

#include <Global.h>
#include <TAMeas.h>

#include "TAStreamFormatter.h"
#include "TObservationFormat.h"

TLEVELWriter::TLEVELWriter(TAStreamFormatter &stream, bool /*hist*/) : TObservationWriter(stream), isAllfixed(false)
{
}

TLEVELWriter::~TLEVELWriter()
{
}

//------------------ Result header---------------------------------------------------------------------------
void TLEVELWriter::writeLEVELHeader(const TLEVEL &fLevel)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	std::string separator = getSeparator();
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpaceExtended(1);
	int obsWidth = getObsWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int headerWidth = 10;

	// first line
	(*stream) << '\n' << TABs;
	(*stream).writeStringLeft(nameWidth, "LEVEL INSTRUMENT: " + fLevel.instrument.ID);
	(*stream) << '\n';

	// second line: Definition of the reference point
	(*stream) << TABs;
	(*stream).writeStringLeft(headerWidth, "REF POINT");
	(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName());
	(*stream).writeStringLeft(headerWidth, "X (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, fLevel.fMeasuredPlane->getReferencePoint()->getEstimatedValueInRoot().getX().getMetresValue());
	(*stream).writeStringLeft(headerWidth, "Y (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, fLevel.fMeasuredPlane->getReferencePoint()->getEstimatedValueInRoot().getY().getMetresValue());
	(*stream).writeStringLeft(headerWidth, "Z (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, fLevel.fMeasuredPlane->getReferencePoint()->getEstimatedValueInRoot().getZ().getMetresValue());

	// third line: definition of the PLANE
	(*stream) << '\n';
	(*stream) << TABs;
	(*stream).writeStringLeft(headerWidth, "PLANE");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(headerWidth, "IH (M)");
	(*stream).writeDouble(obsWidth, lengthPrecision, fLevel.fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue());
	std::string ihFixed = "TRUE";
	if (!fLevel.ihfix)
	{
		(*stream).writeStringLeft(headerWidth, "SIH (MM)");
		(*stream).writeDouble(obsWidth, lengthResPrecision, fLevel.fMeasuredPlane->getRefPDistEstimatedPrecision().getMMetresValue());
		ihFixed = "FALSE";
	}
	(*stream).writeStringLeft(headerWidth, "IHFIX");
	(*stream).writeString(obsWidth, ihFixed);
	(*stream).writeStringLeft(headerWidth, "IHSE (MM)");
	(*stream).writeDouble(obsWidth, lengthResPrecision, fLevel.instrument.sigmaInstrHeight.getMMetresValue());

	(*stream) << '\n' << '\n';
}

void TLEVELWriter::writeDLEVResultsHeader(int nOObs)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// first line
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDLEV), nOObs);
	// Second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the target
	(*stream).writeString(obsWidth, "OBSERVE"); // DLEV: mesurement
	(*stream).writeString(obsResWidth, "SIGMA"); // DLEV: sigma
	(*stream).writeString(obsWidth, "CALCULE"); // DLEV: estimation
	(*stream).writeString(obsResWidth, "RESIDU"); // DLEV: residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // DLEV: residual/sigma
	(*stream).writeString(obsWidth, "COLLIMATION"); // DLEV: collimation angle
	(*stream).writeString(obsResWidth, "SCOLL"); // DLEV: sigma of collimation angle
	(*stream).writeString(obsWidth, "CONST"); // DLEV: Distance correction
	(*stream).writeString(obsResWidth, "SCONST"); // DLEV: 1-Sigma precision for the vertical distance correction
	(*stream).writeString(obsResWidth, "OBSE"); // DLEV: 1-sigma precision for the vertical distance observation
	(*stream).writeString(obsResWidth, "PPM"); // DLEV: ppm value for the vertical distance correction
	(*stream).writeString(obsWidth, "H_TRGT"); // DLEV: Vertical offset of the staff
	(*stream).writeString(obsResWidth, "THSE"); // DLEV: 1-sigma precision for the vertical offset of the staff

	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // Observation identifier

	(*stream) << "\n";

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeString(nameWidth, ""); // POSITION
	(*stream).writeString(obsWidth, "(M)"); // DLEV: mesurement
	(*stream).writeString(obsResWidth, "(MM)"); // DLEV: sigma
	(*stream).writeString(obsWidth, "(M)"); // DLEV: estimation
	(*stream).writeString(obsResWidth, "(MM)"); // DLEV: residual
	(*stream).writeString(obsResWidth, ""); // DLEV: residual/sigma
	(*stream).writeString(obsWidth, "GONS"); // DLEV: collimation angle
	(*stream).writeString(obsResWidth, "CC"); // DLEV: sigma collimation angle

	(*stream).writeString(obsWidth, "(M)"); // DLEV: Distance correction
	(*stream).writeString(obsResWidth, "(MM)"); // DLEV: 1-Sigma precision for the distance correction
	(*stream).writeString(obsResWidth, "(MM)"); // DLEV: 1-sigma precision for the vertical distance observation
	(*stream).writeString(obsResWidth, "(MM/KM)"); // DLEV: ppm value for the distance correction
	(*stream).writeString(obsWidth, "(M)"); // DLEV: Vertical offset of the staff
	(*stream).writeString(obsResWidth, "(MM)"); // DLEV: 1-sigma precision for the vertical offset of the staff

	(*stream) << "\n";
}

void TLEVELWriter::writeDLEVDHORResultsHeader(int nOObs)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	std::string TABs = stream->getCurrSpaceExtended(2);
	////////////////////////////////////////////////////////////
	// first line
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDLEVDHOR), nOObs);
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the target
	(*stream).writeString(obsWidth, "OBSERVE"); // DLEV DHOR: mesurement
	(*stream).writeString(obsResWidth, "SIGMA"); // DLEV DHOR: sigma
	(*stream).writeString(obsWidth, "CALCULE"); // DLEV DHOR: estimation
	(*stream).writeString(obsResWidth, "RESIDU"); // DLEV DHOR: residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // DLEV DHOR: residual/sigma
	(*stream).writeString(obsResWidth, "DSE"); // DLEV DHOR: 1-sigma precision of the horizontal distance
	(*stream).writeString(obsResWidth, "DHPPM"); // DLEV DHOR: ppm value for the horizontal distance correction
	(*stream).writeString(obsWidth, "DHDCOR"); // DLEV DHOR: Distance correction value for the horizontal distance correction
	(*stream) << "\n";
	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeString(nameWidth, ""); // POSITION
	(*stream).writeString(obsWidth, "(M)"); // DLEV: mesurement
	(*stream).writeString(obsResWidth, "(MM)"); // DLEV: sigma
	(*stream).writeString(obsWidth, "(M)"); // DLEV: estimation
	(*stream).writeString(obsResWidth, "(MM)"); // DLEV: residual
	(*stream).writeString(obsResWidth, ""); // DLEV: residual/sigma
	(*stream).writeString(obsResWidth, "(MM)"); // DHOR: 1-sigma precision of the horizontal distance
	(*stream).writeString(obsResWidth, "(MM/KM)"); // DHOR: ppm value for the horizontal distance correction
	(*stream).writeString(obsWidth, "(M)"); // DHOR: Distance correction value for the horizontal distance correction
	(*stream) << "\n";
}

//------------------ Result data---------------------------------------------------------------------------
void TLEVELWriter::writeLEVELResults(const TLEVEL &fLevel)
{
	writeLEVELHeader(fLevel);

	if (!fLevel.measDLEV.empty())
	{
		// Write the DLEV Result
		writeDLEVResults(fLevel.measDLEV, fLevel.instrument);

		if (fLevel.hasDHOR)
			// Write the eventual DLEV:DHOR
			writeDLEVDHORResults(fLevel.measDLEV, fLevel.nbDHOR);
	}
}

void TLEVELWriter::writeDLEVResults(const std::list<TDLEV> &measDLEV, const TInstrumentData::TLEVEL &instr)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string TABs = stream->getCurrSpaceExtended(2);

	writeDLEVResultsHeader((int)measDLEV.size());

	// For each DHOR measurement of the station
	for (auto const &itDlev : measDLEV)
	{
		(*stream) << TABs;

		// Position of the target
		(*stream).writeStringLeft(nameWidth, itDlev.targetPos->getName());

		// DLEV
		//  mesured offset
		(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.getDistance());
		// sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.target.sigmaCombinedDist.getMMetresValue());
		// estimated offset
		(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.getDistanceResidual() + itDlev.getDistance());
		// residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.getDistanceResidual().getMMetresValue());
		// residual/sima
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.getDistanceResidual().getMetresValue() / itDlev.target.sigmaCombinedDist.getMetresValue());

		// Collimation angle
		if (!instr.collAngleAdjustable->isFixed())
		{
			if (isAllfixed)
				if (!isnotanumber(itDlev.fAllFixedCollimation))
					(*stream).writeDouble(obsWidth, getAnglePrecision(), itDlev.fAllFixedCollimation.getGonsValue());
				else
					(*stream).writeString(obsWidth, "FIXED");
			else
				(*stream).writeDouble(obsWidth, getAnglePrecision(), instr.collAngleAdjustable->getEstimatedValue().getGonsValue());

			(*stream).writeDouble(obsResWidth, getAngleResidualPrecision(), instr.collAngleAdjustable->getEstimatedPrecision().getSignedCCValue());
		}
		else
		{
			(*stream).writeDouble(obsWidth, getAnglePrecision(), instr.collAngleAdjustable->getProvisionalValue().getGonsValue());
			(*stream).writeString(obsResWidth, "FIXED");
		}

		(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.target.distCorrectionValue.getMetresValue()); // DHOR: Distance correction
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.target.sigmaDCorr.getMMetresValue()); // DHOR: 1-Sigma precision for the distance correction
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.target.sigmaD.getMMetresValue()); // DHOR: 1-sigma precision for the vertical distance observation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.target.ppmD.getMMetresValue()); // DHOR: ppm value for the distance correction
		(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.target.staffHt.getMetresValue()); // DHOR: Vertical offset of the staff
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.target.sigmaStaffHt.getMMetresValue()); // DHOR: 1-sigma precision for the vertical offset of the staff

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, itDlev.obsID);
		(*stream) << "\n";
	}
	(*stream) << "\n";
}

void TLEVELWriter::writeDLEVDHORResults(const std::list<TDLEV> &measDLEV, const int &numberOfDHOR)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string TABs = stream->getCurrSpaceExtended(2);

	writeDLEVDHORResultsHeader(numberOfDHOR);

	// For each DHOR measurement of the station
	for (auto const &itDlev : measDLEV)
	{
		if (itDlev.dhor)
		{
			(*stream) << TABs;
			// Position of the target
			(*stream).writeStringLeft(nameWidth, itDlev.targetPos->getName());
			// measured dhor
			(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.dhor.get()->getDistance());
			// sigma Dhor
			(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.dhor->target.sigmaCombinedDHor.getMMetresValue());
			// estimated offset
			(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.dhor.get()->getDistanceResidual() + itDlev.dhor.get()->getDistance());
			// residual
			(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.dhor.get()->getDistanceResidual().getMMetresValue());
			// res/sigma
			(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.dhor.get()->getDistanceResidual() / itDlev.dhor->target.sigmaCombinedDHor.getMetresValue());

			(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.dhor->target.sigmaDHor.getMMetresValue()); // DHOR: 1-sigma precision of the horizontal distance
			(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.dhor->target.ppmDHor.getMMetresValue()); // DHOR: ppm value for the horizontal distance correction
			(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.dhor->target.dhorCorrectionValue.getMetresValue()); // DHOR: Distance correction value for the horizontal distance correction
			(*stream) << "\n";
		}
	}
	(*stream) << "\n";
}

//------------------ Simu data---------------------------------------------------------------------------
void TLEVELWriter::writeLEVELSIMUResults(const TLEVEL &fLevel)
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpaceExtended(2);

	writeLEVELHeader(fLevel);

	if (fLevel.measDLEV.size() > 0)
	{
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDLEV), (int)fLevel.measDLEV.size());
		writeDistanceResultsSummary(fLevel.getDLEVObsSummary(), TABs);
	}
	// The DHOR result summary
	if (fLevel.hasDHOR)
	{
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDLEVDHOR), fLevel.nbDHOR);
		writeDistanceResultsSummary(fLevel.getDHORObsSummary(), TABs);
	}
}

//------------------ Reliability header---------------------------------------------------------------------------
void TLEVELWriter::writeReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT REF", "POINT 2", "", "OBSERVATION", "M", "MM");
	return;
}

//------------------ Reliability data---------------------------------------------------------------------------
void TLEVELWriter::writeDHORReliabilityData(const TLEVEL &fLevel, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each station
	for (auto const &ItDhor : fLevel.measDLEV)
	{
		// if we have a dhor measurements, we write the data
		if (ItDhor.dhor)
		{
			// Observation index to take the right value in the statistic vector
			int index = ItDhor.dhor->getFirstObservationIndex();

			// get reference point to the plane
			(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName());
			// get Tg point
			(*stream).writeStringLeft(nameWidth, ItDhor.targetPos->getName());
			// get Point 3
			(*stream).writeStringLeft(nameWidth, "");

			// get the observed distance
			(*stream).writeDouble(obsWidth, lengthPrecision, ItDhor.dhor->getDistance());
			// get the standard deviation
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDhor.dhor->target.sigmaCombinedDHor.getMMetresValue());
			// get the residual
			(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDhor.dhor->getDistanceResidual().getMMetresValue());

			writeReliabilityMM(index, stat);
		}
	}
	return;
}

void TLEVELWriter::writeDLEVReliabilityData(const TLEVEL &fLevel, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each DLEV measurement of the station
	for (auto const &itDlev : fLevel.measDLEV)
	{
		// Observation index to take the right value in the statistic vector
		int index = itDlev.getFirstObservationIndex();

		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, fLevel.fMeasuredPlane->getReferencePoint()->getName());
		// get Tg point
		(*stream).writeStringLeft(nameWidth, itDlev.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, itDlev.getDistance());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.target.sigmaCombinedDist.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

//------------------ Synthesis header-------------------------------------------------------------------------
void TLEVELWriter::writeLEVELSynthesisHeader()
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
	(*stream).writeStringLeft(nameWidth, "DLEV_PLANE"); // plane name
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

//------------------ Synthesis data--------------------------------------------------------------------------

void TLEVELWriter::writeLEVELResultsSynthesis(std::list<const TLGCObsSummary *> &dlevsum)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string TABs = stream->getCurrSpaceExtended(1);

	for (auto const &itDlev : dlevsum)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, itDlev->getObsText()); // Reference point
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev->getResMax()); // residu max
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev->getResMin()); // residu min
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev->getMean()); // residu moy
		(*stream).writeDouble(obsResWidth, lengthResPrecision, itDlev->getStdev()); // ecart type
		(*stream) << endl;
	}
}