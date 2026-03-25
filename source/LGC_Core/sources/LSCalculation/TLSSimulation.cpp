// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLSSimulation.h"

#include <iomanip>
#include <sstream>

#include <Logger.hpp>

#include "TLGCApp.h"
#include "TLGCData.h"
#include "TLSInputMatrices.h"
#include "TLSInputMatricesFiller.h"
#include "TLSResultsMatricesExtractor.h"
#include "TXYH2CCS.h"

TLSSimulation::TLSSimulation(TLGCData &data, int maxIter, std::shared_ptr<TSimulationOutputFileWriter> fResFileWriter) :
	TLSAlgorithm(data), fData(data), fMaxIterations(maxIter), fPointTransformer(&data.getTree(), data.getConfig().referential), fCGenerator(fPointTransformer), fileWriter(fResFileWriter)
{
	/*Initialize the point summaries list*/
	for (auto &point : fData.getPoints())
	{
		if (!point.isFixed())
			fPointSummaries.push_back(TSimPointSummary(point));
	}

	/*Initialize the frames summaries list*/
	for (auto frameIt(fData.getTree().begin()); frameIt != fData.getTree().end(); ++frameIt)
	{
		if (!frameIt->get()->frame.isFixed())
			fFrameSummaries.push_back(TSimFrameSummary(frameIt->get()->frame));
	}
}

Behavior TLSSimulation::run(TLGCData &data, int maxIterations)
{
	TLSAlgorithm lsCalc(data);
	Behavior calcOK;

	// (VV) Get the seed number
	int seedNumber = fData.getConfig().sim.numSeed;
	// (VV) Feed the seed number into the random number engine
	engine.seed(seedNumber);

	int numOfSimMade = 0;
	int totalNumOfSimul = fData.getConfig().sim.numSims;
	double elapsedTimeBeforeIteration = 0.0;

	auto startIterationTimer = [&elapsedTimeBeforeIteration]() {
		TLGCApp::updateProcessingElapsedSeconds();
		elapsedTimeBeforeIteration = TLGCApp::getProcessingElapsedSeconds();
	};

	auto logIterationTime = [&numOfSimMade, &elapsedTimeBeforeIteration]() {
		TLGCApp::updateProcessingElapsedSeconds();
		const double elapsed = TLGCApp::getProcessingElapsedSeconds();
		std::ostringstream msg;
		msg << "Processing time for SIMU " << numOfSimMade << ": " << std::fixed << std::setprecision(7) << (elapsed - elapsedTimeBeforeIteration)
			<< " s (elapsed since start: " << elapsed << " s)";
		logInfo() << msg.str();
	};

	// Run through the first simulation

	try
	{
		startIterationTimer();
		simulateValues();
		calcOK = lsCalc.run(data, maxIterations);

		if (calcOK)
		{
			updateResValues();
			numOfSimMade++;

			logIterationTime();

			// Iteration through the points
			for (auto it(fData.getPoints().begin()); it != fData.getPoints().end(); ++it)
			{
				it->setCovarianceMatrixInRoot(&fData);
				it->transformEstimatedCoordinates(&fData);
			}

			fileWriter->writeFileBegin(); // Write the beginning of the file (data summary, title etc.)
			fileWriter->writeSimSummary(fData, numOfSimMade); // Write results of the first iteration
		}
		else
		{
			throw std::logic_error("Calculation failed in simulation mode.");
			// return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"Calculation failed in simulation mode.");
		}
	}
	catch (std::exception &excp)
	{
		fData.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"Calculation failed in simulation mode.");
	}

	try
	{
		// repeat simulation calculations until all are completed
		while (calcOK && numOfSimMade < totalNumOfSimul)
		{
			// increment the simulation counter
			numOfSimMade++;

			// reinitialize to project to the original form, points, frames, etc.
			fData.reInitForSIMU();

			startIterationTimer();

			// compute the ls results for the current simulation
			simulateValues();
			calcOK = lsCalc.run(data, maxIterations);

			// Updates the values for the 2 final tables (Points and Frames summaries)
			updateResValues();

			logIterationTime();

			// Iteration through the points
			for (auto it(fData.getPoints().begin()); it != fData.getPoints().end(); ++it)
			{
				it->setCovarianceMatrixInRoot(&fData);
				it->transformEstimatedCoordinates(&fData);
			}

			if (numOfSimMade != totalNumOfSimul) /*Mot the last iteration -> write just the summary*/
				fileWriter->writeSimSummary(fData, numOfSimMade);
			else /*Last iteration -> write all the info*/
				fileWriter->writeLastSimResult(fData, numOfSimMade);
		}

		if (calcOK)
		{
			// Write out the 2 tables in the end
			fileWriter->writeSimPointsSummary(fData.getConfig().title, fPointSummaries, numOfSimMade);
			fileWriter->writeSimFramesSummary(fFrameSummaries, numOfSimMade);

			if (!fData.getConfig().fRelErrors.points.empty())
			{
				fileWriter->writeRelErrorHeader();
				fileWriter->writeRelErrorResults(fData);
			}
			if (!fData.getConfig().fRelErrors.frames.empty())
			{
				fileWriter->writeRelErrorFrameHeader();
				fileWriter->writeRelErrorFrameResults(fData);
			}
		}
	}
	catch (std::exception const &excp)
	{
		fData.getFileLogger() << TFileLogger::e_logType::LOG_ERROR << excp.what();
		return Behavior(Behavior::BehaviorCode::ERR_LSCalculation, L"Calculation failed in simulation mode.");
	}
	return calcOK;
}

