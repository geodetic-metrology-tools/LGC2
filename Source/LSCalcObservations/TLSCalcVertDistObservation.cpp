//TLSCalcVertDistObservation.cpp

#include "TLSCalcVertDistObservation.h"
#include "TVerticalDistMeasurement.h"


///////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
///////////////////////////////////////////////////////////////////////////////////////////

TLSCalcVertDistObservation::TLSCalcVertDistObservation(const TVerticalDistMeasurement* vdmeas,LSPosVecIter tg1,LSPosVecIter tg2, LSLengthIter cte):
TALSCalcLengthObservation(*(vdmeas->getObservedValue()),TLength(0)),
fVertDistObs(vdmeas), fDistConst(cte)
{/*constructor taking a vertical distance measurement and pointers to
 lscalcparameters to initialize the lscalcvertdist observation's attributes*/
	fTg1Point = tg1;
	fTg2Point = tg2;
}
