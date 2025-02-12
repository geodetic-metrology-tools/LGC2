#include <Eigen/Dense>

#include <Logger.hpp>
#include <TLSEvaluator.h>

#include "TAdjustableHelmertTransformation.h"
#include "TLSConsistencyCheck.h"
#include "TLSInputMatricesFiller.h"
#include "TLSUniversalMtdComputer.h"
#include <TLSAlgorithm.h>
#include <TReader.h>
#include "TDataAnalyzer.h"

template<typename AdjustableObject>
void setAdjObjFromParamForCollection(LGCAdjustableObjectCollection<AdjustableObject> &collection, const Eigen::VectorXd &para)
{
	for (auto &obj : collection)
	{
		setAdjObjFromParam(obj, para);
	}
}
template<typename AdjustableObject>
void setAdjObjFromParam(AdjustableObject &obj, const Eigen::VectorXd &para)
{
	if (obj.isFixed())
		return;

	for (int idx = obj.getFirstUidx(); idx <= obj.getLastUidx(); ++idx)
	{
		obj.setValue(idx, para(idx));
	}
}
template<typename AdjustableObject>
void fillParamForCollection(const LGCAdjustableObjectCollection<AdjustableObject> &collection, Eigen::VectorXd &para)
{
	for (const auto &obj : collection)
	{
		fillParam(obj, para);
	}
}
template<typename AdjustableObject>
void fillParam(const AdjustableObject &obj, Eigen::VectorXd &para)
{
	if (obj.isFixed())
		return;

	para.middleRows(obj.getFirstUidx(), obj.getNumUnkn()) = obj.getEstVector()(obj.getRelativeUnknIndices());
}

TLSEvaluator::TLSEvaluator(std::shared_ptr<TLGCData> data) : iMat(std::make_unique<TLSInputMatrices>())
{
	fData = data;
	fMatFiller = std::make_unique<TLSInputMatricesFiller>(&fData->getTree(), fData->getConfig().referential, *fData);
}

TLSEvaluator::TLSEvaluator(std::stringstream &fileStream) : iMat(std::make_unique<TLSInputMatrices>())
{
	Behavior successCalculation;
	auto project = std::make_shared<TLGCData>();
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
	fMatFiller = std::make_unique<TLSInputMatricesFiller>(&fData->getTree(), fData->getConfig().referential, *fData);
}

TLSEvaluator::~TLSEvaluator()
{
}

void TLSEvaluator::checkIsUptoDate() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
}

Eigen::VectorXd TLSEvaluator::getEstParams() const
{
	Eigen::VectorXd result(fData->fUEOIndices.UIndex);
	result.setZero();
	fillParamForCollection(fData->getPoints(), result);
	fillParamForCollection(fData->getAngles(), result);
	fillParamForCollection(fData->getPlanes(), result);
	fillParamForCollection(fData->getLength(), result);
	fillParamForCollection(fData->getLines(), result);
	// frames are not organized in adjustable object collection
	for (auto it(fData->getTree().begin()); it != fData->getTree().end(); ++it)
	{
		const auto &trafo(it.node->data.get()->frame);
		fillParam(trafo, result);
	}
	
	return result;
}

bool TLSEvaluator::testParameterSetAndGet()
{
	// set, get and compare
	Eigen::VectorXd testPar = Eigen::VectorXd::LinSpaced(fData->fUEOIndices.UIndex, 0.5, 1.5);
	setParameters(testPar);
	Eigen::VectorXd recoveredPar = getEstParams();
	double diffParameters = (testPar - recoveredPar).norm();

	return isZero(diffParameters);
}

bool TLSEvaluator::testEvaluate()
{
	// set a parameter
	Eigen::VectorXd testPar = Eigen::VectorXd::LinSpaced(fData->fUEOIndices.UIndex, 0.5, 1.5);
	setParameters(testPar);
	// evaluate
	bool success = evaluate();
	// recover some data
	try
	{
		Eigen::VectorXd misc = getMisclosure();
		TSparseMatrix A = getAMatrix();
		TSparseMatrix B = getBMatrix();
		TSparseMatrix A2 = getA2Matrix();
		TSparseMatrix P = getPMatrix();
	}
	catch (const std::exception &ex)
	{
		logWarning() << "evaluate() method of evaluator failed";
		success = false;
	}

	return success;
}

