////////////////////////////////////////////////////////////////////
// Class responsible for reading the least squares calculation measurements and constraints
//	and filling the least squares matrices,
//	specific to a least squares calculation
////////////////////////////////////////////////////////////////////////////////////
#include "TLSInputMatricesFiller.h"

#include <Logger.hpp>
#include <TLGCData.h>

#include "TDataAnalyzer.h"
#include "TLSInputMatrices.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////////////////
TLSInputMatricesFiller::TLSInputMatricesFiller(const TDataTree *tree, const TRefSystemFactory::ERefFrame &referentiel, const TLGCData &data) :
	fPointTransformer(tree, referentiel), fCGenerator(fPointTransformer)
{
}

TLSInputMatricesFiller::~TLSInputMatricesFiller()
{ // Destructor
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool TLSInputMatricesFiller::fillMatrices(TLGCData *projData, TLSInputMatrices *matrices)
{
	bool fillOK = true;
	auto &outputMessages(projData->getFileLogger());

	try
	{
		// Input matrices have to be initialized each time they are filled.
		initMatriceDimension(*projData, matrices);
		// Contribution generator transformations need to update the transformations it stores.
		fPointTransformer.updateTransformations();

		// LGC uses only parametric measurement models, so the B matrix is -Identity
		fillOK &= matrices->setSecondDgnMtrxToMinusIdentity();

		fillOK &= fillParameterWeights(projData, matrices);
		fillOK &= fillSlaveConstraints(projData, matrices);
		fillOK &= fillPointGroupConstraints(projData, matrices);

		// Itteration through the nodes of the tree
		for (TDataTreeIterator itTree = projData->getTree().begin(); itTree != projData->getTree().end(); itTree++)
		{
			// In every node iterate through the Total station measurements (TSTN)
			for (auto &itTSTN : itTree.node->data->measurements.fTSTN)
			{
				if (itTree.node->data->isROOTNode())
				{
					// In every TSTN iterate through ROMS and add contributions for every observation type
					for (auto &itROM : itTSTN->roms)
					{
						addPLR3DContributions(itROM, itTSTN, matrices); // Process all the PLR3D measurement in this ROM
						addHorAngContributions(itROM, itTSTN, matrices); // Process all the ANGL measurement in this ROM
						addSpaDistContributions(itROM->measDIST, itTSTN, matrices);
						addZenDistContributions(itROM->measZEND, itTSTN, matrices);
						addHorDistContributions(itROM->measDHOR, itTSTN, matrices);
						addECTHContributions(itROM, itTSTN, matrices);
						addECDIRContributions(itROM, itTSTN, matrices);
					}
				}
				else
				{
					for (auto &itROM : itTSTN->roms)
					{
						addHorAngContributionsFrame(itROM, itTSTN, matrices);
						addSpaDistContributionsFrame(itROM->measDIST, itTSTN, matrices);
						addZenDistContributionsFrame(itROM->measZEND, itTSTN, matrices);
					}
				}
			}

			// In every node iterate through camera (TCAM) measurements
			for (auto &itCAM : itTree.node->data->measurements.fCAM)
			{
				addUVDContribution(itCAM, matrices);
				addUVECContribution(itCAM, matrices);
			}

			// In every node iterate through the EDM (TEDM) measurements
			for (auto &itEDM : itTree.node->data->measurements.fEDM)
				addDSPTContribution(itEDM.measDSPT, itEDM, matrices);

			// In every node iterate through the LEVEL measurements
			for (auto &itLEVEL : itTree.node->data->measurements.fLEVEL)
				addLevelStContributions(itLEVEL, matrices);

			// In every node iterate through the ECHO measurements
			for (auto &itECHO : itTree.node->data->measurements.fECHO)
				addECHOContributions(itECHO, matrices);

			// In every node iterate through the ECVE measurements
			for (auto &itECVE : itTree.node->data->measurements.fECVE)
				addECVEContributions(itECVE, matrices);

			// In every node iterate through the ECVE measurements
			for (auto &itECSP : itTree.node->data->measurements.fECSP)
				addECSPContributions(itECSP, matrices);

			// In every node iterate through the LEVEL measurements
			for (auto &itORIE : itTree.node->data->measurements.fORIE)
				addORIEContributions(itORIE, matrices);

			// In every node iterate through the INCLY measurements
			for (auto &itINCLY : itTree.node->data->measurements.fINCLY)
				addINCLYContributions(itINCLY, matrices);

			// In every node iterate through the ECWS measurements
			for (auto &itECWS : itTree.node->data->measurements.fECWS)
				addECWSContributions(itECWS, matrices);

			// In every node iterate through the ECWI measurements
			for (auto &itECWI : itTree.node->data->measurements.fECWI)
				addECWIContributions(itECWI, matrices);

			addDVERContribution(itTree.node->data->measurements.fDVER, matrices);

			addRADIContributions(itTree.node->data->measurements.fRADI, matrices);

			addOBSXYZContributions(itTree.node->data->measurements.fOBSXYZ, matrices);

			// add the initialised PDOR
			if (itTree.node->data->measurements.fPDOR.isInitialised())
				addPDORContributions(itTree.node->data->measurements.fPDOR, matrices);
		}
	}
	catch (std::exception const &excp)
	{
		outputMessages << TFileLogger::e_logType::LOG_ERROR << excp.what();
		fillOK = false;
	}

	if (projData->getFileLogger().hasErrors())
		fillOK = false;

	// creates sparse matrix objects from the triplet lists
	matrices->finalizeMatrices();

	return fillOK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
//! initialise the dimensions of the input matrices
void TLSInputMatricesFiller::initMatriceDimension(const TLGCData &projData, TLSInputMatrices *matrices)
{
	if ((projData.fUEOIndices.EIndex == 0))
		throw std::runtime_error("Equation index in LS matrices is null.");

	if ((projData.fUEOIndices.OIndex == 0))
		throw std::runtime_error("Observation index in LS matrices is null.");

	matrices->initMatrices(projData.fUEOIndices);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - fill of models with 1 equation
///////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSInputMatricesFiller::addSpaDistContributions(std::list<TLINE> &distMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	DistMeasContrib contributions;

	for (auto meas(distMeas.begin()); meas != distMeas.end(); ++meas)
	{
		// These indices should be equal as the residuals are associated with the measurements
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getSpatialDistanceContrib(station, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station's contributions into a first design matrix
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding Distance correction contribution
		if (!meas->target.distCorrectionAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, meas->target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

		// Adding instrument height contribution
		if (!station->instrumentHeightAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, station->instrumentHeightAdjustable->getFirstUidx(), contributions.fHIContrib);

		// Adding contributions for STATION transformations parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Spatial Distance contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of Spatial Distance measurement occurred.");
	}
}

void TLSInputMatricesFiller::addSpaDistContributionsFrame(std::list<TLINE> &distMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	DistMeasContribFrame contributions;

	for (auto meas(distMeas.begin()); meas != distMeas.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getSpatialDistanceContribInFrame(station, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station's contributions into a first design matrix
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding Distance correction contribution
		if (!meas->target.distCorrectionAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, meas->target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Spatial Distance contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of Spatial Distance measurement occurred.");
	}
}

void TLSInputMatricesFiller::addHorAngContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContrib contributions;

	for (auto meas(rom->measANGL.begin()); meas != rom->measANGL.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getHorAnglContrib(station, rom, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedAngle = TAngle(sqrt(contributions.fObsVariance));

		// Add station contributions
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Add V0 contribution
		if (!rom->v0->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, rom->v0->getFirstUidx(), contributions.fV0Contrib);

		// Adding contributions of STATION transformation's parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions of TARGET transformation's parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector's contribution
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Horizontal Angle contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Horizontal angle (ANGL) measurement.");
	}
}

void TLSInputMatricesFiller::addHorAngContributionsFrame(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContribFrame contributions;

	for (auto meas(rom->measANGL.begin()); meas != rom->measANGL.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getHorAnglContribInFrame(station, rom, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedAngle = TAngle(sqrt(contributions.fObsVariance));

		// Add station contributions
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Add contributions of transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector's contribution
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Horizontal Angle contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Horizontal angle (ANGL) measurement.");
	}
}

void TLSInputMatricesFiller::addZenDistContributions(std::list<TZEND> &zendMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContrib contributions;

	for (auto meas(zendMeas.begin()); meas != zendMeas.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getZenDistContrib(station, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedAngle = TAngle(sqrt(contributions.fObsVariance));

		// Add station contributions
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Add instrument height contribution
		if (!station->instrumentHeightAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, station->instrumentHeightAdjustable->getFirstUidx(), contributions.fHIContrib);

		// Adding contributions for STATION transformations parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector values
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Zenith Distance contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Zenith Distance (vertical angle) measurement.");
	}
}

void TLSInputMatricesFiller::addZenDistContributionsFrame(std::list<TZEND> &zendMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContribFrame contributions;

	for (auto meas(zendMeas.begin()); meas != zendMeas.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getZenDistContribInFrame(station, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedAngle = TAngle(sqrt(contributions.fObsVariance));

		// Add station contributions
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector values
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Zenith Distance contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Zenith Distance (vertical angle) measurement.");
	}
}

void TLSInputMatricesFiller::addHorDistContributions(std::list<TLINE> &dhorMeas, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	HorDistContrib contributions;

	for (auto meas(dhorMeas.begin()); meas != dhorMeas.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getHorDistContrib(station, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station contributions into a first design matrix
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding Distance Correction contribution
		if (!meas->target.distCorrectionAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, meas->target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

		// Adding contributions for STATION transformations parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Horizontal distance contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Horizontal distance measurement.");
	}
}

void TLSInputMatricesFiller::addECTHContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	ECTHContrib contributions;

	for (auto &meas : rom->measECTH)
	{
		eqIdx = meas.getFirstEquationIndex();
		obsIdx = meas.getFirstObservationIndex();

		contributions = fCGenerator.getECTHContrib(station, rom, meas); // Get the observation contribution

		// Update the sigma
		meas.target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station's contributions into a first design matrix
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fTSTNPtContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas.targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fScaleStationPtContrib, eqIdx, matrices);

		// Add V0 contribution
		if (!rom->v0->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, rom->v0->getFirstUidx(), contributions.fV0Contrib);

		// Adding contributions for STATION transformations parameters
		for (auto &itStTransform : contributions.fTSTNPtTransformContrib)
		{
			if (!itStTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform.first, itStTransform.second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters
		for (auto &itTgTransform : contributions.fStTransformContrib)
		{
			if (!itTgTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform.first, itTgTransform.second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas.getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECTH contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of ECTH measurement occurred.");
	}
}

void TLSInputMatricesFiller::addECDIRContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	ECTHContrib contributions;

	for (auto &meas : rom->measECDIR)
	{
		eqIdx = meas.getFirstEquationIndex();
		obsIdx = meas.getFirstObservationIndex();

		contributions = fCGenerator.getECDIRContrib(station, rom, meas); // Get the observation contribution

		// Update the sigma
		meas.target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station's contributions into a first design matrix
		if (!station->instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*station->instrumentPos, contributions.fTSTNPtContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas.targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fScaleStationPtContrib, eqIdx, matrices);

		// Add V0 contribution
		if (!rom->v0->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, rom->v0->getFirstUidx(), contributions.fV0Contrib);

		// Adding contributions for STATION transformations parameters
		for (auto &itStTransform : contributions.fTSTNPtTransformContrib)
		{
			if (!itStTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform.first, itStTransform.second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters
		for (auto &itTgTransform : contributions.fStTransformContrib)
		{
			if (!itTgTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform.first, itTgTransform.second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas.getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECDIR contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of ECDIR measurement occurred.");
	}
}

void TLSInputMatricesFiller::addLevelStContributions(TLEVEL &levelSt, TLSInputMatrices *matrices)
{
	DLEVContrib contributions;
	HorDistContribLEVEL contributionsDHOR;

	bool isProcessOK = true;
	for (auto itDLEV(levelSt.measDLEV.begin()); itDLEV != levelSt.measDLEV.end(); ++itDLEV)
	{
		MatrixIndex eqIdx = itDLEV->getFirstEquationIndex();
		MatrixIndex obsIdx = itDLEV->getFirstObservationIndex();

		contributions = fCGenerator.getDLEVContrib(levelSt, *itDLEV); // Get the observation contribution

		// Update the sigma
		itDLEV->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add staff's contributions
		if (!itDLEV->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*itDLEV->targetPos, contributions.fStaffContrib, eqIdx, matrices);

		// Add reference point's contributions
		if (!levelSt.fMeasuredPlane->getReferencePoint()->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*levelSt.fMeasuredPlane->getReferencePoint(), contributions.fRefPtContrib, eqIdx, matrices);

		// Adding contributions of STATION transformation's parameters
		for (auto itStaffTransform(contributions.fStaffTransformContrib.begin()); itStaffTransform != contributions.fStaffTransformContrib.end(); ++itStaffTransform)
		{
			if (!itStaffTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStaffTransform->first, itStaffTransform->second, eqIdx, matrices);
		}

		// Adding contributions of TARGET transformation's parameters
		for (auto itRefPTTransform(contributions.fRefPtTransformContrib.begin()); itRefPTTransform != contributions.fRefPtTransformContrib.end(); ++itRefPTTransform)
		{
			if (!itRefPTTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform->first, itRefPTTransform->second, eqIdx, matrices);
		}

		// Adding contribution to a reference point distance
		if (!levelSt.ihfix)
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, levelSt.fMeasuredPlane->getRefPtDistUnknIndex(), contributions.fRefPtDistContrib);

		// Adding Distance correction contribution
		if (!levelSt.instrument.collAngleAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, levelSt.instrument.collAngleAdjustable->getFirstUidx(), contributions.fCollAngleContrib);

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (itDLEV->getDistance() - contributions.fCalcMeas));

		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling DLEV contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		// In a case that optional DHOR measurement is done
		if (itDLEV->dhor)
		{ // i.e. !=nullptr
			MatrixIndex eqIdxHd = itDLEV->dhor->getFirstEquationIndex();
			MatrixIndex obsIdxHd = itDLEV->dhor->getFirstObservationIndex();

			contributionsDHOR = fCGenerator.getHorDistContrib(levelSt.fMeasuredPlane->getReferencePoint(), *itDLEV->dhor); // Get the observation contribution

			// Update the sigma
			itDLEV->dhor->target.sigmaCombinedDHor = TLength(sqrt(contributionsDHOR.fObsVariance));

			// Add levelling staff's contributions
			if (!itDLEV->dhor->targetPos->isFixed())
				isProcessOK = isProcessOK && addPointContribution(*itDLEV->dhor->targetPos, contributionsDHOR.fStaffContrib, eqIdxHd, matrices);

			// Add reference point's contributions
			if (!levelSt.fMeasuredPlane->getReferencePoint()->isFixed())
				isProcessOK = isProcessOK && addPointContribution(*levelSt.fMeasuredPlane->getReferencePoint(), contributionsDHOR.fRefPtContrib, eqIdxHd, matrices);

			// Adding contributions of STATION transformation's parameters
			for (auto itStaffTransform(contributionsDHOR.fStaffTransformContrib.begin()); itStaffTransform != contributionsDHOR.fStaffTransformContrib.end(); ++itStaffTransform)
			{
				if (!itStaffTransform->first.isFixed())
					isProcessOK = isProcessOK && addTransformationContribution(itStaffTransform->first, itStaffTransform->second, eqIdxHd, matrices);
			}

			// Adding contributions of TARGET transformation's parameters
			for (auto itRefPTTransform(contributionsDHOR.fRefPtTransformContrib.begin()); itRefPTTransform != contributionsDHOR.fRefPtTransformContrib.end(); ++itRefPTTransform)
			{
				if (!itRefPTTransform->first.isFixed())
					isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform->first, itRefPTTransform->second, eqIdxHd, matrices);
			}

			// Set Misclosure vector
			isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdxHd, -1.0 * (itDLEV->dhor->getDistance() - contributionsDHOR.fCalcMeas));

			if (contributions.fObsVariance < nullLimit)
				throw std::runtime_error("Error when filling DLEV:DHOR contribution, variance is zero or too small, can not set weight matrix element.");
			else
			{
				isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdxHd, obsIdxHd, 1.0 / contributionsDHOR.fObsVariance);
				isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdxHd, obsIdxHd, contributionsDHOR.fObsVariance);
			}
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of DLEV.");
	}
}

void TLSInputMatricesFiller::addORIEContributions(TORIEROM &orieROM, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	AnglMeasContrib contributions;

	for (auto &meas : orieROM.measORIE)
	{
		eqIdx = meas.getFirstEquationIndex();
		obsIdx = meas.getFirstObservationIndex();

		contributions = fCGenerator.getOrieContrib(orieROM, meas); // Get the observation contribution

		// Update the sigma
		meas.target.sigmaCombinedAngle = TAngle(sqrt(contributions.fObsVariance));

		// Add station contributions
		if (!orieROM.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*orieROM.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions
		if (!meas.targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas.targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Add contributions of transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions of STATION transformation's parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Adding contributions of TARGET transformation's parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Add Misclosure vector's contribution
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas.getAngle() - contributions.fCalcMeas).getRadiansValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Horizontal Angle contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of Horizontal angle (ANGL) measurement.");
	}
}

void TLSInputMatricesFiller::addECHOContributions(TECHOROM &echoROM, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	ECHOContrib contributions;
	for (auto itECHO(echoROM.measECHO.begin()); itECHO != echoROM.measECHO.end(); ++itECHO)
	{
		MatrixIndex eqIdx = itECHO->getFirstEquationIndex();
		MatrixIndex obsIdx = itECHO->getFirstObservationIndex();

		contributions = fCGenerator.getECHOContrib(echoROM, *itECHO); // Get the observation contribution

		// Update the sigma
		itECHO->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station's contributions
		if (!itECHO->targetPos->isFixed())
			isProcessOK = isProcessOK
				&& addPointContribution(*itECHO->targetPos, contributions.fStationContrib, eqIdx, matrices); /*'Target' in ECHO means station, there is no real target in ECHO.*/

		// Adding contributions for STATION transformation parameters
		for (auto itStationTransform(contributions.fStTransformContrib.begin()); itStationTransform != contributions.fStTransformContrib.end(); ++itStationTransform)
		{
			if (!itStationTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStationTransform->first, itStationTransform->second, eqIdx, matrices);
		}

		// Adding controbution to a theta angle, which is for ECHO always variable
		isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, echoROM.fMeasuredPlane->getThetaUnknIndex(), contributions.fThetaPlaneAngleContrib);

		// Adding contribution to a reference point distance, which is at any case variable
		isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, echoROM.fMeasuredPlane->getRefPtDistUnknIndex(), contributions.fRefPtDistContrib);

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (itECHO->getDistance() - contributions.fCalcMeas));

		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECHO contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of ECHO.");
	}
}

void TLSInputMatricesFiller::addECVEContributions(TECVEROM &ecveROM, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	ScaleMeasContrib contributions;
	for (auto itECVE(ecveROM.measECVE.begin()); itECVE != ecveROM.measECVE.end(); ++itECVE)
	{
		MatrixIndex eqIdx = itECVE->getFirstEquationIndex();
		MatrixIndex obsIdx = itECVE->getFirstObservationIndex();

		contributions = fCGenerator.getECVEContrib(ecveROM, *itECVE); // Get the observation contribution

		// Update the sigma
		itECVE->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add point on the line contributions
		if (!ecveROM.fMeasuredLine->getLinePoint()->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*ecveROM.fMeasuredLine->getLinePoint(), contributions.fPointLineContrib, eqIdx, matrices);

		// Adding contributions of the point on the line transformation's parameters
		for (auto itRefPTTransform(contributions.fPtLineTransformContrib.begin()); itRefPTTransform != contributions.fPtLineTransformContrib.end(); ++itRefPTTransform)
		{
			if (!itRefPTTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform->first, itRefPTTransform->second, eqIdx, matrices);
		}

		// Add station's contributions
		if (!itECVE->targetPos->isFixed())
			isProcessOK = isProcessOK
				&& addPointContribution(*itECVE->targetPos, contributions.fStationContrib, eqIdx, matrices); /*'Target' in ECHO means station, there is no real target in ECHO.*/

		// Adding contributions for STATION transformation parameters
		for (auto itStationTransform(contributions.fStTransformContrib.begin()); itStationTransform != contributions.fStTransformContrib.end(); ++itStationTransform)
		{
			if (!itStationTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStationTransform->first, itStationTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (itECVE->getDistance() - contributions.fCalcMeas));

		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECVE contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of ECVE.");
	}
}

void TLSInputMatricesFiller::addECSPContributions(TECSPROM &ecspRom, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	ECSPContrib contributions;

	for (auto &itECSP : ecspRom.measECSP)
	{
		eqIdx = itECSP.getFirstEquationIndex();
		obsIdx = itECSP.getFirstObservationIndex();

		contributions = fCGenerator.getECSPContrib(ecspRom, itECSP); // Get the observation contribution

		// Update the sigma
		itECSP.target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add point on the line contributions
		if (!ecspRom.p1->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*ecspRom.p1, contributions.fPointLineContrib1, eqIdx, matrices);
		if (!ecspRom.p2->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*ecspRom.p2, contributions.fPointLineContrib2, eqIdx, matrices);

		// Adding contributions of the point on the line transformation's parameters
		for (auto &itRefPTTransform : contributions.fPtLineTransformContrib1)
		{
			if (!itRefPTTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform.first, itRefPTTransform.second, eqIdx, matrices);
		}
		for (auto &itRefPTTransform : contributions.fPtLineTransformContrib2)
		{
			if (!itRefPTTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itRefPTTransform.first, itRefPTTransform.second, eqIdx, matrices);
		}

		// Add station's contributions
		if (!itECSP.targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*itECSP.targetPos, contributions.fStationContrib, eqIdx, matrices);

		// Adding contributions for STATION transformation parameters
		for (auto itStationTransform(contributions.fStTransformContrib.begin()); itStationTransform != contributions.fStTransformContrib.end(); ++itStationTransform)
		{
			if (!itStationTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStationTransform->first, itStationTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (itECSP.getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECSP contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of ECSP measurement occurred.");
	}
}

void TLSInputMatricesFiller::addDSPTContribution(std::list<TDSPT> &dsptMeas, const TEDM &edmST, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	DistMeasContrib contributions;

	for (auto meas(dsptMeas.begin()); meas != dsptMeas.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getDSPTContrib(edmST, *meas); // Get the observation contribution

		// Update the sigma
		meas->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station's contributions into a first design matrix
		if (!edmST.instrumentPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*edmST.instrumentPos, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target's contributions into a first design matrix
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding Distance correction contribution
		if (!meas->target.distCorrectionAdjustable->isFixed())
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, meas->target.distCorrectionAdjustable->getFirstUidx(), contributions.fDistCorrection);

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions for STATION transformations parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling DSPT contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of DSPT measurement occurred.");
	}
}

void TLSInputMatricesFiller::addDVERContribution(const std::list<TDVER> &dverMeas, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	DVERContrib contributions;

	for (auto meas(dverMeas.begin()); meas != dverMeas.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getDVERContrib(*meas); // Get the observation contribution

		// Add station contributions into a first design matrix
		if (!meas->station->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->station, contributions.fStCoordContrib, eqIdx, matrices);

		// Add target contributions into a first design matrix
		if (!meas->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions for STATION transformations parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (meas->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling DVER contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of DVER measurement occurred.");
	}
}

void TLSInputMatricesFiller::addPDORContributions(const TPdorObs &pdorObs, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	PtOrientationContrib contributions;

	eqIdx = pdorObs.getFirstEquationIndex();
	obsIdx = pdorObs.getFirstObservationIndex();

	contributions = fCGenerator.getPDORContrib(pdorObs); // Get the observation contribution

	// Add point contribution for the point of orientation
	//  Add target contributions
	if (!pdorObs.orientationPt->isFixed())
		isProcessOK = isProcessOK && addPointContribution(*pdorObs.orientationPt, contributions.oriPointContrib, eqIdx, matrices);

	// Add contributions of transformations parameters
	for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform)
	{
		if (!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Adding contributions of TARGET transformation's parameters
	for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform)
	{
		if (!itTgTransform->first.isFixed())
			isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
	}

	// Add Misclosure vector's contribution
	// GKA (26/09/2019) : change of definition to a bearing, changes also made in TContributionsGenerator.cpp
	isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, contributions.calcmeas);

	// Add weight unknown matrix element
	if (pdorObs.getSigma() < nullLimit)
		throw std::runtime_error("Error when filling pdor contribution, variance is zero or too small, can not set weight matrix element.");
	else
	{
		isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / pow2(pdorObs.getSigma().getRadiansValue()));
		isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, pow2(pdorObs.getSigma().getRadiansValue()));
	}

	if (!isProcessOK)
		throw std::runtime_error("Error occurred during filling input design matrices of PDOR measurement.");
}

void TLSInputMatricesFiller::addRADIContributions(const std::list<TRADI> &radiMeas, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	PtOrientationContrib contributions;

	for (auto meas(radiMeas.begin()); meas != radiMeas.end(); ++meas)
	{
		eqIdx = meas->getFirstEquationIndex();
		obsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getRADIContrib(*meas); // Get the observation contribution

		// Add  contributions into a first design matrix
		if (!meas->station->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*meas->station, contributions.oriPointContrib, eqIdx, matrices);

		// Add contributions of transformations parameters
		for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Adding contributions for TARGET transformations parameters
		for (auto itTgTransform(contributions.fRefPtTransformContrib.begin()); itTgTransform != contributions.fRefPtTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (contributions.calcmeas));

		// Add weight matrix element
		if (pow2(meas->getObservedStDev()) < nullLimit)
			throw std::runtime_error("Error when filling RADI contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / pow2(meas->getObservedStDev()));
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, pow2(meas->getObservedStDev()));
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of RADI measurement occurred.");
	}
}

void TLSInputMatricesFiller::addOBSXYZContributions(const std::list<TOBSXYZ> &obsxyzMeas, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex firstEqIdx = -1;
	MatrixIndex firstObsIdx = -1;
	OBSXYZContrib contributions;

	for (auto meas(obsxyzMeas.begin()); meas != obsxyzMeas.end(); ++meas)
	{
		firstEqIdx = meas->getFirstEquationIndex();
		firstObsIdx = meas->getFirstObservationIndex();

		// Get the observation contribution
		contributions = fCGenerator.getOBSXYZContrib(*meas);

		// Add contributions for coordinates
		if (!meas->station->isFixed())
		{
			isProcessOK = isProcessOK && addPointContribution(*meas->station, TFreeVector(contributions.fTgCoordContrib.contrib.row(0)), firstEqIdx, matrices);
			isProcessOK = isProcessOK && addPointContribution(*meas->station, TFreeVector(contributions.fTgCoordContrib.contrib.row(1)), firstEqIdx + 1, matrices);
			isProcessOK = isProcessOK && addPointContribution(*meas->station, TFreeVector(contributions.fTgCoordContrib.contrib.row(2)), firstEqIdx + 2, matrices);
		}

		// Add contributions of transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
			{
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.thirdEquationTransContrib, firstEqIdx + 2, matrices);
			}
		}

		// Set Misclosure vector
		for (int i = 0; i < 3; i++)
			isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + i, contributions.fMisclosureVector[i]);

		// Add weight matrix element
		if (pow2(meas->getXObservedStDev()) < nullLimit)
			throw std::runtime_error("Error when filling OBSXYZ contribution, X variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0 / pow2(meas->getXObservedStDev()));
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx, firstObsIdx, pow2(meas->getXObservedStDev()));
		}

		if (pow2(meas->getYObservedStDev()) < nullLimit)
			throw std::runtime_error("Error when filling OBSXYZ contribution, Y variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0 / pow2(meas->getYObservedStDev()));
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, pow2(meas->getYObservedStDev()));
		}

		if (pow2(meas->getZObservedStDev()) < nullLimit)
			throw std::runtime_error("Error when filling OBSXYZ contribution, Z variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0 / pow2(meas->getZObservedStDev()));
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, pow2(meas->getZObservedStDev()));
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of RADI measurement occurred.");
	}
}

void TLSInputMatricesFiller::addINCLYContributions(TINCLYROM &inclyROM, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex eqIdx = -1;
	MatrixIndex obsIdx = -1;
	INCLYContrib contributions;

	for (auto &itINCLY : inclyROM.measINCLY)
	{
		eqIdx = itINCLY.getFirstEquationIndex();
		obsIdx = itINCLY.getFirstObservationIndex();

		contributions = fCGenerator.getINCLYContrib(inclyROM, itINCLY); // Get the observation contribution

		// Update the sigma
		itINCLY.target.sigmaCombinedAngle = TAngle(sqrt(contributions.fObsVariance));

		// Adding contributions for STATION transformation parameters
		for (auto &itStTransform : contributions.fStTransformContrib)
		{
			if (!itStTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform.first, itStTransform.second, eqIdx, matrices);
		}

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (itINCLY.getAngle() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling Incly contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of Incly measurement occurred.");
	}
}

void TLSInputMatricesFiller::addECWSContributions(TECWSROM &ecwsROM, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;

	for (auto itECWS(ecwsROM.measECWS.begin()); itECWS != ecwsROM.measECWS.end(); ++itECWS)
	{
		MatrixIndex eqIdx = itECWS->getFirstEquationIndex();
		MatrixIndex obsIdx = itECWS->getFirstObservationIndex();

		ECWSContrib contributions = fCGenerator.getECWSContrib(ecwsROM, *itECWS); // Get the observation contribution

		// Update the sigma
		itECWS->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance));

		// Add station's contributions
		if (!itECWS->targetPos->isFixed())
			isProcessOK = isProcessOK && addPointContribution(*itECWS->targetPos, contributions.fTgCoordContrib, eqIdx, matrices);

		// Adding contributions for STATION transformation parameters
		for (auto &itStTransform : contributions.fWSTransformContrib)
		{
			if (!itStTransform.first.isFixed())
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform.first, itStTransform.second, eqIdx, matrices);
		}

		// Adding controbution to a WS Height, which is at any case variable
		isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, ecwsROM.fMeasuredWSHeight->getFirstUidx(), contributions.fWSContrib);

		// Set Misclosure vector
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(eqIdx, -1.0 * (itECWS->getDistance() - contributions.fCalcMeas));

		// Add weight unknown matrix element
		if (contributions.fObsVariance < nullLimit)
			throw std::runtime_error("Error when filling ECWS contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(obsIdx, obsIdx, 1.0 / contributions.fObsVariance);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(obsIdx, obsIdx, contributions.fObsVariance);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error occurred during filling input design matrices of ECWS.");
	}
}

void TLSInputMatricesFiller::addECWIContributions(TECWIROM &ecwiROM, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;

	for (auto itECWI(ecwiROM.measECWI.begin()); itECWI != ecwiROM.measECWI.end(); ++itECWI)
	{
		MatrixIndex firstEqIdx = itECWI->getFirstEquationIndex();
		MatrixIndex firstObsIdx = itECWI->getFirstObservationIndex();

		ECWIContrib contributions = fCGenerator.getECWIContrib(ecwiROM, *itECWI); // Get the observation contribution

		// Update the sigma
		itECWI->target.sigmaCombinedX = TLength(sqrt(contributions.fObsVariance[0]));
		itECWI->target.sigmaCombinedZ = TLength(sqrt(contributions.fObsVariance[1]));

		// Adding station's contributions
		if (!itECWI->targetPos->isFixed())
		{
			isProcessOK &= addPointContribution(*itECWI->targetPos, contributions.fStCoordContrib[0], firstEqIdx, matrices)
				&& addPointContribution(*itECWI->targetPos, contributions.fStCoordContrib[1], firstEqIdx + 1, matrices);
		}

		// Adding contributions to the transformation parameters
		for (auto &itStTransform : contributions.fWireFirstEqTransformContrib)
		{
			if (!itStTransform.first.isFixed())
			{
				isProcessOK &= addTransformationContribution(itStTransform.first, itStTransform.second, firstEqIdx, matrices);
			}
		}

		for (auto &itStTransform : contributions.fWireSecondEqTransformContrib)
		{
			if (!itStTransform.first.isFixed())
			{
				isProcessOK &= addTransformationContribution(itStTransform.first, itStTransform.second, firstEqIdx + 1, matrices);
			}
		}

		// Setting the misclosure vector elements
		isProcessOK &= matrices->setMisclosureVectorElement(firstEqIdx, -1.0 * (itECWI->getDistance(EECWIDistances::kX) - contributions.fCalcMeas[0]))
			&& matrices->setMisclosureVectorElement(firstEqIdx + 1, -1.0 * (itECWI->getDistance(EECWIDistances::kZ) - contributions.fCalcMeas[1]));

		// Adding controbution to the wire Bearing angle (always variable)
		isProcessOK &= matrices->addFirstDgnMtrxElement(firstEqIdx, ecwiROM.fWireBearing->getFirstUidx(), contributions.fBearingWireContrib[0])
			&& matrices->addFirstDgnMtrxElement(firstEqIdx + 1, ecwiROM.fWireBearing->getFirstUidx(), contributions.fBearingWireContrib[1]);

		// Adding controbution to the wire DRefX (always variable)
		isProcessOK &= matrices->addFirstDgnMtrxElement(firstEqIdx, ecwiROM.fWireDx->getFirstUidx(), contributions.fDRefXDistContrib[0])
			&& matrices->addFirstDgnMtrxElement(firstEqIdx + 1, ecwiROM.fWireDx->getFirstUidx(), contributions.fDRefXDistContrib[1]);

		// Adding controbution to the wire DRefZ (always variable)
		isProcessOK &= matrices->addFirstDgnMtrxElement(firstEqIdx, ecwiROM.fWireDz->getFirstUidx(), contributions.fDRefZDistContrib[0])
			&& matrices->addFirstDgnMtrxElement(firstEqIdx + 1, ecwiROM.fWireDz->getFirstUidx(), contributions.fDRefZDistContrib[1]);

		// Adding controbution to the wire Slope (always variable)
		isProcessOK &= matrices->addFirstDgnMtrxElement(firstEqIdx, ecwiROM.fWireSlope->getFirstUidx(), contributions.fSlopeWireContrib[0])
			&& matrices->addFirstDgnMtrxElement(firstEqIdx + 1, ecwiROM.fWireSlope->getFirstUidx(), contributions.fSlopeWireContrib[1]);

		// Adding controbution to the wire SAG (variable or fixed)
		if (!ecwiROM.sagfix)
		{
			isProcessOK &= matrices->addFirstDgnMtrxElement(firstEqIdx, ecwiROM.sagAdjustable->getFirstUidx(), contributions.fSagContrib[0])
				&& matrices->addFirstDgnMtrxElement(firstEqIdx + 1, ecwiROM.sagAdjustable->getFirstUidx(), contributions.fSagContrib[1]);
		}

		// Add weight unknown matrix element
		if (contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit)
			throw std::runtime_error("Error when filling ECWI contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			for (int i = 0; i < 2; i++)
			{
				isProcessOK &= matrices->addWeightMtrxElement(firstObsIdx + i, firstObsIdx + i, 1.0 / contributions.fObsVariance[i])
					&& matrices->addWeightInvMtrxElement(firstObsIdx + i, firstObsIdx + i, contributions.fObsVariance[i]);
			}
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of ECWI measurement occurred.");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - FILLING more-equations observation
///////////////////////////////////////////////////////////////////////////////////////////////////////

void TLSInputMatricesFiller::addPLR3DContributions(std::shared_ptr<TTSTN::TROM> rom, std::shared_ptr<TTSTN> station, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex firstEqIdx = -1;
	MatrixIndex firstObsIdx = -1;

	for (auto meas(rom->measPLR3D.begin()); meas != rom->measPLR3D.end(); ++meas)
	{
		firstEqIdx = meas->getFirstEquationIndex();
		firstObsIdx = meas->getFirstObservationIndex();
		// Get the observation contribution
		PLR3DContrib contributions = fCGenerator.getPolar3DContrib(station, rom, *meas);

		// Update the sigma
		meas->target.sigmaCombinedPLRAngl = TAngle(sqrt(contributions.fObsVariance[0]));
		meas->target.sigmaCombinedPLRZenD = TAngle(sqrt(contributions.fObsVariance[1]));
		meas->target.sigmaCombinedPLRDist = TLength(sqrt(contributions.fObsVariance[2]));

		const LGCAdjustablePoint &stationPos = *station->instrumentPos;
		const LGCAdjustablePoint &targetPos = *meas->targetPos;
		// Add contributions for STATION coordinates
		if (!stationPos.isFixed())
		{
			isProcessOK = isProcessOK && addPointContribution(stationPos, TFreeVector(contributions.fStCoordContrib.contrib.row(0)), firstEqIdx, matrices);
			isProcessOK = isProcessOK && addPointContribution(stationPos, TFreeVector(contributions.fStCoordContrib.contrib.row(1)), firstEqIdx + 1, matrices);
			isProcessOK = isProcessOK && addPointContribution(stationPos, TFreeVector(contributions.fStCoordContrib.contrib.row(2)), firstEqIdx + 2, matrices);
		}

		// Add contributions for TARGET coordinates
		if (!targetPos.isFixed())
		{
			isProcessOK = isProcessOK && addPointContribution(targetPos, TFreeVector(contributions.fTgCoordContrib.contrib.row(0)), firstEqIdx, matrices);
			isProcessOK = isProcessOK && addPointContribution(targetPos, TFreeVector(contributions.fTgCoordContrib.contrib.row(1)), firstEqIdx + 1, matrices);
			isProcessOK = isProcessOK && addPointContribution(targetPos, TFreeVector(contributions.fTgCoordContrib.contrib.row(2)), firstEqIdx + 2, matrices);
		}

		if (!meas->target.distCorrectionAdjustable->isFixed())
		{
			int distCorUidx = meas->target.distCorrectionAdjustable->getFirstUidx();
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx, distCorUidx, contributions.fDistAndCsContrib[0]);
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx + 1, distCorUidx, contributions.fDistAndCsContrib[1]);
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx + 2, distCorUidx, contributions.fDistAndCsContrib[2]);
		}

		// Add contributions of STATION transformations parameters
		for (auto itStTransform(contributions.fStTransformContrib.begin()); itStTransform != contributions.fStTransformContrib.end(); ++itStTransform)
		{
			if (!itStTransform->first.isFixed())
			{
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itStTransform->first, itStTransform->second.thirdEquationTransContrib, firstEqIdx + 2, matrices);
			}
		}

		// Add contributions of TARGET transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
			{
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.thirdEquationTransContrib, firstEqIdx + 2, matrices);
			}
		}

		// Setting the misclosure vector elements
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + 0, (TAngle(contributions.fCalcMeas[0]) - meas->getAngle(kANGL)).getRadiansValue());
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + 1, (TAngle(contributions.fCalcMeas[1]) - meas->getAngle(kZEND)).getRadiansValue());
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + 2, contributions.fCalcMeas[2] - meas->getDistance().getMetresValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit || contributions.fObsVariance[2] < nullLimit)
			throw std::runtime_error("Error when filling PLR3D contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0 / contributions.fObsVariance[0]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0 / contributions.fObsVariance[1]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);

			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0 / contributions.fObsVariance[2]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, contributions.fObsVariance[2]);
		}

		// Add V0's contribution, V0 is variable at any case. Check in a case of future change.
		if (!rom->v0->isFixed())
		{
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx, rom->v0->getFirstUidx(), contributions.fV0Contrib[0]);
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx + 1, rom->v0->getFirstUidx(), contributions.fV0Contrib[1]);
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx + 2, rom->v0->getFirstUidx(), contributions.fV0Contrib[2]);
		}

		// Add instrument height contributions
		if (!station->instrumentHeightAdjustable->isFixed())
		{
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx, station->instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[0]);
			isProcessOK = isProcessOK
				&& matrices->addFirstDgnMtrxElement(firstEqIdx + 1, station->instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[1]);
			isProcessOK = isProcessOK
				&& matrices->addFirstDgnMtrxElement(firstEqIdx + 2, station->instrumentHeightAdjustable->getFirstUidx(), contributions.fInstrHeightContrib[2]);
		}

		if (station->rot3D)
		{
			if (station->rotX == nullptr || station->rotY == nullptr)
				throw std::runtime_error("TSTN can rotate freely, but rotation angle around X or Y axis is nullptr.");
			for (int i = 0; i < 3; i++)
			{
				isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx + i, station->rotX->getFirstUidx(), contributions.fRxContrib[i]);
				isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(firstEqIdx + i, station->rotY->getFirstUidx(), contributions.fRyContrib[i]);
			}
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of PLR3D measurement occurred.");
	}
}

