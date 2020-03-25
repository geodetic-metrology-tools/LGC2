#include "TEDM.h"

int TEDM::stnCounter_ = 0;

void TEDM::initialiseObsSummaries() {
    // First clear the old contents away
    dsptSummary_.clear();

    if(measDSPT.size() != 0) {
        // Add the residuals of each measurement:
        for(auto const& ItEDM : measDSPT)
            dsptSummary_.addNewResidual(ItEDM.getDistanceResidual().getMMetresValue());

        // Initialise the obsSummaries:
        dsptSummary_.initialise();
    }
}

const TLGCObsSummary& TEDM::getDSPTObsSummary() const { return dsptSummary_; }

const TLGCObsSummary& TEDM::getDSPTObsSummary(std::string text) {
	dsptSummary_.setObsText(text);
	return dsptSummary_;
}
