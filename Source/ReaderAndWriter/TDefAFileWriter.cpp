////////////////////////////////////////////////////////////////////
// TSimFileWriter.cpp : implementation class
// Write an LGC "input" file with simulated values for observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////

#include "TSeparatedFormatTStream.h"
#include "TLGCDataSet.h"
#include "LSCalcDataSet.h"
#include "TDefAFileWriter.h"
#include "TLGCProject.h"
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TDefAFileWriter::TDefAFileWriter() : TAFileWriter()
{//default constructor
}


TDefAFileWriter::TDefAFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TAFileWriter(stream, project)
{//constructor
}


TDefAFileWriter::~TDefAFileWriter()
{//destructor
}

void TDefAFileWriter::writeFile(TLGCDataSet*, const string)
{

}

void TDefAFileWriter::writeFile(TLGCProject &project)
{
	writeTitle(project);
	writePoints(project);

	writeUpperTriangularCovarianceMatrix(project);
}

void TDefAFileWriter::writeTitle(TLGCProject &project)
{
	fStream->writeStringLeft(8, project.getOutputOptions()->getDeformationAnalysisTitle());
	fStream->writeInteger(4, project.getLSCalcDataSet()->getDimensions().OIndex - project.getLSCalcDataSet()->getDimensions().UIndex - (project.getLSCalcDataSet()->numOrieCnstrObs() == 1 ? 1 : 0));
	fStream->writeDouble(15, 8, project.getLSCalcDataSet()->getS0APosteriori().getValue() * project.getLSCalcDataSet()->getS0APosteriori().getValue());
	int pointsCount = project.getDataSet()->getPointsDimension(TSpatialStatus::kVy) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVx) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVz) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVxy) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVxz) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVyz) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVxyz);
	fStream->writeInteger(4, pointsCount + project.getDataSet()->getPointsDimension(TSpatialStatus::kCala));
	fStream->writeInteger(4, project.getDataSet()->getPointsDimension(TSpatialStatus::kCala));
	int unknownCoordinatesCount = project.getDataSet()->getPointsDimension(TSpatialStatus::kVy) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVx) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVz) +
		2 * project.getDataSet()->getPointsDimension(TSpatialStatus::kVxy) +
		2 * project.getDataSet()->getPointsDimension(TSpatialStatus::kVxz) +
		2 * project.getDataSet()->getPointsDimension(TSpatialStatus::kVyz) +
		3 * project.getDataSet()->getPointsDimension(TSpatialStatus::kVxyz);
	fStream->writeInteger(4, unknownCoordinatesCount);
	*fStream << endl;
}

void TDefAFileWriter::writePoints(TLGCProject &project)
{
	*fStream << "(A8,5X,F15.8,3X,F15.8,3X,F15.8,I4)\n";
	if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVx) != 0) writeXAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVy) != 0) writeYAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVz) != 0) writeZAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxy) != 0) writeXYAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxz) != 0) writeXZAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVyz) != 0) writeYZAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxyz) != 0) write3DAnalysis(project);
	writeCALA(project);
}

void TDefAFileWriter::write3DAnalysis(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			write3DPoint(begin->getName(), 
							begin->getXEstValue().getMetresValue(), 
							begin->getYEstValue().getMetresValue(), 
							begin->getZEstValue().getMetresValue());
		}

		begin++;
	}
}


void TDefAFileWriter::writeXYAnalysis(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			// write the 2D coordinates for analysis
			// X & Y coordinates with a dummy third coordinate
			write3DPoint(begin->getName(), 
							begin->getXEstValue().getMetresValue(), 
							begin->getYEstValue().getMetresValue(), 
							LITERAL(-9999.99999999));
		}

		begin++;
	}
}

void TDefAFileWriter::writeXZAnalysis(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			// write the 2D coordinates for analysis
			// X & Z coordinates with a dummy third coordinate
			write3DPoint(begin->getName(), 
							begin->getXEstValue().getMetresValue(), 
							begin->getZEstValue().getMetresValue(), 
							LITERAL(-9999.99999999));
		}

		begin++;
	}
}

void TDefAFileWriter::writeYZAnalysis(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			// write the 2D coordinates for analysis
			// Y & Z coordinates with a dummy third coordinate
			write3DPoint(begin->getName(), 
							begin->getYEstValue().getMetresValue(), 
							begin->getZEstValue().getMetresValue(), 
							LITERAL(-9999.99999999));
		}

		begin++;
	}
}

