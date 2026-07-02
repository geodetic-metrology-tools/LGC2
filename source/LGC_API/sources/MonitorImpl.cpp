// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MonitorImpl.h"

#include <iostream>
#include <stdexcept>

#include "MonitorCore.h"
#include "TAStreamFormatter.h"
#include "Version.h"

template<typename AdjustableObject>
bool isFreeVar(const AdjustableObject &adjObj, int idx)
{
	std::vector<int> freeIdx = adjObj.getRelativeUnknIndices();
	return std::find(freeIdx.begin(), freeIdx.end(), idx) != freeIdx.end();
}


// constructor
Monitor::Monitor(const std::string &inputFilePath) : pimpl_(std::make_unique<MonitorImpl>(inputFilePath)), fFilePath(inputFilePath)
{
}
Monitor::~Monitor() = default;

DECLSPEC void Monitor::reset()
{
	// full internal reset, calling implementation constructor, assuming initialization fiole still exists
	pimpl_ = std::make_unique<MonitorImpl>(fFilePath);
}

#if USE_SERIALIZER
std::string Monitor::getResultsJson()
{
	return pimpl_->getResultsJson();
}
#endif

void Monitor::writeLGCInputFile()
{
	pimpl_->writeLGCInputFile();
}

void Monitor::updateMeas(const std::string &id, const Eigen::VectorXd &measurementVector)
{
	pimpl_->updateMeas(id, measurementVector);
}

void Monitor::setActivationStatus(const std::string &id, bool status)
{
	pimpl_->setActivationStatus(id, status);
}

DECLSPEC bool Monitor::getActivationStatus(const std::string &id)
{
	return pimpl_->getActivationStatus(id);
}

void Monitor::setObsSigma(const std::string &id, const Eigen::VectorXd &sigma)
{
	pimpl_->setObsSigma(id, sigma);
}

void Monitor::setFixedFrameParameter(const std::string &frameName, int idx, double val)
{
	pimpl_->setFixedFrameParameter(frameName, idx, val);
}
void Monitor::setFixedPointParameter(const std::string &frameName, int idx, double val)
{
	pimpl_->setFixedPointParameter(frameName, idx, val);
}

void Monitor::setFixedSagParameter(const std::string &sagName, int idx, double val)
{
	pimpl_->setFixedSagParameter(sagName, idx, val);
}

void Monitor::freezeFrameParameter(const std::string &frameName, int idx, double val)
{
	pimpl_->freezeFrameParameter(frameName, idx, val);
}

void Monitor::unfreezeFrameParameter(const std::string &frameName, int idx)
{
	pimpl_->unfreezeFrameParameter(frameName, idx);
}
void Monitor::freezePointParameter(const std::string &pointName, int idx, double val)
{
	pimpl_->freezePointParameter(pointName, idx, val);
}

void Monitor::freezeSagParameter(const std::string &sagName, int idx, double val)
{
	pimpl_->freezeSagParameter(sagName, idx, val);
}

void Monitor::unfreezePointParameter(const std::string &pointName, int idx)
{
	pimpl_->unfreezePointParameter(pointName, idx);
}

void Monitor::unfreezeSagParameter(const std::string &sagName, int idx)
{
	pimpl_->unfreezeSagParameter(sagName, idx);
}

// triggering the adjustment calculation
bool Monitor::adjust()
{
	return pimpl_->adjust();
}
bool Monitor::getStatus()
{
	return pimpl_->getStatus();
}
// get estimate of point
Eigen::VectorXd Monitor::getPointEstimate(const std::string &pointId)
{
	return pimpl_->getPointEstimate(pointId);
}
// get estimate of frame
Eigen::VectorXd Monitor::getFrameEstimate(const std::string &frameId)
{
	return pimpl_->getFrameEstimate(frameId);
}
Eigen::VectorXd Monitor::getSagEstimate(const std::string &sagId)
{
	return pimpl_->getSagEstimate(sagId);
}
Eigen::VectorXd Monitor::getFrameEstimatePrec(const std::string &frameId)
{
	return pimpl_->getFrameEstimatePrec(frameId);
}

Eigen::VectorXd Monitor::getSagEstimatePrec(const std::string &sagId)
{
	return pimpl_->getSagEstimatePrec(sagId);
}

// get estimate of point in subframe
Eigen::VectorXd Monitor::getPointEstimate(const std::string &id, const std::string &frameName)
{
	return pimpl_->getPointEstimate(id, frameName);
}
// get diagonal elements of covariances of the estimated parameters
Eigen::VectorXd Monitor::getPointEstimatePrec(const std::string &id)
{
	return pimpl_->getPointEstimatePrec(id);
}
// get diagonal elements of covariances of the estimated parameters transformed to a subframe
Eigen::VectorXd Monitor::getPointEstimatePrec(const std::string &id, const std::string &frameName)
{
	return pimpl_->getPointEstimatePrec(id, frameName);
}
// get estimated residual
Eigen::VectorXd Monitor::getEstimateResidual(const std::string &obsId)
{
	return pimpl_->getEstimateResidual(obsId);
}

