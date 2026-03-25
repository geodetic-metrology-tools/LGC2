// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

////////////////////////////////////////////////////////////////////
// TPunchFileWriter.cpp :Implementation file
//
// Write an LGC output file for point coordinates.
// Creates a file from the calculation results and sends the appropriate messages.
//
/////////////////////////////////////////////////////////////////////

// other forward declarations
#include "TPunchFileWriter.h"

#include <StringManager.h>
#include <TLGCData.h>
#include <TLOR2LOR.h>

#include "TAGeoidModel.h"
#include "TAStreamFormatter.h"
#include "TLGCApp.h"
#include "TPointConverter.h"
#include "TRefSystemFactory.h"
#include "TXYH2CCS.h"

/////////////////////////////////////////////////////////////////////////////
// constructor / destructor
/////////////////////////////////////////////////////////////////////////////

TPunchFileWriter::TPunchFileWriter() : TAFileWriter(), fData(nullptr)
{
}

TPunchFileWriter::TPunchFileWriter(TAStreamFormatter *stream, const TLGCData *project) : TAFileWriter(stream, project), fData(project)
{
	stream->setSeparator(project->getConfig().CustomOutputSeparatorPunch.separator);
}

TPunchFileWriter::~TPunchFileWriter()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void TPunchFileWriter::writeFile(const std::string error)
{ // write error messages from project
	writeTitle();
	writeError(error);
}

// Write the LGC results for the given project
void TPunchFileWriter::writeFile()
{
	// write the header title of the results file
	this->writeTitle();

	this->writePoints();

	// Write the FRAME TRANSFORMATION PARAMETERS section only if there are more frames than "ROOT".
	if (fProjectData->getNumberOfFrames() > 1)
	{
		this->writeFrameSectionTitle();
		this->writeFrameSectionHeader();
		this->writeFrameSectionData();
	}
}

void TPunchFileWriter::writePoints()
{
	switch (fProjectData->getConfig().writePunch.fmode)
	{
	case TLGCConfig::TCoordOut::kPLAIN:
		writeXYZHeader();
		break;
	case TLGCConfig::TCoordOut::kE:
		writeXYZVarCovarDeltaHeader();
		break;
	case TLGCConfig::TCoordOut::kEE:
		writeXYZErrorEllHeader();
		break;
	case TLGCConfig::TCoordOut::kH:
		writeXYHHeader();
		break;
	case TLGCConfig::TCoordOut::kZ:
		writeXYZHeader();
		break;
	case TLGCConfig::TCoordOut::kHZ:
		writeXYZHHeader();
		break;
	case TLGCConfig::TCoordOut::kHN:
		writeXYHNHeader();
		break;
	case TLGCConfig::TCoordOut::kZHN:
		writeXYZHNHeader();
		break;
	case TLGCConfig::TCoordOut::kT:
		writeXYZSigmaHeader();
		break;
	case TLGCConfig::TCoordOut::kOUT1:
		writeCooHeader();
		break;
		// case TLGCConfig::TCoordOut::kOUT3: 	writeXYZHeader(); break;
	}

	for (auto point : fProjectData->getPoints())
		writePoint(point, fProjectData->getConfig().writePunch.fmode);
}

