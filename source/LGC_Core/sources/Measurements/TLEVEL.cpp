#include "TLEVEL.h"
#include <LGCAdjustablePoint.h>

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

const TLGCObsSummary& TLEVEL::getDLEVObsSummary(std::string text) noexcept {
	dlevSummary_.setObsText(text);
	return dlevSummary_;
}

const TLGCObsSummary&  TLEVEL::getDHORObsSummary() const { return dhorSummary_; }

const TLGCObsSummary& TLEVEL::getDHORObsSummary(std::string text) noexcept {
	dhorSummary_.setObsText(text);
	return dhorSummary_;
}

#if USE_SERIALIZER
// Inherited via Serializable
void TLEVEL::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("dhorSummary_", dhorSummary_);
	obj.addProperty("dlevSummary_", dlevSummary_);
	obj.addProperty("fMeasuredPlane", fMeasuredPlane);
	obj.addProperty("fRefPt", fRefPt);
	obj.addProperty("hasDHOR", hasDHOR);
	obj.addProperty("instrument", instrument);
	obj.addProperty("line", line);
	obj.addProperty("measDLEV", measDLEV);
	obj.addProperty("stnId", stnId);
}
#endif
