#include "TTSTN.h"
#include "TLGCObsSummary.h"

TLGCObsSummary TTSTN::TROM::getANGLObsSummary() const{
	TLGCObsSummary summary;
	for(std::vector<TANGL>::const_iterator ItANGL = measANGL.begin() ; ItANGL!=measANGL.end() ; ItANGL++)//auto const& ItANGL: measANGL)
		summary.addNewResidual(ItANGL->getAngleResidual().getSignedCCValue());
	return summary;	
}

TLGCObsSummary TTSTN::TROM::getZENDObsSummary() const{
	TLGCObsSummary summary;
	for(std::vector<TZEND>::const_iterator ItZEND = measZEND.begin() ; ItZEND!=measZEND.end() ; ItZEND++)//auto const& ItZEND: measZEND)
		summary.addNewResidual(ItZEND->getAngleResidual().getSignedCCValue());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getDISTObsSummary() const{
	TLGCObsSummary summary;
	for(std::vector<TLINE>::const_iterator ItDIST = measDIST.begin() ; ItDIST!=measDIST.end() ; ItDIST++)//auto const& ItDIST: measDIST)
		summary.addNewResidual(ItDIST->getDistanceResidual().getMMetresValue());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getDHORObsSummary() const{
	TLGCObsSummary summary;
	for(std::vector<TLINE>::const_iterator ItDHOR = measDHOR.begin() ; ItDHOR!=measDHOR.end() ; ItDHOR++)//auto const& ItDHOR: measDHOR)
		summary.addNewResidual(ItDHOR->getDistanceResidual().getMMetresValue());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getECTHObsSummary() const{
	TLGCObsSummary summary;
	for(std::vector<TECTH>::const_iterator ItECTH = measECTH.begin() ; ItECTH!=measECTH.end() ; ItECTH++)//auto const& ItDHOR: measDHOR)
		summary.addNewResidual(ItECTH->getMeasuredValueResidual().getMMetresValue());
	return summary;	
}

TPOLARObsSummary TTSTN::TROM::getPLR3DObsSummary()const{
	TPOLARObsSummary summary;

	for(std::vector<TPLR3D>::const_iterator ItPLR3D = measPLR3D.begin() ; ItPLR3D!=measPLR3D.end() ; ItPLR3D++) {//auto const& ItPLR3D: measPLR3D)
		summary.distObsSum.addNewResidual(ItPLR3D->getDistanceResidual().getMMetresValue());
		summary.anglObsSum.addNewResidual(ItPLR3D->getAngleResidual(EPLR3DAngles::kANGL).getSignedCCValue());
		summary.zendObsSum.addNewResidual(ItPLR3D->getAngleResidual(EPLR3DAngles::kZEND).getSignedCCValue());
	}
	return summary;	
}
