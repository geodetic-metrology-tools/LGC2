#include "TLSSimulation.h"
#include "TLGCData.h"
#include "TLSInputMatricesFiller.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLSInputMatrices.h"

#include "TXYH2CCS.h"

TLSSimulation::TLSSimulation(TLGCData& data, int maxIter, std::shared_ptr<TSimulationOutputFileWriter> fResFileWriter) : TLSAlgorithm(data),
fData(data), fMaxIterations(maxIter),
fPointTransformer(&data.getTree(), data.getConfig().referential),
fSimObs(fPointTransformer),
fileWriter(fResFileWriter)
{

	/*Initialize the point summaries list*/
	for (auto& point : fData.getPoints()){
		if (!point.isFixed())
			fPointSummaries.push_back(TSimPointSummary(point));
	}

	/*Initialize the frames summaries list*/
	for (auto frameIt(fData.getTree().begin()); frameIt != fData.getTree().end(); ++frameIt){
		if (!frameIt->get()->frame.isFixed())
			fFrameSummaries.push_back(TSimFrameSummary(frameIt->get()->frame));
	}

}

bool TLSSimulation::run(TLGCData& data, int fMaxIterations)
{
	TLSAlgorithm lsCalc(data);
	bool calcOK = false;
	engine.seed();

	int numOfSimMade = 0;
	int totalNumOfSimul = fData.getConfig().sim.numSims;

	// Run through the first simulation
	try {
		simulateValues();
		calcOK = lsCalc.run(data, fMaxIterations);

		updateResValues();
		numOfSimMade++;

		fileWriter->writeFileBegin(); //Write the beginning of the file (data summary, title etc.)
		fileWriter->writeSimSummary(fData, numOfSimMade); // Write results of the first iteration
	}
	catch (std::exception& excp) {
		fData.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		calcOK = false;
	}

	try{
		// repeat simulation calculations until all are completed 
		while (calcOK &&  numOfSimMade < totalNumOfSimul)
		{
			// increment the simulation counter
			numOfSimMade++;

			// reinitialize to project to the original form, points, frames, etc. 
			fData.reInitForSIMU();

			// compute the ls results for the current simulation
			simulateValues();
			calcOK = lsCalc.run(data, fMaxIterations);

			// Updates the values for the 2 final tables (Points and Frames summaries)
			updateResValues();

			if (numOfSimMade != totalNumOfSimul) /*Mot the last iteration -> write just the summary*/
				fileWriter->writeSimSummary(fData, numOfSimMade);
			else /*Last iteration -> write all the info*/
				fileWriter->writeLastSimResult(fData, numOfSimMade);
		}

		//Write out the 2 tables in the end
		fileWriter->writeSimPointsSummary(fData.getConfig().title, fPointSummaries, numOfSimMade);
		fileWriter->writeSimFramesSummary(fFrameSummaries, numOfSimMade);

	}
	catch (std::exception const& excp) {
		fData.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		calcOK = false;
	}
	return calcOK;

}


