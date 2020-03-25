#include "TLEVEL.h"

int TLEVEL::stnCounter_ = 0;

void TLEVEL::initialiseObsSummaries() {
    // First clear the old contents away
    dlevSummary_.clear();
    dhorSummary_.clear();

    if(measDLEV.size() != 0) {
        // Add the residuals of each measurement:
        for(auto const& ItDLEV : measDLEV){
            dlevSummary_.addNewResidual(ItDLEV.getDistanceResidual().getMMetresValue());
            if(ItDLEV.dhor)
                dhorSummary_.addNewResidual(ItDLEV.dhor->getDistanceResidual().getMMetresValue());
        }

        // Initialise the obsSummaries:
        dlevSummary_.initialise();
        if(dhorSummary_.getNumberOfObs() != 0) dhorSummary_.initialise();
    }
}

const TLGCObsSummary&  TLEVEL::getDLEVObsSummary() const { return dlevSummary_; }

const TLGCObsSummary& TLEVEL::getDLEVObsSummary(std::string text) {
	dlevSummary_.setObsText(text);
	return dlevSummary_;
}

const TLGCObsSummary&  TLEVEL::getDHORObsSummary() const { return dhorSummary_; }

const TLGCObsSummary& TLEVEL::getDHORObsSummary(std::string text) {
	dhorSummary_.setObsText(text);
	return dhorSummary_;
}

