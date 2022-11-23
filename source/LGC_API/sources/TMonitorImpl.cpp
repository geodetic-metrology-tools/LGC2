#include "TMonitorImpl.h"

#include <iostream>

#include <Eigen/Dense>

#include <Behavior.h>
#include <TLGCData.h>
#include <TLSAlgorithm.h>
#include <TReader.h>

#include "FileUtils.h"
#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSResultsMatrices.h"
#include "TLSSimulation.h"
#include "TVAbstractAlgorithm.h"

// constructor
TMonitorImpl::TMonitorImpl(std::string configFile)
{
	inputFilePath = configFile;
	initialize();
}

void TMonitorImpl::adjust()
{
	Behavior successCalculation;
	TLSResultsMatrices *results(nullptr);
	successCalculation = algorithm->run(*project.get(), 80);
	if (successCalculation)
	{
		results = algorithm->resultMatrices;
		//std::cout << "Adjustment method finished succesfully." << std::endl;
	}
	else
	{
		std::cout << "Problems occured during Adjustment method." << std::endl;
	}
}
void TMonitorImpl::initialize()
{

	Behavior successCalculation;

	std::cout << "Creating monitoring object.\n";
	std::shared_ptr<TLGCData> projTest(new TLGCData);
	project = projTest;
	TReader r(project);

	project->getFileLogger().setOutputfileLocation("C:/Temp/Fras_Test.txt");
	project->getFileLogger().writeReportHeader("Fras output file");

	// Testfile is LB_calcul_3D_CCS_IP_8_HLS_4.lgc
	//std::string inputFilePath = svlTools::getPathFileName("../LB_calcul_3D_CCS_IP_8_HLS_4.lgc");

	std::ifstream inputFileStream(inputFilePath, std::ifstream::in);
	bool succesReading = r.read(inputFileStream);
	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*project.get());
	std::cout << analyzer.dataConsistent() << std::endl;

	algorithm.reset(new TLSAlgorithm(*project.get()));
	// make measurements easy accessible
	createMeasurementReferences();
	createParameterReferences();
	std::cout << "Monitor object initialized." << std::endl;
}
void TMonitorImpl::createParameterReferences()
{
	for (auto &object : project.get()->getPoints())
	{
		paramRefs.POINTS.insert({object.getName(), object});
		paramRefs.types.insert({object.getName(), "POINT"});
	}
	for (auto &object : project.get()->getLines())
	{
		paramRefs.LINES.insert({object.getName(), object});
		paramRefs.types.insert({object.getName(), "LINE"});
	}
	for (auto &object : project.get()->getAngles())
	{
		paramRefs.ANGLES.insert({object.getName(), object});
		paramRefs.types.insert({object.getName(), "ANGLE"});
	}
	for (auto &object : project.get()->getPlanes())
	{
		paramRefs.PLANES.insert({object.getName(), object});
		paramRefs.types.insert({object.getName(), "PLANE"});
	}
	for (auto &object : project.get()->getLength())
	{
		paramRefs.LENGTHS.insert({object.getName(), object});
		paramRefs.types.insert({object.getName(), "LENGTH"});
	}
	// now the unknowns associated to transformations.. (as in TLSResultsMatricesExtractor::extractTransformationParams)
	// as there is no "adjustable transformation collection", we have to iterate over the tree and get them on our own.
	for (auto it(project.get()->getTree().begin()); it != project.get()->getTree().end(); ++it)
	{
		auto object(it.node->data.get()->frame);
		// std::cout << object.getName() << std::endl;
		paramRefs.TRAFOS.insert({object.getName(), object});
		paramRefs.types.insert({object.getName(), "TRAFO"});
	}
}
void TMonitorImpl::createMeasurementReferences()
{
	for (TDataTreeIterator itTree = project.get()->getTree().begin(); itTree != project.get()->getTree().end(); itTree++)
	{
		// Iterate through the Total station measurements (TSTN)
		for (auto itTSTN : itTree.node->data->measurements.fTSTN)
		{
			// Iterate through every ROM of TSTN
			for (auto itROM : itTSTN->roms)
			{
				for (auto &itANGL : itROM->measANGL)
				{
					measRefs.ANGL.insert({std::to_string(itANGL.line), itANGL});
					measRefs.types.insert({std::to_string(itANGL.line), "ANGL"});
				}

				for (auto &itZEND : itROM->measZEND)
				{
					measRefs.ZEND.insert({std::to_string(itZEND.line), itZEND});
					measRefs.types.insert({std::to_string(itZEND.line), "ZEND"});
				}

				for (auto &itDIST : itROM->measDIST)
				{
					measRefs.DIST.insert({std::to_string(itDIST.line), itDIST});
					measRefs.types.insert({std::to_string(itDIST.line), "DIST"});
				}
				for (auto &itECTH : itROM->measECTH)
				{
					measRefs.ECTH.insert({std::to_string(itECTH.line), itECTH});
					measRefs.types.insert({std::to_string(itECTH.line), "ECTH"});
				}

				for (auto &itECDIR : itROM->measECDIR)
				{
					measRefs.ECDIR.insert({std::to_string(itECDIR.line), itECDIR});
					measRefs.types.insert({std::to_string(itECDIR.line), "ECDIR"});
				}

				for (auto &itDHOR : itROM->measDHOR)
				{
					measRefs.DHOR.insert({std::to_string(itDHOR.line), itDHOR});
					measRefs.types.insert({std::to_string(itDHOR.line), "DHOR"});
				}

				for (auto &itPLR3D : itROM->measPLR3D)
				{
					measRefs.PLR3D.insert({std::to_string(itPLR3D.line), itPLR3D});
					measRefs.types.insert({std::to_string(itPLR3D.line), "PLR3D"});
				}
				// what about ORIEs?
			}
		}

		// Iterate through camera (CAM) measurements
		for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM)
		{
			for (auto &itUVD : itCAM->measUVD)
			{
				measRefs.UVD.insert({std::to_string(itUVD.line), itUVD});
				measRefs.types.insert({std::to_string(itUVD.line), "UVD"});
			}

			for (auto &itUVEC : itCAM->measUVEC)
			{
				measRefs.UVEC.insert({std::to_string(itUVEC.line), itUVEC});
				measRefs.types.insert({std::to_string(itUVEC.line), "UVEC"});
			}
		}
		// In every node iterate through the EDM's measurements
		for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
		{
			// Iterate through DSPT measurements
			for (auto &itDSPT : itEDM->measDSPT) // TYPO in original implementation
			{
				measRefs.DSPT.insert({std::to_string(itDSPT.line), itDSPT});
				measRefs.types.insert({std::to_string(itDSPT.line), "DSPT"});
			}
		}
		// In every node iterate through the LEVEL's measurements
		for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
		{
			for (auto &itDLEV : itLEVEL.measDLEV)
			{
				{
					measRefs.DLEV.insert({std::to_string(itDLEV.line), itDLEV});
					measRefs.types.insert({std::to_string(itDLEV.line), "DLEV"});
				}
			}
		}
		// In every node iterate through the ECHOROM's measurements
		for (auto &itECHOrom : itTree.node->data->measurements.fECHO)
		{
			for (auto &itECHO : itECHOrom.measECHO)
			{
				{
					measRefs.ECHO.insert({std::to_string(itECHO.line), itECHO});
					measRefs.types.insert({std::to_string(itECHO.line), "ECHO"});
				}
			}
		}
		// In every node iterate through the ECVEROM's measurements
		for (auto &itECVErom : itTree.node->data->measurements.fECVE)
		{
			for (auto &itECVE : itECVErom.measECVE)
			{
				{
					measRefs.ECVE.insert({std::to_string(itECVE.line), itECVE});
					measRefs.types.insert({std::to_string(itECVE.line), "ECVE"});
				}
			}
			// for (auto itECVE(itECVErom.measECVE.begin()); itECVE != itECVErom.measECVE.end(); ++itECVE)
			//{
			//	measRefs.ECVE.insert({std::to_string(itECVE->line), *itECVE});
			// }
		}
		// In every node iterate through the ORIEROM's measurements
		for (auto &itORIErom : itTree.node->data->measurements.fORIE)
		{
			for (auto &itORIE : itORIErom.measORIE)
			{
				{
					measRefs.ORIE.insert({std::to_string(itORIE.line), itORIE});
					measRefs.types.insert({std::to_string(itORIE.line), "ORIE"});
				}
			}
		}
		for (auto &itDVER : itTree.node->data->measurements.fDVER)
		{
			{
				measRefs.DVER.insert({std::to_string(itDVER.line), itDVER});
				measRefs.types.insert({std::to_string(itDVER.line), "DVER"});
			}
		}
		for (auto &itRADI : itTree.node->data->measurements.fRADI)
		{
			{
				measRefs.RADI.insert({std::to_string(itRADI.line), itRADI});
				measRefs.types.insert({std::to_string(itRADI.line), "RADI"});
			}
		}
		for (auto &itOBSXYZ : itTree.node->data->measurements.fOBSXYZ)
		{
			{
				measRefs.OBSXYZ.insert({std::to_string(itOBSXYZ.line), itOBSXYZ});
				measRefs.types.insert({std::to_string(itOBSXYZ.line), "OBSXYZ"});
			}
		}
		for (auto &itINCLYrom : itTree.node->data->measurements.fINCLY)
		{
			for (auto &itINCLY : itINCLYrom.measINCLY)
			{
				{
					measRefs.INCLY.insert({std::to_string(itINCLY.line), itINCLY});
					measRefs.types.insert({std::to_string(itINCLY.line), "INCLY"});
				}
			}
		}
		// for (auto &itECWSrom : itTree.node->data->measurements.fECWS)
		// {
		// 	for (auto itECWS(itECWSrom.measECWS.begin()); itECWS != itECWSrom.measECWS.end(); ++itECWS)
		// 	{
		// 		measRefs.ECWS.insert({std::to_string(itECWS->line), *itECWS});
		// 	}
		// }
		for (auto &itECWSrom : itTree.node->data->measurements.fECWS)
		{
			for (auto &itECWS : itECWSrom.measECWS)
			{
				{
					measRefs.ECWS.insert({std::to_string(itECWS.line), itECWS});
					measRefs.types.insert({std::to_string(itECWS.line), "ECWS"});
				}
			}
		}
	}
}
void TMonitorImpl::updateMeas(std::string id, Eigen::VectorXd measurementVector)
{
	// manipulate the corresponding measurement by accesing it via the reference map.
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		std::cout << "No measurement with ID " << id << " found." << std::endl;
		return;
	}

	string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		measRefs.ANGL.at(id).setAngle(TAngle(measurementVector[0], TAngle::kGons));
		return;
	}
	else if (type == "ZEND")
	{
		measRefs.ZEND.at(id).setAngle(TAngle(measurementVector[0], TAngle::kGons));
		return;
	}
	else if (type == "DIST")
	{
		measRefs.DIST.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "ECTH")
	{
		measRefs.ECTH.at(id).setDistance(TLength(measurementVector[0]));
		// return;
	}
	else if (type == "ECDIR")
	{
		measRefs.ECDIR.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "DHOR")
	{
		measRefs.DHOR.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "PLR3D")
	{
		measRefs.PLR3D.at(id).setAngle(TAngle(measurementVector[0], TAngle::kGons), kANGL);
		measRefs.PLR3D.at(id).setAngle(TAngle(measurementVector[1], TAngle::kGons), kZEND);
		measRefs.PLR3D.at(id).setDistance(TLength(measurementVector[2]));
		return;
	}
	else if (type == "ORIE")
	{
		measRefs.ORIE.at(id).setAngle(TAngle(measurementVector[0], TAngle::kGons));
		return;
	}
	else if (type == "UVEC")
	{
		TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		measRefs.UVEC.at(id).setVectorMeasurement(direction);
		return;
	}
	else if (type == "UVD")
	{
		TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		TLength distance(measurementVector[3]);
		measRefs.UVD.at(id).setVectorMeasurement(direction);
		measRefs.UVD.at(id).setDistance(distance);
		return;
	}
	else if (type == "DSPT")
	{
		measRefs.DSPT.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "DLEV")
	{
		// ignoring DHOR
		measRefs.DLEV.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "ECHO")
	{
		measRefs.ECHO.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "ECSP")
	{
		measRefs.ECSP.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "ECVE")
	{
		measRefs.ECVE.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "INCLY")
	{
		measRefs.INCLY.at(id).setAngle(TAngle(measurementVector[0], TAngle::kGons));
		return;
	}
	else if (type == "ECWS")
	{
		measRefs.ECWS.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "DVER")
	{
		measRefs.DVER.at(id).setDistance(TLength(measurementVector[0]));
		return;
	}
	else if (type == "RADI")
	{
		std::cout << "RADI is not a real measurement" << std::endl;
		// measRefs.RADI.at(id).set(TAngle(measurementVector[0]));
		return;
	}
	else if (type == "OBSXYZ")
	{
		TPositionVector obsVector(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::ECoordSys::k3DCartesian);
		// using a setter methof for obsxyz
		measRefs.OBSXYZ.at(id).setObservedVector(obsVector);
		return;
	}
}
Eigen::VectorXd TMonitorImpl::getMeas(std::string id)
{
	// get observation value
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		std::cout << "No measurement with ID " << id << " found." << std::endl;
		Eigen::VectorXd result(1);
		result[0] = 0;
		return result;
	}

	string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double) measRefs.ANGL.at(id).getAngle());
		return result;
	}
	else if (type == "ZEND")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double) measRefs.ZEND.at(id).getAngle());
		return result;
	}
	else if (type == "DIST")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double) measRefs.DIST.at(id).getDistance());
		return result;
	}
	else if (type == "ECTH")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double) measRefs.ECTH.at(id).getDistance());
		return result;
	}
	else if (type == "ECDIR")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double) measRefs.ECDIR.at(id).getDistance());
		return result;
	}
	else if (type == "DHOR")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double) measRefs.DHOR.at(id).getDistance());
		return result;
	}
	else if (type == "PLR3D")
	{
		Eigen::VectorXd result(3);
		result << measRefs.PLR3D.at(id).getAngle(kANGL), measRefs.PLR3D.at(id).getAngle(kZEND), measRefs.PLR3D.at(id).getDistance();
		return result;
	}
	else if (type == "ORIE")
	{
		Eigen::VectorXd result(1);
		result << measRefs.ORIE.at(id).getAngle();
		return result;
	}
	else if (type == "UVEC")
	{
		Eigen::VectorXd result(3);
		result *= 0;
		// NOT implemented
		// measRefs.UVEC.at(id).getVectorValue();
		// TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		// measRefs.UVEC.at(id).setVectorMeasurement(direction);
		return result;
	}
	else if (type == "UVD")
	{	
		Eigen::VectorXd result(3);
		result *= 0;
		// NOT implemented
		// TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		// TLength distance(measurementVector[3]);
		// measRefs.UVD.at(id).setVectorMeasurement(direction);
		// measRefs.UVD.at(id).setDistance(distance);
		return result;
	}
	else if (type == "DSPT")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.DSPT.at(id).getDistance();
		return result;
	}
	else if (type == "DLEV")
	{
		// ignoring DHOR
		Eigen::VectorXd result(1);
		result[0] = measRefs.DHOR.at(id).getDistance();
		return result;
	}
	else if (type == "ECHO")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECHO.at(id).getDistance();
		return result;
	}
	else if (type == "ECSP")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECSP.at(id).getDistance();
		return result;
	}
	else if (type == "ECVE")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECVE.at(id).getDistance();
		return result;
	}
	else if (type == "INCLY")
	{
		Eigen::VectorXd result(1);
		result[0]=measRefs.INCLY.at(id).getAngle();
		return result;
	}
	else if (type == "ECWS")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECWS.at(id).getDistance();
		return result;
	}
	else if (type == "DVER")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.DVER.at(id).getDistance();
		return result;
	}
	else if (type == "RADI")
	{
		Eigen::VectorXd result(0);
		result *= 0;
		std::cout << "RADI is not a real measurement" << std::endl;
		// measRefs.RADI.at(id).set(TAngle(measurementVector[0]));
		// NOT IMPLEMENTED
		return result;
	}
	else if (type == "OBSXYZ")
	{
		Eigen::VectorXd result(3);
		result *= 0;
		// NOT IMPLEMENTED
		return result;
	}
}