// get calculated measurement
Eigen::VectorXd Monitor::getCalcMeas(const std::string &obsId)
{
	return pimpl_->getCalcMeas(obsId);
}

DECLSPEC Eigen::VectorXd Monitor::getObsSigma(const std::string &obsId)
{
	return pimpl_->getObsSigma(obsId);
}

Eigen::VectorXd Monitor::getMeas(const std::string &id)
{
	return pimpl_->getMeas(id);
}
// get the sigma0 after adjustment
double Monitor::getSigma0()
{
	return pimpl_->getSigma0();
}
// transform coordinates/directions
DECLSPEC Eigen::Vector3d Monitor::transformCoordinates(const Eigen::Vector3d &coord, const std::string &from, const std::string &to)
{
	return pimpl_->transformCoordinates(coord, from, to);
};
DECLSPEC Eigen::Vector3d Monitor::transformDirection(const Eigen::Vector3d &dir, const std::string &from, const std::string &to)
{
	return pimpl_->transformDirection(dir, from, to);
};
waterRom Monitor::getECWSData(const std::string &ecwsRomName)
{
	return pimpl_->getECWSData(ecwsRomName);
}

DECLSPEC wireRom Monitor::getECWIData(const std::string &ecwiRomName)
{
	return pimpl_->getECWIData(ecwiRomName);
}

// actual Implementation
void Monitor::MonitorImpl::initialize()
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
	// Fail fast on a missing/unreadable file. Without this, an unopened stream
	// is fed to r.read(), the (false) result is ignored, and an empty project
	// is handed to the analyzer/algorithm below, which spins for minutes before
	// eventually throwing. Mirrors the is_open()/read checks in the pyLGC loader.
	if (!inputFileStream.is_open())
	{
		throw std::runtime_error("Cannot open file: " + inputFilePath);
	}
	bool succesReading = r.read(inputFileStream);
	if (!succesReading)
	{
		throw std::runtime_error("Errors found in the input file: " + inputFilePath);
	}
	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*project.get());
	if (!analyzer.dataConsistent())
	{
		throw std::runtime_error("Error during problem initialization: " + inputFilePath);
	}

	algorithm.reset(new TLSAlgorithm(*project.get()));
	// make measurements & parameters accessible
	createMeasurementReferences();
	createParameterReferences();
	std::cout << "Monitor object initialized." << std::endl;
}

#if USE_SERIALIZER
std::string Monitor::MonitorImpl::getResultsJson()
{
	if (!estimationStatus)
		throw std::runtime_error("No adjustment results available - call adjust() first.");
	// The root-frame values/covariances are post-processing quantities which adjust()
	// deliberately does NOT compute for performance reasons (the main program fills them
	// in TLGCCalculation::run after the solve). Refresh them here so the snapshot is
	// self-consistent; the cost is paid per snapshot, not per adjust cycle.
	for (auto it = project->getPoints().begin(); it != project->getPoints().end(); ++it)
	{
		it->setCovarianceMatrixInRoot(project.get());
		it->transformEstimatedCoordinates(project.get());
	}
	JSONObjectSerializer obj;
	obj.addProperty("LGCVersion", getLGCVersion());
	obj.addProperty("LGC_DATA", project.get());
	return obj.getStringRepresentation();
}
#endif

void Monitor::MonitorImpl::writeLGCInputFile()
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
void Monitor::MonitorImpl::setFixedFrameParameter(const std::string &frameName, int idx, double val)
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

void Monitor::MonitorImpl::setFixedPointParameter(const std::string &pointName, int idx, double val)
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

void Monitor::MonitorImpl::setFixedSagParameter(const std::string &sagName, int idx, double val)
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

	// check if the associated parameter is really fixed
	LGCAdjustableSag &sagRef = paramRefs.SAGS.at(sagName);
	bool isFree = isFreeVar(sagRef, idx);
	if (isFree)
	{
		throw std::runtime_error("Index " + std::to_string(idx) + " of sag element " + sagName + " is not a fixed variable. ");
	}

	sagRef.setEstValue(idx, val);

}

void Monitor::MonitorImpl::freezeFrameParameter(const std::string &frameName, int idx, double val)
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

void Monitor::MonitorImpl::unfreezeFrameParameter(const std::string &frameName, int idx)
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

void Monitor::MonitorImpl::freezePointParameter(const std::string &pointName, int idx, double val)
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

