#include <Eigen/Dense>

#include <Logger.hpp>
#include <TLSEvaluator.h>

#include "TAdjustableHelmertTransformation.h"
#include "TLSConsistencyCheck.h"
#include "TLSInputMatricesFiller.h"
#include "TLSUniversalMtdComputer.h"
//#include <Behavior.h>
//#include <TLGCData.h>
//#include <TLSAlgorithm.h>
#include <TReader.h>
//
//#include "FileUtils.h"
#include "TDataAnalyzer.h"
//#include "TLGCCalculation.h"
//#include "TLSResultsMatrices.h"
//#include "TLSSimulation.h"
//#include "TVAbstractAlgorithm.h"
//#include "TInputFileWriter.h"
//#include "TSimFileWriter.h"
//#include "TStreamFormatterFactory.h"
//#include <iomanip> // put_time
//#include <chrono>
//#include "TLSEvaluator.h"

TLSEvaluator::TLSEvaluator(std::shared_ptr<TLGCData> data) : iMat(new TLSInputMatrices)
{
	fData = data;
	fMatFiller = new TLSInputMatricesFiller(&fData->getTree(), fData->getConfig().referential, *fData);
}

TLSEvaluator::TLSEvaluator(std::stringstream& fileStream) : iMat(new TLSInputMatrices)
{
	Behavior successCalculation;
	std::shared_ptr<TLGCData> project(new TLGCData);
	//fProject = project;
	TReader r(project);

	project->getFileLogger().setOutputfileLocation("C:/Temp/pyLog.txt");
	// is this necessary?
	project->getFileLogger().writeReportHeader("Dummy output file python lib");

	// why 2 streams?
	//std::stringstream inputFileStream = fileStream;
	//std::ifstream inputFileStream(lgcFilePath, std::ifstream::in);
	//std::ifstream cp_inputFileStream(lgcFilePath, std::ifstream::in);

	if (r.isLgc2File(fileStream))
	{
		bool readSuccess = r.read(fileStream);
		if (!readSuccess)
		{
			throw std::runtime_error("Errors found in the input file.");
		}
	}
	else
	{
		std::cout << "Only LGC2 input files are supported." << std::endl;
	}
	TDataAnalyzer analyzer(*project.get());
	bool configSuccess = analyzer.dataConsistent();
	if (!configSuccess)
		throw std::runtime_error("Error during problem initialization");

	fData = project;
	fMatFiller = new TLSInputMatricesFiller(&fData->getTree(), fData->getConfig().referential, *fData);
}

TLSEvaluator::~TLSEvaluator()
{
}

Eigen::VectorXd TLSEvaluator::getEstParams()
{
	Eigen::VectorXd result(fData->fUEOIndices.UIndex);
	result.setZero();
	getPointParams(result);
	getAngleParams(result);
	getPlaneParams(result);
	getLengthParams(result);
	getTransformationParams(result);
	getLineParams(result);

	return result;
}

void TLSEvaluator::setParameters(const Eigen::VectorXd &para)
{
	isUptoDate = false;
	Eigen::VectorXd globalPar;
		// check if dimension is correct
		if (para.size() != (fData->fUEOIndices.UIndex))
		{
			std::string message;
			message = "variable has wrong dimension, expected: " + std::to_string(fData->fUEOIndices.UIndex) + ", actual: " + std::to_string(para.size());
			throw std::runtime_error(message);
		}
		globalPar = para;
	// go through all adjustable objects, fill in the appropriate values from the parameter vector
	setPointParams(globalPar);
	setAngleParams(globalPar);
	setPlaneParams(globalPar);
	setLengthParams(globalPar);
	setTransformationParams(globalPar);
	setLineParams(globalPar);
}




void TLSEvaluator::setPointParams(Eigen::VectorXd para)
{
	for (auto &point : fData->getPoints())
	{
		if (point.hasVariable())
		{
			for (int unknIdx = point.getFirstUidx(); unknIdx <= point.getLastUidx(); ++unknIdx)
			{
				point.setValue(unknIdx, para(unknIdx));
			}
		}
	}
}

