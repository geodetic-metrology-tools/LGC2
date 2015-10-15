#include "TLSCombinedMtdComputer.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"
#include "TSparseMatrix.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <Eigen/LU>


TLSCombinedMtdComputer::TLSCombinedMtdComputer()
	:count(1)
{//default constructor
}


TLSCombinedMtdComputer::~TLSCombinedMtdComputer()
{//destructor
}

bool TLSCombinedMtdComputer::computeResults(TLSInputMatrices* im, TLSResultsMatrices* rm)
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

//!Computes the results matrices
bool TLSCombinedMtdComputer::computeResultsMtrs(TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	//test if we do not have a 'nullpointer' in a case that the matrices are not initialize
	if (im->getFirstDgnMtrx() == nullptr || im->getSecondDgnMtrx() == nullptr || im->getWeightInvMtrx() == nullptr)
		throw std::runtime_error("Any of the design matrices is not initialized!");
	
	const TSparseMatrix& A = *im->getFirstDgnMtrx();	
	const TSparseMatrix& B = *im->getSecondDgnMtrx();
	const TSparseMatrix& InvPv = *im->getWeightInvMtrx();
	const TVector & misclV = im->getMisclosureVctr(); //W


	int nbUnk = im->getNbrUnknowns();
	int nbEq = im->getNbrEquations();

#ifdef _DEBUG
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, A=\n " << A << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, B=\n " << B << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, Pv1=\n " <<InvPv << std::endl;
#endif

	TSparseMatrix N2(nbUnk, nbUnk); //N2 = transpose(A1)*inv(N1)*A1
	TVector VP(nbUnk); //VP = transpose(A1)*inv(N1)*W
           

	//calculate N1 = B1*inv(Pv1)*transpose(B1)
	const TSparseMatrix Btransposed = B.transpose();
	TSparseMatrix N1 = B * InvPv * Btransposed;

#ifdef _DEBUG
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, N1=\n " << N1 << std::endl;
#endif

	Eigen::SimplicialLDLT<TSparseMatrix> chol( N1 );

#ifdef _DEBUG
	//std::cout << "TLSCombinedMtdComputer::computeResultsMtrs, det(N1)=\n " << chol.determinant() << std::endl;
#endif

	if(chol.info() != Eigen::Success)
	{
		// cholesky did not work, try fullPiv
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu(N1);

		if (! lu.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedMtdComputer::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return false;
		}
		TSparseMatrix pAtInvN1(nbUnk, nbEq);
		pAtInvN1 = A.transpose() * (lu.inverse());
		N2 = pAtInvN1 * A;
		rm->setIntermediateMatrix(N2);
		VP = pAtInvN1 * misclV;
	} 
	else
	{
		TSparseMatrix pInvN1A(nbEq, nbUnk);
		pInvN1A = chol.solve(A);
		N2 = A.transpose() * pInvN1A;
		rm->setIntermediateMatrix(N2);

		TVector pInvN1W = chol.solve(misclV);
		VP = A.transpose() * pInvN1W;
	}

	//inverse N2
	Eigen::SimplicialLDLT<TSparseMatrix> chol2( N2 );

	#ifdef _DEBUG
			//std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N2)=\n " << chol2.determinant() << std::endl;
	#endif

	if(chol2.info() != Eigen::Success)
	{
		// cholesky did not work, try fullPiv
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu2(N2);

		if (! lu2.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return false;
		}
	
		*(rm->getSolutionVctr()) = -lu2.solve(  VP );
		return true;
	}
	else {
		*(rm->getSolutionVctr()) = -chol2.solve( VP );
		return true;
	}
}

void	TLSCombinedMtdComputer::calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr, TLSResultsMatrices* rm)
{
	if (fError == "")
	{
		int nbUnk = inputMtr->getNbrUnknowns();
		int nbObs = inputMtr->getNbrObservations();
		int nbEq = inputMtr->getNbrEquations();
		TReal sigmaZero2Aposteriori = LITERAL(0.0);

		if (inputMtr->getFirstDgnMtrx() == nullptr || inputMtr->getSecondDgnMtrx() == nullptr || inputMtr->getWeightMtrx() == nullptr || inputMtr->getWeightInvMtrx() == nullptr ||
				rm->getSolutionVctr() == nullptr || rm->getResidualsVctr() == nullptr || rm->getResCovarMtrx() == nullptr)
			throw std::runtime_error("Any of the design matrices is not initialized!");


		const TSparseMatrix &A = *inputMtr->getFirstDgnMtrx();
		const TSparseMatrix AT = A.transpose();
		const TSparseMatrix &B = *inputMtr->getSecondDgnMtrx();
		const TSparseMatrix BT = B.transpose();
		const TSparseMatrix &Pv = *inputMtr->getWeightMtrx();
		const TSparseMatrix &InvPv = *inputMtr->getWeightInvMtrx();
		const TVector & misclV = inputMtr->getMisclosureVctr();
		const TVector & solution = *rm->getSolutionVctr(); 
		TVector & residuals = *rm->getResidualsVctr();
		TSparseMatrix & ResCovarMtrx = *rm->getResCovarMtrx();
		TSparseMatrix & Qxx = *rm->getUnkCovarMtrx();
		const TSparseMatrix & N2 = rm->getIntermediateMatrix();
		TSparseMatrix N1 = B * InvPv * BT;
		
	
		
		//--------------- Residuals ---------------//
		//Calculate S= Inv(P)*Bt*inv(N1)
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu(N1);

		if (! lu.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedMtdComputer::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return;
		}
		TSparseMatrix pS(nbObs, nbEq);
		pS = -InvPv * BT * (lu.inverse());

		residuals =  pS * (A * solution + misclV);

		//--------------- Sigma 0 a posteriri ---------------//
		sigmaZero2Aposteriori = residuals.transpose() * Pv * residuals;
		if(nbObs != nbUnk)
			sigmaZero2Aposteriori /= (nbObs - nbUnk);  
		else
			fError += "Number of equations equals number of unknowns, causes zero division!";
		rm->setSigmaZero2(sigmaZero2Aposteriori);
		struct limits fisherLim = calcSigmaZeroLimits(nbObs, nbUnk, sigmaZero2Aposteriori);
		rm->setSigmaZero2Limits(fisherLim.s0PostLoLimit, fisherLim.s0PostUpLimit);


		//--------------- Residual covariance matrix ---------------//
		//inverse N2
		Eigen::SimplicialLDLT<TSparseMatrix> chol2( N2 );

		#ifdef _DEBUG
				//std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N2)=\n " << chol2.determinant() << std::endl;
		#endif

		if(chol2.info() != Eigen::Success)
		{
			// cholesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu2(N2);

			if (! lu2.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return;
			}
		
			TSparseMatrix SAQ(nbObs, nbUnk);
			SAQ = pS * A * (lu2.inverse()) ; 
			ResCovarMtrx = pS * B * InvPv - SAQ * AT * pS.transpose();

		}
		else {
			TSparseMatrix QAT(nbUnk, nbEq);
			QAT = chol2.solve(AT) ; 
			ResCovarMtrx = pS * B * InvPv - pS * A * QAT * pS.transpose();

		}
		rm->setResCovarMtrx(&ResCovarMtrx);


		//--------------- Unknowns covariance matrix ---------------//
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu2(N2);

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
	}
}
