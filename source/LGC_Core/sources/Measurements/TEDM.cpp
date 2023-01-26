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

#ifdef USE_SERIALIZER
// Inherited via Serializable
void TEDM::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("dsptSummary_", dsptSummary_);
	obj.addProperty("instrument", instrument);
	obj.addProperty("instrumentPos", instrumentPos);
	obj.addProperty("line", line);
	obj.addProperty("measDSPT", measDSPT);
	obj.addProperty("stnCounter_", stnCounter_);
	obj.addProperty("stnId", stnId);
}
#endif