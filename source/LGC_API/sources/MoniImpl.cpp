#include "MoniImpl.h"
#include "Moni.h"

#include "TAStreamFormatter.h"
#include <iostream>


// constructor
Moni::Moni(std::string inputFilePath) : pimpl_(new MoniImpl(inputFilePath)){}
Moni::~Moni() = default;


//void Moni::writeJsonFile(TLGCData const *const dat, const std::string &outputFileLocation)
void Moni::writeResultFile()
{
	pimpl_->writeResultFile();
}

void Moni::writeLGCInputFile()
{
	pimpl_->writeLGCInputFile();
}

void Moni::updateMeas(std::string id, Eigen::VectorXd measurementVector)
{
	pimpl_->updateMeas(id, measurementVector);
}
	
void Moni::setObsSigma(std::string id, Eigen::VectorXd sigma)
{
	pimpl_->setObsSigma(id, sigma);
}
// triggering the adjustment calculation
bool Moni::adjust()
{
	return pimpl_->adjust();
}
bool Moni::getStatus()
{
	return pimpl_->getStatus();
}
// get estimate of point
Eigen::VectorXd Moni::getPointEstimate(std::string pointId)
{
	return pimpl_->getPointEstimate(pointId);
}
// get estimate of frame
Eigen::VectorXd Moni::getFrameEstimate(std::string frameId)
{
	return pimpl_->getFrameEstimate(frameId);
}
Eigen::VectorXd Moni::getFrameEstimatePrec(std::string frameId)
{
	return pimpl_->getFrameEstimatePrec(frameId);
}


// get estimate of point in subframe
Eigen::VectorXd Moni::getPointEstimate(std::string id, std::string frameName)
{
	return pimpl_->getPointEstimate(id, frameName);
}
// get diagonal elements of covariances of the estimated parameters
Eigen::VectorXd Moni::getPointEstimatePrec(std::string id)
{
	return pimpl_->getPointEstimatePrec(id);
}
// get diagonal elements of covariances of the estimated parameters transformed to a subframe
Eigen::VectorXd Moni::getPointEstimatePrec(std::string id, std::string frameName)
{
	return pimpl_->getPointEstimatePrec(id, frameName);
}
// get estimated residual
Eigen::VectorXd Moni::getEstimateResidual(std::string obsId)
{
	return pimpl_->getEstimateResidual(obsId);
}

// get calculated measurement
Eigen::VectorXd Moni::getCalcMeas(std::string obsId)
{
	return pimpl_->getCalcMeas(obsId);
}


// get Meas IDs
std::vector<std::string> Moni::getECWSMeasIds()
{
	return pimpl_->getECWSMeasIds();
}
Eigen::VectorXd Moni::getMeas(std::string id)
{
	return pimpl_->getMeas(id);
}
// get the sigma0 after adjustment
double Moni::getSigma0()
{
	return pimpl_->getSigma0();
}

