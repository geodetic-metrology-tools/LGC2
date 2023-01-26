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

    // Add the residuals of each measurement and initialise the obs summaries:
    
    if(measANGL.size() != 0) {
        for(auto& ItANGL : measANGL)
            anglSummary_.addNewResidual(ItANGL.getAngleResidual().getSignedCCValue());

        anglSummary_.initialise();
    }

    if(measZEND.size() != 0) {
        for(auto& ItZEND : measZEND)
            zendSummary_.addNewResidual(ItZEND.getAngleResidual().getSignedCCValue());
    
        zendSummary_.initialise();
    }

    if(measDIST.size() != 0){
        for(auto& ItDIST : measDIST)
            distSummary_.addNewResidual(ItDIST.getDistanceResidual().getMMetresValue());

        distSummary_.initialise();
    }

    if(measDHOR.size() != 0) {
        for(auto& ItDHOR : measDHOR)
            dhorSummary_.addNewResidual(ItDHOR.getDistanceResidual().getMMetresValue());

        dhorSummary_.initialise();
    }
    
    if(measECTH.size() != 0) {
        for(auto& ItECTH : measECTH)
            ecthSummary_.addNewResidual(ItECTH.getDistanceResidual().getMMetresValue());

        ecthSummary_.initialise();
    }

    if(measECDIR.size() != 0){
        for(auto& ItECDIR : measECDIR)
            ecdirSummary_.addNewResidual(ItECDIR.getDistanceResidual().getMMetresValue());

        ecdirSummary_.initialise();
    }

    if(measPLR3D.size() != 0){
        for(auto& ItPLR3D : measPLR3D){
            plr3dSummary_.distObsSum.addNewResidual(ItPLR3D.getDistanceResidual().getMMetresValue());
            plr3dSummary_.anglObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());
            plr3dSummary_.zendObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());
        }

        plr3dSummary_.distObsSum.initialise();
        plr3dSummary_.anglObsSum.initialise();
        plr3dSummary_.zendObsSum.initialise();
    }
}

const TLGCObsSummary& TTSTN::TROM::getANGLObsSummary() const { return anglSummary_; }

const TLGCObsSummary& TTSTN::TROM::getANGLObsSummary(std::string text) noexcept {
	anglSummary_.setObsText(text);
	return anglSummary_;
}

const TLGCObsSummary& TTSTN::TROM::getZENDObsSummary() const { return zendSummary_; }

const TLGCObsSummary& TTSTN::TROM::getZENDObsSummary(std::string text) noexcept {
	zendSummary_.setObsText(text);
	return zendSummary_;
}

const TLGCObsSummary&  TTSTN::TROM::getDISTObsSummary() const { return distSummary_; }

const TLGCObsSummary& TTSTN::TROM::getDISTObsSummary(std::string text) noexcept {
	distSummary_.setObsText(text);
	return distSummary_;
}

const TLGCObsSummary&  TTSTN::TROM::getDHORObsSummary() const { return dhorSummary_; }

const TLGCObsSummary& TTSTN::TROM::getDHORObsSummary(std::string text) noexcept {
	dhorSummary_.setObsText(text);
	return dhorSummary_;
}

const TLGCObsSummary&  TTSTN::TROM::getECTHObsSummary() const { return ecthSummary_; }

const TLGCObsSummary& TTSTN::TROM::getECTHObsSummary(std::string text) noexcept {
	ecthSummary_.setObsText(text);
	return ecthSummary_;
}

const TLGCObsSummary&  TTSTN::TROM::getECDIRObsSummary() const { return ecdirSummary_; }

const TLGCObsSummary& TTSTN::TROM::getECDIRObsSummary(std::string text) noexcept {
	ecdirSummary_.setObsText(text);
	return ecdirSummary_;
}

const TPOLARObsSummary& TTSTN::TROM::getPLR3DObsSummary() const { return plr3dSummary_; }

const TPOLARObsSummary& TTSTN::TROM::getPLR3DObsSummary(std::string text) noexcept {
	plr3dSummary_.anglObsSum.setObsText(text);
	plr3dSummary_.zendObsSum.setObsText(text);
	plr3dSummary_.distObsSum.setObsText(text);
	return plr3dSummary_;
}


#ifdef USE_SERIALIZER
// Inherited via Serializable
void TTSTN::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("ihfix", ihfix);
	obj.addProperty("instrument", instrument);
	obj.addProperty("instrumentHeightAdjustable", instrumentHeightAdjustable);
	obj.addProperty("instrumentPos", instrumentPos);
	obj.addProperty("line", line);
	//obj.addProperty("roms");
	obj.addProperty("rot3D", rot3D);
	obj.addProperty("rotX", rotX);
	obj.addProperty("rotY", rotY);
	obj.addProperty("stnCounter_", stnCounter_);
	obj.addProperty("stnId", stnId);
}
#endif

#ifdef USE_SERIALIZER
// Inherited via Serializable
void TTSTN::TROM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("acst", acst);
	obj.addProperty("anglActive", anglActive);
	obj.addProperty("anglSummary_", anglSummary_);
	obj.addProperty("defaultTargetId", defaultTargetId);
	obj.addProperty("dhorActive", dhorActive);
	obj.addProperty("dhorSummary_", dhorSummary_);
	obj.addProperty("distActive", distActive);
	obj.addProperty("distSummary_", distSummary_);
	obj.addProperty("ecdirActive", ecdirActive);
	obj.addProperty("ecdirSummary_", ecdirSummary_);
	obj.addProperty("ecthActive", ecthActive);
	obj.addProperty("ecthSummary_", ecthSummary_);
	obj.addProperty("measANGL", measANGL);
	obj.addProperty("measDHOR", measDHOR);
	obj.addProperty("measDIST", measDIST);
	obj.addProperty("measECDIR", measECDIR);
	obj.addProperty("measECTH", measECTH);
	obj.addProperty("measPLR3D", measPLR3D);
	obj.addProperty("measZEND", measZEND);
	obj.addProperty("plr3dSummary_", plr3dSummary_);
	obj.addProperty("plrActive", plrActive);
	obj.addProperty("romCounter_", romCounter_);
	obj.addProperty("romId", romId);
	obj.addProperty("v0", v0);
	obj.addProperty("zendActive", zendActive);
	obj.addProperty("zendSummary_", zendSummary_);
}
#endif