void TLSInputMatricesFiller::addUVDContribution(TCAM &camera, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex firstEqIdx = -1;
	MatrixIndex firstObsIdx = -1;
	UVDContrib contributions;

	for (auto meas(camera.measUVD.begin()); meas != camera.measUVD.end(); ++meas)
	{
		firstEqIdx = meas->getFirstEquationIndex();
		firstObsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getUVDContrib(camera, *meas);

		// Update the sigma
		meas->target.sigmaCombinedX = TLength(sqrt(contributions.fObsVariance[0]));
		meas->target.sigmaCombinedY = TLength(sqrt(contributions.fObsVariance[1]));
		meas->target.sigmaCombinedDist = TLength(sqrt(contributions.fObsVariance[2]));

		const LGCAdjustablePoint &cameraPos = *camera.instrumentPos;
		const LGCAdjustablePoint &targetPos = *meas->targetPos;
		// Add contributions for CAMERA coordinates
		if (!cameraPos.isFixed())
		{
			isProcessOK = isProcessOK && addPointContribution(cameraPos, TFreeVector(contributions.fStCoordContrib.contrib.row(0)), firstEqIdx, matrices);
			isProcessOK = isProcessOK && addPointContribution(cameraPos, TFreeVector(contributions.fStCoordContrib.contrib.row(1)), firstEqIdx + 1, matrices);
			isProcessOK = isProcessOK && addPointContribution(cameraPos, TFreeVector(contributions.fStCoordContrib.contrib.row(2)), firstEqIdx + 2, matrices);
		}

		// Add contributions for TARGET coordinates
		if (!targetPos.isFixed())
		{
			isProcessOK = isProcessOK && addPointContribution(targetPos, TFreeVector(contributions.fTgCoordContrib.contrib.row(0)), firstEqIdx, matrices);
			isProcessOK = isProcessOK && addPointContribution(targetPos, TFreeVector(contributions.fTgCoordContrib.contrib.row(1)), firstEqIdx + 1, matrices);
			isProcessOK = isProcessOK && addPointContribution(targetPos, TFreeVector(contributions.fTgCoordContrib.contrib.row(2)), firstEqIdx + 2, matrices);
		}

		// Add contributions of transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
			{
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.thirdEquationTransContrib, firstEqIdx + 2, matrices);
			}
		}

		// Setting the misclosure vector elements
		TFreeVector obsVec = meas->getVectorValue();
		TReal obsDistance = meas->getDistance().getMetresValue();
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx, contributions.fCalcMeas[0] - obsVec.getX().getMetresValue());
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + 1, contributions.fCalcMeas[1] - obsVec.getY().getMetresValue());
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + 2, contributions.fCalcMeas[2] - obsDistance);

		// Add weight unknown matrix element
		if (contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit || contributions.fObsVariance[2] < nullLimit)
			throw std::runtime_error("Error when filling UVD contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0 / contributions.fObsVariance[0]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0 / contributions.fObsVariance[1]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);

			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx + 2, firstObsIdx + 2, 1.0 / contributions.fObsVariance[2]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx + 2, firstObsIdx + 2, contributions.fObsVariance[2]);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of UVD measurement occurred.");
	}
}

