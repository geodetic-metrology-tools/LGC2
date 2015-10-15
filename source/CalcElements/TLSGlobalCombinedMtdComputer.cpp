#include "lsalgo/TLSGlobalCombinedMtdComputer.h"

#include "lsalgo/TLSResultsMatrices.h"
#include "lsalgo/TLSInputMatrices.h"
//#include "TLSResultsMatricesExtractor.h"
#include "TSparseMatrix.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <Eigen/LU>


TLSGlobalCombinedMtdComputer::TLSGlobalCombinedMtdComputer():
fError("")/*, fS0PostUpLimit(LITERAL(0.0)), fS0PostLoLimit(LITERAL(0.0)), fSigmaZero2(LITERAL(0.0))*/
{//default constructor
	count = 1;
}


TLSGlobalCombinedMtdComputer::~TLSGlobalCombinedMtdComputer()
{//destructor
}

bool TLSGlobalCombinedMtdComputer::computeResults(TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	bool result;
	int nbCnstr = im->getNbrConstraints();
	if(rm->getSolutionVctr()->size() != 0)
	{
		if(nbCnstr == 0)
		{
			result = computeResultsMtrs(im, rm);
		}
		/*else
		{
			result = computeFreeResultsMtrs( im, rm);
		}*/
	}
	else
	{//pas d'inconnue
		result = true;
	}
	return result;
}