void TLSSimulation::updateResValues()
{
	/*Update POINT residual summary*/
	TFreeVector res(TCoordSysFactory::k3DCartesian);
	for (auto &pointSummary : fPointSummaries)
	{
		TDataTreeIterator root = fData.getTree().begin();
		TRefSystemFactory::ERefFrame globalRef = fData.getConfig().referential;
		const LGCAdjustablePoint *point = pointSummary.getAdjustablePoint();

		TPositionVector provisionalValue = point->getProvisionalValue();
		TPositionVector estimatedValue = point->getEstimatedValue();

		// If point is defined in a sub-frame
		if (root != point->getFrameTreePosition())
		{
			TLOR2LOR transfo = TLOR2LOR(point->getFrameTreePosition(), root, "transfo");
			transfo.transform(provisionalValue);
			transfo.transform(estimatedValue);
		}
		else
		{
			if (globalRef != TRefSystemFactory::ERefFrame::kLocalRefFrame)
			{
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
	for (auto &frameSum : fFrameSummaries)
	{
		TransformParameters estimatedParam = frameSum.getAdjustableTransformation()->getEstParam();
		TransformParameters provisionalParam = frameSum.getAdjustableTransformation()->getProvParam();
		TransformParameters result = estimatedParam - provisionalParam;
		frameSum.addNewResValue(result);
	}

	// Reinitialise the observation summaries
	for (auto &node : fData.getTree())
		node->measurements.initialiseObsSummaries();
}

void TLSSimulation::simulateValues()
{ // generate simulated values

	// Tteration through the tree nodes
	for (TDataTreeIterator itTree = fData.getTree().begin(); itTree != fData.getTree().end(); itTree++)
	{
		if (itTree.node->data->isROOTNode())
		{
			for (auto itTSTN : itTree.node->data->measurements.fTSTN)
			{
				// In every TSTN iterate through ROMS
				for (auto itROM : itTSTN->roms)
				{
					updatePLR3DSimValues(itROM, itTSTN); // Simulate PLR3D value
					updateANGLSimValues(itROM, itTSTN); // Fill contribution to an ANGL measurement
					updateZENDSimValues(itROM, itTSTN); // Fill contribution to a ZEND measurement
					updateDISTSimValues(itROM, itTSTN); // Fill contribution to a DIST measurement
					updateDHORSimValues(itROM, itTSTN); // Fill contribution to a DHOR measurement
					updateECTHSimValues(itROM, itTSTN); // Fill contribution to a ECTH measurement
					updateECDIRSimValues(itROM, itTSTN); // Fill contribution to a ECSP measurement
				}
			}
		}
		else
		{
			for (auto itTSTN : itTree.node->data->measurements.fTSTN)
			{
				for (auto itROM : itTSTN->roms)
				{
					updateANGLSimValuesInFrame(itROM, itTSTN);
					updateZENDSimValuesInFrame(itROM, itTSTN);
					updateDISTSimValuesInFrame(itROM, itTSTN);
				}
			}
		}

		// In every node iterate through camera's (CAM) measurements
		for (auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM)
		{
			if (!itCAM->measUVD.empty())
				updateUVDSimValues(*itCAM);
			if (!itCAM->measUVEC.empty())
				updateUVECSimValues(*itCAM);
		}

		// In every node iterate through the EDM's measurements
		for (auto itEDM = itTree.node->data->measurements.fEDM.begin(); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM)
		{
			// Iterate through DPST measurements
			if (!itEDM->measDSPT.empty())
				updateDSPTSimValues(*itEDM, itEDM->measDSPT);
		}

		// In every node iterate through the LEVEL's measurements
		for (auto itLEVEL(itTree.node->data->measurements.fLEVEL.begin()); itLEVEL != itTree.node->data->measurements.fLEVEL.end(); ++itLEVEL)
		{
			updateDLEVSimValues(*itLEVEL, itLEVEL->measDLEV);
		}

		// In every node iterate through the ECHO's measurements
		for (auto &itECHO : itTree.node->data->measurements.fECHO)
			updateECHOSimValues(itECHO, itECHO.measECHO);

		// In every node iterate through the ECVE's measurements
		for (auto &itECVE : itTree.node->data->measurements.fECVE)
			updateECVESimValues(itECVE, itECVE.measECVE);

		// In every node iterate through the ECSP's measurements
		for (auto &itECSP : itTree.node->data->measurements.fECSP)
			updateECSPSimValues(itECSP, itECSP.measECSP);

		// In every node iterate through the ORIE's measurements
		for (auto &itORIE : itTree.node->data->measurements.fORIE)
			updateORIESimValues(itORIE, itORIE.measORIE);

		// No instrument for DVER measurements
		updateDVERSimValues(itTree.node->data->measurements.fDVER);

		// In every node iterate through the INCLY's measurements
		for (auto &itINCLY : itTree.node->data->measurements.fINCLY)
			updateINCLYSimValues(itINCLY, itINCLY.measINCLY);

		// In every node iterate through the ROLLY's measurements
		for (auto &itROLLY : itTree.node->data->measurements.fROLLY)
			updateROLLYSimValues(itROLLY, itROLLY.measROLLY);

		// In every node iterate through the ECWS's measurements
		for (auto &itECWS : itTree.node->data->measurements.fECWS)
			updateECWSSimValues(itECWS, itECWS.measECWS);

		// In every node iterate through the ECWI's measurements
		for (auto &itECWI : itTree.node->data->measurements.fECWI)
			updateECWISimValues(itECWI, itECWI.measECWI);

		// In every node go through the OBSXYZ measurements
		updateOBSXYZSimValues(itTree.node->data->measurements.fOBSXYZ);
	}
}

void TLSSimulation::updateDVERSimValues(std::list<TDVER> &dver)
{
	for (auto &itDVER : dver)
	{
		const auto contrib = fCGenerator.getDVERContrib(itDVER);
		itDVER.setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateDLEVSimValues(const TLEVEL &levelST, std::list<TDLEV> &dlev)
{
	for (auto &itDLEV : dlev)
	{
		if (itDLEV.dhor)
		{
			const auto contribs = fCGenerator.getDLEVContribCombined(levelST, itDLEV);
			itDLEV.setDistance(TLength(getSimulatedValue(contribs.first.fCalcMeas, sqrt(contribs.first.fObsVariance))));
			itDLEV.dhor->setDistance(TLength(getSimulatedValue(contribs.second.fCalcMeas, sqrt(contribs.second.fObsVariance))));
		}
		else
		{
			const auto contrib = fCGenerator.getDLEVContrib(levelST, itDLEV);
			itDLEV.setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
		}
	}
}

void TLSSimulation::updateDSPTSimValues(const TEDM &edmST, std::list<TDSPT> &dspt)
{
	for (auto &itDSPT : dspt)
	{
		const auto contrib = fCGenerator.getDSPTContrib(edmST, itDSPT);
		itDSPT.setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateECHOSimValues(const TECHOROM &echoROM, std::list<TECHO> &echo)
{
	for (auto &itECHO : echo)
	{
		const auto contrib = fCGenerator.getECHOContrib(echoROM, itECHO);
		itECHO.setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateECVESimValues(const TECVEROM &ecveROM, std::list<TECVE> &ecve)
{
	for (auto &itECVE : ecve)
	{
		const auto contrib = fCGenerator.getECVEContrib(ecveROM, itECVE);
		itECVE.setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateECSPSimValues(const TECSPROM &ecspROM, std::list<TECSP> &ecsp)
{
	for (auto &itECSP : ecsp)
	{
		const auto contrib = fCGenerator.getECSPContrib(ecspROM, itECSP);
		itECSP.setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateORIESimValues(const TORIEROM &orieROM, std::list<TORIE> &orie)
{
	for (auto &itORIE : orie)
	{
		const auto contrib = fCGenerator.getOrieContrib(orieROM, itORIE);
		itORIE.setAngle(TAngle(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance)), TAngle::EUnits::kRadians));
	}
}

void TLSSimulation::updateUVDSimValues(TCAM &camera)
{
	for (auto itUVD(camera.measUVD.begin()); itUVD != camera.measUVD.end(); ++itUVD)
	{
		const auto contrib = fCGenerator.getUVDContrib(camera, *itUVD);

		const auto calcX = contrib.fCalcMeas(0);
		const auto calcY = contrib.fCalcMeas(1);
		const auto calcDist = contrib.fCalcMeas(2);
		const auto sigmaX = sqrt(contrib.fObsVariance(0));
		const auto sigmaY = sqrt(contrib.fObsVariance(1));
		const auto sigmaDist = sqrt(contrib.fObsVariance(2));

		TFreeVector measVect;
		measVect.setX(TLength(getSimulatedValue(calcX, sigmaX)));
		measVect.setY(TLength(getSimulatedValue(calcY, sigmaY)));
		const auto measDist = getSimulatedValue(calcDist, sigmaDist);
		/* Probably needs to norm!!! -- the simulation random error could make it no to be unit, ENSURE about that!!! */
		measVect.normalize();
		/*Set the simulated measured values*/
		itUVD->setVectorMeasurement(measVect);
		itUVD->setDistance(TLength(measDist));
	}
}

void TLSSimulation::updateUVECSimValues(TCAM &camera)
{
	for (auto itUVEC(camera.measUVEC.begin()); itUVEC != camera.measUVEC.end(); ++itUVEC)
	{
		const auto contrib = fCGenerator.getUVECContrib(camera, *itUVEC);

		const auto calcX = contrib.fCalcMeas[0];
		const auto calcY = contrib.fCalcMeas[1];
		const auto sigmaX = sqrt(contrib.fObsVariance[0]);
		const auto sigmaY = sqrt(contrib.fObsVariance[1]);

		TFreeVector measVect;
		measVect.setX(TLength(getSimulatedValue(calcX, sigmaX)));
		measVect.setY(TLength(getSimulatedValue(calcY, sigmaY)));
		/* Probably needs to norm!!! -- the simulation random error could make it no to be unit, ENSURE about that!!! */
		measVect.normalize();
		/*Set the simulated measured values*/
		itUVEC->setVectorMeasurement(measVect);
	}
}

void TLSSimulation::updatePLR3DSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measPLR3D.begin()); meas != rom->measPLR3D.end(); ++meas)
	{
		const auto contrib = fCGenerator.getPolar3DContrib(station, rom, *meas);

		const auto calcAngl = contrib.fCalcMeas(0);
		const auto calcZend = contrib.fCalcMeas(1);
		const auto calcDist = contrib.fCalcMeas(2);
		const auto sigmaAngl = sqrt(contrib.fObsVariance(0));
		const auto sigmaZend = sqrt(contrib.fObsVariance(1));
		const auto sigmaDist = sqrt(contrib.fObsVariance(2));

		meas->setAngle(TAngle(getSimulatedValue(calcAngl, sigmaAngl), TAngle::EUnits::kRadians), EPLR3DAngles::kANGL);
		meas->setAngle(TAngle(getSimulatedValue(calcZend, sigmaZend), TAngle::EUnits::kRadians), EPLR3DAngles::kZEND);
		meas->setDistance(TLength(getSimulatedValue(calcDist, sigmaDist)));
	}
}

void TLSSimulation::updateANGLSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measANGL.begin()); meas != rom->measANGL.end(); ++meas)
	{
		const auto contrib = fCGenerator.getHorAnglContrib(station, rom, *meas);
		meas->setAngle(TAngle(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance)), TAngle::EUnits::kRadians));
	}
}

void TLSSimulation::updateZENDSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measZEND.begin()); meas != rom->measZEND.end(); ++meas)
	{
		const auto contrib = fCGenerator.getZenDistContrib(station, *meas);
		meas->setAngle(TAngle(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance)), TAngle::EUnits::kRadians));
	}
}

