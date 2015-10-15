#include "TALSCalcStat.h"

#include	<ctime>

#include <iostream>
using namespace std;

#include "TVNumericValue.h"
#include "QuantileFunctions.h"

//////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////

// Default constructor
TALSCalcStat::TALSCalcStat() : fZ(LITERAL(0.0)), fW(LITERAL(0.0)), fT(LITERAL(0.0)), fDelty(LITERAL(0.0)),
fNablaValue(LITERAL(0.0)), fGValue(LITERAL(0.0)), fError(""), fAreDetermined(false), fGToCompute(false),
fDeltaComputed(false), fSimValueIsWritten(false),fWToCompute(true) {
	
	fIndices.UIndex = 0;
	fIndices.EIndex = 0;
	fIndices.OIndex = 0;
}

// Copy constructor
TALSCalcStat::TALSCalcStat(const TALSCalcStat& source) {
	
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
TALSCalcStat::~TALSCalcStat() {
}

//////////////////////////////////////
// GetIndices
//////////////////////////////////////

// Returns the equations index 
MatrixIndex		TALSCalcStat::getEqnIndex() const {

	return fIndices.EIndex;
}

// Returns the observations index 
MatrixIndex		TALSCalcStat::getObsIndex() const {

	return fIndices.OIndex;
}

////////////////////////////////////////
// sets wToCompute to false
///////////////////////////////////////
void TALSCalcStat::wIsNotToCompute() {

	fWToCompute = false;
	return;
}

/////////////////////////////////////////
// Computes error detection parameters
////////////////////////////////////////
void  TALSCalcStat::computeErrorDetectionParam(TReal alpha, TReal beta, TReal res, TReal varRes, TReal sAPriori, TReal s0) {
	// reset the variables, necessary if the object is reused (e.g. in simulations)
	fZ = fW = fT = fDelty = fNablaValue = fGValue = LITERAL(0.0);
	fAreDetermined = fGToCompute = fDeltaComputed = fSimValueIsWritten = false;

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
			
			// determination of alpha's percentile (upper tail)
            TReal wmax = deviates_normal_upper_tail(alpha/2);
            fDeltaComputed = true;

			
			if ((fabsq(fW) >= wmax) && fWToCompute)
			{// test if g needs to be calculated
				// computes g
				fGValue = -(res/fZ);
				fGToCompute = true;
			}

			// determination of beta's percentile (upper tail)
            TReal d = deviates_normal_upper_tail(beta);
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
UEOIndices	TALSCalcStat::setEOIndices(UEOIndices ueoi) {

	fIndices.EIndex = ueoi.EIndex ++;
	fIndices.OIndex = ueoi.OIndex ++;
	return ueoi;
}


/*//overall
double    TALSCalcStat::computeOverallNetworkReliability(TLGCData* projData)    {

    double    F(LITERAL(0.0));
    TReal    FValue(LITERAL(0.0));

	/*
    LSHorAngIter    iterHa    =    beginLSHorAng();
    while    (iterHa!=endLSHorAng())    {
    if    (iterHa->paramsCanBeDetermined())    {
    FValue    +=    powq(iterHa->getT(),2)    -    1;
    iterHa++;
    }
    else    {
    F.setStatus(TVNumericValue::kNull);
    iterHa    =    endLSHorAng();
    }
    }
	*/

/*
	//Tteration through the tree nodes
	for (TDataTreeIterator itTree = projData->tree.begin(); itTree != projData->tree.end(); itTree++){		
		//In every node iterate through the Total station measurements (TSTN)
		for(auto itTSTN(itTree.node->data->measurements.fTSTN.begin()); itTSTN != itTree.node->data->measurements.fTSTN.end(); ++itTSTN){
			//In every TSTN iterate through ROMS
			for(auto itROM(itTSTN->roms.begin()); itROM != itTSTN->roms.end(); ++itROM){
				//In every ROM iterate through the specific measurements
				for(auto itPLR3D(itROM->measPLR3D.begin()); itPLR3D != itROM->measPLR3D.end(); ++itPLR3D){
					//if    (itPLR3D->paramsCanBeDetermined())    {
					//	 FValue    +=    powq(itPLR3D->getT(),2)    -    1;
					//	 itPLR3D++;
					//	 }
					//	 else    {
					//	 F.setStatus(TVNumericValue::kNull);
					//	 itPLR3D    =    endLSHorAng();
					//	 }
				}


				for(auto itDIR3D(itROM->measDIR3D.begin()); itDIR3D != itROM->measDIR3D.end(); ++itDIR3D)
				{}

				for(auto itANGL(itROM->measANGL.begin()); itANGL != itROM->measANGL.end(); ++itANGL)
				{}

				for(auto itZEND(itROM->measZEND.begin()); itZEND != itROM->measZEND.end(); ++itZEND)
				{}


				for(auto itDIST(itROM->measDIST.begin()); itDIST != itROM->measDIST.end(); ++itDIST)
				{}

				for(auto itDHOR(itROM->measDHOR.begin()); itDHOR != itROM->measDHOR.end(); ++itDHOR)
				{}


				for(auto itECTH(itROM->measECTH.begin()); itECTH != itROM->measECTH.end(); ++itECTH)
				{}
			}
		}


		//In every node iterate through camera's (CAM) measurements
		for(auto itCAM(itTree.node->data->measurements.fCAM.begin()); itCAM != itTree.node->data->measurements.fCAM.end(); ++itCAM){
			for(auto itPLR3D(itCAM->measPLR3D.begin()); itPLR3D != itCAM->measPLR3D.end(); ++itPLR3D)
					

			for(auto itDIR3D(itCAM->measDIR3D.begin()); itDIR3D != itCAM->measDIR3D.end(); ++itDIR3D)
					
		}

		//In every node iterate through the EDM's measurements
		for(auto itEDM(itTree.node->data->measurements.fEDM.begin()); itEDM != itTree.node->data->measurements.fEDM.end(); ++itEDM){
			//Iterate through DPST measurements
			for(auto itDPST(itEDM->measDSPT.begin()); itDPST != itEDM->measDSPT.end(); ++itDPST){	
				//To do
			}
		}

		//In every node iterate through the LEVEL's measurements
		for(auto itLEVEL(itTree.node->data->measurements.fLEVEL.begin()); itLEVEL != itTree.node->data->measurements.fLEVEL.end(); ++itLEVEL)
			
	}

    return    F;
}*/