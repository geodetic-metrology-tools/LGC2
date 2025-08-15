#include "MoniImpl.h"

#include <iostream>

#include "Moni.h"
#include "TAStreamFormatter.h"

template<typename AdjustableObject>
bool isFreeVar(const AdjustableObject &adjObj, int idx)
{
	std::vector<int> freeIdx = adjObj.getRelativeUnknIndices();
	return std::find(freeIdx.begin(), freeIdx.end(), idx) != freeIdx.end();
}


// constructor
Moni::Moni(const std::string &inputFilePath) : pimpl_(std::make_unique<MoniImpl>(inputFilePath)), fFilePath(inputFilePath)
{
}
Moni::~Moni() = default;

DECLSPEC void Moni::reset()
{
	// full internal reset, calling implementation constructor, assuming initialization fiole still exists
	pimpl_ = std::make_unique<MoniImpl>(fFilePath);
}

// void Moni::writeJsonFile(TLGCData const *const dat, const std::string &outputFileLocation)

#if USE_SERIALIZER
void Moni::writeResultFile()
{
	pimpl_->writeResultFile();
}
#endif

void Moni::writeLGCInputFile()
{
	pimpl_->writeLGCInputFile();
}

void Moni::updateMeas(const std::string &id, const Eigen::VectorXd &measurementVector)
{
	pimpl_->updateMeas(id, measurementVector);
}

void Moni::setActivationStatus(const std::string &id, bool status)
{
	pimpl_->setActivationStatus(id, status);
}

void Moni::setObsSigma(const std::string &id, const Eigen::VectorXd &sigma)
{
	pimpl_->setObsSigma(id, sigma);
}

void Moni::setFixedFrameParameter(const std::string &frameName, int idx, double val)
{
	pimpl_->setFixedFrameParameter(frameName, idx, val);
}
void Moni::setFixedPointParameter(const std::string &frameName, int idx, double val)
{
	pimpl_->setFixedPointParameter(frameName, idx, val);
}

void Moni::setFixedSagParameter(const std::string &sagName, int idx, double val)
{
	pimpl_->setFixedSagParameter(sagName, idx, val);
}

void Moni::freezeFrameParameter(const std::string &frameName, int idx, double val)
{
	pimpl_->freezeFrameParameter(frameName, idx, val);
}

void Moni::unfreezeFrameParameter(const std::string &frameName, int idx)
{
	pimpl_->unfreezeFrameParameter(frameName, idx);
}
void Moni::freezePointParameter(const std::string &pointName, int idx, double val)
{
	pimpl_->freezePointParameter(pointName, idx, val);
}

void Moni::freezeSagParameter(const std::string &sagName, int idx, double val)
{
	pimpl_->freezeSagParameter(sagName, idx, val);
}

void Moni::unfreezePointParameter(const std::string &pointName, int idx)
{
	pimpl_->unfreezePointParameter(pointName, idx);
}

