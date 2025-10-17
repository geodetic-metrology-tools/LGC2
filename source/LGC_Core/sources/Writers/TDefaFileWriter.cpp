// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

////////////////////////////////////////////////////////////////////
// TSimFileWriter.cpp : implementation class
// Write an LGC "input" file with simulated values for observations
//
////////////////////////////////////////////////////////////////////////////////////

#include "TSeparatedFormatTStream.h"
#include <TLGCData.h>
#include "TDefaFileWriter.h"
#include "TLSResultsMatrices.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TDefaFileWriter::TDefaFileWriter() : TAFileWriter()
{//default constructor
}


TDefaFileWriter::TDefaFileWriter(TAStreamFormatter* stream, const TLGCData* project) : TAFileWriter(stream, project)
{//constructor
}


TDefaFileWriter::~TDefaFileWriter()
{//destructor
}

void TDefaFileWriter::writeFile(const std::string)
{

}

void TDefaFileWriter::writeFile(const TLGCData &project, const TLSResultsMatrices& rm)
{
	writeTitle(project);
	writePoints(project);

	writeUpperTriangularCovarianceMatrix(project, rm);
}

void TDefaFileWriter::writeTitle(const TLGCData &project)
{
	fStream->writeStringLeft(8, "DEFA: ");
	fStream->writeInteger(4, project.fUEOIndices.OIndex - project.fUEOIndices.UIndex - (project.getMeasurementDimension(TMeasurementsGlobal::EMeasurementType::kORIE) == 1 ? 1 : 0));
	fStream->writeDouble(15, 8, project.getS0APosteriori() * project.getS0APosteriori());
	int pointsCount = project.getPointsDimension(TSpatialStatus::kVy) +
		project.getPointsDimension(TSpatialStatus::kVx) +
		project.getPointsDimension(TSpatialStatus::kVz) +
		project.getPointsDimension(TSpatialStatus::kVxy) +
		project.getPointsDimension(TSpatialStatus::kVxz) +
		project.getPointsDimension(TSpatialStatus::kVyz) +
		project.getPointsDimension(TSpatialStatus::kVxyz);
	fStream->writeInteger(4, pointsCount + project.getPointsDimension(TSpatialStatus::kCala));
	fStream->writeInteger(4, project.getPointsDimension(TSpatialStatus::kCala));
	int unknownCoordinatesCount = project.getPointsDimension(TSpatialStatus::kVy) +
		project.getPointsDimension(TSpatialStatus::kVx) +
		project.getPointsDimension(TSpatialStatus::kVz) +
		2 * project.getPointsDimension(TSpatialStatus::kVxy) +
		2 * project.getPointsDimension(TSpatialStatus::kVxz) +
		2 * project.getPointsDimension(TSpatialStatus::kVyz) +
		3 * project.getPointsDimension(TSpatialStatus::kVxyz);
	fStream->writeInteger(4, unknownCoordinatesCount);
	*fStream << endl;
}

void TDefaFileWriter::writePoints(const TLGCData &project)
{
	*fStream << "(A8,5X,F15.8,3X,F15.8,3X,F15.8,I4)\n";
	if (project.getPointsDimension(TSpatialStatus::kVx) != 0) writeXAnalysis(project);
	else if (project.getPointsDimension(TSpatialStatus::kVy) != 0) writeYAnalysis(project);
	else if (project.getPointsDimension(TSpatialStatus::kVz) != 0) writeZAnalysis(project);
	else if (project.getPointsDimension(TSpatialStatus::kVxy) != 0) writeXYAnalysis(project);
	else if (project.getPointsDimension(TSpatialStatus::kVxz) != 0) writeXZAnalysis(project);
	else if (project.getPointsDimension(TSpatialStatus::kVyz) != 0) writeYZAnalysis(project);
	else if (project.getPointsDimension(TSpatialStatus::kVxyz) != 0) write3DAnalysis(project);
	writeCALA(project);
}

void TDefaFileWriter::write3DAnalysis(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (!begin->isCoordinateFixed(0) || !begin->isCoordinateFixed(1) || !begin->isCoordinateFixed(2))
		{
			write3DPoint(begin->getName(), 
							begin->getEstValue(0).getMetresValue(), 
							begin->getEstValue(1).getMetresValue(),
							begin->getEstValue(2).getMetresValue());
		}

		begin++;
	}
}


void TDefaFileWriter::writeXYAnalysis(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (!begin->isCoordinateFixed(0) || !begin->isCoordinateFixed(1) || !begin->isCoordinateFixed(2))
		{
			// write the 2D coordinates for analysis
			// X & Y coordinates with a dummy third coordinate
			write3DPoint(begin->getName(), 
							begin->getEstValue(0).getMetresValue(), 
							begin->getEstValue(1).getMetresValue(), 
							LITERAL(-9999.99999999));
		}

		begin++;
	}
}

void TDefaFileWriter::writeXZAnalysis(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (!begin->isCoordinateFixed(0) || !begin->isCoordinateFixed(1) || !begin->isCoordinateFixed(2))
		{
			// write the 2D coordinates for analysis
			// X & Z coordinates with a dummy third coordinate
			write3DPoint(begin->getName(), 
							begin->getEstValue(0).getMetresValue(), 
							begin->getEstValue(2).getMetresValue(), 
							LITERAL(-9999.99999999));
		}

		begin++;
	}
}

void TDefaFileWriter::writeYZAnalysis(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (!begin->isCoordinateFixed(0) || !begin->isCoordinateFixed(1) || !begin->isCoordinateFixed(2))
		{
			// write the 2D coordinates for analysis
			// Y & Z coordinates with a dummy third coordinate
			write3DPoint(begin->getName(), 
							begin->getEstValue(1).getMetresValue(), 
							begin->getEstValue(2).getMetresValue(), 
							LITERAL(-9999.99999999));
		}

		begin++;
	}
}

