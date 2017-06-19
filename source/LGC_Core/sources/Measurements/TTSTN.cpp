#include "TTSTN.h"

int TTSTN::stnCounter_ = 0;
int TTSTN::TROM::romCounter_ = 0;

void TTSTN::TROM::initialiseObsSummaries() {
    // First clear the old contents away
    plr3dSummary_.distObsSum.clear();
    plr3dSummary_.anglObsSum.clear();
    plr3dSummary_.zendObsSum.clear();
    anglSummary_.clear();
    zendSummary_.clear();
    distSummary_.clear();
    dhorSummary_.clear();
    ecthSummary_.clear();
    ecdirSummary_.clear();

    // Add the residuals of each measurement:
    
    for(auto& ItANGL : measANGL)
        anglSummary_.addNewResidual(ItANGL.getAngleResidual().getSignedCCValue());

    for(auto& ItZEND : measZEND)
        zendSummary_.addNewResidual(ItZEND.getAngleResidual().getSignedCCValue());

    for(auto& ItDIST : measDIST)
        distSummary_.addNewResidual(ItDIST.getDistanceResidual().getMMetresValue());

    for(auto& ItDHOR : measDHOR)
        dhorSummary_.addNewResidual(ItDHOR.getDistanceResidual().getMMetresValue());

    for(auto& ItECTH : measECTH)
        ecthSummary_.addNewResidual(ItECTH.getDistanceResidual().getMMetresValue());

    for(auto& ItECDIR : measECDIR)
        ecdirSummary_.addNewResidual(ItECDIR.getDistanceResidual().getMMetresValue());

    for(auto& ItPLR3D : measPLR3D){
        plr3dSummary_.distObsSum.addNewResidual(ItPLR3D.getDistanceResidual().getMMetresValue());
        plr3dSummary_.anglObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());
        plr3dSummary_.zendObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());
    }

    // Initialise the obsSummaries:
    if(measPLR3D.size() != 0){
        plr3dSummary_.distObsSum.initialise();
        plr3dSummary_.anglObsSum.initialise();
        plr3dSummary_.zendObsSum.initialise();
    }
    if(measANGL.size() != 0) anglSummary_.initialise();
    if(measZEND.size() != 0) zendSummary_.initialise();
    if(measDIST.size() != 0) distSummary_.initialise();
    if(measDHOR.size() != 0) dhorSummary_.initialise();
    if(measECTH.size() != 0) ecthSummary_.initialise();
    if(measECDIR.size() != 0) ecdirSummary_.initialise();
}

const TLGCObsSummary& TTSTN::TROM::getANGLObsSummary() const { return anglSummary_; }

const TLGCObsSummary& TTSTN::TROM::getZENDObsSummary() const { return zendSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getDISTObsSummary() const { return distSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getDHORObsSummary() const { return dhorSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getECTHObsSummary() const { return ecthSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getECDIRObsSummary() const { return ecdirSummary_; }

const TPOLARObsSummary& TTSTN::TROM::getPLR3DObsSummary() const { return plr3dSummary_; }
