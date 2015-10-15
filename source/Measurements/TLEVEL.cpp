#include "TLEVEL.h"
#include "TLGCObsSummary.h"

TLGCObsSummary  TLEVEL::getDLEVObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItDLEV: measDLEV)
		summary.addNewResidual(ItDLEV.getDistanceResidual()*LGC::M2MM);
	return summary;	
}


TLGCObsSummary  TLEVEL::getDHORObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItDLEV: measDLEV){
		if(ItDLEV.dhor)
			summary.addNewResidual(ItDLEV.dhor->getDistanceResidual()*LGC::M2MM);
	}
	return summary;	
}
