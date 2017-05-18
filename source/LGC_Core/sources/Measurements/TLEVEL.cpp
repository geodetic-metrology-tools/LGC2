#include "TLEVEL.h"
#include "TLGCObsSummary.h"

int TLEVEL::stnCounter = 0;

TLGCObsSummary  TLEVEL::getDLEVObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItDLEV: measDLEV)
		summary.addNewResidual(ItDLEV.getDistanceResidual().getMMetresValue());
	return summary;	
}


TLGCObsSummary  TLEVEL::getDHORObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItDLEV: measDLEV){
		if(ItDLEV.dhor)
			summary.addNewResidual(ItDLEV.dhor->getDistanceResidual().getMMetresValue());
	}
	return summary;	
}