void TLSInputMatricesFiller::addUVECContribution(TCAM &camera, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	MatrixIndex firstEqIdx = -1;
	MatrixIndex firstObsIdx = -1;
	UVECContrib contributions;

	for (auto meas(camera.measUVEC.begin()); meas != camera.measUVEC.end(); ++meas)
	{
		firstEqIdx = meas->getFirstEquationIndex();
		firstObsIdx = meas->getFirstObservationIndex();

		contributions = fCGenerator.getUVECContrib(camera, *meas);

		meas->target.sigmaCombinedX = TLength(sqrt(contributions.fObsVariance[0]));
		meas->target.sigmaCombinedY = TLength(sqrt(contributions.fObsVariance[1]));

		const LGCAdjustablePoint &cameraPos = *camera.instrumentPos;
		const LGCAdjustablePoint &targetPos = *meas->targetPos;
		// Add contributions for CAMERA coordinates
		if (!cameraPos.isFixed())
		{
			isProcessOK = isProcessOK && addPointContribution(cameraPos, contributions.fStCoordContribFirstEq, firstEqIdx, matrices);
			isProcessOK = isProcessOK && addPointContribution(cameraPos, contributions.fStCoordContribSecondEq, firstEqIdx + 1, matrices);
		}

		// Add contributions for TARGET coordinates
		if (!targetPos.isFixed())
		{
			isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContribFirstEq, firstEqIdx, matrices);
			isProcessOK = isProcessOK && addPointContribution(targetPos, contributions.fTgCoordContribSecondEq, firstEqIdx + 1, matrices);
		}

		// Add contributions fot the transformations parameters
		for (auto itTgTransform(contributions.fTgTransformContrib.begin()); itTgTransform != contributions.fTgTransformContrib.end(); ++itTgTransform)
		{
			if (!itTgTransform->first.isFixed())
			{
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.firstEquationTransContrib, firstEqIdx, matrices);
				isProcessOK = isProcessOK && addTransformationContribution(itTgTransform->first, itTgTransform->second.secondEquationTransContrib, firstEqIdx + 1, matrices);
			}
		}

		// Setting the misclosure vector elements
		TFreeVector obsVal = meas->getVectorValue();
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx, contributions.fCalcMeas[0] - obsVal.getX().getMetresValue());
		isProcessOK = isProcessOK && matrices->setMisclosureVectorElement(firstEqIdx + 1, contributions.fCalcMeas[1] - obsVal.getY().getMetresValue());

		// Add weight unknown matrix element
		if (contributions.fObsVariance[0] < nullLimit || contributions.fObsVariance[1] < nullLimit)
			throw std::runtime_error("Error when filling UVEC contribution, variance is zero or too small, can not set weight matrix element.");
		else
		{
			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx, firstObsIdx, 1.0 / contributions.fObsVariance[0]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx, firstObsIdx, contributions.fObsVariance[0]);

			isProcessOK = isProcessOK && matrices->addWeightMtrxElement(firstObsIdx + 1, firstObsIdx + 1, 1.0 / contributions.fObsVariance[1]);
			isProcessOK = isProcessOK && matrices->addWeightInvMtrxElement(firstObsIdx + 1, firstObsIdx + 1, contributions.fObsVariance[1]);
		}

		if (!isProcessOK)
			throw std::runtime_error("Error when filling input design matrices of UVEC measurement occurred.");
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE - SUPPORTING FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool TLSInputMatricesFiller::addTransformationContribution(const TAdjustableHelmertTransformation &trafo, const TransformationContrib &trContrib, int eqIndex, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;

	for (int i = 0; i < 3; i++)
	{
		if (!trafo.isTranslationFixed(i))
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIndex, trafo.getTranslationUnknIndex(i), trContrib.fTranslContrib[i]);

		if (!trafo.isRotationFixed(i))
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIndex, trafo.getRotationUnknIndex(i), trContrib.fRotationContrib[i]);
	}

	if (!trafo.isScaleFixed())
		isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIndex, trafo.getScaleUnknIndex(), trContrib.fScaleContrib);

	return isProcessOK;
}

