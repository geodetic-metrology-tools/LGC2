#include <TLSEvaluator.h>
#include "TLSInputMatricesFiller.h"
#include "TLSConsistencyCheck.h"
#include "TLSUniversalMtdComputer.h"
#include <Logger.hpp>
#include <Eigen/Dense>
#include "TAdjustableHelmertTransformation.h"

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
	// create matrices for model evaluation
   	TLSInputMatrices matrices;
   	matrices.initMatrices(fData->fUEOIndices);
	// evaluate using the standard inputMatrixFiller
   	bool success =	fMatFiller.fillMatrices(fData.get(), true, &matrices);
	// get misclosure
   	Eigen::VectorXd result = matrices.getMisclosureVctr();
	return result;
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
	setPointParams(para);
	setAngleParams(para);
	setPlaneParams(para);
	setLengthParams(para);
	setTransformationParams(para);
	setLineParams(para);

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
			para.middleRows(trafo.getFirstUidx(), trafo.getNumUnkn()) == trafoParams;
		}
	}
}
