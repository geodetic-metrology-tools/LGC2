#include "RoundOfMeasurements.h"
#include "TLGCObsSummary.h"


TLGCObsSummary  TECHOROM::getECHOObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItECHOROM: measECHO)
		summary.addNewResidual(ItECHOROM.getDistanceResidual()*LGC::M2MM);
	return summary;	
}

TLGCObsSummary  TORIEROM::getORIEObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItORIE: measORIE)
		summary.addNewResidual(ItORIE.getAngleResidual().getSignedCCValue());
	return summary;	
}

