// TALSCalcObservation.cpp

#include "TALSCalcObservation.h"

#include	<ctime>
//#include	<cmath>
//#include	<nag.h>
//#include	<nagg01.h>

#include <iostream>
using namespace std;


#include "TVNumericValue.h"
#include <QuantileFunctions.h>
//////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////

// Default constructor
TALSCalcObservation::TALSCalcObservation() : fZ(LITERAL(0.0)), fW(LITERAL(0.0)), fT(LITERAL(0.0)), fDelty(LITERAL(0.0)),
fNablaValue(LITERAL(0.0)), fGValue(LITERAL(0.0)), fError(""), fAreDetermined(false), fGToCompute(false),
fDeltaComputed(false), fSimValueIsWritten(false),fWToCompute(true) {
	
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;
}

// Copy constructor
TALSCalcObservation::TALSCalcObservation(const TALSCalcObservation& source) {
	
	fAreDetermined = source.fAreDetermined;
	fGToCompute = source.fGToCompute;
	fDeltaComputed = source.fDeltaComputed;
	fError = source.fError;
	fZ = source.fZ;
	fW = source.fW;
	fT = source.fT;
	fDelty = source.fDelty;
	fNablaValue = source.fNablaValue;
	fGValue = source.fGValue;
	fIndices = source.fIndices;
	fSimValueIsWritten = source.fSimValueIsWritten;
	fWToCompute = source.fWToCompute;

}

// Destructor
TALSCalcObservation::~TALSCalcObservation() {
}

//////////////////////////////////////
// GetIndices
//////////////////////////////////////

// Returns the equations index 
MatrixIndex		TALSCalcObservation::getEqnIndex() const {

	return fIndices.EIndex;
}

// Returns the observations index 
MatrixIndex		TALSCalcObservation::getObsIndex() const {

	return fIndices.OIndex;
}

////////////////////////////////////////
// sets wToCompute to false
///////////////////////////////////////
void TALSCalcObservation::wIsNotToCompute() {

	fWToCompute = false;
	return;
}

/////////////////////////////////////////
// Computes error detection parameters
////////////////////////////////////////
void  TALSCalcObservation::computeErrorDetectionParam(TReal alpha, TReal beta, TReal res, TReal varRes, TReal sAPriori, TReal s0) {

	if (sAPriori!=LITERAL(0.0) && varRes!=LITERAL(0.0)) {
		
		fAreDetermined = true;
		
		// compute z		
		TReal cof = varRes / powq(s0,2);
		fZ = cof * (1 / powq(sAPriori,2));
		// check on z consistency
		if ((fZ < LITERAL(1.0000001))&&(fZ>LITERAL(1.0))) 
		{
			fZ = LITERAL(1.0);
		}

		
		if ((fZ > LITERAL(1.0))|| (fZ < LITERAL(0.0005)))
		{
			fAreDetermined = false;
		}
		else
		{

			if (fWToCompute)
			{// computes w
				fW = res / (sqrtq(varRes));
			
			}
						
			// computes T
			fT = sqrtq(1/fZ);

			// computes delta
			// nagc error message, to be set if there is a problem. 
			// It should not be printed on the screen

			//static NagError fail;
			//fail.print = false;

			// determination of alpha's percentile (upper tail)
			//TReal wmax = nag_deviates_normal(Nag_UpperTail,alpha/LITERAL(2.0),&fail);
            TReal wmax = deviates_normal_upper_tail(alpha/2);
			//if (fail.code == NE_NOERROR) 
			//{
				//fDeltaComputed = true;
			//}
            fDeltaComputed = true;

			if ((fabsq(fW) >= wmax) && fWToCompute)
			{// test if g needs to be calculated
				// computes g
				fGValue = -(res/fZ);
				fGToCompute = true;
			}

			// determination of beta's percentile (upper tail)
			//TReal d = nag_deviates_normal(Nag_UpperTail,beta,&fail);
            TReal d = deviates_normal_upper_tail(beta);
			//if (fail.code == NE_NOERROR)
			//{
				//fDeltaComputed = true;
			//}
            fDeltaComputed = true;


			if (fDeltaComputed)
			{
				TReal delta(wmax+d);
				//compute nabla
				fNablaValue = (delta * (sqrtq(varRes)/fZ));///powq(s0,2);
				if (fNablaValue>LITERAL(1000.0)*sAPriori)
					fAreDetermined = false;
				// computes DELTY
				fDelty = delta * sqrtq(powq(fT,2) - LITERAL(1.0));
			}
		}
	}

	return;

}	 


//////////////////////////////////////////
// Set Indices
//////////////////////////////////////////

// Sets observations and equations indices
UEOIndices	TALSCalcObservation::setEOIndices(UEOIndices ueoi) {

	fIndices.EIndex = ueoi.EIndex ++;
	fIndices.OIndex = ueoi.OIndex ++;
	return ueoi;
}