void Monitor::MonitorImpl::freezeSagParameter(const std::string &sagName, int idx, double val)
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
	bool isFixed = !isFreeVar(sagRef, idx);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of sag element " + sagName + " is not a free variable. Nothing to freeze!");
	}

	// freeze the parameter
	sagRef.setEstValue(idx, val);
	project->fParameterMask.insert(sagRef.getUnknIndex(idx));

}

void Monitor::MonitorImpl::unfreezePointParameter(const std::string &pointName, int idx)
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

void Monitor::MonitorImpl::unfreezeSagParameter(const std::string &sagName, int idx)
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

	// check if the associated parameter is really free
	LGCAdjustableSag &sagRef = paramRefs.SAGS.at(sagName);
	bool isFixed = !isFreeVar(sagRef, idx);
	if (isFixed)
	{
		// maybe not throw an error only a warning?
		throw std::runtime_error("Index " + std::to_string(idx) + " of sag element " + sagName + " is not a free variable. Nothing to unfreeze!");
	}

	// get the index of the variable
	int frozenIndex = -1;
	frozenIndex = sagRef.getUnknIndex(idx);
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

void Monitor::MonitorImpl::createParameterReferences()
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
void Monitor::MonitorImpl::createMeasurementReferences()
{
	// Register every measurement under its observation id; also keep ECWS/ECWI round-of-measurement
	// references for the water/wire network result accessors.
	auto reg = [this](auto &m) { measRefs.meas.insert({m.getObsId(), m}); };
	for (TDataTreeIterator itTree = project.get()->getTree().begin(); itTree != project.get()->getTree().end(); itTree++)
	{
		auto &meas = itTree.node->data->measurements;
		for (auto &itTSTN : meas.fTSTN)
			for (auto &itROM : itTSTN->roms)
			{
				for (auto &m : itROM->measPLR3D) reg(m);
				for (auto &m : itROM->measANGL) reg(m);
				for (auto &m : itROM->measZEND) reg(m);
				for (auto &m : itROM->measDIST) reg(m);
				for (auto &m : itROM->measDHOR) reg(m);
				for (auto &m : itROM->measECTH) reg(m);
				for (auto &m : itROM->measECDIR) reg(m);
			}
		for (auto &itCAM : meas.fCAM) { for (auto &m : itCAM.measUVD) reg(m); for (auto &m : itCAM.measUVEC) reg(m); }
		for (auto &itEDM : meas.fEDM) for (auto &m : itEDM.measDSPT) reg(m);
		for (auto &itLEVEL : meas.fLEVEL) for (auto &m : itLEVEL.measDLEV) reg(m);
		for (auto &rom : meas.fECHO) for (auto &m : rom.measECHO) reg(m);
		for (auto &rom : meas.fECVE) for (auto &m : rom.measECVE) reg(m);
		for (auto &rom : meas.fECSP) for (auto &m : rom.measECSP) reg(m);
		for (auto &rom : meas.fORIE) for (auto &m : rom.measORIE) reg(m);
		for (auto &rom : meas.fINCLY) for (auto &m : rom.measINCLY) reg(m);
		for (auto &rom : meas.fROLLY) for (auto &m : rom.measROLLY) reg(m);
		for (auto &rom : meas.fECWS) { romRefs.ecwsRoms.insert({rom.romName, rom}); for (auto &m : rom.measECWS) reg(m); }
		for (auto &rom : meas.fECWI) { romRefs.ecwiRoms.insert({rom.romName, rom}); for (auto &m : rom.measECWI) reg(m); }
		for (auto &m : meas.fDVER) reg(m);
		for (auto &m : meas.fRADI) reg(m);
		for (auto &m : meas.fOBSXYZ) reg(m);
	}
}

TVMeas &Monitor::MonitorImpl::getMeasurement(const std::string &id)
{
	auto it = measRefs.meas.find(id);
	if (it == measRefs.meas.end())
		throw std::runtime_error("No measurement with ID " + id + " found.");
	return it->second;
}

Eigen::VectorXd Monitor::MonitorImpl::toVectorXd(TFreeVector freeVector)
{
	Eigen::VectorXd vector(3);
	vector << (double)freeVector.getX(), (double)freeVector.getY(), (double)freeVector.getZ();
	return vector;
}

Eigen::VectorXd Monitor::MonitorImpl::toVectorXd(TPositionVector posVector)
{
	Eigen::VectorXd vector(3);
	vector << (double)posVector.getX(), (double)posVector.getY(), (double)posVector.getZ();
	return vector;
}

