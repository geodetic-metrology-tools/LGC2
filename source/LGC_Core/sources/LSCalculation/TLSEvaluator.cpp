#include <TLSEvaluator.h>
#include "TLSInputMatricesFiller.h"
#include "TLSConsistencyCheck.h"
#include "TLSUniversalMtdComputer.h"
#include <Logger.hpp>

TLSEvaluator::TLSEvaluator(std::shared_ptr<TLGCData> data) : fMatFiller(&data->getTree(), data->getConfig().referential)
{
	// create a copy of the LGCData object to use it for manipulating parameter and observation values
	// std::shared_ptr<TLGCData> aux = data->clone();
	fData = data->clone();

	//TLSInputMatricesFiller fMatFiller(&fData->getTree(), fData->getConfig().referential);
	//TLSInputMatricesFiller matFiller(&fData->getTree(), fData->getConfig().referential);
//	matFiller = aux;
	//std::shared_ptr<TLSInputMatricesFiller> matFiller(new TLSInputMatricesFiller(&fData->getTree(), fData->getConfig().referential));
	//matrixFiller(&fData->getTree(), fData->getConfig().referential);


	//// identify the constraints necessary, create them
	//if (data.getConfig().libre.isActive())
	//{
	//	fLibrCnstrGenerator.initCnstrIdentifier(data);
	//	data.fUEOIndices.CIndex = fLibrCnstrGenerator.getNumberOfConstraint();
	//}
}


Eigen::VectorXd TLSEvaluator::evaluate(Eigen::VectorXd parameter)
{
	// 1. set parameters in "estimated" fields of adjustable objects
	setParameters(parameter);
	//Eigen::VectorXd result(fData->fUEOIndices.EIndex);
	TLSInputMatrices matrices;
	matrices.initMatrices(fData->fUEOIndices);
	//matrices->initMatrices(fData->fUEOIndices);
	bool success =	fMatFiller.fillMatrices(fData.get(), true, &matrices);
	Eigen::VectorXd result = matrices.getMisclosureVctr();

	//fMatFiller.fillMatrices();
	//matrixFiller.
	//result.setZero();
	// 2. call inputmatrixfiller
	// 3. get result from inputmatrices object
	return result;
}
void TLSEvaluator::setParameters(Eigen::VectorXd para)
{
	// check if dimension is correct
	if (para.size() != (fData->fUEOIndices.UIndex))
	{
		std::string message;
		message = "variable has wrong dimension, expected: " + std::to_string(fData->fUEOIndices.UIndex) + ", actual: " + std::to_string(para.size());
		std::cout << message << std::endl;
	}
	// go through all adjustable objects, fill in the appropriate values from the parameter vector
	// setPointParams(para);
	// setAngleParams(para);
	// setPlaneParams(para);
	// setLengthParams(para);
	// setTransformationParams(para);
	// setLineParams(para);

}

