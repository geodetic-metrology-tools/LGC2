#include "TINCL.h"

int TINCL::stnCounter_ = 0;

void TINCL::initialiseObsSummaries() {
	inclySummary_.clear();

	// Add the residuals of each measurement and initialise the obs summaries:

	if (measINCLY.size() != 0) {
		for (auto& ItINCLY : measINCLY)
			inclySummary_.addNewResidual(ItINCLY.getAngleResidual().getSignedCCValue());

		inclySummary_.initialise();
	}
}

const TLGCObsSummary& TINCL::getINCLYObsSummary() const { return inclySummary_; }
