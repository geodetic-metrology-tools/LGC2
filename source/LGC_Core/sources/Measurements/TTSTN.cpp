#include "TTSTN.h"

int TTSTN::stnCounter_ = 0;
int TTSTN::TROM::romCounter_ = 0;

void TTSTN::TROM::initialiseObsSummaries() {
	// First clear the old contents away
	plr3dSummary_.distObsSum.clear();
	plr3dSummary_.anglObsSum.clear();
	plr3dSummary_.zendObsSum.clear();
	plr3dSummary_.anglObsOffsetSum.clear();
	plr3dSummary_.zendObsOffsetSum.clear();
	anglSummary_.clear();
	zendSummary_.clear();
	anglOffsetsSummary_.clear();
	zendOffsetsSummary_.clear();
	distSummary_.clear();
	dhorSummary_.clear();
	ecthSummary_.clear();
	ecdirSummary_.clear();

	// Add the residuals of each measurement and initialise the obs summaries:

	if (measANGL.size() != 0) {
		for (auto& ItANGL : measANGL) {
			anglSummary_.addNewResidual(ItANGL.getAngleResidual().getSignedCCValue());
			anglOffsetsSummary_.addNewResidual(ItANGL.getAngleResidualOffset().getMMetresValue());  // Adds the corresponding residual estimated in MM
		}
		anglSummary_.initialise();
		anglOffsetsSummary_.initialise();
	}


	if (measZEND.size() != 0) {
		for (auto& ItZEND : measZEND) {
			zendSummary_.addNewResidual(ItZEND.getAngleResidual().getSignedCCValue());
			zendOffsetsSummary_.addNewResidual(ItZEND.getAngleResidualOffset().getMMetresValue());  // Adds the corresponding residual estimated in MM
		}
		zendSummary_.initialise();
		zendOffsetsSummary_.initialise();
	}

	if (measDIST.size() != 0) {
		for (auto& ItDIST : measDIST)
			distSummary_.addNewResidual(ItDIST.getDistanceResidual().getMMetresValue());

		distSummary_.initialise();
	}

	if (measDHOR.size() != 0) {
		for (auto& ItDHOR : measDHOR)
			dhorSummary_.addNewResidual(ItDHOR.getDistanceResidual().getMMetresValue());

		dhorSummary_.initialise();
	}

	if (measECTH.size() != 0) {
		for (auto& ItECTH : measECTH)
			ecthSummary_.addNewResidual(ItECTH.getDistanceResidual().getMMetresValue());

		ecthSummary_.initialise();
	}

	if (measECDIR.size() != 0) {
		for (auto& ItECDIR : measECDIR)
			ecdirSummary_.addNewResidual(ItECDIR.getDistanceResidual().getMMetresValue());

		ecdirSummary_.initialise();
	}

	if (measPLR3D.size() != 0) {
		for (auto& ItPLR3D : measPLR3D) {
			plr3dSummary_.distObsSum.addNewResidual(ItPLR3D.getDistanceResidual().getMMetresValue());

			plr3dSummary_.anglObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());
			plr3dSummary_.anglObsOffsetSum.addNewResidual(ItPLR3D.getAngleResidualOffset(EPLR3DAngles::kANGL).getMMetresValue());  // Adds the corresponding residual estimated in MM

			plr3dSummary_.zendObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());
			plr3dSummary_.anglObsOffsetSum.addNewResidual(ItPLR3D.getAngleResidualOffset(EPLR3DAngles::kZEND).getMMetresValue());  // Adds the corresponding residual estimated in MM
		}

		plr3dSummary_.distObsSum.initialise();
		plr3dSummary_.anglObsSum.initialise();
		plr3dSummary_.zendObsSum.initialise();
		plr3dSummary_.anglObsOffsetSum.initialise();
		plr3dSummary_.zendObsOffsetSum.initialise();

	}
}

const TLGCObsSummary& TTSTN::TROM::getANGLObsSummary() const { return anglSummary_; }

const TLGCObsSummary& TTSTN::TROM::getZENDObsSummary() const { return zendSummary_; }

const TLGCObsSummary& TTSTN::TROM::getANGLOffsetsObsSummary() const { return anglOffsetsSummary_; }

const TLGCObsSummary& TTSTN::TROM::getZENDOffsetsObsSummary() const { return zendOffsetsSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getDISTObsSummary() const { return distSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getDHORObsSummary() const { return dhorSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getECTHObsSummary() const { return ecthSummary_; }

const TLGCObsSummary&  TTSTN::TROM::getECDIRObsSummary() const { return ecdirSummary_; }

const TPOLARObsSummary& TTSTN::TROM::getPLR3DObsSummary() const { return plr3dSummary_; }



