// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TCAMWriter.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <Global.h>
#include <TAMeas.h>

#include "TAStreamFormatter.h"
#include "TObservationFormat.h"

TCAMWriter::TCAMWriter(TAStreamFormatter &stream, bool /*hist*/) : TObservationWriter(stream)
{
}

TCAMWriter::~TCAMWriter()
{
}

//------------------ Result header---------------------------------------------------------------------------
void TCAMWriter::writeCAMResults(const TCAM &camera)
{
	writeCAMHeader(camera);

	if (camera.measUVD.size() > 0)
		writeUVDResults(camera.measUVD);

	if (camera.measUVEC.size() > 0)
		writeUVECResults(camera.measUVEC);

	if (camera.measUV.size() > 0)
		writeUVResults(camera.measUV);
}

void TCAMWriter::writeCAMHeader(const TCAM &camera)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	// first line
	(*stream) << endl << TABs;
	(*stream).writeStringLeft(nameWidth, "CAMERA INSTRUMENT: " + camera.instrument.ID);
	(*stream) << endl;
	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Point on which is the CAM positioned
	(*stream).writeStringLeft(nameWidth, camera.instrumentPos->getName());
	(*stream) << endl << endl;
	///////////////////////////////////////////////////////////////////////////////////

	if (camera.instrument.isPixelCamera)
		writeInteriorOrientation(camera);
}

void TCAMWriter::writeInteriorOrientation(const TCAM &camera)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	std::string TABs = stream->getCurrSpaceExtended(1);

	// pixel parameters with the precision of the pixel coordinates, the unitless distortion, which spans orders of magnitude, in
	// scientific notation with as many significant digits. The values are padded with spaces so that the decimal points of a
	// column line up; a column holds a sign, five digits before the decimal point and the decimals of a coefficient with its exponent.
	const int pixelPrecision = getLengthPrecision();
	const int tailWidth = pixelPrecision + 3; // characters after the decimal point of a coefficient
	const int valueWidth = std::max(obsWidth, 7 + tailWidth);
	const int parameterWidth = 9, unitWidth = 4; // "PARAMETER", "UNIT"
	auto aligned = [pixelPrecision, tailWidth](TReal value, bool inPixels) {
		std::ostringstream text;
		if (inPixels)
			text << std::fixed << std::setprecision(pixelPrecision) << value;
		else
			text << std::scientific << std::setprecision(std::max(pixelPrecision - 1, 0)) << value;
		const std::string number = text.str();
		const size_t point = number.find('.');
		const int tail = point == std::string::npos ? 0 : (int)(number.size() - point - 1);
		return number + std::string(std::max(tailWidth - tail, 0), ' ');
	};
	auto significant = [](TReal value, int digits) {
		std::ostringstream text;
		text << std::setprecision(digits) << value;
		return text.str();
	};

	(*stream) << TABs << "INTERIOR ORIENTATION" << endl;
	(*stream) << TABs;
	(*stream).writeStringLeft(parameterWidth, "PARAMETER");
	(*stream).writeString(unitWidth, "UNIT");
	(*stream).writeString(valueWidth, "PROVISIONAL");
	(*stream).writeString(valueWidth, "ESTIMATED");
	(*stream).writeString(valueWidth, "SIGMA");
	(*stream).writeString(valueWidth, "SIGMA APRI");
	(*stream) << endl;

	for (int i = 0; i < TInstrumentData::TCAMD::kNumIOParams; i++)
	{
		const TAdjustableScalar *ioAdj = camera.instrument.io[i].adjustable;
		const bool inPixels = TInstrumentData::TCAMD::ioParamInPixels(i);

		(*stream) << TABs;
		(*stream).writeStringLeft(parameterWidth, TInstrumentData::TCAMD::ioParamNames[i]);
		(*stream).writeString(unitWidth, inPixels ? "(PX)" : "-");
		(*stream).writeString(valueWidth, aligned(ioAdj->getProvisionalValue(), inPixels));
		(*stream).writeString(valueWidth, aligned(ioAdj->getEstimatedValue(), inPixels));
		(*stream).writeString(valueWidth, ioAdj->isFixed() ? std::string("FIXED") : aligned(ioAdj->getEstimatedPrecision(), inPixels));
		(*stream).writeString(valueWidth, camera.instrument.io[i].sigma > 0.0 ? aligned(camera.instrument.io[i].sigma, inPixels) : std::string());
		(*stream) << endl;
	}
	(*stream) << endl;

	const auto &sensor = camera.instrument.sensorSize;
	if (!camera.instrument.hasSensor())
		return;
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "SENSOR");
	(*stream) << significant(sensor[0], 8) + " X " + significant(sensor[1], 8) + " PX" << endl << endl;
}

