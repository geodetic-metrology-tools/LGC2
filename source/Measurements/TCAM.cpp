#include "TCAM.h"
#include "TLGCObsSummary.h"

TUVDObsSummary TCAM::getUVDObsSummary()const{
	TUVDObsSummary summary;

	for(std::vector<TUVD>::const_iterator ItUVD = measUVD.begin() ; ItUVD!=measUVD.end() ; ItUVD++) {//auto const& ItUVD: measUVD){
		summary.xVectorCompObsSum.addNewResidual(ItUVD->getXCompVectorResidual() * LGC::VECCONVINV);
		summary.yVectorCompObsSum.addNewResidual(ItUVD->getYCompVectorResidual() * LGC::VECCONVINV); 
		summary.distObsSum.addNewResidual(ItUVD->getDistanceResidual() * M2MM);
	}
	return summary;	
}

TUVECObsSummary TCAM::getUVECObsSummary()const{
	TUVECObsSummary summary;

	for(std::vector<TUVEC>::const_iterator ItUVEC = measUVEC.begin() ; ItUVEC!=measUVEC.end() ; ItUVEC++) {//auto const& ItUVEC: measUVEC){
		summary.xVectorCompObsSum.addNewResidual(ItUVEC->getXCompVectorResidual()* LGC::VECCONVINV);
		summary.yVectorCompObsSum.addNewResidual(ItUVEC->getYCompVectorResidual()* LGC::VECCONVINV);
	}
	return summary;	
}
