// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

////////////////////////////////////////////////////////////////////
// TSimFileWriter.cpp : implementation class
// Write an LGC "input" file with simulated values for observations
//
////////////////////////////////////////////////////////////////////////////////////
#include "TCovarFileWriter.h"

#include <TLGCData.h>

#include "LGCAdjustableObjectCollection.h"
#include "TLSResultsMatrices.h"
#include "TSeparatedFormatTStream.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TCovarFileWriter::TCovarFileWriter() : TAFileWriter()
{ // default constructor
}

TCovarFileWriter::TCovarFileWriter(TAStreamFormatter *stream, const TLGCData *project) : TAFileWriter(stream, project)
{ // constructor
}

TCovarFileWriter::~TCovarFileWriter()
{ // destructor
}

void TCovarFileWriter::writeFile(const std::string)
{
}

void TCovarFileWriter::writeFile(const TLGCData &project)
{
	writeTitle(project);
	const LGCAdjustablePointCollection &AdjPointIter = project.getPoints();

	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
	{
		if (itTree->get()->frame.getName() != "ROOT")
		{
			writeFrames(itTree->get());

			for (const auto &pointIt : AdjPointIter)
				if (pointIt.getFrameTreePosition().node->data.get()->ID == itTree->get()->ID)
				{
					// Write points defined in the frame
					writePoints(pointIt);
				}
		}
		else
		{
			for (const auto &pointIt : AdjPointIter)
				if (pointIt.getFrameTreePosition().node->data.get()->ID == itTree->get()->ID)
				{
					// Write points defined in the frame
					writePoints(pointIt);
				}
		}
	}
}

void TCovarFileWriter::writeTitle(const TLGCData &project)
{
	fStream->writeStringLeft(8, "FILE OF COVARIANCE MATRICES: ");
	fStream->writeStringLeft(8, project.getConfig().title);
	*fStream << endl;
	fStream->writeStringLeft(8, "S0 a posteriori = ");
	fStream->writeDouble(15, 8, project.getS0APosteriori());
	*fStream << endl;
}

void TCovarFileWriter::writePoints(const LGCAdjustablePoint &point)
{
	// write point name
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	std::string TABs = stream->getCurrSpace();

	// Writting point: name X Y Z
	fStream->writeStringLeft(nameWidth, point.getName());
	(*stream) << TABs;
	fStream->writeDouble(28, 16, point.getEstimatedValue().getX());
	(*stream) << TABs;
	fStream->writeDouble(28, 16, point.getEstimatedValue().getY());
	(*stream) << TABs;
	fStream->writeDouble(28, 16, point.getEstimatedValue().getZ());
	(*stream) << endl;

	// Write frame upper covariance matrix
	//  sx  cov_xy   cov_xz
	//       sy      cov_yz
	//                 sz
	writePointUpperTriangularCovarianceMatrix(point);
}

void TCovarFileWriter::writeFrames(TTreeEntry *frameIt)
{
	// Write frame definition
	writeFrameDefinition(frameIt->frame);

	// Write frame upper covariance matrix
	writeFrameUpperTriangularCovarianceMatrix(frameIt->frame);
}

void TCovarFileWriter::writeFrameDefinition(TAdjustableHelmertTransformation &frameDef)
{
	// Write frame definition
	// name tx ty tz rx ry rz scale

	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	std::string separator = getSeparator();
	int lengthPrecision = getLengthPrecision();
	int anglePrecision = getAnglePrecision();
	std::string TABs = stream->getCurrSpace();
	// int				obsResWidth = getObsResWidth();
	// int				lengthResidualPrecision = getLengthResidualPrecision();
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);
	stream->setPrecisionFormat(anglePrecision);

	// Writing name
	(*stream) << endl;
	fStream->writeStringLeft(nameWidth, frameDef.getName());

	// Writing translations
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(0));
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(1));
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstTranslation(2));

	// Writing rotations
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(0).getRadiansValue());
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(1).getRadiansValue());
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, anglePrecision, frameDef.getEstRotation(2).getRadiansValue());

	// Writing scale factor
	(*stream) << TABs;
	fStream->writeDouble(obsWidth, lengthPrecision, frameDef.getEstScale());

	(*stream) << endl;
}