void TCAMWriter::writeUVDResultsHeader(int nOObs)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// first line
	(*stream) << TABs;
	this->writeObsTitle(this->getObsDescriptionFR(TCAMWriter::kUVD), nOObs);
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); // Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the target
	//(*stream).writeString(obsWidth,	"HEIGHT"); //target height
	//(*stream).writeString(obsWidth,	"CONST"); //provisional dist corr
	//(*stream).writeString(obsWidth,	"CONST CALC"); //calculated distance corr
	//(*stream).writeString(obsResWidth,	"SCCONST");  //sigma distance corr

	(*stream).writeString(obsWidth, "OBSXV"); // mesured x vector component
	(*stream).writeString(obsResWidth, "SXV"); // sigma x vector component
	(*stream).writeString(obsWidth, "CALCXV"); // estimated x vector component
	(*stream).writeString(obsResWidth, "RESXV"); // residual

	(*stream).writeString(obsWidth, "OBSYV"); // mesured y vector component
	(*stream).writeString(obsResWidth, "SYV"); // sigma y vector component
	(*stream).writeString(obsWidth, "CALCYV"); // estimated y vector component
	(*stream).writeString(obsResWidth, "RESYV"); // residual

	(*stream).writeString(obsWidth, "OBSDIST"); // mesured DIST
	(*stream).writeString(obsResWidth, "SDIST"); // sigma DIST
	(*stream).writeString(obsWidth, "CALCDIST"); // estimated DIST
	(*stream).writeString(obsResWidth, "RESDIST"); // residual

	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // Observation identifier

	(*stream) << endl;

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // TARGET ID
	(*stream).writeStringLeft(nameWidth, ""); // POSITION
	//	(*stream).writeString(obsWidth,	"(M)"); //height of the target
	//	(*stream).writeString(obsWidth,	"(M)"); //provisional dist corr
	//	(*stream).writeString(obsWidth,	"(M)"); //calculated distance corr
	//	(*stream).writeString(obsResWidth,	"(MM)"); //sigma distance corr

	(*stream).writeString(obsWidth, "()"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); // sigma observed value
	(*stream).writeString(obsWidth, "()"); // estimated
	(*stream).writeString(obsResWidth, "()"); // residual

	(*stream).writeString(obsWidth, "()"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); // sigma observed value
	(*stream).writeString(obsWidth, "()"); // estimated
	(*stream).writeString(obsResWidth, "()"); // residual

	(*stream).writeString(obsWidth, "(M)"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(MM)"); // sigma observed value
	(*stream).writeString(obsWidth, "(M)"); // estimated DIST
	(*stream).writeString(obsResWidth, "(MM)"); // residual
	(*stream) << endl;
}

void TCAMWriter::writeUVECResultsHeader(int nOObs)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// first line
	(*stream) << TABs;
	this->writeObsTitle(this->getObsDescriptionFR(TALGCObjectWriter::kUVEC), nOObs);
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "TRGT"); // Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the target
	//(*stream).writeString(obsWidth,	"HEIGHT"); //target height

	(*stream).writeString(obsWidth, "OBSXV"); // mesured x vector component
	(*stream).writeString(obsResWidth, "SXV"); // sigma x vector component
	(*stream).writeString(obsWidth, "CALCXV"); // estimated x vector component
	(*stream).writeString(obsResWidth, "RESXV"); // residual

	(*stream).writeString(obsWidth, "OBSYV"); // mesured y vector component
	(*stream).writeString(obsResWidth, "SYV"); // sigma y vector component
	(*stream).writeString(obsWidth, "CALCYV"); // estimated y vector component
	(*stream).writeString(obsResWidth, "RESYV"); // residual

	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // Observation identifier

	(*stream) << endl;

	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // TARGET ID
	(*stream).writeStringLeft(nameWidth, ""); // POSITION
	//(*stream).writeString(obsWidth,	"(M)"); //height of the target

	(*stream).writeString(obsWidth, "()"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); // sigma observed value
	(*stream).writeString(obsWidth, "()"); // estimated
	(*stream).writeString(obsResWidth, "()"); // residual

	(*stream).writeString(obsWidth, "()"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "()"); // sigma observed value
	(*stream).writeString(obsWidth, "()"); // estimated
	(*stream).writeString(obsResWidth, "()"); // residual

	(*stream) << endl;
}