// actual Implementation
void Moni::MoniImpl::initialize()
{
	estimationStatus = false;
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
	analyzer.dataConsistent();

	algorithm.reset(new TLSAlgorithm(*project.get()));
	// make measurements & parameters accessible
	createMeasurementReferences();
	createParameterReferences();
	std::cout << "Monitor object initialized." << std::endl;

}
void Moni::MoniImpl::writeResultFile()
{
	jsonSerializerObject ser;
	SerializerObject::SerializationHelper obj = ser.getSerializationHelper();
	obj.addProperty("LGC_DATA", project.get());
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream filename;
	filename << "resultDump_"<< std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H_%M_%S_%S") << ".json";
	std::ofstream fout(filename.str());
	fout << ser.getStringRepresentation();
}
void Moni::MoniImpl::writeLGCInputFile()
{
    // Create and initialise stream:
    std::shared_ptr<TAStreamFormatter> stream;
	TFileParameters resultFileParam;
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream filename;
	filename << "inputDump_"<< std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H_%M_%S_%S") << ".lgc";

	resultFileParam.setFileName(filename.str());

	//Some keywords(options) in the input file responsible for this, for now just setting here one of them (column), but can be semi-colon, dash etc.
	TAStreamFormatter::ETextFormat resultsFileFormat = TAStreamFormatter::ETextFormat::kColumnFormat;

	TStreamFormatterFactory* formatterFactory = TStreamFormatterFactory::instance();
	TDataParameters dataParam;
	dataParam.setRefFrame(TRefSystemFactory::ERefFrame::kCCS); //default param because not redefine
	dataParam.setPrecision(project->getConfig().outPrecision.digits);
	dataParam.setObsIdWidth(project->getConfig().obsIDwidth);

	TADataSet tads(resultFileParam, dataParam);
    stream.reset(formatterFactory->getFormatter(&tads, resultsFileFormat, "   " /* separator */));
    stream->setReferenceFrame(dataParam.getRefFrame());  //default param because not redefine
    stream->setCoordSys(TCoordSysFactory::k3DCartesian);

    TPointFormat* pointFormat = stream->getPointFormat();
	//Set the name width to the max width of a name point +1 characters.
	if (project->getConfig().pointNameWidth>pointFormat->getNameWidth())
	{
		pointFormat->setNameWidth(project->getConfig().pointNameWidth+1);
        stream->setPointFormat(*pointFormat);
	}
	//As the name width is used also for frames. Set the name width to the max of of frame name +1 characters, if necessary.
	for (const auto& itTree : project->getTree())
	{
		if (itTree.get()->frame.getName().length() >= pointFormat->getNameWidth()) {
			pointFormat->setNameWidth((int)itTree.get()->frame.getName().length() + 1);
			stream->setPointFormat(*pointFormat);
		}
	}
	// Create writer, write the file:
	TInputFileWriter infileWriter(stream.get(), project.get());
	infileWriter.writeFile();
}
void Moni::MoniImpl::createParameterReferences()
{
	for (auto &object : project.get()->getPoints())
	{
		paramRefs.POINTS.insert({object.getName(), object});
	}
	for (auto &object : project.get()->getLines())
	{
		paramRefs.LINES.insert({object.getName(), object});
	}
	for (auto &object : project.get()->getAngles())
	{
		paramRefs.ANGLES.insert({object.getName(), object});
	}
	for (auto &object : project.get()->getPlanes())
	{
		paramRefs.PLANES.insert({object.getName(), object});
	}
	for (auto &object : project.get()->getLength())
	{
		paramRefs.LENGTHS.insert({object.getName(), object});
	}
	// now the unknowns associated to transformations.. (as in TLSResultsMatricesExtractor::extractTransformationParams)
	// as there is no "adjustable transformation collection", we have to iterate over the tree and get them on our own.
	for (auto it(project.get()->getTree().begin()); it != project.get()->getTree().end(); ++it)
	{
		auto& object(it.node->data.get()->frame);
		paramRefs.FRAMES.insert({object.getName(), object});
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
					measRefs.ANGL.insert({itANGL.obsID, itANGL});
					measRefs.types.insert({itANGL.obsID, "ANGL"});
				}

				for (auto &itZEND : itROM->measZEND)
				{
					measRefs.ZEND.insert({itZEND.obsID, itZEND});
					measRefs.types.insert({itZEND.obsID, "ZEND"});
				}

				for (auto &itDIST : itROM->measDIST)
				{
					measRefs.DIST.insert({itDIST.obsID, itDIST});
					measRefs.types.insert({itDIST.obsID, "DIST"});
				}
				for (auto &itECTH : itROM->measECTH)
				{
					measRefs.ECTH.insert({itECTH.obsID, itECTH});
					measRefs.types.insert({itECTH.obsID, "ECTH"});
				}

				for (auto &itECDIR : itROM->measECDIR)
				{
					measRefs.ECDIR.insert({itECDIR.obsID, itECDIR});
					measRefs.types.insert({itECDIR.obsID, "ECDIR"});
				}

				for (auto &itDHOR : itROM->measDHOR)
				{
					measRefs.DHOR.insert({itDHOR.obsID, itDHOR});
					measRefs.types.insert({itDHOR.obsID, "DHOR"});
				}

				for (auto &itPLR3D : itROM->measPLR3D)
				{
					measRefs.PLR3D.insert({itPLR3D.obsID, itPLR3D});
					measRefs.types.insert({itPLR3D.obsID, "PLR3D"});
				}
				// what about ORIEs?
			}
		}

		// Iterate through camera (CAM) measurements
		for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM)
		{
			for (auto &itUVD : itCAM->measUVD)
			{
				measRefs.UVD.insert({itUVD.obsID, itUVD});
				measRefs.types.insert({itUVD.obsID, "UVD"});
			}

			for (auto &itUVEC : itCAM->measUVEC)
			{
				measRefs.UVEC.insert({itUVEC.obsID, itUVEC});
				measRefs.types.insert({itUVEC.obsID, "UVEC"});
			}
		}
		// In every node iterate through the EDM's measurements
		for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
		{
			// Iterate through DSPT measurements
			for (auto &itDSPT : itEDM->measDSPT) // TYPO in original implementation
			{
				measRefs.DSPT.insert({itDSPT.obsID, itDSPT});
				measRefs.types.insert({itDSPT.obsID, "DSPT"});
			}
		}
		// In every node iterate through the LEVEL's measurements
		for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
		{
			for (auto &itDLEV : itLEVEL.measDLEV)
			{
				measRefs.DLEV.insert({itDLEV.obsID, itDLEV});
				measRefs.types.insert({itDLEV.obsID, "DLEV"});
			}
		}
		// In every node iterate through the ECHOROM's measurements
		for (auto &itECHOrom : itTree.node->data->measurements.fECHO)
		{
			for (auto &itECHO : itECHOrom.measECHO)
			{
				measRefs.ECHO.insert({itECHO.obsID, itECHO});
				measRefs.types.insert({itECHO.obsID, "ECHO"});
			}
		}
		// In every node iterate through the ECVEROM's measurements
		for (auto &itECVErom : itTree.node->data->measurements.fECVE)
		{
			for (auto &itECVE : itECVErom.measECVE)
			{
				measRefs.ECVE.insert({itECVE.obsID, itECVE});
				measRefs.types.insert({itECVE.obsID, "ECVE"});
			}
		}
		// In every node iterate through the ORIEROM's measurements
		for (auto &itORIErom : itTree.node->data->measurements.fORIE)
		{
			for (auto &itORIE : itORIErom.measORIE)
			{
				measRefs.ORIE.insert({itORIE.obsID, itORIE});
				measRefs.types.insert({itORIE.obsID, "ORIE"});
			}
		}
		for (auto &itDVER : itTree.node->data->measurements.fDVER)
		{
			measRefs.DVER.insert({itDVER.obsID, itDVER});
			measRefs.types.insert({itDVER.obsID, "DVER"});
		}
		for (auto &itRADI : itTree.node->data->measurements.fRADI)
		{
			measRefs.RADI.insert({itRADI.obsID, itRADI});
			measRefs.types.insert({itRADI.obsID, "RADI"});
		}
		for (auto &itOBSXYZ : itTree.node->data->measurements.fOBSXYZ)
		{
			measRefs.OBSXYZ.insert({itOBSXYZ.obsID, itOBSXYZ});
			measRefs.types.insert({itOBSXYZ.obsID, "OBSXYZ"});
		}
		for (auto &itINCLYrom : itTree.node->data->measurements.fINCLY)
		{
			for (auto &itINCLY : itINCLYrom.measINCLY)
			{
				measRefs.INCLY.insert({itINCLY.obsID, itINCLY});
				measRefs.types.insert({itINCLY.obsID, "INCLY"});
			}
		}
		for (auto &itECWSrom : itTree.node->data->measurements.fECWS)
		{
			for (auto &itECWS : itECWSrom.measECWS)
			{
				measRefs.ECWS.insert({itECWS.obsID, itECWS});
				measRefs.types.insert({itECWS.obsID, "ECWS"});
			}
		}	
		for (auto &itECWIrom : itTree.node->data->measurements.fECWI)
		{
			for (auto &itECWI : itECWIrom.measECWI)
			{
				measRefs.ECWI.insert({itECWI.obsID, itECWI});
				measRefs.types.insert({itECWI.obsID, "ECWI"});
			}
		}

	}
}

