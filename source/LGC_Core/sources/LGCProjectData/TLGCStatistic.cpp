#include <memory>
#include <iostream>

#include <TLGCStatistic.h>
#include "QuantileFunctions.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"
#include "TSparseMatrix.h"
#include <TConstants.h>


#ifndef isnotanumber
#define isnotanumber(x) ((x)!=(x))
#endif

//////////////////////////
// no argument constructor
//////////////////////////
TLGCStatistic::TLGCStatistic():fError(""),
fWToCompute(true),
fAreDetermined(nullptr),
fGToCompute(nullptr),
fDeltaComputed(nullptr),
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

	fAreDetermined.reset(new TVector(nbObs));
	fGToCompute.reset(new TVector(nbObs));
	fDeltaComputed.reset(new TVector(nbObs));
}

void TLGCStatistic::clearVectors(){
	fZ.reset(nullptr);
	fW.reset(nullptr);
	fT.reset(nullptr);
	fDelty.reset(nullptr);
	fNablaValue.reset(nullptr);
	fGValue.reset(nullptr);


	fAreDetermined.reset(nullptr);
	fGToCompute.reset(nullptr);
	fDeltaComputed.reset(nullptr);
}

void TLGCStatistic::calcReliabilityVector(TReal alpha, TReal beta, const TLSInputMatrices* im, TLSResultsMatrices* rm, bool hasPdor, bool combinedcase)
{
	int nbObs = im->getNbrObservations();
	int nbEq = im->getNbrEquations();
	// double s02 = rm->getSigmaZero2();
	TReal varAPriori = 1;
	double varRes = 0;
	double res = 0;

	// compute z
	TSparseMatrix Z(nbObs,nbEq);
	if (combinedcase)
		Z = -1.0**(rm->getResCovarMtrx()) * *(im->getWeightMtrx()) *(im->getSecondDgnMtrx()->transpose());
	else
		Z = *(rm->getResCovarMtrx()) * *(im->getWeightMtrx()); // *(im->getSecondDgnMtrx()->transpose());

	//V = Cw => C=Qvv* P*BT
	//rm->setIntermediateMatrix2(Z);
	//rm->saveMatricesToFile(0);
	//im->saveMatricesToFile(0);

	//loop for each unknowns
	int i = 0;
	while (i<nbObs)
	{		
		// reset the variables, necessary if the object is reused (e.g. in simulations)
		(*fAreDetermined)(i) = (*fGToCompute)(i) = (*fDeltaComputed)(i) = false;

		varAPriori = im->getWeightInvMtrx()->coeff(i,i);
		varRes = rm->getResCovarMtrxElmt(i,i);
		res = rm->getResidualsVctrElmt(i);

		if (varAPriori != LITERAL(0.0) && varRes != LITERAL(0.0)) {
			
			(*fAreDetermined)(i) = true;
			
			// compute z		
			(*fZ)(i) = Z.coeff(i, i);
			//TReal qwe = Z.coeff(i, i);
			// check on z consistency
			if ((fZ->coeff(i) < LITERAL(1.0000001))&&(fZ->coeff(i)>LITERAL(1.0))) 
			{
				(*fZ)(i) = LITERAL(1.0);
			}
			
			if ((fZ->coeff(i) > LITERAL(1.0))|| (fZ->coeff(i) < LITERAL(0.0005)))
			{
				(*fAreDetermined)(i) = false;
				(*fZ)(i) = NO_VALf;
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
		        (*fDeltaComputed)(i) = true;

				if ((fabsq(fW->coeff(i)) >= wmax) && fWToCompute)
				{// test if g needs to be calculated
					// computes g
					(*fGValue)(i) = -(res/fZ->coeff(i));
					(*fGToCompute)(i) = true;
				}

				// determination of beta's percentile (upper tail)
		        TReal d = deviates_normal_upper_tail(beta);
				TReal delta(wmax+d);
				//compute nabla
				(*fNablaValue)(i) = (delta * (sqrtq(varRes)/fZ->coeff(i)));///powq(s0,2);

				// computes DELTY
				(*fDelty)(i) = delta * sqrtq(powq(fT->coeff(i),2) - LITERAL(1.0));
			}
		}
		i++;
	}

	//last observation is for the PDOR
	if (hasPdor)
		calcOverall(nbObs-1);
	else
		calcOverall(nbObs);

	return;
}


void    TLGCStatistic::calcOverall(int nbObs)
{
	fOverall = 0.0;
	int i = 0;
	while (i<nbObs)
	{
		// double k = fZ->coeff(i);
		if (fAreDetermined->coeff(i))
			fOverall += powq(1/fZ->coeff(i),2)-1;
		else
		{
			fOverall = NO_VALf;
			break;
		}
		i++;
	}
	if (!isnotanumber(fOverall))
		fOverall /= (nbObs - 1);
	return;
}