#include "TLSCnstMtdComputer.h"
#include "TLSResultsMatrices.h"
#include "TLSInputMatrices.h"
#include "TSparseMatrix.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <Eigen/LU>


TLSCnstMtdComputer::TLSCnstMtdComputer()
	:count(1)
{//default constructor
}


TLSCnstMtdComputer::~TLSCnstMtdComputer()
{//destructor
}

bool TLSCnstMtdComputer::computeResults(TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	bool result = false;
	int nbCnstr = im->getNbrConstraints();
	if(rm->getSolutionVctr()->size() != 0)
	{
		if(nbCnstr != 0)
		{
			result = computeResultsMtrs(im, rm);
		}
		else
		{
			//result = computeFreeResultsMtrs(im, rm);
			fError = "We are not in a constraint network!";
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
bool TLSCnstMtdComputer::computeResultsMtrs(TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	//test if we do not have a 'nullpointer' in a case that the matrices are not initialize
	if (im->getFirstDgnMtrx() == nullptr || im->getSecondDgnMtrx() == nullptr || im->getWeightInvMtrx() == nullptr)
		throw std::runtime_error("Any of the design matrices is not initialized!");
	
	const TSparseMatrix& A = *im->getFirstDgnMtrx();	
	const TSparseMatrix& B = *im->getSecondDgnMtrx();
	const TSparseMatrix& InvPv = *im->getWeightInvMtrx();
	const TVector & misclV = im->getMisclosureVctr(); //W
	const TSparseMatrix& A2 = *im->getCnstrFirstDgnMtrx();//A2
	const TSparseMatrix A2Transpose = A2.transpose();//A2
	const TVector & W2 = im->getCnstrMisclosureVctr(); //W2

	int nbUnk = im->getNbrUnknowns();
	int nbEq = im->getNbrEquations();
	int nbCnstr = im->getNbrConstraints();

#ifdef _DEBUG
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, A=\n " << A << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, B=\n " << B << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, Pv1=\n " <<InvPv << std::endl;
#endif

	//Intermediate matrces and vector
	TSparseMatrix N3(nbCnstr,nbCnstr); //N3 = A2*inv(N2)*transpose(A2)
	TSparseMatrix N2(nbUnk, nbUnk); //N2 = transpose(A1)*inv(N1)*A1
	TVector VP(nbUnk); //VP = transpose(A1)*inv(N1)*W
    TVector VP2(nbUnk); //VP2 = W2 - transpose(A2)*inv(N2)*VP      

	//calculate N1 = B1*inv(Pv1)*transpose(B1)
	const TSparseMatrix Btransposed = B.transpose();
	TSparseMatrix N1 = B * InvPv * Btransposed;

	//Inverse N1
	Eigen::SimplicialLDLT<TSparseMatrix> chol( N1 );

	#ifdef _DEBUG
		std::cout << "TLSCombinedMtdComputer::computeResultsMtrs, det(N1)=\n " << chol.determinant() << std::endl;
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
		// AtInvN1, Intermediate matrix to calcuate N2 and VP
		TSparseMatrix AtInvN1(nbUnk, nbEq);  
		AtInvN1 = A.transpose() * (lu.inverse());  //AtInvN1 = A1.transpose() * Inv(N1)
		N2 = AtInvN1 * A;
		rm->setIntermediateMatrix(N2);

		VP = AtInvN1 * misclV;
	} 
	else
	{
		// InvN1A, Intermediate matrix to calcuate N2
		TSparseMatrix InvN1A(nbEq, nbUnk);
		InvN1A = chol.solve(A);  // InvN1A = Inv(n1) * A1
		N2 = A.transpose() * InvN1A;
		rm->setIntermediateMatrix(N2);
	
		// InvN1W, Intermediate matrix to calcuate VP
		TVector InvN1W = chol.solve(misclV);  // InvN1W = inv(N1) * W1
		VP = A.transpose() * InvN1W;
	}

	//inverse N2
	Eigen::SimplicialLDLT<TSparseMatrix> chol2( N2 );

	#ifdef _DEBUG
			std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N2)=\n " << chol2.determinant() << std::endl;
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
		// Intermediate matrix to calculate VP2 
		TVector InvN2VP = lu2.solve(  VP );
		VP2 = W2 - A2*InvN2VP;

		// Intermediate matrix to calculate N3
		TSparseMatrix N3P(nbCnstr,nbUnk);
		N3P = A2*lu2.inverse();
		N3 = N3P*A2Transpose;

		//inverse N3
		Eigen::SimplicialLDLT<TSparseMatrix> chol3( N3 );
	
		#ifdef _DEBUG
				std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N3)=\n " << chol3.determinant() << std::endl;
		#endif
	
		if(chol3.info() != Eigen::Success)
		{
			// cholesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu3(N3);
	
			if (! lu3.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return false;
			}

				TVector InvN3VP2 = lu3.solve(VP2);  // InvN3VP2 = inv(n3)*VP2
				TVector rightPart = A2Transpose * InvN3VP2 + VP;  // rightPart = A2.transpose()*inv(n3)*VP2 + VP
				*(rm->getSolutionVctr()) = -lu2.solve(  rightPart ); // solution = - inv(N2) * rightPart
				return true;
			}
			else {
				TVector InvN3VP2 = chol3.solve(VP2); // InvN3VP2 = inv(n3)*VP2
				TVector rightPart = A2Transpose * InvN3VP2 + VP; // rightPart = A2.transpose()*inv(n3)*VP2 + VP
				*(rm->getSolutionVctr()) = -lu2.solve(  rightPart ); // solution = - inv(N2) * rightPart
				return true;
			}
			
	}
	else{
		// Intermediate matrix to calculate VP2
		TVector InvN2VP = chol2.solve(  VP );
		VP2 = W2 - A2*InvN2VP;
	
		// Intermediate matrix to calculate N3
		TSparseMatrix N3P(nbUnk,nbCnstr);
		N3P = chol2.solve( A2Transpose);
		N3 = A2*N3P;
	
		//inverse N3
		Eigen::SimplicialLDLT<TSparseMatrix> chol3( N3 );
	
		#ifdef _DEBUG
				std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N3)=\n " << chol3.determinant() << std::endl;
		#endif
	
		if(chol3.info() != Eigen::Success)
		{
			// cholesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu3(N3);
	
			if (! lu3.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return false;
			}
	
			TVector InvN3VP2 = lu3.solve(VP2); // InvN3VP2 = inv(n3)*VP2
			TVector rightPart = A2Transpose * InvN3VP2 + VP; // rightPart = A2.transpose()*inv(n3)*VP2 + VP
			*(rm->getSolutionVctr()) = -chol2.solve(  rightPart ); // solution = - inv(N2) * rightPart
			return true;
		}
		else {
			TVector InvN3VP2 = chol3.solve(VP2); // InvN3VP2 = inv(n3)*VP2
			TVector rightPart = A2Transpose * InvN3VP2 + VP; // rightPart = A2.transpose()*inv(n3)*VP2 + VP
			*(rm->getSolutionVctr()) = -chol2.solve(  rightPart ); // solution = - inv(N2) * rightPart
			return true;
		}
	}
}

