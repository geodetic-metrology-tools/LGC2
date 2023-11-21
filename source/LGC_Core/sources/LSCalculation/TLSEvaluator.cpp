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
Eigen::VectorXd TLSEvaluator::getConstraintMisclosure(bool useMask)
{
	// 1. set parameters in "estimated" fields of adjustable objects
	evaluate();
	Eigen::VectorXd result = iMat->getCnstrMisclosureVctr();
	// there is not yet a mask for constraints
// 	if (useMask)
// 		return result;
	return result;
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

const TSparseMatrix TLSEvaluator::getB(bool useMask)
{	
	// 1. set parameters in "estimated" fields of adjustable objects
	// Masking: no "parameters" t mask in B matrix
	evaluate();
	if (useMask)
	{
		Eigen::SparseMatrix<double> result = maskColumns(currentMask.equationIndices, maskRows(currentMask.equationIndices, *iMat->getSecondDgnMtrx()));
		return result;
	}
	else
		return *iMat->getSecondDgnMtrx();
}

const TSparseMatrix TLSEvaluator::getA2(bool useMask)
{
	// 1. set parameters in "estimated" fields of adjustable objects
	evaluate();
	if (useMask)
	{
		Eigen::SparseMatrix<double> result = maskColumns(currentMask.parameterIndices, *iMat->getCnstrFirstDgnMtrx());
		return result;
	}
	else
		return *iMat->getCnstrFirstDgnMtrx();
}

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
	// PARAMETERS
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
	bool successParameters = true;
	for (int i = 0; i < dimensions.UIndex; i++)
	{
		double diff = (retrievedParameter(i) - testParameter(i));
		if (fabs(diff) > 1e-6)
		{
			successParameters = false;
			std::cout << "Parameter at Index " << i << " is not set correctly. Set to " << testParameter(i) << " vs retrieved value " << retrievedParameter(i) << std::endl;
		}
	}
	if (successParameters == false)
	{
		std::cout << "Parameter set/get test failed." << std::endl;
	}

	// restore the original parameters in the data structure
	setParameters(prov);

	// OBSERVATIONS
	Eigen::VectorXd originalObservation(dimensions.OIndex);
	getObservations(originalObservation);
	Eigen::VectorXd testObservation(dimensions.OIndex);
	testObservation.setZero();
	// create a dummy variable
	for (int i = 0; i < dimensions.OIndex; i++)
	{
		testObservation(i) = 1.0 / (i+1);
	}
	//set the observation
	setObservations(testObservation);

	// now get the observations and check if they were set accordingly
	Eigen::VectorXd retrievedObservation(dimensions.OIndex);
	retrievedObservation.setZero();
	getObservations(retrievedObservation);
	// compare both
	bool successObseravtions = true;
	int counter = 0;
	for (int i = 0; i < dimensions.OIndex; i++)
	{
		double diff = (retrievedObservation(i) - testObservation(i));
		if (fabs(diff) > 1e-6)
		{
			std::cout << counter++ << std::endl;
			successObseravtions  = false;
			std::cout << "Observation at Index " << i << " is not set correctly. Set to " << testObservation(i) << " vs retrieved value " << retrievedObservation(i) << std::endl;
		}
	}
	if (successObseravtions  == false)
	{
		std::cout << "Observation set/get test failed." << std::endl;
	}

	// restore the original observations in the data structure
	setObservations(originalObservation);


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

void TLSEvaluator::setObservations(Eigen::VectorXd L)
{
	for (TDataTreeIterator itTree = fData.get()->getTree().begin(); itTree != fData.get()->getTree().end(); itTree++)
	{
		// PDOR
		if (itTree.node->data->measurements.fPDOR.isInitialised())
		{
			TPdorObs &pdor = itTree.node->data->measurements.fPDOR;
			pdor.setBearing(TAngle(L(pdor.getFirstObservationIndex())));
		}

		// Iterate through the Total station measurements (TSTN)
		for (auto itTSTN : itTree.node->data->measurements.fTSTN)
		{
			// Iterate through every ROM of TSTN
			for (auto itROM : itTSTN->roms)
			{
				for (auto &itANGL : itROM->measANGL)
				{
					itANGL.setAngle(TAngle(L(itANGL.getFirstObservationIndex())));
				}

				for (auto &itZEND : itROM->measZEND)
				{
					itZEND.setAngle(TAngle(L(itZEND.getFirstObservationIndex())));
				}

				for (auto &itDIST : itROM->measDIST)
				{
					itDIST.setDistance(TLength(L(itDIST.getFirstObservationIndex())));
				}
				for (auto &itECTH : itROM->measECTH)
				{
					itECTH.setDistance(TLength(L(itECTH.getFirstObservationIndex())));
				}

				for (auto &itECDIR : itROM->measECDIR)
				{
					itECDIR.setDistance(TLength(L(itECDIR.getFirstObservationIndex())));
				}

				for (auto &itDHOR : itROM->measDHOR)
				{
					itDHOR.setDistance(TLength(L(itDHOR.getFirstObservationIndex())));
				}

				for (auto &itPLR3D : itROM->measPLR3D)
				{
					itPLR3D.setAngle(TAngle(L(itPLR3D.getFirstObservationIndex())), kANGL);
					itPLR3D.setAngle(TAngle(L(itPLR3D.getFirstObservationIndex() + 1)), kZEND);
					itPLR3D.setDistance(TLength(L(itPLR3D.getFirstObservationIndex() + 2)));
				}
			}
		}

		// Iterate through camera (CAM) measurements
		for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM)
		{
			for (auto &itUVD : itCAM->measUVD)
			{
				int firstObsIdx = itUVD.getFirstObservationIndex();
				// internally UVD has 3 residuals: x,y, and distance
				// zcomp is such that x,y,z is normalized
				double zObs = sqrt(1 - pow2(L(firstObsIdx)) - pow2(L(firstObsIdx + 1)));
				TFreeVector direction(L(firstObsIdx), L(firstObsIdx + 1), zObs, TCoordSysFactory::k3DCartesian);
				itUVD.setVectorMeasurement(direction);
				itUVD.setDistance(TLength(L(firstObsIdx + 2)));
			}

			for (auto &itUVEC : itCAM->measUVEC)
			{
				int firstObsIdx = itUVEC.getFirstObservationIndex();
				// internally UVEC has 2 residuals: x,y
				// zcomp is such that x,y,z is normalized
				double zObs = sqrt(1 - pow2(L(firstObsIdx)) - pow2(L(firstObsIdx + 1)));
				TFreeVector direction(L(firstObsIdx), L(firstObsIdx + 1), zObs, TCoordSysFactory::k3DCartesian);
				itUVEC.setVectorMeasurement(direction);
			}
		}
		// In every node iterate through the EDM's measurements
		for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
		{
			// Iterate through DSPT measurements
			for (auto &itDSPT : itEDM->measDSPT) // TYPO in original implementation
			{
				itDSPT.setDistance(TLength(L(itDSPT.getFirstObservationIndex())));
			}
		}
		// In every node iterate through the LEVEL's measurements
		for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
		{
			for (auto &itDLEV : itLEVEL.measDLEV)
			{
				itDLEV.setDistance(TLength(L(itDLEV.getFirstObservationIndex())));
				// In a case that optional DHOR measurement is done
				if (itDLEV.dhor)
				{ // i.e. !=nullptr
					itDLEV.dhor->setDistance(TLength(L(itDLEV.dhor->getFirstObservationIndex())));
				}
			}
		}
		// In every node iterate through the ECHOROM's measurements
		for (auto &itECHOrom : itTree.node->data->measurements.fECHO)
		{
			for (auto &itECHO : itECHOrom.measECHO)
			{
				itECHO.setDistance(TLength(L(itECHO.getFirstObservationIndex())));
			}
		}
		// In every node iterate through the ECSP measurements
		for (auto &itECSProm : itTree.node->data->measurements.fECSP)
		{
			for (auto &itECSP : itECSProm.measECSP)
			{
				itECSP.setDistance(TLength(L(itECSP.getFirstObservationIndex())));
			}
		}
		// In every node iterate through the ECVEROM's measurements
		for (auto &itECVErom : itTree.node->data->measurements.fECVE)
		{
			for (auto &itECVE : itECVErom.measECVE)
			{
				itECVE.setDistance(TLength(L(itECVE.getFirstObservationIndex())));
			}
		}
		// In every node iterate through the ORIEROM's measurements
		for (auto &itORIErom : itTree.node->data->measurements.fORIE)
		{
			for (auto &itORIE : itORIErom.measORIE)
			{
				itORIE.setAngle(TAngle(L(itORIE.getFirstObservationIndex())));
			}
		}
		for (auto &itDVER : itTree.node->data->measurements.fDVER)
		{
			itDVER.setDistance(TLength(L(itDVER.getFirstObservationIndex())));
		}
		for (auto &itRADI : itTree.node->data->measurements.fRADI)
		{
			// radi is a "constraint"
			itRADI.setAngleCnstr(TAngle(L(itRADI.getFirstObservationIndex())));
		}
		for (auto &itOBSXYZ : itTree.node->data->measurements.fOBSXYZ)
		{
			int firstObsIdx = itOBSXYZ.getFirstObservationIndex();
			TPositionVector obsVector(L(firstObsIdx), L(firstObsIdx + 1), L(firstObsIdx + 2), TCoordSysFactory::ECoordSys::k3DCartesian);
			itOBSXYZ.obsValue = obsVector;
		}
		for (auto &itINCLYrom : itTree.node->data->measurements.fINCLY)
		{
			for (auto &itINCLY : itINCLYrom.measINCLY)
			{
				itINCLY.setAngle(TAngle(L(itINCLY.getFirstObservationIndex())));
			}
		}
		for (auto &itECWSrom : itTree.node->data->measurements.fECWS)
		{
			for (auto &itECWS : itECWSrom.measECWS)
			{
				itECWS.setDistance(TLength(L(itECWS.getFirstObservationIndex())));
			}
		}
		for (auto &itECWIrom : itTree.node->data->measurements.fECWI)
		{
			for (auto &itECWI : itECWIrom.measECWI)
			{
				itECWI.setDistance(TLength(L(itECWI.getFirstObservationIndex())), EECWIDistances::kX);
				itECWI.setDistance(TLength(L(itECWI.getFirstObservationIndex() + 1)), EECWIDistances::kZ);
			}
		}
	}
}


