#include "TINCL.h"

int TINCL::stnCounter_ = 0;

void TINCL::initialiseObsSummaries() {
}

const TLGCObsSummary& TINCL::getINCLYObsSummary() const { return inclySummary_; }
