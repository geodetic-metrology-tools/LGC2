//TLSCalcWTWObservation.cpp

#include "TLSCalcWTWObservation.h"
#include "TWireToWireDistMeas.h"

////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////

// Default constructor
TLSCalcWTWObservation::TLSCalcWTWObservation() {
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
	fWire1End1Point = 0;
	fWire1End2Point = 0;
	fWire2End1Point = 0;
	fWire2End2Point = 0;
	fPosition1 = 0;
	fPosition2 = 0;
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;	
}

// constructor taking a wire to wire measurement and pointers to lscalcparameters to initialize the lscalcwtw observation's attributes
TLSCalcWTWObservation::TLSCalcWTWObservation(TWireToWireDistMeas& wtwmeas,
											 TSpatialPoint* w1ex1,TSpatialPoint* w1ex2,TSpatialPoint* w2ex1,TSpatialPoint* w2ex2,
											 TLength* pos1,TLength* pos2):
fWire1End1Point(w1ex1),fWire1End2Point(w1ex2),fWire2End1Point(w2ex1),fWire2End2Point(w2ex2),
fPosition1(pos1),fPosition2(pos2) {
	fVOffset = wtwmeas.getVDist();
	fVOffSigmaAPriori = wtwmeas.getVSigma();
	fHOffset = wtwmeas.getHDist();
	fHOffSigmaAPriori = wtwmeas.getHSigma();
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
TLSCalcWTWObservation::TLSCalcWTWObservation(const TLSCalcWTWObservation& source) 
{
	// not implemented
}

// copy assignment operator
TLSCalcWTWObservation& TLSCalcWTWObservation::operator=(TLSCalcWTWObservation& right) 
{	
	// not implemented
}
*/

// Destructor
TLSCalcWTWObservation::~TLSCalcWTWObservation() {
	
	if (fWire1End1Point!=0) delete fWire1End1Point;
	if (fWire1End2Point!=0) delete fWire1End2Point;
	if (fWire2End1Point!=0) delete fWire2End1Point;
	if (fWire2End2Point!=0) delete fWire2End2Point;
	if (fPosition1!=0) delete fPosition1;
	if (fPosition2!=0) delete fPosition2;
}


//////////////////////////////////////////////////////////////////////////
// PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////

// Returns the observed vertical offset
TLength TLSCalcWTWObservation::getVOffset() const {

	return fVOffset;
}

// Returns the sigma a priori of the observed vertical offset
TLength	TLSCalcWTWObservation::getVOffSigmaAPriori() const {

	return fVOffSigmaAPriori;
}

// Returns the observed horizontal offset
TLength TLSCalcWTWObservation::getHOffset() const {

	return fHOffset;
}

// Returns the sigma a priori of the observed horizontal offset 
TLength	TLSCalcWTWObservation::getHOffSigmaAPriori() const {

	return fHOffSigmaAPriori;
}

// Returns a pointer to the wire 1's end 1 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalcWTWObservation::getWire1End1Point() const {
	
	return fWire1End1Point;
}

// Returns a pointer to the wire 1's end 2 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalcWTWObservation::getWire1End2Point() const {
	
	return fWire1End2Point;
}

// Returns a pointer to the wire 2's end 1 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalcWTWObservation::getWire2End1Point() const {
	
	return fWire2End1Point;
}

// Returns a pointer to the wire 2's end 2 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalcWTWObservation::getWire2End2Point() const {
	
	return fWire2End2Point;
}

// Returns a pointer to the position on wire 1 :Attention, le type devra être remplacé par TLSCalcLengthParam 
TLength* TLSCalcWTWObservation::getPosOnWire1() const {

	return fPosition1;
}

// Returns a pointer to the position on wire 2 :Attention, le type devra être remplacé par TLSCalcLengthParam 
TLength* TLSCalcWTWObservation::getPosOnWire2() const {

	return fPosition2;
}

/////////////////////////////////////////////////////////////////////////
// PUBLIC & PROTECTED SETTING METHODS
/////////////////////////////////////////////////////////////////////////

// Sets the sigma a posteriori for the vertical offset
void TLSCalcWTWObservation::setVOffSigmaAPosteriori(TLength vsap) {

	fVOffSigmaAPosteriori = vsap;
}

// Sets the sigma a posteriori for the horizontal offset
void TLSCalcWTWObservation::setHOffSigmaAPosteriori(TLength hsap) {

	fVOffSigmaAPosteriori = hsap;
}

// Sets the residual of the computed vertical offset measurement 
void TLSCalcWTWObservation::setVResidual(TLength vres) {

	fVOffResidue = vres;
	setEstimatedVOffset();
}

// Sets the residual of the computed horizontal offset measurement 
void TLSCalcWTWObservation::setHResidual(TLength hres) {

	fHOffResidue = hres;
	setEstimatedHOffset();
}

// Sets the simulated value of the observed vertical offset 
void TLSCalcWTWObservation::setSimulatedVOffset(TLength sObsVO) {
	// the observed vertical offset is now equal to a simulated value
	fVOffset = sObsVO; 
}

// Sets the simulated value of the observed horizontal offset 
void TLSCalcWTWObservation::setSimulatedHOffset(TLength sObsHO) {
	// the observed horizontal offset is now equal to a simulated value
	fHOffset = sObsHO; 
}

// Sets the estimated vertical offset (observed + residual) 
void TLSCalcWTWObservation::setEstimatedVOffset() {

	fVEstimatedOff = fVOffset - fVOffResidue;
}

// Sets the estimated horizontal offset (observed + residual) 
void TLSCalcWTWObservation::setEstimatedHOffset() {

	fHEstimatedOff = fHOffset - fHOffResidue;
}

// Sets observations and equations indices
UEOIndices	TLSCalcWTWObservation::setIndices(UEOIndices ueoi) {

	fIndices.EIndex = ueoi.EIndex;
	ueoi.EIndex++;
	fIndices.OIndex = ueoi.OIndex;
	ueoi.OIndex++;
	return ueoi;
}

/*
// constructor taking the initial values (spatial measurements) as arguments
TLSCalcWTWObservation::TLSCalcWTWObservation(TLength voff,TLength svoff,
		TLength hoff,TLength shoff,TSpatialPoint* w1ex1,TSpatialPoint* w1ex2,
		TSpatialPoint* w2ex1,TSpatialPoint* w2ex2,TLength* pos1,TLength* pos2):
fVOffset(voff),fVOffSigmaAPriori(svoff),fHOffset(hoff),fHOffSigmaAPriori(shoff) {
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