void Moni::unfreezeSagParameter(const std::string &sagName, int idx)
{
	pimpl_->unfreezeSagParameter(sagName, idx);
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
Eigen::VectorXd Moni::getPointEstimate(const std::string &pointId)
{
	return pimpl_->getPointEstimate(pointId);
}
// get estimate of frame
Eigen::VectorXd Moni::getFrameEstimate(const std::string &frameId)
{
	return pimpl_->getFrameEstimate(frameId);
}
Eigen::VectorXd Moni::getSagEstimate(const std::string &sagId)
{
	return pimpl_->getSagEstimate(sagId);
}
Eigen::VectorXd Moni::getFrameEstimatePrec(const std::string &frameId)
{
	return pimpl_->getFrameEstimatePrec(frameId);
}

Eigen::VectorXd Moni::getSagEstimatePrec(const std::string &sagId)
{
	return pimpl_->getSagEstimatePrec(sagId);
}

// get estimate of point in subframe
Eigen::VectorXd Moni::getPointEstimate(const std::string &id, const std::string &frameName)
{
	return pimpl_->getPointEstimate(id, frameName);
}
// get diagonal elements of covariances of the estimated parameters
Eigen::VectorXd Moni::getPointEstimatePrec(const std::string &id)
{
	return pimpl_->getPointEstimatePrec(id);
}
// get diagonal elements of covariances of the estimated parameters transformed to a subframe
Eigen::VectorXd Moni::getPointEstimatePrec(const std::string &id, const std::string &frameName)
{
	return pimpl_->getPointEstimatePrec(id, frameName);
}
// get estimated residual
Eigen::VectorXd Moni::getEstimateResidual(const std::string &obsId)
{
	return pimpl_->getEstimateResidual(obsId);
}

// get calculated measurement
Eigen::VectorXd Moni::getCalcMeas(const std::string &obsId)
{
	return pimpl_->getCalcMeas(obsId);
}

DECLSPEC Eigen::VectorXd Moni::getObsSigma(const std::string &obsId)
{
	return pimpl_->getObsSigma(obsId);
}

// get Meas IDs
std::vector<std::string> Moni::getECWSMeasIds()
{
	return pimpl_->getECWSMeasIds();
}
Eigen::VectorXd Moni::getMeas(const std::string &id)
{
	return pimpl_->getMeas(id);
}
// get the sigma0 after adjustment
double Moni::getSigma0()
{
	return pimpl_->getSigma0();
}
// transform coordinates/directions
DECLSPEC Eigen::Vector3d Moni::transformCoordinates(const Eigen::Vector3d &coord, const std::string &from, const std::string &to)
{
	return pimpl_->transformCoordinates(coord, from, to);
};
DECLSPEC Eigen::Vector3d Moni::transformDirection(const Eigen::Vector3d &dir, const std::string &from, const std::string &to)
{
	return pimpl_->transformDirection(dir, from, to);
};
waterRom Moni::getECWSData(const std::string &ecwsRomName)
{
	return pimpl_->getECWSData(ecwsRomName);
}

DECLSPEC wireRom Moni::getECWIData(const std::string &ecwiRomName)
{
	return pimpl_->getECWIData(ecwiRomName);
}

// actual Implementation
void Moni::MoniImpl::initialize()
{
	estimationStatus = false;
	Behavior successCalculation;
	std::cout << "Creating monitoring object with LGC input file " << inputFilePath << std::endl;
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

#if USE_SERIALIZER
void Moni::MoniImpl::writeResultFile()
{
	jsonSerializerObject ser;
	SerializerObject::SerializationHelper obj = ser.getSerializationHelper();
	obj.addProperty("LGC_DATA", project.get());
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream filename;
	filename << "resultDump_" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H_%M_%S_%S") << ".json";
	std::ofstream fout(filename.str());
	fout << ser.getStringRepresentation();
}
#endif

void Moni::MoniImpl::writeLGCInputFile()
{
	// Create and initialise stream:
	std::shared_ptr<TAStreamFormatter> stream;
	TFileParameters resultFileParam;
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream filename;
	filename << "inputDump_" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%dT%H_%M_%S_%S") << ".lgc";

	resultFileParam.setFileName(filename.str());

	// Some keywords(options) in the input file responsible for this, for now just setting here one of them (column), but can be semi-colon, dash etc.
	TAStreamFormatter::ETextFormat resultsFileFormat = TAStreamFormatter::ETextFormat::kColumnFormat;

	TStreamFormatterFactory *formatterFactory = TStreamFormatterFactory::instance();
	TDataParameters dataParam;
	dataParam.setRefFrame(TRefSystemFactory::ERefFrame::kCCS); // default param because not redefine
	dataParam.setPrecision(project->getConfig().outPrecision.digits);
	dataParam.setObsIdWidth(project->getConfig().obsIDwidth);

	TADataSet tads(resultFileParam, dataParam);
	stream.reset(formatterFactory->getFormatter(&tads, resultsFileFormat, "   " /* separator */));
	stream->setReferenceFrame(dataParam.getRefFrame()); // default param because not redefine
	stream->setCoordSys(TCoordSysFactory::k3DCartesian);

	TPointFormat *pointFormat = stream->getPointFormat();
	// Set the name width to the max width of a name point +1 characters.
	if (project->getConfig().pointNameWidth > pointFormat->getNameWidth())
	{
		pointFormat->setNameWidth(project->getConfig().pointNameWidth + 1);
		stream->setPointFormat(*pointFormat);
	}
	// As the name width is used also for frames. Set the name width to the max of of frame name +1 characters, if necessary.
	for (const auto &itTree : project->getTree())
	{
		if (itTree.get()->frame.getName().length() >= pointFormat->getNameWidth())
		{
			pointFormat->setNameWidth((int)itTree.get()->frame.getName().length() + 1);
			stream->setPointFormat(*pointFormat);
		}
	}
	// Create writer, write the file:
	TInputFileWriter infileWriter(stream.get(), project.get());
	infileWriter.writeFile();
}
void Moni::MoniImpl::setFixedFrameParameter(const std::string &frameName, int idx, double val)
{
	// set a fixed frame parameter (for example scale of Frame which is determined by temperature)
	if (paramRefs.FRAMES.count(frameName) == 0)
	{
		throw std::runtime_error("Frame " + frameName + " does not exist.");
	}

	// this method will change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the frame is really fixed
	TAdjustableHelmertTransformation &frameRef = paramRefs.FRAMES.at(frameName);
	bool isFree = isFreeVar(frameRef, idx);
	if (isFree)
	{
		throw std::runtime_error("Index " + std::to_string(idx) + " of frame " + frameName + " is not a fixed variable. ");
	}


	// set the parameter
	if (idx < 3)
	{
		frameRef.setTranslation(idx, TLength(val));
	}
	else if (idx < 6)
	{
		frameRef.setRotation(idx - 3, TAngle(val));
	}
	else if (idx == 6)
	{
		frameRef.setScale(TReal(val));
	}
}

void Moni::MoniImpl::setFixedPointParameter(const std::string &pointName, int idx, double val)
{
	// set a fixed point parameter
	if (paramRefs.POINTS.count(pointName) == 0)
	{
		throw std::runtime_error("Point " + pointName + " does not exist.");
	}

	// this method will change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the frame is really fixed
	TAdjustablePoint &pointRef = paramRefs.POINTS.at(pointName);
	bool isFixed = pointRef.isCoordinateFixed(idx);
	if (!isFixed)
	{
		throw std::runtime_error("Index " + std::to_string(idx) + " of point " + pointName + " is not a fixed variable. ");
	}

	pointRef.setEstVal(idx, val);
	
}

void Moni::MoniImpl::setFixedSagParameter(const std::string &sagName, int idx, double val)
{
	if (idx<0 || idx>3)
	{
		throw std::runtime_error("Index of sag element " + sagName + " has to be 0,1,2 or 3.");
	}
	// set a fixed sag parameter
	if (paramRefs.SAGS.count(sagName) == 0)
	{
		throw std::runtime_error("Sag element " + sagName + " does not exist.");
	}

	// this method will change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the frame is really fixed
	LGCAdjustableSag &sagRef = paramRefs.SAGS.at(sagName);
	// attention: for the user onyl variable 1,2,3,4 are visible, the bearing (0) is always free
	bool isFree = isFreeVar(sagRef, idx + 1);
	if (isFree)
	{
		throw std::runtime_error("Index " + std::to_string(idx) + " of sag element " + sagName + " is not a fixed variable. ");
	}

	sagRef.setEstValue(idx+1, val);

}

void Moni::MoniImpl::freezeFrameParameter(const std::string &frameName, int idx, double val)
{
	// freeze a frame parameter that is free in the original configuration
	if (paramRefs.FRAMES.count(frameName) == 0)
	{
		throw std::runtime_error("Frame " + frameName + " does not exist.");
	}

	// this method will potentially change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the frame is really free
	TAdjustableHelmertTransformation &frameRef = paramRefs.FRAMES.at(frameName);
	bool isFixed = !isFreeVar(frameRef, idx);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of frame " + frameName + " is not a free variable. Nothing to freeze!");
	}

	// freeze the parameter
	if (idx < 3)
	{
		frameRef.setTranslation(idx, TLength(val));
		project->fParameterMask.insert(frameRef.getTranslationUnknIndex(idx));
	}
	else if (idx < 6)
	{
		frameRef.setRotation(idx - 3, TAngle(val));
		project->fParameterMask.insert(frameRef.getRotationUnknIndex(idx - 3));
	}
	else if (idx == 6)
	{
		frameRef.setScale(TReal(val));
		project->fParameterMask.insert(frameRef.getScaleUnknIndex());
	}
}

void Moni::MoniImpl::unfreezeFrameParameter(const std::string &frameName, int idx)
{
	// freeze a frame parameter that is free in the original configuration
	if (paramRefs.FRAMES.count(frameName) == 0)
	{
		throw std::runtime_error("Frame " + frameName + " does not exist.");
	}

	// this method will potentially change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the frame is really free
	TAdjustableHelmertTransformation &frameRef = paramRefs.FRAMES.at(frameName);
	bool isFixed = !isFreeVar(frameRef, idx);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of frame " + frameName + " is not a free variable. Nothing to unfreeze!");
	}

	// get the index of the variable
	int frozenIndex = -1;
	if (idx < 3)
	{
		frozenIndex = frameRef.getTranslationUnknIndex(idx);
	}
	else if (idx < 6)
	{
		frozenIndex = frameRef.getRotationUnknIndex(idx - 3);
	}
	else if (idx == 6)
	{
		frozenIndex = frameRef.getScaleUnknIndex();
	}
	// remove this index from the masked parameter indices

	// Find the iterator to the element
	auto it = std::find(project->fParameterMask.begin(), project->fParameterMask.end(), frozenIndex);

	// Check if the element was found in the parameter mask
	if (it != project->fParameterMask.end())
	{
		// Erase the element
		project->fParameterMask.erase(it);
	}
	else
	{
		throw std::runtime_error("Index " + std::to_string(idx) + " of frame " + frameName + " was not frozen. Nothing to unfreeze!");
	}
}

