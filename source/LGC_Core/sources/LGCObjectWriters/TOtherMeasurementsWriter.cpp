#include "TOtherMeasurementsWriter.h"

#include <TAMeas.h>

#include "LGCAdjustablePoint.h"
#include "TAStreamFormatter.h"
#include "TObservationFormat.h"

TOtherMeasurementWriter::TOtherMeasurementWriter(TAStreamFormatter &stream, bool /*hist*/) : TObservationWriter(stream)
{
}

TOtherMeasurementWriter::~TOtherMeasurementWriter()
{
}

//------------------ Result header---------------------------------------------------------------------------
void TOtherMeasurementWriter::writeDVERResultsHeader()
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
	(*stream).writeStringLeft(nameWidth, "POINT 1"); // Position of the inst
	(*stream).writeStringLeft(nameWidth, "POINT 2"); // Position of the scale
	(*stream).writeString(obsWidth, "OBSERVE"); // observed dver
	(*stream).writeString(obsResWidth, "SIGMA"); // sigma
	(*stream).writeString(obsWidth, "CALCULE"); // estimated dver
	(*stream).writeString(obsResWidth, "RESIDU"); // residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // residual/sigma
	(*stream).writeString(obsWidth, "DCOR"); // DVER: distance correction value
	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // Observation ID
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // Position of the inst
	(*stream).writeStringLeft(nameWidth, ""); // Position of the scale
	(*stream).writeString(obsWidth, "(M)"); // observed dver
	(*stream).writeString(obsResWidth, "(MM)"); // sigma observed value
	(*stream).writeString(obsWidth, "(M)"); // estimated dver
	(*stream).writeString(obsResWidth, "(MM)"); // residual
	(*stream).writeString(obsResWidth, ""); // residual/sigma
	(*stream).writeString(obsWidth, "(M)"); // DVER: distance correction value
	(*stream) << endl;
}

void TOtherMeasurementWriter::writeORIEResultsHeader()
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
	(*stream).writeStringLeft(nameWidth, "STATION"); // Position of the inst
	(*stream).writeStringLeft(nameWidth, "POINT"); // Position of the scale
	(*stream).writeString(obsWidth, "OBSERVE"); // observed orie
	(*stream).writeString(obsResWidth, "SIGMA"); // sigma
	(*stream).writeString(obsWidth, "CALCULE"); // estimated orie
	(*stream).writeString(obsResWidth, "RESIDU"); // residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // residu/sigma
	(*stream).writeString(nameWidth, "TRGT"); // Name of the target
	(*stream).writeString(obsResWidth, "OBSE"); // observation sigma ORIE
	(*stream).writeString(obsResWidth, "TCSE"); // target centering sigma
	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // observation ID
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // Position of the inst
	(*stream).writeStringLeft(nameWidth, ""); // Position of the scale
	(*stream).writeString(obsWidth, "(GON)"); // observed orie
	(*stream).writeString(obsResWidth, "(CC)"); // sigma observed value
	(*stream).writeString(obsWidth, "(GON)"); // estimated orie
	(*stream).writeString(obsResWidth, "(CC)"); // residual
	(*stream).writeString(obsResWidth, ""); // residu/sigma
	(*stream).writeString(nameWidth, ""); // TARGET ID
	(*stream).writeString(obsResWidth, "(CC)"); // observation sigma ORIE
	(*stream).writeString(obsResWidth, "(MM)"); // target centering sigma
	(*stream) << endl;
}

void TOtherMeasurementWriter::writePDORResultsHeader()
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
	(*stream).writeString(nameWidth, "CALA POINT");
	(*stream).writeString(nameWidth, "POINT");
	(*stream).writeString(obsWidth, "OBSERVE"); // observed bearing
	(*stream).writeString(obsWidth, "CALCULE"); // estimated bearing
	(*stream).writeString(obsResWidth, "RESIDU"); // residual
	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // observation id
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(obsWidth, "(GON)"); // observed bearing
	(*stream).writeString(obsWidth, "(GON)"); // estimated bearing
	(*stream).writeString(obsResWidth, "(CC)"); // residual
	(*stream) << endl;
}

