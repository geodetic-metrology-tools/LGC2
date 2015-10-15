#ifndef SU_TLSResultsMatrices
#define SU_TLSResultsMatrices


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////

#include "TSparseMatrix.h"
#include "UEOIndices.h"


//!Class for result matrices of a least squares calculation
class TLSResultsMatrices{

public:

	/*!@name Constructors / Destructor*/
	//@{
	TLSResultsMatrices(int numUnknowns, int numEquations, int numObs);

	//!Constructor
	/*!@param ueoi an object holding the number of unknowns, equations and observations,
	thus enabling the dimensioning of the matrices (set to zero)*/
	TLSResultsMatrices(UEOIndices ueoi);

	//!Constructor
	/*	!@param ueoi an object holding the number of unknowns, equations and observations,
		thus enabling the dimensioning of the matrices (set to zero)
		!@param numConstraints indicates the number of constraints added for a free network
		calculation
	*/
	TLSResultsMatrices(UEOIndices ueoi, int numConstraints);

	//!Destructor
	virtual ~TLSResultsMatrices();
	//@}

	/*!@name Access methods*/
	//@{
	/*!@return an element of the solution vector
	@param row the desired element's index*/
	virtual MatrixElmt	getSolutionVctrElmt(MatrixIndex row) const {return (*fSolutionVctr)(row);}

	/*!@return an element of the residuals vector
	@param row the desired element's index*/
	virtual MatrixElmt	getResidualsVctrElmt(MatrixIndex row) const {return (*fResidualsVctr)(row);}

	/*!@return an element of the residuals covariance matrix
	@param row the desired element's row index
	@param column the desired element's column index*/
	virtual MatrixElmt	getResCovarMtrxElmt(MatrixIndex row, MatrixIndex column) const {return (*fResCovarianceMtrx).coeff(row, column);}
	
	/*!@return an element of the observations covariance matrix
	@param row the desired element's row index
	@param column the desired element's column index*/
	virtual MatrixElmt	getUnkCovarMtrxElmt(MatrixIndex row, MatrixIndex column) const {return (*fUnkCovarianceMtrx).coeff(row, column);}

	/*!@return a pointer to the Solution Vector*/
	virtual TVector*		getSolutionVctr() const {return fSolutionVctr;}

	/*!@return a pointer to the Residuals Vector*/
	virtual TVector*		getResidualsVctr() const {return fResidualsVctr;}

	/*!@return a pointer to the residuals covariance matrix*/
	virtual TSparseMatrix*		getResCovarMtrx() const {return fResCovarianceMtrx;}

	/*!@return a pointer to the observations covariance matrix*/
	virtual TSparseMatrix*		getUnkCovarMtrx() const {return fUnkCovarianceMtrx;}

	/*!@return the boolean indicating if sigma zero should be applied or not */
	virtual bool				S0APosterioriVariances() const {return fS0APosterioriVariances;}

	/*!@return  the squared sigma zero */
	virtual TReal				getSigmaZero2() const { return fSigmaZero2; }
	//@}

	/*! sets the sigma zero */
	virtual void			setSigmaZero2(TReal s);

	/*! sets the residuals covariance matrix */
	virtual void				setResCovarMtrx(TSparseMatrix* m) { fResCovarianceMtrx = m; }

	/*! sets the unknowns covariance matrix */
	virtual void	setUnkCovarMtrx(TSparseMatrix* m) { fUnkCovarianceMtrx = m; }

	/*! sets the boolean */
	virtual void			setS0APosterioriVariances(bool s0) {fS0APosterioriVariances = s0; return;}

	/*! Save the results matrices in a file */
	void			saveMatricesToFile(int nbIter) const;

	/*! compute and return a vector of observations variances */
	//TSparseMatrix computeCovarObs(const TSparseMatrix& A);
	

	const TSparseMatrix & getIntermediateMatrix() const {return fIntermediateMatrix;}
	void setIntermediateMatrix(const TSparseMatrix & matrix) { fIntermediateMatrix = matrix; }

	void setSigmaZero2Limits(TReal loLimit, TReal upLimit) {
		fSigmaZero2LowLimit =  loLimit; 
		fSigmaZero2UpLimit = upLimit;
	}

	TReal getSigmaZeroLowLimit() const {return fSigmaZero2LowLimit;}
	TReal getSigmaZeroUpLimit() const {return fSigmaZero2UpLimit;}

private:

	TVector*	fSolutionVctr; /*!< vector (u x 1) containing the calculated parameters */
	TVector*	fResidualsVctr; /*!< vector (o x 1) containing the calculated residues on observations */
	TSparseMatrix*	fResCovarianceMtrx; /*!< matrix (o x o) containing the variances and covariances for residuals */
	TSparseMatrix*	fUnkCovarianceMtrx; /*!< matrix (u x u) containing the variances and covariances for unknowns */
	TReal			fSigmaZero2; /*!< calculated "average variance" on residues */

	TReal			fSigmaZero2LowLimit;
	TReal			fSigmaZero2UpLimit;

	bool			fS0APosterioriVariances; /*!< indicates if the s0 a posteriori should be applied */

	TSparseMatrix fIntermediateMatrix;
};

/////////////////////
// Inline Definitions
/////////////////////
inline void TLSResultsMatrices::setSigmaZero2(TReal s){ fSigmaZero2 = s;}

#endif