bool TLSInputMatricesFiller::addConstraintTransformationContribution(const TAdjustableHelmertTransformation &trafo, const TransformationContrib &trContrib, int eqIndex, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;

	for (int i = 0; i < 3; i++)
	{
		if (!trafo.isTranslationFixed(i))
			isProcessOK = isProcessOK && matrices->addCnstrFirstDgnMtrxElement(eqIndex, trafo.getTranslationUnknIndex(i), trContrib.fTranslContrib[i]);

		if (!trafo.isRotationFixed(i))
			isProcessOK = isProcessOK && matrices->addCnstrFirstDgnMtrxElement(eqIndex, trafo.getRotationUnknIndex(i), trContrib.fRotationContrib[i]);
	}

	if (!trafo.isScaleFixed())
		isProcessOK = isProcessOK && matrices->addCnstrFirstDgnMtrxElement(eqIndex, trafo.getScaleUnknIndex(), trContrib.fScaleContrib);

	return isProcessOK;
}

bool TLSInputMatricesFiller::addPointContribution(const LGCAdjustablePoint &pointAdj, const TFreeVector &pointContrib, int eqIdx, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;

	for (int i = 0; i < 3; i++)
	{
		if (!pointAdj.isCoordinateFixed(i))
			isProcessOK = isProcessOK && matrices->addFirstDgnMtrxElement(eqIdx, pointAdj.getCoordinateUnknIndex(i), pointContrib[i]);
	}
	return isProcessOK;
}

