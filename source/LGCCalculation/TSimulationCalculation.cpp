#include "TSimulationCalculation.h"
#include "TLGCData.h"
#include "TLSInputMatricesFiller.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLSInputMatrices.h"
#include "TLSParametricMtdComputer.h"
#include "TLSCombinedMtdComputer.h"
#include "TLSWeightedUnkMtdComputer.h"
#include "TContributionsGenerator.h"
#include "TLSSolutionCalculator.h"
#include "TXYH2CCS.h"

TSimulationCalculation::TSimulationCalculation(TLGCData& data, int maxIter, TReal convCrit) : 
	fData(data), fMaxIterations(maxIter), fConvCriteria(convCrit), 
	fCg(&data.getTree(), data.getConfig().referential)
{
	/*Initialize the point summaries list*/
	for (auto& point : fData.getPoints()){
		if(!point.isFixed())
			fPointSummaries.push_back(TSimPointSummary(point));
	}

	/*Initialize the frames summaries list*/
	for (auto frameIt( fData.getTree().begin()); frameIt != fData.getTree().end(); ++frameIt){
		if(!frameIt->get()->frame.isFixed())
			fFrameSummaries.push_back(TSimFrameSummary(frameIt->get()->frame));
	}
}


bool	TSimulationCalculation::computeSimulatedResults(std::shared_ptr<TResSimFileWriter> fileWriter){
	// seed the random number generator
	
	bool calcOK = false;
	engine.seed();
	
	int numOfSimMade = 0;
	int totalNumOfSimul = fData.getConfig().sim.numSims;

	// Run through the first simulation
	try {
		calcOK = processSimCalculation();
		updateResValues();
		numOfSimMade++;
		fileWriter->writeFileBegin(); //Write the beginning of the file (data summary, title etc.)
		fileWriter->writeSimSummary(fData, numOfSimMade); // Write results of the first iteration
	} catch (std::exception& excp) {
		fData.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		calcOK = false;
	}

	try{
		// repeat simulation calculations until all are completed 
		while ( calcOK &&  numOfSimMade < totalNumOfSimul)
		{
			// increment the simulation counter
			numOfSimMade++;

			// reinitialize to project to the original form, points, frames, etc. 
			fData.reInitForSIMU();

			// compute the ls results for the current simulation
			calcOK = processSimCalculation();

			// Updates the values for the 2 final tables (Points and Frames summaries)
			updateResValues();

			if(numOfSimMade != totalNumOfSimul) /*Mot the last iteration -> write just the summary*/
				fileWriter->writeSimSummary(fData, numOfSimMade);
			else /*Last iteration -> write all the info*/
				fileWriter->writeLastSimResult(fData, numOfSimMade);
		}

		//Write out the 2 tables in the end
		fileWriter->writeSimPointsSummary(fData.getConfig().title, fPointSummaries, numOfSimMade);
		fileWriter->writeSimFramesSummary(fFrameSummaries, numOfSimMade);

	}
	catch(std::exception const& excp) {
		fData.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		calcOK = false;
	}
	return calcOK;
}