void TPunchFileWriter::writePoint(LGCAdjustablePoint const &point, TLGCConfig::TCoordOut::eMode output_type)
{
	TAStreamFormatter *stream = getStream();
	stream->setLengthUnits(TLength::kMetres);
	stream->setWidthFormat(getCoordWidth());
	stream->setPrecisionFormat(getCoordPrecision());

	(*stream).setWidthFormat(20);
	auto writeName = [&](LGCAdjustablePoint const &point) { (*stream) << point.getName() << " "; };
	(*stream).setWidthFormat(8);

	// lambda function to transform the point coordinate in root
	auto point_in_root = [&](LGCAdjustablePoint const &point) {
		// Transformation of the point to the root
		TPositionVector estimatedValue = point.getEstimatedValue();
		TDataTreeIterator root = fProjectData->getTree().begin();

		if (root != point.getFrameTreePosition())
		{
			TLOR2LOR transfo = TLOR2LOR(point.getFrameTreePosition(), fProjectData->getTree().begin(), "toroot");
			transfo.transform(estimatedValue);
		}

		// Set new coordinates to the point expressed in the root
		LGCAdjustablePoint point_int_root = LGCAdjustablePoint(estimatedValue, point.isCoordinateFixed(0), point.isCoordinateFixed(1), point.isCoordinateFixed(2),
			point.getName(), point.getReferenceFrame(), point.getFrameTreePosition());
		point_int_root.eolcomment = point.eolcomment;

		return point_int_root;
	};

	auto pointRoot = point_in_root(point);

	switch (output_type)
	{
	///< Nom_Pt X Y Z
	case TLGCConfig::TCoordOut::kPLAIN:
		writeXYZData(pointRoot);
		break;

	///< Nom_Pt X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz
	case TLGCConfig::TCoordOut::kE:
		writeXYZVarCovarDeltaData(point);
		break;

	///< Nom_Pt X Y Z Gist_gd_axe Gd_axe Pt_axe Sz dx dy dz
	case TLGCConfig::TCoordOut::kEE:
		if (point.getFrameTreePosition()->get()->isROOTNode())
			writeXYZErrorEllData(point);
		else
			writeXYZData(pointRoot);
		break;

	///< Nom_Pt X Y H
	case TLGCConfig::TCoordOut::kH:
		writeXYHData(pointRoot);
		break;

	///< Nom_Pt X Y Z
	case TLGCConfig::TCoordOut::kZ:
		writeXYZData(pointRoot);
		break;

	///< Nom_Pt X Y Z H
	case TLGCConfig::TCoordOut::kHZ:
		writeXYZHData(pointRoot);
		break;

	///< Nom_Pt X Y Z H NLEP
	case TLGCConfig::TCoordOut::kHN:
		writeXYHNData(pointRoot);
		break;

	///< Nom_Pt X Y Z H NLEP
	case TLGCConfig::TCoordOut::kZHN:
		writeXYZHNData(pointRoot);
		break;

	///< Nom_Pt X Y Z Sx Sy Sz
	case TLGCConfig::TCoordOut::kT:
		writeXYZSigmaData(point);
		break;

	///< .coo file for GEODE
	case TLGCConfig::TCoordOut::kOUT1:
		if (point.getFrameTreePosition()->get()->isROOTNode())
			writeCooData(point);
		else
			writeCooData(pointRoot);
		break;

	///< .mes file for GEODE
	case TLGCConfig::TCoordOut::kOUT3:

		// ?
		break;
	}

	(*stream) << endl;
}