Eigen::MatrixXd TLSEvaluator::getFiniteDifferenceA(double finiteDiffEpsilon)
{
	Eigen::VectorXd basePar = getEstParams();
	// compute the Jacobian column by column using finite differences
	Eigen::VectorXd baseEval = getMisclosure();
	Eigen::VectorXd testEval;
	Eigen::VectorXd testPar = basePar;
	Eigen::VectorXd jacobianCol = Eigen::VectorXd::Zero(fData->fUEOIndices.EIndex);
	// the result will be written in a dense matrix object
	Eigen::MatrixXd finiteDiffJacobian = Eigen::MatrixXd::Zero(fData->fUEOIndices.EIndex,fData->fUEOIndices.UIndex);
	for (int j = 0; j < fData->fUEOIndices.UIndex; j++)
	{
		testPar = basePar;
		testPar(j) += finiteDiffEpsilon;
		setParameters(testPar);
		evaluate();
		testEval = getMisclosure();
		jacobianCol = (testEval - baseEval) / finiteDiffEpsilon;
		finiteDiffJacobian.col(j) = jacobianCol;
	}
	return finiteDiffJacobian;	
}

bool TLSEvaluator::tryLGCSolve(TVector &solution)
{
	bool success = false;
	Behavior successCalculation;
	TLSAlgorithm algo(*fData);
	successCalculation = algo.run(*fData, 80);
	if (successCalculation)
	{
		success = true;
		solution = getEstParams();
	}

	return success;
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
	setAdjObjFromParamForCollection(fData->getPoints(), para);
	setAdjObjFromParamForCollection(fData->getAngles(), para);
	setAdjObjFromParamForCollection(fData->getPlanes(), para);
	setAdjObjFromParamForCollection(fData->getLength(), para);
	setAdjObjFromParamForCollection(fData->getLines(), para);
	// frames are not organized in adjustable object collection
	for (auto it(fData->getTree().begin()); it != fData->getTree().end(); ++it)
	{
		TAdjustableHelmertTransformation &trafo(it.node->data.get()->frame);
		setAdjObjFromParam(trafo, para);
	}
}

bool TLSEvaluator::evaluate()
{
	bool success = false;
	if (isUptoDate)
	{
		// nothing to do, results are ready
		return true;
	}
	if (fMatFiller->fillMatrices(fData.get(), iMat.get()))
	{
		// evaluation completed successfully
			isUptoDate = true;
		return true;
	}
	return false;
}

const Eigen::VectorXd &TLSEvaluator::getMisclosure() const
{
	checkIsUptoDate();
	return iMat->getMisclosureVctr();
}

const TSparseMatrix &TLSEvaluator::getAMatrix() const
{
	checkIsUptoDate();
	return iMat->getFirstDgnMtrx();
}

const TSparseMatrix &TLSEvaluator::getBMatrix() const
{
	checkIsUptoDate();
	return iMat->getSecondDgnMtrx();
}

const TSparseMatrix &TLSEvaluator::getInvBMatrix() const
{
	checkIsUptoDate();
	return iMat->getSecondDgnBlockDiagInvMtrx();
}

const Eigen::VectorXd &TLSEvaluator::getConstraintMisclosure() const
{
	checkIsUptoDate();
	return iMat->getCnstrMisclosureVctr();
}

const TSparseMatrix &TLSEvaluator::getA2Matrix() const
{
	checkIsUptoDate();
	return iMat->getCnstrFirstDgnMtrx();
}

const TSparseMatrix &TLSEvaluator::getPMatrix() const
{
	checkIsUptoDate();
	return iMat->getWeightMtrx();
}

const TLSInputMatrices &TLSEvaluator::getInputMatricesRef() const
{
	checkIsUptoDate();
	return *iMat;
}

TLSInputMatrices TLSEvaluator::getInputMatricesCopy()
{
	checkIsUptoDate();
	return *iMat;
}