void TCAMWriter::writeUVResultsHeader(int nOObs)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// first line
	(*stream) << TABs;
	this->writeObsTitle(this->getObsDescriptionFR(TALGCObjectWriter::kUV), nOObs);
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(2, ""); // Marker of a target behind the camera
	(*stream).writeStringLeft(nameWidth, "TRGT"); // Name of the target used
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the target

	(*stream).writeString(obsWidth, "OBSU"); // mesured u pixel coordinate
	(*stream).writeString(obsResWidth, "SU"); // sigma u
	(*stream).writeString(obsWidth, "CALCU"); // estimated u
	(*stream).writeString(obsResWidth, "RESU"); // residual

	(*stream).writeString(obsWidth, "OBSV"); // mesured v pixel coordinate
	(*stream).writeString(obsResWidth, "SV"); // sigma v
	(*stream).writeString(obsWidth, "CALCV"); // estimated v
	(*stream).writeString(obsResWidth, "RESV"); // residual

	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // Observation identifier

	(*stream) << endl;

	(*stream) << TABs;
	(*stream).writeStringLeft(2, ""); // MARKER
	(*stream).writeStringLeft(nameWidth, ""); // TARGET ID
	(*stream).writeStringLeft(nameWidth, ""); // POSITION
	for (int i = 0; i < 8; i++)
		(*stream).writeString(i % 2 == 0 ? obsWidth : obsResWidth, "(PX)");

	(*stream) << endl;
}

//------------------ Result data---------------------------------------------------------------------------
void TCAMWriter::writeUVDResults(const std::list<TUVD> &measUVD)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	writeUVDResultsHeader((int)measUVD.size()); // write the title line for the observations
	(*stream) << endl;
	for (auto const &ItUVD : measUVD)
	{
		(*stream) << TABs;
		// write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItUVD.target.ID);
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItUVD.targetPos->getName());

		// write the target height
		//(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.target.targetHt);

		// write the distance cste (TLength (M))
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		//	(*stream)<<ItUVD.target.distCorrectionAdjustable->getProvisionalValue()<<separator;

		// X component
		// write the observed X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getMetresValue());

		// write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaCombinedX.getMMetresValue()); /*Unitless, delete or replace with appropriate conversion*/

		// write the estimated X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getX().getMetresValue() + ItUVD.getXCompVectorResidual());

		// write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getXCompVectorResidual() * M2MM); /*Unitless, delete or replace with appropriate conversion*/

		// Y vector component
		// write the observed Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getMetresValue());

		// write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaCombinedY.getMMetresValue()); /*Unitless, delete or replace with appropriate conversion*/

		// write the estimated Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getVectorValue().getY().getMetresValue() + ItUVD.getYCompVectorResidual());

		// write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getYCompVectorResidual() * M2MM); /*Unitless, delete or replace with appropriate conversion*/

		// DISTANCE
		// measured DIST value
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance());

		// write the sigma DIST
		// Output sigmadist, Output value in meters [mm], stored in [m], lower the precision because it of the m2mm conversion
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.target.sigmaCombinedDist.getMMetresValue());

		// write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVD.getDistance() + ItUVD.getDistanceResidual());

		// write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVD.getDistanceResidual().getMMetresValue()); /*Unitless, delete or replace with appropriate conversion*/

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItUVD.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

void TCAMWriter::writeUVECResults(const std::list<TUVEC> &measUVEC)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	writeUVECResultsHeader((int)measUVEC.size()); // write the title line for the observations
	(*stream) << endl;
	for (auto const &ItUVEC : measUVEC)
	{
		(*stream) << TABs;
		// write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItUVEC.target.ID);
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItUVEC.targetPos->getName());

		// write the target height
		//(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.target.targetHt);

		// X component
		// write the observed X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX());

		// write the sigma X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.target.sigmaCombinedX.getMMetresValue()); /*Unitless, delete or replace with appropriate conversion*/

		// write the estimated X vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getX() + ItUVEC.getXCompVectorResidual());

		// write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.getXCompVectorResidual() * M2MM); /*Unitless, delete or replace with appropriate conversion*/

		// Y vector component
		// write the observed Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY());

		// write the sigma Y vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.target.sigmaCombinedY.getMMetresValue()); /*Unitless, delete or replace with appropriate conversion*/

		// write the estimated Y vector component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUVEC.getVectorValue().getY() + ItUVEC.getYCompVectorResidual());

		// write the residual X vector component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUVEC.getYCompVectorResidual() * M2MM); /*Unitless, delete or replace with appropriate conversion*/

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItUVEC.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

