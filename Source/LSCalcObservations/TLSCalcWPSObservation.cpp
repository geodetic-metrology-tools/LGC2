//TLSCalcWPSObservation.cpp

#include "TLSCalcWPSObservation.h"
#include "TWPSMeasurement.h"

////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
////////////////////////////////////////////////////////

// Default constructor
TLSCalcWPSObservation::TLSCalcWPSObservation() {
	//sets attributes to 0
	fVOffset = TLength(LITERAL(0.0));
	fVOffSigmaAPriori = TLength(LITERAL(0.0));
	fTOffset = TLength(LITERAL(0.0));
	fTOffSigmaAPriori = TLength(LITERAL(0.0));
	fVEstimatedOff = TLength(LITERAL(0.0));
	fTEstimatedOff = TLength(LITERAL(0.0));
	fVOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fTOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fVOffResidue = TLength(LITERAL(0.0));
	fTOffResidue = TLength(LITERAL(0.0));
	fEnd1Point = 0;
	fEnd2Point = 0;
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;
}

// constructor taking a wps measurement and pointers to lscalcparameters  to initialize the lscalcwps observation's attributes
TLSCalcWPSObservation::TLSCalcWPSObservation(TWPSMeasurement& wpsmeas,
											 TSpatialPoint* ex1,TSpatialPoint* ex2):
fEnd1Point(ex1),fEnd2Point(ex2){
	fVOffset = wpsmeas.getVertOffset();	
	fVOffSigmaAPriori = wpsmeas.getVertSigma() ;	
	fTOffset = wpsmeas.getTransOffset();	
	fTOffSigmaAPriori = wpsmeas.getTransSigma();
	fVEstimatedOff = fVOffset;
	fTEstimatedOff = fTOffset;
	fVOffSigmaAPosteriori = fVOffSigmaAPriori;
	fTOffSigmaAPosteriori = fTOffSigmaAPriori;
	fVOffResidue = TLength(LITERAL(0.0));
	fTOffResidue = TLength(LITERAL(0.0));
	
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;
}

/*
// copy constructor
TLSCalcWPSObservation::TLSCalcWPSObservation(const TLSCalcWPSObservation& source) 
{
	// not implemented
}

// copy assignment operator
TLSCalcWPSObservation& TLSCalcWPSObservation::operator=(TLSCalcWPSObservation& right) 
{	
	// not implemented
}
*/

// Destructor
TLSCalcWPSObservation::~TLSCalcWPSObservation() {

	if (fEnd1Point!=0) delete fEnd1Point;
	if (fEnd2Point!=0) delete fEnd2Point;
}


//////////////////////////////////////////////////////////////////////////
// PUBLIC ACCESS METHODS
//////////////////////////////////////////////////////////////////////////

// Returns the observed vertical offset
TLength TLSCalcWPSObservation::getVOffset() const {

	return fVOffset;
}

// Returns the sigma a priori of the observed vertical offset
TLength	TLSCalcWPSObservation::getVOffSigmaAPriori() const {

	return fVOffSigmaAPriori;
}

// Returns the observed transversal offset
TLength TLSCalcWPSObservation::getTOffset() const {

	return fTOffset;
}

// Returns the sigma a priori of the observed transversal offset 
TLength	TLSCalcWPSObservation::getTOffSigmaAPriori() const {

	return fTOffSigmaAPriori;
}

// Returns a pointer to the wire's end 1 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalcWPSObservation::getEnd1Point() const {
	
	return fEnd1Point;
}

// Returns a pointer to the wire's end 2 point: Attention, le type devra être remplacé par TLSCalcPosVectParam 
TSpatialPoint*	TLSCalcWPSObservation::getEnd2Point() const {
	
	return fEnd2Point;
}

/////////////////////////////////////////////////////////////////////////
// PUBLIC & PROTECTED SETTING METHODS
/////////////////////////////////////////////////////////////////////////

// Sets the sigma a posteriori for the vertical offset
void TLSCalcWPSObservation::setVOffSigmaAPosteriori(TLength vsap) {

	fVOffSigmaAPosteriori = vsap;
}

// Sets the sigma a posteriori for the transversal offset
void TLSCalcWPSObservation::setTOffSigmaAPosteriori(TLength tsap) {

	fVOffSigmaAPosteriori = tsap;
}

// Sets the residual of the computed vertical offset measurement 
void TLSCalcWPSObservation::setVResidual(TLength vres) {

	fVOffResidue = vres;
	setEstimatedVOffset();
}

// Sets the residual of the computed transversal offset measurement 
void TLSCalcWPSObservation::setTResidual(TLength tres) {

	fTOffResidue = tres;
	setEstimatedTOffset();
}

// Sets the simulated value of the observed vertical offset 
void TLSCalcWPSObservation::setSimulatedVOffset(TLength sObsVO) {
	// the observed vertical offset is now equal to a simulated value
	fVOffset = sObsVO; 
}

// Sets the simulated value of the observed transversal offset 
void TLSCalcWPSObservation::setSimulatedTOffset(TLength sObsTO) {
	// the observed horizontal offset is now equal to a simulated value
	fTOffset = sObsTO; 
}

// Sets the estimated vertical offset (observed + residual) 
void TLSCalcWPSObservation::setEstimatedVOffset() {

	fVEstimatedOff = fVOffset - fVOffResidue;
}

// Sets the estimated transversal offset (observed + residual) 
void TLSCalcWPSObservation::setEstimatedTOffset() {

	fTEstimatedOff = fTOffset - fTOffResidue;
}

// Sets observations and equations indices
UEOIndices	TLSCalcWPSObservation::setIndices(UEOIndices ueoi) {

	fIndices.EIndex = ueoi.EIndex;
	ueoi.EIndex++;
	fIndices.OIndex = ueoi.OIndex;
	ueoi.OIndex++;
	return ueoi;
}


/*
// constructor taking the initial values (spatial measurements) as arguments
TLSCalcWPSObservation::TLSCalcWPSObservation(TLength voff,TLength svoff,
		TLength toff,TLength stoff,TSpatialPoint* ex1,TSpatialPoint* ex2):
fVOffset(voff),fVOffSigmaAPriori(svoff),fTOffset(toff),fTOffSigmaAPriori(stoff),
fEnd1Point(ex1),fEnd2Point(ex2) {
	// sets all the "calculation" attributes to 0
	fVEstimatedOff = TLength(LITERAL(0.0));
	fTEstimatedOff = TLength(LITERAL(0.0));
	fVOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fTOffSigmaAPosteriori = TLength(LITERAL(0.0));
	fVOffResidue = TLength(LITERAL(0.0));
	fTOffResidue = TLength(LITERAL(0.0));
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;
}
*/


