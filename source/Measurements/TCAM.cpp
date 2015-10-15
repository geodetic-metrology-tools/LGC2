#include "TCAM.h"
#include "TLGCObsSummary.h"

TPLR3DObsSummary TCAM::getPLR3DObsSummary()const{
	TPLR3DObsSummary summary;

	for(auto const& ItPLR3D: measPLR3D){
		summary.distObsSum.addNewResidual(ItPLR3D.getScalarResidual()*LGC::M2MM);
		summary.anglObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kANGL).cc());
		summary.zendObsSum.addNewResidual(ItPLR3D.getAngleResidual(EPLR3DAngles::kZEND).cc());
	}
	return summary;	
}

TDIR3DObsSummary TCAM::getDIR3DObsSummary()const{
	TDIR3DObsSummary summary;

	for(auto const& ItDIR3D: measDIR3D){
		summary.anglObsSum.addNewResidual(ItDIR3D.getAngleResidual(EPLR3DAngles::kANGL).cc());
		summary.zendObsSum.addNewResidual(ItDIR3D.getAngleResidual(EPLR3DAngles::kZEND).cc());
	}
	return summary;	
}