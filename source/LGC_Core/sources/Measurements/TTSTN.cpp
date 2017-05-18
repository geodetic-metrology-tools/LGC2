#include <TTSTN.h>
#include "TLGCObsSummary.h"

int TTSTN::stnCounter = 0;
int TTSTN::TROM::romCounter = 0;

TLGCObsSummary TTSTN::TROM::getANGLObsSummary() const{
	TLGCObsSummary summary;
	for(auto& ItANGL:measANGL)
		summary.addNewResidual(ItANGL.getAngleResidual().getSignedCCValue());
	return summary;	
}

TLGCObsSummary TTSTN::TROM::getZENDObsSummary() const{
	TLGCObsSummary summary;
	for(auto& ItZEND:measZEND)
		summary.addNewResidual(ItZEND.getAngleResidual().getSignedCCValue());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getDISTObsSummary() const{
	TLGCObsSummary summary;
	for(auto& ItDIST:measDIST)
		summary.addNewResidual(ItDIST.getDistanceResidual().getMMetresValue());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getDHORObsSummary() const{
	TLGCObsSummary summary;
	for(auto& ItDHOR:measDHOR)
		summary.addNewResidual(ItDHOR.getDistanceResidual().getMMetresValue());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getECTHObsSummary() const{
	TLGCObsSummary summary;
	for(auto& ItECTH:measECTH)
		summary.addNewResidual(ItECTH.getDistanceResidual().getMMetresValue());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getECDIRObsSummary() const{
	TLGCObsSummary summary;
	for (auto& ItECDIR :measECDIR)
		summary.addNewResidual(ItECDIR.getDistanceResidual().getMMetresValue());
	return summary;
}

TPOLARObsSummary TTSTN::TROM::getPLR3DObsSummary()const{
	TPOLARObsSummary summary;

	for(auto& ItPLR3D:measPLR3D){
		summary.distObsSum.addNewResidual(ItPLR3D.getDistanceResidual().getMMetresValue());
		summary.anglObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());
		summary.zendObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());
	}
	return summary;	
}