Eigen::VectorXd Moni::MoniImpl::toVectorXd(TFreeVector freeVector)
{
	Eigen::VectorXd vector(3);
	vector << (double)freeVector.getX(), (double)freeVector.getY(), (double)freeVector.getZ();
	return vector;
}

Eigen::VectorXd Moni::MoniImpl::toVectorXd(TPositionVector posVector)
{
	Eigen::VectorXd vector(3);
	vector << (double)posVector.getX(), (double)posVector.getY(), (double)posVector.getZ();
	return vector;
}

void Moni::MoniImpl::updateMeas(std::string id, Eigen::VectorXd measurementVector)
{

	// reset estimationStatus to false as soon as new measurement data is supplied
	estimationStatus = false;
	// manipulate the corresponding measurement by accessing it via the reference map.
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
	else if (type == "ECWI")
	{	
		measRefs.ECWI.at(id).setDistance(TLength(measurementVector[0]), EECWIDistances::kX);
		measRefs.ECWI.at(id).setDistance(TLength(measurementVector[1]), EECWIDistances::kZ);
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
		// using a setter method for obsxyz
		measRefs.OBSXYZ.at(id).setObservedVector(obsVector);
		return;
	}
}
Eigen::VectorXd Moni::MoniImpl::getEstimateResidual(std::string id)
{
	// manipulate the corresponding measurement by accesing it via the reference map.
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		Eigen::VectorXd res(1);
		res << -1;
		std::cout << "No measurement with ID " << id << " found." << std::endl;
		return res;
	}

	string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		Eigen::VectorXd res(1);
		res<<(double) measRefs.ANGL.at(id).getAngleResidual();
		return res;
	}
	else if (type == "ZEND")
	{
		Eigen::VectorXd res(1);
		res<<(double) measRefs.ZEND.at(id).getAngleResidual();
		return res;
	}
	else if (type == "DIST")
	{
		Eigen::VectorXd res(1);
		res << (double)measRefs.DIST.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "ECTH")
	{
		Eigen::VectorXd res(1);
		// what kind of residual ? distance, angle?
		res << measRefs.ECTH.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "ECDIR")
	{
		Eigen::VectorXd res(1);
		res << measRefs.ECDIR.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "DHOR")
	{
		Eigen::VectorXd res(1);
		res << measRefs.DHOR.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "PLR3D")
	{
		Eigen::VectorXd res(3);
		res << measRefs.PLR3D.at(id).getAngleResidual(kANGL), measRefs.PLR3D.at(id).getAngleResidual(kZEND), measRefs.PLR3D.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "ORIE")
	{
		Eigen::VectorXd res(1);
		res << measRefs.ORIE.at(id).getAngleResidual();
		return res;
	}
	else if (type == "UVEC")
	{
		// to be checked! 
		Eigen::VectorXd res(2);
		res << measRefs.UVEC.at(id).getXCompVectorResidual(), measRefs.UVEC.at(id).getYCompVectorResidual();
		//TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		//measRefs.UVEC.at(id).setVectorMeasurement(direction);
		return res;
	}
	else if (type == "UVD")
	{
		// to be checked, particular the order! 
		Eigen::VectorXd res(3);
		res << measRefs.UVD.at(id).getXCompVectorResidual(), measRefs.UVD.at(id).getYCompVectorResidual(), measRefs.UVD.at(id).getDistanceResidual();
		//TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		//TLength distance(measurementVector[3]);
		//measRefs.UVD.at(id).setVectorMeasurement(direction);
		//measRefs.UVD.at(id).setDistance(distance);
		return res;
	}
	else if (type == "DSPT")
	{
		Eigen::VectorXd res(1);
		res << measRefs.DSPT.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "DLEV")
	{
		// ignoring DHOR
		Eigen::VectorXd res(1);
		res << measRefs.DLEV.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "ECHO")
	{
		Eigen::VectorXd res(1);
		res << measRefs.ECHO.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "ECSP")
	{
		Eigen::VectorXd res(1);
		res << measRefs.ECSP.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "ECVE")
	{
		Eigen::VectorXd res(1);
		res << measRefs.ECVE.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "INCLY")
	{
		Eigen::VectorXd res(1);
		res << measRefs.INCLY.at(id).getAngleResidual();
		return res;
	}
	else if (type == "ECWS")
	{
		Eigen::VectorXd res(1);
		res << measRefs.ECWS.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "ECWI")
	{
		Eigen::VectorXd res(2);
		res << measRefs.ECWI.at(id).getDistanceResidual(EECWIDistances::kX), measRefs.ECWI.at(id).getDistanceResidual(EECWIDistances::kZ);
		return res;
	}
	else if (type == "DVER")
	{
		Eigen::VectorXd res(1);
		res << measRefs.DVER.at(id).getDistanceResidual();
		return res;
	}
	else if (type == "RADI")
	{
		Eigen::VectorXd res(1);
		res << -1;
		std::cout << "RADI is not a real measurement" << std::endl;
		// measRefs.RADI.at(id).set(TAngle(measurementVector[0]));
		return res;
	}
	else if (type == "OBSXYZ")
	{
		Eigen::VectorXd res(3);
		res << measRefs.OBSXYZ.at(id).getXResidual(), measRefs.OBSXYZ.at(id).getYResidual(), measRefs.OBSXYZ.at(id).getZResidual();
		return res;
	}
	Eigen::VectorXd resDummy(1);
	resDummy << -1;
}