void Moni::MoniImpl::freezePointParameter(const std::string &pointName, int idx, double val)
{
	// freeze a frame parameter that is free in the original configuration
	if (paramRefs.POINTS.count(pointName) == 0)
	{
		throw std::runtime_error("Point " + pointName + " does not exist.");
	}

	// this method will potentially change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the frame is really free
	LGCAdjustablePoint &pointRef = paramRefs.POINTS.at(pointName);
	bool isFixed = pointRef.isCoordinateFixed(idx);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of point " + pointName + " is not a free variable. Nothing to freeze!");
	}

	// freeze the parameter
	//pointRef.setEstVal(idx, TLength(val));
	pointRef.setValue(pointRef.getCoordinateUnknIndex(idx), val);
	project->fParameterMask.insert(pointRef.getCoordinateUnknIndex(idx));
}

void Moni::MoniImpl::freezeSagParameter(const std::string &sagName, int idx, double val)
{
	if (idx<0 || idx>3)
	{
		throw std::runtime_error("Index of sag element " + sagName + " has to be 0,1,2 or 3.");
	}
	// freeze a sag parameter that is free in the original configuration
	if (paramRefs.SAGS.count(sagName) == 0)
	{
		throw std::runtime_error("Sag element" + sagName + " does not exist.");
	}

	// this method will potentially change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the frame is really free
	LGCAdjustableSag &sagRef = paramRefs.SAGS.at(sagName);
	// +1 because bearing is hidden in interface
	bool isFixed = !isFreeVar(sagRef, idx + 1);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of sag element " + sagName + " is not a free variable. Nothing to freeze!");
	}

	// freeze the parameter
	sagRef.setEstValue(idx + 1, val);
	project->fParameterMask.insert(sagRef.getUnknIndex(idx+1));

}