void	TLSCnstMtdComputer::calcResiduAndVarCovMatrice(const TLSInputMatrices* inputMtr, TLSResultsMatrices* rm)
{
	if (fError == "")
	{
		int nbUnk = inputMtr->getNbrUnknowns();
		int nbObs = inputMtr->getNbrObservations();
		int nbEq = inputMtr->getNbrEquations();
		int nbCnstr = inputMtr->getNbrConstraints();
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
		const TSparseMatrix N1 = B * InvPv * BT;
		const TSparseMatrix& A2 = *inputMtr->getCnstrFirstDgnMtrx(); //A2
		const TSparseMatrix A2Transpose = A2.transpose(); //A2.transpose()
		//Not used so far, maybe later, commented for now
		//const TVector & W2 = inputMtr->getCnstrMisclosureVctr(); //W2
		
		TSparseMatrix N3(nbCnstr,nbCnstr); //N3 = A2*inv(N2)*transpose(A2)
		TSparseMatrix InvN2(nbUnk, nbUnk);
		TSparseMatrix MPassage(nbUnk, nbUnk); //MPassage = A2.transpose() * InvN3 *A2 * InvN2, use in CX
		TSparseMatrix CX(nbUnk,nbEq); // Matrix defining Qxx : Qxx = CX*N1*CX.transpose()
		TSparseMatrix CV(nbObs, nbEq); //Matrix defining ResCovarMtrx : ResCovarMtrx = CV*N1*CV.transpose()

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
		pS = InvPv * BT * (lu.inverse());
		residuals = - pS * (A * solution + misclV);

		// Intermediate matrix,  A1TInvN1 = A1.transpose()*Inv(n1)
		TSparseMatrix A1TInvN1(nbUnk,nbEq);
		A1TInvN1 = AT*lu.inverse();

		//--------------- Sigma 0 a posteriri ---------------//
		sigmaZero2Aposteriori = residuals.transpose() * Pv * residuals;
		if(nbObs != nbUnk)
			sigmaZero2Aposteriori /= (nbObs+nbCnstr - nbUnk);  
		else
			fError += "Number of equations equals number of unknowns, causes zero division!";
		rm->setSigmaZero2(sigmaZero2Aposteriori);
		struct limits fisherLim = calcSigmaZeroLimits(nbObs+nbCnstr, nbUnk, sigmaZero2Aposteriori);
		rm->setSigmaZero2Limits(fisherLim.s0PostLoLimit, fisherLim.s0PostUpLimit);


		//--------------- Unknowns covariance matrix ---------------//
		//Calculate inverse of N2 (InvN2) using LU decomposition
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu2(N2);

		if (! lu2.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return;
		}
		
		//Intermediate matrix A2*invN2
		TSparseMatrix A2InvN2(nbCnstr,nbUnk);
		A2InvN2 = A2*lu2.inverse();

		TSparseMatrix Id(nbUnk,nbUnk); // need identity matrix to use .inverse()
		for (int i = 0; i<nbUnk; i++)
			Id.insert(i,i) = 1;
		InvN2 = Id*lu2.inverse();


		N3 = A2*InvN2*A2Transpose;
		
		//inverse N3
		Eigen::SimplicialLDLT<TSparseMatrix> chol3( N3 );

		#ifdef _DEBUG
				std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N3)=\n " << chol3.determinant() << std::endl;
		#endif

		if(chol3.info() != Eigen::Success)
		{
			// cholesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu3(N3);

			if (! lu3.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return;
			}
			//Intermediate matrix  A2.transpose() * inv(N3)
			TSparseMatrix A2TInvN3(nbCnstr,nbUnk);
			A2TInvN3 = A2Transpose*lu3.inverse();
			MPassage = A2TInvN3*A2InvN2;  

		}
		else {
			TSparseMatrix N3A2InvN2(nbCnstr, nbUnk);
			N3A2InvN2 = chol3.solve(A2InvN2);
			MPassage = A2Transpose*N3A2InvN2;

		}

		CX = InvN2*(MPassage-Id)*A1TInvN1;
		Qxx = CX*N1*CX.transpose();
		rm->setUnkCovarMtrx(&Qxx);

		
		//--------------- Residual covariance matrix ---------------//
		TSparseMatrix I(nbEq,nbEq);
		for (int i = 0; i<nbEq; i++)
			I.insert(i,i) = 1;

		//inverse N1
		Eigen::SimplicialLDLT<TSparseMatrix> chol4( N1 );

		#ifdef _DEBUG
				std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N1)=\n " << chol4.determinant() << std::endl;
		#endif

		if(chol4.info() != Eigen::Success)
		{
			// cholesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu4(N1);

			if (! lu4.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return;
			}
			//Calculate CV = - invP* BT * InvN1*(A*CX+I)
			TSparseMatrix BTInvN1(nbObs,nbEq); //intermediate matrix BT * invN1
			BTInvN1 = BT*lu4.inverse();
			CV = -InvPv*BTInvN1*(A*CX+I);
		
		}
		else {
			//Calculate CV = - invP* BT * InvN1*(A*CX+I)
			TSparseMatrix MPass(nbEq,nbEq); // intermediate matrix MPass = InvN1*(A*CX+I)
			TSparseMatrix rightMatrix = A*CX+I;
			MPass = chol4.solve(rightMatrix);
			CV = -InvPv *B.transpose() *MPass;

		}

		ResCovarMtrx = CV*N1*CV.transpose();
		rm->setResCovarMtrx(&ResCovarMtrx);
	
	}
}