bool TLSInputMatricesFiller::addPointConstraintContribution(const LGCAdjustablePoint &pointAdj, const TFreeVector &pointContrib, int eqIdx, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;

	for (int i = 0; i < 3; i++)
	{
		if (!pointAdj.isCoordinateFixed(i))
			isProcessOK = isProcessOK && matrices->addCnstrFirstDgnMtrxElement(eqIdx, pointAdj.getCoordinateUnknIndex(i), pointContrib[i]);
	}
	return isProcessOK;
}

bool TLSInputMatricesFiller::fillParameterWeights(TLGCData *projData, TLSInputMatrices *matrices)
{
	bool isProcessOK = true;
	auto &outputMessages(projData->getFileLogger());

	// loop through points and add weight contributions
	for (auto &pt : projData->getPoints())
	{
		if (pt.hasPointSigma())
		{
			pointSigmaData &ptSigma = pt.getPointSigmaData();
			pointSigmaContrib contribution = fCGenerator.getPointSigmaContrib(pt, projData);
			int cIdx = ptSigma.firstCIdx;
			int obsIdx = ptSigma.firstObsIdx;
			// handle observations
			// cut out the weight matrix from the coordinates that are treated as observations
			Eigen::MatrixXd activeObsWeightMatrix = ptSigma.fWeightMatrix(ptSigma.fRelObsIdx, ptSigma.fRelObsIdx);

			isProcessOK &= matrices->addWeightMtrxBlock(obsIdx, activeObsWeightMatrix);
			for (int relRowIdx : ptSigma.fRelObsIdx)
			{ // its a observation
				// set the row of the A matrix
				for (int colIdx = 0; colIdx < 3; colIdx++)
				{
					if (!pt.isCoordinateFixed(colIdx))
						matrices->addFirstDgnMtrxElement(obsIdx, pt.getCoordinateUnknIndex(colIdx), ptSigma.fRotMat(relRowIdx, colIdx));
				}
				isProcessOK &= matrices->setMisclosureVectorElement(obsIdx, contribution.misclosure(relRowIdx));
				obsIdx++;
			}

			// handle constraints
			for (int relRowIdx : ptSigma.fRelCIdx)
			{
				for (int colIdx = 0; colIdx < 3; colIdx++)
				{
					if (!pt.isCoordinateFixed(colIdx))
						matrices->addCnstrFirstDgnMtrxElement(cIdx, pt.getCoordinateUnknIndex(colIdx), ptSigma.fRotMat(relRowIdx, colIdx));
				}
				isProcessOK &= matrices->setCnstrMisclosureVectorElement(cIdx, contribution.misclosure(relRowIdx));
				cIdx++;
			}
		}
	}
	return isProcessOK;
}