void TLSSimulation::updateDISTSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measDIST.begin()); meas != rom->measDIST.end(); ++meas)
	{
		const auto contrib = fCGenerator.getSpatialDistanceContrib(station, *meas);
		meas->setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateECTHSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measECTH.begin()); meas != rom->measECTH.end(); ++meas)
	{
		const auto contrib = fCGenerator.getECTHContrib(station, rom, *meas);
		meas->setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateECDIRSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measECDIR.begin()); meas != rom->measECDIR.end(); ++meas)
	{
		const auto contrib = fCGenerator.getECDIRContrib(station, rom, *meas);
		meas->setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateDHORSimValues(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measDHOR.begin()); meas != rom->measDHOR.end(); ++meas)
	{
		const auto contrib = fCGenerator.getHorDistContrib(station, *meas);
		meas->setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateANGLSimValuesInFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	// same function as in the noframe case? NO : uses the in frame contrib
	for (auto meas(rom->measANGL.begin()); meas != rom->measANGL.end(); ++meas)
	{
		const auto contrib = fCGenerator.getHorAnglContribInFrame(station, rom, *meas);
		meas->setAngle(TAngle(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance)), TAngle::EUnits::kRadians));
	}
}

void TLSSimulation::updateZENDSimValuesInFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measZEND.begin()); meas != rom->measZEND.end(); ++meas)
	{
		const auto contrib = fCGenerator.getZenDistContribInFrame(station, *meas);
		meas->setAngle(TAngle(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance)), TAngle::EUnits::kRadians));
	}
}

