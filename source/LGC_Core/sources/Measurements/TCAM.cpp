#include "TCAM.h"

int TCAM::stnCounter_ = 0;

void TCAM::initialiseObsSummaries() {
    // First clear the old contents away
    uvdSummary_.xVectorCompObsSum.clear();
    uvdSummary_.yVectorCompObsSum.clear();
    uvdSummary_.distObsSum.clear();
    uvecSummary_.xVectorCompObsSum.clear();
    uvecSummary_.yVectorCompObsSum.clear();

    // Add the residuals of each measurement and initialise the obs summaries:

    // UVD
    if(measUVD.size() != 0){
        for(std::list<TUVD>::const_iterator ItUVD = measUVD.begin(); ItUVD != measUVD.end(); ItUVD++) {//auto const& ItUVD: measUVD){
            uvdSummary_.xVectorCompObsSum.addNewResidual(ItUVD->getXCompVectorResidual() * LGC::VECCONVINV);
            uvdSummary_.yVectorCompObsSum.addNewResidual(ItUVD->getYCompVectorResidual() * LGC::VECCONVINV);
            uvdSummary_.distObsSum.addNewResidual(ItUVD->getDistanceResidual().getMMetresValue());
        }

        uvdSummary_.xVectorCompObsSum.initialise();
        uvdSummary_.yVectorCompObsSum.initialise();
        uvdSummary_.distObsSum.initialise();
    }

    // UVEC
    if(measUVEC.size() != 0){
        for(std::list<TUVEC>::const_iterator ItUVEC = measUVEC.begin(); ItUVEC != measUVEC.end(); ItUVEC++) {//auto const& ItUVEC: measUVEC){
            uvecSummary_.xVectorCompObsSum.addNewResidual(ItUVEC->getXCompVectorResidual()* LGC::VECCONVINV);
            uvecSummary_.yVectorCompObsSum.addNewResidual(ItUVEC->getYCompVectorResidual()* LGC::VECCONVINV);
        }

        uvecSummary_.xVectorCompObsSum.initialise();
        uvecSummary_.yVectorCompObsSum.initialise();
    }
}

const TUVDObsSummary& TCAM::getUVDObsSummary() const { return uvdSummary_; }

const TUVECObsSummary& TCAM::getUVECObsSummary() const { return uvecSummary_; }