//!Computes the results matrices
bool TLSGlobalCombinedMtdComputer::computeResultsMtrs(TLSInputMatrices* im, TLSResultsMatrices* rm)
{
	/* in this method, the solution is computed with a numeric equations solver method (nagc lib).
	   The unknown variance-covariance matrix is thus not computed here.
	   The fAtPAInv attribute contains the AtPA product, and is updated during each iteration.
	   It is finally inverted outside the method, when there are no iterations left to
	   be performed (in TLSCalculation). */

	const TSparseMatrix * A1 = im->getFirstDgnMtrx();
	const TSparseMatrix * B1 = im->getSecondDgnMtrx();
	const TSparseMatrix * Pv1 = im->getWeightMtrx();
	const TVector & misclV1 = im->getMisclosureVctr(); //W1


	const TSparseMatrix * A2 = im->getFirstDgnMtrx(); //method to redefine
	const TSparseMatrix * B2 = im->getSecondDgnMtrx();//method to redefine
	const TSparseMatrix * Pv2 = im->getWeightMtrx();//method to redefine
	const TVector & misclV2 = im->getMisclosureVctr(); //W2  method to redefine

	const TSparseMatrix * A3 = im->getFirstDgnMtrx(); //method to redefine
	const TSparseMatrix * Px = im->getWeightMtrx();//method to redefine
	const TVector & misclV3 = im->getMisclosureVctr(); //W3  method to redefine

#ifdef _DEBUG
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, A1=\n " << *A1 << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, A2=\n " << *A2 << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, A3=\n " << *A3 << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, B1=\n " << *B1 << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, B2=\n " << *B2 << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, Pv1=\n " <<*Pv1 << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, Pv2=\n " <<*Pv2 << std::endl;
	//std::cout << "TLSCombinedCalculation::computeResultsMtrs, Px=\n " <<*Px << std::endl;
#endif

	//inverse the weight matrices
	Eigen::SimplicialLDLT<TSparseMatrix> chol1( *Pv1 );
	Eigen::SimplicialLDLT<TSparseMatrix> chol2( *Pv2 );

	//calculate N1 = B1*inv(Pv1)*transpose(B1)
	TSparseMatrix* MPassage11;
	TSparseMatrix transposed1 = B1->transpose();
	*MPassage11 = chol1.solve( transposed1 );
	TSparseMatrix N1 = (*B1) * (*MPassage11);

	//inverse N1
	Eigen::SimplicialLDLT<TSparseMatrix> chol12( N1 );

	//calculate N2 = B2*inv(Pv2)*transpose(B2)
	TSparseMatrix* MPassage21;
	TSparseMatrix transposed2 = B2->transpose();
	*MPassage21 = chol2.solve( transposed2 );
	TSparseMatrix N2 = (*B2) * (*MPassage21);

	//inverse N2
	Eigen::SimplicialLDLT<TSparseMatrix> chol22( N2 );


	TSparseMatrix* PartW1;
	TSparseMatrix* PartW2;
	TSparseMatrix*  Q;
	TSparseMatrix* K3;



#ifdef _DEBUG
	std::cout << "TLSCombinedMtdComputer::computeResultsMtrs, det(N1)=\n " << chol12.determinant() << std::endl;
	std::cout << "TLSCombinedMtdComputer::computeResultsMtrs, det(N2)=\n " << chol22.determinant() << std::endl;
#endif

	//inverse N1 with choleski didn't wokrk
	if(chol12.info() != Eigen::Success)
	{
		// colesky did not work, try fullPiv
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu1(N1);

		if (! lu1.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedMtdComputer::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return false;
		}

		TSparseMatrix * qwe;
		*qwe = A1->transpose() * ( lu1.inverse() );
		*PartW1 = *qwe * (misclV1);
		*Q = *Q + *qwe * (*A1);

		//rm->setIntermediateMatrix(N1);
	}
	else {
		TSparseMatrix* MPassage12;
		//* MPassage12 = chol12.solve(misclV1);    TO DO: convert misclV into a matrix
		*PartW1 = A1->transpose() *( * MPassage12 );

		TSparseMatrix* MPassage13;
		* MPassage13 = chol12.solve(*A1);
		*Q = *Q + A1->transpose() * ( * MPassage13 );

		//rm->setIntermediateMatrix(N1);
	}

	//inverse N2 with choleski didn't wokrk
	if(chol22.info() != Eigen::Success)
	{
		// colesky did not work, try fullPiv
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu2(N1);

		if (! lu2.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedMtdComputer::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return false;
		}

		TSparseMatrix * qwe2;
		*qwe2 = A2->transpose() * ( lu2.inverse() );
		*PartW2 = *qwe2 * (misclV2);
		*Q = *Q + *qwe2 * (*A2);

		//rm->setIntermediateMatrix(N2);
	}
	else {
		TSparseMatrix* MPassage22;
		TSparseMatrix* W2; //misclV2   TSparseMatrix not TVector
		* MPassage22 = chol2.solve( *W2 );  
		*PartW2 = A2->transpose() *( * MPassage22 );

		TSparseMatrix* MPassage23;
		* MPassage23 = chol12.solve(*A2);
		*Q = *Q + A2->transpose() * ( * MPassage23 );

		//rm->setIntermediateMatrix(N2);
	}


	//invert Q
	Eigen::SimplicialLDLT<TSparseMatrix> chol3( *Q );
	if(chol3.info() != Eigen::Success)
	{
		// colesky did not work, try fullPiv
		typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
		Eigen::FullPivLU<TMat> lu3( *Q );

		if (! lu3.isInvertible()) {
			std::ostringstream foo;
			foo << "TLSCombinedMtdComputer::computeResultsMtrs:\n\tsolution failed.";
			fError += foo.str();
			return false;
		}
		
		//Calculate K3
		TSparseMatrix* W3; //misclV3   TSparseMatrix not TVector
		TSparseMatrix* MPassage4;
		* MPassage4 = *A3 * (lu3.inverse());
		* MPassage4 = * MPassage4 * A3->transpose();
		TSparseMatrix* PartK3;
		*PartK3 = (*W3) -(*A3) * (*PartW1) -(*A3) * (*PartW2) ;
		
		//inverse MPassage4
		Eigen::SimplicialLDLT<TSparseMatrix> chol4( *MPassage4 );
		if(chol4.info() != Eigen::Success)
		{
			// colesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu4( *MPassage4 );
		
			if (! lu4.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedMtdComputer::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return false;
			} 

			//*K3 = lu4.solve( *PartK3);
			//*K3 = -lu4.inverse() * (*A3) * (*PartW1);

		}
		else{
			*K3 = chol4.solve( * PartK3 );
		}
		//End K3

		*(rm->getSolutionVctr()) = -lu3.inverse()* ( A3->transpose() * (*K3) + *PartW1 + *PartW2);
		return true;
		
	}
	else {
		//Calculate K3;

		//TSparseMatrix * MPassage3;
		//* MPassage3 = -A3->transpose() * (*K3); // - *PartW1 - *PartW2;
		//*(rm->getSolutionVctr()) = chol3.solve( *MPassage3 - *PartW1 - *PartW2);

		
		return true;
	}

}