void Monitor::MonitorImpl::updateMeas(const std::string &id, const Eigen::VectorXd &measurementVector)
{
	// any new observation data invalidates the current estimation
	estimationStatus = false;
	getMeasurement(id).setObsVector(measurementVector);
}
void Monitor::MonitorImpl::setActivationStatus(const std::string &id, bool status)
{
	estimationStatus = false;
	getMeasurement(id).setActive(status);
}
bool Monitor::MonitorImpl::getActivationStatus(const std::string &id)
{
	return getMeasurement(id).isActive();
}

Eigen::VectorXd Monitor::MonitorImpl::getEstimateResidual(const std::string &id)
{
	return getMeasurement(id).getResidualVector();
}

bool Monitor::MonitorImpl::adjust()
{
	Behavior successCalculation;
	// Monitor only needs parameter precisions, not the observation covariance Qvv or the
	// reliability statistics derived from it - skip them to save the bulk of the var-covar cost.
	project->setComputeObsCovar(false);
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

Eigen::VectorXd Monitor::MonitorImpl::getObsSigma(const std::string &id)
{
	return getMeasurement(id).getObsSigmaVector();
}
double Monitor::MonitorImpl::getSigma0()
{
	return project->getS0APosteriori();
}
Eigen::Vector3d Monitor::MonitorImpl::transformCoordinates(const Eigen::Vector3d &coord, const std::string &from, const std::string &to)
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
Eigen::Vector3d Monitor::MonitorImpl::transformDirection(const Eigen::Vector3d &dir, const std::string &from, const std::string &to)
{
	return Monitor::MonitorImpl::transformCoordinates(dir, from, to) - Monitor::MonitorImpl::transformCoordinates(Eigen::Vector3d(0, 0, 0), from, to);
}
waterRom Monitor::MonitorImpl::getECWSData(const std::string &ecwsRomName)
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
wireRom Monitor::MonitorImpl::getECWIData(const std::string &ecwiRomName)
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
Eigen::VectorXd Monitor::MonitorImpl::getMeas(const std::string &id)
{
	return getMeasurement(id).getObsVector();
}

void Monitor::MonitorImpl::setObsSigma(const std::string &id, const Eigen::VectorXd &sigma)
{
	// new weights invalidate the current estimation
	estimationStatus = false;
	getMeasurement(id).setObsSigmaVector(sigma);
}

// get estimate
Eigen::VectorXd Monitor::MonitorImpl::getPointEstimate(const std::string &pointId)
{
	if (paramRefs.POINTS.count(pointId) == 0)
	{
		throw std::runtime_error("No point with Id " + pointId + " found");
	}
	return paramRefs.POINTS.at(pointId).getEstVector();
}
Eigen::VectorXd Monitor::MonitorImpl::getPointEstimate(const std::string &pointId, const std::string &destFrame)
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

Eigen::VectorXd Monitor::MonitorImpl::getFrameEstimate(const std::string &frameId)
{
	if (paramRefs.FRAMES.count(frameId) == 0)
	{
		throw std::runtime_error("No Frame with Id " + frameId + " found");
	}

	return paramRefs.FRAMES.at(frameId).getEstVector();
}

Eigen::VectorXd Monitor::MonitorImpl::getSagEstimate(const std::string &sagId)
{	
	if (paramRefs.SAGS.count(sagId) == 0)
	{
		throw std::runtime_error("No Sag element with Id " + sagId + " found");
	}

	return paramRefs.SAGS.at(sagId).getEstVector();
}

Eigen::VectorXd Monitor::MonitorImpl::getFrameEstimatePrec(const std::string &frameId)
{
	if (paramRefs.FRAMES.count(frameId) == 0)
	{
		throw std::runtime_error("No Frame with Id " + frameId + " found");
	}

	const auto &f = paramRefs.FRAMES.at(frameId);
	Eigen::VectorXd resultVector(7);
	resultVector << f.getEstimatedPrecisionTransl(0), f.getEstimatedPrecisionTransl(1), f.getEstimatedPrecisionTransl(2),
		f.getEstimatedPrecisionRot(0), f.getEstimatedPrecisionRot(1), f.getEstimatedPrecisionRot(2),
		f.getEstimatedPrecisionScale();
	return resultVector;
}

Eigen::VectorXd Monitor::MonitorImpl::getSagEstimatePrec(const std::string &sagId)
{
	if (paramRefs.SAGS.count(sagId) == 0)
	{
		throw std::runtime_error("No Sag element with Id " + sagId + " found");
	}

	Eigen::MatrixXd fullCovar = paramRefs.SAGS.at(sagId).getCovar();

	return fullCovar.diagonal().array().sqrt();
}

Eigen::VectorXd Monitor::MonitorImpl::getPointEstimatePrec(const std::string &pointId)
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

Eigen::VectorXd Monitor::MonitorImpl::getPointEstimatePrec(const std::string &pointId, const std::string &destFrame)
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
