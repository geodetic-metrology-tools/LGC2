////////////////////////////////////////////////////////////////////
// Class responsible for reading the least squares calculation measurements and constraints 
//	and filling the least squares matrices,
//	specific to a least squares calculation
////////////////////////////////////////////////////////////////////////////////////
#include "TLSInputMatricesFiller.h"
#include "lsalgo/TLSInputMatrices.h"
#include "TDataAnalyzer.h"
#include "TLGCData.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////////
TLSInputMatricesFiller::TLSInputMatricesFiller(TDataTree* tree, const TCalcType& calcType): fcalcType(calcType), fCGenerator(tree, calcType.fReferential )
{}


TLSInputMatricesFiller::~TLSInputMatricesFiller()
{//Destructor
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool   TLSInputMatricesFiller::fillMatrices(TLGCData* projData, TLSInputMatrices*  matrices){
	bool fillOK = true;
	auto& outputMessages(projData->fOutputFileWriter);

	try{
		//Input matrices have to be initialized each time they are filled.
		initMatriceDimension(*projData, matrices);
		fCGenerator.updateTransformations();	//Fill matrices called, contribution generator transformations need to be updated
		fillOK = fillOK && fillWeightUnkMtrx(projData, matrices);
	
		//Tteration through the tree nodes
		for (TDataTreeIterator itTree = projData->tree.begin(); itTree != projData->tree.end(); itTree++){		
			//In every node iterate through the Total station measurements (TSTN)
			for(auto itTSTN(itTree.node->data->measurements.fTSTN.begin()); itTSTN != itTree.node->data->measurements.fTSTN.end(); ++itTSTN){
				//In every TSTN iterate through ROMS
				for(auto itROM(itTSTN->roms.begin()); itROM != itTSTN->roms.end(); ++itROM){
					//In every ROM iterate through the specific measurements
					for(auto itPLR3D(itROM->measPLR3D.begin()); itPLR3D != itROM->measPLR3D.end(); ++itPLR3D)
						addPLR3DContribution(*itPLR3D, *itROM, *itTSTN, matrices);  //Fill contribution to a PLR3D measurement


					for(auto itDIR3D(itROM->measDIR3D.begin()); itDIR3D != itROM->measDIR3D.end(); ++itDIR3D)
						addDIR3DContribution(*itDIR3D, *itROM, *itTSTN, matrices); //Fill contribution to a DIR3D measurement

					for(auto itANGL(itROM->measANGL.begin()); itANGL != itROM->measANGL.end(); ++itANGL)
						addHorAngContribution(*itANGL, *itROM, *itTSTN, matrices); //Fill contribution to an ANGL measurement

					for(auto itZEND(itROM->measZEND.begin()); itZEND != itROM->measZEND.end(); ++itZEND)
						addZenDistContributions(*itZEND, *itTSTN, matrices); //Fill contribution to a ZEND measurement


					for(auto itDIST(itROM->measDIST.begin()); itDIST != itROM->measDIST.end(); ++itDIST)
						addSpaDistContribution(*itDIST, *itTSTN, matrices); //Fill contribution to a DIST measurement

					for(auto itDHOR(itROM->measDHOR.begin()); itDHOR != itROM->measDHOR.end(); ++itDHOR)
						addHorDistContributions(*itDHOR, *itTSTN, matrices); //Fill contribution to a DHOR measurement


					for(auto itECTH(itROM->measECTH.begin()); itECTH != itROM->measECTH.end(); ++itECTH){

					}
				}
			}


			//In every node iterate through camera's (CAM) measurements
			for(auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM){
				for(auto itPLR3D(itCAM->measPLR3D.begin()); itPLR3D != itCAM->measPLR3D.end(); ++itPLR3D)
						addPLR3DContribution(*itPLR3D, *itCAM, matrices);

				for(auto itDIR3D(itCAM->measDIR3D.begin()); itDIR3D != itCAM->measDIR3D.end(); ++itDIR3D)
						addDIR3DContribution(*itDIR3D, *itCAM, matrices);
			}

			//In every node iterate through the EDM's measurements
			for(auto itEDM(itTree.node->data->measurements.fEDM.begin()); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM){
				//Iterate through DPST measurements
				for(auto itDPST(itEDM->measDSPT.begin()); itDPST != itEDM->measDSPT.end(); ++itDPST){	
					//To do
				}
			}

			//In every node iterate through the LEVEL's measurements
			for(auto itLEVEL(itTree.node->data->measurements.fLEVEL.begin()); itLEVEL != itTree.node->data->measurements.fLEVEL.end(); ++itLEVEL)
				addLevelStContributions(*itLEVEL, matrices);
		}
	}
	catch (std::exception const & excp)
	{
		outputMessages << TFileLogger::e_logType::LOG_ERROR << excp.what();
		fillOK = false;
	}

	if(projData->fOutputFileWriter.hasErrors())
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
	bool cnstrObs;
	if (fcalcType.fOrieUsed)
		cnstrObs = true;
	else
		cnstrObs = false;

	if(fcalcType.fLIBR){
		int cnstrNumber = fcalcType.freeNetwCnstr.getNumberOfConstraint();
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
	else
		matrices->setDimensions(projData.fUEOIndices.UIndex, projData.fUEOIndices.EIndex, projData.fUEOIndices.OIndex, cnstrObs); // Setting of the input matrices size with the observations & parameters indices
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - FILLING 1-equations observation
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSInputMatricesFiller::addSpaDistContribution(const TLINE& meas, const TTSTN& station, TLSInputMatrices*  matrices){
	MatrixIndex eqIdx = meas.getFirstEquationIndex(); 
	MatrixIndex obsIdx = meas.getFirstObservationIndex(); 
	bool isProcessOK = true;

	DistMeasContrib contributions = fCGenerator.getSpatialDistanceContrib(station, meas); //Get the observation contribution

	// Add station's contributions into a first design matrix
	if(!station.instrumentPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

	// Add target's contributions into a first design matrix
	if(!meas.targetPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

	// Adding Distance correction contribution
	if(!meas.target.distCorrectionAdjustable->isFixed())
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, meas.target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

	// Adding instrument height contribution
	if(!station.instrumentHeightAdjustable->isFixed())
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fHIContrib);

	// Adding contributions of transformation's parameters 
	for(auto itTgTransform( contributions.fTgTransformContrib.begin()); itTgTransform !=  contributions.fTgTransformContrib.end(); ++itTgTransform){
		if(!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Set Misclosure vector
	isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx,  -1.0 * (meas.getScalar() - contributions.fCalcMeas));

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

void  TLSInputMatricesFiller::addHorAngContribution(const TANGL& meas, const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices){
	MatrixIndex eqIdx = meas.getFirstEquationIndex(); 
	MatrixIndex obsIdx = meas.getFirstObservationIndex();
	bool isProcessOK = true; 

	AnglMeasContrib contributions = fCGenerator.getHorAnglContrib(station, rom, meas); //Get the observation contribution

	// Add station's contributions 
	if(!station.instrumentPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

	// Add target's contributions
	if(!meas.targetPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

	// Add V0's contribution
	if(!rom.v0->isFixed())
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, rom.v0->getFirstUidx(), contributions.fV0Contrib);

	// Add contributions of transformations parameters 
	for(auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
		if(!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Add Misclosure vector's contribution 
	isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * ( meas.getAngle() - contributions.fCalcMeas).rad());

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

void  TLSInputMatricesFiller::addZenDistContributions(const TZEND& meas, const TTSTN& station, TLSInputMatrices*  matrices){
	MatrixIndex eqIdx = meas.getFirstEquationIndex(); 
	MatrixIndex obsIdx = meas.getFirstObservationIndex();
	bool isProcessOK = true; 

	AnglMeasContrib contributions = fCGenerator.getZenDistContrib(station, meas); //Get the observation contribution

	// Add station's contributions into a first design matrix
	if(!station.instrumentPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

	// Add target's contributions
	if(!meas.targetPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

	// Add instrument height's contribution
	if(!station.instrumentHeightAdjustable->isFixed())
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fHIContrib);

	// Add contributions of transformations parameters 
	for(auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
		if(!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Add Misclosure vector's contribution 
	isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * ( meas.getAngle() - contributions.fCalcMeas).rad());

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

void  TLSInputMatricesFiller::addHorDistContributions(const TLINE& meas, const TTSTN& station, TLSInputMatrices*  matrices){
	MatrixIndex eqIdx = meas.getFirstEquationIndex(); 
	MatrixIndex obsIdx = meas.getFirstObservationIndex();
	bool isProcessOK = true;
	std::vector<std::pair<TAdjustableHelmertTransformation, TransformationContrib>> fTgTransformContrib;

	HorDistContrib contributions = fCGenerator.getHorDistContrib(station, meas); //Get the observation contribution

	// Add station's contributions into a first design matrix
	if(!station.instrumentPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*station.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

	// Add target's contributions into a first design matrix
	if(!meas.targetPos->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

	// Adding Distance Correction contribution
	if(!meas.target.distCorrectionAdjustable->isFixed())
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, meas.target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

	// Adding contributions of transformation's parameters 
	for(auto itTgTransform( contributions.fTgTransformContrib.begin()); itTgTransform !=  contributions.fTgTransformContrib.end(); ++itTgTransform){
		if(!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Set Misclosure vector
	isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx,  -1.0 * (meas.getScalar() - contributions.fCalcMeas));

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


void  TLSInputMatricesFiller::addLevelStContributions(const TLEVEL& levelSt, TLSInputMatrices*  matrices){
	for(auto itDLEV(levelSt.measDLEV.begin()); itDLEV != levelSt.measDLEV.end(); ++itDLEV){
		MatrixIndex eqIdx = itDLEV->getEquationIndex(); 
		MatrixIndex obsIdx = itDLEV->getObservationIndex();
		bool isProcessOK = true;

		DLEVContrib contributions = fCGenerator.getDLEVContrib(levelSt,*itDLEV); //Get the observation contribution

		// Add station's contributions
		if(!itDLEV->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*itDLEV->targetPos, contributions.fStaffTargetContrib, eqIdx, matrices);

		// Adding controbution to a reference poiunt distance, which is at any case variable
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, levelSt.fMeasuredPlane->getRefPtUidx(), contributions.fRefPtDistContrib);

		// Adding Distance correction contribution
		if(!levelSt.instrument.collAngleAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, levelSt.instrument.collAngleAdjustable->getFirstUidx(), contributions.fCollAngleContrib);

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx,  -1.0 * (itDLEV->getMeasuredVerticalDistance() - contributions.fCalcMeas));  

		if(contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling DLEV contribution, variance is zero or too small, can not set weight matrix element.");
		else{
			isProcessOK = isProcessOK && matrices->setWeightMtrxElement(obsIdx, obsIdx, 1.0/contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		// Adding the contribution to the second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdx, obsIdx, -1.0);

		if(!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of DLEV.");
	}

	for(auto itDHOR(levelSt.measDHOR.begin()); itDHOR != levelSt.measDHOR.end(); ++itDHOR){
		MatrixIndex eqIdxHd = itDHOR->getFirstEquationIndex(); 
		MatrixIndex obsIdxHd = itDHOR->getFirstObservationIndex(); 
		bool isProcessOK = true;

		HorDistContribLEVEL contributionsDHOR = fCGenerator.getHorDistContrib(levelSt.fMeasuredPlane->getReferencePoint(), *itDHOR); //Get the observation contribution

		// Add reference point's contributions 
		if(!levelSt.fMeasuredPlane->getReferencePoint()->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*levelSt.fMeasuredPlane->getReferencePoint(), contributionsDHOR.fRpCoordContrib, eqIdxHd,matrices);

		// Add levelling staff's contributions
		if(!itDHOR->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*itDHOR->targetPos, contributionsDHOR.fLevStaffCoordContrib, eqIdxHd, matrices);

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdxHd,  -1.0 * (itDHOR->getScalar() - contributionsDHOR.fCalcMeas));

		// Adding the contribution to the second design matrix
		isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(eqIdxHd, obsIdxHd, -1.0);

		if(!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of DLEV's Horizontal distance measurement.");	
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - FILLING more-equations observation
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSInputMatricesFiller::addPLR3DContribution(const TPLR3D& meas, const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices){
	//Get the first and last equation index
	MatrixIndex firstEqIdx = meas.getFirstEquationIndex();
	MatrixIndex firstObsIdx = meas.getFirstObservationIndex();

	bool isProcessOK = true;

	//Get the observation contribution
	PLR3DContrib contributions = fCGenerator.getPolar3DContrib(station, rom, meas);
	isProcessOK = isProcessOK && addPLR3DBaseContrib(station.instrumentPos, meas, firstEqIdx, firstObsIdx, contributions, matrices);

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

		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.rotX->getFirstUidx(), contributions.fRxContrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.rotX->getFirstUidx(), contributions.fRxContrib[1]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, station.rotX->getFirstUidx(), contributions.fRxContrib[2]); 

		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.rotY->getFirstUidx(), contributions.fRyContrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.rotY->getFirstUidx(), contributions.fRyContrib[1]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, station.rotY->getFirstUidx(), contributions.fRyContrib[2]); 
	}

	if(!isProcessOK)
		throw std::runtime_error("Error when filling input design matrices of PLR3D measurement occurred.");
}

void TLSInputMatricesFiller::addPLR3DContribution(const TPLR3D& meas, const TCAM& camera,  TLSInputMatrices*  matrices){
	//Get the observation contribution
	PLR3DContrib contributions = fCGenerator.getPolar3DContrib(camera, meas);
	bool isProcessOK = addPLR3DBaseContrib(camera.instrumentPos, meas, meas.getFirstEquationIndex(), meas.getFirstObservationIndex(), contributions, matrices);

	if(!isProcessOK)
		throw std::runtime_error("Error when filling input design matrices of PLR3D measurement occurred.");
}


void TLSInputMatricesFiller::addDIR3DContribution(const TDIR3D& meas, const TCAM& camera, TLSInputMatrices*  matrices){
	//Get the observation contribution
	DIR3DContrib contributions = fCGenerator.getDIR3DContrib(camera, meas);
	bool isProcessOK = addDIR3DBaseContrib(camera.instrumentPos, meas, meas.getFirstEquationIndex(), meas.getFirstObservationIndex(), contributions, matrices);

	if(!isProcessOK)
		throw std::runtime_error("Error when filling input design matrices of DIR3D measurement occurred.");
}

void TLSInputMatricesFiller::addDIR3DContribution(const TDIR3D& meas, const TTSTN::TROM& rom, const TTSTN& station, TLSInputMatrices*  matrices){
	/*
	//Get the first and last equation index
	MatrixIndex firstEqIdx = meas.getFirstEquationIndex();
	MatrixIndex firstObsIdx = meas.getFirstObservationIndex();

	//Get the observation contribution
	DIR3DContrib contributions = fCGenerator.getDIR3DContrib(station, rom, meas);
	bool isProcessOK = addDIR3DBaseContrib(station.instrumentPos, meas, firstEqIdx, firstObsIdx, contributions, matrices);

	// Add V0's contribution
	if(!rom.v0->isFixed()){
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, rom.v0->getFirstUidx(), contributions.fV0Contrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, rom.v0->getFirstUidx(), contributions.fV0Contrib[1]);
	}

	// Add instrument height contributions
	if(!station.instrumentHeightAdjustable->isFixed()){
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[1]); 
	}

	if(station.rot3D){
		if(station.rotX == nullptr || station.rotY == nullptr)
			throw std::runtime_error("TSTN can rotate freely, but rotation angle around X or Y axis is nullptr.");

		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.rotX->getFirstUidx(), contributions.fRxContrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.rotX->getFirstUidx(), contributions.fRxContrib[1]); 

		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.rotY->getFirstUidx(), contributions.fRyContrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.rotY->getFirstUidx(), contributions.fRyContrib[1]); 
	}

	if(!isProcessOK)
		throw std::runtime_error("Error when filling input design matrices of DIR3D measurement occurred.");
	*/
			//Get the first and last equation index
	MatrixIndex firstEqIdx = meas.getFirstEquationIndex();
	MatrixIndex firstObsIdx = meas.getFirstObservationIndex();

	bool isProcessOK = true;

	//Get the observation contribution
	DIR3DContrib contributions = fCGenerator.getDIR3DContrib(station, rom, meas);
	isProcessOK = isProcessOK && addDIR3DBaseContrib(station.instrumentPos, meas, firstEqIdx, firstObsIdx, contributions, matrices);

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

		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.rotX->getFirstUidx(), contributions.fRxContrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.rotX->getFirstUidx(), contributions.fRxContrib[1]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, station.rotX->getFirstUidx(), contributions.fRxContrib[2]); 

		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, station.rotY->getFirstUidx(), contributions.fRyContrib[0]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, station.rotY->getFirstUidx(), contributions.fRyContrib[1]); 
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, station.rotY->getFirstUidx(), contributions.fRyContrib[2]); 
	}

	if(!isProcessOK)
		throw std::runtime_error("Error when filling input design matrices of PLR3D measurement occurred.");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - SUPPORTING FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////

bool TLSInputMatricesFiller::addDIR3DBaseContrib(const TAdjustablePoint* stationPos, const TDIR3D& meas, const MatrixIndex firstEqIdx, const MatrixIndex firstObsIdx, const DIR3DContrib& contributions, TLSInputMatrices*  matrices){
/*	bool isProcessOK = true;

	if(!stationPos->isFixed()){
		if(!stationPos->isCoordinateFixed(0)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContribFirstEq.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContribSecondEq.getX().getValue());
            isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContribSecondEq.getX().getValue());
		}
		if(!stationPos->isCoordinateFixed(1)){ 
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContribFirstEq.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContribSecondEq.getY().getValue());
            isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContribSecondEq.getX().getValue());
		}
		if(!stationPos->isCoordinateFixed(2)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContribFirstEq.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContribSecondEq.getZ().getValue());
            isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContribSecondEq.getX().getValue());
		}
	}

	const TAdjustablePoint* targetPos = meas.targetPos;
	if(!targetPos->isFixed()){
		if(!targetPos->isCoordinateFixed(0)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContribFirstEq.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContribSecondEq.getX().getValue());
		}
		if(!targetPos->isCoordinateFixed(1)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContribFirstEq.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContribSecondEq.getY().getValue());
		}
		if(!targetPos->isCoordinateFixed(2)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContribFirstEq.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContribSecondEq.getZ().getValue());
		}
	}

	// Add contributions of transformations parameters 
	for(auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform){
		if(!itTgTransform->first.isFixed()){
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
		}
	}

	// Setting contributions for observations into a second design matrix
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx, contributions.fThetaContrib[0]);  // Theta contrib for the first equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx, contributions.fThetaContrib[1]); // Theta contrib for the second equation

	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx + 1, contributions.fPhiContrib[0]); // Phi contrib for the first equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx + 1, contributions.fPhiContrib[1]); // Phi contrib for the second equation

	//Setting the misclosure vector elements
	for(int i = 0; i<2; i++)
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + i,  contributions.fMisclosureVector[i]);


	// Add weight unknown matrix element
	if(contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit )
		throw std::runtime_error("Error when filling DIR3D contribution, variance is zero or too small, can not set weight matrix element.");
	else{
		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0/contributions.fObsVariance[0]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/contributions.fObsVariance[1]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);
	}

	return isProcessOK;
    */

	bool isProcessOK = true;
	// Add station's contributions into a first design matrix
	if(!stationPos->isFixed()){
		if(!stationPos->isCoordinateFixed(0)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContrib.firstEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContrib.secondEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContrib.thirdEqPtContrib.getX().getValue());
		}
		if(!stationPos->isCoordinateFixed(1)){ 
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContrib.firstEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContrib.secondEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContrib.thirdEqPtContrib.getY().getValue());
		}
		if(!stationPos->isCoordinateFixed(2)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContrib.firstEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContrib.secondEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContrib.thirdEqPtContrib.getZ().getValue());
		}
	}

	if(!meas.targetPos->isFixed()){
		if(!meas.targetPos->isCoordinateFixed(0)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, meas.targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContrib.firstEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, meas.targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContrib.secondEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, meas.targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContrib.thirdEqPtContrib.getX().getValue());
		}
		if(!meas.targetPos->isCoordinateFixed(1)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, meas.targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContrib.firstEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, meas.targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContrib.secondEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, meas.targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContrib.thirdEqPtContrib.getY().getValue());
		}
		if(!meas.targetPos->isCoordinateFixed(2)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, meas.targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContrib.firstEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, meas.targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContrib.secondEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, meas.targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContrib.thirdEqPtContrib.getZ().getValue());
		}
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
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx, contributions.fThetaContrib[0]);  // Theta contrib for the first equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx, contributions.fThetaContrib[1]); // Theta contrib for the second equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx, contributions.fThetaContrib[2]); // Theta contrib for the third equation

	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx + 1, contributions.fPhiContrib[0]); // Phi contrib for the first equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx + 1, contributions.fPhiContrib[1]); // Phi contrib for the second equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx + 1, contributions.fPhiContrib[2]); // Phi contrib for the third equation 

	//Setting the misclosure vector elements
	for(int i = 0; i<3; i++)
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + i,  contributions.fMisclosureVector[i]);


	// Add weight unknown matrix element
	if(contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit || contributions.fObsVariance[2] < nullLimit)
		throw std::runtime_error("Error when filling PLR3D contribution, variance is zero or too small, can not set weight matrix element.");
	else{

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0/(pow2q(meas.target.sigmaAngl)));
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, pow2q(meas.target.sigmaAngl));

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/(pow2q(meas.target.sigmaZenD)));
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, pow2q(meas.target.sigmaZenD));


#if 0
		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0/contributions.fObsVariance[0]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/contributions.fObsVariance[1]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);
		/*
		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0/contributions.fObsVariance[2]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, contributions.fObsVariance[2]);*/
#endif
	}

	return isProcessOK;
}
bool TLSInputMatricesFiller::addPLR3DBaseContrib(const TAdjustablePoint* stationPos, const TPLR3D& meas, const MatrixIndex firstEqIdx, const MatrixIndex firstObsIdx, const PLR3DContrib& contributions, TLSInputMatrices*  matrices){
	bool isProcessOK = true;
	// Add station's contributions into a first design matrix
	if(!stationPos->isFixed()){
		if(!stationPos->isCoordinateFixed(0)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContrib.firstEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContrib.secondEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(0), contributions.fStCoordContrib.thirdEqPtContrib.getX().getValue());
		}
		if(!stationPos->isCoordinateFixed(1)){ 
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContrib.firstEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContrib.secondEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(1), contributions.fStCoordContrib.thirdEqPtContrib.getY().getValue());
		}
		if(!stationPos->isCoordinateFixed(2)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContrib.firstEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContrib.secondEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, stationPos->getCoordinateUnknIndex(2), contributions.fStCoordContrib.thirdEqPtContrib.getZ().getValue());
		}
	}

	if(!meas.targetPos->isFixed()){
		if(!meas.targetPos->isCoordinateFixed(0)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, meas.targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContrib.firstEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, meas.targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContrib.secondEqPtContrib.getX().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, meas.targetPos->getCoordinateUnknIndex(0), contributions.fTgCoordContrib.thirdEqPtContrib.getX().getValue());
		}
		if(!meas.targetPos->isCoordinateFixed(1)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, meas.targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContrib.firstEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, meas.targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContrib.secondEqPtContrib.getY().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, meas.targetPos->getCoordinateUnknIndex(1), contributions.fTgCoordContrib.thirdEqPtContrib.getY().getValue());
		}
		if(!meas.targetPos->isCoordinateFixed(2)){
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, meas.targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContrib.firstEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1, meas.targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContrib.secondEqPtContrib.getZ().getValue());
			isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, meas.targetPos->getCoordinateUnknIndex(2), contributions.fTgCoordContrib.thirdEqPtContrib.getZ().getValue());
		}
	}

	if(!meas.target.distCorrectionAdjustable->isFixed()){
		int distCorUidx = meas.target.distCorrectionAdjustable->getFirstUidx();
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx, distCorUidx, contributions.fDistAndCsContrib[0]);
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 1,distCorUidx, contributions.fDistAndCsContrib[1]);
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(firstEqIdx + 2, distCorUidx, contributions.fDistAndCsContrib[2]);
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
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx, contributions.fThetaContrib[0]);  // Theta contrib for the first equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx, contributions.fThetaContrib[1]); // Theta contrib for the second equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx, contributions.fThetaContrib[2]); // Theta contrib for the third equation

	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx + 1, contributions.fPhiContrib[0]); // Phi contrib for the first equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx + 1, contributions.fPhiContrib[1]); // Phi contrib for the second equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx + 1, contributions.fPhiContrib[2]); // Phi contrib for the third equation 

	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx, firstObsIdx + 2, contributions.fDistAndCsContrib[0]);  // Distance (s) contrib for the first equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 1, firstObsIdx + 2, contributions.fDistAndCsContrib[1]);  // Distance (s) contrib for the second equation
	isProcessOK = isProcessOK && matrices->setSecondDgnMtrxElement(firstEqIdx + 2, firstObsIdx + 2, contributions.fDistAndCsContrib[2]);  // Distance (s) contrib for the second equation

	//Setting the misclosure vector elements
	for(int i = 0; i<3; i++)
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + i,  contributions.fMisclosureVector[i]);


	// Add weight unknown matrix element
	if(contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit || contributions.fObsVariance[2] < nullLimit)
		throw std::runtime_error("Error when filling PLR3D contribution, variance is zero or too small, can not set weight matrix element.");
	else{
		
		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0/(pow2q(meas.target.sigmaAngl)));
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, pow2q(meas.target.sigmaAngl));

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/(pow2q(meas.target.sigmaZenD)));
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, pow2q(meas.target.sigmaZenD));

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0/(pow2q(meas.target.sigmaDist /*+ meas.getScalar()*meas.target.ppmDist*/)));
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, pow2q(meas.target.sigmaDist /*+ meas.getScalar()*meas.target.ppmDist*/));

