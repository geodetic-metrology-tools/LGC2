//
// TLSSparseMatrixComputer.cpp : implementation file
// class for a least squares computer producing the results matrices 
// using the parametric method (case where 2nd dgn mtrx = -I)
//


#include "TLSSparseMatrixComputer.h"
#include "TLSResultsSparseMatrices.h"
#include "TLSInputSparseMatrices.h"

//////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
//////////////////////////////////////////////////////////
TLSSparseMatrixComputer::TLSSparseMatrixComputer():
fError("")/*, fS0PostUpLimit(LITERAL(0.0)), fS0PostLoLimit(LITERAL(0.0)), fSigmaZero2(LITERAL(0.0))*/
{//default constructor
	count = 1;
}


TLSSparseMatrixComputer::~TLSSparseMatrixComputer()
{//destructor
}



bool TLSSparseMatrixComputer::computeResults(TLSInputSparseMatrices* im , TLSResultsSparseMatrices* rm, bool isCombinedCase, bool isFreeNetworkOrECHO, bool hasConstraints)
{
	bool result;
	//int nbCnstr = im->getNbrConstraints();
	if(rm->getSolutionVctr()->dimension() != 0)
	{
		if(isFreeNetworkOrECHO || hasConstraints)
		{
			result = computeFreeOrConstrainedResultsMtrs( im, rm, isCombinedCase, isFreeNetworkOrECHO);
		}
		else
		{
			result = computeResultsMtrs(im, rm, isCombinedCase);
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
bool TLSSparseMatrixComputer::computeResultsMtrs(TLSInputSparseMatrices* im, TLSResultsSparseMatrices* rm, bool isCombinedCase)
{
	/* in this method, the solution is computed with a numeric equations solver method (nagc lib).
	   The unknown variance-covariance matrix is thus not computed here. 
	   The fAtPAInv attribute contains the AtPA product, and is updated during each iteration. 
	   It is finally inverted outside the method, when there are no iterations left to 
	   be performed (in TLSCalculation). */

	if (isCombinedCase)
	{
		const TSparseMatrix* firstDMTransposed = im->getFirstDgnMtrxTransposed();
		const TSparseMatrix* secondDMTransposed = im->getSecondDgnMtrxTransposed();
		TSparseMatrix* weightMInversed = im->getWeightMtrx()->invert_diagonal_matrix();
		im->setWeightMatrixInverted(weightMInversed);
		const TColumnVector& misclV = im->getMisclosureVctr();
		
		TSparseMatrix* firstDM = firstDMTransposed->transposed();
		im->setFirstDesignMatrix(firstDM);
		TSparseMatrix* secondDM = secondDMTransposed->transposed();
		TSparseMatrix* bTimesWInvTimesBTrans = secondDM->multiply_three_returning_lower_triangular_F(*weightMInversed, *secondDMTransposed);
        delete secondDM;

		TSparseMatrix* bTimesWInvTimesBTransInverted = bTimesWInvTimesBTrans->symmetric_lower_inverse();
		delete bTimesWInvTimesBTrans;
		if (bTimesWInvTimesBTransInverted == NULL)
		{
			return false;
		}
		im->setBTimesWInvTimesBTransInverted(bTimesWInvTimesBTransInverted);

		TSparseMatrix* aTransTimesBTimesWInvTimesBTransInverted = firstDMTransposed->multiply_F(*bTimesWInvTimesBTransInverted);

		TSparseMatrix* solutionMatrixA = aTransTimesBTimesWInvTimesBTransInverted->multiply_returning_lower_triangular_F(*firstDM);

		if (rm->getL() != NULL)
		{
			delete rm->getL();
		}
		TSparseMatrix* L = solutionMatrixA->cholesky_decompose_lower_triangular_returning_lower_triangular();
		delete solutionMatrixA;

		if (L == NULL)
		{
			delete aTransTimesBTimesWInvTimesBTransInverted;
			return false; // Matrix is not positive definite
		}

		rm->setL(L);

		TReal* solutionVectorb = *aTransTimesBTimesWInvTimesBTransInverted * misclV;
		for (int i = 0; i < aTransTimesBTimesWInvTimesBTransInverted->rowsCount(); i++)
		{
			solutionVectorb[i] = -solutionVectorb[i];
		}
		delete aTransTimesBTimesWInvTimesBTransInverted;

		TReal* solution = L->solve_eqn(solutionVectorb);

		delete[] solutionVectorb;

		TColumnVector* solutionVector = rm->getSolutionVctr();
		*solutionVector = TColumnVector(im->getNbrUnknowns());
		for (int i = 0; i < solutionVector->dimension(); i++)
		{
			(*solutionVector)(i) = solution[i];
		}

		delete[] solution;
	}
	else
	{
		const TSparseMatrix* firstDMTransposed = im->getFirstDgnMtrxTransposed();
		const TSparseMatrix* weightM = im->getWeightMtrx();
		const TColumnVector& misclV = im->getMisclosureVctr();
		
		TSparseMatrix* firstDM = firstDMTransposed->transposed();
		im->setFirstDesignMatrix(firstDM);
		TSparseMatrix* aTransTimesW = firstDMTransposed->multiply_F(*weightM);

		TSparseMatrix* fAtPA = aTransTimesW->multiply_returning_lower_triangular_F(*firstDM);
#if _DEBUG
		firstDM->write_matrix_file("C:\\A.txt");
		aTransTimesW->write_matrix_file("C:\\AtP.txt");
		fAtPA->write_matrix_file("C:\\AtPA.txt");
		for (int i = 0; i < misclV.dimension(); i++)
		{
			printf("%.20e\n", (double) misclV(i));
		}
#endif

		if (rm->getL() != NULL)
		{
			delete rm->getL();
		}
		TSparseMatrix* L = fAtPA->cholesky_decompose_lower_triangular_returning_lower_triangular();
		delete fAtPA;

		if (L == NULL)
		{
			return false; // Matrix is not positive definite
		}

		rm->setL(L);

		TReal* solutionVectorb = *aTransTimesW * misclV;

		for (int i = 0; i < aTransTimesW->rowsCount(); i++)
		{
			solutionVectorb[i] = -solutionVectorb[i];
#if _DEBUG
			printf("%.20e\n", (double) solutionVectorb[i]);
#endif
		}
		delete aTransTimesW;

		TReal* solution = L->solve_eqn(solutionVectorb);

		delete[] solutionVectorb;

		TColumnVector* solutionVector = rm->getSolutionVctr();
		*solutionVector = TColumnVector(im->getNbrUnknowns());
		for (int i = 0; i < solutionVector->dimension(); i++)
		{
			(*solutionVector)(i) = solution[i];
		}

		delete[] solution;
	}

	return true;
}


////////////////////////////////////////////////////////////////
//COMPUTES THE RESULTS MATRICES FOR FREE CALCULATION
////////////////////////////////////////////////////////////////
bool TLSSparseMatrixComputer::computeFreeOrConstrainedResultsMtrs(TLSInputSparseMatrices* im, TLSResultsSparseMatrices* rm, bool isCombinedCase, bool isFreeNetworkOrECHO)
{
	if (isCombinedCase)
	{
		if (isFreeNetworkOrECHO)
		{
			const TSparseMatrix* firstDMTransposed = im->getFirstDgnMtrxTransposed();
			const TSparseMatrix* secondDMTransposed = im->getSecondDgnMtrxTransposed();
			const TSparseMatrix* constraintFirstDM = im->getCnstrFirstDgnMtrx();
			TSparseMatrix* weightMInversed = im->getWeightMtrx()->invert_diagonal_matrix();
			im->setWeightMatrixInverted(weightMInversed);
			const TColumnVector& misclV = im->getMisclosureVctr();
			const TColumnVector& constraintMisclV = im->getCnstrMisclosureVctr();
			
			TSparseMatrix* firstDM = firstDMTransposed->transposed();
			im->setFirstDesignMatrix(firstDM);
			TSparseMatrix* secondDM = secondDMTransposed->transposed();		
			// TODO: multiplying three matrices is slower than twice two matrices - probably should change that
			TSparseMatrix* bTimesWInvTimesBTrans =
				secondDM->multiply_three_returning_lower_triangular_F(*weightMInversed, *secondDMTransposed);
			delete secondDM;

			TSparseMatrix* bTimesWInvTimesBTransInverted = bTimesWInvTimesBTrans->symmetric_lower_inverse();
			delete bTimesWInvTimesBTrans;
			im->setBTimesWInvTimesBTransInverted(bTimesWInvTimesBTransInverted);

			TSparseMatrix* aTransTimesBTimesWInvTimesBTransInverted = firstDMTransposed->multiply_F(*bTimesWInvTimesBTransInverted);

			TSparseMatrix* temp = aTransTimesBTimesWInvTimesBTransInverted->multiply_returning_lower_triangular_F(*firstDM);
	        
			TReal* aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec =
				*aTransTimesBTimesWInvTimesBTransInverted * misclV;
			int solVecRows = aTransTimesBTimesWInvTimesBTransInverted->rowsCount();
			delete aTransTimesBTimesWInvTimesBTransInverted;
			
			int nnz = temp->columnPointers()[temp->columnsCount()] + constraintFirstDM->columnPointers()[constraintFirstDM->columnsCount()];
			int cols = temp->columnsCount() + constraintFirstDM->rowsCount();
			TReal* bigValues = new TReal[nnz];
			int* bigRowind = new int[nnz];
			int* bigColptr = new int[cols + 1];
			bigColptr[0] = 0;

			// computing the "big" matrix
			int count = 0;
			for (int i = 0; i < temp->columnsCount(); i++)
			{
				for (int j = temp->columnPointers()[i]; j < temp->columnPointers()[i + 1]; j++)
				{
					bigValues[count] = temp->values()[j];
					bigRowind[count++] = temp->rowIndices()[j];
				}
				for (int j = constraintFirstDM->columnPointers()[i]; j < constraintFirstDM->columnPointers()[i + 1]; j++)
				{
					bigValues[count] = constraintFirstDM->values()[j];
					bigRowind[count++] = constraintFirstDM->rowIndices()[j] + temp->rowsCount();
				}
				bigColptr[i + 1] = count;
			}
			for (int i = temp->columnsCount() + 1; i <= cols; i++)
			{
				bigColptr[i] = count;
			}
			delete temp;

			TSparseMatrix* bigMatrix = new TSparseMatrix(cols, cols,
					bigValues, bigRowind, bigColptr);
	#if _DEBUG
			bigMatrix->write_matrix_file("C:\\big.txt");
	#endif

			TReal* bigSolutionVector = new TReal[cols];
			for (int i = 0; i < solVecRows; i++)
			{
				bigSolutionVector[i] = -aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec[i];
			}
			for (int i = solVecRows, j = 0; j < constraintMisclV.dimension(); i++, j++)
			{
				bigSolutionVector[i] = -constraintMisclV(j);
			}
			delete[] aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec;

			if (rm->getBigMatrix() != NULL)
			{
				delete rm->getBigMatrix();
			}
			TReal* D;
			TSparseMatrix* ldlt = bigMatrix->ldlt_decompose_lower_triangular_returning_lower_triangular(D);
			if (ldlt == NULL)
			{
				delete bigMatrix;
				delete[] bigSolutionVector;
				return false;
			}
			rm->setBigMatrix(bigMatrix);

			TReal* solution = ldlt->solve_ldlt(D, bigSolutionVector);
			delete[] bigSolutionVector;

			TColumnVector* s = rm->getSolutionVctr();
			for (int i = 0; i < s->dimension(); i++)
			{
				(*s)(i) = solution[i];
			}

			delete[] solution;
		}
		else
		{
			const TSparseMatrix* firstDMTransposed = im->getFirstDgnMtrxTransposed();
			const TSparseMatrix* secondDMTransposed = im->getSecondDgnMtrxTransposed();
			const TSparseMatrix* constraintFirstDM = im->getCnstrFirstDgnMtrx();
			TSparseMatrix* weightMInversed = im->getWeightMtrx()->invert_diagonal_matrix();
			im->setWeightMatrixInverted(weightMInversed);
			const TColumnVector& misclV = im->getMisclosureVctr();
			const TColumnVector& constraintMisclV = im->getCnstrMisclosureVctr();
			
			TSparseMatrix* firstDM = firstDMTransposed->transposed();
			im->setFirstDesignMatrix(firstDM);
			TSparseMatrix* secondDM = secondDMTransposed->transposed();		
			TSparseMatrix* constraintFirstDMTransposed = constraintFirstDM->transposed();
			// TODO: multiplying three matrices is slower than twice two matrices - probably should change that
			TSparseMatrix* bTimesWInvTimesBTrans =
				secondDM->multiply_three_returning_lower_triangular_F(*weightMInversed, *secondDMTransposed);
			delete secondDM;

			TSparseMatrix* bTimesWInvTimesBTransInverted = bTimesWInvTimesBTrans->symmetric_lower_inverse();
			delete bTimesWInvTimesBTrans;
			if (bTimesWInvTimesBTransInverted == NULL)
			{
				return false;
			}
			im->setBTimesWInvTimesBTransInverted(bTimesWInvTimesBTransInverted);

			TSparseMatrix* aTransTimesBTimesWInvTimesBTransInverted = firstDMTransposed->multiply_F(*bTimesWInvTimesBTransInverted);

			TSparseMatrix* temp = aTransTimesBTimesWInvTimesBTransInverted->multiply_returning_lower_triangular_F(*firstDM);
	        
			TReal* aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec =
				*aTransTimesBTimesWInvTimesBTransInverted * misclV;
			int solVecRows = aTransTimesBTimesWInvTimesBTransInverted->rowsCount();
			delete aTransTimesBTimesWInvTimesBTransInverted;

			temp->write_matrix_file("C:\\aTransTimesBTimesWInvTimesBTransInvertedTimesA.txt");

			if (rm->getL() != NULL)
			{
				delete rm->getL();
			}
			TSparseMatrix* decomposed = temp->cholesky_decompose_lower_triangular_returning_lower_triangular();
			delete temp;

			if (decomposed == NULL)
			{
				delete[] aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec;
				return false;
			}
			rm->setL(decomposed);
			TSparseMatrix* aTransTimesBTimesWInvTimesBTransInvertedTimesAInverted = decomposed->invert_lower_triangular_cholesky_decomposed();
			TSparseMatrix* cstrATimesATransTimesBTimesWInvTimesBTransInvertedTimesAInverted =
				constraintFirstDM->multiply_F(*aTransTimesBTimesWInvTimesBTransInvertedTimesAInverted);

			TSparseMatrix* solutionMatrixA = cstrATimesATransTimesBTimesWInvTimesBTransInvertedTimesAInverted->
					multiply_returning_lower_triangular_F(*constraintFirstDMTransposed);

			decomposed = solutionMatrixA->cholesky_decompose_lower_triangular_returning_lower_triangular();

			delete solutionMatrixA;
			if (decomposed == NULL)
			{
				delete cstrATimesATransTimesBTimesWInvTimesBTransInvertedTimesAInverted;
				return false;
			}

			TReal* solutionVectorb = *cstrATimesATransTimesBTimesWInvTimesBTransInvertedTimesAInverted *
				aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec;
			delete cstrATimesATransTimesBTimesWInvTimesBTransInvertedTimesAInverted;

			for (int i = 0; i < constraintMisclV.dimension(); i++)
			{
				solutionVectorb[i] = constraintMisclV(i) - solutionVectorb[i];
			}

			TReal* solution = decomposed->solve_eqn(solutionVectorb);
			delete[] solutionVectorb;

			solutionVectorb = *constraintFirstDMTransposed * solution;
			delete[] solution;
			
			for (int i = 0; i < constraintFirstDMTransposed->rowsCount(); i++)
			{
				solutionVectorb[i] = -solutionVectorb[i] - aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec[i];
			}
			delete constraintFirstDMTransposed;
			delete[] aTransTimesBTimesWInvTimesBTransInvertedTimesMiscVec;

			solution = *aTransTimesBTimesWInvTimesBTransInvertedTimesAInverted * solutionVectorb;
			delete[] solutionVectorb;
			delete aTransTimesBTimesWInvTimesBTransInvertedTimesAInverted;

			TColumnVector* s = rm->getSolutionVctr();
			for (int i = 0; i < s->dimension(); i++)
			{
				(*s)(i) = solution[i];
			}

			delete[] solution;
		}
	}
	else
    {
		if (isFreeNetworkOrECHO)
		{
			const TSparseMatrix* firstDMTransposed = im->getFirstDgnMtrxTransposed();
			const TSparseMatrix* constraintFirstDM = im->getCnstrFirstDgnMtrx();
			const TSparseMatrix* weightM = im->getWeightMtrx();
			const TColumnVector& misclV = im->getMisclosureVctr();
			const TColumnVector& constraintMisclV = im->getCnstrMisclosureVctr();

			TSparseMatrix* firstDM = firstDMTransposed->transposed();
			im->setFirstDesignMatrix(firstDM);

			TSparseMatrix* aTransTimesW = firstDMTransposed->multiply_F(*weightM);

			TSparseMatrix* temp = aTransTimesW->multiply_returning_lower_triangular_F(*firstDM);
			TReal* aTransTimesWTimesMiscVec = *aTransTimesW * misclV;
			int solVecRows = aTransTimesW->rowsCount();

	#if _DEBUG
			firstDM->write_matrix_file("C:\\A.txt");
			aTransTimesW->write_matrix_file("C:\\AtP.txt");
			temp->write_matrix_file("C:\\AtPA.txt");
			constraintFirstDM->write_matrix_file("C:\\C.txt");

			for (int i = 0; i < misclV.dimension(); i++)
			{
				printf("%.20e\n", (double) misclV(i));
			}
	#endif
			delete aTransTimesW;

			int nnz = temp->columnPointers()[temp->columnsCount()] + constraintFirstDM->columnPointers()[constraintFirstDM->columnsCount()];
			int cols = temp->columnsCount() + constraintFirstDM->rowsCount();
			TReal* bigValues = new TReal[nnz];
			int* bigRowind = new int[nnz];
			int* bigColptr = new int[cols + 1];
			bigColptr[0] = 0;

			// computing the "big" matrix
			int count = 0;
			for (int i = 0; i < temp->columnsCount(); i++)
			{
				for (int j = temp->columnPointers()[i]; j < temp->columnPointers()[i + 1]; j++)
				{
					bigValues[count] = temp->values()[j];
					bigRowind[count++] = temp->rowIndices()[j];
				}
				for (int j = constraintFirstDM->columnPointers()[i]; j < constraintFirstDM->columnPointers()[i + 1]; j++)
				{
					bigValues[count] = constraintFirstDM->values()[j];
					bigRowind[count++] = constraintFirstDM->rowIndices()[j] + temp->rowsCount();
				}
				bigColptr[i + 1] = count;
			}
			for (int i = temp->columnsCount() + 1; i <= cols; i++)
			{
				bigColptr[i] = count;
			}
			delete temp;

			TSparseMatrix* bigMatrix = new TSparseMatrix(cols, cols,
					bigValues, bigRowind, bigColptr);
	#if _DEBUG
			bigMatrix->write_matrix_file("C:\\big.txt");
	#endif

			TReal* bigSolutionVector = new TReal[cols];
			for (int i = 0; i < solVecRows; i++)
			{
				bigSolutionVector[i] = -aTransTimesWTimesMiscVec[i];
			}
			for (int i = solVecRows, j = 0; j < constraintMisclV.dimension(); i++, j++)
			{
				bigSolutionVector[i] = -constraintMisclV(j);
			}
			delete[] aTransTimesWTimesMiscVec;

			if (rm->getBigMatrix() != NULL)
			{
				delete rm->getBigMatrix();
			}
			TReal* D;
			TSparseMatrix* ldlt = bigMatrix->ldlt_decompose_lower_triangular_returning_lower_triangular(D);
			if (ldlt == NULL)
			{
				delete bigMatrix;
				delete[] bigSolutionVector;
				return false;
			}
			rm->setBigMatrix(bigMatrix);

			TReal* solution = ldlt->solve_ldlt(D, bigSolutionVector);
			delete[] bigSolutionVector;

			TColumnVector* s = rm->getSolutionVctr();
			for (int i = 0; i < s->dimension(); i++)
			{
				(*s)(i) = solution[i];
			}

			delete[] solution;
		}
		else
		{
			const TSparseMatrix* firstDMTransposed = im->getFirstDgnMtrxTransposed();
			const TSparseMatrix* constraintFirstDM = im->getCnstrFirstDgnMtrx();
			const TSparseMatrix* weightM = im->getWeightMtrx();
			const TColumnVector& misclV = im->getMisclosureVctr();
			const TColumnVector& constraintMisclV = im->getCnstrMisclosureVctr();
			
			TSparseMatrix* firstDM = firstDMTransposed->transposed();
			im->setFirstDesignMatrix(firstDM);
			TSparseMatrix* constraintFirstDMTransposed = constraintFirstDM->transposed();
			
			TSparseMatrix* aTransTimesW = firstDMTransposed->multiply_F(*weightM);

			TSparseMatrix* temp = aTransTimesW->multiply_returning_lower_triangular_F(*firstDM);
	        
			TReal* aTransTimesWTimesMiscVec = *aTransTimesW * misclV;
			int solVecRows = aTransTimesW->rowsCount();
            delete aTransTimesW;

			TSparseMatrix* aTransTimesWTimesAInverted = temp->symmetric_lower_inverse();
			delete temp;
			if (aTransTimesWTimesAInverted == NULL)
			{
				delete[] aTransTimesWTimesMiscVec;
				return false;
			}
			
			TSparseMatrix* cstrATimesATransTimesWTimesAInverted =
				constraintFirstDM->multiply_F(*aTransTimesWTimesAInverted);

			TSparseMatrix* solutionMatrixA = cstrATimesATransTimesWTimesAInverted->
					multiply_returning_lower_triangular_F(*constraintFirstDMTransposed);
			
			if (rm->getL() != NULL)
			{
				delete rm->getL();
			}
			TSparseMatrix* decomposed = solutionMatrixA->cholesky_decompose_lower_triangular_returning_lower_triangular();
			delete solutionMatrixA;
			if (decomposed == NULL)
			{
				delete cstrATimesATransTimesWTimesAInverted;
				return false;
			}
			rm->setL(decomposed);

			TReal* solutionVectorb = *cstrATimesATransTimesWTimesAInverted *
				aTransTimesWTimesMiscVec;
			delete cstrATimesATransTimesWTimesAInverted;

			for (int i = 0; i < constraintMisclV.dimension(); i++)
			{
				solutionVectorb[i] = constraintMisclV(i) - solutionVectorb[i];
			}

			TReal* solution = decomposed->solve_eqn(solutionVectorb);
			delete[] solutionVectorb;

			solutionVectorb = *constraintFirstDMTransposed * solution;
			delete[] solution;
			
			for (int i = 0; i < constraintFirstDMTransposed->rowsCount(); i++)
			{
				solutionVectorb[i] = -solutionVectorb[i] - aTransTimesWTimesMiscVec[i];
			}
			delete constraintFirstDMTransposed;
			delete[] aTransTimesWTimesMiscVec;

			solution = *aTransTimesWTimesAInverted * solutionVectorb;
			delete[] solutionVectorb;
			delete aTransTimesWTimesAInverted;

			TColumnVector* s = rm->getSolutionVctr();
			for (int i = 0; i < s->dimension(); i++)
			{
				(*s)(i) = solution[i];
			}

			delete[] solution;
		}
    }

	return true;
}



////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////
