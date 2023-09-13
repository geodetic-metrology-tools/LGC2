#include <TLSEvaluator.h>
#include "TLSInputMatricesFiller.h"
#include "TLSConsistencyCheck.h"
#include "TLSUniversalMtdComputer.h"
#include <Logger.hpp>
#include <TLSGraph.h>
#include <Eigen/Dense>
#include "TAdjustableHelmertTransformation.h"

TLSEvaluator::TLSEvaluator(std::shared_ptr<TLGCData> data) : iMat(new TLSInputMatrices)
{
	// create a copy of the LGCData object to use it for manipulating parameter and observation values
	// std::shared_ptr<TLGCData> aux = data->clone();
	// Plan was to use the clone method, but it breaks the links between the cloned tree and the cloned adj object collections... 
	// probably the pointers to the lengths are not reset correctly in the clone method.
	// fData = data->clone();
	// instead take the original object now. The links there work.
	fData = data;
	fMatFiller = new TLSInputMatricesFiller(&fData->getTree(), fData->getConfig().referential, *fData);
	//fMatFiller(filler);
	dimensions = data->fUEOIndices;

	//initialize the mask: all equations and parameters are considered.
	unmask();


	// // do some tests
	// testSetterAndGetter();
	// bool setterEffect = testSetterEffect();
}

TLSEvaluator::~TLSEvaluator()
{
	//delete fMatFiller;
}

void TLSEvaluator::unmask()
{
	// initialize mask Data of active indices, default= all variables and equations are active
	std::vector<int> parIdx;
	for (int i=0;i<dimensions.UIndex;i++){
		parIdx.push_back(i);
	}
	std::vector<int> eqIdx;
	for (int i=0;i<dimensions.EIndex;i++){
		eqIdx.push_back(i);
	}
	currentMask.parameterIndices = parIdx;
	currentMask.equationIndices= eqIdx;
}
Eigen::VectorXd TLSEvaluator::getMisclosure(bool useMask)
{
	// 1. set parameters in "estimated" fields of adjustable objects
	evaluate();
	Eigen::VectorXd result = iMat->getMisclosureVctr();
	if (useMask)
		return result(currentMask.equationIndices);
	return result;
	//// create matrices for model evaluation
   	//TLSInputMatrices matrices;
   	//matrices.initMatrices(fData->fUEOIndices);
	//// evaluate using the standard inputMatrixFiller
   	//bool success =	fMatFiller->fillMatrices(fData.get(), true, &matrices);
	//// get misclosure
   	//Eigen::VectorXd misclosure = matrices.getMisclosureVctr();
	//return misclosure;
}
//Eigen::VectorXd TLSEvaluator::getConstraintMisclosure()
//{
//	evaluate();
//	return iMat->getCnstrMisclosureVctr();
//}
Eigen::VectorXd TLSEvaluator::getResidual(bool useMask)
{
	evaluate();
	return getMisclosure(useMask);
}
Eigen::VectorXd TLSEvaluator::getWeightedResidual(bool useMask)
{
	evaluate();
	return getSqrtPv(useMask) * getResidual(useMask);
}
double TLSEvaluator::getObjective(bool useMask)
{
	return getWeightedResidual().squaredNorm();
}
const TSparseMatrix TLSEvaluator::getA(bool useMask)
{
	// 1. set parameters in "estimated" fields of adjustable objects
	evaluate();
	if (useMask)
	{
		Eigen::SparseMatrix<double> result = maskColumns(currentMask.parameterIndices, maskRows(currentMask.equationIndices, *iMat->getFirstDgnMtrx()));
		return result;
	}
	else
		return *iMat->getFirstDgnMtrx();

}

//const TSparseMatrix *TLSEvaluator::getA2(bool useMask)
//{
//	evaluate();
//	return iMat->getCnstrFirstDgnMtrx();
//}

