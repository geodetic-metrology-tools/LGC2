// SPDX-FileCopyrightText: 2025 CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TEDM.h"
#include <LGCAdjustablePoint.h>

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

const TLGCObsSummary& TEDM::getDSPTObsSummary(std::string text) noexcept {
	dsptSummary_.setObsText(text);
	return dsptSummary_;
}

#if USE_SERIALIZER
// Inherited via Serializable
void TEDM::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("dsptSummary_", dsptSummary_);
	obj.addProperty("instrument", instrument);
	if (instrumentPos)
		obj.addProperty("instrumentPos", instrumentPos->getName());
	obj.addProperty("line", line);
	obj.addProperty("measDSPT", measDSPT);
	obj.addProperty("stnId", stnId);
}
#endif
