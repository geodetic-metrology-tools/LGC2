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
	TReader r(project);
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
		if (!r.readLgc1File(fileStream))
		{
			throw std::runtime_error("Errors found in the input file.");
		}
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

bool TLSEvaluator::testParameterSetAndGet()
{
	// parameters
	// set, get and compare
	Eigen::VectorXd testPar = Eigen::VectorXd::LinSpaced(fData->fUEOIndices.UIndex, 0.5, 1.5);
	setParameters(testPar);
	Eigen::VectorXd recoveredPar = getEstParams();
	double diffParameters = (testPar - recoveredPar).norm();
	// // observations
	// // set, get and compare
	// Eigen::VectorXd testObs = Eigen::VectorXd::LinSpaced(fData->fUEOIndices.OIndex, 0.5, 1.5);
	// setObservations(testObs);
	// Eigen::VectorXd recoveredObs = getObservations();
	// double diffObservations = (testObs - recoveredObs).norm();

	bool success = isZero(diffParameters);
	//&&isZero(diffObservations);
	return success;
}

bool TLSEvaluator::testEvaluate()
{
	// set a parameter
	Eigen::VectorXd testPar = Eigen::VectorXd::LinSpaced(fData->fUEOIndices.UIndex, 0.5, 1.5);
	setParameters(testPar);
	// evaluate
	//evaluate();
	// recover some data
	Eigen::VectorXd misc = getMisclosure();
	TSparseMatrix A = getAMatrix();
	TSparseMatrix B = getBMatrix();
	TSparseMatrix A2 = getA2Matrix();
	TSparseMatrix P = getPMatrix();

	std::cout << "misclosure = " << std::endl;
	std::cout << misc << std::endl;
	std::cout << "A = " << std::endl;
	std::cout << A.toDense() << std::endl;
	std::cout << "B = " << std::endl;
	std::cout << B.toDense() << std::endl;
	std::cout << "A2 = " << std::endl;
	std::cout << A2.toDense() << std::endl;
	std::cout << "P = " << std::endl;
	std::cout << P.toDense() << std::endl;


	return false;
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

bool TLSEvaluator::evaluate()
{
	bool success = false;
	if (isUptoDate)
	{
		// nothing to compute, results are already there
		success = true;
	}
	else
	{
		// trigger the evaluation
		success = fMatFiller->fillMatrices(fData.get(), iMat);
		if (success)
			// results are ready
			isUptoDate = true;
	}
	return success;
}

Eigen::VectorXd TLSEvaluator::getMisclosure() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
	return iMat->getMisclosureVctr();
}

TSparseMatrix TLSEvaluator::getAMatrix() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
	return *iMat->getFirstDgnMtrx();
}

TSparseMatrix TLSEvaluator::getBMatrix() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
	return *iMat->getSecondDgnMtrx();
}

Eigen::VectorXd TLSEvaluator::getConstraintMisclosure() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
	return iMat->getCnstrMisclosureVctr();
}

TSparseMatrix TLSEvaluator::getA2Matrix() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
	return *iMat->getCnstrFirstDgnMtrx();
}

TSparseMatrix TLSEvaluator::getPMatrix() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
	return *iMat->getWeightInvMtrx();
}