const TSparseMatrix TLSEvaluator::getPv(bool useMask)
{
	evaluate();
	Eigen::SparseMatrix<double> result;
	if (useMask)
	{
		result = maskColumns(currentMask.equationIndices, maskRows(currentMask.equationIndices, *iMat->getWeightMtrx()));
		return result;
	}
	else
	{
		result = *iMat->getWeightMtrx();
	}
	return result;
}

const TSparseMatrix TLSEvaluator::getSqrtPv(bool useMask)
{
	evaluate();
	Eigen::VectorXd diagEntries = getPv(useMask).diagonal();
	int dim = diagEntries.rows();
	Eigen::SparseMatrix<double> result(dim, dim);
	for (int j = 0; j < dim; j++)
	{
		result.coeffRef(j, j) = sqrt(diagEntries(j));
	}
	return result;
}

Eigen::VectorXd TLSEvaluator::getEstParams(bool useMask)
{
	Eigen::VectorXd result(fData->fUEOIndices.UIndex);
	result.setZero();
	getPointParams(result);
	getAngleParams(result);
	getPlaneParams(result);
	getLengthParams(result);
	getTransformationParams(result);
	getLineParams(result);

	if (useMask)
		return result(currentMask.parameterIndices);

	return result;

}


void TLSEvaluator::setParameters(Eigen::VectorXd para, bool useMask)
{
	isUptoDate = false;
	Eigen::VectorXd globalPar;
	if (useMask)
	{
		if (para.size()!=currentMask.parameterIndices.size()){
			throw std::runtime_error("parameter needs to have consistent dimension with active parameter mask");
		}
		// in this case only the active parameters get touched
		// get the full parameter vector
		globalPar = getEstParams(false);
		// manipulate the parameters at the active indices
		globalPar(currentMask.parameterIndices) = para;
	}
	else
	{
		// check if dimension is correct
		if (para.size() != (fData->fUEOIndices.UIndex))
		{
			std::string message;
			message = "variable has wrong dimension, expected: " + std::to_string(fData->fUEOIndices.UIndex) + ", actual: " + std::to_string(para.size());
			throw std::runtime_error(message);
		}
		globalPar = para;
	}
	// go through all adjustable objects, fill in the appropriate values from the parameter vector
	setPointParams(globalPar);
	setAngleParams(globalPar);
	setPlaneParams(globalPar);
	setLengthParams(globalPar);
	setTransformationParams(globalPar);
	setLineParams(globalPar);
}

void TLSEvaluator::testSetterAndGetter()
{
	Eigen::VectorXd prov = getEstParams();
	Eigen::VectorXd testParameter(dimensions.UIndex);
	testParameter.setZero();
	// create a dummy variable
	for (int i = 0; i < dimensions.UIndex; i++)
	{
		testParameter(i) = 1.0 / (i+1);
	}
	//set the parameter
	setParameters(testParameter);

	// now get the parameters and check if they were set accordingly
	Eigen::VectorXd retrievedParameter(dimensions.UIndex);
	retrievedParameter.setZero();
	retrievedParameter = getEstParams();
	// compare both
	bool success = true;
	for (int i = 0; i < dimensions.UIndex; i++)
	{
		double diff = (retrievedParameter(i) - testParameter(i));
		if (fabs(diff) > 1e-6)
		{
			success = false;
			std::cout << "Parameter at Index " << i << " is not set correctly. Set to " << testParameter(i) << " vs retrieved value " << retrievedParameter(i) << std::endl;
		}
	}
	if (success == false)
	{
		std::cout << "Parameter set/get test failed." << std::endl;
	}

	// restore the original parameters in the data structure
	setParameters(prov);

}