void TOtherMeasurementWriter::writeRADIResultsHeader()
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
	(*stream).writeStringLeft(nameWidth, "POINT"); // Position of the ponit
	(*stream).writeString(obsWidth, "GIS"); // observed radi
	(*stream).writeString(obsResWidth, "SIGMA"); // sigma
	(*stream).writeString(obsResWidth, "RESIDU"); // residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // residual/sigma
	(*stream).writeString(obsResWidth, "OBSE"); // observation standard error
	(*stream).writeString(obsWidth, "ACST"); // constant angle
	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // observation id
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // Position of the inst
	(*stream).writeString(obsWidth, "(GON)"); // observed radi
	(*stream).writeString(obsResWidth, "(MM)"); // sigma observed value
	(*stream).writeString(obsResWidth, "(MM)"); // residual
	(*stream).writeString(obsResWidth, ""); // residual/sigma
	(*stream).writeString(obsResWidth, "(MM)"); // observation standard error
	(*stream).writeString(obsWidth, "(GON)"); // constant angle
	(*stream) << endl;
}

void TOtherMeasurementWriter::writeOBSXYZResultsHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TAB;
	(*stream).writeStringLeft(nameWidth, "POINT"); // Position of the ponit
	(*stream).writeString(obsResWidth, "SIGMA"); // sigma
	(*stream).writeString(obsResWidth, "RESIDU"); // residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // residual/sigma
	if (obsIdWidth != 0)
		(*stream).writeString(obsIdWidth, "ID"); // Observation identifier
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TAB;
	(*stream).writeStringLeft(nameWidth, ""); // Position of the inst
	(*stream).writeString(obsResWidth, "(MM)"); // sigma observed value
	(*stream).writeString(obsResWidth, "(MM)"); // residual
	(*stream).writeString(obsResWidth, ""); // residual/sigma
	(*stream) << endl;
}

void TOtherMeasurementWriter::writePointSigmaResultsHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int headerWidth = 9;

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TAB;
	(*stream).writeStringLeft(nameWidth, "POINT"); // point name
	for (auto angleName : pointSigmaData().fAngleNames)
		(*stream).writeString(headerWidth, angleName); // angle
	(*stream).writeString(headerWidth, "SX"); // a-priori sigma
	(*stream).writeString(headerWidth, "SY"); // a-priori sigma
	(*stream).writeString(headerWidth, "SZ"); // a-priori sigma
	(*stream).writeString(headerWidth, "RESX"); // residual
	(*stream).writeString(headerWidth, "RESY"); // residual
	(*stream).writeString(headerWidth, "RESZ"); // residual
	(*stream).writeString(headerWidth, "RESX"); // residual/sigma
	(*stream).writeString(headerWidth, "RESY"); // residual/sigma
	(*stream).writeString(headerWidth, "RESZ"); // residual/sigma
	(*stream).writeString(headerWidth, "SXPOST"); // a-posteriori sigma
	(*stream).writeString(headerWidth, "SYPOST"); // a-posteriori sigma
	(*stream).writeString(headerWidth, "SZPOST"); // a-posteriori sigma
	(*stream) << "\n";

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TAB;
	(*stream).writeStringLeft(nameWidth, ""); // point name
	(*stream).writeString(headerWidth, "(GON)"); // angle
	(*stream).writeString(headerWidth, "(GON)"); // angle
	(*stream).writeString(headerWidth, "(GON)"); // angle
	(*stream).writeString(headerWidth, "(GON)"); // angle
	(*stream).writeString(headerWidth, "(MM)"); // a-priori sigma
	(*stream).writeString(headerWidth, "(MM)"); // a-priori sigma
	(*stream).writeString(headerWidth, "(MM)"); // a-priori sigma
	(*stream).writeString(headerWidth, "(MM)"); // residual
	(*stream).writeString(headerWidth, "(MM)"); // residual
	(*stream).writeString(headerWidth, "(MM)"); // residual
	(*stream).writeString(headerWidth, "/SX"); // residual/sigma
	(*stream).writeString(headerWidth, "/SY"); // residual/sigma
	(*stream).writeString(headerWidth, "/SZ"); // residual/sigma
	(*stream).writeString(headerWidth, "(MM)"); // a-posteriori sigma
	(*stream).writeString(headerWidth, "(MM)"); // a-posteriori sigma
	(*stream).writeString(headerWidth, "(MM)"); // a-posteriori sigma
	(*stream) << "\n";
}