void TPunchFileWriter::writeTitle()
{
	TAStreamFormatter *stream = getStream();
	// write software id.
	(*stream) << "#" << (TLGCApp::getProgId()) << endl;

	// write software copyright
	(*stream) << "#" << (TLGCApp::getCopyright()) << endl;

	// write title
	(*stream) << "# " << endl;
	(*stream) << "#" << (fProjectData->getConfig().title) << endl;

	// Add warning for CONSI LIBR usage
	if (fProjectData->getConfig().useConsiLibr.isActive())
	{
		(*stream) << "#WARNING: THIS IS THE RESULT OF A FREE CALCULATION WITH *CONSI LIBR, SEE .LOG2 FILE. DO NOT INSERT THESE COORDINATES!" << '\n';
	}
	else
	{
		(*stream) << "#" << '\n';
	}

	// write date and time
	(*stream) << "#CALCUL DU " << TLGCApp::getStartProcessingTimestamp() << ". PROCESSING ELAPSED SECONDS " << TLGCApp::getProcessingElapsedSeconds() << endl;
	(*stream) << "#";
	if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		(*stream) << "*SPHE";
	else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		(*stream) << "*LEP";
	else if (fProjectData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		(*stream) << "*RS2K";
	else
		(*stream) << "*OLOC";
	(*stream) << endl;
	(*stream) << "%" << endl;
	(*stream) << "%" << endl;
}

void TPunchFileWriter::writeXYZHeader()
{ // XYZ
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeXYZVarCovarDeltaHeader()
{ // X Y Z Vx Vy Vz Cxy Cxz Cyz Dx Dy Dz

	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();
	int covCoordPrec = 2 * getCoordPrecision() + 1;

	if (fData->getConfig().useApriori.isActive())
	{
		(*stream) << "%";
		(*stream) << "LES SIGMAS ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)" << endl;
		(*stream) << "%" << endl;
	}
	else
	{
		(*stream) << "%";
		(*stream) << "LES SIGMAS ET COVARIANCES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI" << endl;
		(*stream) << "%" << endl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "SX ");
	(*stream).writeString(coordResWidth, "SY ");
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(covCoordPrec, "COVXY");
	(*stream).writeString(covCoordPrec, "COVXZ");
	(*stream).writeString(covCoordPrec, "COVYZ");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordWidth, "DX ");
	(*stream).writeString(coordWidth, "DY ");
	(*stream).writeString(coordWidth, "DZ ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(covCoordPrec, "(MM2)");
	(*stream).writeString(covCoordPrec, "(MM2)");
	(*stream).writeString(covCoordPrec, "(MM2)");
	(*stream).width(1);
	(*stream) << "";
	(*stream).writeString(coordWidth, "(MM)");
	(*stream).writeString(coordWidth, "(MM)");
	(*stream).writeString(coordWidth, "(MM)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeXYZErrorEllHeader()
{ // X Y Z gist_gd_axe Gd_axe Pt_axe Sz Cyz Dx Dy Dz

	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	// int				obsWidth = max(getObsWidth(), 11);

	if (fData->getConfig().useApriori.isActive())
	{
		(*stream) << "%";
		(*stream) << "LES ELLIPSES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)" << endl;
		(*stream) << "%" << endl;
	}
	else
	{
		(*stream) << "%";
		(*stream) << "LES ELLIPSES SONT CALCULEES PAR RAPPORT AU SIGMA ZERO A POSTERIORI" << endl;
		(*stream) << "%" << endl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM  ");
	stream->writeString(coordWidth, "X");
	stream->writeString(coordWidth, "Y");
	stream->writeString(coordWidth, "Z");
	// Direction vectors: (+0.000, -0.000, +0.000) => 24
	const int vecwidth(24);
	stream->writeString(vecwidth, "DIRECTION X");
	stream->writeString(vecwidth, "DIRECTION Y");
	stream->writeString(vecwidth, "DIRECTION Z");
	stream->writeString(coordWidth, "LONGUEUR X");
	stream->writeString(coordWidth, "LONGUEUR Y");
	stream->writeString(coordWidth, "LONGUEUR Z");
	stream->writeString(coordWidth, "DX");
	stream->writeString(coordWidth, "DY");
	stream->writeString(coordWidth, "DZ");

	*stream << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	stream->writeString(nameWidth, ""); // Nom
	stream->writeString(coordWidth, "(M)"); // coordianate
	stream->writeString(coordWidth, "(M)"); // coordianate
	stream->writeString(coordWidth, "(M)"); // coordianate
	stream->writeString(vecwidth, ""); // Direction
	stream->writeString(vecwidth, ""); // Direction
	stream->writeString(vecwidth, ""); // Direction
	stream->writeString(coordWidth, "(MM)"); // Axis length
	stream->writeString(coordWidth, "(MM)"); // Axis length
	stream->writeString(coordWidth, "(MM)"); // Axis length
	stream->writeString(1, ""); // just to align unit with the title
	stream->writeString(coordWidth, "(MM)"); // delta
	stream->writeString(coordWidth, "(MM)"); // delta
	stream->writeString(coordWidth, "(MM)"); // delta
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeXYHHeader()
{ // XYH

	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeXYZHHeader()
{ // XYZH

	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeXYHNHeader()
{ // XYHN

	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	/////////////////////////////////////////////////////////////////////////////////////////////
	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeXYZHNHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "H ");
	(*stream).writeString(coordWidth, "N ");
	(*stream) << endl;

	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeXYZSigmaHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();

	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordResWidth, "SX ");
	(*stream).writeString(coordResWidth, "SY ");
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream) << endl;

	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeCooHeader()
{ // XYH

	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	// First line
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "NOM");
	(*stream).writeString(coordWidth, "X ");
	(*stream).writeString(coordWidth, "Y ");
	if (fData->getConfig().referential != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		(*stream).writeString(coordWidth, "H ");
	else
		(*stream).writeString(coordWidth, "Z ");
	(*stream).writeString(coordWidth, "ID");
	(*stream).writeString(coordWidth, "DX ");
	(*stream).writeString(coordWidth, "DY ");
	(*stream).writeString(coordWidth, "DZ ");
	(*stream).writeString(coordWidth, "DCUM ");
	(*stream).writeString(nameWidth, "OPTION ");
	(*stream) << endl;

	// second line : units
	(*stream).width(1);
	(*stream) << "%";
	(*stream).writeString(nameWidth, "");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "(M)");
	(*stream).writeString(coordWidth, "");
	(*stream).writeString(coordWidth, "(MM)");
	(*stream).writeString(coordWidth, "(MM)");
	(*stream).writeString(coordWidth, "(MM)");
	(*stream) << endl;
	(*stream) << "%" << endl;

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA
///////////////////////////////////////////////////////////////////////////////////////////////////////

void TPunchFileWriter::writeXYZData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	std::string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());
	return;
}

void TPunchFileWriter::writeXYHData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	std::string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstValue(0), point.getEstValue(1), TLength(point.getHEstValue()));
	return;
}

void TPunchFileWriter::writeXYZVarCovarDeltaData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int covCoordPrec = 2 * getCoordPrecision() + 1;

	std::string separator = stream->getSeparator();

	if (point.getFrameTreePosition()->get()->isROOTNode())
	{
		(*stream).width(1);
		(*stream) << "";
		converter.writeName(point.getName(), nameWidth);
		converter.writeXYZ(getCoordWidth(), getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());

		// Variance
		(*stream).width(1);
		(*stream) << "";

		// Create pseudo TLength to write correctly variance and covariance values (precision, digits...)
		converter.writeCoordinateParam(point.getSpatialStatus(), getCoordResWidth(), getCoordPrecision(), TLength::kMillimetres, separator,
			TLength(point.getXEstPrecision().getMMetresValue(), TLength::EUnits::kMillimetres), TLength(point.getYEstPrecision().getMMetresValue(), TLength::EUnits::kMillimetres),
			TLength(point.getZEstPrecision().getMMetresValue(), TLength::EUnits::kMillimetres), " ");
		(*stream).width(1);
		(*stream) << "";
		// Covariance
		converter.writeCoordinateParam(point.getSpatialStatus(), covCoordPrec, covCoordPrec - 6, TLength::kMetres, separator,
			TLength(point.getXYCovar(), TLength::EUnits::kMetres) * TReal(1e6), TLength(point.getXZCovar(), TLength::EUnits::kMetres) * TReal(1e6),
			TLength(point.getYZCovar(), TLength::EUnits::kMetres) * TReal(1e6), " ", true);

		(*stream).width(1);
		(*stream) << "";

		// Delta
		TPositionVector xyzValue = point.getProvisionalValue();

		converter.writeCoordinateParam(point.getSpatialStatus(), getCoordWidth(), getCoordPrecision(), TLength::EUnits::kMillimetres, separator, point.getDXValue(),
			point.getDYValue(), point.getDZValue(), " ");
	}
	else
	{
		(*stream).width(1);
		(*stream) << "";
		converter.writeName(point.getName(), nameWidth);

		// Transformation of the point to the root
		TPositionVector estimatedValue = point.getEstimatedValue();
		TPositionVector provisionalValue = point.getProvisionalValue();
		TDataTreeIterator root = fProjectData->getTree().begin();

		if (root != point.getFrameTreePosition())
		{
			TLOR2LOR transfo = TLOR2LOR(point.getFrameTreePosition(), fProjectData->getTree().begin(), "toroot");
			transfo.transform(estimatedValue);
			transfo.transform(provisionalValue);
		}

		// transform sigma in root
		TFreeVector sigmaRoot;
		sigmaRoot = point.transformSigmaInRoot(point, fProjectData);

		converter.writeXYZ(getCoordWidth(), getCoordPrecision(), TLength::kMetres, separator, estimatedValue);

		// SIGMA
		(*stream).width(1);
		(*stream) << "";
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(getCoordResWidth());
		stream->setPrecisionFormat(getCoordPrecision());
		(*stream) << TLength((sigmaRoot.getX()));
		(*stream) << (separator);
		(*stream) << TLength((sigmaRoot.getY()));
		(*stream) << (separator);
		(*stream) << TLength((sigmaRoot.getZ()));
		(*stream) << (separator);
		(*stream).width(1);
		(*stream) << "";

		// No Covariance for the points which are not defined in ROOT
		writeString(getCoordResWidth(), " ");
		(*stream) << (separator);
		writeString(getCoordResWidth(), " ");
		(*stream) << (separator);
		writeString(getCoordResWidth(), " ");
		(*stream) << (separator);
		(*stream).width(1);
		(*stream) << "";

		// Delta
		converter.writeCoordinateParam(point.getSpatialStatus(), getCoordWidth(), getCoordPrecision(), TLength::kMillimetres, separator,
			TLength(estimatedValue.getX() - provisionalValue.getX()), TLength(estimatedValue.getY() - provisionalValue.getY()),
			TLength(estimatedValue.getZ() - provisionalValue.getZ()), " ");
	}

	return;
}

void TPunchFileWriter::writeXYZErrorEllData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	int coordResWidth = getCoordResWidth();
	// int				obsWidth = max(getObsWidth(),11);
	std::string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());

	if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxyz)
	{ // Error ellipsoid
		const auto &ell(point.getErrorEllipsoid());

		(*stream).width(1);
		(*stream) << "";
		const int vecwidth(24);
		char vecstr[32]; // format the vector output here and write as a string
		sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vx[0], ell.vx[1], ell.vx[2]);
		stream->writeString(vecwidth, vecstr);
		sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vy[0], ell.vy[1], ell.vy[2]);
		stream->writeString(vecwidth, vecstr);
		sprintf(vecstr, "(% .3f  % .3f  % .3f)", ell.vz[0], ell.vz[1], ell.vz[2]);
		stream->writeString(vecwidth, vecstr);
		stream->writeDouble(coordWidth, coordResWidth, ell.lx * M2MM);
		stream->writeDouble(coordWidth, coordResWidth, ell.ly * M2MM);
		stream->writeDouble(coordWidth, coordResWidth, ell.lz * M2MM);
	}
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxy || point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxz
		|| point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVyz)
	{ // Error ellipse

		(*stream).width(1);
		(*stream) << "";
		const int vecwidth(24);
		// char vecstr[32]; // format the vector output here and write as a string
		stream->writeDouble(vecwidth, coordResWidth, point.getErrorEllGis().getGonsValue());
		stream->writeString(vecwidth, "");
		stream->writeString(vecwidth, "");
		stream->writeDouble(coordWidth, coordResWidth, point.getErrorEllMajorAxis().getMMetresValue());
		stream->writeDouble(coordWidth, coordResWidth, point.getErrorEllMinorAxis().getMMetresValue());
		stream->writeString(coordWidth, "");
	}
	else
	{
		(*stream).width(1);
		(*stream) << "";
		const int vecwidth(24);
		// char vecstr[32]; // format the vector output here and write as a string
		stream->writeString(vecwidth, "");
		stream->writeString(vecwidth, "");
		stream->writeString(vecwidth, "");
		stream->writeString(coordWidth, "");
		stream->writeString(coordWidth, "");
		stream->writeString(coordWidth, "");
	}

	// Delta
	TPositionVector xyzValue = point.getProvisionalValue();

	converter.writeCoordinateParam(
		point.getSpatialStatus(), coordWidth, getCoordPrecision(), TLength::kMillimetres, separator, point.getDXValue(), point.getDYValue(), point.getDZValue(), "");

	return;
}

