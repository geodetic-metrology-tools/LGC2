#include "TTSTN.h"
#include "TLGCObsSummary.h"


#if 0
TLGCObsSummary TTSTN::TROM::getPLR3DObsSummary(){

	TLGCObsSummary summary;

	// clear the observation summary statistics
	summary.clear();

	for(auto const& ItPLR3D: measPLR3D)
	{
		summary.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL getResidue().getSignedCCValue());

			iter++;
		}

		fZENIStatisticsOK = true;
	}

	return;
	
}

#endif


TLGCObsSummary TTSTN::TROM::getANGLObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItANGL: measANGL)
		summary.addNewResidual(ItANGL.getAngleResidual().cc());
	return summary;	
}

TLGCObsSummary TTSTN::TROM::getZENDObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItZEND: measZEND)
		summary.addNewResidual(ItZEND.getAngleResidual().cc());
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getDISTObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItDIST: measDIST)
		summary.addNewResidual(ItDIST.getScalarResidual()*LGC::M2MM);
	return summary;	
}

TLGCObsSummary  TTSTN::TROM::getDHORObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItDHOR: measDHOR)
		summary.addNewResidual(ItDHOR.getScalarResidual()*LGC::M2MM);
	return summary;	
}

T3ObsSummary TTSTN::TROM::getPLR3DObsSummary()const{
	T3ObsSummary summary;

	for(auto const& ItPLR3D: measPLR3D){
		summary.distObsSum.addNewResidual(ItPLR3D.getScalarResidual()*LGC::M2MM);
		summary.anglObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).cc());
		summary.zendObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).cc());
	}
	return summary;	
}

T2ObsSummary TTSTN::TROM::getDIR3DObsSummary()const{
	T2ObsSummary summary;

	for(auto const& ItDIR3D: measDIR3D){
		summary.anglObsSum.addNewResidual(ItDIR3D.getAngleResidual(EPLR3DAngles::kANGL).cc());
		summary.zendObsSum.addNewResidual(ItDIR3D.getAngleResidual(EPLR3DAngles::kZEND).cc());
	}
	return summary;	
}