//------------------ Result data---------------------------------------------------------------------------
void TOtherMeasurementWriter::writePDORResults(const TPdorObs &fPDOR)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	int anglePrecision = getAnglePrecision();

	// first line
	this->writeObsTitle(this->getObsDescriptionFR(TALGCObjectWriter::kPDOR), 1);

	(*stream).writeString(nameWidth, "CALA POINT");
	(*stream).writeString(nameWidth, fPDOR.calaPt->getName());
	(*stream).writeString(nameWidth, "POINT");
	(*stream).writeString(nameWidth, fPDOR.orientationPt->getName());
	(*stream).writeString(obsWidth, "OBS (GON)"); // observed bearing
	(*stream).writeDouble(obsWidth, anglePrecision, fPDOR.getBearing().getGonsValue());
	(*stream).writeString(obsWidth, "CALC (GON)"); // estimated bearing
	(*stream).writeDouble(obsWidth, anglePrecision, fPDOR.getBearing().getGonsValue() + fPDOR.getAngleResidual().getGonsValue());
	(*stream).writeString(obsResWidth, "RES (CC)"); // residual
	(*stream).writeDouble(obsResWidth, angleResPrecision, fPDOR.getAngleResidual().getSignedCCValue());
	(*stream) << endl;
	(*stream) << endl << endl;
}

void TOtherMeasurementWriter::writeDVERResults(const std::list<TDVER> &fDVER)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// first line
	(*stream) << stream->getCurrSpaceExtended(1) << "DVER" << endl;
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDVER), (int)fDVER.size());
	writeDVERResultsHeader(); // write the title line for the observations

	for (auto const &ItDVER : fDVER)
	{
		(*stream) << TABs;
		// write INST POSITION
		(*stream).writeStringLeft(nameWidth, ItDVER.station->getName());
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItDVER.targetPos->getName());

		// write the observed DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDVER.getDistance()); // Output value in meters [m], stored in [m]

		// write the sigma DIST
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getObservedStDev().getMMetresValue()); // Output value in meters [mm], stored in [m]

		// write the estimated DIST
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDVER.getDistance() + ItDVER.getDistanceResidual()); // Output value in meters [m], stored in [m]

		// write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getDistanceResidual().getMMetresValue()); // Output value in meters [mm], stored in [m]

		// write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDVER.getDistanceResidual() / ItDVER.getObservedStDev()); // Output value in meters [mm], stored in [m]

		// write the distance correction value
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDVER.getDistanceCorrection()); // Output value in meters [m], stored in [m]

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItDVER.obsID);
		(*stream) << endl;
	}
	(*stream) << endl;
}

void TOtherMeasurementWriter::writeORIEResults(const std::list<TORIE> &fORIE, const LGCAdjustablePoint &instPos)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int angleResidualPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	int anglePrecision = getAnglePrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// first line
	(*stream) << stream->getCurrSpaceExtended(1) << "CONTRAINTES D'ORIENTATION" << endl;
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kORIE), (int)fORIE.size());
	writeORIEResultsHeader(); // write the title line for the observations

	for (auto const &ItORIE : fORIE)
	{
		(*stream) << TABs;
		// write INST POSITION
		(*stream).writeStringLeft(nameWidth, instPos.getName());
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItORIE.targetPos->getName());

		// write the observed ORIE
		(*stream).writeDouble(obsWidth, anglePrecision, ItORIE.getAngle().getGonsValue());

		// write the sigma ORIE
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.target.sigmaCombinedAngle.getSignedCCValue());

		// write the estimated ORIE
		TReal value = ItORIE.getAngle().getGonsValue() + ItORIE.getAngleResidual().getGonsValue();
		(*stream).writeDouble(obsWidth, anglePrecision, (value > 400.0 ? value - 400 : value));

		// write the residual
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.getAngleResidual().getSignedCCValue());

		// write the resi/sigma
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.getAngleResidual().getSignedCCValue() / ItORIE.target.sigmaCombinedAngle.getSignedCCValue());

		// write TARGET ID
		(*stream).writeString(nameWidth, ItORIE.target.ID);

		// Write the sigma of the angle observation (OBSE)
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.target.sigmaAngl.getSignedCCValue());

		// Write the sigma of the target centering (TCSE)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItORIE.target.sigmaTargetCentering.getMMetresValue());

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItORIE.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