bool Moni::MoniImpl::adjust()
{
	Behavior successCalculation;
	TLSResultsMatrices *results(nullptr);
	successCalculation = algorithm->run(*project.get(), 80);
	if (successCalculation)
	{
		results = algorithm->resultMatrices;
		estimationStatus = true;
		//std::cout << "Adjustment method finished succesfully." << std::endl;
	}
	else
	{
		std::cout << "Problems occured during Adjustment method." << std::endl;
	}
	return estimationStatus;
}

std::vector<std::string> Moni::MoniImpl::getECWSMeasIds()
{
	std::vector<std::string> theIds;
	for (auto aux : measRefs.ECWS)
	{
		theIds.push_back(aux.first);
	}
	return theIds;
}
double Moni::MoniImpl::getSigma0()
{
	return project->getS0APosteriori();
}
// get measurement
Eigen::VectorXd Moni::MoniImpl::getMeas(std::string id)
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
		return toVectorXd(measRefs.UVEC.at(id).getVectorValue());
	}
	else if (type == "UVD")
	{	
		Eigen::VectorXd result(4);
		result << toVectorXd(measRefs.UVD.at(id).getVectorValue()), measRefs.UVD.at(id).getDistance().getMetresValue();
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
	else if (type == "ECWI")
	{	
		Eigen::VectorXd result(2);
		result << measRefs.ECWI.at(id).getDistance(EECWIDistances::kX), measRefs.ECWI.at(id).getDistance(EECWIDistances::kZ);
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
		std::cout << "RADI is not a real measurement, the \"observed\" value always corresponds to 0" << std::endl;
		// measRefs.RADI.at(id).set(TAngle(measurementVector[0]));
		// NOT IMPLEMENTED
		return result;
	}
	else if (type == "OBSXYZ")
	{
		return toVectorXd(measRefs.OBSXYZ.at(id).obsValue);
	}
}