void Moni::MoniImpl::unfreezePointParameter(const std::string &pointName, int idx)
{
	// freeze a point parameter that is free in the original configuration
	if (paramRefs.POINTS.count(pointName) == 0)
	{
		throw std::runtime_error("Point " + pointName + " does not exist.");
	}

	// this method will potentially change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the point is really free
	LGCAdjustablePoint &pointRef = paramRefs.POINTS.at(pointName);
	bool isFixed = pointRef.isCoordinateFixed(idx);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of point " + pointName + " is not a free variable. Nothing to unfreeze!");
	}

	// get the index of the variable
	int frozenIndex = -1;
	frozenIndex = pointRef.getCoordinateUnknIndex(idx);
	// remove this index from the masked parameter indices

	// Find the iterator to the element
	auto it = std::find(project->fParameterMask.begin(), project->fParameterMask.end(), frozenIndex);

	// Check if the element was found in the parameter mask
	if (it != project->fParameterMask.end())
	{
		// Erase the element
		project->fParameterMask.erase(it);
	}
	else
	{
		throw std::runtime_error("Index " + std::to_string(idx) + " of point " + pointName + " was not frozen. Nothing to unfreeze!");
	}
}

void Moni::MoniImpl::unfreezeSagParameter(const std::string &sagName, int idx)
{	// unfreeze a sag parameter that is free in the original configuration
	if (idx<0 || idx>3)
	{
		throw std::runtime_error("Index of sag element " + sagName + " has to be 0,1,2 or 3.");
	}
	if (paramRefs.SAGS.count(sagName) == 0)
	{
		throw std::runtime_error("Sag element " + sagName + " does not exist.");
	}

	// this method will potentially change the solution of the estimation, so reset of status necessary
	estimationStatus = false;

	// check if the associated parameter of the point is really free
	LGCAdjustableSag &sagRef = paramRefs.SAGS.at(sagName);
	bool isFixed = !isFreeVar(sagRef, idx + 1);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of sag element " + sagName + " is not a free variable. Nothing to unfreeze!");
	}

	// get the index of the variable
	int frozenIndex = -1;
	frozenIndex = sagRef.getUnknIndex(idx + 1);
	// remove this index from the masked parameter indices

	// Find the iterator to the element
	auto it = std::find(project->fParameterMask.begin(), project->fParameterMask.end(), frozenIndex);

	// Check if the element was found in the parameter mask
	if (it != project->fParameterMask.end())
	{
		// Erase the element
		project->fParameterMask.erase(it);
	}
	else
	{
		throw std::runtime_error("Index " + std::to_string(idx) + " of sag element " + sagName + " was not frozen. Nothing to unfreeze!");
	}

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
	for (auto &object : project.get()->getSags())
	{
		paramRefs.SAGS.insert({object.getName(), object});
	}

	// now the unknowns associated to transformations.. (as in TLSResultsMatricesExtractor::extractTransformationParams)
	// as there is no "adjustable transformation collection", we have to iterate over the tree and get them on our own.
	for (auto it(project.get()->getTree().begin()); it != project.get()->getTree().end(); ++it)
	{
		auto &object(it.node->data.get()->frame);
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
			romRefs.ecwsRoms.insert({itECWSrom.romName, itECWSrom});
			for (auto &itECWS : itECWSrom.measECWS)
			{
				measRefs.ECWS.insert({itECWS.obsID, itECWS});
				measRefs.types.insert({itECWS.obsID, "ECWS"});
			}
		}
		for (auto &itECWIrom : itTree.node->data->measurements.fECWI)
		{
			romRefs.ecwiRoms.insert({itECWIrom.romName, itECWIrom});
			for (auto &itECWI : itECWIrom.measECWI)
			{
				measRefs.ECWI.insert({itECWI.obsID, itECWI});
				measRefs.types.insert({itECWI.obsID, "ECWI"});
			}
		}
	}
}

