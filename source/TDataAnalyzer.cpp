#include "TDataAnalyzer.h"
#include "ContributionsGenerators\TLOR2LOR.h"

TDataAnalyzer::TDataAnalyzer(TLGCData& dat) : fData(dat), fStandDevUsed(false)
{}

bool TDataAnalyzer::dataConsistent(){
	bool consistent = true;
	int nCALAinROOT = 0;
	auto& outputMessages(fData.getFileLogger());
	outputMessages.writeReportHeader("Data consistency check:");
	
	//IF PDOR keyword used, point which comes after this keword must exist and be defined either under POIN or VXY, i.e. variable in X and Y. 
	auto pdor(fData.getConfig().pdor);
	if(pdor.isActive()){
		if(fData.getPoints().doesObjectExist(pdor.fptname)){
			auto& pdorPoint(fData.getPoints().getObject(pdor.fptname));
			if(pdorPoint.isCoordinateFixed(0) || pdorPoint.isCoordinateFixed(1))
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "PDOR keyword used, but point : " + pdor.fptname + " can be only defined under POIN or VXY keyword."; 
		}
		else{
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "PDOR keyword used, but point : " + pdor.fptname + " is not defined!"; 
		}
	}

	//Unknown indices
	int lastUidx = 0;
	const TDataTree& fTree= fData.getTree();

	//Run through tree and check that whether all frames were initialized, assign unknown indices
	//It is necessary to firstly iterate over the tree, because a Reference point might be created in DLEV measurement and measured plane is initialized
	for (auto it( fTree.begin()); it != fTree.end(); ++it){	
		auto& frame(it.node->data.get()->frame);
		if(!frame.isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Frame: " +	frame.getName() + " is not initialized!"; 
		}
		//Assign unknown indices
		if(!frame.isFixed()){
			frame.setFirstUidx(lastUidx);
			lastUidx = frame.getLastUidx() + 1;
		}

		if(frame.hasStandDev())  //If a frame has standard deviation assigned 
			fStandDevUsed = true;

		int levelCounter = 0;
		//If Reference point was not provided to a DLEV measurement, adjustable plane which is measured needs to be initialized
		for (auto itLEVEL( it.node->data.get()->measurements.fLEVEL.begin()); itLEVEL != it.node->data.get()->measurements.fLEVEL.end(); ++itLEVEL){
			if(!itLEVEL->fMeasuredPlane->isInitialized()){
				TReal referencePoint[3] = {0,0,0};
				for (auto itDLEVMeas( itLEVEL->measDLEV.begin()); itDLEVMeas != itLEVEL->measDLEV.end(); ++itDLEVMeas){
					TPositionVector targetPos = itDLEVMeas->targetPos->getEstimatedValue();
					TLOR2LOR transformation(itDLEVMeas->targetPos->getFrameTreePosition(), fTree.begin(),"Target2ROOT");
					transformation.transform(targetPos);

					referencePoint[0] += targetPos.getX().getValue();
					referencePoint[1] += targetPos.getY().getValue();
					referencePoint[2] += targetPos.getZ().getValue();
				}
				int numberOfMeasurements = itLEVEL->measDLEV.size();
				if(numberOfMeasurements>0){
					levelCounter++;
					referencePoint[0] /=numberOfMeasurements;
					referencePoint[1] /=numberOfMeasurements;
					referencePoint[2] /=numberOfMeasurements;

					TAdjustablePoint& rp =
						fData.getPoints().addObject(TAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2],TCoordSysFactory::ECoordSys::k3DCartesian), 
						false, false, true, "RefPtDLEV" + std::to_string(levelCounter), TLGCRefFrame::ERefs::kOLOC, fTree.begin()));

						itLEVEL->fMeasuredPlane->initialize(&rp,TScalar(0.0),
							TFreeVector(0,0,1,TCoordSysFactory::k3DCartesian),std::bitset<3>(std::string("111")));
				}
				else
					outputMessages << TFileLogger::e_logType::LOG_WARNING << "DLEV group of measurements defined, using *DLEV keyword, but no measurement found."; 
			}
		}
	}

	//Run through point collection and check whether all points were initialized, assign unknown indices at the same time and check that if PDOR used exactly one point in ROOT defined as CALA
	for (std::list<TAdjustablePoint>::iterator it = fData.getPoints().objects.begin(); it != fData.getPoints().objects.end(); ++it){	
		//Check whether initialized
		if(!it->isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Point: " + it->getName() + " is not initialized!"; 
		}
		//Count number of CALA in ROOT, if pdor active there must be exactly one point defined as CALA
		if(pdor.isActive() && it->getFrameTreePosition()->get()->frame.getName() == "ROOT" && it->isFixed() == true)
			nCALAinROOT++;

		if(it->hasStandDeviations())  //If point has standard deviation assigned 
			fStandDevUsed = true;

		//Assign unknown indices
		if(!it->isFixed()){
			it->setFirstUidx(lastUidx);
			lastUidx = it->getLastUidx() + 1;
		}
	}

	if (pdor.isActive() && nCALAinROOT != 1){
		consistent = false;
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "If PDOR keyword used, there must be exactly one point defined under CALA in a ROOT node."; 
	}


	//Run through scalar collection and check whether all objects were initialized, assign unknown indices
	for (auto it = fData.getScalars().objects.begin(); it != fData.getScalars().objects.end(); ++it){		
		if(!it->isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Scalar: " + it->getName() + " is not initialized!"; 
		}

		if(!it->isFixed()){
			it->setFirstUidx(lastUidx);
			lastUidx = it->getLastUidx() + 1;
		}
	}

	//Run through angle collection and check whether all objects were initialized, assign unknown indices
	for (auto it = fData.getAngles().objects.begin(); it != fData.getAngles().objects.end(); ++it){		
		if(!it->isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Angle: " + it->getName() + " is not initialized!"; 
		}

		if(!it->isFixed()){
			it->setFirstUidx(lastUidx);
			lastUidx = it->getLastUidx() + 1;
		} 
	}

	//Run through line collection and check whether all objects were initialized, assign unknown indices
	for (auto it = fData.getLines().objects.begin(); it != fData.getLines().objects.end(); ++it){		
		if(!it->isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Line: " + it->getName() + " is not initialized!"; 
		}

		if(!it->isFixed()){
			it->setFirstUidx(lastUidx);
			lastUidx = it->getLastUidx() + 1;
		}
	}

	//Run through plane collection and check whether all objects were initialized, assign unknown indices
	for (auto it = fData.getPlanes().objects.begin(); it != fData.getPlanes().objects.end(); ++it){		
		if(!it->isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Plane: " +	it->getName() + " is not initialized!"; 
		}

		if(!it->isFixed()){
			it->setFirstUidx(lastUidx);
			lastUidx = it->getLastUidx() + 1;
		}
	}

	//Save total number of unknowns without sigmas
	fData.fUEOIndices.UIndex = lastUidx;


	if (fData.fUEOIndices.UIndex > fData.fUEOIndices.EIndex)
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "There are more unknowns than observations"; 		


	return consistent;
}

TCalcType TDataAnalyzer::getCalculationType(){
	auto configuration(fData.getConfig());	
	TCalcType cType;
	cType.fLIBR = configuration.libre.isActive();
	cType.fOrieUsed = false;
	cType.fPDORused = false;

	// IF *ALLFIXED keyword used, set all points to be fixed
	if(configuration.allfixed.isActive()){
		for (auto it = fData.getPoints().objects.begin(); it != fData.getPoints().objects.end(); ++it){		
			it->updateFixedState(true, true, true);
		}
	}

	if(cType.fLIBR){
		addNetworkConstraints();
	}

	if(configuration.sim.isActive()){
		setSimulation();
	}

	cType.fCombinedCase = fData.isCombinedCaseUsed();  //Tells if combined case LS model needed (if PLR3D or DIR3D measurements used)

	cType.fStandDevUsed = fStandDevUsed;

	cType.fReferential = configuration.referential.type();

	return cType;
}

void TDataAnalyzer::addNetworkConstraints(){
	throw runtime_error("*LIBR keyword not enabled yet!");
}

void TDataAnalyzer::setSimulation(){
	throw runtime_error("*SIMU keyword not enabled yet!");
}