Eigen::VectorXd TMonitorImpl::getEstimate(std::string paramId)
{
	if (paramRefs.types.count(paramId) == 0)
	{
		std::cout << "No parameter with Id " << paramId << " found" << std::endl;
	}
	// get type and return result
	if (paramRefs.types.at(paramId) == "POINT")
	{
		TPositionVector result = paramRefs.POINTS.at(paramId).getEstimatedValue();
		Eigen::VectorXd vector(3);
		vector[0] = (double)result.getX();
		vector[1] = (double)result.getY();
		vector[2] = (double)result.getZ();
		return vector;
	}
	else if (paramRefs.types.at(paramId) == "LINE")
	{
		// how many dimensions does this have??	
		TFreeVector result = paramRefs.LINES.at(paramId).getLineVectorEstimatedValue();
		Eigen::VectorXd vector(3);
		vector[0] = (double)result.getX();
		vector[1] = (double)result.getY();
		vector[2] = (double)result.getZ();

		return vector;
	}
	else if (paramRefs.types.at(paramId) == "ANGLE")
	{
		Eigen::VectorXd resultVector(1);
		resultVector[0]=(paramRefs.ANGLES.at(paramId).getEstimatedValue());

		return resultVector;
	}
	else if (paramRefs.types.at(paramId) == "PLANE")
	{
		Eigen::VectorXd resultVector(3);
		resultVector[0] = (double)paramRefs.PLANES.at(paramId).getRefPtDistEstimatedValue();
		resultVector[1] = (double)paramRefs.PLANES.at(paramId).getPhiEstimatedValue();
		resultVector[2] = (double)paramRefs.PLANES.at(paramId).getThetaEstimatedValue();

		return resultVector;
	}
	else if (paramRefs.types.at(paramId) == "LENGTH")
	{
		Eigen::VectorXd resultVector(1);
		resultVector[0] = (double)paramRefs.LENGTHS.at(paramId).getEstimatedValue();

		return resultVector;
	}
	else if (paramRefs.types.at(paramId) == "TRAFO")
	{
		Eigen::VectorXd resultVector(7);
		resultVector[0] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().kappa;
		resultVector[1] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().omega;
		resultVector[2] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().phi;
		resultVector[3] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().tX;
		resultVector[4] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().tY;
		resultVector[5] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().tZ;
		resultVector[6] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().scale;

		return resultVector;
	}


}
Eigen::VectorXd TMonitorImpl::getEstimateCovar(std::string paramId)
{
	// Only Points are implemented for now, will give the sigmas in the frame where the point is declared
	if (paramRefs.types.count(paramId) == 0)
	{
		std::cout << "No parameter with Id " << paramId << " found" << std::endl;
	}
	// get type and return result
	if (paramRefs.types.at(paramId) == "POINT")
	{
		// get precisions, the diagonal covar elements are the square roots
		Eigen::VectorXd vector(3);
		vector[0] = pow((double) paramRefs.POINTS.at(paramId).getXEstPrecision(),2);
		vector[1] = pow((double) paramRefs.POINTS.at(paramId).getYEstPrecision(),2);
		vector[2] = pow((double) paramRefs.POINTS.at(paramId).getZEstPrecision(),2);
		return vector;
	}
	else if (paramRefs.types.at(paramId) == "LINE")
	{
		// how many dimensions does this have??	
		TFreeVector result = paramRefs.LINES.at(paramId).getLineVectorEstimatedValue();
		Eigen::VectorXd vector(3);
		vector[0] = (double)result.getX();
		vector[1] = (double)result.getY();
		vector[2] = (double)result.getZ();

		return vector;
	}
	else if (paramRefs.types.at(paramId) == "ANGLE")
	{
		Eigen::VectorXd resultVector(1);
		resultVector[0]=(paramRefs.ANGLES.at(paramId).getEstimatedValue());

		return resultVector;
	}
	else if (paramRefs.types.at(paramId) == "PLANE")
	{
		Eigen::VectorXd resultVector(3);
		resultVector[0] = (double)paramRefs.PLANES.at(paramId).getRefPtDistEstimatedValue();
		resultVector[1] = (double)paramRefs.PLANES.at(paramId).getPhiEstimatedValue();
		resultVector[2] = (double)paramRefs.PLANES.at(paramId).getThetaEstimatedValue();

		return resultVector;
	}
	else if (paramRefs.types.at(paramId) == "LENGTH")
	{
		Eigen::VectorXd resultVector(1);
		resultVector[0] = (double)paramRefs.LENGTHS.at(paramId).getEstimatedValue();

		return resultVector;
	}
	else if (paramRefs.types.at(paramId) == "TRAFO")
	{
		Eigen::VectorXd resultVector(7);
		resultVector[0] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().kappa;
		resultVector[1] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().omega;
		resultVector[2] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().phi;
		resultVector[3] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().tX;
		resultVector[4] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().tY;
		resultVector[5] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().tZ;
		resultVector[6] = (double)paramRefs.TRAFOS.at(paramId).getEstParam().scale;

		return resultVector;
	}


}

double TMonitorImpl::getSigma0()
{
	return project->getS0APosteriori();
}

std::vector<std::string> TMonitorImpl::getECWSMeasIds()
{
	std::vector<std::string> theIds;
	for (auto aux : measRefs.ECWS)
	{
		theIds.push_back(aux.first);
	}
	return theIds;
}