bool TLSEvaluator::testSetterEffect()
{
	// test if changing the parameters has an effect on the misclosure. If not, somewhere is a problem (maybe in the evaluator itself or elsewhere)
	Eigen::VectorXd baseVar = getEstParams();
	setParameters(baseVar);
	Eigen::VectorXd baseEval = getMisclosure();

	double smallPerturbation = 1e-5;
	//double smallPerturbation = 0.666;
	bool testPassed = true;
	for (int i = 0; i < dimensions.UIndex; i++)
	{
		Eigen::VectorXd pertVar = baseVar;
		pertVar(i) += smallPerturbation;
		setParameters(pertVar);
		Eigen::VectorXd pertEval = getMisclosure();
		double diff = (baseEval - pertEval).norm();
		if (diff<1e-12)
		{
			testPassed = false;
			std::cout << "Parameter i=" << i << " seems to have no influence on the misclosure." << std ::endl;
			// print norm of associated column of Jacobian in this case. if its 0 this means LGC agrees that no influence on misclosure.. consi check should also complain in this case
			TDenseMatrix Jac = getA().toDense();
			std::cout << "norm of i-th column of LGC Jacobian = " << Jac.col(i).norm() << std::endl;
		}
	}
	// restore the original parameters in the data structure
	setParameters(baseVar);
	return testPassed;
}

bool TLSEvaluator::evaluate()
{ 
	bool success;
	if (isUptoDate)
	{
		// iMat object already contains up to date data
		success = true;
	}
	else
	{
		// evaluate
		iMat->initMatrices(fData->fUEOIndices);
		// evaluate using the standard inputMatrixFiller
		success = fMatFiller->fillMatrices(fData.get(), true, iMat);
		isUptoDate = true;
	}
	return success;
}

void TLSEvaluator::setPointParams(Eigen::VectorXd para)
{
	for (auto &point : fData->getPoints())
	{
		if (point.hasVariable())
		{
			for (int unknIdx = point.getFirstUidx(); unknIdx <= point.getLastUidx(); ++unknIdx)
			{
				// if (unknIdx >= rm.getSolutionVectByConst()->size())
				// {
				// 	logCritical() << "Extract parameters of the adjustable points from the calculated matrices: Unknown index of point" << point.getName() << " exceeds
				// matrix dimensions!"; 	throw std::runtime_error("Unknown index of an point: " + point.getName() + " exceeds matrix dimensions!");
				// }
				point.setEstVal(unknIdx, para(unknIdx));
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
			//			if (unknIdx >= rm.getSolutionVectByConst()->size())
			//				throw std::runtime_error("Unknown index of an angle: " + angle.getName() + " exceeds matrix dimensions!");
			//
			angle.setEstVal(unknIdx, para(unknIdx));
		}
	}
}

void TLSEvaluator::setPlaneParams(Eigen::VectorXd para)
{
	//	logDebug() << "Extract parameters of the adjustable planes from the calculated matrices";
	//
	//	bool critNotExceeded = true;
	//
	for (auto &plane : fData->getPlanes())
	{
		if (plane.hasVariable())
		{
			for (int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); unknIdx++)
			{
				//				if (unknIdx >= rm.getSolutionVectByConst()->size())
				//					throw std::runtime_error("Unknown index of a plane: " + plane.getName() + " exceeds matrix dimensions!");
				//
				plane.setEstVal(unknIdx, para(unknIdx));
				// if (fabsq(correction) > convCrit)
				//	critNotExceeded = false;
			}
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
				//				if (unknIdx >= rm.getSolutionVectByConst()->size())
				//					throw std::runtime_error("Unknown index of a plane: " + line.getName() + " exceeds matrix dimensions!");
				//
				line.setEstVal(unknIdx, para(unknIdx));
				// if (fabsq(correction) > convCrit)
				//	critNotExceeded = false;
			}
		}
	}
}

void TLSEvaluator::setLengthParams(Eigen::VectorXd para)
{
	//	logDebug() << "Extract parameters of the adjustable lengths from the calculated matrices";
	//
	//	bool critNotExceeded = true;
	for (auto &length : fData->getLength())
	{
		if (!length.isFixed())
		{
			MatrixIndex unknIdx = length.getFirstUidx(); // first=last only one unknown fo angle class
			//
			//			if (unknIdx >= rm.getSolutionVectByConst()->size())
			//				throw std::runtime_error("Unknown index of a scalar: " + length.getName() + " exceeds matrix dimensions!");
			//
			length.setEstVal(unknIdx, para(unknIdx));
			//			if (fabsq(correction) > convCrit)
			//				critNotExceeded = false;
		}
	}
}

