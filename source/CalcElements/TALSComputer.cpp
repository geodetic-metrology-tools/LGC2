#include "lsalgo/TALSComputer.h"
#include "TVNumericValue.h"
#include "lsalgo/QuantileFunctions.h"
#include <memory>
#include <iostream>

//////////////////////////
// no argument constructor
//////////////////////////
TALSComputer::TALSComputer():fError(""), fAreDetermined(false), fGToCompute(false),
fDeltaComputed(false),fWToCompute(true)
{
	fS0APosterioriVariances = true;

	fZ = nullptr;
	fW = nullptr;
	fT = nullptr;
	fDelty = nullptr;
	fNablaValue = nullptr;
	fGValue = nullptr;
}

// Copy constructor
TALSComputer::TALSComputer(const TALSComputer& source) {
	
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
	fWToCompute = source.fWToCompute;
	
}

/////////////
// destructor
/////////////
TALSComputer::~TALSComputer(){
	clearVectors();
}

void TALSComputer::clearVectors(){
	delete fZ;
	delete fW;
	delete fT;
	delete fDelty;
	delete fNablaValue;
	delete fGValue ;

	fZ = nullptr;
	fW = nullptr;
	fT = nullptr;
	fDelty = nullptr;
	fNablaValue = nullptr;
	fGValue = nullptr;
}

void TALSComputer::initialiseStatVector(const TLSInputMatrices* im)
{
	int nbObs = im->getNbrObservations(); 
	fZ = new TVector(nbObs);
	fW = new TVector(nbObs);
	fT = new TVector(nbObs);
	fDelty = new TVector(nbObs);
	fNablaValue = new TVector(nbObs);
	fGValue = new TVector(nbObs);
}

void TALSComputer::calcStatisticVector(TReal alpha, TReal beta, const TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	int nbObs = im->getNbrObservations(); 
	double s02 = rm->getSigmaZero2();
	TReal sigmaAPriori = 0;
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
			TReal cof = varRes;// / s02;
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
		//std::cout<<"Z: "<<fZ->coeff(i)<<"  W: "<<fW->coeff(i)<<"  T: "<<fT->coeff(i)<<"  G: "<<fGValue->coeff(i)<<"  Nabla: "<<fNablaValue->coeff(i)<<"   Delty: "<<fDelty->coeff(i)<<std::endl;
		i++;
	}

	return;
}



limits	TALSComputer::calcSigmaZeroLimits(const int nbObs, const int nbUnk, const double sigmaZero2)
{
	// computes and saves limits of interval for test chi of S0 validity
	struct limits resultat;
	resultat.s0PostUpLimit = LITERAL(0.0);
	resultat.s0PostLoLimit = LITERAL(0.0);
	double d = LITERAL(0.0);
	
	d = nbObs - nbUnk;
	
	if(d>0)
	{
        double chiUp = deviates_chi_sq(0.975, d);
        double chiLow = deviates_chi_sq(1-0.975, d);

		if (fError == "")
		{
			//Limits
			resultat.s0PostUpLimit = sqrtq(chiUp/d);
			resultat.s0PostLoLimit = sqrtq(chiLow/d);

			if ((sqrtq(sigmaZero2) < resultat.s0PostUpLimit) && (sqrtq(sigmaZero2) > resultat.s0PostLoLimit))
			{
				fS0APosterioriVariances = false;
			}
		}
	}
	return resultat;
}


void    TALSComputer::calcOverall(TVector* fZ)
{
	int nbUnk = fZ->size();
	int i = 0;
	while (i<nbUnk)
	{
		fOverall += powq(fZ->coeff(i),2)-1;
		i++;
	}
	fOverall /= nbUnk;
	return;
}