// void TLSEvaluator::setPointParams(Eigen::VectorXd para)
//{
//
//	for (auto& point : fData->getPoints())
//	{
//		if (point.hasVariable()) {
//			true;
//		}
//	}
//	logDebug() << "Checking converging criteria on point parameters: " << nParamsOutsideCriteria << "of" << nParamsTotal << "coordinates outside criteria (" << convCrit << ")";
//
//	return critNotExceeded;
//}
//
// void TLSEvaluator::setAngleParams(Eigen::VectorXd para)
//{
//	logDebug() << "Extract parameters of the adjustable angles from the calculated matrices";
//
//	bool critNotExceeded = true;
//
//	for (auto& angle : fDataSet->getAngles()) {
//		if (!angle.isFixed()) {
//			MatrixIndex	unknIdx = angle.getFirstUidx();	//first=last only one unknown fo angle class
//
//			if (unknIdx >= rm.getSolutionVectByConst()->size())
//				throw std::runtime_error("Unknown index of an angle: " + angle.getName() + " exceeds matrix dimensions!");
//
//			TReal	correctionVal = rm.getSolutionVctrElmt(unknIdx);
//			angle.setCorrection(unknIdx, correctionVal);
//			if (fabsq(correctionVal) > convCrit)
//				critNotExceeded = false;
//		}
//	}
//	return critNotExceeded;
//}
//
//
// void TLSEvaluator::setPlaneParams(Eigen::VectorXd para)
//{
//	logDebug() << "Extract parameters of the adjustable planes from the calculated matrices";
//
//	bool critNotExceeded = true;
//
//	for (auto& plane : fDataSet->getPlanes()) {
//		if (plane.hasVariable()) {
//			for (int unknIdx = plane.getFirstUidx(); unknIdx <= plane.getLastUidx(); unknIdx++) {
//				if (unknIdx >= rm.getSolutionVectByConst()->size())
//					throw std::runtime_error("Unknown index of a plane: " + plane.getName() + " exceeds matrix dimensions!");
//
//				TReal	correction = rm.getSolutionVctrElmt(unknIdx);
//				plane.setCorrection(unknIdx, correction);
//				if (fabsq(correction) > convCrit)
//					critNotExceeded = false;
//			}
//		}
//	}
//	return critNotExceeded;
//}
//
// void TLSEvaluator::setLineParams(Eigen::VectorXd para)
//{
//	logDebug() << "Extract parameters of the adjustable lines from the calculated matrices";
//
//	bool critNotExceeded = true;
//
//	for (auto& line : fDataSet->getLines()) {
//		if (!line.isFixed()) {
//			for (int unknIdx = line.getFirstUidx(); unknIdx <= line.getLastUidx(); unknIdx++) {
//				if (unknIdx >= rm.getSolutionVectByConst()->size())
//					throw std::runtime_error("Unknown index of a plane: " + line.getName() + " exceeds matrix dimensions!");
//
//				TReal	correction = rm.getSolutionVctrElmt(unknIdx);
//				line.setCorrection(unknIdx, correction);
//				if (fabsq(correction) > convCrit)
//					critNotExceeded = false;
//			}
//		}
//	}
//	return critNotExceeded;
//}
//
// void TLSEvaluator::setLengthParams(Eigen::VectorXd para)
//{
//	logDebug() << "Extract parameters of the adjustable lengths from the calculated matrices";
//
//	bool critNotExceeded = true;
//	for (auto& length : fDataSet->getLength()) {
//		if (!length.isFixed()) {
//			MatrixIndex unknIdx = length.getFirstUidx();	//first=last only one unknown fo angle class
//
//			if (unknIdx >= rm.getSolutionVectByConst()->size())
//				throw std::runtime_error("Unknown index of a scalar: " + length.getName() + " exceeds matrix dimensions!");
//
//			TReal	correction = rm.getSolutionVctrElmt(unknIdx);
//			length.setCorrection(unknIdx, correction);
//			if (fabsq(correction) > convCrit)
//				critNotExceeded = false;
//		}
//	}
//	return critNotExceeded;
//}
//
// void TLSEvaluator::setTransformationParams(Eigen::VectorXd para)
//{
//	logDebug() << "Extract parameters of the adjustable transformations from the calculated matrices";
//
//	bool critNotExceeded = true;
//
//	for (auto it(fDataSet->getTree().begin()); it != fDataSet->getTree().end(); ++it) {
//		auto& trafo(it.node->data.get()->frame);
//
//		if (trafo.hasVariable()) {
//			for (int unknIdx = trafo.getFirstUidx(); unknIdx <= trafo.getLastUidx(); unknIdx++) {
//				if (unknIdx >= rm.getSolutionVectByConst()->size())
//					throw std::runtime_error("Unknown index of a transformation: " + trafo.getName() + " exceeds matrix dimensions!");
//
//				TReal	correction = rm.getSolutionVctrElmt(unknIdx);
//				trafo.setCorrection(unknIdx, correction);
//				if (fabsq(correction) > convCrit)
//					critNotExceeded = false;
//			}
//		}
//	}
//	return critNotExceeded;
//}