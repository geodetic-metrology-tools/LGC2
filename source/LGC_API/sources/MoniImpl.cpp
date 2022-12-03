#include "MoniImpl.h"
#include "Moni.h"

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
Moni::Moni(std::string inputFilePath) : pimpl_(new MoniImpl(inputFilePath)){}
Moni::~Moni() = default;
void Moni::MoniImpl::initialize()
{
	Behavior successCalculation;
	std::cout << "Creating monitoring object with LGC input file "<< inputFilePath << std::endl;
	std::shared_ptr<TLGCData> projTest(new TLGCData);
	project = projTest;
	TReader r(project);

	project->getFileLogger().setOutputfileLocation("C:/Temp/Fras_Test.txt");
	project->getFileLogger().writeReportHeader("Fras output file");

	// Testfile is LB_calcul_3D_CCS_IP_8_HLS_4.lgc

	std::ifstream inputFileStream(inputFilePath, std::ifstream::in);
	bool succesReading = r.read(inputFileStream);
	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*project.get());
	//std::cout << analyzer.dataConsistent() << std::endl;

	algorithm.reset(new TLSAlgorithm(*project.get()));
	// make measurements & parameters accessible
	createMeasurementReferences();
	createParameterReferences();
	std::cout << "Monitor object initialized." << std::endl;

}

void Moni::MoniImpl::createParameterReferences()
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
void Moni::MoniImpl::createMeasurementReferences()
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