void TDefaFileWriter::writeXAnalysis(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (!begin->isCoordinateFixed(0) || !begin->isCoordinateFixed(1) || !begin->isCoordinateFixed(2))
		{
			// write the 1D coordinate for analysis
			// two dummy coordinates plus the X-coordinate
			write3DPoint(begin->getName(), 
							LITERAL(-9999.99999999), 
							LITERAL(-9999.99999999), 
							begin->getEstValue(0).getMetresValue());
		}

		begin++;
	}
}

void TDefaFileWriter::writeYAnalysis(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (!begin->isCoordinateFixed(0) || !begin->isCoordinateFixed(1) || !begin->isCoordinateFixed(2))
		{
			// write the 1D coordinate for analysis
			// two dummy coordinates plus the Y-coordinate
			write3DPoint(begin->getName(), 
							LITERAL(-9999.99999999), 
							LITERAL(-9999.99999999), 
							begin->getEstValue(1).getMetresValue());
		}

		begin++;
	}
}

void TDefaFileWriter::writeZAnalysis(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (!begin->isCoordinateFixed(0) || !begin->isCoordinateFixed(1) || !begin->isCoordinateFixed(2))
		{
			// write the 1D coordinate for analysis
			// two dummy coordinates plus the Z-coordinate
			write3DPoint(begin->getName(), 
							LITERAL(-9999.99999999), 
							LITERAL(-9999.99999999), 
							begin->getEstValue(2).getMetresValue());
		}

		begin++;
	}
}

void TDefaFileWriter::writeCALA(const TLGCData &project)
{
	auto begin = project.getPoints().begin();
	auto end = project.getPoints().end();
		
	while (begin != end)
	{
		if (begin->isCoordinateFixed(0) && begin->isCoordinateFixed(1) && begin->isCoordinateFixed(2))
		{
			if (project.getPointsDimension(TSpatialStatus::kVx) != 0) 
				write3DPoint(begin->getName(), 
								LITERAL(-9999.99999999), 
								LITERAL(-9999.99999999), 
								begin->getEstValue(0).getMetresValue());
			else if (project.getPointsDimension(TSpatialStatus::kVy) != 0) 
				write3DPoint(begin->getName(), 
								LITERAL(-9999.99999999), 
								LITERAL(-9999.99999999), 
								begin->getEstValue(1).getMetresValue());
			else if (project.getPointsDimension(TSpatialStatus::kVz) != 0) 
				write3DPoint(begin->getName(), 
								LITERAL(-9999.99999999), 
								LITERAL(-9999.99999999), 
								begin->getEstValue(2).getMetresValue());
			else if (project.getPointsDimension(TSpatialStatus::kVxy) != 0) 
				write3DPoint(begin->getName(), 
								begin->getEstValue(0).getMetresValue(), 
								begin->getEstValue(1).getMetresValue(), 
								LITERAL(-9999.99999999));
			else if (project.getPointsDimension(TSpatialStatus::kVxz) != 0) 
				write3DPoint(begin->getName(), 
								begin->getEstValue(0).getMetresValue(), 
								begin->getEstValue(2).getMetresValue(), 
								LITERAL(-9999.99999999));
			else if (project.getPointsDimension(TSpatialStatus::kVyz) != 0) 
				write3DPoint(begin->getName(), 
								begin->getEstValue(1).getMetresValue(), 
								begin->getEstValue(2).getMetresValue(), 
								LITERAL(-9999.99999999));
			else if (project.getPointsDimension(TSpatialStatus::kVxyz) != 0) 
				write3DPoint(begin->getName(), 
								begin->getEstValue(0).getMetresValue(), 
								begin->getEstValue(1).getMetresValue(), 
								begin->getEstValue(2).getMetresValue());
		}

		begin++;
	}
}

void TDefaFileWriter::write3DPoint(std::string name, TReal X, TReal Y, TReal Z)
{
	fStream->writeStringLeft(8, name);
	*fStream << "     ";
	fStream->writeDouble(15, 8, X);
	*fStream << "   ";
	fStream->writeDouble(15, 8, Y);
	*fStream << "   ";
	fStream->writeDouble(15, 8, Z);
	*fStream << "   1" << endl;
}

void TDefaFileWriter::writeUpperTriangularCovarianceMatrix(const TLGCData& project, const TLSResultsMatrices& rm)
{
	std::list<LGCAdjustablePoint>::const_iterator begin;
	std::list<LGCAdjustablePoint>::const_iterator end = project.getPoints().end();

	for (int i = 0; i < rm.getUnkCovarMtrxByConst()->rows(); i++)
	{
		begin = project.getPoints().begin();

		while (begin != end)
		{
			if (!begin->isCoordinateFixed(0) && begin->getCoordinateUnknIndex(0) >= i)
			{
				fStream->writeDouble(20, 16, rm.getUnkCovarMtrxElmt(i, begin->getCoordinateUnknIndex(0)));
				*fStream << endl;
			}
			if (!begin->isCoordinateFixed(1) && begin->getCoordinateUnknIndex(1) >= i)
			{
				fStream->writeDouble(20, 16, rm.getUnkCovarMtrxElmt(i, begin->getCoordinateUnknIndex(1)));
				*fStream << endl;
			}
			if (!begin->isCoordinateFixed(2) && begin->getCoordinateUnknIndex(2) >= i)
			{
				fStream->writeDouble(20, 16, rm.getUnkCovarMtrxElmt(i, begin->getCoordinateUnknIndex(2)));
				*fStream << endl;
			}

			begin++;
		}
	}
}