/*		
		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0/contributions.fObsVariance[0]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0/contributions.fObsVariance[1]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);

		isProcessOK = isProcessOK && matrices->setWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0/contributions.fObsVariance[2]);
		isProcessOK = isProcessOK && matrices->setWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, contributions.fObsVariance[2]);*/

	}

	return isProcessOK;
}

bool TLSInputMatricesFiller::addTransformationContribution(const TAdjustableHelmertTransformation& trafo, const TransformationContrib& trContrib, int eqIndex, TLSInputMatrices* matrices){
	bool isProcessOK = true;

	if(!trafo.isTranslationFixed(0))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getTranslationUnknIndex(0) , trContrib.fTranslContrib.getX().getValue());
	if(!trafo.isTranslationFixed(1))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getTranslationUnknIndex(1) , trContrib.fTranslContrib.getY().getValue());
	if(!trafo.isTranslationFixed(2))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getTranslationUnknIndex(2) , trContrib.fTranslContrib.getZ().getValue());

	if(!trafo.isRotationFixed(0))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getRotationUnknIndex(0) , trContrib.fRotationContrib.getX().getValue());
	if(!trafo.isRotationFixed(1))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getRotationUnknIndex(1) , trContrib.fRotationContrib.getY().getValue());
	if(!trafo.isRotationFixed(2))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getRotationUnknIndex(2) , trContrib.fRotationContrib.getZ().getValue());
			
	if(!trafo.isScaleFixed())
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIndex, trafo.getScaleUnknIndex() , trContrib.fScaleContrib);

	return isProcessOK;
}