TStatusObject &Moni::MoniImpl::getStatusObject(const std::string &id)
{
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		throw std::runtime_error("No measurement with ID " + id + " found.");
	}

	std::string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		return measRefs.ANGL.at(id);
	}
	else if (type == "ZEND")
	{
		return measRefs.ZEND.at(id);
	}
	else if (type == "DIST")
	{
		return measRefs.DIST.at(id);
	}
	else if (type == "ECTH")
	{
		return measRefs.ECTH.at(id);
	}
	else if (type == "ECDIR")
	{
		return measRefs.ECDIR.at(id);
	}
	else if (type == "DHOR")
	{
		return measRefs.DHOR.at(id);
	}
	else if (type == "PLR3D")
	{
		return measRefs.PLR3D.at(id);
	}
	else if (type == "ORIE")
	{
		return measRefs.ORIE.at(id);
	}
	else if (type == "UVEC")
	{
		return measRefs.UVEC.at(id);
	}
	else if (type == "UVD")
	{
		return measRefs.UVD.at(id);
	}
	else if (type == "DSPT")
	{
		return measRefs.DSPT.at(id);
	}
	else if (type == "DLEV")
	{
		return measRefs.DLEV.at(id);
	}
	else if (type == "ECHO")
	{
		return measRefs.ECHO.at(id);
	}
	else if (type == "ECSP")
	{
		return measRefs.ECSP.at(id);
	}
	else if (type == "ECVE")
	{
		return measRefs.ECVE.at(id);
	}
	else if (type == "INCLY")
	{
		return measRefs.INCLY.at(id);
	}
	else if (type == "ECWS")
	{
		return measRefs.ECWS.at(id);
	}
	else if (type == "ECWI")
	{
		return measRefs.ECWI.at(id);
	}
	else if (type == "DVER")
	{
		return measRefs.DVER.at(id);
	}
	else if (type == "RADI")
	{
		return measRefs.RADI.at(id);
	}
	else if (type == "OBSXYZ")
	{
		return measRefs.OBSXYZ.at(id);
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

void Moni::MoniImpl::updateMeas(const std::string &id, const Eigen::VectorXd &measurementVector)
{
	// reset estimationStatus to false as soon as new measurement data is supplied
	estimationStatus = false;
	// manipulate the corresponding measurement by accessing it via the reference map.
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		throw std::runtime_error("No measurement with ID " + id + " found.");
		return;
	}

	std::string type = measRefs.types.at(id);
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
		//TPositionVector obsVector(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::ECoordSys::k3DCartesian);
		// using a setter method for obsxyz
		//measRefs.OBSXYZ.at(id).setObservedVector(obsVector);
		measRefs.OBSXYZ.at(id).setObsVector(measurementVector);
		return;
	}
}
void Moni::MoniImpl::setActivationStatus(const std::string &id, bool status)
{
	estimationStatus = false;
	getStatusObject(id).setActive(status);
}
Eigen::VectorXd Moni::MoniImpl::getEstimateResidual(const std::string &id)
{
	// manipulate the corresponding measurement by accesing it via the reference map.
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		Eigen::VectorXd res(1);
		res << -1;
		throw std::runtime_error("No measurement with ID " + id + " found.");
		return res;
	}

	std::string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		Eigen::VectorXd res(1);
		res << (double)measRefs.ANGL.at(id).getAngleResidual();
		return res;
	}
	else if (type == "ZEND")
	{
		Eigen::VectorXd res(1);
		res << (double)measRefs.ZEND.at(id).getAngleResidual();
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
		// TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		// measRefs.UVEC.at(id).setVectorMeasurement(direction);
		return res;
	}
	else if (type == "UVD")
	{
		// to be checked, particular the order!
		Eigen::VectorXd res(3);
		res << measRefs.UVD.at(id).getXCompVectorResidual(), measRefs.UVD.at(id).getYCompVectorResidual(), measRefs.UVD.at(id).getDistanceResidual();
		// TFreeVector direction(measurementVector[0], measurementVector[1], measurementVector[2], TCoordSysFactory::k3DCartesian);
		// TLength distance(measurementVector[3]);
		// measRefs.UVD.at(id).setVectorMeasurement(direction);
		// measRefs.UVD.at(id).setDistance(distance);
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
	successCalculation = algorithm->run(*project.get(), 80);
	if (successCalculation)
	{
		estimationStatus = true;
	}
	else
	{
		throw std::runtime_error("Problems occured during Adjustment method.");
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
Eigen::VectorXd Moni::MoniImpl::getObsSigma(const std::string &id)
{
	// get observation sigma
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		throw std::runtime_error("No measurement with ID " + id + " found.");
		Eigen::VectorXd result(1);
		result[0] = 0;
		return result;
	}

	std::string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ANGL.at(id).target.sigmaAngl.getRadiansValue());
		return result;
	}
	else if (type == "ZEND")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ZEND.at(id).target.sigmaZenD.getRadiansValue());
		return result;
	}
	else if (type == "DIST")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.DIST.at(id).target.sigmaDist.getMetresValue());
		return result;
	}
	else if (type == "ECTH")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ECTH.at(id).target.sigmaD.getMetresValue());
		return result;
	}
	else if (type == "ECDIR")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ECDIR.at(id).target.sigmaD.getMetresValue());
		return result;
	}
	else if (type == "DHOR")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.DHOR.at(id).target.sigmaDist.getMetresValue());
		return result;
	}
	else if (type == "PLR3D")
	{
		Eigen::VectorXd result(3);
		result << measRefs.PLR3D.at(id).target.sigmaAngl.getRadiansValue(), measRefs.PLR3D.at(id).target.sigmaZenD.getRadiansValue(),
			measRefs.PLR3D.at(id).target.sigmaDist.getMetresValue();
		return result;
	}
	else if (type == "ORIE")
	{
		Eigen::VectorXd result(1);
		result << measRefs.ORIE.at(id).target.sigmaAngl.getRadiansValue();
		return result;
	}
	else if (type == "UVEC")
	{
		Eigen::VectorXd result(2);
		result << measRefs.UVEC.at(id).target.sigmaX, measRefs.UVEC.at(id).target.sigmaY;
		return result;
	}
	else if (type == "UVD")
	{
		Eigen::VectorXd result(3);
		result << measRefs.UVD.at(id).target.sigmaX, measRefs.UVD.at(id).target.sigmaY, measRefs.UVD.at(id).target.sigmaDist.getMetresValue();
		return result;
	}
	else if (type == "DSPT")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.DSPT.at(id).target.sigmaDSpt.getMetresValue();
		return result;
	}
	else if (type == "DLEV")
	{
		// ignoring DHOR
		Eigen::VectorXd result(1);
		result[0] = measRefs.DLEV.at(id).target.sigmaD.getMetresValue();
		return result;
	}
	else if (type == "ECHO")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECHO.at(id).target.sigmaD.getMetresValue();
		return result;
	}
	else if (type == "ECSP")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECSP.at(id).target.sigmaD.getMetresValue();
		return result;
	}
	else if (type == "ECVE")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECVE.at(id).target.sigmaD.getMetresValue();
		return result;
	}
	else if (type == "INCLY")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.INCLY.at(id).target.sigmaAngl.getRadiansValue();
		return result;
	}
	else if (type == "ECWS")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.ECWS.at(id).target.sigmaDist.getMetresValue();
		return result;
	}
	else if (type == "ECWI")
	{
		Eigen::VectorXd result(2);
		result << measRefs.ECWI.at(id).target.sigmaX.getMetresValue(), measRefs.ECWI.at(id).target.sigmaZ.getMetresValue();
		return result;
	}
	else if (type == "DVER")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.DVER.at(id).getObservedStDev().getMetresValue();
		return result;
	}
	else if (type == "RADI")
	{
		Eigen::VectorXd result(1);
		result[0] = measRefs.RADI.at(id).getObservedStDev().getMetresValue();
		return result;
	}
	else if (type == "OBSXYZ")
	{
		Eigen::VectorXd result(3);
		result << measRefs.OBSXYZ.at(id).getXObservedStDev().getMetresValue(), measRefs.OBSXYZ.at(id).getYObservedStDev().getMetresValue(),
			measRefs.OBSXYZ.at(id).getZObservedStDev().getMetresValue();
		return result;
	}
}
double Moni::MoniImpl::getSigma0()
{
	return project->getS0APosteriori();
}
Eigen::Vector3d Moni::MoniImpl::transformCoordinates(const Eigen::Vector3d &coord, const std::string &from, const std::string &to)
{
	if (paramRefs.FRAMES.count(from) == 0)
	{
		throw std::runtime_error("Source Frame with Id " + from + " found");
	}
	if (paramRefs.FRAMES.count(to) == 0)
	{
		throw std::runtime_error("Destination Frame with Id " + to + " found");
	}

	TPositionVector temp(TVector(coord), TCoordSysFactory::k3DCartesian);
	const TLOR2LOR lorTrafo(project->locateNode(from), project->locateNode(to), "fromTo");
	lorTrafo.transform(temp);
	return temp.toRealVector();
}
Eigen::Vector3d Moni::MoniImpl::transformDirection(const Eigen::Vector3d &dir, const std::string &from, const std::string &to)
{
	return Moni::MoniImpl::transformCoordinates(dir, from, to) - Moni::MoniImpl::transformCoordinates(Eigen::Vector3d(0, 0, 0), from, to);
}
waterRom Moni::MoniImpl::getECWSData(const std::string &ecwsRomName)
{
	// find the ecws rom reference
	auto it = romRefs.ecwsRoms.find(ecwsRomName);
	if (it != romRefs.ecwsRoms.end())
	{
		TECWSROM &rom = romRefs.ecwsRoms.at(ecwsRomName);
		waterRom result(rom.romName, double(rom.fMeasuredWSHeight->getEstimatedValue()), double(rom.fMeasuredWSHeight->getEstimatedPrecision()));
		return result;
	}
	else
	{
		throw std::runtime_error("No ECWS round of measurements named " + ecwsRomName + " found.");
	}
}
wireRom Moni::MoniImpl::getECWIData(const std::string &ecwiRomName)
{
	// find the ecwi rom reference
	auto it = romRefs.ecwiRoms.find(ecwiRomName);
	if (it != romRefs.ecwiRoms.end())
	{
		TECWIROM &rom = romRefs.ecwiRoms.at(ecwiRomName);
		Eigen::VectorXd data(5), sigmas(5);
		data << double(rom.fWireDx->getEstimatedValue()), double(rom.fWireDz->getEstimatedValue()), double(rom.fWireBearing->getEstimatedValue()),
			double(rom.fWireSlope->getEstimatedValue()), double(rom.sagAdjustable->getEstimatedValue());
		sigmas << double(rom.fWireDx->getEstimatedPrecision()), double(rom.fWireDz->getEstimatedPrecision()), double(rom.fWireBearing->getEstimatedPrecision()),
			double(rom.fWireSlope->getEstimatedPrecision()), double(rom.sagAdjustable->getEstimatedPrecision());
		auto &sagAdj = rom.sagAdjustable;

		return wireRom(ecwiRomName, data, sigmas);
	}
	else
	{
		throw std::runtime_error("No ecwi round of measurements named " + ecwiRomName + " found.");
	}
}
// get measurement
Eigen::VectorXd Moni::MoniImpl::getMeas(const std::string &id)
{
	// get observation value
	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		throw std::runtime_error("No measurement with ID " + id + " found.");
		Eigen::VectorXd result(1);
		result[0] = 0;
		return result;
	}

	std::string type = measRefs.types.at(id);
	if (type == "ANGL")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ANGL.at(id).getAngle());
		return result;
	}
	else if (type == "ZEND")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ZEND.at(id).getAngle());
		return result;
	}
	else if (type == "DIST")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.DIST.at(id).getDistance());
		return result;
	}
	else if (type == "ECTH")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ECTH.at(id).getDistance());
		return result;
	}
	else if (type == "ECDIR")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.ECDIR.at(id).getDistance());
		return result;
	}
	else if (type == "DHOR")
	{
		Eigen::VectorXd result(1);
		result[0] = ((double)measRefs.DHOR.at(id).getDistance());
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
		Eigen::VectorXd result(3);
		Eigen::VectorXd direction = toVectorXd(measRefs.UVD.at(id).getVectorValue());
		result << direction[0], direction[1], measRefs.UVD.at(id).getDistance().getMetresValue();
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
		result[0] = measRefs.INCLY.at(id).getAngle();
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

void Moni::MoniImpl::setObsSigma(const std::string &id, const Eigen::VectorXd &sigma)
{
	// reset estimationStatus to false as soon as new weights are provided
	estimationStatus = false;

	// check if id exists
	if (measRefs.types.count(id) == 0)
	{
		throw std::runtime_error("No measurement with ID " + id + " found.");
	}

	std::string type = measRefs.types.at(id);
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
		measRefs.ECTH.at(id).target.sigmaD.setMetresValue(sigma(0));
	}
	else if (type == "ECDIR")
	{
		measRefs.ECDIR.at(id).target.sigmaD.setMetresValue(sigma(0));
	}
	else if (type == "DHOR")
	{
		measRefs.DHOR.at(id).target.sigmaDist.setMetresValue(TLength(sigma(0)));
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
Eigen::VectorXd Moni::MoniImpl::getPointEstimate(const std::string &pointId)
{
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		throw std::runtime_error("No point with Id " + pointId + " found");
	}
	TPositionVector result = paramRefs.POINTS.at(pointId).getEstimatedValue();
	return toVectorXd(result);
}
Eigen::VectorXd Moni::MoniImpl::getPointEstimate(const std::string &pointId, const std::string &destFrame)
{
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		throw std::runtime_error("No point with Id " + pointId + " found");
	}
	if (paramRefs.FRAMES.count(destFrame) == 0)
	{
		throw std::runtime_error("No Frame with Id " + destFrame + " found");
	}

	// transform to destination frame
	TPositionVector point = paramRefs.POINTS.at(pointId).getEstimatedValue();
	const TLOR2LOR lorTrafo(paramRefs.POINTS.at(pointId).getFrameTreePosition(), project->locateNode(destFrame), "sub2Dest");
	lorTrafo.transform(point);
	return toVectorXd(point);
}

