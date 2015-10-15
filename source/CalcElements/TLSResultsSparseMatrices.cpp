//TLSResultsSparseMatrices.h : header file
// class for result matrices as defined for survey purposes 
// and for the least squares solving algorithm
#include <iomanip>



#include "TLSResultsSparseMatrices.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////
TLSResultsSparseMatrices::TLSResultsSparseMatrices()
{// no argument constructor

	fSolutionVctr = 0;
	fResidualsVctr = 0;
	fSigmaZero2 = LITERAL(0.0);
	fUnknownsCovarianceMtrx = NULL;
	fS0APosterioriVariances = false;
	L = NULL;
	bigMatrix = NULL;
}


TLSResultsSparseMatrices::TLSResultsSparseMatrices(UEOIndices ueoi)
{// constructor dimensioning the matrices
	fSolutionVctr = new TColumnVector(ueoi.UIndex);
	fResidualsVctr = new TColumnVector(ueoi.OIndex);
	fSigmaZero2 = LITERAL(0.0);
	//fUnknownsCovarianceMtrx = new TMatrix(ueoi.UIndex, ueoi.UIndex);
	fUnknownsCovarianceMtrx = NULL;
	fS0APosterioriVariances = false;
	L = NULL;
	bigMatrix = NULL;
}


TLSResultsSparseMatrices::TLSResultsSparseMatrices(TColumnVector* solut, TColumnVector* resid, 
									   TReal sigm2)
{// constructor setting the results

	std::cout<<(double) sigm2<<std::endl<<std::endl;
	fSolutionVctr = new TColumnVector (*solut);
	fResidualsVctr = new TColumnVector (*resid);
	fSigmaZero2 = sigm2;
	//fUnknownsCovarianceMtrx = new TMatrix(*unkcov);
	fUnknownsCovarianceMtrx = NULL;
	fS0APosterioriVariances = false;
	L = NULL;
	bigMatrix = NULL;
}


TLSResultsSparseMatrices::TLSResultsSparseMatrices(int numUnknowns, int numEquations)
{// constructor creating the results matrices with the input dimensions

	fSolutionVctr = new TColumnVector (numUnknowns);
	fResidualsVctr = new TColumnVector (numEquations);
	fSigmaZero2 = LITERAL(0.0);
	//fUnknownsCovarianceMtrx = new TMatrix(numUnknowns,numUnknowns);
	fUnknownsCovarianceMtrx = NULL;
	fS0APosterioriVariances = false;
	L = NULL;
	bigMatrix = NULL;
}


TLSResultsSparseMatrices::~TLSResultsSparseMatrices()
{// destructor
	if(fSolutionVctr != 0)
	{
		delete fSolutionVctr;
	}

	if(fResidualsVctr != 0)
	{
		delete fResidualsVctr;
	}

	if(fUnknownsCovarianceMtrx != 0)
	{
		delete fUnknownsCovarianceMtrx;
	}

}


//////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////

TColumnVector	TLSResultsSparseMatrices::computeVarObs(const TSparseMatrix& A, const TSparseMatrix& ATransposed) 
{
	int nobs = A.rowsCount(); //number of observations
	int nunk = A.columnsCount(); //number of observations
	TReal* vals = new TReal[fUnknownsCovarianceMtrx->columnPointers()[nunk]];
	int* rows = new int[fUnknownsCovarianceMtrx->columnPointers()[nunk]];
	int* cols = new int[nunk + 1];
	cols[0] = 0;
	int count = 0;
	for (int i = 0; i < nunk; i++)
	{
		for (int j = fUnknownsCovarianceMtrx->columnPointers()[i];
			j < fUnknownsCovarianceMtrx->columnPointers()[i + 1] && fUnknownsCovarianceMtrx->rowIndices()[j] < nunk;
			j++)
		{
			vals[count] = fUnknownsCovarianceMtrx->values()[j];
			rows[count++] = fUnknownsCovarianceMtrx->rowIndices()[j];
		}
		cols[i + 1] = count;
	}
	TSparseMatrix* ucm = new TSparseMatrix(nunk, nunk, vals, rows, cols);
	TReal *result = A.multiply_three_returning_diagonal(*ucm, ATransposed);
	delete ucm;
	TColumnVector var(nobs);
	for (int i = 0; i < nobs; i++)
	{
		var(i) = result[i];
	}

	delete result;

	return var;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////////////////////////////////////////

