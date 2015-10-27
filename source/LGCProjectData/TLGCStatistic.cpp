#include <memory>
#include <iostream>

#include "TLGCStatistic.h"
#include "TVNumericValue.h"
#include "QuantileFunctions.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"

//////////////////////////
// no argument constructor
//////////////////////////
TLGCStatistic::TLGCStatistic():fError(""), fAreDetermined(false), fGToCompute(false),
fDeltaComputed(false),fWToCompute(true),
fZ(nullptr),
fW(nullptr),
fT(nullptr),
fDelty(nullptr),
fNablaValue(nullptr),
fGValue(nullptr)
{ }

/////////////
// destructor
/////////////
TLGCStatistic::~TLGCStatistic(){
}


void TLGCStatistic::initialiseStatVector(const TLSInputMatrices* im)
{
	int nbObs = im->getNbrObservations(); 
	fZ.reset(new TVector(nbObs));
	fW.reset(new TVector(nbObs));
	fT.reset(new TVector(nbObs));
	fDelty.reset(new TVector(nbObs));
	fNablaValue.reset(new TVector(nbObs));
	fGValue.reset(new TVector(nbObs));
}

void TLGCStatistic::clearVectors(){
	fZ.reset(nullptr);
	fW.reset(nullptr);
	fT.reset(nullptr);
	fDelty.reset(nullptr);
	fNablaValue.reset(nullptr);
	fGValue.reset(nullptr);
}

void TLGCStatistic::calcReliabilityVector(TReal alpha, TReal beta, const TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	int nbObs = im->getNbrObservations() /*+ im->getNbrConstraints()*/; 


	double s02 = rm->getSigmaZero2();
	TReal sigmaAPriori = 1;
	double varRes = 0;
	double res = 0;

	// reset the variables, necessary if the object is reused (e.g. in simulations)
	fAreDetermined = fGToCompute = fDeltaComputed  = false;


	//loop for each unknowns
	int i = 0;
	while (i<nbObs)
	{		
		sigmaAPriori = im->getWeightInvMtrx()->coeff(i,i);
		varRes = rm->getResCovarMtrxElmt(i,i);
		res = rm->getResidualsVctrElmt(i);

		if (sigmaAPriori!=LITERAL(0.0) && varRes!=LITERAL(0.0)) {
			
			fAreDetermined = true;
			
			// compute z		
			TReal cof = varRes/s02;
			(*fZ)(i) = cof * (1 / sigmaAPriori);
			// check on z consistency
			if ((fZ->coeff(i) < LITERAL(1.0000001))&&(fZ->coeff(i)>LITERAL(1.0))) 
			{
				(*fZ)(i) = LITERAL(1.0);
			}
			
			if ((fZ->coeff(i) > LITERAL(1.0))|| (fZ->coeff(i) < LITERAL(0.0005)))
			{
				fAreDetermined = false;
			}
			else
			{

				if (fWToCompute)
				{// computes w
					(*fW)(i) = res / (sqrtq(varRes));
				}
							
				// computes T
				(*fT)(i) = sqrtq(1/fZ->coeff(i));			
				
				// determination of alpha's percentile (upper tail)
		        TReal wmax = deviates_normal_upper_tail(alpha/2);
		        fDeltaComputed = true;

				if ((fabsq(fW->coeff(i)) >= wmax) && fWToCompute)
				{// test if g needs to be calculated
					// computes g
					(*fGValue)(i) = -(res/fZ->coeff(i));
					fGToCompute = true;
				}

				// determination of beta's percentile (upper tail)
		        TReal d = deviates_normal_upper_tail(beta);
		        fDeltaComputed = true;


				if (fDeltaComputed)
				{
					TReal delta(wmax+d);
					//compute nabla
					(*fNablaValue)(i) = (delta * (sqrtq(varRes)/fZ->coeff(i)));///powq(s0,2);

					if (fNablaValue->coeff(i)>LITERAL(1000.0)*sigmaAPriori)
						fAreDetermined = false;
					// computes DELTY
					(*fDelty)(i) = delta * sqrtq(powq(fT->coeff(i),2) - LITERAL(1.0));
				}
			}
		}
		i++;
	}

	return;
}


void    TLGCStatistic::calcOverall(TVector* fZ)
{
	int nbUnk = (int)fZ->size();
	int i = 0;
	while (i<nbUnk)
	{
		fOverall += powq(fZ->coeff(i),2)-1;
		i++;
	}
	fOverall /= nbUnk;
	return;
}