void TLSSimulation::updateDISTSimValuesInFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station)
{
	for (auto meas(rom->measDIST.begin()); meas != rom->measDIST.end(); ++meas)
	{
		const auto contrib = fCGenerator.getSpatialDistanceContribInFrame(station, *meas);
		meas->setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateINCLYSimValues(const TINCLYROM &inclyROM, std::list<TINCLY> &incly)
{
	for (auto &itINCLY : incly)
	{
		const auto contrib = fCGenerator.getINCLYContrib(inclyROM, itINCLY);
		itINCLY.setAngle(TAngle(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance)), TAngle::EUnits::kRadians));
	}
}

void TLSSimulation::updateROLLYSimValues(const TROLLYROM &rollyROM, std::list<TROLLY> &rolly)
{
	for (auto &itROLLY : rolly)
	{
		const auto contrib = fCGenerator.getROLLYContrib(rollyROM, itROLLY);
		itROLLY.setAngle(TAngle(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance)), TAngle::EUnits::kRadians));
	}
}

void TLSSimulation::updateECWSSimValues(const TECWSROM &ecwsROM, std::list<TECWS> &ecws)
{
	for (auto &itECWS : ecws)
	{
		const auto contrib = fCGenerator.getECWSContrib(ecwsROM, itECWS);
		itECWS.setDistance(TLength(getSimulatedValue(contrib.fCalcMeas, sqrt(contrib.fObsVariance))));
	}
}