bool TLSInputMatricesFiller::fillSlaveConstraints(TLGCData *projData, TLSInputMatrices *matrices)
{
	bool fillOK = true;
	auto &outputMessages(projData->getFileLogger());
	try
	{
		// iterate over all slave groups
		for (TLGCFrameConstraintGroup slaveGroup : projData->getSlaveGroups())
		{
			int cIdx = slaveGroup.getFirstCIndex();
			// check if group has more then one frame
			if (slaveGroup.slaves.size() == 1)
			{
				logWarning() << "Slave group " << slaveGroup.getGroupName() << " does not add any constraint.";
			}
			else
			{
				TAdjustableHelmertTransformation &masterFrame = projData->locateNode(slaveGroup.slaves.front()).node->data.get()->frame;
				// iterate over the non-master frames
				for (auto slaveIt = ++slaveGroup.slaves.begin(); slaveIt != slaveGroup.slaves.end(); slaveIt++)
				{
					{
						TAdjustableHelmertTransformation &slaveFrame = projData->locateNode(*slaveIt).node->data.get()->frame;
						for (int idx = 0; idx < 3; idx++)
						{
							if (!masterFrame.isTranslationFixed(idx))
							{
								double translMisclosure = masterFrame.getEstTranslation(idx).getMetresValue() - slaveFrame.getEstTranslation(idx).getMetresValue();
								matrices->setCnstrMisclosureVectorElement(cIdx, translMisclosure);
								matrices->addCnstrFirstDgnMtrxElement(cIdx, masterFrame.getTranslationUnknIndex(idx), 1);
								matrices->addCnstrFirstDgnMtrxElement(cIdx, slaveFrame.getTranslationUnknIndex(idx), -1);
								cIdx++;
							}
							if (!masterFrame.isRotationFixed(idx))
							{
								double rotMisclosure = masterFrame.getEstRotation(idx).getRadiansValue() - slaveFrame.getEstRotation(idx).getRadiansValue();
								matrices->setCnstrMisclosureVectorElement(cIdx, rotMisclosure);
								matrices->addCnstrFirstDgnMtrxElement(cIdx, masterFrame.getRotationUnknIndex(idx), 1);
								matrices->addCnstrFirstDgnMtrxElement(cIdx, slaveFrame.getRotationUnknIndex(idx), -1);
								cIdx++;
							}
						}
						if (!masterFrame.isScaleFixed())
						{
							double misclosure = masterFrame.getEstScale() - slaveFrame.getEstScale();
							matrices->setCnstrMisclosureVectorElement(cIdx, misclosure);
							matrices->addCnstrFirstDgnMtrxElement(cIdx, masterFrame.getScaleUnknIndex(), 1);
							matrices->addCnstrFirstDgnMtrxElement(cIdx, slaveFrame.getScaleUnknIndex(), -1);
							cIdx++;
						}
					}
				}
			}
		}
	}
	catch (std::exception const &excp)
	{
		outputMessages << TFileLogger::e_logType::LOG_ERROR << excp.what();
		fillOK = false;
	}

	return fillOK;
}

