#include "TCAM.h"
#include "TLGCObsSummary.h"

TUVDObsSummary TCAM::getUVDObsSummary()const{
	TUVDObsSummary summary;

	for(auto const& ItUVD: measUVD){
		summary.xVectorCompObsSum.addNewResidual(ItUVD.getXCompVectorResidual());
		summary.yVectorCompObsSum.addNewResidual(ItUVD.getYCompVectorResidual()); 
		summary.distObsSum.addNewResidual(ItUVD.getDistanceResidual() *LGC::M2MM);
	}
	return summary;	
}

TUVECObsSummary TCAM::getUVECObsSummary()const{
	TUVECObsSummary summary;

	for(auto const& ItUVECIt: measUVEC){
		summary.xVectorCompObsSum.addNewResidual(ItUVECIt.getXCompVectorResidual());
		summary.yVectorCompObsSum.addNewResidual(ItUVECIt.getYCompVectorResidual());
	}
	return summary;	
}
