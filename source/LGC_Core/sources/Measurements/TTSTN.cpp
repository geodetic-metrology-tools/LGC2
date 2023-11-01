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


#if USE_SERIALIZER
// Inherited via Serializable
void TTSTN::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("ihfix", ihfix);
	obj.addProperty("instrument", instrument);
	obj.addProperty("instrumentHeightAdjustable", instrumentHeightAdjustable);
	if (instrumentPos)
		obj.addProperty("instrumentPos", instrumentPos->getName());
	obj.addProperty("line", line);
	if (!roms.empty())
		obj.addProperty("roms", roms);
	obj.addProperty("rot3D", rot3D);
	obj.addProperty("rotX", rotX);
	obj.addProperty("rotY", rotY);
	obj.addProperty("stnId", stnId);
}
#endif

#if USE_SERIALIZER
// Inherited via Serializable
void TTSTN::TROM::serialize(ObjectSerializer &obj) const
{
	TStatusObject::serialize(obj);
	obj.addProperty("acst", acst.getRadiansValue());
	obj.addProperty("anglActive", anglActive);
	if (anglSummary_.getNumberOfObs())
		obj.addProperty("anglSummary_", anglSummary_);
	obj.addProperty("defaultTargetId", defaultTargetId);
	obj.addProperty("dhorActive", dhorActive);
	if (dhorSummary_.getNumberOfObs())
		obj.addProperty("dhorSummary_", dhorSummary_);
	obj.addProperty("distActive", distActive);
	if (distSummary_.getNumberOfObs())
		obj.addProperty("distSummary_", distSummary_);
	obj.addProperty("ecdirActive", ecdirActive);
	if (ecdirSummary_.getNumberOfObs())
		obj.addProperty("ecdirSummary_", ecdirSummary_);
	obj.addProperty("ecthActive", ecthActive);
	if (ecthSummary_.getNumberOfObs())
		obj.addProperty("ecthSummary_", ecthSummary_);
	if (!measANGL.empty())
		obj.addProperty("measANGL", measANGL);
	if (!measDHOR.empty())
		obj.addProperty("measDHOR", measDHOR);
	if (!measDIST.empty())
		obj.addProperty("measDIST", measDIST);
	if (!measECDIR.empty())
		obj.addProperty("measECDIR", measECDIR);
	if (!measECTH.empty())
		obj.addProperty("measECTH", measECTH);
	if (!measPLR3D.empty())
		obj.addProperty("measPLR3D", measPLR3D);
	if (!measZEND.empty())
		obj.addProperty("measZEND", measZEND);
	if (plr3dSummary_.anglObsSum.getNumberOfObs())
		obj.addProperty("plr3dSummary_", plr3dSummary_);
	obj.addProperty("plrActive", plrActive);
	obj.addProperty("romId", romId);
	obj.addProperty("v0", v0);
	obj.addProperty("zendActive", zendActive);
	if (zendSummary_.getNumberOfObs())
		obj.addProperty("zendSummary_", zendSummary_);
}
#endif