bool TLSInputMatricesFiller::fillPointGroupConstraints(TLGCData *projData, TLSInputMatrices *matrices)
{
	bool fillOK = true;
	auto &outputMessages(projData->getFileLogger());
	try
	{
		// iterate over all point constraint groups
		for (TLGCPointConstraintGroup group : projData->getPointGroups())
		{
			int cIdx = group.getFirstCIndex();
			LIBRPointGroupContrib contrib = fCGenerator.getPointGroupConstraintContrib(group, *projData);
			// fill the corresponding constraints. misclosure and derivatives
			// as each point in the group is transformed to root, it contributes possibly helmert trafo contribution and a position variable contribution
			// so its important to not overwrite helmert trafo contributions but to add them

			std::array<bool, 7> active = group.getConstraintSignature();
			// loop over all 7 possible constraints
			for (size_t relCIdx = 0; relCIdx < 7; relCIdx++)
			{
				// is this constraint active?
				if (active[relCIdx] == true)
				{
					// Dummy Implementation for translation in x constraint
					// loop over points in group
					for (std::string pointName : group.getAffectedPoints())
					{
						LGCAdjustablePoint adjPoint = projData->getPoints().getObject(pointName);

						// translation constraint
						if (0 <= relCIdx && relCIdx < 3)
						{
							// contribution from the point coordinates itself
							fillOK = fillOK
								&& addPointConstraintContribution(adjPoint, TFreeVector(contrib.cogConstraintContrib.PointContrib[pointName].row(relCIdx)), cIdx, matrices);
							// contribution of helmert transformations
							for (auto frameContribPair : contrib.cogConstraintContrib.TransformContrib[pointName])
							{
								if (!frameContribPair.first.isFixed())
								{
									fillOK = fillOK && addConstraintTransformationContribution(frameContribPair.first, frameContribPair.second.getContrib(relCIdx), cIdx, matrices);
								}
							}
						}
						// rotation constraint
						if (3 <= relCIdx && relCIdx < 6)
						{
							// contribution from the point coordinates itself
							fillOK = fillOK
								&& addPointConstraintContribution(adjPoint, TFreeVector(contrib.momentumConstraintContrib.PointContrib[pointName].row(relCIdx - 3)), cIdx, matrices);
							// contribution of helmert transformations
							for (auto frameContribPair : contrib.momentumConstraintContrib.TransformContrib[pointName])
							{
								if (!frameContribPair.first.isFixed())
								{
									fillOK = fillOK
										&& addConstraintTransformationContribution(frameContribPair.first, frameContribPair.second.getContrib(relCIdx - 3), cIdx, matrices);
								}
							}
						}
						// scale constraint
						if (relCIdx == 6)
						{
							// contribution from the point coordinates itself
							fillOK = fillOK && addPointConstraintContribution(adjPoint, TFreeVector(contrib.scaleConstraintContrib.PointContrib[pointName]), cIdx, matrices);
							// contribution of helmert transformations
							for (auto frameContribPair : contrib.scaleConstraintContrib.TransformContrib[pointName])
							{
								if (!frameContribPair.first.isFixed())
								{
									fillOK = fillOK && addConstraintTransformationContribution(frameContribPair.first, frameContribPair.second, cIdx, matrices);
								}
							}
						}
					}
					// misclosure
					if (0 <= relCIdx && relCIdx < 3)
					{
						fillOK = fillOK && matrices->setCnstrMisclosureVectorElement(cIdx, contrib.cogConstraintContrib.constraintMisclosure[relCIdx]);
						cIdx++;
					}
					else if (3 <= relCIdx && relCIdx < 6)
					{
						fillOK = fillOK && matrices->setCnstrMisclosureVectorElement(cIdx, contrib.momentumConstraintContrib.constraintMisclosure[relCIdx - 3]);
						cIdx++;
					}
					else if (relCIdx == 6)
					{
						fillOK = fillOK && matrices->setCnstrMisclosureVectorElement(cIdx, contrib.scaleConstraintContrib.constraintMisclosure);
						cIdx++;
					}
				}
			}
			// check if there is a constraint that does not depend on any parameter
			// this can happen for example if a TX constraint was (manually) added but the only points defined are *VZ (they only vary in Z)
			if (matrices->hasDegenerateConstraints())
			{
				throw std::runtime_error("A constraint that does not depend on any variable was added.");
			}
		}
	}
	catch (std::exception const &excp)
	{
		outputMessages << TFileLogger::e_logType::LOG_ERROR << excp.what();
		fillOK = false;
	}

	return fillOK;
}