void TCAMWriter::writeUVResults(const std::list<TUV> &measUV)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsIdWidth = getObsIdWidth();
	int obsResWidth = getObsResWidth();
	int pixelPrecision = getLengthPrecision();
	int pixelResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string TABs = stream->getCurrSpaceExtended(2);

	writeUVResultsHeader((int)measUV.size()); // write the title line for the observations
	(*stream) << endl;
	for (auto const &ItUV : measUV)
	{
		const Eigen::VectorXd obs = ItUV.getObsVector();
		const TReal sigma[2] = {ItUV.target.sigmaCombinedX.getMetresValue(), ItUV.target.sigmaCombinedY.getMetresValue()};
		const TReal residual[2] = {ItUV.getXCompVectorResidual(), ItUV.getYCompVectorResidual()};

		(*stream) << TABs;
		// mark a target behind the camera, explained below the table
		(*stream).writeStringLeft(2, ItUV.behindCamera ? "*" : "");
		// write TARGET ID
		(*stream).writeStringLeft(nameWidth, ItUV.target.ID);
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItUV.targetPos->getName());

		// u and v pixel coordinates
		for (int i = 0; i < 2; i++)
		{
			// write the observed pixel coordinate
			(*stream).writeDouble(obsWidth, pixelPrecision, obs[i]);

			// write the sigma
			(*stream).writeDouble(obsResWidth, pixelResPrecision, sigma[i]);

			// write the estimated pixel coordinate
			(*stream).writeDouble(obsWidth, pixelPrecision, obs[i] + residual[i]);

			// write the residual
			(*stream).writeDouble(obsResWidth, pixelResPrecision, residual[i]);
		}

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItUV.obsID);

		(*stream) << endl;
	}
	if (std::any_of(measUV.begin(), measUV.end(), [](const TUV &uv) { return uv.behindCamera; }))
		(*stream) << TABs << "* TARGET BEHIND THE CAMERA, PROJECTED MIRRORED THROUGH THE PROJECTION CENTRE" << endl;
	(*stream) << endl;
}

//------------------ Simu data---------------------------------------------------------------------------

void TCAMWriter::writeCAMResultsSIMU(const TCAM &camera)
{
	TAStreamFormatter *stream = getStream();
	std::string TABs = stream->getCurrSpaceExtended(2);
	writeCAMHeader(camera);

	if (camera.measUVD.size() > 0)
	{
		const auto &summary = camera.getUVDObsSummary();
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVD) + ": XVECT", (int)camera.measUVD.size());
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs);
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVD) + ": YVECT", (int)camera.measUVD.size());
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs);
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVD) + ": DIST", (int)camera.measUVD.size());
		writeDistanceResultsSummary(summary.distObsSum, TABs);
	}
	if (camera.measUVEC.size() > 0)
	{
		const auto &summary = camera.getUVECObsSummary();
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVEC) + ": XVECT", (int)camera.measUVEC.size());
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs);
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUVEC) + ": YVECT", (int)camera.measUVEC.size());
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs);
	}
	if (!camera.measUV.empty())
	{
		const auto &summary = camera.getUVObsSummary();
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUV) + ": U", (int)camera.measUV.size());
		writeUnitlessResultsSummary(summary.xVectorCompObsSum, TABs, "[PX]");
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kUV) + ": V", (int)camera.measUV.size());
		writeUnitlessResultsSummary(summary.yVectorCompObsSum, TABs, "[PX]");
	}
}

//------------------ Reliability header--------------------------------------------------------------------
void TCAMWriter::writeUVECReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PIVOT", "TARGET", "", "OBS_i", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("PIVOT", "TARGET", "", "OBS_j", "[]", "[]");
	return;
}

void TCAMWriter::writeUVReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PIVOT", "TARGET", "", "OBS_u", "PX", "PX");
	this->TObservationWriter::writeReliabilityHeader("", "", "", "OBS_v", "PX", "PX");
	return;
}

void TCAMWriter::writeUVDReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("PIVOT", "TARGET", "", "OBS_i", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("", "", "", "OBS_j", "[]", "[]");
	this->TObservationWriter::writeReliabilityHeader("", "", "", "OBS_dist", "M", "MM");
	return;
}

//------------------ Reliability data---------------------------------------------------------------------------

void TCAMWriter::writeUVECReliabilityData(const TCAM &fCam, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each DHOR measurement of the station
	for (auto const &ItUvec : fCam.measUVEC)
	{
		//------------------- 1rst obs----------------------------------------------------//
		// Observation index to take the right value in the statistic vector
		int index = ItUvec.getFirstObservationIndex();

		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, fCam.instrumentPos->getName());
		// get Tg point
		(*stream).writeStringLeft(nameWidth, ItUvec.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed i component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvec.getVectorValue().getX());
		// get the sigma of i component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.target.sigmaCombinedX.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.getXCompVectorResidual() * M2MM);

		writeReliabilityMM(index, stat);

		//------------------- 2nd obs----------------------------------------------------//
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		// get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		index = index + 1;

		// get the observed j component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvec.getVectorValue().getY());
		// get the sigma of j component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.target.sigmaCombinedY.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvec.getYCompVectorResidual() * M2MM);

		writeReliabilityMM(index, stat);
	}
	return;
}