Eigen::VectorXd Moni::MoniImpl::getFrameEstimate(const std::string &frameId)
{
	if (paramRefs.FRAMES.count(frameId) == 0)
	{
		throw std::runtime_error("No Frame with Id " + frameId + " found");
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

Eigen::VectorXd Moni::MoniImpl::getSagEstimate(const std::string &sagId)
{	
	if (paramRefs.SAGS.count(sagId) == 0)
	{
		throw std::runtime_error("No Sag element with Id " + sagId + " found");
	}

	Eigen::VectorXd resultVector(4);
	// bearing (index 0 is not wanted)
	return paramRefs.SAGS.at(sagId).getEstVector().bottomRows(4);
}

Eigen::VectorXd Moni::MoniImpl::getFrameEstimatePrec(const std::string &frameId)
{
	if (paramRefs.FRAMES.count(frameId) == 0)
	{
		throw std::runtime_error("No Frame with Id " + frameId + " found");
	}

	Eigen::VectorXd resultVector(7);
	TAdjustableHelmertTransformation &aux = paramRefs.FRAMES.at(frameId);
	resultVector[0] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionTransl(0);
	resultVector[1] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionTransl(1);
	resultVector[2] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionTransl(2);
	resultVector[3] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionRot(0);
	resultVector[4] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionRot(1);
	resultVector[5] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionRot(2);
	resultVector[6] = (double)paramRefs.FRAMES.at(frameId).getEstimatedPrecisionScale();

	return resultVector;
}

Eigen::VectorXd Moni::MoniImpl::getSagEstimatePrec(const std::string &sagId)
{
	if (paramRefs.SAGS.count(sagId) == 0)
	{
		throw std::runtime_error("No Sag element with Id " + sagId + " found");
	}

	// bearing (index 0 is not wanted)
	Eigen::MatrixXd fullCovar = paramRefs.SAGS.at(sagId).getCovar().bottomRightCorner(4,4);

	return fullCovar.diagonal().array().sqrt();
}

Eigen::VectorXd Moni::MoniImpl::getPointEstimatePrec(const std::string &pointId)
{
	// Only Points are implemented for now, will give the sigmas in the frame where the point is declared
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		throw std::runtime_error("No Point with Id " + pointId + " found");
	}
	Eigen::VectorXd prec(3);
	prec.setZero();
	// get precisions, the diagonal covar elements are the square roots
	prec << (double)paramRefs.POINTS.at(pointId).getXEstPrecision(), (double)paramRefs.POINTS.at(pointId).getYEstPrecision(),
		(double)paramRefs.POINTS.at(pointId).getZEstPrecision();

	return prec;
}

Eigen::VectorXd Moni::MoniImpl::getPointEstimatePrec(const std::string &pointId, const std::string &destFrame)
{
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		throw std::runtime_error("No Point with Id " + pointId + " found");
	}
	if (paramRefs.FRAMES.count(destFrame) == 0)
	{
		throw std::runtime_error("No Point with Id " + destFrame + " found");
	}

	LGCAdjustablePoint point = paramRefs.POINTS.at(pointId);
	Eigen::VectorXd prec = toVectorXd(point.transformSigma(point, project.get(), destFrame));

	return prec;
}
