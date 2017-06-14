#include "TDataAnalyzer.h"
#include "TLOR2LOR.h"
#include <TLGCData.h>
#include "TAllfixedParamGenerator.h"
#include <TPointTransformer.h>

#include "TDist.h" 
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
	

    // Initialise unknown, equation, and observation indices in the data,
    // initialise the counts of different types of points and measurements
    // (these are recounted in this function and assignEOIndices()):
    fData.setDefaultValues();

    // Assign the equation and observation indices, add different types
    // of measurements into the count of total in TLGCData:
    assignEOIndices();

	checkPDOR(outputMessages, consistent);

	//cannot predetermine V in simulation and LIBR
	if (!fData.getConfig().libre.isActive() && !fData.getConfig().sim.isActive())
		predeterminePLR3DV0();


	//Run through tree and check that whether all frames were initialized, assign unknown indices
	//It is necessary to firstly iterate over the tree, because a Reference point might be created in DLEV measurement and measured plane is initialized
	for (auto it( fTree.begin()); it != fTree.end(); ++it){	
		auto& frame(it.node->data.get()->frame);
		if(!frame.isInitialized()){
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Frame: " +	frame.getName() + " is not initialized!"; 
			return false;
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

					LGCAdjustablePoint& rp =
						fData.getPoints().addObject(LGCAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2],TCoordSysFactory::ECoordSys::k3DCartesian), 
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
					LGCAdjustablePoint& rp =
						fData.getPoints().addObject(LGCAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2], TCoordSysFactory::ECoordSys::k3DCartesian),
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

					LGCAdjustablePoint& rp =
						fData.getPoints().addObject(LGCAdjustablePoint(TPositionVector(referencePoint[0], referencePoint[1], referencePoint[2], TCoordSysFactory::ECoordSys::k3DCartesian),
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
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Point: " + point.getName() + " is not initialized!"; 
			return false;
		}
		//Count number of CALA in ROOT
		if (fData.getConfig().pdor.isActive() && point.getFrameTreePosition()->get()->frame.getName() == "ROOT" && point.isFixed() == true)
			nCALAinROOT++;

		if(point.hasStandDeviations()){  //If point has standard deviation assigned 
			fStandDevUsed = true;
			fData.setStandDevUsed();
		}

        // Add the point to the count of total based on its type:
        fData.addToPointNum(point.getSpatialStatus());

		//Assign unknown indices
		if(!point.isFixed()){
			point.setFirstUidx(lastUidx);
			lastUidx = point.getLastUidx() + 1;
		}
	}

	


	//Run through length collection and check whether all objects were initialized, assign unknown indices
	for (auto& length : fData.getLength()){		
		if(!length.isInitialized()){
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Length: " + length.getName() + " is not initialized!"; 
			return false;
		}

		if(!length.isFixed()){
			length.setFirstUidx(lastUidx);
			lastUidx = length.getLastUidx() + 1;
		}
	}

	//Run through angle collection and check whether all objects were initialized, assign unknown indices
	for (auto& angle : fData.getAngles()){		
		if(!angle.isInitialized()){
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Angle: " + angle.getName() + " is not initialized!"; 
			return false;
		}

		if(!angle.isFixed()){
			angle.setFirstUidx(lastUidx);
			lastUidx = angle.getLastUidx() + 1;
		} 
	}

	//Run through line collection and check whether all objects were initialized, assign unknown indices
	for (auto& line : fData.getLines()){		
		if(!line.isInitialized()){
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Line: " + line.getName() + " is not initialized!"; 
			return false;
		}

		if(!line.isFixed()){
			line.setFirstUidx(lastUidx);
			lastUidx = line.getLastUidx() + 1;
		}
	}

	//Run through plane collection and check whether all objects were initialized, assign unknown indices
	for (auto& plane : fData.getPlanes()){		
		if(!plane.isInitialized()){
			outputMessages << TFileLogger::e_logType::LOG_ERROR << "Plane: " +	plane.getName() + " is not initialized!"; 
			return false;
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
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "ALLFIXED and LIBR option cannot be used in the same calculation";
		return false;
	}

	//Not SIMU + LIBR with free frame
	if (fData.getConfig().libre.isActive() && fData.getConfig().sim.isActive())
	{
		for (auto it(fTree.begin()); it != fTree.end(); ++it){
			auto& frame(it.node->data.get()->frame);
			
			//free frame
			if (!frame.isFixed()){
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "SIMU + LIBR options cannot cannot have free subframe";
				return false;
			}

			if (frame.hasStandDev()){  //If a frame has standard deviation assigned 
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "SIMU + LIBR options cannot cannot have free subframe";
				return false;
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
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "ALLFIXED options cannot cannot have free subframe";
				return false;
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
				outputMessages << TFileLogger::e_logType::LOG_ERROR << "LIBR options cannot cannot have free subframe";
				return false;
			}
		}
	}

	if (fData.fUEOIndices.UIndex > fData.fUEOIndices.EIndex){
		outputMessages << TFileLogger::e_logType::LOG_ERROR << "There are more unknowns than equations, UNKNOWNS = " + std::to_string(fData.fUEOIndices.UIndex) +  
				", EQUATIONS = " + std::to_string(fData.fUEOIndices.EIndex) + ". LS calculation can not work. Add measurements or fix some unknowns"; 
		return false;
	}


	return consistent;
}

