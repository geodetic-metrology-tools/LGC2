#include "TDataAnalyzer.h"
#include "TLOR2LOR.h"
#include "TLGCData.h"
#include "TAllfixedParamGenerator.h"
#include "TPointTransformer.h"
#include <bitset>

TDataAnalyzer::TDataAnalyzer(TLGCData& dat) : fData(dat), fStandDevUsed(false)
{}

bool TDataAnalyzer::dataConsistent(){
	bool consistent = true;
	int nCALAinROOT = 0;
	auto& outputMessages(fData.getFileLogger());
	outputMessages.writeReportHeader("Data consistency check:");
	int lastUidx = 0; //Unknown indices
	const TDataTree& fTree = fData.getTree();
	
	checkPDOR(outputMessages, consistent);

	if (!fData.getConfig().libre.isActive())
		predeterminePLR3DV0();


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

		for (auto itECHO(it.node->data.get()->measurements.fECHO.begin()); itECHO != it.node->data.get()->measurements.fECHO.end(); ++itECHO){
			if (!itECHO->fMeasuredPlane->isInitialized())
			{
				TReal referencePoint[3] = { 0, 0, 0 };
				TReal initialRefPtDistance = 0.0;
				for (auto itECHOMeas(itECHO->measECHO.begin()); itECHOMeas != itECHO->measECHO.end(); ++itECHOMeas){
					TPositionVector stationPos = itECHOMeas->targetPos->getEstimatedValue(); /*In ECHO the targetPos is a stationPos (SCALE instrument used has no target)*/
					TLOR2LOR transformation(itECHOMeas->targetPos->getFrameTreePosition(), fTree.begin(), "Target2ROOT");
					transformation.transform(stationPos);

					referencePoint[0] += stationPos.getX().getMetresValue();
					referencePoint[1] += stationPos.getY().getMetresValue();
					referencePoint[2] += stationPos.getZ().getMetresValue();

					if (!fData.getConfig().sim.isActive())
						initialRefPtDistance += itECHOMeas->getDistance();
				}
				int numberOfMeasurements = (int)itECHO->measECHO.size();
				if (numberOfMeasurements > 0){
					referencePoint[0] /= numberOfMeasurements;
					referencePoint[1] /= numberOfMeasurements;
					referencePoint[2] /= numberOfMeasurements;

					initialRefPtDistance /= numberOfMeasurements;

					/*Fixed reference point for the ECHO measurement*/
					TAdjustablePoint& rp =
						fData.getPoints().addObject(TAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2], TCoordSysFactory::ECoordSys::k3DCartesian),
						true, true, true, "ECHO_line" + std::to_string(itECHO->line), fData.getConfig().referential, fTree.begin()));

					/*Calculation of the initial approximation value for the theta angle of the plane.*/
					const TPositionVector& firstPoint = itECHO->measECHO.begin()->targetPos->getEstimatedValue();
					const TPositionVector& lastPoint = itECHO->measECHO.back().targetPos->getEstimatedValue();

					TReal thetaLineVectorAngle = atan2q(lastPoint.getX().getMetresValue() - firstPoint.getX().getMetresValue(), lastPoint.getY().getMetresValue() - firstPoint.getY().getMetresValue());

					itECHO->fMeasuredPlane->initialize(&rp, TLength(initialRefPtDistance), TAngle(thetaLineVectorAngle, TAngle::EUnits::kRadians),
						TAngle(M_PI_2, TAngle::EUnits::kRadians), false, true);
				}
				else
					outputMessages << TFileLogger::e_logType::LOG_WARNING << "ECHO group of measurements defined, using *ECHO keyword, but no measurement found.";
			}
		}


		//If Reference point was not provided to a ECVE measurement, adjustable line which is measured needs to be initialized
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

;
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
		//Count number of CALA in ROOT
		if (fData.getConfig().pdor.isActive() && point.getFrameTreePosition()->get()->frame.getName() == "ROOT" && point.isFixed() == true)
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


	//Not run ALLFIXED and LIBR in the same time
	if (fData.getConfig().libre.isActive() && fData.getConfig().allfixed.isActive())
	{
		consistent = false;
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "ALLFIXED and LIBR option cannot be used in the same calculation";
	}

	//Not SIMU + LIBR with free frame
	if (fData.getConfig().libre.isActive() && fData.getConfig().sim.isActive())
	{
		for (auto it(fTree.begin()); it != fTree.end(); ++it){
			auto& frame(it.node->data.get()->frame);
			
			//free frame
			if (!frame.isFixed()){
				consistent = false;
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "SIMU + LIBR options cannot cannot have free subframe";
			}

			if (frame.hasStandDev()){  //If a frame has standard deviation assigned 
				consistent = false;
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "SIMU + LIBR options cannot cannot have free subframe";
			}
		}
	}

	//Not ALLFIXED with free frame
	if (fData.getConfig().allfixed.isActive())
	{
		for (auto it(fTree.begin()); it != fTree.end(); ++it){
			auto& frame(it.node->data.get()->frame);

			//free frame
			if (!frame.isFixed() || frame.hasStandDev()){
				consistent = false;
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "ALLFIXED options cannot cannot have free subframe";
			}
		}
	}

	//Not LIBR with free frame
	if (fData.getConfig().libre.isActive())
	{
		for (auto it(fTree.begin()); it != fTree.end(); ++it){
			auto& frame(it.node->data.get()->frame);

			//free frame
			if (!frame.isFixed() || frame.hasStandDev()){
				consistent = false;
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "LIBR options cannot cannot have free subframe";
			}
		}
	}

	if (fData.fUEOIndices.UIndex > fData.fUEOIndices.EIndex){
		consistent = false;
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "There are more unknowns than equations, UNKNOWNS = " + std::to_string(fData.fUEOIndices.UIndex) +  
				", EQUATIONS = " + std::to_string(fData.fUEOIndices.EIndex) + ". LS calculation can not work."; 
	}


	return consistent;
}