void TPunchFileWriter::writeXYZHData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZandH(point.getEstimatedValue(), TLength(point.getHEstValue()));
	return;
}

void TPunchFileWriter::writeXYHNData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	std::string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstValue(0), point.getEstValue(1), TLength(point.getHEstValue()));
	converter.writeN(coordWidth, getCoordPrecision(), getN(point));
	(*stream) << separator;
	return;
}

void TPunchFileWriter::writeXYZHNData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);
	converter.writeXYZandH(point.getEstimatedValue(), TLength(point.getHEstValue()));
	converter.writeN(coordWidth, getCoordPrecision(), getN(point));
	(*stream) << stream->getSeparator();
	return;
}

void TPunchFileWriter::writeXYZSigmaData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	std::string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);

	if (point.getFrameTreePosition()->get()->isROOTNode())
	{
		// Coordinate
		converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());

		// Sigma
		converter.writeCoordinateParam(point.getSpatialStatus(), getCoordResWidth(), getCoordPrecision(), TLength::kMillimetres, separator, point.getXEstPrecision(),
			point.getYEstPrecision(), point.getZEstPrecision(), " "); /*sigma*/
	}
	else
	{
		// lambda function to transform the point coordinate in root
		auto point_in_root = [&](LGCAdjustablePoint const &point) {
			// Transformation of the point to the root
			TPositionVector estimatedValue = point.getEstimatedValue();
			TDataTreeIterator root = fProjectData->getTree().begin();

			if (root != point.getFrameTreePosition())
			{
				TLOR2LOR transfo = TLOR2LOR(point.getFrameTreePosition(), fProjectData->getTree().begin(), "toroot");
				transfo.transform(estimatedValue);
			}

			// Set new coordinates to the point expressed in the root
			LGCAdjustablePoint point_int_root = LGCAdjustablePoint(estimatedValue, point.isCoordinateFixed(0), point.isCoordinateFixed(1), point.isCoordinateFixed(2),
				point.getName(), point.getReferenceFrame(), point.getFrameTreePosition());
			point_int_root.eolcomment = point.eolcomment;

			return point_int_root;
		};

		auto pointRoot = point_in_root(point);
		// Coordinate
		converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, pointRoot.getEstimatedValue());

		// transform sigma in root
		TFreeVector sigmaRoot;
		sigmaRoot = point.transformSigmaInRoot(point, fProjectData);

		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(getCoordResWidth());
		stream->setPrecisionFormat(getCoordPrecision());
		(*stream) << (sigmaRoot.getX());
		(*stream) << (separator);
		(*stream) << (sigmaRoot.getY());
		(*stream) << (separator);
		(*stream) << (sigmaRoot.getZ());
		(*stream) << (separator);
	}
	return;
}