void TOtherMeasurementWriter::writeRADIResults(const std::list<TRADI> &fRADI)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int anglePrecision = getAnglePrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// first line
	(*stream) << stream->getCurrSpaceExtended(1) << "CONTRAINTES RADIALES" << endl;
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kRADI), (int)fRADI.size());
	writeRADIResultsHeader(); // write the title line for the observations

	for (auto const &It : fRADI)
	{
		(*stream) << TABs;
		// write INST POSITION
		(*stream).writeString(nameWidth, It.station->getName());

		// write the bearing
		(*stream).writeDouble(obsWidth, anglePrecision, It.getAngleCnstr().getGonsValue());

		// write the sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getObservedStDev().getMMetresValue());

		// write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getResidual().getMMetresValue());

		// write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getResidual() / It.getObservedStDev());

		// write the observation standard error (OBSE)
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getObservedStDev().getMMetresValue());

		// write the constant angle (ACST)
		(*stream).writeDouble(obsWidth, anglePrecision, It.getConstAngle().getGonsValue());

		// Write the observation identifier
		(*stream).writeString(obsIdWidth, It.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

void TOtherMeasurementWriter::writeOBSXYZResults(const std::list<TOBSXYZ> &fOBSXYZ)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// first line
	this->writeObsTitle(TAB + this->getObsDescriptionFR(TALGCObjectWriter::kOBSXYZ), (int)fOBSXYZ.size());
	writeOBSXYZResultsHeader(); // write the title line for the observations

	for (auto const &It : fOBSXYZ)
	{
		// X
		// write INST POSITION
		(*stream) << TAB;
		(*stream).writeStringLeft(nameWidth, It.station->getName() + " (x)");
		// write the sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getXObservedStDev().getMMetresValue());
		// write the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getXResidual().getMMetresValue());
		// write the residual/sigma
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getXResidual() / It.getXObservedStDev());
		// Write the observation identifier
		(*stream).writeString(obsIdWidth, It.obsID);
		(*stream) << endl;

		// Y
		(*stream) << TAB;
		(*stream).writeStringLeft(nameWidth, It.station->getName() + " (y)");
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getYObservedStDev().getMMetresValue());
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getYResidual().getMMetresValue());
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getYResidual() / It.getYObservedStDev());
		(*stream).writeString(obsIdWidth, It.obsID);
		(*stream) << endl;
		// Z
		(*stream) << TAB;
		(*stream).writeStringLeft(nameWidth, It.station->getName() + " (z)");
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getZObservedStDev().getMMetresValue());
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getZResidual().getMMetresValue());
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getZResidual() / It.getZObservedStDev());
		(*stream).writeString(obsIdWidth, It.obsID);
		(*stream) << endl;
	}
	(*stream) << endl;
}