void TLSSimulation::updateECWISimValues(const TECWIROM &ecwiROM, std::list<TECWI> &ecwi)
{
	for (auto &itECWI : ecwi)
	{
		const auto contrib = fCGenerator.getECWIContrib(ecwiROM, itECWI);

		const auto calcX = contrib.fCalcMeas[0];
		const auto calcZ = contrib.fCalcMeas[1];
		const auto sigmaX = sqrt(contrib.fObsVariance[0]);
		const auto sigmaZ = sqrt(contrib.fObsVariance[1]);

		itECWI.setDistance(TLength(getSimulatedValue(calcX, sigmaX)), EECWIDistances::kX);
		itECWI.setDistance(TLength(getSimulatedValue(calcZ, sigmaZ)), EECWIDistances::kZ);
	}
}

void TLSSimulation::updateOBSXYZSimValues(std::list<TOBSXYZ> &obsxyz)
{
	for (auto &itOBSXYZ : obsxyz)
	{
		const auto contrib = fCGenerator.getOBSXYZContrib(itOBSXYZ);

		const auto calcX = contrib.fCalcMeas.getX();
		const auto calcY = contrib.fCalcMeas.getY();
		const auto calcZ = contrib.fCalcMeas.getZ();
		const auto sigmaX = sqrt(contrib.fObsVariance(0));
		const auto sigmaY = sqrt(contrib.fObsVariance(1));
		const auto sigmaZ = sqrt(contrib.fObsVariance(2));

		itOBSXYZ.obsValue.setX(TLength(getSimulatedValue(calcX, sigmaX)));
		itOBSXYZ.obsValue.setY(TLength(getSimulatedValue(calcY, sigmaY)));
		itOBSXYZ.obsValue.setZ(TLength(getSimulatedValue(calcZ, sigmaZ)));
	}
}

TReal TLSSimulation::getSimulatedValue(const TReal val, const TReal sigma)
{
	return val + std::normal_distribution<double>(0, sigma)(engine);
}