void Moni::MoniImpl::setObsSigma(std::string id, Eigen::VectorXd sigma)
{
	// get observation value
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		std::cout << "No measurement with ID " << id << " found." << std::endl;
	}

	string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		measRefs.ANGL.at(id).target.sigmaAngl.setRadiansValue(sigma(0));
	}
	else if (type == "ZEND")
	{
		measRefs.ZEND.at(id).target.sigmaZenD.setRadiansValue(sigma(0));
	}
	else if (type == "DIST")
	{
		measRefs.DIST.at(id).target.sigmaDist.setMetresValue(sigma(0));
	}
	else if (type == "ECTH")
	{
		measRefs.ECTH.at(id).obsHorAngle.setRadiansValue(sigma(0));
	}
	else if (type == "ECDIR")
	{
		measRefs.ECDIR.at(id).obsHorAngle.setRadiansValue(sigma(0));
		measRefs.ECDIR.at(id).obsVertAngle.setRadiansValue(sigma(1));
	}
	else if (type == "DHOR")
	{
		// not supported
		//	measRefs.DHOR.at(id).setDHORSigma(TLength(sigma(0)));
	}
	else if (type == "PLR3D")
	{
		measRefs.PLR3D.at(id).target.sigmaAngl.setRadiansValue(sigma(0));
		measRefs.PLR3D.at(id).target.sigmaZenD.setRadiansValue(sigma(1));
		measRefs.PLR3D.at(id).target.sigmaDist.setMetresValue(sigma(2));
	}
	else if (type == "ORIE")
	{
		measRefs.ORIE.at(id).target.sigmaAngl.setRadiansValue(sigma(0));
	}
	else if (type == "UVEC")
	{
		measRefs.UVEC.at(id).target.sigmaX = sigma(0); // unitless
		measRefs.UVEC.at(id).target.sigmaY = sigma(1); // unitless
	}
	else if (type == "UVD")
	{	
		measRefs.UVD.at(id).target.sigmaX = sigma(0); // unitless
		measRefs.UVD.at(id).target.sigmaY = sigma(1); // unitless
		measRefs.UVD.at(id).target.sigmaDist.setMetresValue(sigma(2));
	}
	else if (type == "DSPT")
	{
		measRefs.DSPT.at(id).target.sigmaDSpt.setMetresValue(sigma(0));
	}
	else if (type == "DLEV")
	{
		measRefs.DLEV.at(id).target.sigmaD.setMetresValue(sigma(0));
	}
	else if (type == "ECHO")
	{
		measRefs.ECHO.at(id).target.sigmaD.setMetresValue(sigma(0));
	}
	else if (type == "ECSP")
	{
		measRefs.ECSP.at(id).target.sigmaD.setMetresValue(sigma(0));
	}
	else if (type == "ECVE")
	{
		measRefs.ECVE.at(id).target.sigmaD.setMetresValue(sigma(0));
	}
	else if (type == "INCLY")
	{
		measRefs.INCLY.at(id).target.sigmaAngl.setRadiansValue(sigma(0));
	}
	else if (type == "ECWS")
	{
		measRefs.ECWS.at(id).target.sigmaDist.setMetresValue(sigma(0));
	}
	else if (type == "ECWI")
	{	
		measRefs.ECWI.at(id).target.sigmaX.setMetresValue(sigma(0));
		measRefs.ECWI.at(id).target.sigmaZ.setMetresValue(sigma(1));
	}
	else if (type == "DVER")
	{
		measRefs.DVER.at(id).setObservedStDev(TLength(sigma(0)));
	}
	else if (type == "RADI")
	{
		measRefs.RADI.at(id).setObservedStDev(TLength(sigma(0)));
	}
	else if (type == "OBSXYZ")
	{
		measRefs.OBSXYZ.at(id).setXObservedStDev(TLength(sigma(0)));
		measRefs.OBSXYZ.at(id).setYObservedStDev(TLength(sigma(1)));
		measRefs.OBSXYZ.at(id).setZObservedStDev(TLength(sigma(2)));
	}
}



