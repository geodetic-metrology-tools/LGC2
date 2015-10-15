//TLSCalc2DOffsetObservation.cpp

#include "TLSCalc2DOffsetObservation.h"
#include "T2DOffsetObservation.h"
#include "T2DOffsetMeasurement.h"

////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////

// Default constructor
TLSCalc2DOffsetObservation::TLSCalc2DOffsetObservation() {
	//sets attributes to 0
	fVOffset = TLength(LITERAL(0.0));
	fVOffSigmaAPriori = TLength(LITERAL(0.0));
	fHOffset = TLength(LITERAL(0.0));
	fHOffSigmaAPriori = TLength(LITERAL(0.0));
	fVEstimatedOff = TLength(LITERAL(0.0));
	fHEstimatedOff = TLength(LITERAL(0.0));
	fVOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fHOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fVOffResidue = TLength(LITERAL(0.0));
	fHOffResidue = TLength(LITERAL(0.0));
	fEnd1Point = 0;
	fEnd2Point = 0;
	fStPoint = 0;
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;
}

// constructor taking an 2D offset measurement and pointers to the lscalcparameters  to initialize the lscalc2doffset observation's attributes
TLSCalc2DOffsetObservation::TLSCalc2DOffsetObservation(T2DOffsetMeasurement& offmeas,TSpatialPoint* st,
													   TSpatialPoint* ex1,TSpatialPoint* ex2):
fEnd1Point(ex1),fEnd2Point(ex2),fStPoint(st) {
	fVOffset = offmeas.getVertOffset();
	fVOffSigmaAPriori = offmeas.getVertSigma();
	fHOffset = offmeas.getHorOffset();
	fHOffSigmaAPriori = offmeas.getHorSigma();
	fVEstimatedOff = fVOffset;
	fHEstimatedOff = fHOffset;
	fVOffSigmaAPosteriori = fVOffSigmaAPriori;
	fHOffSigmaAPosteriori = fHOffSigmaAPriori;
	fVOffResidue = TLength(LITERAL(0.0));
	fHOffResidue = TLength(LITERAL(0.0));

	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;	
}

/*
// copy constructor
TLSCalc2DOffsetObservation::TLSCalc2DOffsetObservation(const TLSCalc2DOffsetObservation& source) 
{
	// not implemented
}

// copy assignment operator
TLSCalc2DOffsetObservation& TLSCalc2DOffsetObservation::operator=(TLSCalc2DOffsetObservation& right) 
{	
	// not implemented
}
*/

// Destructor
TLSCalc2DOffsetObservation::~TLSCalc2DOffsetObservation() {

	if (fStPoint!=0) delete fStPoint;
	if (fEnd1Point!=0) delete fEnd1Point;
	if (fEnd2Point!=0) delete fEnd2Point;
}


//////////////////////////////////////////////////////////////////////////
// PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////

// Returns the observed vertical offset
TLength TLSCalc2DOffsetObservation::getVOffset() const {

	return fVOffset;
}

// Returns the sigma a priori of the observed vertical offset
TLength	TLSCalc2DOffsetObservation::getVOffSigmaAPriori() const {

	return fVOffSigmaAPriori;
}

// Returns the observed horizontal offset
TLength TLSCalc2DOffsetObservation::getHOffset() const {

	return fHOffset;
}

// Returns the sigma a priori of the observed horizontal offset 
TLength	TLSCalc2DOffsetObservation::getHOffSigmaAPriori() const {

	return fHOffSigmaAPriori;
}


// Returns a pointer to  the stationned point: Attention, le type devra être remplacé par TLSCalcPosVectParam
TSpatialPoint* TLSCalc2DOffsetObservation::getStPoint() const {

	return fStPoint;
}

// Returns a pointer to  the wire's end 1 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalc2DOffsetObservation::getEnd1Point() const {
	
	return fEnd1Point;
}

// Returns a pointer to  the wire's end 2 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalc2DOffsetObservation::getEnd2Point() const {
	
	return fEnd2Point;
}

/////////////////////////////////////////////////////////////////////////
// PUBLIC & PROTECTED SETTING METHODS
/////////////////////////////////////////////////////////////////////////

// Sets the sigma a posteriori for the vertical offset
void TLSCalc2DOffsetObservation::setVOffSigmaAPosteriori(TLength vsap) {

	fVOffSigmaAPosteriori = vsap;
}

// Sets the sigma a posteriori for the horizontal offset
void TLSCalc2DOffsetObservation::setHOffSigmaAPosteriori(TLength hsap) {

	fVOffSigmaAPosteriori = hsap;
}

// Sets the residual of the computed vertical offset measurement 
void TLSCalc2DOffsetObservation::setVResidual(TLength vres) {

	fVOffResidue = vres;
}

// Sets the residual of the computed horizontal offset measurement 
void TLSCalc2DOffsetObservation::setHResidual(TLength hres) {

	fHOffResidue = hres;
}

// Sets the simulated value of the observed vertical offset 
void TLSCalc2DOffsetObservation::setSimulatedVOffset(TLength sObsVO) {
	// the observed vertical offset is now equal to a simulated value
	fVOffset = sObsVO; 
}

// Sets the simulated value of the observed horizontal offset 
void TLSCalc2DOffsetObservation::setSimulatedHOffset(TLength sObsHO) {
	// the observed horizontal offset is now equal to a simulated value
	fHOffset = sObsHO; 
}

// Sets the estimated vertical offset (observed + residual) 
void TLSCalc2DOffsetObservation::setEstimatedVOffset() {

	fVEstimatedOff = fVOffset - fVOffResidue;
}

// Sets the estimated horizontal offset (observed + residual) 
void TLSCalc2DOffsetObservation::setEstimatedHOffset() {

	fHEstimatedOff = fHOffset - fHOffResidue;
}

// Sets observations and equations indices
UEOIndices	TLSCalc2DOffsetObservation::setIndices(UEOIndices ueoi) {

	fIndices.EIndex = ueoi.EIndex;
	ueoi.EIndex++;
	fIndices.OIndex = ueoi.OIndex;
	ueoi.OIndex++;
	return ueoi;
}



/*
// constructor taking the initial values (spatial measurements) as arguments
TLSCalc2DOffsetObservation::TLSCalc2DOffsetObservation(TLength voff,TLength svoff,
		TLength hoff,TLength shoff,TSpatialPoint* st,TSpatialPoint* ex1,
		TSpatialPoint* ex2):
fVOffset(voff),fVOffSigmaAPriori(svoff),fHOffset(hoff),fHOffSigmaAPriori(shoff),
fStPoint(st),fEnd1Point(ex1),fEnd2Point(ex2) {
	// sets all the "calculation" attributes to 0
	fVEstimatedOff = TLength(LITERAL(0.0));
	fHEstimatedOff = TLength(LITERAL(0.0));
	fVOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fHOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fVOffResidue = TLength(LITERAL(0.0));
	fHOffResidue = TLength(LITERAL(0.0));
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;
}
*/