void TPunchFileWriter::writeCooData(LGCAdjustablePoint const &point)
{
	TAStreamFormatter *stream = getStream();
	TPointConverter converter(stream, fProjectData->getConfig().referential);
	int nameWidth = getNameWidth();
	int coordWidth = getCoordWidth();
	// int coordResWidth = getCoordResWidth();
	std::string separator = stream->getSeparator();

	(*stream).width(1);
	(*stream) << "";
	converter.writeName(point.getName(), nameWidth);

	if (fData->getConfig().referential != TRefSystemFactory::ERefFrame::kLocalRefFrame)
		converter.writeXYH(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstValue(0), point.getEstValue(1), TLength(point.getHEstValue()));
	else
		converter.writeXYZ(coordWidth, getCoordPrecision(), TLength::kMetres, separator, point.getEstimatedValue());

	// ID
	if (!point.eolcomment.compare(0, 1, "$"))
	{
		// eolcomment = $cumul ID comments
		std::vector<std::string> comments = tokenizefileString(point.eolcomment.substr(1));
		if (comments.size() < 2)
			stream->writeString(coordWidth, "-1");
		else
			stream->writeString(coordWidth, comments.at(1));
	}
	else
		stream->writeString(coordWidth, "-1");

	// Delta
	TPositionVector xyzValue = point.getProvisionalValue();

	converter.writeCoordinateParam(
		point.getSpatialStatus(), coordWidth, getCoordPrecision(), TLength::kMillimetres, separator, point.getDXValue(), point.getDYValue(), point.getDZValue(), "0.0");

	// DCUM
	if (!point.eolcomment.compare(0, 1, "$"))
	{
		std::vector<std::string> comments = tokenizefileString(point.eolcomment.substr(1));
		if (comments.size() < 2)
			stream->writeString(coordWidth, "-1");
		else
			stream->writeDouble(coordWidth, 3, std::stod(comments.at(0)));
	}
	else
		stream->writeString(coordWidth, "-1");

	// OPT
	std::string status;
	if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kCala)
		status = "CALA";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxyz)
		status = "POIN";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxy)
		status = "VXY";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVxz)
		status = "VXZ";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVyz)
		status = "VYZ";
	else if (point.getSpatialStatus() == TSpatialStatus::ESpatialStatus::kVz)
		status = "VZ";

	stream->writeString(nameWidth, status);
	return;
}