/*void	TLSCombinedMtdComputer::calcResiduAndVarCovMatrice(//LSCalcDataSet& calcDS,
													const TLSInputMatrices* inputMtr,
													TLSResultsMatrices* rm,
													TLSResultsMatricesExtractor* /*,
													TLSParametricMtdComputer* *//*)
{
	if (fError == "")
	{
		int nbUnk = inputMtr->getNbrUnknowns();
		int nbObs = inputMtr->getNbrObservations();
		TReal sigmaZero2 = LITERAL(0.0);


		auto firstDM = inputMtr->getFirstDgnMtrx();
		auto secondDM = inputMtr->getSecondDgnMtrx();
		auto weightM = inputMtr->getWeightMtrx();
		const TVector & misclV = inputMtr->getMisclosureVctr();
		TVector * solution = rm->getSolutionVctr();
		TVector * residuals = rm->getResidualsVctr();

		Eigen::SimplicialLDLT<TSparseMatrix> chol( *weightM );
		TSparseMatrix N1 = (*secondDM) * (*weightM) * (secondDM->transpose());
		Eigen::SimplicialLDLT<TSparseMatrix> chol( N1 );
		#ifdef _DEBUG
			std::cout << "TLSCombinedCalculation::computeResultsMtrs, det(N1)=\n " << chol.determinant() << std::endl;
		#endif
		if(chol.info() != Eigen::Success)
		{
			// cholesky did not work, try fullPiv
			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> TMat;
			Eigen::FullPivLU<TMat> lu(N1);

			if (! lu.isInvertible()) {
				std::ostringstream foo;
				foo << "TLSCombinedCalculation::computeResultsMtrs:\n\tsolution failed.";
				fError += foo.str();
				return;
			}
			rm->setIntermediateMatrix(N1);
			//N1 bien inversé pour residual?
			*residuals = *weightM * secondDM->transpose() * lu.solve( *firstDM * (*solution) + misclV);
		} 
		else
		{
			*residuals = *weightM * secondDM->transpose() * chol.solve(*firstDM * (*solution) + misclV);
		} 
		
		if (!fCalcParams.isFaultDetectToBeSaved())
		{
			delete firstDM;
			delete secondDM;
		}

		if (calcDS.getDimensions().UIndex == calcDS.getDimensions().OIndex)
		{
			sigmaZero2 = 1;
		}
		else
		{
			sigmaZero2 = ( residuals->transpose() * (*weightM) * (*residuals) );
			sigmaZero2 /= (scaleK*scaleK)*(nbObs - nbUnk);
		}

		delete weightM;

		rm->setSigmaZero2(sigmaZero2);

		struct limits fisherLim =calcSigmaZeroLimits(nbObs, nbUnk, sigmaZero2);

		calcDS.setChiUpLimit(fisherLim.s0PostUpLimit);
		calcDS.setChiLoLimit(fisherLim.s0PostLoLimit);

		// setting of the s0 a poseriori boolean
		rm->setS0APosterioriVariances(fS0APosterioriVariances);

		// Setting of covmatrix done in inverse
		TSparseMatrix * unkcov = new TSparseMatrix(rm->getIntermediateMatrix());
		*unkcov /= scaleK*scaleK;

		rm->setUnkCovarMtrx(unkcov);
	}*/