// get estimate
Eigen::VectorXd Moni::MoniImpl::getPointEstimate(std::string pointId)
{
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		std::cout << "No point with Id " << pointId << " found" << std::endl;
	}
	TPositionVector result = paramRefs.POINTS.at(pointId).getEstimatedValue();
	return toVectorXd(result);
}
Eigen::VectorXd Moni::MoniImpl::getPointEstimate(std::string pointId, std::string destFrame)
{
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		std::cout << "No point with Id " << pointId << " found" << std::endl;
	}
	if (paramRefs.FRAMES.count(destFrame) == 0)
	{
		std::cout << "No frame with Id " << destFrame << " found" << std::endl;
	}

	// transform to destination frame
	TPositionVector point = paramRefs.POINTS.at(pointId).getEstimatedValue();	
	const TLOR2LOR lorTrafo(paramRefs.POINTS.at(pointId).getFrameTreePosition(), project->getTree().begin(), "sub2Dest");
	lorTrafo.transform(point);
	return toVectorXd(point);
}

Eigen::VectorXd Moni::MoniImpl::getFrameEstimate(std::string frameId)
{
	if (paramRefs.FRAMES.count(frameId) == 0)
	{
		std::cout << "No frame with Id " << frameId << " found" << std::endl;
	}

	Eigen::VectorXd resultVector(7);
	resultVector[0] = (double)paramRefs.FRAMES.at(frameId).getEstParam().tX;
	resultVector[1] = (double)paramRefs.FRAMES.at(frameId).getEstParam().tY;
	resultVector[2] = (double)paramRefs.FRAMES.at(frameId).getEstParam().tZ;
	resultVector[3] = (double)paramRefs.FRAMES.at(frameId).getEstParam().omega;
	resultVector[4] = (double)paramRefs.FRAMES.at(frameId).getEstParam().phi;
	resultVector[5] = (double)paramRefs.FRAMES.at(frameId).getEstParam().kappa;
	resultVector[6] = (double)paramRefs.FRAMES.at(frameId).getEstParam().scale;

	return resultVector;
}