void TLSEvaluator::getObservations(Eigen::VectorXd &L)
{
	for (TDataTreeIterator itTree = fData.get()->getTree().begin(); itTree != fData.get()->getTree().end(); itTree++)
	{
		// PDOR
		if (itTree.node->data->measurements.fPDOR.isInitialised())
		{
			TPdorObs &pdor = itTree.node->data->measurements.fPDOR;
			L(pdor.getFirstObservationIndex()) = pdor.getBearing();
		}

		// Iterate through the Total station measurements (TSTN)
		for (auto itTSTN : itTree.node->data->measurements.fTSTN)
		{
			// Iterate through every ROM of TSTN
			for (auto itROM : itTSTN->roms)
			{
				for (auto &itANGL : itROM->measANGL)
				{
					L(itANGL.getFirstObservationIndex()) = itANGL.getAngle();
				}

				for (auto &itZEND : itROM->measZEND)
				{
					L(itZEND.getFirstObservationIndex()) = itZEND.getAngle();
				}

				for (auto &itDIST : itROM->measDIST)
				{
					L(itDIST.getFirstObservationIndex()) = itDIST.getDistance();
				}
				for (auto &itECTH : itROM->measECTH)
				{
					L(itECTH.getFirstObservationIndex()) = itECTH.getDistance();
				}

				for (auto &itECDIR : itROM->measECDIR)
				{
					L(itECDIR.getFirstObservationIndex()) = itECDIR.getDistance();
				}

				for (auto &itDHOR : itROM->measDHOR)
				{
					L(itDHOR.getFirstObservationIndex()) = itDHOR.getDistance();
				}

				for (auto &itPLR3D : itROM->measPLR3D)
				{
					L(itPLR3D.getFirstObservationIndex()) = itPLR3D.getAngle(kANGL);
					L(itPLR3D.getFirstObservationIndex() + 1) = itPLR3D.getAngle(kZEND);
					L(itPLR3D.getFirstObservationIndex() + 2) = itPLR3D.getDistance();
				}
			}
		}

		// Iterate through camera (CAM) measurements
		for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM)
		{
			for (auto &itUVD : itCAM->measUVD)
			{
				int firstObsIdx = itUVD.getFirstObservationIndex();
				// internally UVD has 3 residuals: x,y, and distance
				// zcomp is such that x,y,z is normalized
				L(itUVD.getFirstObservationIndex()) = itUVD.getVectorValue().getX();
				L(itUVD.getFirstObservationIndex() + 1) = itUVD.getVectorValue().getY();
				L(itUVD.getFirstObservationIndex() + 2) = itUVD.getDistance();
			}

			for (auto &itUVEC : itCAM->measUVEC)
			{
				int firstObsIdx = itUVEC.getFirstObservationIndex();
				// internally UVEC has 2 residuals: x,y
				L(itUVEC.getFirstObservationIndex()) = itUVEC.getVectorValue().getX();
				L(itUVEC.getFirstObservationIndex() + 1) = itUVEC.getVectorValue().getY();
			}
		}
		// In every node iterate through the EDM's measurements
		for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
		{
			// Iterate through DSPT measurements
			for (auto &itDSPT : itEDM->measDSPT) // TYPO in original implementation
			{
				L(itDSPT.getFirstObservationIndex()) = itDSPT.getDistance();
			}
		}
		// In every node iterate through the LEVEL's measurements
		for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
		{
			for (auto &itDLEV : itLEVEL.measDLEV)
			{
				L(itDLEV.getFirstObservationIndex()) = itDLEV.getDistance();	
				// In a case that optional DHOR measurement is done
				if (itDLEV.dhor)
				{ // i.e. !=nullptr
					L(itDLEV.dhor->getFirstObservationIndex()) = itDLEV.dhor->getDistance();
				}
			}
		}
		// In every node iterate through the ECHOROM's measurements
		for (auto &itECHOrom : itTree.node->data->measurements.fECHO)
		{
			for (auto &itECHO : itECHOrom.measECHO)
			{
				L(itECHO.getFirstObservationIndex()) = itECHO.getDistance();
			}
		}	
		// In every node iterate through the ECSP measurements
		for (auto &itECSProm : itTree.node->data->measurements.fECSP)
		{
			for (auto &itECSP : itECSProm.measECSP)
			{
				L(itECSP.getFirstObservationIndex()) = itECSP.getDistance();
			}
		}
		// In every node iterate through the ECVEROM's measurements
		for (auto &itECVErom : itTree.node->data->measurements.fECVE)
		{
			for (auto &itECVE : itECVErom.measECVE)
			{
				L(itECVE.getFirstObservationIndex()) = itECVE.getDistance();
			}
		}
		// In every node iterate through the ORIEROM's measurements
		for (auto &itORIErom : itTree.node->data->measurements.fORIE)
		{
			for (auto &itORIE : itORIErom.measORIE)
			{
				L(itORIE.getFirstObservationIndex()) = itORIE.getAngle();
			}
		}
		for (auto &itDVER : itTree.node->data->measurements.fDVER)
		{
			L(itDVER.getFirstObservationIndex()) = itDVER.getDistance();
		}
		for (auto &itRADI : itTree.node->data->measurements.fRADI)
		{
			// radi is a "constraint"
			L(itRADI.getFirstObservationIndex()) = itRADI.getAngleCnstr();
		}
		for (auto &itOBSXYZ : itTree.node->data->measurements.fOBSXYZ)
		{
			int firstObsIdx = itOBSXYZ.getFirstObservationIndex();
			L(firstObsIdx) = itOBSXYZ.obsValue.getX();
			L(firstObsIdx + 1) = itOBSXYZ.obsValue.getY();
			L(firstObsIdx + 2) = itOBSXYZ.obsValue.getZ();
		}
		for (auto &itINCLYrom : itTree.node->data->measurements.fINCLY)
		{
			for (auto &itINCLY : itINCLYrom.measINCLY)
			{
				L(itINCLY.getFirstObservationIndex()) = itINCLY.getAngle();
			}
		}
		for (auto &itECWSrom : itTree.node->data->measurements.fECWS)
		{
			for (auto &itECWS : itECWSrom.measECWS)
			{
				L(itECWS.getFirstObservationIndex()) = itECWS.getDistance();
			}
		}
		for (auto &itECWIrom : itTree.node->data->measurements.fECWI)
		{
			for (auto &itECWI : itECWIrom.measECWI)
			{
				L(itECWI.getFirstObservationIndex()) = itECWI.getDistance(EECWIDistances::kX);
				L(itECWI.getFirstObservationIndex() + 1) = itECWI.getDistance(EECWIDistances::kZ);
			}
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
