#ifndef SU_TLSResultsMatrices
#define SU_TLSResultsMatrices

#include "TSparseMatrix.h"
#include "UEOIndices.h"


/*!
	\ingroup CalcElements
	\brief Class for result matrices of a least squares calculation.
*/
class TLSResultsMatrices{

public:

	/*!@name Constructors / Destructor*/
	//@{
	TLSResultsMatrices(int numUnknowns, int numEquations, int numObs);


	/*! 
		\brief Constructor

		\param[in] ueoi an object holding the number of unknowns, equations and observations,
	thus enabling the dimensioning of the matrices (set to zero)
	*/
	TLSResultsMatrices(UEOIndices ueoi);

	/*!
		\brief Constructor

		\param[in] ueoi an object holding the number of unknowns, equations and observations,
		thus enabling the dimensioning of the matrices (set to zero)
		\param[in] numConstraints indicates the number of constraints added for a free network
		calculation
	*/
	TLSResultsMatrices(UEOIndices ueoi, int numConstraints);

	//!Destructor
	virtual ~TLSResultsMatrices();
	//@}

	/*!@name Access methods*/
	//@{
	/*! 
		\brief Returns an element of the solution vector

		\param[in] row the desired element's index
	*/
	virtual MatrixElmt	getSolutionVctrElmt(MatrixIndex row) const {return (*fSolutionVctr)(row);}

	/*!
		\brief Returns an element of the residuals vector

		\param[in] row the desired element's index*/
	virtual MatrixElmt	getResidualsVctrElmt(MatrixIndex row) const {return (*fResidualsVctr)(row);}

	/*!\brief Returns an element of the residuals covariance matrix

	\param[in] row the desired element's row index
	\param[in] column the desired element's column index*/
	virtual MatrixElmt	getResCovarMtrxElmt(MatrixIndex row, MatrixIndex column) const {return (*fResCovarianceMtrx).coeff(row, column);}
	
	/*!\brief Returns an element of the observations covariance matrix

	\param[in] row the desired element's row index
	\param[in] column the desired element's column index*/
	virtual MatrixElmt	getUnkCovarMtrxElmt(MatrixIndex row, MatrixIndex column) const {return (*fUnkCovarianceMtrx).coeff(row, column);}

	/// Returns a pointer to the Solution Vector
	virtual TVector*		getSolutionVctr() const {return fSolutionVctr;}

	/// Returns a pointer to the Residuals Vector
	virtual TVector*		getResidualsVctr() const {return fResidualsVctr;}

	/// Returns a pointer to the residuals covariance matrix
	virtual TSparseMatrix*		getResCovarMtrx() const {return fResCovarianceMtrx;}

	/// Returns a pointer to the unknowns covariance matrix
	virtual TSparseMatrix*		getUnkCovarMtrx() const {return fUnkCovarianceMtrx;}

	/// Returns  the squared sigma zero 
	virtual TReal				getSigmaZero2() const { return fSigmaZero2; }
	//@}

	/// Sets the sigma zero 
	virtual void			setSigmaZero2(TReal s){ fSigmaZero2 = s;}

	/// Sets the residuals covariance matrix 
	virtual void				setResCovarMtrx(TSparseMatrix* m) { fResCovarianceMtrx = m; }

	/// Sets the unknowns covariance matrix 
	virtual void	setUnkCovarMtrx(TSparseMatrix* m) { fUnkCovarianceMtrx = m; }

	/// Save the results matrices in a file 
	void			saveMatricesToFile(int nbIter) const;
	
	/// Returns the normal matrix
	const TSparseMatrix & getIntermediateMatrix() const {return fIntermediateMatrix;}
	/*! 
		\brief  Set an intermediate matirx

		This intermediate matrix is the normal matrix, which is used to calculated the covariance matrices. 
	*/
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

	TSparseMatrix fIntermediateMatrix;
};

#endif