void TCovarFileWriter::writeFrameUpperTriangularCovarianceMatrix(TAdjustableHelmertTransformation &frameDef)
{
	// Write frame upper covariance matrix in radian and meter
	//  vartx	cov_txty	cov_txtz	cov_txrx	cov_txry	cov_txrz	cov_txl
	//             varty     cov_tytz	cov_tyrx	cov_tyry	cov_tyrz	cov_tyl
	//                        vartz      cov_tzrx	cov_tzry	cov_tzrz	cov_tzl
	//                                     varrx     cov_rxry    cov_rxrz	cov_rxl
	//												  varry     cov_ryrz	cov_ryl
	//                                                              varrz	cov_rzl
	//                                                                         varl

	TAStreamFormatter *stream = getStream();
	// int				nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	std::string separator = getSeparator();
	int lengthPrecision = getLengthPrecision();
	// int              anglePrecision = getAnglePrecision();
	std::string TABs = stream->getCurrSpace();
	// int				obsResWidth = getObsResWidth();
	// int				lengthResidualPrecision = getLengthResidualPrecision();
	// set length parameters
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);

	if (!frameDef.isFixed())
	{
		TDenseMatrix frameCovar = frameDef.getCovar();
		for (int row = 0; row < 7; row++)
		{
			// write the upper part of the row
			for (int col = 0; col < 7; col++)
			{
				if (col < row)
				{
					fStream->writeString(20, "");
					(*stream) << TABs;
				}
				else
				{
					if (col > row)
					{
						(*stream) << TABs;
					}
					fStream->writeDouble(20, 16, frameCovar(row, col));
				}
			}
			(*stream) << endl;
		}
	}
	(*stream) << endl;
}

void TCovarFileWriter::writePointUpperTriangularCovarianceMatrix(const LGCAdjustablePoint &point)
{
	TAStreamFormatter *stream = getStream();
	// int				nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	std::string separator = getSeparator();
	int lengthPrecision = getLengthPrecision();
	std::string TABs = stream->getCurrSpace();
	// int				obsResWidth = getObsResWidth();
	// int				lengthResidualPrecision = getLengthResidualPrecision();
	// set length parameters
	stream->setWidthFormat(obsWidth);
	stream->setPrecisionFormat(lengthPrecision);

	if (!point.isFixed())
	{
		if (!point.isCoordinateFixed(0))
		{
			// variance
			fStream->writeDouble(20, 16, pow2(point.getXEstPrecision()));

			// covariances
			if (!point.isCoordinateFixed(1))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getXYCovar());
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
			}
			if (!point.isCoordinateFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getXZCovar());
				(*stream) << endl;
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
				(*stream) << endl;
			}
		}
		else
		{
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!point.isCoordinateFixed(1))
		{
			// variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, pow2(point.getYEstPrecision()));

			// covariances
			if (!point.isCoordinateFixed(2))
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, point.getYZCovar());
				(*stream) << endl;
			}
			else
			{
				(*stream) << TABs;
				fStream->writeDouble(20, 16, 0.0);
				(*stream) << endl;
			}
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}

		if (!point.isCoordinateFixed(2))
		{
			// variance
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, pow2(point.getZEstPrecision()));
			(*stream) << endl;
		}
		else
		{
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeString(20, "");
			(*stream) << TABs;
			fStream->writeDouble(20, 16, 0.0);
			(*stream) << endl;
		}
	}
	else
	{
		(*stream) << TABs;
		fStream->writeString(20, "");
		(*stream) << TABs;
		fStream->writeString(20, "");
		(*stream) << TABs;
		fStream->writeString(20, "");
		(*stream) << endl;
	}
}