void TDefAFileWriter::writeXAnalysis(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			// write the 1D coordinate for analysis
			// two dummy coordinates plus the X-coordinate
			write3DPoint(begin->getName(), 
							LITERAL(-9999.99999999), 
							LITERAL(-9999.99999999), 
							begin->getXEstValue().getMetresValue());
		}

		begin++;
	}
}

void TDefAFileWriter::writeYAnalysis(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			// write the 1D coordinate for analysis
			// two dummy coordinates plus the Y-coordinate
			write3DPoint(begin->getName(), 
							LITERAL(-9999.99999999), 
							LITERAL(-9999.99999999), 
							begin->getYEstValue().getMetresValue());
		}

		begin++;
	}
}

void TDefAFileWriter::writeZAnalysis(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			// write the 1D coordinate for analysis
			// two dummy coordinates plus the Z-coordinate
			write3DPoint(begin->getName(), 
							LITERAL(-9999.99999999), 
							LITERAL(-9999.99999999), 
							begin->getZEstValue().getMetresValue());
		}

		begin++;
	}
}

void TDefAFileWriter::writeCALA(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() == TALSCalcParameter::kFixed && begin->getYStatus() == TALSCalcParameter::kFixed && begin->getZStatus() == TALSCalcParameter::kFixed)
		{
			if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVx) != 0) 
				write3DPoint(begin->getName(), 
								LITERAL(-9999.99999999), 
								LITERAL(-9999.99999999), 
								begin->getXEstValue().getMetresValue());
			else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVy) != 0) 
				write3DPoint(begin->getName(), 
								LITERAL(-9999.99999999), 
								LITERAL(-9999.99999999), 
								begin->getYEstValue().getMetresValue());
			else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVz) != 0) 
				write3DPoint(begin->getName(), 
								LITERAL(-9999.99999999), 
								LITERAL(-9999.99999999), 
								begin->getZEstValue().getMetresValue());
			else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxy) != 0) 
				write3DPoint(begin->getName(), 
								begin->getXEstValue().getMetresValue(), 
								begin->getYEstValue().getMetresValue(), 
								LITERAL(-9999.99999999));
			else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxz) != 0) 
				write3DPoint(begin->getName(), 
								begin->getXEstValue().getMetresValue(), 
								begin->getZEstValue().getMetresValue(), 
								LITERAL(-9999.99999999));
			else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVyz) != 0) 
				write3DPoint(begin->getName(), 
								begin->getYEstValue().getMetresValue(), 
								begin->getZEstValue().getMetresValue(), 
								LITERAL(-9999.99999999));
			else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxyz) != 0) 
				write3DPoint(begin->getName(), 
								begin->getXEstValue().getMetresValue(), 
								begin->getYEstValue().getMetresValue(), 
								begin->getZEstValue().getMetresValue());
		}

		begin++;
	}
}

void TDefAFileWriter::write3DPoint(string name, TReal X, TReal Y, TReal Z)
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

void TDefAFileWriter::writeUpperTriangularCovarianceMatrix(TLGCProject& project)
{
	LSPosVecConstIter begin;
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();

	for (int i = 0; i < project.getLSCalcDataSet()->getUnknownsCovarianceMtrx()->rows(); i++)
	{
		begin = project.getLSCalcDataSet()->beginPV();

		while (begin != end)
		{
			if (begin->getXStatus() == TALSCalcParameter::kVariable && begin->getXIndex() >= i)
			{
				fStream->writeDouble(20, 16, (*project.getLSCalcDataSet()->getUnknownsCovarianceMtrx()).coeff(i, begin->getXIndex()));
				*fStream << endl;
			}
			if (begin->getYStatus() == TALSCalcParameter::kVariable && begin->getYIndex() >= i)
			{
				fStream->writeDouble(20, 16, (*project.getLSCalcDataSet()->getUnknownsCovarianceMtrx()).coeff(i, begin->getYIndex()));
				*fStream << endl;
			}
			if (begin->getZStatus() == TALSCalcParameter::kVariable && begin->getZIndex() >= i)
			{
				fStream->writeDouble(20, 16, (*project.getLSCalcDataSet()->getUnknownsCovarianceMtrx()).coeff(i, begin->getZIndex()));
				*fStream << endl;
			}

			begin++;
		}
	}
}