void TLSEvaluator::setTransformationParams(Eigen::VectorXd para)
{
	//	logDebug() << "Extract parameters of the adjustable transformations from the calculated matrices";
	//
	//	bool critNotExceeded = true;
	//
	for (auto it(fData->getTree().begin()); it != fData->getTree().end(); ++it)
	{
		auto &trafo(it.node->data.get()->frame);

		if (trafo.hasVariable())
		{
			for (int unknIdx = trafo.getFirstUidx(); unknIdx <= trafo.getLastUidx(); unknIdx++)
			{
				//	if (unknIdx >= rm.getSolutionVectByConst()->size())
				//		throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");

				trafo.setEstVal(unknIdx, para(unknIdx));
				//		if (fabsq(correction) > convCrit)
				//			critNotExceeded = false;
			}
		}
	}
}

// getters
void TLSEvaluator::getPointParams(Eigen::VectorXd & para)
{
	for (auto &point : fData->getPoints())
	{
		if (point.hasVariable())
		{
	//		std::cout << point.getEstParamVector() << std::endl;
			para.middleRows(point.getFirstUidx(), point.getNumUnkn()) = point.getEstParamVector()(point.getRelativeUnknIndices());
			//std::cout << "pt setter" << std::endl << point.getEstParamVector()(point.getRelativeUnknIndices()) << std::endl;
		}
	}
}

void TLSEvaluator::getAngleParams(Eigen::VectorXd & para)
{
	for (auto &angle : fData->getAngles())
	{
		if (!angle.isFixed())
		{
			// angle has at most 1 value
			para(angle.getFirstUidx()) = angle.getEstParamVector()(0);
		}
	}
}

void TLSEvaluator::getPlaneParams(Eigen::VectorXd & para)
{
	//	logDebug() << "Extract parameters of the adjustable planes from the calculated matrices";
	//
	//	bool critNotExceeded = true;
	//
	for (auto &plane : fData->getPlanes())
	{
		if (plane.hasVariable())
		{
			para.middleRows(plane.getFirstUidx(), plane.getNumUnkn()) = plane.getEstParamVector()(plane.getRelativeUnknIndices());
		}
	}
}

void TLSEvaluator::getLineParams(Eigen::VectorXd & para)
{
	for (auto &line : fData->getLines())
	{
		if (!line.isFixed())
		{
			Eigen::VectorXd vect(3);
			vect << line.getLineVectorEstimatedValue().getX(), line.getLineVectorEstimatedValue().getY(), line.getLineVectorEstimatedValue().getZ();
			para.middleRows(line.getFirstUidx(), line.getNumUnkn()) = vect(line.getRelativeUnknIndices());
		}
	}
}



void TLSEvaluator::getLengthParams(Eigen::VectorXd & para)
{
	//	logDebug() << "Extract parameters of the adjustable lengths from the calculated matrices";
	//
	//	bool critNotExceeded = true;
	for (auto &length : fData->getLength())
	{
		if (!length.isFixed())
		{
			para(length.getFirstUidx()) = length.getEstimatedValue();
		}
	}
}

void TLSEvaluator::getTransformationParams(Eigen::VectorXd & para)
{
	//	logDebug() << "Extract parameters of the adjustable transformations from the calculated matrices";
	//
	//	bool critNotExceeded = true;
	//
	for (auto it(fData->getTree().begin()); it != fData->getTree().end(); ++it)
	{
		auto &trafo(it.node->data.get()->frame);

		if (trafo.hasVariable())
		{
			int nFreeParams = trafo.getNumUnkn();
			Eigen::VectorXd trafoParams(nFreeParams);
			trafoParams = trafo.getEstParamVector()(trafo.getRelativeUnknIndices());
			//std::cout << std::endl << "frame params = " << std::endl << trafoParams << std::endl;
			para.middleRows(trafo.getFirstUidx(), trafo.getNumUnkn()) = trafoParams;
		}
	}
}
