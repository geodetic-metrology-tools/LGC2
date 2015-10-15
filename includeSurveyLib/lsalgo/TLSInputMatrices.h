#ifndef SU_TLSINPUTMATRICES
#define SU_TLSINPUTMATRICES

#include "TSparseMatrix.h"
#include "UEOIndices.h"


/*!
	\ingroup CalcElements

	\brief Class for the input matrices used in the LS-calculation.
*/
class TLSInputMatrices{

public:

	/*!@name Constructors / Destructor*/
	//@{
		//!Constructor
		TLSInputMatrices();
		//!Destructor
		~TLSInputMatrices();
	//@}


	/*!@name Set methods*/
	//@{
		/*!
			\brief Sets the dimensions of the matrices

			\param[in] nbUnknowns the survey network's number of unknowns
			\param[in] nbEquations the survey network's number of equations
			\param[in] nbObservations the survey network's number of observations
			\param[in] nbObservations the survey network's number of observation constraints
		*/
		virtual void setDimensions(int nbUnknowns, int nbEquations, int nbObservations, int nbCnstrObs);
	
		/*! \brief Sets the dimensions of the matrices
			
			\param[in] nbUnknowns the survey network's number of unknowns
			\param[in] nbEquations the survey network's number of equations
			\param[in] nbObservations the survey network's number of observations
			\param[in] nbConstraints the free survey network's number of observation constraints
			\param[in] constraints the free survey network's number of constraints
		*/
		virtual void setDimensions(int unknowns, int equations, int observations, int nbCnstrObs, int constraints);
	
		//!Sets a coefficient of the first design matrix
		virtual bool setFirstDgnMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the second design matrix
		virtual bool setSecondDgnMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the misclosure vector
		virtual bool setMisclosureVectorElement(MatrixIndex row, TReal coeff);
		//!Sets a coefficient of the weight matrix
		virtual bool setWeightMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the inverted weight matrix
		virtual bool setWeightInvMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the weight matrix for the unknowns
		virtual bool setWeightUnkMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the constraint first design matrix 
		virtual bool setCnstrFirstDgnMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the constraint misclosure vector
		virtual bool setCnstrMisclosureVectorElement(MatrixIndex row, TReal coeff);
	//@}

	 /// Returns the number of unknowns
	 virtual int		getNbrUnknowns() const;
	 /// Returns the number of observations
	 virtual int		getNbrObservations() const;
	 /// Returns the number of equations
	 virtual int		getNbrEquations()const;
	 /// Returns the number of observations constraints
	 virtual int		getNbrConstraintObs()const;
	 /// Returns the number of constraints
	 virtual int		getNbrConstraints()const;


	/// Returns a const reference to the first design matrix
	const TSparseMatrix* getFirstDgnMtrx() const;
	/// Returns a const reference to the second design matrix
	const TSparseMatrix* getSecondDgnMtrx() const;
	/// Returns a const reference to the weight matrix
	const TSparseMatrix* getWeightMtrx() const;
	/// Returns a const reference to the inverted weight matrix
	const TSparseMatrix* getWeightInvMtrx() const;
	/// Returns a const reference to the weight matrix for the unknowns
	const TSparseMatrix* getWeightUnkMtrx() const;
	/// Returns a const reference to the misclosure vector
	const TVector&	getMisclosureVctr() const;
	/// Returns a const reference to the constraint first design matrix
	const TSparseMatrix*	getCnstrFirstDgnMtrx() const;
	/// Returns a const reference to the constraint misclosure vector
	const TVector&	getCnstrMisclosureVctr() const;
	/// Debug method
	void saveMatricesToFile(int nbIter) const;

private:

	MatrixIndex		fNbUnk; /*!< number of unknowns: u */
	MatrixIndex		fNbObs; /*!< number of observations: o */
	MatrixIndex		fNbEqn; /*!< number of equations: e */
	int fNbCnstrObs; /*!< number of constraint observations */
	MatrixIndex		fNbCnstr;/*!<number of constraint: c */


	TVector*	fMisclosureVector; /*!< vector (u) for misclosure errors */

	TSparseMatrix*	fCnstrFirstDesignMtrx; /*!< matrix (u x c) for the parametric part of the model */
	TVector*	fCnstrMisclosureVector; /*!< vector for misclosure errors */

	TSparseMatrix*	firstDesignMatrix; /*!< matrix (u x e) for the parametric part of the model */
	TSparseMatrix*	secondDesignMatrix; /*!< matrix (o x e) for the conditional part of the model  */
	TSparseMatrix*	weightMatrix; /*!< matrix (o x o) for observations weights */
	TSparseMatrix*	weightInvMatrix; /*!< matrix (o x o) for observations weights */
	TSparseMatrix*	weightUnkMatrix; /*!< matrix (o x o) for unknowns weights */

	void clearMatrices();
};


#endif