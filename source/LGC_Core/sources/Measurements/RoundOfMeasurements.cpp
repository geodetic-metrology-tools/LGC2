#include "RoundOfMeasurements.h"
#include "TLGCObsSummary.h"


int TECHOROM::romCounter = 0;
int TECSPROM::romCounter = 0;
int TECVEROM::romCounter = 0;
int TORIEROM::romCounter = 0;

TLGCObsSummary  TECHOROM::getECHOObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItECHOROM: measECHO)
		summary.addNewResidual(ItECHOROM.getDistanceResidual().getMMetresValue());
	return summary;	
}

TLGCObsSummary  TECSPROM::getECSPObsSummary() const{
	TLGCObsSummary summary;
	for (auto const& ItECSPROM : measECSP)
		summary.addNewResidual(ItECSPROM.getDistanceResidual().getMMetresValue());
	return summary;
}

TLGCObsSummary  TECVEROM::getECVEObsSummary() const{
	TLGCObsSummary summary;
	for (auto const& ItECVEROM : measECVE)
		summary.addNewResidual(ItECVEROM.getDistanceResidual().getMMetresValue());
	return summary;
}

TLGCObsSummary  TORIEROM::getORIEObsSummary() const{
	TLGCObsSummary summary;
	for(auto const& ItORIE: measORIE)
		summary.addNewResidual(ItORIE.getAngleResidual().getSignedCCValue());
	return summary;	
}

