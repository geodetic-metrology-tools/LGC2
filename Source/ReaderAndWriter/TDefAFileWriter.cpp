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

void TDefAFileWriter::writeFile(const string error)
{
	*fStream << error;
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
	int unknownCoordinatesCount = project.getDataSet()->getPointsDimension(TSpatialStatus::kVy) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVx) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVz) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVxy) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVxz) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVyz) +
		project.getDataSet()->getPointsDimension(TSpatialStatus::kVxyz);
	fStream->writeInteger(4, unknownCoordinatesCount + project.getDataSet()->getPointsDimension(TSpatialStatus::kCala));
	fStream->writeInteger(4, project.getDataSet()->getPointsDimension(TSpatialStatus::kCala));
	fStream->writeInteger(4, unknownCoordinatesCount * 3);
	*fStream << endl;
}

void TDefAFileWriter::writePoints(TLGCProject &project)
{
	if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVx) != 0) write3DAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVy) != 0) write3DAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVz) != 0) write3DAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxy) != 0) write2DAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxz) != 0) write2DAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVyz) != 0) write2DAnalysis(project);
	else if (project.getDataSet()->getPointsDimension(TSpatialStatus::kVxyz) != 0) write3DAnalysis(project);
	writeCALA(project);
}

void TDefAFileWriter::write3DAnalysis(TLGCProject &project)
{
	*fStream << "(A8,5X,F15.8,3X,F15.8,3X,F15.8,I4)\n";
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			write3DPoint(*begin);
		}

		begin++;
	}
}

void TDefAFileWriter::write3DPoint(const TLSCalcPosVectorParam& param)
{
	fStream->writeStringLeft(8, param.getName());
	*fStream << "     ";
	fStream->writeDouble(15, 8, param.getXEstValue().getMetresValue());
	*fStream << "   ";
	fStream->writeDouble(15, 8, param.getYEstValue().getMetresValue());
	*fStream << "   ";
	fStream->writeDouble(15, 8, param.getZEstValue().getMetresValue());
	*fStream << "   1" << endl;
}

void TDefAFileWriter::write2DAnalysis(TLGCProject &project)
{
	*fStream << "(A8,5X,F15.8,3X,F15.8,I4)\n";
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() != TALSCalcParameter::kFixed || begin->getYStatus() != TALSCalcParameter::kFixed || begin->getZStatus() != TALSCalcParameter::kFixed)
		{
			write2DPoint(*begin);
		}

		begin++;
	}
}

void TDefAFileWriter::write2DPoint(const TLSCalcPosVectorParam& param)
{
	fStream->writeStringLeft(8, param.getName());
	*fStream << "     ";
	if (param.getXStatus() == TSpatialStatus::kVxy || param.getXStatus() == TSpatialStatus::kVxz)
	{
		fStream->writeDouble(15, 8, param.getXEstValue().getMetresValue());
		*fStream << "   ";
	}
	if (param.getYStatus() == TSpatialStatus::kVxy || param.getYStatus() == TSpatialStatus::kVyz)
	{
		fStream->writeDouble(15, 8, param.getYEstValue().getMetresValue());
		*fStream << "   ";
	}
	if (param.getZStatus() == TSpatialStatus::kVyz || param.getZStatus() == TSpatialStatus::kVxz)
	{
		fStream->writeDouble(15, 8, param.getZEstValue().getMetresValue());
		*fStream << "   ";
	}
	*fStream << "1" << endl;
}

void TDefAFileWriter::writeCALA(TLGCProject &project)
{
	LSPosVecConstIter begin = project.getLSCalcDataSet()->beginPV();
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
		
	while (begin != end)
	{
		if (begin->getXStatus() == TALSCalcParameter::kFixed && begin->getYStatus() == TALSCalcParameter::kFixed && begin->getZStatus() == TALSCalcParameter::kFixed)
		{
			write3DPoint(*begin);
		}

		begin++;
	}
}

void TDefAFileWriter::writeUpperTriangularCovarianceMatrix(TLGCProject& project)
{
	LSPosVecConstIter begin;
	LSPosVecConstIter end = project.getLSCalcDataSet()->endPV();
	TSparseMatrix& cxx(*(project.getLSCalcDataSet()->getUnknownsCovarianceMtrx()));

	for (int i = 0; i < project.getLSCalcDataSet()->getUnknownsCovarianceMtrx()->rows(); i++)
	{
		begin = project.getLSCalcDataSet()->beginPV();

		while (begin != end)
		{
			if (begin->getXStatus() == TALSCalcParameter::kVariable && begin->getXIndex() >= i)
			{
				fStream->writeDouble(20, 16, cxx.coeff(i, begin->getXIndex()));
				*fStream << endl;
			}
			if (begin->getYStatus() == TALSCalcParameter::kVariable && begin->getYIndex() >= i)
			{
				fStream->writeDouble(20, 16, cxx.coeff(i, begin->getYIndex()));
				*fStream << endl;
			}
			if (begin->getZStatus() == TALSCalcParameter::kVariable && begin->getZIndex() >= i)
			{
				fStream->writeDouble(20, 16, cxx.coeff(i, begin->getZIndex()));
				*fStream << endl;
			}

			begin++;
		}
	}
}
