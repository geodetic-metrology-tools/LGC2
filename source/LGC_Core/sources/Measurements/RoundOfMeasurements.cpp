#include "RoundOfMeasurements.h"

int TECHOROM::romCounter_ = 0;
int TECSPROM::romCounter_ = 0;
int TECVEROM::romCounter_ = 0;
int TORIEROM::romCounter_ = 0;

void TECHOROM::initialiseObsSummaries() {
    // First clear the old contents away
    echoSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measECHO.size() != 0) {
        for(auto const& ItECHOROM : measECHO)
            echoSummary_.addNewResidual(ItECHOROM.getDistanceResidual().getMMetresValue());

        echoSummary_.initialise();
    }
}

void TECSPROM::initialiseObsSummaries() {
    // First clear the old contents away
    ecspSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measECSP.size() != 0) {
        for(auto const& ItECSPROM : measECSP)
            ecspSummary_.addNewResidual(ItECSPROM.getDistanceResidual().getMMetresValue());

        ecspSummary_.initialise();
    }
}

void TECVEROM::initialiseObsSummaries() {
    // First clear the old contents away
    ecveSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measECVE.size() != 0) {
        for(auto const& ItECVEROM : measECVE)
            ecveSummary_.addNewResidual(ItECVEROM.getDistanceResidual().getMMetresValue());

        ecveSummary_.initialise();
    }
}

void TORIEROM::initialiseObsSummaries() {
    // First clear the old contents away
    orieSummary_.clear();

    // Add the residuals of each measurement and initialise the obsSummaries:
    if(measORIE.size() != 0) {
        for(auto const& ItORIE : measORIE)
            orieSummary_.addNewResidual(ItORIE.getAngleResidual().getSignedCCValue());

        orieSummary_.initialise();
    }
}

const TLGCObsSummary&  TECHOROM::getECHOObsSummary() const { return echoSummary_; }

const TLGCObsSummary&  TECSPROM::getECSPObsSummary() const { return ecspSummary_; }

const TLGCObsSummary&  TECVEROM::getECVEObsSummary() const { return ecveSummary_; }

const TLGCObsSummary&  TORIEROM::getORIEObsSummary() const { return orieSummary_; }

