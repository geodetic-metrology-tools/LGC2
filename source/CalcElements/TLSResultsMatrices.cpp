//TLSResultsMatrices.h : header file
// class for result matrices as defined for survey purposes 
// and for the least squares solving algorithm
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>


#include "lsalgo/TLSResultsMatrices.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
////////////////////////////////////////////////////////////////////////////////////////////////////////

TLSResultsMatrices::TLSResultsMatrices(UEOIndices ueoi)
{// constructor dimensioning the matrices
	fSolutionVctr = new TVector(ueoi.UIndex);
	fResidualsVctr = new TVector(ueoi.OIndex);
	fSigmaZero2 = LITERAL(0.0);
	fResCovarianceMtrx = new TSparseMatrix(ueoi.OIndex, ueoi.OIndex);
	fUnkCovarianceMtrx =new TSparseMatrix(ueoi.UIndex, ueoi.UIndex);
	fS0APosterioriVariances = false;

}


TLSResultsMatrices::TLSResultsMatrices(UEOIndices ueoi, int numConstraints)
{// constructor dimensioning the matrices
	fSolutionVctr = new TVector(ueoi.UIndex);
	fResidualsVctr = new TVector(ueoi.OIndex);
	fSigmaZero2 = LITERAL(0.0);
	fResCovarianceMtrx = new TSparseMatrix(ueoi.OIndex + numConstraints, ueoi.OIndex + numConstraints);
	fUnkCovarianceMtrx = new TSparseMatrix(ueoi.UIndex, ueoi.UIndex);
	fS0APosterioriVariances = false;
}


TLSResultsMatrices::TLSResultsMatrices(int numUnknowns, int numEquations,int numObs)
{// constructor creating the results matrices with the input dimensions

	fSolutionVctr = new TVector (numUnknowns);
	fResidualsVctr = new TVector (numObs);
	fSigmaZero2 = LITERAL(0.0);
	fResCovarianceMtrx = new TSparseMatrix(numObs,numObs);
	fUnkCovarianceMtrx = new TSparseMatrix(numUnknowns,numUnknowns);
	fS0APosterioriVariances = false;

}


TLSResultsMatrices::~TLSResultsMatrices()
{// destructor
	if(fSolutionVctr != 0)
	{
		delete fSolutionVctr;
	}

	if(fResidualsVctr != 0)
	{
		delete fResidualsVctr;
	}

	if(fResCovarianceMtrx != 0)
	{
		delete fResCovarianceMtrx;
	}

	if(fUnkCovarianceMtrx != 0)
	{
		delete fUnkCovarianceMtrx;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
//MEMBER FUNCTION
//////////////////////////////////////////////////////////////////////////////////////////

//TSparseMatrix TLSResultsMatrices::computeCovarObs(const TSparseMatrix& A)
//{
//	return  (A - *fResCovarianceMtrx);
//}

////////////////////////////////////////////////////////////////////////////////////////////////////
//DEBUG METHOD 
////////////////////////////////////////////////////////////////////////////////////////////////////
void TLSResultsMatrices::saveMatricesToFile(int nbIter) const
{//saves the content of the matrices to a text file

	// TODO: fix
	std::ostringstream oss;

	oss << "C:\\temp\\resultsMatrices" << nbIter << ".txt";
	std::string fileName = oss.str();

	std::ofstream of(fileName.c_str(), std::ios::out);
	if (!of){
		std::cout << "Impossible d'ouvrir le fichier C:\\temp\\resultsMatrices.txt" << '\n';
		std::exit (1);
	}

	of << std::setprecision(9);

	of << "Number of Unknowns : " << fUnkCovarianceMtrx->rows() << std::endl;
	of << "Number of Observations : " << fResidualsVctr->size() << std::endl << std::endl;
//	of << "Number of Equations : " << fNbEqn << std::endl << std::endl;

	of << "****************" << std::endl;
	of << "* SIGMA ZERO ^2*" << std::endl;
	of << "****************" << std::endl << std::endl;
	of << fSigmaZero2 << std::endl <<std::endl << std::endl;



	of << "*******************" << std::endl;
	of << "* SOLUTION VECTOR *" << std::endl;
	of << "*******************" << std::endl << std::endl;
	
	of << *fSolutionVctr << std::endl;
	of << std::endl << std::endl;

	of << "******************************" << std::endl;
	of << "* UNKNOWNS COVARIANCE MATRIX *" << std::endl;
	of << "******************************" << std::endl << std::endl;

	of<< *fUnkCovarianceMtrx << std::endl;
	of << std::endl << std::endl;

	of << "********************" << std::endl;
	of << "* RESIDUALS VECTOR *" << std::endl;
	of << "********************" << std::endl << std::endl;

	of << *fResidualsVctr << std::endl;
	of << std::endl << std::endl;


	of << "******************************" << std::endl;
	of << "* RESIDUALS COVARIANCE MATRIX *" << std::endl;
	of << "******************************" << std::endl << std::endl;

	of<< *fResCovarianceMtrx << std::endl;
	of << std::endl << std::endl;

	of.close();
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////////////////////////////////////////////////