TReal TPunchFileWriter::getN(LGCAdjustablePoint const &point)
{
	TRefSystemFactory::EGeoid fGeoidModel;
	if (fData->getConfig().referential == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
		fGeoidModel = TRefSystemFactory::EGeoid::kCGSphere;
	else if (fData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG2000Machine;
	else if (fData->getConfig().referential == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
		fGeoidModel = TRefSystemFactory::EGeoid::kCG1985Machine;
	else
		fGeoidModel = TRefSystemFactory::EGeoid::kNoGeoid;

	// Toujours en CCS pendant le calcul TAReferenceFrame* pointRefFrame = getStreamFormatter()->getReferenceFrame();
	TAReferenceFrame *ccs = TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS);
	TAGeoidModel *geoid = TRefSystemFactory::getRefSystemFactory()->getGeoid(fGeoidModel);

	TSpatialPosition pos(ccs);
	pos.setCoordinates(point.getEstimatedValue());
	return geoid->getN(pos).getMetresValue();
}

void TPunchFileWriter::writeFrameSectionTitle()
{
	TAStreamFormatter *stream = getStream();

	(*stream) << "%" << endl;
	(*stream) << "%" << endl;
	(*stream).width(1);
	(*stream) << "%";
	(*stream) << "FRAME TRANSFORMATION PARAMETERS" << endl;
	(*stream) << "%" << endl;
}

void TPunchFileWriter::writeFrameSectionHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int paramWidth = getObsWidth() + 8;
	int coefWidth = 6;
	/////////////////////////////////////////////////////////////////////////////////////////////
	// Header line 1
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "FRAME_NAME");
	(*stream).writeStringLeft(nameWidth, "FRAME_ID");
	(*stream).writeString(paramWidth, "TX");
	(*stream).writeString(paramWidth, "TY");
	(*stream).writeString(paramWidth, "TZ");
	(*stream).writeString(paramWidth, "RX");
	(*stream).writeString(paramWidth, "RY");
	(*stream).writeString(paramWidth, "RZ");
	(*stream).writeString(paramWidth, "SCL");
	(*stream).writeString(coefWidth, "(TX)");
	(*stream).writeString(coefWidth, "(TY)");
	(*stream).writeString(coefWidth, "(TZ)");
	(*stream).writeString(coefWidth, "(RX)");
	(*stream).writeString(coefWidth, "(RY)");
	(*stream).writeString(coefWidth, "(RZ)");
	(*stream).writeString(coefWidth, "(SCL)");
	(*stream) << endl;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Header line 2
	(*stream) << "%";
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(paramWidth, "(M)");
	(*stream).writeString(paramWidth, "(M)");
	(*stream).writeString(paramWidth, "(M)");
	(*stream).writeString(paramWidth, "(GON)");
	(*stream).writeString(paramWidth, "(GON)");
	(*stream).writeString(paramWidth, "(GON)");
	(*stream) << endl;
	(*stream) << "%" << endl;
	return;
}