void TLSEvaluator::setAngleParams(Eigen::VectorXd para)
{
	for (auto &angle : fData->getAngles())
	{
		if (!angle.isFixed())
		{
			MatrixIndex unknIdx = angle.getFirstUidx(); // first=last only one unknown fo angle class
			//
			angle.setValue(unknIdx, para(unknIdx));
		}
	}
}

void TLSEvaluator::setPlaneParams(Eigen::VectorXd para)
{
	for (auto &plane : fData->getPlanes())
	{
		if (plane.hasVariable())
		{
			for (int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); unknIdx++)
				plane.setValue(unknIdx, para(unknIdx));
		}
	}
}

void TLSEvaluator::setLineParams(Eigen::VectorXd para)
{
	for (auto &line : fData->getLines())
	{
		if (!line.isFixed())
		{
			for (int unknIdx = line.getFirstUidx(); unknIdx <= line.getLastUidx(); unknIdx++)
			{
				line.setValue(unknIdx, para(unknIdx));
			}
		}
	}
}

void TLSEvaluator::setLengthParams(Eigen::VectorXd para)
{
	for (auto &length : fData->getLength())
	{
		if (!length.isFixed())
		{
			MatrixIndex unknIdx = length.getFirstUidx(); // first=last only one unknown fo angle class
			length.setValue(unknIdx, para(unknIdx));
		}
	}
}

void TLSEvaluator::setTransformationParams(Eigen::VectorXd para)
{
	for (auto it(fData->getTree().begin()); it != fData->getTree().end(); ++it)
	{
		auto &trafo(it.node->data.get()->frame);

		if (trafo.hasVariable())
		{
			for (int unknIdx = trafo.getFirstUidx(); unknIdx <= trafo.getLastUidx(); unknIdx++)
			{
				trafo.setValue(unknIdx, para(unknIdx));
			}
		}
	}
}

// getters
void TLSEvaluator::getPointParams(Eigen::VectorXd &para)
{
	for (auto &point : fData->getPoints())
	{
		if (point.hasVariable())
		{
			para.middleRows(point.getFirstUidx(), point.getNumUnkn()) = point.getEstVector()(point.getRelativeUnknIndices());
		}
	}
}

void TLSEvaluator::getAngleParams(Eigen::VectorXd &para)
{
	for (auto &angle : fData->getAngles())
	{
		if (!angle.isFixed())
		{
			para(angle.getFirstUidx()) = angle.getEstVector()(0);
		}
	}
}

void TLSEvaluator::getPlaneParams(Eigen::VectorXd &para)
{
	for (auto &plane : fData->getPlanes())
	{
		if (plane.hasVariable())
		{
			para.middleRows(plane.getFirstUidx(), plane.getNumUnkn()) = plane.getEstVector()(plane.getRelativeUnknIndices());
		}
	}
}

void TLSEvaluator::getLineParams(Eigen::VectorXd &para)
{
	for (auto &line : fData->getLines())
	{
		if (!line.isFixed())
		{
			para.middleRows(line.getFirstUidx(), line.getNumUnkn()) = line.getEstVector()(line.getRelativeUnknIndices());
		}
	}
}

void TLSEvaluator::getLengthParams(Eigen::VectorXd &para)
{
	for (auto &length : fData->getLength())
	{
		if (!length.isFixed())
		{
			para(length.getFirstUidx()) = length.getEstimatedValue();
		}
	}
}

void TLSEvaluator::getTransformationParams(Eigen::VectorXd &para)
{
	for (auto it(fData->getTree().begin()); it != fData->getTree().end(); ++it)
	{
		auto &trafo(it.node->data.get()->frame);

		if (trafo.hasVariable())
		{
			int nFreeParams = trafo.getNumUnkn();
			Eigen::VectorXd trafoParams(nFreeParams);
			trafoParams = trafo.getEstVector()(trafo.getRelativeUnknIndices());
			para.middleRows(trafo.getFirstUidx(), trafo.getNumUnkn()) = trafoParams;
		}
	}
}