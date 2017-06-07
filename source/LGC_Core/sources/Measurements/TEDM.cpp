#include "TEDM.h"

int TEDM::stnCounter_ = 0;

void TEDM::initialiseObsSummaries() {
    // First clear the old contents away
    dsptSummary_.clear();

    // Add the residuals of each measurement:
    for(auto const& ItEDM : measDSPT)
        dsptSummary_.addNewResidual(ItEDM.getDistanceResidual().getMMetresValue());
}

TLGCObsSummary  TEDM::getDSPTObsSummary() const { return dsptSummary_; }
