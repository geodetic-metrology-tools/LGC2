#include "TDataAnalyzer.h"
#include "TLOR2LOR.h"
#include "TLGCData.h"
#include <bitset>

TDataAnalyzer::TDataAnalyzer(TLGCData& dat) : fData(dat), fStandDevUsed(false)
{}

bool TDataAnalyzer::dataConsistent(){
	bool consistent = true;
	int nCALAinROOT = 0;
	auto& outputMessages(fData.getFileLogger());
	outputMessages.writeReportHeader("Data consistency check:");
	
	//IF PDOR keyword used, point which comes after this keyword must exist and be defined either under POIN or VXY, i.e. variable in X and Y. 
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

		if(frame.hasStandDev()){  //If a frame has standard deviation assigned 
			fStandDevUsed = true;
			fData.setStandDevUsed();
		}

		//If Reference point was not provided to a DLEV measurement, adjustable plane which is measured needs to be initialized
		for (auto itLEVEL( it.node->data.get()->measurements.fLEVEL.begin()); itLEVEL != it.node->data.get()->measurements.fLEVEL.end(); ++itLEVEL){
			if(!itLEVEL->fMeasuredPlane->isInitialized()){
				TReal referencePoint[3] = {0,0,0};
				for (auto itDLEVMeas( itLEVEL->measDLEV.begin()); itDLEVMeas != itLEVEL->measDLEV.end(); ++itDLEVMeas){
					TPositionVector targetPos = itDLEVMeas->targetPos->getEstimatedValue();
					TLOR2LOR transformation(itDLEVMeas->targetPos->getFrameTreePosition(), fTree.begin(),"Target2ROOT");
					transformation.transform(targetPos);

               referencePoint[0] += targetPos.getX().getMetresValue();
               referencePoint[1] += targetPos.getY().getMetresValue();
               referencePoint[2] += targetPos.getZ().getMetresValue();
				}
				int numberOfMeasurements = (int)itLEVEL->measDLEV.size();
				if(numberOfMeasurements>0){
					referencePoint[0] /=numberOfMeasurements;
					referencePoint[1] /=numberOfMeasurements;
					referencePoint[2] /=numberOfMeasurements;

					TAdjustablePoint& rp =
						fData.getPoints().addObject(TAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2],TCoordSysFactory::ECoordSys::k3DCartesian), 
						false, false, true, "DLEV_line" + std::to_string(itLEVEL->line), fData.getConfig().referential, fTree.begin()));

						itLEVEL->fMeasuredPlane->initialize(&rp,TLength(0.0), TAngle(0.0, TAngle::EUnits::kRadians), 
															TAngle(0.0, TAngle::EUnits::kRadians), true, true);
				}
				else
					outputMessages << TFileLogger::e_logType::LOG_WARNING << "DLEV group of measurements defined, using *DLEV keyword, but no measurement found."; 
			}
		}

		for (auto itECHO( it.node->data.get()->measurements.fECHO.begin()); itECHO != it.node->data.get()->measurements.fECHO.end(); ++itECHO){
				TReal referencePoint[3] = {0,0,0};
				TReal initialRefPtDistance = 0.0;
				for (auto itECHOMeas( itECHO->measECHO.begin()); itECHOMeas != itECHO->measECHO.end(); ++itECHOMeas){
					TPositionVector stationPos = itECHOMeas->targetPos->getEstimatedValue(); /*In ECHO the targetPos is a stationPos (SCALE instrument used has no target)*/
					TLOR2LOR transformation(itECHOMeas->targetPos->getFrameTreePosition(), fTree.begin(),"Target2ROOT");
					transformation.transform(stationPos);

					referencePoint[0] += stationPos.getX().getMetresValue();
					referencePoint[1] += stationPos.getY().getMetresValue();
					referencePoint[2] += stationPos.getZ().getMetresValue();

					if (! fData.getConfig().sim.isActive())
						initialRefPtDistance += itECHOMeas->getDistance();
				}
				int numberOfMeasurements = (int)itECHO->measECHO.size();
				if(numberOfMeasurements>0){
					referencePoint[0] /=numberOfMeasurements;
					referencePoint[1] /=numberOfMeasurements;
					referencePoint[2] /=numberOfMeasurements;

					initialRefPtDistance /=numberOfMeasurements;

					/*Fixed reference point for the ECHO measurement*/
					TAdjustablePoint& rp =
						fData.getPoints().addObject(TAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2],TCoordSysFactory::ECoordSys::k3DCartesian), 
						true, true, true, "ECHO_line" + std::to_string(itECHO->line), fData.getConfig().referential, fTree.begin()));

					/*Calculation of the initial approximation value for the theta angle of the plane.*/
					const TPositionVector& firstPoint = itECHO->measECHO.begin()->targetPos->getEstimatedValue();
					const TPositionVector& lastPoint = itECHO->measECHO.back().targetPos->getEstimatedValue();

					TReal thetaLineVectorAngle = atan2q(lastPoint.getX().getMetresValue() - firstPoint.getX().getMetresValue(), lastPoint.getY().getMetresValue() - firstPoint.getY().getMetresValue());

					itECHO->fMeasuredPlane->initialize(&rp,TLength(initialRefPtDistance), TAngle(thetaLineVectorAngle, TAngle::EUnits::kRadians), 
							TAngle(M_PI_2, TAngle::EUnits::kRadians), false, true);
				}
				else
					outputMessages << TFileLogger::e_logType::LOG_WARNING << "ECHO group of measurements defined, using *ECHO keyword, but no measurement found."; 
		}


		//If Reference point was not provided to a ECVE measurement, adjustable plane which is measured needs to be initialized
		for (auto itECVE(it.node->data.get()->measurements.fECVE.begin()); itECVE != it.node->data.get()->measurements.fECVE.end(); ++itECVE){
			if (!itECVE->fMeasuredLine->isInitialized()){
				TReal referencePoint[3] = { 0, 0, 0 };
				for (auto itECVEMeas(itECVE->measECVE.begin()); itECVEMeas != itECVE->measECVE.end(); ++itECVEMeas){
					TPositionVector targetPos = itECVEMeas->targetPos->getEstimatedValue();
					TLOR2LOR transformation(itECVEMeas->targetPos->getFrameTreePosition(), fTree.begin(), "Target2ROOT");
					transformation.transform(targetPos);

					referencePoint[0] += targetPos.getX().getMetresValue();
					referencePoint[1] += targetPos.getY().getMetresValue();
					referencePoint[2] += targetPos.getZ().getMetresValue();
				}
				int numberOfMeasurements = (int)itECVE->measECVE.size();
				if (numberOfMeasurements > 0){
					referencePoint[0] /= numberOfMeasurements;
					referencePoint[1] /= numberOfMeasurements;
					referencePoint[2] /= numberOfMeasurements;

					TAdjustablePoint& rp =
						fData.getPoints().addObject(TAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2], TCoordSysFactory::ECoordSys::k3DCartesian),
						false, false, true, "ECVE_line" + std::to_string(itECVE->line), fData.getConfig().referential, fTree.begin()));

					itECVE->fMeasuredLine->initialize(&rp, TFreeVector(0.0, 0.0, 1.0, TCoordSysFactory::ECoordSys::k3DCartesian), std::bitset<3>(111));
				}
				else
					outputMessages << TFileLogger::e_logType::LOG_WARNING << "ECVE group of measurements defined, using *ECVE keyword, but no measurement found.";
			}
		}

	}

	//Run through point collection and check whether all points were initialized, assign unknown indices at the same time and check that if PDOR used exactly one point in ROOT defined as CALA
	for (auto& point : fData.getPoints()){	
		//Check whether initialized
		if(!point.isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Point: " + point.getName() + " is not initialized!"; 
		}
		//Count number of CALA in ROOT, if pdor active there must be exactly one point defined as CALA
		if(pdor.isActive() && point.getFrameTreePosition()->get()->frame.getName() == "ROOT" && point.isFixed() == true)
			nCALAinROOT++;

		if(point.hasStandDeviations()){  //If point has standard deviation assigned 
			fStandDevUsed = true;
			fData.setStandDevUsed();
		}

		//Assign unknown indices
		if(!point.isFixed()){
			point.setFirstUidx(lastUidx);
			lastUidx = point.getLastUidx() + 1;
		}
	}

	// if PDOR keyword is present
	if (pdor.isActive())
	{
		//
		if (nCALAinROOT != 1)
		{
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "If PDOR keyword used, there must be exactly one point defined under CALA in a ROOT node.";
		}
		else
		{
			// Keep point to set the pdor measurement
			TAdjustablePoint* cala;
			for (auto& itPoint : fData.getPoints())
				if (itPoint.isFixed())
				{
					cala = &itPoint;
					break;
				}

			TAdjustablePoint& oriPt = fData.getPoints().getObject(pdor.fptname);

			//initialize pdor measurement function
			auto initialize = [&](TPdorObs& pdor_meas) {
				
				pdor_meas.Initialise(*cala, oriPt, pdor.fgis);
				pdor_meas.setFirstEquationIndex(fData.fUEOIndices.EIndex);
				pdor_meas.setFirstObservationIndex(fData.fUEOIndices.OIndex);
				fData.fUEOIndices.EIndex++;
				fData.fUEOIndices.OIndex++;
				fData.addToMeasurementNum(TMeasurementsGlobal::kPDOR);
			};

			// Go in root node to initialize pdor
			if (fData.getCurrentNode().isROOTNode() && !fData.getCurrentNode().measurements.fPDOR.isInitialised())
				initialize(fData.getCurrentNode().measurements.fPDOR);
			else
				for (auto it(fTree.begin()); it != fTree.end(); ++it)
					if (it.node->data->isROOTNode() && !it.node->data->measurements.fPDOR.isInitialised())
					{
						initialize(it.node->data->measurements.fPDOR);
						break;
					}
		}
	}

	//Run through length collection and check whether all objects were initialized, assign unknown indices
	for (auto& length : fData.getLength()){		
		if(!length.isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Length: " + length.getName() + " is not initialized!"; 
		}

		if(!length.isFixed()){
			length.setFirstUidx(lastUidx);
			lastUidx = length.getLastUidx() + 1;
		}
	}

	//Run through angle collection and check whether all objects were initialized, assign unknown indices
	for (auto& angle : fData.getAngles()){		
		if(!angle.isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Angle: " + angle.getName() + " is not initialized!"; 
		}

		if(!angle.isFixed()){
			angle.setFirstUidx(lastUidx);
			lastUidx = angle.getLastUidx() + 1;
		} 
	}

	//Run through line collection and check whether all objects were initialized, assign unknown indices
	for (auto& line : fData.getLines()){		
		if(!line.isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Line: " + line.getName() + " is not initialized!"; 
		}

		if(!line.isFixed()){
			line.setFirstUidx(lastUidx);
			lastUidx = line.getLastUidx() + 1;
		}
	}

	//Run through plane collection and check whether all objects were initialized, assign unknown indices
	for (auto& plane : fData.getPlanes()){		
		if(!plane.isInitialized()){
			consistent = false;
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Plane: " +	plane.getName() + " is not initialized!"; 
		}

		if(!plane.isFixed()){
			plane.setFirstUidx(lastUidx);
			lastUidx = plane.getLastUidx() + 1;
		}
	}

	//Save total number of unknowns without sigmas
	fData.fUEOIndices.UIndex = lastUidx;


	if (fData.fUEOIndices.UIndex > fData.fUEOIndices.EIndex){
		consistent = false;
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "There are more unknowns than equations, UNKNOWNS = " + std::to_string(fData.fUEOIndices.UIndex) +  
				", EQUATIONS = " + std::to_string(fData.fUEOIndices.EIndex) + ". LS calculation can not work."; 
	}


	return consistent;
}

void TDataAnalyzer::addNetworkConstraints(){
	fData.setNumberOfConstraints(0);
	throw runtime_error("*LIBR keyword not enabled yet!");
}