void TLSSimulation::updateResValues(){
	/*Update POINT residual summary*/
	TFreeVector res(TCoordSysFactory::k3DCartesian);
	for (auto& pointSummary : fPointSummaries){
		TDataTreeIterator root = fData.getTree().begin();
		TRefSystemFactory::ERefFrame globalRef = fData.getConfig().referential;
		const TAdjustablePoint* point = pointSummary.getAdjustablePoint();

		TPositionVector provisionalValue = point->getProvisionalValue();
		TPositionVector estimatedValue = point->getEstimatedValue();

		//If point is defined in a sub-frame
		if (root != point->getFrameTreePosition()){
			TLOR2LOR transfo = TLOR2LOR(point->getFrameTreePosition(), root, "transfo");
			transfo.transform(provisionalValue);
			transfo.transform(estimatedValue);
		}
		else{
			if (globalRef != TRefSystemFactory::ERefFrame::kLocalRefFrame){
				if (globalRef == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
					TXYH2CCS::XYHs2CCS(provisionalValue);
				else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
					TXYH2CCS::XYHg2000Machine2CCS(provisionalValue);
				else if (globalRef == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
					TXYH2CCS::XYHg1985Machine2CCS(provisionalValue);
			}
		}

		res = estimatedValue - provisionalValue;
		pointSummary.addNewResValue(res);
	}

	/*Update FRAME residual summary*/
	for (auto& frameSum : fFrameSummaries){
		TransformParameters estimatedParam = frameSum.getAdjustableTransformation()->getEstParam();
		TransformParameters provisionalParam = frameSum.getAdjustableTransformation()->getProvParam();
		TransformParameters res = estimatedParam - provisionalParam;
		frameSum.addNewResValue(res);
	}
}

void TLSSimulation::simulateValues()
{//generate simulated values

	//Tteration through the tree nodes
	for (TDataTreeIterator itTree = fData.getTree().begin(); itTree != fData.getTree().end(); itTree++){

		//In every node iterate through the Total station measurements (TSTN)
		for (auto itTSTN : itTree.node->data->measurements.fTSTN){
			//In every TSTN iterate through ROMS
			for (auto itROM : itTSTN->roms){
				getPLR3DSimValues(*itTSTN, *itROM, itROM->measPLR3D);  //Simulate PLR3D value
				getANGLSimValues(*itTSTN, *itROM, itROM->measANGL); //Fill contribution to an ANGL measurement
				getZENDSimValues(*itTSTN, itROM->measZEND); //Fill contribution to a ZEND measurement
				getDISTSimValues(*itTSTN, itROM->measDIST); //Fill contribution to a DIST measurement
				getDHORSimValues(*itTSTN, itROM->measDHOR); //Fill contribution to a DHOR measurement
				getECTHSimValues(*itTSTN, *itROM, itROM->measECTH); //Fill contribution to a ECTH measurement
			}
		}

		//In every node iterate through camera's (CAM) measurements
		for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM){
			if (!itCAM->measUVD.empty())
				getUVDSimValues(*itCAM);
			if (!itCAM->measUVEC.empty())
				getUVECSimValues(*itCAM);
		}

		//In every node iterate through the EDM's measurements
		for (auto itEDM(itTree.node->data->measurements.fEDM.begin()); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM){
			//Iterate through DPST measurements
			if (!itEDM->measDSPT.empty())
				getDSPTSimValues(*itEDM, itEDM->measDSPT);
		}

		//In every node iterate through the LEVEL's measurements
		for (auto itLEVEL(itTree.node->data->measurements.fLEVEL.begin()); itLEVEL != itTree.node->data->measurements.fLEVEL.end(); ++itLEVEL)
		{
			getDLEVSimValues(*itLEVEL, itLEVEL->measDLEV);
			if (itLEVEL->hasDHOR)
			{
				for (auto& itDHOR : itLEVEL->measDLEV)
				{
					if (itDHOR.dhor)
						getHorDistSimValues(itLEVEL->fMeasuredPlane->getReferencePoint(), *itDHOR.dhor.get());
				}
			}
		}


		//In every node iterate through the ECHO's measurements
		for (auto& itECHO : itTree.node->data->measurements.fECHO)
			getECHOSimValues(itECHO, itECHO.measECHO);

		//In every node iterate through the ECVE's measurements
		for (auto& itECVE : itTree.node->data->measurements.fECVE)
			getECVESimValues(itECVE, itECVE.measECVE);

		//In every node iterate through the ECSP's measurements
		for (auto& itECSP : itTree.node->data->measurements.fECSP)
			getECSPSimValues(itECSP, itECSP.measECSP);

		//In every node iterate through the ORIE's measurements
		for (auto& itORIE : itTree.node->data->measurements.fORIE)
			getORIESimValues(itORIE, itORIE.measORIE);

		//No instrument for DVER measurements
		getDVERSimValues(itTree.node->data->measurements.fDVER);

	}
}

void	TLSSimulation::getDVERSimValues(std::vector<TDVER>& dver){
	for (auto& itDVER : dver){
		TReal calcVal = fSimObs.getDVERCalcMeas(itDVER);
		TReal sigma = itDVER.getObservedStDev();
		itDVER.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TLSSimulation::getDLEVSimValues(const TLEVEL& levelST, std::vector<TDLEV>& dlev){
	for (auto& itDLEV : dlev){
		TReal calcVal = fSimObs.getDLEVCalcMeas(levelST, itDLEV);
		TReal sigma = itDLEV.target.sigmaD;
		itDLEV.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

/*DHOR made in DLEV measurement, different from the DHOR obs.*/
void	TLSSimulation::getHorDistSimValues(const TAdjustablePoint* referencePoint, TDLEV::TDHOR& dhorlevel){
	TReal calcVal = fSimObs.getHorDistCalcMeas(referencePoint, dhorlevel);
	TReal sigma = dhorlevel.target.sigmaD;
	dhorlevel.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
}

void	TLSSimulation::getDSPTSimValues(const TEDM& edmST, std::vector<TDSPT>& dspt){
	for (auto& itDSPT : dspt){
		TReal calcVal = fSimObs.getDSPTCalcMeas(edmST, itDSPT);
		TReal sigma = itDSPT.target.sigmaDSpt;
		itDSPT.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void TLSSimulation::getECHOSimValues(const TECHOROM& echoROM, std::vector<TECHO>& echo){
	for (auto& itECHO : echo){
		TReal calcVal = fSimObs.getECHOCalcMeas(echoROM, itECHO);
		TReal sigma = itECHO.target.sigmaD;
		itECHO.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void TLSSimulation::getECVESimValues(const TECVEROM& ecveROM, std::vector<TECVE>& ecve){
	for (auto& itECVE : ecve){
		TReal calcVal = fSimObs.getECVECalcMeas(ecveROM, itECVE);
		TReal sigma = itECVE.target.sigmaD;
		itECVE.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void TLSSimulation::getECSPSimValues(const TECSPROM& ecspROM, std::vector<TECSP>& ecsp){
	for (auto& itECSP : ecsp){
		TReal calcVal = fSimObs.getECSPCalcMeas(ecspROM, itECSP);
		TReal sigma = itECSP.target.sigmaD;
		itECSP.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TLSSimulation::getORIESimValues(const TORIEROM& orieROM, std::vector<TORIE>& orie){
	for (auto& itORIE : orie){
		TReal calcVal = fSimObs.getORIECalcMeas(orieROM, itORIE);
		TReal sigma = itORIE.target.sigmaAngl;
		itORIE.setAngle(TAngle(getSimulatedValue(calcVal, sigma), TAngle::EUnits::kRadians));
	}
}

void	TLSSimulation::getUVDSimValues(TCAM& camera){
	UVDCalcMeas calcMeas;
	for (auto itUVD(camera.measUVD.begin()); itUVD != camera.measUVD.end(); ++itUVD){
		calcMeas = fSimObs.getUVDCalcMeas(camera, *itUVD);


		/*Simulate the values*/
		calcMeas.fMeasuredVector.setX(TLength(getSimulatedValue(calcMeas.fMeasuredVector.getX().getMetresValue(), itUVD->target.sigmaX)));
		calcMeas.fMeasuredVector.setY(TLength(getSimulatedValue(calcMeas.fMeasuredVector.getY().getMetresValue(), itUVD->target.sigmaY)));
		calcMeas.fsDistance = getSimulatedValue(calcMeas.fsDistance, itUVD->target.sigmaDist);

		/* Probably needs to norm!!! -- the simulation random error could make it no to be unit, ENSURE about that!!! */
		calcMeas.fMeasuredVector.normalize();

		/*Set the simulated measured values*/
		itUVD->setVectorMeasurement(calcMeas.fMeasuredVector);
		itUVD->setDistance(TLength(calcMeas.fsDistance));
	}
}

void	TLSSimulation::getUVECSimValues(TCAM& camera){
	TFreeVector calcMeas;
	for (auto itUVEC(camera.measUVEC.begin()); itUVEC != camera.measUVEC.end(); ++itUVEC){
		calcMeas = fSimObs.getUVECCalcMeas(camera, *itUVEC);

		/*Simulate the values*/
		calcMeas.setX(TLength(getSimulatedValue(calcMeas.getX().getMetresValue(), itUVEC->target.sigmaX)));
		calcMeas.setY(TLength(getSimulatedValue(calcMeas.getY().getMetresValue(), itUVEC->target.sigmaY)));

		/* Probably needs to norm!!! -- the simulation random error could make it no to be unit, ENSURE about that!!! */
		calcMeas.normalize();

		itUVEC->setVectorMeasurement(calcMeas);
	}
}

void	TLSSimulation::getPLR3DSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TPLR3D>& plr3D){
	for (auto& itPLR3D : plr3D){
		TReal calcValTheta = fSimObs.getANGLCalcMeas(station, rom, itPLR3D.targetPos);
		TReal calcValPhi = fSimObs.getZENDCalcMeas(station, itPLR3D.targetPos, itPLR3D.target.targetHt);
		TReal calcValDist = fSimObs.getDISTCalcMeas(station, itPLR3D.targetPos, itPLR3D.target.targetHt, itPLR3D.target.distCorrectionAdjustable->getEstimatedValue());

		TReal sigmaTheta = itPLR3D.target.sigmaAngl;
		TReal sigmaPhi = itPLR3D.target.sigmaZenD;
		TReal sigmaDist = itPLR3D.target.sigmaDist;

		itPLR3D.setAngle(TAngle(getSimulatedValue(calcValTheta, sigmaTheta), TAngle::EUnits::kRadians), EPLR3DAngles::kANGL);
		itPLR3D.setAngle(TAngle(getSimulatedValue(calcValPhi, sigmaPhi), TAngle::EUnits::kRadians), EPLR3DAngles::kZEND);
		itPLR3D.setDistance(TLength(getSimulatedValue(calcValDist, sigmaDist)));
	}
}

void	TLSSimulation::getANGLSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TANGL>& angl){
	for (auto& itANGL : angl){
		TReal calcVal = fSimObs.getANGLCalcMeas(station, rom, itANGL.targetPos);
		TReal sigma = itANGL.target.sigmaAngl;
		itANGL.setAngle(TAngle(getSimulatedValue(calcVal, sigma), TAngle::EUnits::kRadians));
	}
}

void	TLSSimulation::getZENDSimValues(const TTSTN& station, std::vector<TZEND>& zend){
	for (auto& itZEND : zend){
		TReal calcVal = fSimObs.getZENDCalcMeas(station, itZEND.targetPos, itZEND.target.targetHt);
		TReal sigma = itZEND.target.sigmaZenD;
		itZEND.setAngle(TAngle(getSimulatedValue(calcVal, sigma), TAngle::EUnits::kRadians));
	}
}

void	TLSSimulation::getDISTSimValues(const TTSTN& station, std::vector<TLINE>& dist){
	for (auto& itDIST : dist){
		TReal calcVal = fSimObs.getDISTCalcMeas(station, itDIST.targetPos, itDIST.target.targetHt, itDIST.target.distCorrectionAdjustable->getEstimatedValue());
		TReal sigma = itDIST.target.sigmaDist;
		itDIST.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TLSSimulation::getECTHSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TECTH>& ecth){
	for (auto& itECTH : ecth){
		TReal calcVal = fSimObs.getECTHCalcMeas(station, rom, itECTH);
		TReal sigma = itECTH.target.sigmaD;
		itECTH.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}


void	TLSSimulation::getDHORSimValues(const TTSTN& station, std::vector<TLINE>& dhor){
	for (auto& itDHOR : dhor){
		TReal calcVal = fSimObs.getDHORCalcMeas(station, itDHOR);
		TReal sigma = itDHOR.target.sigmaDist;
		itDHOR.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

TReal TLSSimulation::getSimulatedValue(const TReal val, const TReal sigma)
{
	return val + std::normal_distribution<double>(0, sigma)(engine);
}