void TDataAnalyzer::checkPDOR(TFileLogger& fileLog, bool dataConsistent)
{	
	const TDataTree& fTree = fData.getTree();

	auto pdor(fData.getConfig().pdor);
	if (pdor.isActive()){
		//IF PDOR keyword used, point which comes after this keyword must exist and be defined either under POIN or VXY, i.e. variable in X and Y. 
		if (fData.getPoints().doesObjectExist(pdor.fptname)){
			auto& pdorPoint(fData.getPoints().getObject(pdor.fptname));
			if (pdorPoint.isCoordinateFixed(0) || pdorPoint.isCoordinateFixed(1))
				fileLog << TFileLogger::e_logType::LOG_ERROR << "PDOR keyword used, but point : " + pdor.fptname + " can be only defined under POIN or VXY keyword.";
		}
		else{
			fileLog << TFileLogger::e_logType::LOG_ERROR << "PDOR keyword used, but point : " + pdor.fptname + " is not defined!";
		}

		//keep the first fixed point in root. Now we give a warning message if more than 1 point is CALA and not an error.
		TAdjustablePoint* cala;
		for (auto& itPoint : fData.getPoints())
			if (itPoint.isFixed() && itPoint.getFrameTreePosition().node->data->isROOTNode())
			{
				cala = &itPoint;
				break;
			}
			else
			{
				dataConsistent = false;
				fileLog << TFileLogger::e_logType::LOG_ERROR << "If PDOR keyword used, there must be at least one point defined under CALA in a ROOT node.";
			}

		TAdjustablePoint& oriPt = fData.getPoints().getObject(pdor.fptname);

		//initialize pdor measurement function
		auto initialize = [&](TPdorObs& pdor_meas) {

			pdor_meas.Initialise(*cala, oriPt, pdor.fgis, pdor.hasBearing);
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

void TDataAnalyzer::predeterminePLR3DV0()
{
	const TDataTree& fTree = fData.getTree();

	if (fData.getMeasurementDimension(TMeasurementsGlobal::EMeasurementType::kPLR3D) != 0)
	{
		for (auto& it(fTree.begin()); it != fTree.end(); ++it)
			for (auto itTSTN : it.node->data->measurements.fTSTN)
				for (auto itplr : itTSTN->roms)
				{
					auto firstMeas = itplr->measPLR3D.at(0);
					//calul v0 app
					TPointTransformer fPointTransfo(&fTree, fData.getConfig().referential);
					TPositionVector targetPos(TCoordSysFactory::ECoordSys::k3DCartesian);
					TPositionVector stationPos(TCoordSysFactory::ECoordSys::k3DCartesian);
					targetPos = firstMeas.targetPos->getEstimatedValue();
					const TLOR2LOR& tgLor2RootTrafo = fPointTransfo.getLORTransformation(firstMeas.targetPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Target lor" to "ROOT"
					tgLor2RootTrafo.transform(targetPos);

					stationPos = itTSTN->instrumentPos->getEstimatedValue();
					const TLOR2LOR& stLor2RootTrafo = fPointTransfo.getLORTransformation(itTSTN->instrumentPos->getFrameTreePosition(), fPointTransfo.getTree()->begin()); //Get transformation from "Station lor" to "ROOT"
					stLor2RootTrafo.transform(stationPos);

					// If not OLOC used and station can not rotate freely => contributions calculated in MLA of the station, otherwise in ROOT of the tree.
					if (fPointTransfo.getRefFrame() != TRefSystemFactory::ERefFrame::kLocalRefFrame && itTSTN->rot3D != true){
						fPointTransfo.transformPointsToMLASystem(itTSTN->instrumentPos->getName(), stationPos, targetPos);
						fPointTransfo.setMLA(true);
					}
					else
						fPointTransfo.setMLA(false);

					TReal xSt = stationPos.getX().getMetresValue();
					TReal ySt = stationPos.getY().getMetresValue();

					TReal xTg = targetPos.getX().getMetresValue();
					TReal yTg = targetPos.getY().getMetresValue();

					//Calculated measurement value
					TAngle V0app = TAngle::aTan2((xTg - xSt), (yTg - ySt)) - itplr->acst - itplr->measPLR3D.begin()->getAngle(EPLR3DAngles::kANGL);  //ACST is the constant orientation of the instrument
			

					// estimated value = 0.0 + correction (V0app)
					int indexV0 = itplr->v0->getFirstUidx();
					itplr->v0->setCorrection(indexV0, V0app);

				}
				

	}
}