bool TLSInputMatricesFiller::addPointContribution(const TAdjustablePoint& pointAdj, const TFreeVector& pointContrib, int eqIdx, TLSInputMatrices* matrices){
	bool isProcessOK = true;

	if(!pointAdj.isCoordinateFixed(0))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, pointAdj.getCoordinateUnknIndex(0), pointContrib.getX().getValue());
	if(!pointAdj.isCoordinateFixed(1))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, pointAdj.getCoordinateUnknIndex(1), pointContrib.getY().getValue());
	if(!pointAdj.isCoordinateFixed(2))
		isProcessOK = isProcessOK && matrices->setFirstDgnMtrxElement(eqIdx, pointAdj.getCoordinateUnknIndex(2), pointContrib.getZ().getValue());

	return isProcessOK;
}


bool	TLSInputMatricesFiller::fillWeightUnkMtrx(TLGCData* projData, TLSInputMatrices* matrices){
	bool fillOK = true;
	auto& outputMessages(projData->fOutputFileWriter);

	for (auto it = projData->points.objects.begin(); it != projData->points.objects.end(); ++it){	
		if(it->hasStandDeviations()){
			for(int i = 0; i<3; i++){
				TReal variance = pow2(it->getStandDev(i));

				if(variance < nullLimit){
					fillOK = false;
					outputMessages << TFileLogger::e_logType::LOG_ERROR <<  "Standard deviation assigned to a cordinate of a point " + it->getName() + " is too small, causes zero division.";
				}
				else
					matrices->setWeightUnkMtrxElement(it->getCoordinateUnknIndex(i), it->getCoordinateUnknIndex(i), 1/variance);
			}
		}
	}

	for (auto it( projData->tree.begin()); it != projData->tree.end(); ++it){	
		auto& frame(it.node->data.get()->frame);
		if(frame.hasStandDev()){
			for(int i = 0;i<3;i++){
				if(frame.hasRotationStandDev(i)){
					TReal variance = pow2(frame.getRotationStandDev(i).rad());
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
				TReal variance = pow2(frame.getScaleStandDev().getMetresValue());
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
