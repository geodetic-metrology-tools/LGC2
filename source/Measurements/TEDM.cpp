#include "TEDM.h"
#include "TLGCObsSummary.h"

TLGCObsSummary  TEDM::getDSPTObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItEDM: measDSPT)
		summary.addNewResidual(ItEDM.getDistanceResidual()*M2MM);
	return summary;	
}