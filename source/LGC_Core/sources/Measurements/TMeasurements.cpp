
#include "TMeasurements.h"

void TMeasurements::initialiseObsSummaries() {
	// First clear the old contents away
	dverSummary_.clear();
	radiSummary_.clear();
	obsxyzSummary_.obsXObsSum.clear();
	obsxyzSummary_.obsYObsSum.clear();
	obsxyzSummary_.obsZObsSum.clear();

	plrGlobalSummary_.anglObsSum.clear();
	plrGlobalSummary_.zendObsSum.clear();
	plrGlobalSummary_.distObsSum.clear();
	anglGlobalSummary_.clear();
	zendGlobalSummary_.clear();
	distGlobalSummary_.clear();
	dhorGlobalSummary_.clear();
	ecthGlobalSummary_.clear();
	ecdirGlobalSummary_.clear();
	uvdGlobalSummary_.xVectorCompObsSum.clear();
	uvdGlobalSummary_.yVectorCompObsSum.clear();
	uvdGlobalSummary_.distObsSum.clear();
	uvecGlobalSummary_.xVectorCompObsSum.clear();
	uvecGlobalSummary_.yVectorCompObsSum.clear();
	dsptGlobalSummary_.clear();
	dlevGlobalSummary_.clear();
	dlevDHORGlobalSummary_.clear();
	orieGlobalSummary_.clear();
	echoGlobalSummary_.clear();
	ecveGlobalSummary_.clear();
	ecspGlobalSummary_.clear();

	// Add the residuals of each measurement and initialise the summaries:

	// DVER
	if (fDVER.size() != 0) {
		for (auto &dver : fDVER)
			dverSummary_.addNewResidual(dver.getDistanceResidual().getMMetresValue());

		dverSummary_.initialise();
	}

	// RADI
	if (fRADI.size() != 0) {
		for (auto &radi : fRADI)
			radiSummary_.addNewResidual(radi.getResidual().getMMetresValue());

		radiSummary_.initialise();
	}

	// OBSXYZ
	if (fOBSXYZ.size() != 0) {
		for (auto &obs : fOBSXYZ) {
			obsxyzSummary_.obsXObsSum.addNewResidual(obs.getXResidual().getMMetresValue());
			obsxyzSummary_.obsYObsSum.addNewResidual(obs.getYResidual().getMMetresValue());
			obsxyzSummary_.obsZObsSum.addNewResidual(obs.getZResidual().getMMetresValue());
		}

		obsxyzSummary_.obsXObsSum.initialise();
		obsxyzSummary_.obsYObsSum.initialise();
		obsxyzSummary_.obsZObsSum.initialise();
	}

	// Initialise the observation summaries of
	// measurements that use an instrument. Initialise
	// also the global observation summaries.

	std::list<const TLGCObsSummary*>
		allPlrANGLSummaries,
		allPlrZENDSummaries,
		allPlrDISTSummaries,
		allANGLSummaries,
		allZENDSummaries,
		allDISTSummaries,
		allDHORSummaries,
		allECTHSummaries,
		allECDIRSummaries,
		allUvdXSummaries,
		allUvdYSummaries,
		allUvdDSummaries,
		allUvecXSummaries,
		allUvecYSummaries,
		allDSPTSummaries,
		allDLEVSummaries,
		allDlevDHORSummaries,
		allORIESummaries,
		allECHOSummaries,
		allECVESummaries,
		allECSPSummaries;

	// TSTN
	for (auto &tstn : fTSTN)
		for (auto &rom : tstn->roms) {
			rom->initialiseObsSummaries();

			// Add the initialised obs summaries of each rom to the global collection:
			allPlrANGLSummaries.push_back(&rom->getPLR3DObsSummary().anglObsSum);
			allPlrZENDSummaries.push_back(&rom->getPLR3DObsSummary().zendObsSum);
			allPlrDISTSummaries.push_back(&rom->getPLR3DObsSummary().distObsSum);
			allANGLSummaries.push_back(&rom->getANGLObsSummary());
			allZENDSummaries.push_back(&rom->getZENDObsSummary());
			allDISTSummaries.push_back(&rom->getDISTObsSummary());
			allDHORSummaries.push_back(&rom->getDHORObsSummary());
			allECTHSummaries.push_back(&rom->getECTHObsSummary());
			allECDIRSummaries.push_back(&rom->getECDIRObsSummary());

			// FRK: Rajouter les offsets (mm) correspondants aux angles ANGL et ZEND
		}

	// CAM
	for (auto &cam : fCAM) {
		cam.initialiseObsSummaries();

		// Add the initialised obs summaries of each rom to the global collection:
		allUvdXSummaries.push_back(&cam.getUVDObsSummary().xVectorCompObsSum);
		allUvdYSummaries.push_back(&cam.getUVDObsSummary().yVectorCompObsSum);
		allUvdDSummaries.push_back(&cam.getUVDObsSummary().distObsSum);
		allUvecXSummaries.push_back(&cam.getUVECObsSummary().xVectorCompObsSum);
		allUvecYSummaries.push_back(&cam.getUVECObsSummary().yVectorCompObsSum);
	}

	// DSPT
	for (auto &edm : fEDM) {
		edm.initialiseObsSummaries();

		allDSPTSummaries.push_back(&edm.getDSPTObsSummary());
	}

	// DLEV
	for (auto &level : fLEVEL) {
		level.initialiseObsSummaries();

		allDLEVSummaries.push_back(&level.getDLEVObsSummary());
		allDlevDHORSummaries.push_back(&level.getDHORObsSummary());
	}

	// ORIE
	for (auto &orierom : fORIE) {
		orierom.initialiseObsSummaries();

		allORIESummaries.push_back(&orierom.getORIEObsSummary());
	}

	// ECHO
	for (auto &echorom : fECHO) {
		echorom.initialiseObsSummaries();

		allECHOSummaries.push_back(&echorom.getECHOObsSummary());
	}

	// ECVE
	for (auto &ecverom : fECVE) {
		ecverom.initialiseObsSummaries();

		allECVESummaries.push_back(&ecverom.getECVEObsSummary());
	}

	// ECSP
	for (auto &ecsprom : fECSP) {
		ecsprom.initialiseObsSummaries();

		allECSPSummaries.push_back(&ecsprom.getECSPObsSummary());
	}

	// Create the global summaries from the collections:

	// TSTN:
	plrGlobalSummary_.anglObsSum = TLGCObsSummary::merge(allPlrANGLSummaries);
	plrGlobalSummary_.zendObsSum = TLGCObsSummary::merge(allPlrZENDSummaries);
	plrGlobalSummary_.distObsSum = TLGCObsSummary::merge(allPlrDISTSummaries);
	anglGlobalSummary_ = TLGCObsSummary::merge(allANGLSummaries);
	zendGlobalSummary_ = TLGCObsSummary::merge(allZENDSummaries);
	distGlobalSummary_ = TLGCObsSummary::merge(allDISTSummaries);
	dhorGlobalSummary_ = TLGCObsSummary::merge(allDHORSummaries);
	ecthGlobalSummary_ = TLGCObsSummary::merge(allECTHSummaries);
	ecdirGlobalSummary_ = TLGCObsSummary::merge(allECDIRSummaries);

	// CAM:
	uvdGlobalSummary_.xVectorCompObsSum = TLGCObsSummary::merge(allUvdXSummaries);
	uvdGlobalSummary_.yVectorCompObsSum = TLGCObsSummary::merge(allUvdYSummaries);
	uvdGlobalSummary_.distObsSum = TLGCObsSummary::merge(allUvdDSummaries);
	uvecGlobalSummary_.xVectorCompObsSum = TLGCObsSummary::merge(allUvecXSummaries);
	uvecGlobalSummary_.yVectorCompObsSum = TLGCObsSummary::merge(allUvecYSummaries);

	// Other:
	dsptGlobalSummary_ = TLGCObsSummary::merge(allDSPTSummaries);
	dlevGlobalSummary_ = TLGCObsSummary::merge(allDLEVSummaries);
	dlevDHORGlobalSummary_ = TLGCObsSummary::merge(allDlevDHORSummaries);
	orieGlobalSummary_ = TLGCObsSummary::merge(allORIESummaries);
	echoGlobalSummary_ = TLGCObsSummary::merge(allECHOSummaries);
	ecveGlobalSummary_ = TLGCObsSummary::merge(allECVESummaries);
	ecspGlobalSummary_ = TLGCObsSummary::merge(allECSPSummaries);

}

