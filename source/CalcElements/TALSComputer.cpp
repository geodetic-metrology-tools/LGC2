#include "TALSComputer.h"
#include "TVNumericValue.h"
#include "QuantileFunctions.h"
#include <memory>
#include <iostream>

//////////////////////////
// no argument constructor
//////////////////////////
TALSComputer::TALSComputer():fError("")
{}


/////////////
// destructor
/////////////
TALSComputer::~TALSComputer(){}

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

		}
	}
	return resultat;
}