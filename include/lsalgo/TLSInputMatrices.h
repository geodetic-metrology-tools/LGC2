/*SURVEYLIB VERSION!!!!!
// TLSInputMatrices.h

//////////////////////////////////////////////////////////////////////

/*! Class for input matrices of a least squares calculation

  Pattern:

  Copyright 2000 CERN EST/SU. All rights reserved.
*/
//////////////////////////////////////////////////////////////////////


#ifndef SU_TLSINPUTMATRICES
#define SU_TLSINPUTMATRICES


#if _MSC_VER >= 1000
#pragma once
#pragma warning(disable:4786)

#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////
#include "TSparseMatrix.h"
#include "UEOIndices.h"


//! class for input matrices used in the ls-calculation
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
		//!Sets the dimensions of the matrices
		/*!@param nbUnknowns the survey network's number of unknowns
		@param nbEquations the survey network's number of equations
		@param nbObservations the survey network's number of observations*/
		virtual void setDimensions(int nbUnknowns, int nbEquations, int nbObservations, int nbCnstrObs);
	
		//!Sets the dimensions of the matrices
		/*! \param nbUnknowns the survey network's number of unknowns
			\param nbEquations the survey network's number of equations
			\param nbObservations the survey network's number of observations
			\param nbConstraints the free survey network's number of constraint*/
		virtual void setDimensions(int nbUnknowns, int nbEquations, int nbCnstrObs, int nbObservations, int nbConstraints);
	
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
		//!Sets a coefficient of the weight matrix
		virtual bool setWeightUnkMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the constraint first design matrix
		virtual bool setCnstrFirstDgnMtrxElement(MatrixIndex row, MatrixIndex column, TReal coefficient);
		//!Sets a coefficient of the constraint misclosure vector
		virtual bool setCnstrMisclosureVectorElement(MatrixIndex row, TReal coeff);
	//@}

	 virtual int		getNbrUnknowns() const;
	 virtual int		getNbrObservations() const;
	 virtual int		getNbrEquations()const;
	 virtual int		getNbrConstraintObs()const;
	 virtual int		getNbrConstraints()const;


	/*!@return a const reference to the first design matrix*/
	const TSparseMatrix* getFirstDgnMtrx() const;
	/*!@return a const reference to the second design matrix*/
	const TSparseMatrix* getSecondDgnMtrx() const;
	/*!@return a const reference to the weight design matrix*/
	const TSparseMatrix* getWeightMtrx() const;
	/*!@return a const reference to the inverted weight design matrix*/
	const TSparseMatrix* getWeightInvMtrx() const;
	/*!@return a const reference to the weight design matrix*/
	const TSparseMatrix* getWeightUnkMtrx() const;
	/*!@return a const reference to the misclosure vector*/
	const TVector&	getMisclosureVctr() const;
	/*!@return a const reference to the constraint first design matrix*/
	const TSparseMatrix*	getCnstrFirstDgnMtrx() const;
	/*!@return a const reference to the constraint misclosure vector*/
	const TVector&	getCnstrMisclosureVctr() const;
	//!Debug method
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