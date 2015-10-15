//
// TLSComputer.cpp : implementation file
// class for a least squares computer producing the results matrices 
// using the parametric method (case where 2nd dgn mtrx = -I)
//

#include "TLSInputMatrices.h"
#include "TLSResultsMatrices.h"

#include "TLSComputer.h"

//#include	<nag.h>
//#include	<nagg01.h>
//////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////
TLSComputer::TLSComputer():
fAtPA(0), fError("")/*, fS0PostUpLimit(0.0), fS0PostLoLimit(0.0), fSigmaZero2(0.0)*/
{//default constructor
}


TLSComputer::~TLSComputer()
{//destructor
	if(fAtPA!=0)
	{
		delete fAtPA;
	}
}



//bool TLSComputer::computeResults(const TLSInputMatrices* im , TLSResultsMatrices* rm)
bool TLSComputer::computeResults(TLSInputMatrices* im , TLSResultsMatrices* rm)
{
	bool result;
	int nbCnstr = im->getNbrConstraints();
	if(rm->getSolutionVctr()->dimension() != 0)
	{
		if(nbCnstr == 0)
		{
			result = computeResultsByParametricMethod(im, rm);
		}
		else
		{
			result = computeResultsByParametricMethodWithMeislBordering( im, rm);
		}
	}
	else
	{//pas d'inconnue
		result = true;
	}
	return result;
}





////////////////////////////////////////////////////////////////
//COMPUTES THE RESULTS MATRICES
////////////////////////////////////////////////////////////////
bool TLSComputer::computeResultsByParametricMethod(TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	/* in this method, the solution is computed with a numeric equations solver method (nagc lib).
	   The unknown variance-covariance matrix is thus not computed here. 
	   The fAtPAInv attribute contains the AtPA product, and is updated during each iteration. 
	   It is finally inverted outside the method, when there are no iterations left to 
	   be performed (in TLSCalculation). */

	const TMatrix& firstDM = im->getFirstDgnMtrx();
	const TMatrix& weightM = im->getWeightMtrx();
	const TColumnVector& misclV = im->getMisclosureVctr();

	int nbUnk = im->getNbrUnknowns();
	int nbObs = im->getNbrObservations();
	int nbCnstrObs = im->getNbrConstraintObs();
	
	//intermediate (AtPA) matrix
	if (fAtPA == 0)
	{
		fAtPA =  new TMatrix(nbUnk, nbUnk);
	}
	else 
	{
		*fAtPA = 0.0;
	}

	// computation of normal equations matrix (not inverted yet)
	*fAtPA = (firstDM.transposed() * weightM * firstDM);



	//computation of the solution vector
	TColumnVector* solution = rm->getSolutionVctr();
	/**************************/
	/*gets to here and crashes*/
	/**************************/
	*solution = TReal(0.0);
	// computes solution using a equations system solver (Nx = B)
	*solution = firstDM.transposed() * weightM * misclV * -1.0;	
	*solution = fAtPA->eqnSolve(*solution);

	if (solution->isNull())
	{// if eqnSolve method fails, an error message is generated
		fError = fAtPA->getError();
		return false;
	}

	return true;
}


////////////////////////////////////////////////////////////////
//COMPUTES THE RESULTS MATRICES FOR FREE CALCULATION
////////////////////////////////////////////////////////////////
bool TLSComputer::computeResultsByParametricMethodWithMeislBordering(TLSInputMatrices* im, TLSResultsMatrices* rm){

//	cout << "Entered computer\n";

	const TMatrix& firstDM = im->getFirstDgnMtrx(); //A1
	const TMatrix& weightM = im->getWeightMtrx();  //P
	const TColumnVector& misclV = im->getMisclosureVctr(); //W1

	const TMatrix& cnstrFirstDM = im->getCnstrFirstDgnMtrx(); //A2
	const TColumnVector& cnstrMisclV = im->getCnstrMisclosureVctr(); //W2      

	int nbUnk = im->getNbrUnknowns();
	int nbObs = im->getNbrObservations();
	int nbCnstr = im->getNbrConstraints();
	int nbCnstrObs = im->getNbrConstraintObs();

	//intermediate N = (A1tPA1) matrix
	TMatrix	N (nbUnk, nbUnk);
	N = 0.0;
	
	N = firstDM.transposed() * weightM * firstDM;

	//intermediate Nbig
	/*
	Nbig = ( (N  , A2t)
			 (A2, 0 ))
	*/
	TMatrix	Nbig (nbUnk + nbCnstr, nbUnk + nbCnstr);
	Nbig = 0.0;

	//insert N in Nbig
	int i = 0;
	while( i < nbUnk )
	{
		//insert N
		int j = 0;
		while(j < nbUnk)
		{// modif
			Nbig(i,j) = N(i,j);
			j++;
		}

		
		int J = 0;
		while(j<nbUnk + nbCnstr)
		{
			Nbig(i,j) = cnstrFirstDM(J,i);//insert A2t
			Nbig(j,i) = cnstrFirstDM(J,i);//insert A2
			j++;
			J++;
		}
		i++;
	}

	//inverse Nbig matrix
	TMatrix NbigInv (nbUnk + nbCnstr, nbUnk + nbCnstr);
	NbigInv = 0,0;
	// NbigInv = intermediate matrix containing LU decomposition for solving of equation system
////	int n_pivot(NbigInv.numRows()-1); // pivot used in LU decomposition
////	int* pivot_i; // pivot used in LU decomposition
////	int* pivot_j; // pivot used in LU decomposition

////	pivot_i = new int [n_pivot+1];
////	pivot_j = new int [n_pivot+1];
////	// LU decomposition
////	NbigInv = Nbig.dfact(&n_pivot,pivot_i,pivot_j);


	
////	if (NbigInv.isNull())
////	{// if inverse method fails, an error message is generated
////		fError = NbigInv.getError();
////		return false;
////	}

	//intermediate ColumnVector Cbig
	/*
	Cbig = ( (A1t*p*misclV)
			 (cnstrMisclV ) )
	*/
	TColumnVector Cbig ( nbUnk + nbCnstr);
	Cbig = TReal(0.0);
	TColumnVector C ( nbUnk);
	C = TReal(0.0);
	C = firstDM.transposed() * weightM * misclV*(-1.0);

	//insert C in Cbig
	i = 0;
	while( i < nbUnk )
	{
		Cbig(i) = C(i);
		i++;
	}

	//insert cnstrMisclV in Nbig
	i = nbUnk;
	int I = 0;
	while( i < (nbUnk + nbCnstr) )
	{
		Cbig(i) = cnstrMisclV(I);
		i++;
		I++;
	}


	//computation of the solution vector
////	TColumnVector solutionBig (nbUnk + nbCnstr);
////	solutionBig = 0.0;
////	solutionBig = NbigInv.dfeqn(&Cbig,n_pivot,pivot_i,pivot_j);
	TColumnVector solutionBig = NbigInv.eqnSolve(Cbig); 

////	delete[] pivot_i;
////	delete[] pivot_j;


	if (solutionBig.isNull())
	{// if dfeqn method fails, an error message is generated
		fError = NbigInv.getError();
		return false;
	}

	TColumnVector* solution = rm->getSolutionVctr();
	(*solution) = TReal(0.0);
	// extraction of solution from solutionBig
	i = 0;
	while( i < nbUnk)
	{
		(*solution)(i) =  solutionBig(i);
		i++;
	}

	if (fAtPA == 0)
	{
		fAtPA = new TMatrix (nbUnk + nbCnstr, nbUnk + nbCnstr);
	}
	*fAtPA = Nbig;


	return true;
}



////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////