void TPunchFileWriter::writeFrameSectionData()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int paramWidth = getObsWidth() + 8;
	int coefWidth = 6;
	int anglePrecision = getAnglePrecision() + 5;
	int lengthPrecision = getLengthPrecision() + 5;

	// Tteration through the tree nodes
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
	{
		if (itTree->get()->frame.getName() != "ROOT")
		{
			std::string nameID;
			for (std::vector<int>::const_iterator it = itTree->get()->ID.begin(); it != itTree->get()->ID.end(); ++it)
			{
				if (it == itTree->get()->ID.begin())
					nameID += std::to_string(*it);
				else
					nameID += "_" + std::to_string(*it);
			}

			(*stream).width(1);
			(*stream) << "";
			(*stream).writeStringLeft(nameWidth, itTree->get()->frame.getName());
			(*stream).writeStringLeft(nameWidth, nameID);

			// Write the translation values (TX, TY, TZ)
			for (int coef = 0; coef < 3; coef++)
			{
				if (!itTree->get()->frame.isTranslationFixed(coef))
				{
					(*stream).writeDouble(paramWidth, lengthPrecision, itTree->get()->frame.getEstTranslation(coef));
				}
				else
				{
					(*stream).writeDouble(paramWidth, lengthPrecision, itTree->get()->frame.getProvTranslation(coef));
				}
			}

			// Write the rotation values (RX, RY, RZ)
			for (int coef = 0; coef < 3; coef++)
			{
				if (!itTree->get()->frame.isRotationFixed(coef))
				{
					(*stream).writeDouble(paramWidth, anglePrecision, itTree->get()->frame.getEstRotation(coef).getGonsValue());
				}
				else
				{
					(*stream).writeDouble(paramWidth, anglePrecision, itTree->get()->frame.getProvRotation(coef).getGonsValue());
				}
			}

			// Write the scale value (SCL)
			if (!itTree->get()->frame.isScaleFixed())
			{
				(*stream).writeDouble(paramWidth, anglePrecision, itTree->get()->frame.getEstScale());
			}
			else
			{
				(*stream).writeDouble(paramWidth, anglePrecision, itTree->get()->frame.getProvScale());
			}

			// Write the translation values status (Fixed = 1, Estimated = 0)
			for (int coef = 0; coef < 3; coef++)
			{
				(*stream).writeString(coefWidth, std::to_string(itTree->get()->frame.isTranslationFixed(coef)));
			}
			// Write the rotation values status (Fixed = 1, Estimated = 0)
			for (int coef = 0; coef < 3; coef++)
			{
				(*stream).writeString(coefWidth, std::to_string(itTree->get()->frame.isRotationFixed(coef)));
			}
			// Write the scale values status (Fixed = 1, Estimated = 0)
			(*stream).writeString(coefWidth, std::to_string(itTree->get()->frame.isScaleFixed()));

			(*stream) << endl;
		}
	}

	return;
}
