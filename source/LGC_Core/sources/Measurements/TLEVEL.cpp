#include "TLEVEL.h"

int TLEVEL::stnCounter_ = 0;

void TLEVEL::initialiseObsSummaries() {
    // First clear the old contents away
    dlevSummary_.clear();
    dhorSummary_.clear();

    // Add the residuals of each measurement:

    for(auto const& ItDLEV : measDLEV){
        dlevSummary_.addNewResidual(ItDLEV.getDistanceResidual().getMMetresValue());
        if(ItDLEV.dhor)
            dhorSummary_.addNewResidual(ItDLEV.dhor->getDistanceResidual().getMMetresValue());
    }
}

TLGCObsSummary  TLEVEL::getDLEVObsSummary() const { return dlevSummary_; }


TLGCObsSummary  TLEVEL::getDHORObsSummary() const { return dhorSummary_; }
