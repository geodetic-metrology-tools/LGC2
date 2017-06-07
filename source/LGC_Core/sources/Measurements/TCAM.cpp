#include "TCAM.h"

int TCAM::stnCounter_ = 0;

void TCAM::initialiseObsSummaries() {
    // First clear the old contents away
    uvdSummary_.xVectorCompObsSum.clear();
    uvdSummary_.yVectorCompObsSum.clear();
    uvdSummary_.distObsSum.clear();
    uvecSummary_.xVectorCompObsSum.clear();
    uvecSummary_.yVectorCompObsSum.clear();

    // Add the residuals of each measurement:

    for(std::list<TUVD>::const_iterator ItUVD = measUVD.begin(); ItUVD != measUVD.end(); ItUVD++) {//auto const& ItUVD: measUVD){
        uvdSummary_.xVectorCompObsSum.addNewResidual(ItUVD->getXCompVectorResidual() * LGC::VECCONVINV);
        uvdSummary_.yVectorCompObsSum.addNewResidual(ItUVD->getYCompVectorResidual() * LGC::VECCONVINV);
        uvdSummary_.distObsSum.addNewResidual(ItUVD->getDistanceResidual().getMMetresValue());
    }

    for(std::list<TUVEC>::const_iterator ItUVEC = measUVEC.begin(); ItUVEC != measUVEC.end(); ItUVEC++) {//auto const& ItUVEC: measUVEC){
        uvecSummary_.xVectorCompObsSum.addNewResidual(ItUVEC->getXCompVectorResidual()* LGC::VECCONVINV);
        uvecSummary_.yVectorCompObsSum.addNewResidual(ItUVEC->getYCompVectorResidual()* LGC::VECCONVINV);
    }
}

TUVDObsSummary TCAM::getUVDObsSummary() const { return uvdSummary_; }

TUVECObsSummary TCAM::getUVECObsSummary() const { return uvecSummary_; }