void TDataAnalyzer::assignEOIndices(){

    // Iterate the whole tree and assign  to the measurements
    for(auto &node : fData.getTree()){
        auto &measurements = node->measurements;

        auto numOfTSTN = measurements.fTSTN.size();

        // TSTN
        for(auto &tstn : measurements.fTSTN){

            //If station can rotate freely, we have two angles representing rotation around X a Y axis. Rotation around Z axis is made by the V0, which is Z-axis rotation.
            if(tstn->rot3D){
                tstn->rotX = &fData.getAngles().addObject(TAdjustableAngle(::TAngle(0.0, ::TAngle::kGons), false, "ROTX" + node->frame.getName() + to_string(numOfTSTN) + std::to_string(tstn->stnId)));
                tstn->rotY = &fData.getAngles().addObject(TAdjustableAngle(::TAngle(0.0, ::TAngle::kGons), false, "ROTY" + node->frame.getName() + to_string(numOfTSTN) + std::to_string(tstn->stnId)));

                // If ROT3D used, instrument height is fixed and is equal to 0
                // (NB. These parameters will not affect in lgc1 case (instrumentHeightAdjustable,
                // see above), since the option ROT3D is not possible in lgc1)
                tstn->ihfix = true;
                tstn->instrument.instrHeight = TLength(0.0);
            }

            // Check if the adjustable length already exists (only in lgc2 case, see above)
            if(!fData.isLGCv1())
                tstn->instrumentHeightAdjustable = &fData.getLength().addObject(TAdjustableLength(tstn->instrument.instrHeight, tstn->ihfix, "TSTN" + node->frame.getName() + tstn->instrument.ID + to_string(numOfTSTN) + std::to_string(tstn->stnId)));

            for(auto &rom : tstn->roms){

                // PLR3D
                for(auto &plr : rom->measPLR3D){
                    // set indices of LS matrices, PLR3D introduces 3 equations and 3 observations
                    plr.setFirstEquationIndex(fData.fUEOIndices.EIndex);
                    plr.setFirstObservationIndex(fData.fUEOIndices.OIndex);
                    fData.fUEOIndices.EIndex += 3;
                    fData.fUEOIndices.OIndex += 3;
                    fData.addToMeasurementNum(TMeasurementsGlobal::kPLR3D);
                }

                // ANGL
                for(auto &angl : rom->measANGL){
                    // set indices of LS matrices, ANGL introduces 1 equation and 1 observation
                    angl.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                    angl.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                    fData.addToMeasurementNum(TMeasurementsGlobal::kANGL);
                }

                // ZEND
                for(auto &zend : rom->measZEND){
                    // set indices of LS matrices, ZEND introduces 1 equation and 1 observation
                    zend.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                    zend.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                    fData.addToMeasurementNum(TMeasurementsGlobal::kZEND);
                }

                // DIST
                for(auto &dist : rom->measDIST){
                    // set indices of LS matrices, DIST introduces 1 equation and 1 observation
                    dist.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                    dist.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                    fData.addToMeasurementNum(TMeasurementsGlobal::kDIST);
                }

                // DHOR
                for(auto &dhor : rom->measDHOR){
                    // set indices of LS matrices, DHOR introduces 1 equation and 1 observation
                    dhor.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                    dhor.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                    fData.addToMeasurementNum(TMeasurementsGlobal::kDHOR);
                }

                // ECTH
                for(auto &ecth : rom->measECTH){
                    // set indices of LS matrices, ECTH introduces 1 equation and 1 observation
                    ecth.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                    ecth.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                    fData.addToMeasurementNum(TMeasurementsGlobal::kECTH);
                }

                // ECDIR
                for(auto &ecdir : rom->measECDIR){
                    // set indices of LS matrices, ECDIR introduces 1 equation and 1 observation
                    ecdir.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                    ecdir.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                    fData.addToMeasurementNum(TMeasurementsGlobal::kECDIR);
                }
            }
        }

        // CAM
        for(auto &cam : measurements.fCAM){
            
            // UVD
            for(auto &uvd : cam.measUVD){
                // set indices of LS matrices, UVD introduces 3 equations and 3 observations
                uvd.setFirstEquationIndex(fData.fUEOIndices.EIndex);
                uvd.setFirstObservationIndex(fData.fUEOIndices.OIndex);
                fData.fUEOIndices.EIndex += 3;
                fData.fUEOIndices.OIndex += 3;
                fData.addToMeasurementNum(TMeasurementsGlobal::kUVD);
            }

            // UVEC
            for(auto &uvec : cam.measUVEC){
                // set indices of LS matrices, UVEC introduces 2 equations and 2 observations
                uvec.setFirstEquationIndex(fData.fUEOIndices.EIndex);
                uvec.setFirstObservationIndex(fData.fUEOIndices.OIndex);
                fData.fUEOIndices.EIndex += 2;
                fData.fUEOIndices.OIndex += 2;
                fData.addToMeasurementNum(TMeasurementsGlobal::kUVEC);
            }
        }

        // DSPT
        for(auto &edm : measurements.fEDM)
            for(auto &dspt : edm.measDSPT){
                // set indices of LS matrices, DSPT introduces 1 equation and 1 observation
                dspt.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                dspt.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                fData.addToMeasurementNum(TMeasurementsGlobal::kDSPT);
            }

        // DLEV
        for(auto &level : measurements.fLEVEL){
            // Reinitialise hasDHOR attribute:
            level.hasDHOR = false;

            for(auto &dlev : level.measDLEV){
                // set indices of LS matrices, DLEV introduces 1 equation and 1 observation
                dlev.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                dlev.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                fData.addToMeasurementNum(TMeasurementsGlobal::kDLEV);

                // DLEV::DHOR
                if(dlev.dhor){
                    dlev.dhor->setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                    dlev.dhor->setFirstObservationIndex(fData.fUEOIndices.OIndex++);

                    // If DHOR exists, tell the station:
                    level.hasDHOR = true;
                }
            }
        }

        // DVER
        for(auto &dver : measurements.fDVER){
            // set indices of LS matrices, DVER introduces 1 equation and 1 observation
            dver.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
            dver.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
            fData.addToMeasurementNum(TMeasurementsGlobal::kDVER);
        }

        // ORIE
        for(auto &orierom : measurements.fORIE)
            for(auto &orie : orierom.measORIE){
                // set indices of LS matrices, ORIE introduces 1 equation and 1 observation
                orie.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                orie.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                fData.addToMeasurementNum(TMeasurementsGlobal::kORIE);
            }

        // ECHO
        for(auto &echorom : measurements.fECHO)
            for(auto &echo : echorom.measECHO){
                // set indices of LS matrices, ECHO introduces 1 equation and 1 observation
                echo.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                echo.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                fData.addToMeasurementNum(TMeasurementsGlobal::kECHO);
            }
        
        // ECVE
        for(auto &ecverom : measurements.fECVE)
            for(auto &ecve : ecverom.measECVE){
                // set indices of LS matrices, ECVE introduces 1 equation and 1 observation
                ecve.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                ecve.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                fData.addToMeasurementNum(TMeasurementsGlobal::kECVE);
            }

        // ECSP
        for(auto &ecsprom : measurements.fECSP)
            for(auto &ecsp : ecsprom.measECSP){
                // set indices of LS matrices, ECSP introduces 1 equation and 1 observation
                ecsp.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
                ecsp.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
                fData.addToMeasurementNum(TMeasurementsGlobal::kECSP);
            }

        // RADI
        for(auto &radi : measurements.fRADI){
            // set indices of LS matrices, RADI introduces 1 equation and 1 observation
            radi.setFirstEquationIndex(fData.fUEOIndices.EIndex++);
            radi.setFirstObservationIndex(fData.fUEOIndices.OIndex++);
            fData.addToMeasurementNum(TMeasurementsGlobal::kRADI);
        }
    }
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
		LGCAdjustablePoint* cala;
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

		LGCAdjustablePoint& oriPt = fData.getPoints().getObject(pdor.fptname);

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
		for (auto it(fTree.begin()); it != fTree.end(); ++it)  // FRK 17/11/2016: suppressed reference "auto&"
			for (auto itTSTN : it.node->data->measurements.fTSTN)
				for (auto itplr : itTSTN->roms)
				{
					if (itplr->measPLR3D.size() != 0)
					{

						auto firstMeas = itplr->measPLR3D.front();
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
}