////////////////////////////////////////////////////////////////////
// Class responsible for reading the least squares calculation measurements and constraints 
//	and filling the least squares matrices,
//	specific to a least squares calculation
////////////////////////////////////////////////////////////////////////////////////
#include "TLSInputMatricesFiller.h"
#include "TLSInputMatrices.h"
#include "TDataAnalyzer.h"
#include "TLGCData.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////////
TLSInputMatricesFiller::TLSInputMatricesFiller(const TDataTree* tree, const TRefSystemFactory::ERefFrame& referentiel): fCGenerator(tree, referentiel)
{}


TLSInputMatricesFiller::~TLSInputMatricesFiller()
{//Destructor
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool   TLSInputMatricesFiller::fillMatrices(TLGCData* projData, bool fillWeightUnkn, TLSInputMatrices*  matrices){
	bool fillOK = true;
	auto& outputMessages(projData->getFileLogger());

	try{
		//Input matrices have to be initialized each time they are filled.
		initMatriceDimension(*projData, matrices);
		//Contribution generator transformations need to update the transformations it stores.
		fCGenerator.updateTransformations();

		//If weight unknown matrix should be filled
		if(fillWeightUnkn)
			fillOK = fillOK && fillWeightUnkMtrx(projData, matrices);

		//Itteration through the nodes of the tree
		for (TDataTreeIterator itTree = projData->getTree().begin(); itTree != projData->getTree().end(); itTree++){		

			//In every node iterate through the Total station measurements (TSTN)
			for(auto itTSTN(itTree.node->data->measurements.fTSTN.begin()); itTSTN != itTree.node->data->measurements.fTSTN.end(); ++itTSTN){
				//In every TSTN iterate through ROMS and add contributions for every observation type
				for(auto& itROM:itTSTN->roms){
					addPLR3DContributions(itROM, *itTSTN, matrices); //Process all the PLR3D measurement in this ROM
					addHorAngContributions(itROM, *itTSTN, matrices); //Process all the ANGL measurement in this ROM
					addSpaDistContributions(itROM.measDIST, *itTSTN, matrices);  
					addZenDistContributions(itROM.measZEND, *itTSTN, matrices);
					addHorDistContributions(itROM.measDHOR, *itTSTN, matrices);
					addECTHContributions(itROM, *itTSTN, matrices);
				}
			}

			//In every node iterate through camera (TCAM) measurements
			for(auto& itCAM:itTree.node->data->measurements.fCAM){
				addUVDContribution(itCAM, matrices);
				addUVECContribution(itCAM, matrices);
			}

			//In every node iterate through the EDM (TEDM) measurements
			for(auto& itEDM:itTree.node->data->measurements.fEDM)
					addDSPTContribution(itEDM.measDSPT, itEDM, matrices);

			//In every node iterate through the LEVEL measurements
			for(auto& itLEVEL:itTree.node->data->measurements.fLEVEL)
				addLevelStContributions(itLEVEL, matrices);

			//In every node iterate through the ECHO measurements
			for(auto& itECHO:itTree.node->data->measurements.fECHO)
				addECHOContributions(itECHO, matrices);

			//In every node iterate through the ECSP measurements
			//for (auto& itECSP : itTree.node->data->measurements.fECSP)
			//	addECSPContributions(itECSP, matrices);

			//In every node iterate through the ECVE measurements
			for (auto& itECVE : itTree.node->data->measurements.fECVE)
				addECVEContributions(itECVE, matrices);

			//In every node iterate through the LEVEL measurements
			for (auto& itORIE:itTree.node->data->measurements.fORIE)
				addORIEContributions(itORIE, matrices);

			addDVERContribution(itTree.node->data->measurements.fDVER, matrices);

			addRADIContributions(itTree.node->data->measurements.fRADI, matrices);

			//add the initialised PDOR
			if (itTree.node->data->measurements.fPDOR.isInitialised())
				addPDORContributions(itTree.node->data->measurements.fPDOR, matrices);
		}
	}
	catch (std::exception const & excp)
	{
		outputMessages << TFileLogger::e_logType::LOG_ERROR << excp.what();
		fillOK = false;
	}

	if(projData->getFileLogger().hasErrors())
		fillOK = false;

	return fillOK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
//! initialise the dimensions of the input matrices
void   TLSInputMatricesFiller::initMatriceDimension(const TLGCData& projData, TLSInputMatrices* matrices){
	if ((projData.fUEOIndices.EIndex == 0))
		throw std::runtime_error("Equation index in LS matrices is null.");

	if ((projData.fUEOIndices.OIndex == 0))
		throw std::runtime_error("Observation index in LS matrices is null.");	


	//So far checks nothing, in LGC1: this is TRUE if a "static bool	fUsedPDOR" is TRUE
	//This should be detected in analyzer, whether this option was used or not.
	/*
		IF ORIE used, need to set up cnstrObs = TRUE
			if (fcalcType.fOrieUsed)
				cnstrObs = true;
			else
				cnstrObs = false;
	*/

	bool cnstrObs = false;

	if(projData.getConfig().libre.isActive()){
		int cnstrNumber = projData.getNumberOfConstraints();

		if (cnstrNumber != 0){
			//This case is not yet implemented

			// Setting of the input matrices size with the observations & parameters indices
			matrices->setDimensions(projData.fUEOIndices.UIndex, projData.fUEOIndices.EIndex, projData.fUEOIndices.OIndex, cnstrObs, cnstrNumber);

			//processFreeCnstr(data, matrices, data.getFreeConstraints());
			throw std::runtime_error("LIBR keyword is not implemented yet.");		
		}
		else
			throw std::runtime_error("Constraint index is null even though the LIBR option is used.");		
	}
	else//No LIBR used, sets the input matrices size with the observations & parameters indices
		matrices->setDimensions(projData.fUEOIndices.UIndex, projData.fUEOIndices.EIndex, projData.fUEOIndices.OIndex, cnstrObs); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - fill of models with 1 equation
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSInputMatricesFiller::addSpaDistContributions(const std::vector<TLINE>& distMeas, const TTSTN& station, TLSInputMatrices*  matrices){
	bool isProcessOK = true; 
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	DistMeasContrib contributions;

	for(auto meas(distMeas.begin()); meas != distMeas.end(); ++meas){
		eqIdx = meas->getFirstEquationIndex(); 
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getSpatialDistanceContrib(station, *meas); //Get the observation contribution

		// Add station's contributions into a first design matrix
		if(!station.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if(!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding Distance correction contribution
		if(!meas->target.distCorrectionAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, meas->target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

		// Adding instrument height contribution
		if(!station.instrumentHeightAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fHIContrib);

		// Adding contributions for STATION transformations parameters 
		for(auto itStTransform( contributions.fStTransformContrib.begin()); itStTransform !=  contributions.fStTransformContrib.end(); ++itStTransform){
			if(!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters 
		for(auto itTgTransform( contributions.fTgTransformContrib.begin()); itTgTransform !=  contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx,  -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if(contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Spatial Distance contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0/contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if(!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of Spatial Distance measurement occurred.");
	}
}

void  TLSInputMatricesFiller::addHorAngContributions(const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices){
	bool isProcessOK = true; 
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContrib contributions;

	for(auto meas(rom.measANGL.begin()); meas != rom.measANGL.end(); ++meas){
		eqIdx = meas->getFirstEquationIndex(); 
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getHorAnglContrib(station, rom, *meas); //Get the observation contribution

		// Add station contributions 
		if(!station.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if(!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Add V0 contribution
		if(!rom.v0->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, rom.v0->getFirstUidx(), contributions.fV0Contrib);

		// Add contributions of transformations parameters 
		for(auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions of STATION transformation's parameters 
		for(auto itStTransform( contributions.fStTransformContrib.begin()); itStTransform !=  contributions.fStTransformContrib.end(); ++itStTransform){
			if(!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions of TARGET transformation's parameters 
		for(auto itTgTransform( contributions.fTgTransformContrib.begin()); itTgTransform !=  contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector's contribution 
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * ( meas->getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if(contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Horizontal Angle contribution, variance is zero or too small, can not set weight matrix element.");
		else{
				isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0/contributions.fObsVariance);
				isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if(!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Horizontal angle (ANGL) measurement.");
	}
}

void  TLSInputMatricesFiller::addZenDistContributions(const std::vector<TZEND>& zendMeas, const TTSTN& station, TLSInputMatrices*  matrices){
	bool isProcessOK = true; 
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContrib contributions;

	for(auto meas(zendMeas.begin()); meas != zendMeas.end(); ++meas){
		eqIdx = meas->getFirstEquationIndex(); 
		obsIdx = meas->getFirstObservationIndex();

		AnglMeasContrib contributions = fCGenerator.getZenDistContrib(station, *meas); //Get the observation contribution

		// Add station contributions
		if(!station.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if(!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Add instrument height contribution
		if(!station.instrumentHeightAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fHIContrib);

		// Adding contributions for STATION transformations parameters 
		for(auto itStTransform( contributions.fStTransformContrib.begin()); itStTransform !=  contributions.fStTransformContrib.end(); ++itStTransform){
			if(!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters  
		for(auto itTgTransform( contributions.fTgTransformContrib.begin()); itTgTransform !=  contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector values
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * ( meas->getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if(contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Zenith Distance contribution, variance is zero or too small, can not set weight matrix element.");
		else{
				isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0/contributions.fObsVariance);
				isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		// Adding the contribution to the second design matrix , -1 on the diagonal
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if(!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Zenith Distance (vertical angle) measurement.");
	}
}

void  TLSInputMatricesFiller::addHorDistContributions(const std::vector<TLINE>& dhorMeas, const TTSTN& station, TLSInputMatrices*  matrices){
bool isProcessOK = true; 
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	HorDistContrib contributions;

	for(auto meas(dhorMeas.begin()); meas != dhorMeas.end(); ++meas){
		eqIdx = meas->getFirstEquationIndex(); 
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getHorDistContrib(station, *meas); //Get the observation contribution

		// Add station contributions into a first design matrix
		if(!station.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if(!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding Distance Correction contribution
		if(!meas->target.distCorrectionAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, meas->target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

		// Adding contributions for STATION transformations parameters  
		for(auto itStTransform( contributions.fStTransformContrib.begin()); itStTransform !=  contributions.fStTransformContrib.end(); ++itStTransform){
			if(!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters  
		for(auto itTgTransform( contributions.fTgTransformContrib.begin()); itTgTransform !=  contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx,  -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		if(contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Horizontal distance contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0/contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		// Adding the contribution to the second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if(!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Horizontal distance measurement.");
	}
}

void TLSInputMatricesFiller::addECTHContributions(const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	ECTHContrib contributions;

	for (auto& meas:rom.measECTH){
		eqIdx = meas.getFirstEquationIndex();
		obsIdx = meas.getFirstObservationIndex();
		
		contributions = fCGenerator.getECTHContrib(station, rom, meas); //Get the observation contribution

		// Add station's contributions into a first design matrix
		if (!station.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fTSTNPtContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas.targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fScaleStationPtContrib, eqIdx, matrices);

		// Add V0 contribution
		if (!rom.v0->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, rom.v0->getFirstUidx(), contributions.fV0Contrib);

		// Adding contributions for STATION transformations parameters 
		for (auto& itStTransform:contributions.fTSTNPtTransformContrib){
			if (!itStTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform.first, itStTransform.second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters 
		for (auto& itTgTransform:contributions.fStTransformContrib){
			if (!itTgTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform.first, itTgTransform.second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas.getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECTH contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of Spatial Distance measurement occurred.");
	}
}

void  TLSInputMatricesFiller::addLevelStContributions(const TLEVEL& levelSt, TLSInputMatrices*  matrices){
	DLEVContrib contributions;
	HorDistContribLEVEL contributionsDHOR;

	bool isProcessOK = true;
	for(auto itDLEV(levelSt.measDLEV.begin()); itDLEV != levelSt.measDLEV.end(); ++itDLEV){
		MatrixIndex eqIdx = itDLEV->getFirstEquationIndex(); 
		MatrixIndex obsIdx = itDLEV->getFirstObservationIndex();

		contributions = fCGenerator.getDLEVContrib(levelSt,*itDLEV); //Get the observation contribution

		// Add staff's contributions
		if(!itDLEV->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*itDLEV->targetPos, contributions.fStaffContrib, eqIdx, matrices);

		// Add reference point's contributions
		if(!levelSt.fMeasuredPlane->getReferencePoint()->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*levelSt.fMeasuredPlane->getReferencePoint(), contributions.fRefPtContrib, eqIdx, matrices);

		// Adding contributions of STATION transformation's parameters 
		for(auto itStaffTransform( contributions.fStaffTransformContrib.begin()); itStaffTransform !=  contributions.fStaffTransformContrib.end(); ++itStaffTransform){
			if(!itStaffTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStaffTransform->first, itStaffTransform->second, eqIdx, matrices);
		}

		// Adding contributions of TARGET transformation's parameters 
		for(auto itRefPTTransform( contributions.fRefPtTransformContrib.begin()); itRefPTTransform !=  contributions.fRefPtTransformContrib.end(); ++itRefPTTransform){
			if(!itRefPTTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform->first, itRefPTTransform->second, eqIdx, matrices);
		}

		// Adding controbution to a reference poiunt distance, which is at any case variable
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, levelSt.fMeasuredPlane->getRefPtDistUidx(), contributions.fRefPtDistContrib);

		// Adding Distance correction contribution
		if(!levelSt.instrument.collAngleAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, levelSt.instrument.collAngleAdjustable->getFirstUidx(), contributions.fCollAngleContrib);

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx,  -1.0 * (itDLEV->getDistance() - contributions.fCalcMeas));  

		if(contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling DLEV contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0/contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		// Adding the contribution to the second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);


		// In a case that optional DHOR measurement is done
		if (itDLEV->dhor){ // i.e. !=nullptr
				MatrixIndex eqIdxHd = itDLEV->dhor->getFirstEquationIndex(); 
				MatrixIndex obsIdxHd = itDLEV->dhor->getFirstObservationIndex(); 

				contributionsDHOR = fCGenerator.getHorDistContrib(levelSt.fMeasuredPlane->getReferencePoint(), *itDLEV->dhor); //Get the observation contribution

				// Add levelling staff's contributions
				if(!itDLEV->dhor->targetPos->isFixed())
					isProcessOK = isProcessOK && addPointContribution(*itDLEV->dhor->targetPos, contributionsDHOR.fStaffContrib, eqIdxHd, matrices);

				// Add reference point's contributions 
				if(!levelSt.fMeasuredPlane->getReferencePoint()->isFixed())
					isProcessOK = isProcessOK && addPointContribution(*levelSt.fMeasuredPlane->getReferencePoint(), contributionsDHOR.fRefPtContrib, eqIdxHd,matrices);

				// Adding contributions of STATION transformation's parameters 
				for(auto itStaffTransform( contributionsDHOR.fStaffTransformContrib.begin()); itStaffTransform !=  contributionsDHOR.fStaffTransformContrib.end(); ++itStaffTransform){
					if(!itStaffTransform->first.isFixed())
						isProcessOK = isProcessOK && addTransformationContribution(itStaffTransform->first, itStaffTransform->second, eqIdxHd, matrices);
				}

				// Adding contributions of TARGET transformation's parameters 
				for(auto itRefPTTransform( contributionsDHOR.fRefPtTransformContrib.begin()); itRefPTTransform !=  contributionsDHOR.fRefPtTransformContrib.end(); ++itRefPTTransform){
					if(!itRefPTTransform->first.isFixed())
						isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform->first, itRefPTTransform->second, eqIdxHd, matrices);
				}

				// Set Misclosure vector
				isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdxHd,  -1.0 * (itDLEV->dhor->getDistance() - contributionsDHOR.fCalcMeas));

				// Adding the contribution to the second design matrix
				isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdxHd, obsIdxHd, -1.0);

				if(!isnan(itDLEV->dhor->getDHORSigma())){
					isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdxHd, obsIdxHd, 1.0/pow2q(itDLEV->dhor->getDHORSigma()));
					isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdxHd, obsIdxHd, pow2q(itDLEV->dhor->getDHORSigma()) );
				}
				else{
					//Throw exception or set some typical sigma, but we should force user to provide it
					throw std::runtime_error("If DHOR defined for DLEV measurement, standard deviation has to be provided!");
				}
		}

		if(!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of DLEV.");
	}
}

void  TLSInputMatricesFiller::addORIEContributions(const TORIEROM& orieROM, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContrib contributions;

	for (auto& meas:orieROM.measORIE){
		eqIdx = meas.getFirstEquationIndex();
		obsIdx = meas.getFirstObservationIndex();

		contributions = fCGenerator.getOrieContrib(orieROM, meas); //Get the observation contribution

		// Add station contributions 
		if (!orieROM.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*orieROM.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if (!meas.targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Add contributions of transformations parameters 
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions of STATION transformation's parameters 
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform){
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions of TARGET transformation's parameters 
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector's contribution 
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas.getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Horizontal Angle contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Horizontal angle (ANGL) measurement.");
	}
}

void  TLSInputMatricesFiller::addECHOContributions(const TECHOROM& echoROM, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	ECHOContrib contributions;
	for(auto itECHO(echoROM.measECHO.begin()); itECHO != echoROM.measECHO.end(); ++itECHO){
		MatrixIndex eqIdx = itECHO->getFirstEquationIndex(); 
		MatrixIndex obsIdx = itECHO->getFirstObservationIndex();

		contributions = fCGenerator.getECHOContrib(echoROM,*itECHO); //Get the observation contribution

		// Add station's contributions
		if(!itECHO->targetPos->isFixed()) 
			isProcessOK = isProcessOK && addPointContribution(*itECHO->targetPos, contributions.fStationContrib, eqIdx, matrices); /*'Target' in ECHO means station, there is no real target in ECHO.*/

		// Adding contributions for STATION transformation parameters 
		for(auto itStationTransform( contributions.fStTransformContrib.begin()); itStationTransform !=  contributions.fStTransformContrib.end(); ++itStationTransform){
			if(!itStationTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStationTransform->first, itStationTransform->second, eqIdx, matrices);
		}

		// Adding controbution to a theta angle, which is for ECHO always variable
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, echoROM.fMeasuredPlane->getThetaUnknIndex(), contributions.fThetaPlaneAngleContrib);

		// Adding controbution to a reference point distance, which is at any case variable
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, echoROM.fMeasuredPlane->getRefPtDistUidx(), contributions.fRefPtDistContrib);

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx,  -1.0 * (itECHO->getDistance() - contributions.fCalcMeas));  

		if(contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECHO contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0/contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		// Adding the contribution to the second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if(!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of ECHO.");
	}
}

void  TLSInputMatricesFiller::addECVEContributions(const TECVEROM& ecveROM, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	ScaleMeasContrib contributions;
	for (auto itECVE(ecveROM.measECVE.begin()); itECVE != ecveROM.measECVE.end(); ++itECVE){
		MatrixIndex eqIdx = itECVE->getFirstEquationIndex();
		MatrixIndex obsIdx = itECVE->getFirstObservationIndex();

		contributions = fCGenerator.getECVEContrib(ecveROM, *itECVE); //Get the observation contribution

		
		// Add point on the line contributions
		if (!ecveROM.fMeasuredLine->getLinePoint()->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*ecveROM.fMeasuredLine->getLinePoint(), contributions.fPointLineContrib, eqIdx, matrices);


		// Adding contributions of the point on the line transformation's parameters 
		for (auto itRefPTTransform(contributions.fPtLineTransformContrib.begin()); itRefPTTransform != contributions.fPtLineTransformContrib.end(); ++itRefPTTransform){
			if (!itRefPTTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform->first, itRefPTTransform->second, eqIdx, matrices);
		}

		// Add station's contributions
		if (!itECVE->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*itECVE->targetPos, contributions.fStationContrib, eqIdx, matrices); /*'Target' in ECHO means station, there is no real target in ECHO.*/

		// Adding contributions for STATION transformation parameters 
		for (auto itStationTransform(contributions.fStTransformContrib.begin()); itStationTransform != contributions.fStTransformContrib.end(); ++itStationTransform){
			if (!itStationTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStationTransform->first, itStationTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (itECVE->getDistance() - contributions.fCalcMeas));

		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECVE contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		// Adding the contribution to the second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of ECVE.");
	}
}

void  TLSInputMatricesFiller::addDSPTContribution(const std::vector<TDSPT>& dsptMeas, const TEDM& edmST, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	DistMeasContrib contributions;

	for (auto meas(dsptMeas.begin()); meas != dsptMeas.end(); ++meas){
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getDSPTContrib(edmST, *meas); //Get the observation contribution

		// Add station's contributions into a first design matrix
		if (!edmST.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*edmST.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target's contributions into a first design matrix
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding Distance correction contribution
		if (!meas->target.distCorrectionAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, meas->target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

		// Adding contributions for TARGET transformations parameters 
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions for STATION transformations parameters 
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform){
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling DSPT contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of DSPT measurement occurred.");
	}
}

void  TLSInputMatricesFiller::addDVERContribution(const std::vector<TDVER>& dverMeas, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	DVERContrib contributions;

	for (auto meas(dverMeas.begin()); meas != dverMeas.end(); ++meas){
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getDVERContrib(*meas); //Get the observation contribution

		// Add station contributions into a first design matrix
		if (!meas->station->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->station, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding contributions for TARGET transformations parameters 
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions for STATION transformations parameters 
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform){
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling DVER contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of DVER measurement occurred.");
	}
}

void TLSInputMatricesFiller::addPDORContributions(const TPdorObs& pdorObs, TLSInputMatrices*  matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	PtOrientationContrib contributions;

	eqIdx = pdorObs.getFirstEquationIndex();
	obsIdx = pdorObs.getFirstObservationIndex();

	contributions = fCGenerator.getPDORContrib(pdorObs); //Get the observation contribution

	//Add point contribution for the point of orientation
	// Add target contributions
	if (!pdorObs.orientationPt->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*pdorObs.orientationPt, contributions.oriPointContrib, eqIdx, matrices);

	// Add contributions of transformations parameters 
	for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform){
		if (!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Adding contributions of TARGET transformation's parameters 
	for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform){
		if (!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Add Misclosure vector's contribution 
	//if the bearing is not defined, the point of orientation is not moving
	if (pdorObs.isBearingDefined())
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (pdorObs.getBearing().getRadiansValue() - contributions.calcmeas));
	else
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, 0.0);

	// Add weight unknown matrix element
	if (pdorObs.getSigma() < nullLimit)
		throw std::runtime_error("Error when filling pdor contribution, variance is zero or too small, can not set weight matrix element.");
	else{
		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0 / pow2(pdorObs.getSigma().getRadiansValue()));
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, pow2(pdorObs.getSigma().getRadiansValue()));
	}

	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

	if (!isProcessOK)
		throw std::runtime_error("Error occurred during filling input design matrices of PDOR measurement.");
}

void  TLSInputMatricesFiller::addRADIContributions(const std::vector<TRADI>& radiMeas, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	PtOrientationContrib contributions;

	for (auto meas(radiMeas.begin()); meas != radiMeas.end(); ++meas){
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getRADIContrib(*meas); //Get the observation contribution

		// Add  contributions into a first design matrix
		if (!meas->station->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->station, contributions.oriPointContrib, eqIdx, matrices);

		// Add contributions of transformations parameters 
		for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform){
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters 
		for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform){
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * ( contributions.calcmeas));

		// Add weight matrix element
		if (pow2(meas->getObservedStDev()) < nullLimit)
			throw std::runtime_error("Error when filling RADI contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0 / pow2(meas->getObservedStDev()));
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, pow2(meas->getObservedStDev()));
		}

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of RADI measurement occurred.");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - FILLING more-equations observation
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSInputMatricesFiller::addPLR3DContributions(const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices){
	bool isProcessOK = true; 
	MatrixIndex firstEqIdx = -1;
	MatrixIndex firstObsIdx = -1;
	PLR3DContrib contributions;

	for(auto meas(rom.measPLR3D.begin()); meas != rom.measPLR3D.end(); ++meas){
		firstEqIdx = meas->getFirstEquationIndex(); 
		firstObsIdx = meas->getFirstObservationIndex();
		//Get the observation contribution
		contributions = fCGenerator.getPolar3DContrib(station, rom, *meas);

		const TAdjustablePoint& stationPos = *station.instrumentPos;
		const TAdjustablePoint& targetPos = *meas->targetPos;
		// Add contributions for STATION coordinates
		if(!stationPos.isFixed()){
				isProcessOK = isProcessOK && addPointContribution(stationPos, contributions.fStCoordContrib.firstEqPtContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addPointContribution(stationPos, contributions.fStCoordContrib.secondEqPtContrib, firstEqIdx + 1 , matrices);
				isProcessOK = isProcessOK && addPointContribution(stationPos, contributions.fStCoordContrib.thirdEqPtContrib, firstEqIdx + 2 , matrices);
		}

		// Add contributions for TARGET coordinates
		if(!targetPos.isFixed()){
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContrib.firstEqPtContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContrib.secondEqPtContrib, firstEqIdx + 1 , matrices);
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContrib.thirdEqPtContrib, firstEqIdx + 2 , matrices);
		}

		if(!meas->target.distCorrectionAdjustable->isFixed()){
			int distCorUidx = meas->target.distCorrectionAdjustable->getFirstUidx();
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, distCorUidx, contributions.fDistAndCsContrib[0]);
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1,distCorUidx, contributions.fDistAndCsContrib[1]);
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, distCorUidx, contributions.fDistAndCsContrib[2]);
		}

		// Add contributions of STATION transformations parameters 
		for(auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform){
			if(!itStTransform->first.isFixed()){
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second.thirdEquationTransContrib, firstEqIdx + 2, matrices);
			}
		}

		// Add contributions of TARGET transformations parameters 
		for(auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed()){
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.thirdEquationTransContrib, firstEqIdx + 2, matrices);
			}
		}

		// Setting contributions for observations into a second design matrix
		for(int i = 0; i<3;i++){
			isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + i , firstObsIdx, contributions.fThetaContrib[i]); // Theta contrib for the first,second and third equation
			isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + i, firstObsIdx + 1, contributions.fPhiContrib[i]); // Phi contrib for the first,second and third equation
			isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + i, firstObsIdx + 2, contributions.fDistAndCsContrib[i]); // Distance contrib for the first,second and third equation
		}

		//Setting the misclosure vector elements
		for(int i = 0; i<3; i++)
			isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + i,  contributions.fMisclosureVector[i]);


		// Add weight unknown matrix element
		if(contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit || contributions.fObsVariance[2] < nullLimit)
			throw std::runtime_error("Error when filling PLR3D contribution, variance is zero or too small, can not set weight matrix element.");
		else{
		
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx,1.0/contributions.fObsVariance[0]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/contributions.fObsVariance[1]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);

			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0/contributions.fObsVariance[2]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, contributions.fObsVariance[2]);
	

		}

		// Add V0's contribution, V0 is variable at any case. Check in a case of future change.
		if(!rom.v0->isFixed()){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, rom.v0->getFirstUidx(), contributions.fV0Contrib[0]);
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, rom.v0->getFirstUidx(), contributions.fV0Contrib[1]);
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, rom.v0->getFirstUidx(), contributions.fV0Contrib[2]);
		}

		// Add instrument height contributions
		if(!station.instrumentHeightAdjustable->isFixed()){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[0]); 
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[1]); 
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[2]); 
		}

		if(station.rot3D){
			if(station.rotX == nullptr || station.rotY == nullptr)
				throw std::runtime_error("TSTN can rotate freely, but rotation angle around X or Y axis is nullptr.");
			for(int i = 0; i<3;i++){
				isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + i, station.rotX->getFirstUidx(), contributions.fRxContrib[i]); 
				isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + i, station.rotY->getFirstUidx(), contributions.fRyContrib[i]); 
			}
		}

		if(!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of PLR3D measurement occurred.");
	}
}


void TLSInputMatricesFiller::addUVDContribution(const TCAM& camera, TLSInputMatrices*  matrices){
	bool isProcessOK = true; 
	MatrixIndex firstEqIdx = -1;
	MatrixIndex firstObsIdx = -1;
	UVDContrib contributions;

	for(auto meas(camera.measUVD.begin()); meas != camera.measUVD.end(); ++meas){
		firstEqIdx = meas->getFirstEquationIndex(); 
		firstObsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getUVDContrib(camera, *meas);

		const TAdjustablePoint& cameraPos = *camera.instrumentPos;
		const TAdjustablePoint& targetPos = *meas->targetPos;
		// Add contributions for CAMERA coordinates
		if(!cameraPos.isFixed()){
				isProcessOK = isProcessOK && addPointContribution(cameraPos, contributions.fStCoordContrib.firstEqPtContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addPointContribution(cameraPos, contributions.fStCoordContrib.secondEqPtContrib, firstEqIdx + 1 , matrices);
				isProcessOK = isProcessOK && addPointContribution(cameraPos, contributions.fStCoordContrib.thirdEqPtContrib, firstEqIdx + 2 , matrices);
		}

		// Add contributions for TARGET coordinates
		if(!targetPos.isFixed()){
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContrib.firstEqPtContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContrib.secondEqPtContrib, firstEqIdx + 1 , matrices);
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContrib.thirdEqPtContrib, firstEqIdx + 2 , matrices);
		}

		// Add contributions of transformations parameters 
		for(auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed()){
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.thirdEquationTransContrib, firstEqIdx + 2, matrices);
			}
		}

		// Setting contributions for observations into a second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx, contributions.fXCompContrib[0]);  // X vector component contribution (i), first eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx, contributions.fXCompContrib[1]); // X vector component contribution (i), second eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx, contributions.fXCompContrib[2]); // X vector component contribution (i), third eq

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx + 1, contributions.fYCompContrib[0]); // Y vector component contribution (j), first eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx + 1, contributions.fYCompContrib[1]); // Y vector component contribution (j), second eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx + 1, contributions.fYCompContrib[2]); // Y vector component contribution (j), third eq

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx + 2, contributions.fDistContrib[0]); // dist contrib, first eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx + 2, contributions.fDistContrib[1]); //dist contrib, second eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx + 2, contributions.fDistContrib[2]); //dist contrib, second eq

		//Setting the misclosure vector elements
		for(int i = 0; i<3; i++)
			isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + i,  contributions.fMisclosureVector[i]);


		// Add weight unknown matrix element
		if(contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit || contributions.fObsVariance[2] < nullLimit)
			throw std::runtime_error("Error when filling UVD contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0/contributions.fObsVariance[0]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/contributions.fObsVariance[1]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);


			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0/contributions.fObsVariance[2]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, contributions.fObsVariance[2]);
		}

		if(!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of UVD measurement occurred.");
	}
}


void TLSInputMatricesFiller::addUVECContribution(const TCAM& camera, TLSInputMatrices*  matrices){
	bool isProcessOK = true; 
	MatrixIndex firstEqIdx = -1;
	MatrixIndex firstObsIdx = -1;
	UVECContrib contributions;

	for(auto meas(camera.measUVEC.begin()); meas != camera.measUVEC.end(); ++meas){
		firstEqIdx = meas->getFirstEquationIndex(); 
		firstObsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getUVECContrib(camera, *meas);

		const TAdjustablePoint& cameraPos = *camera.instrumentPos;
		const TAdjustablePoint& targetPos = *meas->targetPos;
		// Add contributions for CAMERA coordinates
		if(!cameraPos.isFixed()){
				isProcessOK = isProcessOK && addPointContribution(cameraPos, contributions.fStCoordContribFirstEq, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addPointContribution(cameraPos, contributions.fStCoordContribSecondEq, firstEqIdx + 1 , matrices);
		}

		// Add contributions for TARGET coordinates
		if(!targetPos.isFixed()){
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContribFirstEq, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContribSecondEq, firstEqIdx + 1 , matrices);
		}

		// Add contributions fot the transformations parameters 
		for(auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
			if(!itTgTransform->first.isFixed()){
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
			}
		}

		// Setting contributions for observations into a second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx, contributions.fXCompContrib[0]);  // X vector component contribution (i), first eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx, contributions.fXCompContrib[1]); // X vector component contribution (i), second eq

		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx + 1, contributions.fYCompContrib[0]); // Y vector component contribution (j), first eq
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx + 1, contributions.fYCompContrib[1]); // Y vector component contribution (j), second eq

		//Setting the misclosure vector elements
		for(int i = 0; i<2; i++)
			isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + i,  contributions.fMisclosureVector[i]);

		// Add weight unknown matrix element
		if(contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit )
			throw std::runtime_error("Error when filling UVEC contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0/contributions.fObsVariance[0]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/contributions.fObsVariance[1]);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);
		}

		if(!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of UVEC measurement occurred.");
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - SUPPORTING FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool TLSInputMatricesFiller::addTransformationContribution(const TAdjustableHelmertTransformation& trafo, const TransformationContrib& trContrib, int eqIndex, TLSInputMatrices* matrices){
	bool isProcessOK = true;

	for(int i = 0; i<3;i++){
		if(!trafo.isTranslationFixed(i))
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getTranslationUnknIndex(i) , trContrib.fTranslContrib[i]);

		if(!trafo.isRotationFixed(i))
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getRotationUnknIndex(i) , trContrib.fRotationContrib[i]);
	}
			
	if(!trafo.isScaleFixed())
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getScaleUnknIndex() , trContrib.fScaleContrib);

	return isProcessOK;
}


bool TLSInputMatricesFiller::addPointContribution(const TAdjustablePoint& pointAdj, const TFreeVector& pointContrib, int eqIdx, TLSInputMatrices* matrices){
	bool isProcessOK = true;

	for(int i = 0; i<3;i++){
		if(!pointAdj.isCoordinateFixed(i))
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, pointAdj.getCoordinateUnknIndex(i) , pointContrib[i]);
	}
	return isProcessOK;
}


bool	TLSInputMatricesFiller::fillWeightUnkMtrx(TLGCData* projData, TLSInputMatrices* matrices){
	bool fillOK = true;
	auto& outputMessages(projData->getFileLogger());

	for (auto& point : projData->getPoints()){	
		if(point.hasStandDeviations()){
			for(int i = 0; i<3; i++){
				TReal variance = pow2(point.getStandDev(i));

				if(variance < nullLimit){
					fillOK = false;
					outputMessages << TFileLogger::e_logType::LOG_ERROR <<  "Standard deviation assigned to a cordinate of a point " + point.getName() + " is too small, causes zero division.";
				}
				else
					matrices->setWeightUnkMtrxElement(point.getCoordinateUnknIndex(i), point.getCoordinateUnknIndex(i), 1/variance);
			}
		}
	}

	for (auto it( projData->getTree().begin()); it != projData->getTree().end(); ++it){	
		auto& frame(it.node->data.get()->frame);
		if(frame.hasStandDev()){
			for(int i = 0;i<3;i++){
				if(frame.hasRotationStandDev(i)){
					TReal variance = pow2(frame.getRotationStandDev(i).getRadiansValue());
					if(variance < nullLimit){
						fillOK = false;
						outputMessages << TFileLogger::e_logType::LOG_ERROR <<  "Standard deviation assigned to a rotation of a frame: " + frame.getName() + " is too small, causes zero division"; 
					}
					else
						matrices->setWeightUnkMtrxElement(frame.getRotationUnknIndex(i), frame.getRotationUnknIndex(i), 1/variance);
				}

				if(frame.hasTranslStandDev(i)){
					TReal variance = pow2(frame.getTranslationStandDev(i).getMetresValue());
					if(variance < nullLimit){
						fillOK = false;
						outputMessages << TFileLogger::e_logType::LOG_ERROR <<  "Standard deviation assigned to a translation of a frame: " + frame.getName() + " is too small, causes zero division"; 
					}
					else
						matrices->setWeightUnkMtrxElement(frame.getTranslationUnknIndex(i), frame.getTranslationUnknIndex(i), 1/variance);
				}
			}

			if(frame.hasScaleStandDev()){
				TReal variance = pow2(frame.getScaleStandDev());
				if(variance < nullLimit){
					fillOK = false;
					outputMessages << TFileLogger::e_logType::LOG_ERROR <<  "Standard deviation assigned to a scale of a frame: " + frame.getName() + " is too small, causes zero division"; 
				}
				else
					matrices->setWeightUnkMtrxElement(frame.getScaleUnknIndex(), frame.getScaleUnknIndex(), 1/variance);
			}
		}
	}
	return fillOK;
}
