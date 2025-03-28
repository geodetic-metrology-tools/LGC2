#include <Eigen/Dense>

#include <Logger.hpp>
#include <TLSEvaluator.h>

#include "TAdjustableHelmertTransformation.h"
#include "TLSConsistencyCheck.h"
#include "TLSInputMatricesFiller.h"
#include "TLSUniversalMtdComputer.h"
//#include <Behavior.h>
//#include <TLGCData.h>
<<<<<<< HEAD
//#include <TLSAlgorithm.h>
=======
#include <TLSAlgorithm.h>
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
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
	evaluate();
	// recover some data
	Eigen::VectorXd misc = getMisclosure();
	TSparseMatrix A = getAMatrix();
	TSparseMatrix B = getBMatrix();
	TSparseMatrix A2 = getA2Matrix();
	TSparseMatrix P = getPMatrix();

<<<<<<< HEAD
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
=======
//	std::cout << "misclosure = " << std::endl;
//	std::cout << misc << std::endl;
//	std::cout << "A = " << std::endl;
//	std::cout << A.toDense() << std::endl;
//	std::cout << "B = " << std::endl;
//	std::cout << B.toDense() << std::endl;
//	std::cout << "A2 = " << std::endl;
//	std::cout << A2.toDense() << std::endl;
//	std::cout << "P = " << std::endl;
//	std::cout << P.toDense() << std::endl;
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40


	return false;
}

<<<<<<< HEAD
=======
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

>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
Eigen::VectorXd TLSEvaluator::getObservations()
{
	// initialize the observation vector
	Eigen::VectorXd L(fData->fUEOIndices.OIndex);
	L.setZero();
//	auto updateL = [&L](const auto &itObs) { L.middleRows(itObs.getFirstObservationIndex(), itObs.getObsDim()) = itObs.getObsVector(); };
	auto updateL = [&L](const auto &measGroup) {
		for (const auto &itObs : measGroup)
		{
			L.middleRows(itObs.getFirstObservationIndex(), itObs.getObsDim()) = itObs.getObsVector();
		}
	};

	// off course it would be nice to have just one big container of measurement objects. but the TAMeas class is templated.
	// maybe there should be a non templated base class and aomewhere a global collection of all measurement objects with the basic methods.
	// also not all observations are derived from TAMeas..
	for (TDataTreeIterator itTree = fData.get()->getTree().begin(); itTree != fData.get()->getTree().end(); itTree++)
	{
		// TODO: iterate over all points and add the point with sigma observations

		// PDOR
		if (itTree.node->data->measurements.fPDOR.isInitialised())
		{
			TPdorObs &pdor = itTree.node->data->measurements.fPDOR;
			L.middleRows(pdor.getFirstObservationIndex(), pdor.getObsDim()) = pdor.getObsVector();
		}

		// Iterate through the Total station measurements (TSTN)
		for (auto itTSTN : itTree.node->data->measurements.fTSTN)
		{
			// Iterate through every ROM of TSTN
			for (auto itROM : itTSTN->roms)
			{
				updateL(itROM->measANGL);
				updateL(itROM->measZEND);
				updateL(itROM->measDIST);
				updateL(itROM->measECTH);
				updateL(itROM->measECDIR);
				updateL(itROM->measDHOR);
				updateL(itROM->measPLR3D);
			}
		}

		// Iterate through camera (CAM) measurements
		for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM)
		{
			updateL(itCAM->measUVD);
			updateL(itCAM->measUVEC);
		}
		// In every node iterate through the EDM's measurements
		for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
			updateL(itEDM->measDSPT);
		// In every node iterate through the LEVEL's measurements
		for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
		{
			updateL(itLEVEL.measDLEV);
			// here we need to additionally check the dhor because they are only stored inside the dlev
			for (auto &itDLEV : itLEVEL.measDLEV)
			{
				if (itDLEV.dhor)
				{ // i.e. !=nullptr
					L.middleRows(itDLEV.getFirstObservationIndex(), itDLEV.getObsDim()) = itDLEV.getObsVector();
				}
			}
		}
		// In every node iterate through the ECHOROM's measurements
		for (auto &itECHOrom : itTree.node->data->measurements.fECHO)
			updateL(itECHOrom.measECHO);
		// In every node iterate through the ECSP measurements
		for (auto &itECSProm : itTree.node->data->measurements.fECSP)
			updateL(itECSProm.measECSP);
		// In every node iterate through the ECVEROM's measurements
		for (auto &itECVErom : itTree.node->data->measurements.fECVE)
			updateL(itECVErom.measECVE);
		// In every node iterate through the ORIEROM's measurements
		for (auto &itORIErom : itTree.node->data->measurements.fORIE)
			updateL(itORIErom.measORIE);

		updateL(itTree.node->data->measurements.fDVER);
		updateL(itTree.node->data->measurements.fRADI);
		updateL(itTree.node->data->measurements.fOBSXYZ);

		for (auto &itINCLYrom : itTree.node->data->measurements.fINCLY)
			updateL(itINCLYrom.measINCLY);
		for (auto &itECWSrom : itTree.node->data->measurements.fECWS)
			updateL(itECWSrom.measECWS);
		for (auto &itECWIrom : itTree.node->data->measurements.fECWI)
			updateL(itECWIrom.measECWI);
	}
	return L;
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
<<<<<<< HEAD
	return *iMat->getFirstDgnMtrx();
=======
	return iMat->getFirstDgnMtrx();
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
}

TSparseMatrix TLSEvaluator::getBMatrix() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
<<<<<<< HEAD
	return *iMat->getSecondDgnMtrx();
=======
	return iMat->getSecondDgnMtrx();
}

TSparseMatrix TLSEvaluator::getInvBMatrix() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
	return iMat->getSecondDgnBlockDiagInvMtrx();
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
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
<<<<<<< HEAD
	return *iMat->getCnstrFirstDgnMtrx();
=======
	return iMat->getCnstrFirstDgnMtrx();
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
}

TSparseMatrix TLSEvaluator::getPMatrix() const
{
	if (!isUptoDate)
		throw std::logic_error("Must call evaluate() before using getters");
<<<<<<< HEAD
	return *iMat->getWeightInvMtrx();
=======
	return iMat->getWeightMtrx();
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
}