void TOtherMeasurementWriter::writePointSigmaResults(const LGCAdjustablePointCollection &pointCollection, TDataTreeIterator itTree)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);
	int anglePrecision = getAnglePrecision();

	int headerWidth = 9;

	// first line, count points with sigma in this frame
	int nPointsWithSigma = std::count_if(
		pointCollection.begin(), pointCollection.end(), [itTree](const auto &pt) { return pt.hasPointSigma() && pt.getFrameTreePosition() == itTree; });

	if (nPointsWithSigma > 0)
	{
		this->writeObsTitle(TAB + "POINTS WITH SIGMA", nPointsWithSigma);
		(*stream) << "	LES PRECISIONS ET LES RESIDUS SONT EXPRIMES PAR RAPPORT AU(X) SYSTEME(S) DE COORDONNEES TOURNE(S).\n";
		writePointSigmaResultsHeader(); // write the title line for the observations

		for (const auto &pt : pointCollection)
		{
			if (pt.hasPointSigma() && pt.getFrameTreePosition() == itTree)
			{
				const pointSigmaData &ptSigma = pt.getPointSigmaData();
				Eigen::Vector3d rotOffset = ptSigma.fRotRes;
				Eigen::Vector3d rotOffsetNormalized = ptSigma.fRotResNormalized;

				Eigen::Matrix3d rotatedCovarMat = ptSigma.fRotCovar;

				(*stream) << TAB;
				(*stream).writeStringLeft(nameWidth, pt.getName());
				for (const TAngle &angle : ptSigma.fAngles)
					(*stream).writeDouble(headerWidth, anglePrecision, angle.getGonsValue());

				Eigen::Vector3d rotatedAposterioriSigmas = rotatedCovarMat.diagonal().array().sqrt();
				// write the given precisions
				if (ptSigma.fHasApriCovMat)
				{
					for (int j = 0; j < 3; j++)
						(*stream).writeStringLeft(headerWidth, "*APRICOV*");
				}
				else
				{
					for (int j = 0; j < 3; j++)
						(*stream).writeDouble(headerWidth, lengthResPrecision, ptSigma.fSigmas(j) * M2MM);
				}
				// write the residuals (=offsets in the rotated sytems)
				for (int j = 0; j < 3; j++)
					(*stream).writeDouble(headerWidth, lengthResPrecision, rotOffset(j) * M2MM);
				// write the normalized residual
				if (ptSigma.fHasApriCovMat)
				{
					for (int j = 0; j < 3; j++)
						(*stream).writeStringLeft(headerWidth, "*APRICOV*");
				}
				else
				{
					for (int j = 0; j < 3; j++)
						(*stream).writeDouble(headerWidth, lengthResPrecision, rotOffsetNormalized(j));
				}
				// write the a-posteriori precisions
				for (int j = 0; j < 3; j++)
					(*stream).writeDouble(headerWidth, lengthResPrecision, rotatedAposterioriSigmas(j) * M2MM);

				(*stream) << "\n";
			}
		}
		(*stream) << "\n";
	}
}

void TOtherMeasurementWriter::writeDVERSIMUResults(const TMeasurements &tmeas)
{
	TAStreamFormatter *stream = getStream();
	// Third hierarchy level from local FRAME
	std::string TABs = stream->getCurrSpaceExtended(2);

	if (!tmeas.fDVER.empty())
	{
		(*stream) << TAB << "DIFFERENCES VERTICALES";
		(*stream) << endl << endl;
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kDVER), (int)tmeas.fDVER.size());
		writeDistanceResultsSummary(tmeas.getDVERObsSummary(), TABs);
	}
}

void TOtherMeasurementWriter::writeORIESIMUResults(const TORIEROM &orieRom)
{
	TAStreamFormatter *stream = getStream();
	std::string STAB = stream->getCurrSpaceExtended(1);
	std::string TABs = stream->getCurrSpaceExtended(2);

	(*stream) << STAB << "CONTRAINTES D ORIENTATION";
	// Write definition of ROM
	(*stream) << endl << endl;
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kORIE), (int)orieRom.measORIE.size());
	writeAngleResultsSummary(orieRom.getORIEObsSummary(), TABs);
}

void TOtherMeasurementWriter::writeRADISIMUResults(const TMeasurements &tmeas)
{
	TAStreamFormatter *stream = getStream();
	// Third hierarchy level from local FRAME
	std::string TABs = stream->getCurrSpaceExtended(2);

	if (!tmeas.fRADI.empty())
	{
		(*stream) << TAB << "CONTRAINTES RADIALES";
		(*stream) << endl << endl;
		this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kRADI), (int)tmeas.fRADI.size());
		writeDistanceResultsSummary(tmeas.getRADIObsSummary(), TABs);
	}
}

//------------------ Reliability header---------------------------------------------------------------------------
void TOtherMeasurementWriter::writeDVERReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT 1", "POINT 2", "", "OBSERVATION", "M", "MM");
	return;
}

void TOtherMeasurementWriter::writeORIEReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("STATION", "POINT", "", "OBSERVATION", "GON", "CC");
	return;
}

void TOtherMeasurementWriter::writeRADIReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT", "", "", "OBSERVATION", "M", "MM");
	return;
}

void TOtherMeasurementWriter::writeOBSXYZReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("POINT", "", "", "OBSERVATION", "M", "MM");
	return;
}