void TCAMWriter::writeUVDReliabilityData(const TCAM &fCam, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each DHOR measurement of the station
	for (auto const &ItUvd : fCam.measUVD)
	{
		//------------------- 1rst obs----------------------------------------------------//
		// Observation index to take the right value in the statistic vector
		int index = ItUvd.getFirstObservationIndex();

		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, fCam.instrumentPos->getName());
		// get Tg point
		(*stream).writeStringLeft(nameWidth, ItUvd.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed i component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvd.getVectorValue().getX());
		// get the sigma of i component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaCombinedX.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.getXCompVectorResidual() * M2MM);

		writeReliabilityMM(index, stat);

		//------------------- 2nd obs----------------------------------------------------//
		index = index + 1;
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		// get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed j component
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvd.getVectorValue().getY());
		// get the sigma of j component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaCombinedX.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.getYCompVectorResidual() * M2MM);

		writeReliabilityMM(index, stat);

		//------------------- 3rd obs----------------------------------------------------//
		index = index + 1;
		// get reference point to the plane
		(*stream).writeStringLeft(nameWidth, "");
		// get Tg point
		(*stream).writeStringLeft(nameWidth, "");
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItUvd.getDistance());
		// get the sigma of dist component
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.target.sigmaCombinedDist.getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItUvd.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

void TCAMWriter::writeUVReliabilityData(const TCAM &fCam, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int pixelPrecision = getLengthPrecision();
	int pixelResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	for (auto const &ItUV : fCam.measUV)
	{
		const Eigen::VectorXd obs = ItUV.getObsVector();
		const TReal sigma[2] = {ItUV.target.sigmaCombinedX.getMetresValue(), ItUV.target.sigmaCombinedY.getMetresValue()};
		const TReal residual[2] = {ItUV.getXCompVectorResidual(), ItUV.getYCompVectorResidual()};

		// u and v pixel coordinates, the station and the target are written on the first line only
		for (int i = 0; i < 2; i++)
		{
			// Observation index to take the right value in the statistic vector
			int index = ItUV.getFirstObservationIndex() + i;

			(*stream).writeStringLeft(nameWidth, i == 0 ? fCam.instrumentPos->getName() : "");
			(*stream).writeStringLeft(nameWidth, i == 0 ? ItUV.targetPos->getName() : "");
			(*stream).writeStringLeft(nameWidth, "");

			(*stream).writeDouble(obsWidth, pixelPrecision, obs[i]);
			(*stream).writeDouble(obsResWidth, pixelResPrecision, sigma[i]);
			(*stream).writeDouble(obsResWidth, pixelResPrecision, residual[i]);

			writeReliabilityPX(index, stat);
		}
	}
	return;
}

//------------------ Synthesis header-------------------------------------------------------------------------
void TCAMWriter::writeUVDSynthesisHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	// int				obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	(*stream).writeStringLeft(nameWidth, "UVD"); // instrument
	(*stream) << endl;
	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // instrument
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream).writeString(obsResWidth, "d(MM)");
	(*stream) << endl;
}

void TCAMWriter::writeUVECSynthesisHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	// int				obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	(*stream).writeStringLeft(nameWidth, "UVEC"); // instrument
	(*stream) << endl;
	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // instrument
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream) << endl;
	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "x");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream).writeString(obsResWidth, "y");
	(*stream) << endl;

	(*stream) << endl;
}

void TCAMWriter::writeResultsSynthesisHeaderUnitless(const std::string &unit)
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
	(*stream).writeStringLeft(nameWidth, "INSTR_POS"); // instrument
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream) << endl;
	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, unit);
	(*stream).writeString(obsResWidth, unit);
	(*stream).writeString(obsResWidth, unit);
	(*stream).writeString(obsResWidth, unit);
	(*stream) << endl;
}

void TCAMWriter::writeResultsSynthesisHeaderDistance()
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
	(*stream).writeStringLeft(nameWidth, "INSTR_POS"); // instrument
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
void TCAMWriter::writeDefResultsSynthesis(std::list<const TLGCObsSummary *> &meassum, int obsResWidth, int ResPrecision)
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