Eigen::VectorXd Moni::MoniImpl::getFrameEstimatePrec(std::string frameId)
{
	if (paramRefs.FRAMES.count(frameId) == 0)
	{
		std::cout << "No frame with Id " << frameId << " found" << std::endl;
	}

	Eigen::VectorXd resultVector(7);
	TAdjustableHelmertTransformation& aux = paramRefs.FRAMES.at(frameId);
	resultVector[0] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionTransl(0);
	resultVector[1] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionTransl(1);
	resultVector[2] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionTransl(2);
	resultVector[3] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionRot(0);
	resultVector[4] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionRot(1);
	resultVector[5] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionRot(2);
	resultVector[6] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionScale();

	return resultVector;
}



Eigen::VectorXd Moni::MoniImpl::getPointEstimatePrec(std::string pointId)
{
	// Only Points are implemented for now, will give the sigmas in the frame where the point is declared
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		std::cout << "No point with Id " << pointId << " found" << std::endl;
	}
	Eigen::VectorXd prec(3);
	prec.setZero();
	// get precisions, the diagonal covar elements are the square roots
	prec << (double)paramRefs.POINTS.at(pointId).getXEstPrecision(), (double)paramRefs.POINTS.at(pointId).getYEstPrecision(),
		(double)paramRefs.POINTS.at(pointId).getZEstPrecision();

	return prec;
}

Eigen::VectorXd Moni::MoniImpl::getPointEstimatePrec(std::string pointId, std::string destFrame)
{	
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		std::cout << "No point with Id " << pointId << " found" << std::endl;
	}
	if (paramRefs.FRAMES.count(destFrame) == 0)
	{
		std::cout << "No frame with Id " << destFrame << " found" << std::endl;
	}

	LGCAdjustablePoint point = paramRefs.POINTS.at(pointId);
	Eigen::VectorXd prec = toVectorXd(point.transformSigma(point, project.get(),destFrame));

	return prec;
}