void TSimulationCalculation::updateResValues(){
	/*Update POINT residual summary*/
	TFreeVector res(TCoordSysFactory::k3DCartesian);
	for (auto& pointSummary : fPointSummaries){
		TDataTreeIterator root = fData.getTree().begin();
		TRefSystemFactory::ERefFrame globalRef = fData.getConfig().referential;
		const TAdjustablePoint* point = pointSummary.getAdjustablePoint();

		TPositionVector provisionalValue = point->getProvisionalValue();
		TPositionVector estimatedValue = point->getEstimatedValue();

		//If point is defined in a sub-frame
		if(root != point->getFrameTreePosition()){
			TLOR2LOR transfo = TLOR2LOR(point->getFrameTreePosition(), root, "transfo");
			transfo.transform(provisionalValue);
			transfo.transform(estimatedValue);
		}
		else{
			if(globalRef!=TRefSystemFactory::ERefFrame::kLocalRefFrame){
				if(globalRef == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
					TXYH2CCS::XYHs2CCS(provisionalValue);
				else if(globalRef == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
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


// Process a simulation calculation
// generate a new set of simulated measurement values and proceed to calculation,
// then extract the simulation results and store them
bool	TSimulationCalculation::processSimCalculation()
{
	bool calcOK = false;
	// simulate measured values for the LS calc observations
	simulateValues();

	TLSSolutionCalculator lsCalc;
	calcOK = lsCalc.iterate2Solution(fData, fMaxIterations, fConvCriteria);

	return calcOK;
}


void TSimulationCalculation::simulateValues()
{//generate simulated values
	TContributionsGenerator cg(&fData.getTree(), fData.getConfig().referential);

	//Tteration through the tree nodes
	for (TDataTreeIterator itTree = fData.getTree().begin(); itTree != fData.getTree().end(); itTree++){		

		//In every node iterate through the Total station measurements (TSTN)
		for(auto itTSTN(itTree.node->data->measurements.fTSTN.begin()); itTSTN != itTree.node->data->measurements.fTSTN.end(); ++itTSTN){
			//In every TSTN iterate through ROMS
			for(auto itROM(itTSTN->roms.begin()); itROM != itTSTN->roms.end(); ++itROM){
					getPLR3DSimValues(*itTSTN,*itROM,itROM->measPLR3D);  //Simulate PLR3D value
					getANGLSimValues(*itTSTN, *itROM, itROM->measANGL); //Fill contribution to an ANGL measurement
					getZENDSimValues(*itTSTN, itROM->measZEND); //Fill contribution to a ZEND measurement
					getDISTSimValues(*itTSTN, itROM->measDIST); //Fill contribution to a DIST measurement
					getDHORSimValues(*itTSTN, itROM->measDHOR); //Fill contribution to a DHOR measurement
					getECTHSimValues(*itTSTN, *itROM, itROM->measECTH); //Fill contribution to a ECTH measurement
			}
		}

		//In every node iterate through camera's (CAM) measurements
		for(auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM){
			if (! itCAM->measUVD.empty())
				getUVDSimValues(*itCAM);
			if (!itCAM->measUVEC.empty())
				getUVECSimValues(*itCAM);
		}

		//In every node iterate through the EDM's measurements
		for(auto itEDM(itTree.node->data->measurements.fEDM.begin()); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM){
			//Iterate through DPST measurements
			if(!itEDM->measDSPT.empty())	
				getDSPTSimValues(*itEDM, itEDM->measDSPT);
		}

		//In every node iterate through the LEVEL's measurements
		for(auto itLEVEL(itTree.node->data->measurements.fLEVEL.begin()); itLEVEL != itTree.node->data->measurements.fLEVEL.end(); ++itLEVEL)
		{
			getDLEVSimValues(*itLEVEL, itLEVEL->measDLEV);
			if (itLEVEL->hasDHOR)
			{
				for (auto& itDHOR:itLEVEL->measDLEV)
				{
					if (itDHOR.dhor)
						getHorDistSimValues(itLEVEL->fMeasuredPlane->getReferencePoint(), *itDHOR.dhor.get());
				}
			}
		}


		//In every node iterate through the ECHO's measurements
		for(auto& itECHO:itTree.node->data->measurements.fECHO)
			getECHOSimValues(itECHO, itECHO.measECHO);

		//In every node iterate through the ECHO's measurements
		for (auto& itECVE : itTree.node->data->measurements.fECVE)
			getECVESimValues(itECVE, itECVE.measECVE);

		//In every node iterate through the ORIE's measurements
		for (auto& itORIE:itTree.node->data->measurements.fORIE)
			getORIESimValues(itORIE, itORIE.measORIE);

		//No instrument for DVER measurements
		getDVERSimValues(itTree.node->data->measurements.fDVER);

}
}

void	TSimulationCalculation::getDVERSimValues(std::vector<TDVER>& dver){
	for (auto& itDVER:dver){
		TReal calcVal = fCg.getDVERCalcMeas(itDVER);
		TReal sigma = itDVER.getObservedStDev();
		itDVER.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TSimulationCalculation::getDLEVSimValues(const TLEVEL& levelST, std::vector<TDLEV>& dlev){
	for(auto& itDLEV:dlev){
		TReal calcVal = fCg.getDLEVCalcMeas(levelST, itDLEV);
		TReal sigma = itDLEV.target.sigmaD;
           itDLEV.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

/*DHOR made in DLEV measurement, different from the DHOR obs.*/
void	TSimulationCalculation::getHorDistSimValues(const TAdjustablePoint* referencePoint, TDLEV::TDHOR& dhorlevel){
	TReal calcVal = fCg.getHorDistCalcMeas(referencePoint, dhorlevel);
	TReal sigma = dhorlevel.target.sigmaD;
	dhorlevel.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
}

void	TSimulationCalculation::getDSPTSimValues(const TEDM& edmST, std::vector<TDSPT>& dspt){
	for(auto& itDSPT:dspt){
		TReal calcVal = fCg.getDSPTCalcMeas(edmST, itDSPT);
		TReal sigma = itDSPT.target.sigmaDSpt;
        itDSPT.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void TSimulationCalculation::getECHOSimValues(const TECHOROM& echoROM, std::vector<TECHO>& echo){
	for(auto& itECHO:echo){
		TReal calcVal = fCg.getECHOCalcMeas(echoROM, itECHO);
		TReal sigma = itECHO.target.sigmaD;
        itECHO.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void TSimulationCalculation::getECVESimValues(const TECVEROM& ecveROM, std::vector<TECVE>& ecve){
	for (auto& itECVE:ecve){
		TReal calcVal = fCg.getECVECalcMeas(ecveROM, itECVE);
		TReal sigma = itECVE.target.sigmaD;
		itECVE.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TSimulationCalculation::getORIESimValues(const TORIEROM& orieROM, std::vector<TORIE>& orie){
	for (auto& itORIE:orie){
		TReal calcVal = fCg.getORIECalcMeas(orieROM, itORIE);
		TReal sigma = itORIE.target.sigmaAngl;
		itORIE.setAngle(TAngle(getSimulatedValue(calcVal, sigma), TAngle::EUnits::kRadians));
	}
}

void	TSimulationCalculation::getUVDSimValues(TCAM& camera){
	UVDCalcMeas calcMeas;
	for(auto itUVD(camera.measUVD.begin()); itUVD != camera.measUVD.end(); ++itUVD){
		calcMeas = fCg.getUVDCalcMeas(camera, *itUVD);


		/*Simulate the values*/
		calcMeas.fMeasuredVector.setX( TLength(getSimulatedValue(calcMeas.fMeasuredVector.getX().getMetresValue(),itUVD->target.sigmaX)));
        calcMeas.fMeasuredVector.setY( TLength(getSimulatedValue(calcMeas.fMeasuredVector.getY().getMetresValue(),itUVD->target.sigmaY)));
		calcMeas.fsDistance = getSimulatedValue(calcMeas.fsDistance,itUVD->target.sigmaDist);

		/* Probably needs to norm!!! -- the simulation random error could make it no to be unit, ENSURE about that!!! */
		calcMeas.fMeasuredVector.normalize();

		/*Set the simulated measured values*/
		itUVD->setVectorMeasurement(calcMeas.fMeasuredVector);
        itUVD->setDistance(TLength(calcMeas.fsDistance));
	}
}

void	TSimulationCalculation::getUVECSimValues(TCAM& camera){
	TFreeVector calcMeas;
	for(auto itUVEC(camera.measUVEC.begin()); itUVEC != camera.measUVEC.end(); ++itUVEC){
		calcMeas = fCg.getUVECCalcMeas(camera, *itUVEC);

		/*Simulate the values*/
        calcMeas.setX( TLength(getSimulatedValue(calcMeas.getX().getMetresValue(), itUVEC->target.sigmaX)));
        calcMeas.setY( TLength(getSimulatedValue(calcMeas.getY().getMetresValue(), itUVEC->target.sigmaY)));

		/* Probably needs to norm!!! -- the simulation random error could make it no to be unit, ENSURE about that!!! */
		calcMeas.normalize();

		itUVEC->setVectorMeasurement(calcMeas);
	}
}

void	TSimulationCalculation::getPLR3DSimValues(const TTSTN& station,const TTSTN::TROM& rom,std::vector<TPLR3D>& plr3D){
	for(auto& itPLR3D:plr3D){
		TReal calcValTheta = fCg.getANGLCalcMeas(station,rom,itPLR3D.targetPos);
		TReal calcValPhi = fCg.getZENDCalcMeas(station,itPLR3D.targetPos,itPLR3D.target.targetHt);
		TReal calcValDist = fCg.getDISTCalcMeas(station,itPLR3D.targetPos,itPLR3D.target.targetHt, itPLR3D.target.distCorrectionAdjustable->getEstimatedValue());

		TReal sigmaTheta = itPLR3D.target.sigmaAngl;
		TReal sigmaPhi = itPLR3D.target.sigmaZenD;
		TReal sigmaDist = itPLR3D.target.sigmaDist;

		itPLR3D.setAngle(TAngle(getSimulatedValue(calcValTheta, sigmaTheta), TAngle::EUnits::kRadians),EPLR3DAngles::kANGL);
		itPLR3D.setAngle(TAngle(getSimulatedValue(calcValPhi, sigmaPhi), TAngle::EUnits::kRadians),EPLR3DAngles::kZEND);
		itPLR3D.setDistance(TLength(getSimulatedValue(calcValDist, sigmaDist)));
	}
}

void	TSimulationCalculation::getANGLSimValues(const TTSTN& station,const TTSTN::TROM& rom,std::vector<TANGL>& angl){
	for(auto& itANGL:angl){
		TReal calcVal = fCg.getANGLCalcMeas(station, rom, itANGL.targetPos);
		TReal sigma = itANGL.target.sigmaAngl;
		itANGL.setAngle(TAngle(getSimulatedValue(calcVal, sigma), TAngle::EUnits::kRadians));
	}
}

void	TSimulationCalculation::getZENDSimValues(const TTSTN& station, std::vector<TZEND>& zend){
	for(auto& itZEND:zend){
		TReal calcVal = fCg.getZENDCalcMeas(station, itZEND.targetPos, itZEND.target.targetHt);
		TReal sigma = itZEND.target.sigmaZenD;
		itZEND.setAngle(TAngle(getSimulatedValue(calcVal, sigma), TAngle::EUnits::kRadians));
	}
}

void	TSimulationCalculation::getDISTSimValues(const TTSTN& station, std::vector<TLINE>& dist){
	for(auto& itDIST:dist){
		TReal calcVal = fCg.getDISTCalcMeas(station, itDIST.targetPos, itDIST.target.targetHt, itDIST.target.distCorrectionAdjustable->getEstimatedValue());
		TReal sigma = itDIST.target.sigmaDist;
		itDIST.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TSimulationCalculation::getECTHSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TECTH>& ecth){
	for (auto& itECTH:ecth){
		TReal calcVal = fCg.getECTHCalcMeas(station, rom, itECTH);
		TReal sigma = itECTH.target.sigmaD;
		itECTH.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TSimulationCalculation::getECSPSimValues(const TTSTN& station, const TTSTN::TROM& rom, std::vector<TECSP>& ecsp){
	for (auto& itECSP : ecsp){
		TReal calcVal = fCg.getECSPCalcMeas(station, rom, itECSP);
		TReal sigma = itECSP.target.sigmaD;
		itECSP.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

void	TSimulationCalculation::getDHORSimValues(const TTSTN& station, std::vector<TLINE>& dhor){
	for (auto& itDHOR:dhor){
		TReal calcVal = fCg.getDHORCalcMeas(station, itDHOR);
		TReal sigma = itDHOR.target.sigmaDist;
		itDHOR.setDistance(TLength(getSimulatedValue(calcVal, sigma)));
	}
}

TReal TSimulationCalculation::getSimulatedValue(const TReal val, const TReal sigma)
{
	return val + std::normal_distribution<double>(0, sigma)(engine);
}