//------------------ Reliability data---------------------------------------------------------------------------
void TOtherMeasurementWriter::writeDVERReliabilityData(const std::list<TDVER> &fDVER, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each DVER measurement of the station
	for (auto const &ItDver : fDVER)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItDver.getFirstObservationIndex();

		// get Point 1 of the line or the linenumber?
		(*stream).writeStringLeft(nameWidth, ItDver.station->getName());
		// get Tg point
		(*stream).writeStringLeft(nameWidth, ItDver.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed distance
		(*stream).writeDouble(obsWidth, lengthPrecision, ItDver.getDistance());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDver.getObservedStDev().getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, ItDver.getDistanceResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

void TOtherMeasurementWriter::writeORIEReliabilityData(const std::list<TORIE> &fORIE, const TLGCStatistic &stat, const LGCAdjustablePoint &instPos)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int anglePrecision = getAnglePrecision();
	int angleResidualPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	std::string separator = getSeparator();

	// For each orie measurement of the station
	for (auto const &ItORIE : fORIE)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItORIE.getFirstObservationIndex();

		// get Point 1 of the line or the linenumber?
		(*stream).writeStringLeft(nameWidth, instPos.getName());
		// get Tg point
		(*stream).writeStringLeft(nameWidth, ItORIE.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed ORIE
		(*stream).writeDouble(obsWidth, anglePrecision, ItORIE.getAngle().getGonsValue());
		// write the sigma ORIE
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.target.sigmaCombinedAngle.getSignedCCValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, angleResidualPrecision, ItORIE.getAngleResidual().getSignedCCValue());

		writeReliabilityCC(index, stat);
	}
	return;
}

void TOtherMeasurementWriter::writeRADIReliabilityData(const std::list<TRADI> &fRADI, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each RADI measurement of the station
	for (auto const &It : fRADI)
	{
		// Observation index to take the right value in the statistic vector
		int index = It.getFirstObservationIndex();

		// get Point 1 of the line or the linenumber?
		(*stream).writeStringLeft(nameWidth, It.station->getName());
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");

		// get the bearing
		(*stream).writeDouble(obsWidth, lengthPrecision, It.getAngleCnstr() + It.getConstAngle());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getObservedStDev().getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getResidual().getMMetresValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

void TOtherMeasurementWriter::writeOBSXYZReliabilityData(const std::list<TOBSXYZ> &fOBSXYZ, const TLGCStatistic &stat)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int lengthPrecision = getLengthPrecision();
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	// For each OBSXYZ measurement of the station
	for (auto const &It : fOBSXYZ)
	{
		// Observation index to take the right value in the statistic vector
		int index = It.getFirstObservationIndex();

		// get Point X
		(*stream).writeStringLeft(nameWidth, It.station->getName() + " (x)");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		// get the observed X coordinate
		(*stream).writeDouble(obsWidth, lengthPrecision, It.obsValue.getX().getMetresValue());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getXObservedStDev().getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getXResidual().getMMetresValue());
		writeReliabilityMM(index, stat);

		// Y
		(*stream).writeStringLeft(nameWidth, It.station->getName() + " (y)");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		// get the observed Y coordinate
		(*stream).writeDouble(obsWidth, lengthPrecision, It.obsValue.getY().getMetresValue());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getYObservedStDev().getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getYResidual().getMMetresValue());
		writeReliabilityMM(index + 1, stat);

		// Z
		(*stream).writeStringLeft(nameWidth, It.station->getName() + " (z)");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		// get the observed Z coordinate
		(*stream).writeDouble(obsWidth, lengthPrecision, It.obsValue.getZ().getMetresValue());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getZObservedStDev().getMMetresValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, lengthResPrecision, It.getZResidual().getMMetresValue());
		writeReliabilityMM(index + 2, stat);
	}
	return;
}

//------------------ Synthesis header---------------------------------------------------------------------------
void TOtherMeasurementWriter::writeResultsSynthesisHeader()
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

void TOtherMeasurementWriter::writeResultsSynthesisHeaderAngles()
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
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream) << endl;
}

//------------------ Synthesis data---------------------------------------------------------------------------
void TOtherMeasurementWriter::writeDefResultsSynthesis(std::list<const TLGCObsSummary *> &meassum, int obsResWidth, int ResPrecision)
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
