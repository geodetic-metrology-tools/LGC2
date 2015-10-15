//
// TLSParametricMtdComputer.cpp : implementation file
// class for a least squares computer producing the results matrices
// using the parametric method (case where 2nd dgn mtrx = -I)
//


#include "TLSParametricMtdComputer.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"
#include "TSparseMatrix.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <Eigen/LU>
//#include <Eigen/Cholesky>

//////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////
TLSParametricMtdComputer::TLSParametricMtdComputer()
	:count(1)
{//default constructor
}


TLSParametricMtdComputer::~TLSParametricMtdComputer()
{//destructor
}



bool TLSParametricMtdComputer::computeResults(TLSInputMatrices* im , TLSResultsMatrices* rm)
{
	bool result = false;
	int nbCnstr = im->getNbrConstraints();
	if(rm->getSolutionVctr()->size() != 0)
	{
		if(nbCnstr == 0)
		{
			result = computeResultsMtrs(im, rm);
		}
		else
		{
			//result = computeFreeResultsMtrs(im, rm);
			fError = "Free network calculation not yet implemented!";
			result = false;
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
bool TLSParametricMtdComputer::computeResultsMtrs(TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	//test if we do not have a 'nullpointer' in a case that the matrices are not initialize
	if (im->getFirstDgnMtrx() == nullptr || im->getSecondDgnMtrx() == nullptr || im->getWeightInvMtrx() == nullptr)
		throw std::runtime_error("Any of the design matrices is not initialized!");


	const TSparseMatrix & A = *im->getFirstDgnMtrx();
	const TSparseMatrix & W = *im->getWeightMtrx();

#ifdef _DEBUG
	//std::cout << "TLSParametricMtdComputer::computeResultsMtrs, A=\n " << *A << std::endl;
	//std::cout << "TLSParametricMtdComputer::computeResultsMtrs, W=\n " <<*W << std::endl;
#endif

	const TVector & misclV = im->getMisclosureVctr();
	TSparseMatrix N = A.transpose() * W * A;
	rm->setIntermediateMatrix(N);

	Eigen::SimplicialLDLT<TSparseMatrix> chol( N );

#ifdef _DEBUG
	//std::cout << "TLSParametricMtdComputer::computeResultsMtrs, det(N)=\n " << chol.determinant() << std::endl;
#endif

if(chol.info() != Eigen::Success)
	{
		// cholesky did not work, try fullPiv
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu(N);

		if (! lu.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSParametricMtdComputer::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return false;
		}
		*(rm->getSolutionVctr()) = lu.solve( -A.transpose() * W * misclV );
		return true;

	} 
	else {
		*(rm->getSolutionVctr()) = chol.solve( -A.transpose() * W * misclV );
		return true;
	}
}



void	TLSParametricMtdComputer::calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr, TLSResultsMatrices* rm)
{
	if (fError == "")
	{
		int nbUnk = inputMtr->getNbrUnknowns();
		int nbObs = inputMtr->getNbrObservations();
		TReal sigmaZero2Aposteriori = LITERAL(0.0);

		const TSparseMatrix & A1 = *inputMtr->getFirstDgnMtrx();
		const TSparseMatrix A1T = A1.transpose();
		const TSparseMatrix & Pv = *inputMtr->getWeightMtrx();
		const TSparseMatrix & InvPv = *inputMtr->getWeightInvMtrx();
		const TVector & misclV = inputMtr->getMisclosureVctr();
		TVector & solution = *rm->getSolutionVctr();
		TVector & residuals = *rm->getResidualsVctr();
		TSparseMatrix & ResCovarMtrx = *rm ->getResCovarMtrx(); 
		TSparseMatrix & Qxx = *rm->getUnkCovarMtrx();
		TSparseMatrix N = rm->getIntermediateMatrix();
		TSparseMatrix MP(nbObs, nbObs);

		//--------------- Residuals ---------------//
		residuals = (A1 * solution + misclV);

		//--------------- Sigma 0 a posteriri ---------------//
		sigmaZero2Aposteriori = residuals.transpose() * Pv * residuals;
		if(nbObs != nbUnk)
			sigmaZero2Aposteriori /= (nbObs - nbUnk);  
		else
			fError += "Number of equations equals number of unknowns, causes zero division!";
		rm->setSigmaZero2(sigmaZero2Aposteriori);
		struct limits fisherLim = calcSigmaZeroLimits(nbObs, nbUnk, sigmaZero2Aposteriori);
		rm->setSigmaZero2Limits(fisherLim.s0PostLoLimit, fisherLim.s0PostUpLimit);


		//--------------- Unknowns covariance matrix ---------------//
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu2(N);
		
		if (! lu2.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return;
		}
		TSparseMatrix Id(nbUnk,nbUnk);
		for (int i = 0; i<nbUnk; i++)
			Id.insert(i,i) = 1;
		
		Qxx = Id*lu2.inverse();
		rm->setUnkCovarMtrx(&Qxx);


		//--------------- Residual covariance matrix ---------------//
		Eigen::SimplicialLDLT<TSparseMatrix> chol( N ); 

		#ifdef _DEBUG
			//std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N)=\n " << chol.determinant() << std::endl;
		#endif

		if(chol.info() != Eigen::Success)
		{
			// cholesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu(N);

			if (! lu.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return;
			}
			TSparseMatrix MP1(nbObs,nbUnk);
			MP1 = A1 * (lu.inverse()) ;
			MP = MP1 * A1.transpose();
		} 
		else
		{
			//rm->setIntermediateMatrix(N);
			TSparseMatrix MP1(nbUnk,nbObs);
			MP1 = chol.solve(A1T);
			MP = A1 * MP1 ;
		} 
		ResCovarMtrx = InvPv - MP;
		rm->setResCovarMtrx(&ResCovarMtrx);
	}
}

////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////
