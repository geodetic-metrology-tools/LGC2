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
}

TLGCObsSummary TTSTN::TROM::getANGLObsSummary() const {	return anglSummary_; }

TLGCObsSummary TTSTN::TROM::getZENDObsSummary() const { return zendSummary_; }

TLGCObsSummary  TTSTN::TROM::getDISTObsSummary() const { return distSummary_; }

TLGCObsSummary  TTSTN::TROM::getDHORObsSummary() const { return dhorSummary_; }

TLGCObsSummary  TTSTN::TROM::getECTHObsSummary() const { return ecthSummary_; }

TLGCObsSummary  TTSTN::TROM::getECDIRObsSummary() const { return ecdirSummary_; }

TPOLARObsSummary TTSTN::TROM::getPLR3DObsSummary() const { return plr3dSummary_; }
