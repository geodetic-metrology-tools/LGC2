
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
	inclyGlobalSummary_.clear();
    ecwsGlobalSummary_.clear();

    // Add the residuals of each measurement and initialise the summaries:

    // DVER
    if(fDVER.size() != 0){
        for(auto &dver : fDVER)
            dverSummary_.addNewResidual(dver.getDistanceResidual().getMMetresValue());

        dverSummary_.initialise();
    }

    // RADI
    if(fRADI.size() != 0){
        for(auto &radi : fRADI)
            radiSummary_.addNewResidual(radi.getResidual().getMMetresValue());

        radiSummary_.initialise();
    }

    // OBSXYZ
    if(fOBSXYZ.size() != 0) {
        for(auto &obs : fOBSXYZ){
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
        allECSPSummaries,
		allINCLYSummaries,
        allECWSSummaries;

    // TSTN
    for(auto &tstn : fTSTN)
        for(auto &rom : tstn->roms){
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
        }   

    // CAM
    for(auto &cam : fCAM){
        cam.initialiseObsSummaries();

        // Add the initialised obs summaries of each rom to the global collection:
        allUvdXSummaries.push_back(&cam.getUVDObsSummary().xVectorCompObsSum);
        allUvdYSummaries.push_back(&cam.getUVDObsSummary().yVectorCompObsSum);
        allUvdDSummaries.push_back(&cam.getUVDObsSummary().distObsSum);
        allUvecXSummaries.push_back(&cam.getUVECObsSummary().xVectorCompObsSum);
        allUvecYSummaries.push_back(&cam.getUVECObsSummary().yVectorCompObsSum);
    }

    // DSPT
    for(auto &edm : fEDM){
        edm.initialiseObsSummaries();

        allDSPTSummaries.push_back(&edm.getDSPTObsSummary());
    }

    // DLEV
    for(auto &level : fLEVEL){
        level.initialiseObsSummaries();

        allDLEVSummaries.push_back(&level.getDLEVObsSummary());
        allDlevDHORSummaries.push_back(&level.getDHORObsSummary());
    }

    // ORIE
    for(auto &orierom : fORIE){
        orierom.initialiseObsSummaries();

        allORIESummaries.push_back(&orierom.getORIEObsSummary());
    }

    // ECHO
    for(auto &echorom : fECHO){
        echorom.initialiseObsSummaries();

        allECHOSummaries.push_back(&echorom.getECHOObsSummary());
    }

    // ECVE
    for(auto &ecverom : fECVE){
        ecverom.initialiseObsSummaries();

        allECVESummaries.push_back(&ecverom.getECVEObsSummary());
    }

    // ECSP
    for(auto &ecsprom : fECSP){
        ecsprom.initialiseObsSummaries();

        allECSPSummaries.push_back(&ecsprom.getECSPObsSummary());
    }

	// INCLY
	for (auto &inclyrom : fINCLY) {
		inclyrom.initialiseObsSummaries();

		allINCLYSummaries.push_back(&inclyrom.getINCLYObsSummary());
	}

    // ECWS
    for (auto& ecwsrom : fECWS) {
        ecwsrom.initialiseObsSummaries();

        allECWSSummaries.push_back(&ecwsrom.getECWSObsSummary());
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
	inclyGlobalSummary_ = TLGCObsSummary::merge(allINCLYSummaries);

}

const TLGCObsSummary& TMeasurements::getDVERObsSummary() const { return dverSummary_; }

const TLGCObsSummary& TMeasurements::getDVERObsSummary(std::string text) {
	dverSummary_.setObsText(text);
	return dverSummary_;
}

const TLGCObsSummary&  TMeasurements::getRADIObsSummary() const { return radiSummary_; }

const TLGCObsSummary& TMeasurements::getRADIObsSummary(std::string text) {
	radiSummary_.setObsText(text);
	return radiSummary_;
}

const TOBSXYZObsSummary& TMeasurements::getOBSXYZObsSummary() const { return obsxyzSummary_; }

const TOBSXYZObsSummary& TMeasurements::getOBSXYZObsSummary(std::string text) {
	obsxyzSummary_.obsXObsSum.setObsText(text);
	obsxyzSummary_.obsYObsSum.setObsText(text);
	obsxyzSummary_.obsZObsSum.setObsText(text);
	return obsxyzSummary_;
}

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

const TLGCObsSummary& TMeasurements::getINCLYGlobalObsSummary() const { return inclyGlobalSummary_; }

const TLGCObsSummary& TMeasurements::getECWSGlobalObsSummary() const { return ecwsGlobalSummary_; }

#ifdef USE_SERIALIZER
// Inherited via Serializable
void TMeasurements::serialize(SerializerObject::SerializationHelper &obj) const
{
	if (anglGlobalSummary_.getNumberOfObs())
		obj.addProperty("anglGlobalSummary_", anglGlobalSummary_);
	if (dhorGlobalSummary_.getNumberOfObs())
		obj.addProperty("dhorGlobalSummary_", dhorGlobalSummary_);
	if (distGlobalSummary_.getNumberOfObs())
		obj.addProperty("distGlobalSummary_", distGlobalSummary_);
	if (dlevDHORGlobalSummary_.getNumberOfObs())
		obj.addProperty("dlevDHORGlobalSummary_", dlevDHORGlobalSummary_);
	if (dlevGlobalSummary_.getNumberOfObs())
		obj.addProperty("dlevGlobalSummary_", dlevGlobalSummary_);
	if (dsptGlobalSummary_.getNumberOfObs())
		obj.addProperty("dsptGlobalSummary_", dsptGlobalSummary_);

	if (dverSummary_.getNumberOfObs())
		obj.addProperty("dverSummary_", dverSummary_);
	if (ecdirGlobalSummary_.getNumberOfObs())
		obj.addProperty("ecdirGlobalSummary_", ecdirGlobalSummary_);
	if (echoGlobalSummary_.getNumberOfObs())
		obj.addProperty("echoGlobalSummary_", echoGlobalSummary_);
	if (ecspGlobalSummary_.getNumberOfObs())
		obj.addProperty("ecspGlobalSummary_", ecspGlobalSummary_);
	if (ecthGlobalSummary_.getNumberOfObs())
		obj.addProperty("ecthGlobalSummary_", ecthGlobalSummary_);
	if (ecveGlobalSummary_.getNumberOfObs())
		obj.addProperty("ecveGlobalSummary_", ecveGlobalSummary_);
	if (ecwsGlobalSummary_.getNumberOfObs())
		obj.addProperty("ecwsGlobalSummary_", ecwsGlobalSummary_);

	if (inclyGlobalSummary_.getNumberOfObs())
		obj.addProperty("inclyGlobalSummary_", inclyGlobalSummary_);
	if (orieGlobalSummary_.getNumberOfObs())
		obj.addProperty("orieGlobalSummary_", orieGlobalSummary_);
	if (radiSummary_.getNumberOfObs())
		obj.addProperty("radiSummary_", radiSummary_);
	if (zendGlobalSummary_.getNumberOfObs())
		obj.addProperty("zendGlobalSummary_", zendGlobalSummary_);

	obj.addProperty("dverActive", dverActive);
	obj.addProperty("obsxyzActive", obsxyzActive);
	obj.addProperty("radiActive", radiActive);

    if (!obsxyzGlobalSummary_.obsXObsSum.getNumberOfObs())
	    obj.addProperty("obsxyzGlobalSummary_", obsxyzGlobalSummary_);
	
    obj.addProperty("obsxyzSummary_", obsxyzSummary_);
	obj.addProperty("plrGlobalSummary_", plrGlobalSummary_);
	obj.addProperty("uvdGlobalSummary_", uvdGlobalSummary_);
	obj.addProperty("uvecGlobalSummary_", uvecGlobalSummary_);

	if (!fCAM.empty())
		obj.addProperty("fCAM", fCAM);
	if (!fDVER.empty())
		obj.addProperty("fDVER", fDVER);
	if (!fECHO.empty())
		obj.addProperty("fECHO", fECHO);
	if (!fECSP.empty())
		obj.addProperty("fECSP", fECSP);
	if (!fECVE.empty())
		obj.addProperty("fECVE", fECVE);
	if (!fECWS.empty())
		obj.addProperty("fECWS", fECWS);
	if (!fEDM.empty())
		obj.addProperty("fEDM", fEDM);
	if (!fINCLY.empty())
		obj.addProperty("fINCLY", fINCLY);
	if (!fLEVEL.empty())
		obj.addProperty("fLEVEL", fLEVEL);
	if (!fOBSXYZ.empty())
		obj.addProperty("fOBSXYZ", fOBSXYZ);
	if (!fORIE.empty())
		obj.addProperty("fORIE", fORIE);
	if (fPDOR.isInitialised())
        obj.addProperty("fPDOR", fPDOR);
	if (!fRADI.empty())
	    obj.addProperty("fRADI", fRADI);
	if (!fLEVEL.empty())
		obj.addProperty("fLEVEL", fLEVEL);
	if (!fTSTN.empty())
	    obj.addProperty("fTSTN", fTSTN);
}
#endif