const TLGCObsSummary& TMeasurements::getDVERObsSummary() const { return dverSummary_; }

const TLGCObsSummary&  TMeasurements::getRADIObsSummary() const { return radiSummary_; }

const TOBSXYZObsSummary& TMeasurements::getOBSXYZObsSummary() const { return obsxyzSummary_; }


const TPOLARObsSummary& TMeasurements::getPOLARGlobalObsSummary() const { return plrGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getANGLGlobalObsSummary() const { return anglGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getZENDGlobalObsSummary() const { return zendGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getDISTGlobalObsSummary() const { return distGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getDHORGlobalObsSummary() const { return dhorGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getECTHGlobalObsSummary() const { return ecthGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getECDIRGlobalObsSummary() const { return ecdirGlobalSummary_; }


const TUVDObsSummary& TMeasurements::getUVDGlobalObsSummary() const { return uvdGlobalSummary_; }

const TUVECObsSummary& TMeasurements::getUVECGlobalObsSummary() const { return uvecGlobalSummary_; }


const TLGCObsSummary& TMeasurements::getDSPTGlobalObsSummary() const { return dsptGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getDLEVGlobalObsSummary() const { return dlevGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getDlevDHORGlobalObsSummary() const { return dlevDHORGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getORIEGlobalObsSummary() const { return orieGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getECHOGlobalObsSummary() const { return echoGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getECVEGlobalObsSummary() const { return ecveGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getECSPGlobalObsSummary() const { return ecspGlobalSummary_; }