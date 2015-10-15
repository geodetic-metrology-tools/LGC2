#include <vector>
#include <memory>
#include <Eigen/LU>
#include <QuantileFunctions.h>
#include "TANumericValue.h"
#include "TLSMatricesAdapter.h"

using namespace std;

namespace {
	typedef std::unique_ptr<TSparseMatrix> pSpM;
	typedef std::unique_ptr<const TSparseMatrix> pcSpM;
	typedef std::unique_ptr<TVector> pVec;
}

#if 0
struct TLSInputMatricesAdapter::D 
{
	D() :
	firstDgnMtrxRowCnt(0),
	secondDgnMtrxRowCnt(0),
	CnstrfirstDgnMtrxColCnt(0),
	weightMtrxCnt(0),
	u(0), e(0), o(0), c(0), co(0), offc(0)
	{}
	
	int firstDgnMtrxRowCnt;
	int secondDgnMtrxRowCnt;
	int CnstrfirstDgnMtrxColCnt;
	int weightMtrxCnt; /// Has only entries on the main diagonal in this implementation
	int u;  /// Number of unknowns
	int e;  /// Number of equations
	int o;  /// Number of observations
	int c;  /// Number of constraints
	int co; /// Number of constraint observations
	int offc; // Number of offset constraints

	/// First design matrix entries, NOT transposed
	std::vector<TTriplet> firstDgnMtrxElems;
	/// Second design matrix entries, NOT transposed
	std::vector<TTriplet> secondDgnMtrxElems;
	/// Weight matrix entries
	std::vector<TTriplet> weightMtrxElems;
	/// Constraints for the first design matrix
	std::vector<TTriplet> CnstrfirstDgnMtrxElems;
	/// Misclorsure vector
	TVector misclosureVec;
};

struct TLSResultsMatricesAdapter::D 
{

};

void TLSInputMatricesAdapter::setDimensions(int nbUnknowns, int nbEquations, 
											int nbObservations, int nbCnstrObs)
{
	d->u  = nbUnknowns;
	d->e  = nbEquations;
	d->o  = nbObservations;
	d->co = nbCnstrObs;

	d->misclosureVec.resize(d->u);
	d->misclosureVec.setZero();
}

void TLSInputMatricesAdapter::setDimensions(int nbUnknowns, int nbEquations, int nbCnstrObs, 
		                                    int nbObservations, int nbConstraints, int offsetContraints)
{
	d->u = nbUnknowns;
	d->e = nbEquations;
	d->o = nbObservations;
	d->c = nbConstraints;
	d->co = nbCnstrObs;
	d->offc = offsetContraints;

	d->misclosureVec.resize(d->u);
	d->misclosureVec.setZero();
}

bool TLSInputMatricesAdapter::setFirstDgnMtrxTransElement(int row, TReal coefficient)
{
	// row and column are exchanged to store it untransposed
	d->firstDgnMtrxElems.emplace_back(TTriplet(d->firstDgnMtrxRowCnt++, row, coefficient));
	return true;
}

bool TLSInputMatricesAdapter::setSecondDgnMtrxTransElement(int row, TReal coefficient)
{
	// row and column are exchanged to store it untransposed
	d->secondDgnMtrxElems.emplace_back(TTriplet(d->secondDgnMtrxRowCnt++, row, coefficient));
	return true;
}

bool TLSInputMatricesAdapter::setMisclosureVectorElement(int row, TReal coeff)
{
	d->misclosureVec(row) = coeff;
	return true;
}

bool TLSInputMatricesAdapter::setWeightMtrxElement(TReal coefficient) 
{
	d->weightMtrxElems.emplace_back(TTriplet(d->weightMtrxCnt, d->weightMtrxCnt++, coefficient));
	return true;
}

 bool TLSInputMatricesAdapter::setCnstrFirstDgnMtrxElement(int row, TReal coefficient) 
 {
	 d->CnstrfirstDgnMtrxElems.emplace_back(TTriplet(row, d->CnstrfirstDgnMtrxColCnt++, coefficient));
	 return true;
 }
#else

class  TColumnVector : public  TANumericValue//: public TObject  
{
public:
	
	/*!\name Constructors and Destructors */
	//@{
		//!Default Constructor 
		TColumnVector();

		//!Constructor
		/*!\param nRows the vector's number of rows*/
		TColumnVector(int nRows);


		//!Copy Constructor 
		TColumnVector(const  TColumnVector&);

		//!Destructor
		virtual  ~TColumnVector();
	//@}


	/*!\name operators */
	//@{

	//!Copy assignement operator
	virtual TColumnVector&	operator=(const TColumnVector&);
	
	//!return the sum of this vector and a second one
	virtual TColumnVector	operator+(const TColumnVector&) const;

	//!replaces this vector by its sum with a second one
	/*!\return the sum vector*/
	virtual TColumnVector&	operator+=(const TColumnVector&);

	//!@return the difference of this vector and a second one
	virtual TColumnVector	operator-(const TColumnVector&) const;

	//!replaces this vector by its difference with a second one
	/*!\return the sum vector*/
	virtual TColumnVector&	operator-=(const TColumnVector&);

	//!return the product of this vector by a scalar
	virtual TColumnVector	operator*(const TReal k);
	
	//!return the scalar product of this vector and a second one
	virtual TReal 	operator*(const TColumnVector&) const;

	//!* Initializes all the vector's elements to a common value
	/*!	\param comVal the common value of all the vector's elements*/
	virtual void	operator=(const TReal comVal);

	//!Vector element access operator (non-const version). Indices start at 1.
	/*!\param row the row of the desired element*/
	virtual TReal&		operator()(const int row);

	//!Vector element access operator (const version). Indices start at 1.
	/*!\param row the row of the desired element*/
	virtual TReal		operator()(const int row) const;

	
	//@}


	/*!\name member functions */
	//@{

	//!Get dimension
	virtual int dimension() const;

	//!Sets the dimension of the vector. Destroys previously stored values.
	/*!\param nRows the vectors's number of rows*/
	virtual void	setDimension(const int nRows);


	//@}


private:

	//!return the adress of the vector's first element
	virtual TReal *	getFirstEltAdr() const;

	TReal*			fVector;
	int				fNbRows;

};


TColumnVector::TColumnVector()
{//!Default constructor	
	fVector = 0;
	fNbRows = 0;
	setStatus( TANumericValue::kNull );
}


TColumnVector::TColumnVector(int nRows):
fNbRows(nRows)
{//!Constructor setting the dimensions of the vector
	fVector = new TReal [fNbRows];
	(*this) = TReal(LITERAL(0.0));
	setStatus( TANumericValue::kKnown );
}
		


TColumnVector::TColumnVector( const  TColumnVector& source ):
fNbRows(source.fNbRows)
{//!Copy constructor

	//!dimensioning of the vector
	fVector = new TReal [fNbRows];

	//!copy the vector coefficients
	for (int i = 0; i<fNbRows; i++)
		fVector[i] = source.fVector[i];

	//!copy the status
	setStatus( source.getStatus() );

}



TColumnVector::~TColumnVector()
{//!Destructor
	delete[] fVector;
}



//////////////////////////////////////////////////////////////////////
///operator
//////////////////////////////////////////////////////////////////////
TColumnVector&  TColumnVector::operator=(const TColumnVector& right)
{//!Copy assignement operator
	if (this != &right)
	{
		if (dimension() <= right.dimension() && dimension()!=0)
		{
			for (int i=0; i<fNbRows; i++)
				(const_cast<TColumnVector*>(this))->operator()(i) = right(i);
			setStatus (right.getStatus());
		}
		else
		{
			this->setStatus(kNull);
			this->setDimension(0);
		}
	}

	return *this;
}


TColumnVector TColumnVector::operator +(const TColumnVector& right) const
{//!returns the sum of this vector and a second one
	TColumnVector resultat;
	resultat.setStatus(kNull);
	EStatus status=this->testStatus(right);
	if (status!=kNull && dimension() == right.dimension())
	{
		TColumnVector resul (dimension());
		for (int i=0; i<fNbRows; i++)
		{
			(resul)(i) = (*this)(i) + right(i);
		}
		resultat.setDimension(dimension());
		resultat = resul;
		resultat.setStatus(status);
	}
	return resultat;
}


TColumnVector& TColumnVector::operator +=(const TColumnVector& right)
{//!replaces this vector by its sum with a second one
	(*this) = (*this) + (right);
	return (*this);
}



TColumnVector TColumnVector::operator -(const TColumnVector& right) const
{//!returns the difference of this vector and a second one

	TColumnVector resultat;
	resultat.setStatus(kNull);
	EStatus status=this->testStatus(right);
	if (status!=kNull && dimension() == right.dimension())
	{
		TColumnVector resul (dimension());
		for (int i=0; i<fNbRows; i++)
		{
			(resul)(i) = (*this)(i) - right(i);
		}
		resultat.setDimension(dimension());
		resultat = resul;
		resultat.setStatus(status);
	}
	return resultat;	
}


TColumnVector& TColumnVector::operator -=(const TColumnVector& right)
{//!replaces this vector by its difference with a second one
	(*this) = (*this) - (right);
	return (*this);
}


TColumnVector TColumnVector::operator*(const TReal k)
{//!multiplies the vector by a scalar
	TColumnVector result (dimension());
	for (int i=0; i<fNbRows; i++)
		(result)(i) = (*this)(i)*k;
	return result;
}

//TColumnVector TColumnVector::operator*(const TDouble k)
//{//!multiplies the vector by a TDouble
//	TColumnVector resultat;
//	EStatus status=this->testStatus(k);
//	if (status!=kNull )
//	{
//		resultat.setDimension(dimension());
//		TReal d=k.getValue();
//		for (int i=0; i<fNbRows; i++)
//			(resultat)(i) = (*this)(i)*d;
//	}
//	resultat.setStatus(status);
//	return resultat;
//}


TReal TColumnVector::operator *(const TColumnVector& right) const
{//!returns the scalar product of this vector and a second one
	TReal resultat;
	EStatus status=this->testStatus(right);
	if (status!=kNull && dimension() == right.dimension())
	{
		TReal result = LITERAL(0.0);
		for (int i=0; i<dimension(); i++)
			result =result + (*this)(i) * right(i);

		resultat = result;
	}
	else 
	{
		throw std::runtime_error("Could not calculate scalar product in TColumnVector"
			                     "(dimension mismatch).");
	}
	return resultat;
}


void TColumnVector::operator=(const TReal comVal)
{//!inits all the vector coefficients to a common value
	for (int i = 0; i<fNbRows; i++)
		fVector[i] = comVal;
}


TReal& TColumnVector::operator()(const int row)
{//!returns a reference to a vector coefficient
	return fVector[row];
}


TReal TColumnVector::operator()(const int row) const
{//!returns a copy of a vector coefficient (const version of the previous method)
	return fVector[row];
}


///////////////////////////////////////////////////////////////////
///member functions
///////////////////////////////////////////////////////////////////

int TColumnVector::dimension() const
{//!Get dimension
	return fNbRows;
}


void TColumnVector::setDimension(const int nRows)
{//!Sets the dimensions of the vector

	//!destroy the previous values
	if (fVector != 0)
	{
		delete[] fVector;
	}

	//!creates the new vector
	fNbRows = nRows;
	fVector = new TReal [fNbRows];
}


//TMatrix TColumnVector::transposed()const
//{//!return the tranposed row vector as a matrix
//	TMatrix resultat (1,dimension());
//	if((this->isNull())==false)
//	{	for (int i=0; i<dimension(); i++)
//		{
//			(resultat)(0,i) = (*this)(i);
//		}
//	}
//	resultat.setStatus(getStatus());
//	return resultat;
//}


/////////////////////////////////////////////////////
///Private Function
/////////////////////////////////////////////////////

TReal* TColumnVector::getFirstEltAdr() const
{//!returns the adress of the vector's first element
	return fVector;
}



class TSparseMatrixAdapt {
public:
	TSparseMatrixAdapt(int rows, int columns, TReal* vals, int* rowInds, int* colPtr) {
		this->rows = rows;
		cols = columns;

		this->vals = vals;
		colptr = colPtr;
		rowind = rowInds;
	}
	
	// this returns a new matrix, which is the transpose of the original. Takes linear time on the number of non-zeros in the matrix.
	TSparseMatrixAdapt* transposed() const
	{
		int* temp = new int[rows];
		int i, j, q;

		TReal* values = new TReal[colptr[cols]];
		int* newColptr = new int[rows + 1];
		int* newRowind = new int[colptr[cols]];
		for (i = 0; i < rows; i++)
		{
			temp[i] = 0;
		}

		for (i = 0; i < colptr[cols]; i++)
		{
			temp[rowind[i]]++;
		}

		int total = 0;
		for (i = 0; i < rows; i++)
		{
			newColptr[i] = total;
			total += temp[i];
			temp[i] = newColptr[i];
		}
		newColptr[rows] = total;

		for (i = 0; i < cols; i++)
		{
			for (j = colptr[i]; j < colptr[i + 1]; j++)
			{
				q = temp[rowind[j]]++;
				newRowind[q] = i;
				values[q] = vals[j];
			}
		}

		delete[] temp;

		return new TSparseMatrixAdapt(cols, rows, values, newRowind, newColptr);
	}
	
	// adds two sparse matrices. Linear in the number of non-zeros in both matrices combined.
    TSparseMatrixAdapt* add(const TSparseMatrixAdapt& second) const
	{
		int first = 0, col = 1, sec = 0;
		int total = 0;

		while (col <= cols)
		{
			while (first < colptr[col] && sec < second.colptr[col])
			{
				if (rowind[first] == second.rowind[sec] && vals[first++] + second.vals[sec++] != 0)
				{
					total++;
				}
				else if (rowind[first] < second.rowind[sec])
				{
					first++;
					total++;
				}
				else
				{
					sec++;
					total++;
				}
			}
			total += colptr[col] - first + second.colptr[col] - sec;
			first = colptr[col];
			sec = second.colptr[col++];
		}

		TSparseMatrixAdapt* result = new TSparseMatrixAdapt(rows, cols, total);
		first = 0;
		col = 1;
		sec = 0;
		total = 0;

		int colp = 1;

		result->colptr[0] = 0;
		while (col <= cols)
		{
			while (first < colptr[col] && sec < second.colptr[col])
			{
				TReal res;
				if (rowind[first] == second.rowind[sec])
				{
					res = vals[first] + second.vals[sec++];
					if (res != 0)
					{
						result->rowind[total] = rowind[first];
						result->vals[total++] = res;
					}
					first++;
				}
				else if (rowind[first] < second.rowind[sec])
				{
					result->rowind[total] = rowind[first];
					result->vals[total++] = vals[first++];
				}
				else
				{
					result->rowind[total] = second.rowind[sec];
					result->vals[total++] = second.vals[sec++];
				}
			}
			while (first < colptr[col])
			{
				result->rowind[total] = rowind[first];
				result->vals[total++] = vals[first++];
			}
			while (sec < second.colptr[col])
			{
				result->rowind[total] = second.rowind[sec];
				result->vals[total++] = second.vals[sec++];
			}
			col++;
			result->colptr[colp++] = total;
		}

		return result;
	}


	inline int columnsCount() const { return cols; }
	inline int* columnPointers() const { return colptr; }
	inline int* rowIndices() const { return rowind; }



// internals
	int rows, cols;

	int* colptr;	// length cols + 1
	int* rowind;	// length colptr[cols]
	TReal* vals;	// length colptr[cols]

	TSparseMatrixAdapt(int rows, int columns, int nnz)
	{
		this->rows = rows;
		cols = columns;

		vals = new TReal[nnz];
		colptr = new int[cols + 1];
		rowind = new int[nnz];
	}
};





struct TLSInputMatricesAdapter::D 
{
	MatrixIndex		fNbUnk; /*!< number of unknowns: u */
	MatrixIndex		fNbObs; /*!< number of observations: o */
	MatrixIndex		fNbEqn; /*!< number of equations: e */
	int				fNbCnstrObs; /*!< number of constraint observations */
	MatrixIndex		fNbCnstr;/*!<number of free constraint: c */
	MatrixIndex		fNbTotalCnstr;/*!<total number of constraint: c */

	std::vector<TReal>*	firstDesignMatrixTransposedValues; /*!< matrix (u x e) for the parametric part of the model */
	std::vector<int>*	firstDesignMatrixTransposedColPtr;
	std::vector<int>*	firstDesignMatrixTransposedRowInd;

	std::vector<TReal>*	secondDesignMatrixTransposedValues; /*!< matrix (o x e) for the parametric part of the model  */
	std::vector<int>*	secondDesignMatrixTransposedColPtr;
	std::vector<int>*	secondDesignMatrixTransposedRowInd;

	std::vector<TReal>*	constraintFirstDesignMatrixValues; /*!< matrix (c x u) for the conditional part of the model  */
	std::vector<int>*	constraintFirstDesignMatrixColPtr;
	std::vector<int>*	constraintFirstDesignMatrixRowInd;

	std::vector<TReal>*	constraintFirstDesignMatrixTransposedValues; /*!< matrix (u x c) for the conditional part of the model  */
	std::vector<int>*	constraintFirstDesignMatrixTransposedColPtr;
	std::vector<int>*	constraintFirstDesignMatrixTransposedRowInd;

	std::vector<TReal>*	weightMatrixValues; /*!< matrix (o x o) for observations weights */

	TColumnVector*	fMisclosureVector; /*!< vector (u) for misclosure errors */
//	TReal			fS0APrioriScaleFactor; /*!< indicates if there is a priori scale factor or not */

	TSparseMatrixAdapt*	fCnstrFirstDesignMtrx; /*!< matrix (c x u) for the parametric part of the model */
	TColumnVector*	fCnstrMisclosureVector; /*!< vector for misclosure errors */

	TSparseMatrixAdapt*	firstDesignMatrix; /*!< matrix (e x u) for the parametric part of the model */
	TSparseMatrixAdapt*	firstDesignMatrixTransposed; /*!< matrix (u x e) for the parametric part of the model */
	TSparseMatrixAdapt*	secondDesignMatrixTransposed; /*!< matrix (u x e) for the parametric part of the model */
	TSparseMatrixAdapt*	weightMatrix; /*!< matrix (o x o) for the observations weights */
	TSparseMatrixAdapt*	weightMatrixInverted;
	TSparseMatrixAdapt*	bTimesWInvTimesBTransInverted;

	D() {
		firstDesignMatrixTransposedValues = new vector<TReal>();
		firstDesignMatrixTransposedColPtr = new vector<int>();
		firstDesignMatrixTransposedRowInd = new vector<int>();

		secondDesignMatrixTransposedValues = new vector<TReal>();
		secondDesignMatrixTransposedColPtr = new vector<int>();
		secondDesignMatrixTransposedRowInd = new vector<int>();

		weightMatrixValues = new vector<TReal>();

		firstDesignMatrixTransposed = NULL;
		secondDesignMatrixTransposed = NULL;
		weightMatrix = NULL;
		bTimesWInvTimesBTransInverted = NULL;
		weightMatrixInverted = NULL;

		fCnstrFirstDesignMtrx = NULL;

		fMisclosureVector = NULL;

		fCnstrMisclosureVector = 0;

		constraintFirstDesignMatrixValues = NULL;
		constraintFirstDesignMatrixColPtr = NULL;
		constraintFirstDesignMatrixRowInd = NULL;

		constraintFirstDesignMatrixTransposedValues = NULL;
		constraintFirstDesignMatrixTransposedColPtr = NULL;
		constraintFirstDesignMatrixTransposedRowInd = NULL;
		
		fNbUnk = 0;
		fNbEqn = 0;
		fNbObs = 0;
		fNbCnstr = 0;
		fNbTotalCnstr = 0;
		fNbCnstrObs = 0;
	}

	~D() {
		delete firstDesignMatrixTransposedValues;
		delete firstDesignMatrixTransposedColPtr;
		delete firstDesignMatrixTransposedRowInd;
		delete secondDesignMatrixTransposedValues;
		delete secondDesignMatrixTransposedColPtr;
		delete secondDesignMatrixTransposedRowInd;
		delete weightMatrixValues;

		if (constraintFirstDesignMatrixValues != NULL)
		{
			delete constraintFirstDesignMatrixValues;
			delete constraintFirstDesignMatrixColPtr;
			delete constraintFirstDesignMatrixRowInd;
			delete constraintFirstDesignMatrixTransposedValues;
			delete constraintFirstDesignMatrixTransposedColPtr;
			delete constraintFirstDesignMatrixTransposedRowInd;
			delete fCnstrFirstDesignMtrx;		
			delete fCnstrFirstDesignMtrx;
			delete fCnstrMisclosureVector;
		}
	}
};

void TLSInputMatricesAdapter::setDimensions(int unknowns, int equations, int observations,  int cnstrObs)
{//sets the dimensions of the matrices

	d->fNbUnk = unknowns;
	d->fNbObs = observations; // number of observations + constraint observations
	d->fNbEqn = equations;
	d->fNbCnstrObs = cnstrObs;

	d->firstDesignMatrixTransposedColPtr->reserve(observations + 1);
	d->secondDesignMatrixTransposedColPtr->reserve(equations + 1);

	clearMatrices();
	d->fMisclosureVector = new TColumnVector(d->fNbObs);
}

void TLSInputMatricesAdapter::setDimensions(int unknowns, int equations, int observations, int nbCnstrObs, int constraints, int offsetContraints)
{//sets the dimensions of the matrices

	d->fNbUnk = unknowns;
	d->fNbObs = observations; // number of observations + constraint observations
	d->fNbEqn = equations;
	d->fNbCnstr = constraints;
	d->fNbTotalCnstr = constraints + offsetContraints;
	d->fNbCnstrObs = nbCnstrObs;

	d->firstDesignMatrixTransposedColPtr->reserve(observations + 1);
	d->secondDesignMatrixTransposedColPtr->reserve(equations + 1);

	clearMatrices();
	d->fMisclosureVector = new TColumnVector(d->fNbObs);
	d->fCnstrMisclosureVector = new TColumnVector(d->fNbTotalCnstr);

	d->constraintFirstDesignMatrixValues = new vector<TReal>();
	d->constraintFirstDesignMatrixColPtr = new vector<int>();
	d->constraintFirstDesignMatrixRowInd = new vector<int>();

	d->constraintFirstDesignMatrixTransposedValues = new vector<TReal>();
	d->constraintFirstDesignMatrixTransposedColPtr = new vector<int>();
	d->constraintFirstDesignMatrixTransposedRowInd = new vector<int>();
	
	d->constraintFirstDesignMatrixColPtr->reserve(d->fNbUnk);
	d->constraintFirstDesignMatrixTransposedColPtr->reserve(d->fNbTotalCnstr);
}

void TLSInputMatricesAdapter::clearMatrices()
{
	if (d->firstDesignMatrixTransposed != NULL)
	{
		delete d->secondDesignMatrixTransposed;
		delete d->firstDesignMatrixTransposed;
		delete d->weightMatrix;

		d->firstDesignMatrixTransposed = NULL;
		d->secondDesignMatrixTransposed = NULL;
		d->weightMatrix = NULL;
	}

	if (d->fCnstrFirstDesignMtrx != NULL)
	{
		delete d->fCnstrFirstDesignMtrx;
		d->fCnstrFirstDesignMtrx = NULL;
	}

	if (d->weightMatrixInverted != NULL)
	{
		delete d->weightMatrixInverted;
		d->weightMatrixInverted = NULL;
	}

	if (d->bTimesWInvTimesBTransInverted != NULL)
	{
		delete d->bTimesWInvTimesBTransInverted;
		d->bTimesWInvTimesBTransInverted = NULL;
	}

	if (d->constraintFirstDesignMatrixValues != NULL)
	{
		delete d->constraintFirstDesignMatrixValues;
		delete d->constraintFirstDesignMatrixColPtr;
		delete d->constraintFirstDesignMatrixRowInd;

		d->constraintFirstDesignMatrixValues = NULL;
		d->constraintFirstDesignMatrixColPtr = NULL;
		d->constraintFirstDesignMatrixRowInd = NULL;
		
		delete d->constraintFirstDesignMatrixTransposedValues;
		delete d->constraintFirstDesignMatrixTransposedColPtr;
		delete d->constraintFirstDesignMatrixTransposedRowInd;

		d->constraintFirstDesignMatrixTransposedValues = NULL;
		d->constraintFirstDesignMatrixTransposedColPtr = NULL;
		d->constraintFirstDesignMatrixTransposedRowInd = NULL;
	}

	if (d->fMisclosureVector != NULL)
	{
		delete d->fMisclosureVector;
        d->fMisclosureVector = NULL;
	}
	if (d->fCnstrMisclosureVector != NULL)
	{
		delete d->fCnstrMisclosureVector;
        d->fCnstrMisclosureVector = NULL;
	}
}

bool TLSInputMatricesAdapter::setFirstDgnMtrxTransElement(MatrixIndex row, TReal coeff)
{//sets an element of the first design matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		d->firstDesignMatrixTransposedValues->push_back(coeff);
		d->firstDesignMatrixTransposedRowInd->push_back(row);
	}

	return successfullySet;
}

bool TLSInputMatricesAdapter::setSecondDgnMtrxTransElement(MatrixIndex row, TReal coeff)
{//sets an element of the second design matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		d->secondDesignMatrixTransposedValues->push_back(coeff);
		d->secondDesignMatrixTransposedRowInd->push_back(row);
	}

	return successfullySet;
}

bool TLSInputMatricesAdapter::setMisclosureVectorElement(MatrixIndex row, TReal coeff)
{//sets an element of the misclosure vector
	bool successfullySet = true;
	if (row <= d->fNbEqn)
		(*d->fMisclosureVector)(row) = (*d->fMisclosureVector)(row) + coeff;
	else
		successfullySet = false;

	return successfullySet;
}


bool TLSInputMatricesAdapter::setWeightMtrxElement(TReal coeff)
{//sets en element of the weight matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		d->weightMatrixValues->push_back(coeff);
	}

	return successfullySet;
}


bool TLSInputMatricesAdapter::setCnstrFirstDgnMtrxElement(MatrixIndex row, TReal coeff)
{//sets an element of the constraint first design matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		d->constraintFirstDesignMatrixValues->push_back(coeff);
		d->constraintFirstDesignMatrixRowInd->push_back(row);
	}
	return successfullySet;
}


bool TLSInputMatricesAdapter::setCnstrFirstDgnMtrxTransposedElement(MatrixIndex row, TReal coeff)
{//sets an element of the constraint first design matrix


	bool successfullySet = true;
	if (coeff != 0)
	{
		d->constraintFirstDesignMatrixTransposedValues->push_back(coeff);
		d->constraintFirstDesignMatrixTransposedRowInd->push_back(row);
	}
	return successfullySet;
}


bool TLSInputMatricesAdapter::setCnstrMisclosureVectorElement(MatrixIndex row, TReal coeff)
{//sets an element of the constraint misclosure vector
	bool successfullySet = true;
	if (row <= d->fNbTotalCnstr)
	{
		(*d->fCnstrMisclosureVector)(row) = (*d->fCnstrMisclosureVector)(row) + coeff;
	}
	else
	{
		successfullySet = false;
	}
	return successfullySet;
}

void TLSInputMatricesAdapter::setNewColumn()
{
	d->firstDesignMatrixTransposedColPtr->push_back(d->firstDesignMatrixTransposedValues->size());
	d->secondDesignMatrixTransposedColPtr->push_back(d->secondDesignMatrixTransposedValues->size());
}

void TLSInputMatricesAdapter::setConstraintNewColumn()
{
	d->constraintFirstDesignMatrixColPtr->push_back(d->constraintFirstDesignMatrixValues->size());
}

void TLSInputMatricesAdapter::setConstraintTransposedNewColumn()
{
	d->constraintFirstDesignMatrixTransposedColPtr->push_back(d->constraintFirstDesignMatrixTransposedValues->size());
}


void TLSInputMatricesAdapter::finishedFillingMatrices()
{
	TReal* vals = new TReal[d->firstDesignMatrixTransposedValues->size()];
	int* cols = new int[d->firstDesignMatrixTransposedColPtr->size()];
	int* rows = new int[d->firstDesignMatrixTransposedRowInd->size()];
	vector<TReal>::const_iterator iter = d->firstDesignMatrixTransposedValues->begin();
	vector<int>::const_iterator iterRows = d->firstDesignMatrixTransposedRowInd->begin();
	int i = 0;
	while (iter != d->firstDesignMatrixTransposedValues->end())
	{
		rows[i] = *iterRows;
		vals[i++] = *iter;

		iter++;
		iterRows++;
	}
	d->firstDesignMatrixTransposedValues->clear();
	d->firstDesignMatrixTransposedRowInd->clear();

	i = 0;
	vector<int>::const_iterator iterCols = d->firstDesignMatrixTransposedColPtr->begin();
	while (iterCols != d->firstDesignMatrixTransposedColPtr->end())
	{
		cols[i++] = *iterCols;

		iterCols++;
	}
	d->firstDesignMatrixTransposedColPtr->clear();

	TReal* vals2 = new TReal[d->secondDesignMatrixTransposedValues->size()];
	int* cols2 = new int[d->secondDesignMatrixTransposedColPtr->size()];
	int* rows2 = new int[d->secondDesignMatrixTransposedRowInd->size()];

	vector<TReal>::const_iterator iter2 = d->secondDesignMatrixTransposedValues->begin();
	vector<int>::const_iterator iterRows2 = d->secondDesignMatrixTransposedRowInd->begin();
	i = 0;
	while (iter2 != d->secondDesignMatrixTransposedValues->end())
	{
		rows2[i] = *iterRows2;
		vals2[i++] = *iter2;

		iter2++;
		iterRows2++;
	}
	d->secondDesignMatrixTransposedValues->clear();
	d->secondDesignMatrixTransposedRowInd->clear();

	i = 0;
	vector<int>::const_iterator iterCols2 = d->secondDesignMatrixTransposedColPtr->begin();
	while (iterCols2 != d->secondDesignMatrixTransposedColPtr->end())
	{
		cols2[i++] = *iterCols2;

		iterCols2++;
	}
	d->secondDesignMatrixTransposedColPtr->clear();

	d->firstDesignMatrixTransposed = new TSparseMatrixAdapt(d->fNbUnk, d->fNbEqn,
		vals, rows, cols);
	d->secondDesignMatrixTransposed = new TSparseMatrixAdapt(d->fNbObs, d->fNbEqn,
		vals2, rows2, cols2);
	int* rowinds = new int[d->fNbObs + 1];
	for (i = 0; i <= d->fNbObs; i++)
	{
		rowinds[i] = i;
	}

	TReal* vals3 = new TReal[d->weightMatrixValues->size()];
	vector<TReal>::const_iterator iter3 = d->weightMatrixValues->begin();
	i = 0;
	while (iter3 != d->weightMatrixValues->end())
	{
		vals3[i++] = *iter3;

		iter3++;
	}
	d->weightMatrixValues->clear();

	d->weightMatrix = new TSparseMatrixAdapt(d->fNbEqn, d->fNbEqn,
		vals3, rowinds, rowinds);

	if (d->fNbTotalCnstr != 0)
	{
		while (d->constraintFirstDesignMatrixColPtr->size() <= d->fNbUnk)
		{
			setConstraintNewColumn();
		}
		while (d->constraintFirstDesignMatrixTransposedColPtr->size() <= d->fNbTotalCnstr)
		{
			setConstraintTransposedNewColumn();
		}

        TReal* vals4 = new TReal[d->constraintFirstDesignMatrixValues->size()];
        int* cols4 = new int[d->constraintFirstDesignMatrixColPtr->size()];
        int* rows4 = new int[d->constraintFirstDesignMatrixRowInd->size()];
        vector<TReal>::const_iterator iter4 = d->constraintFirstDesignMatrixValues->begin();
        vector<int>::const_iterator iterRows4 = d->constraintFirstDesignMatrixRowInd->begin();
        i = 0;
        while (iter4 != d->constraintFirstDesignMatrixValues->end())
        {
            rows4[i] = *iterRows4;
            vals4[i++] = *iter4;

            iter4++;
            iterRows4++;
        }
        d->constraintFirstDesignMatrixValues->clear();
        d->constraintFirstDesignMatrixRowInd->clear();

        i = 0;
        vector<int>::const_iterator iterCols4 = d->constraintFirstDesignMatrixColPtr->begin();
        while (iterCols4 != d->constraintFirstDesignMatrixColPtr->end())
        {
            cols4[i++] = *iterCols4;

            iterCols4++;
        }
        d->constraintFirstDesignMatrixColPtr->clear();		

        TReal* vals5 = new TReal[d->constraintFirstDesignMatrixTransposedValues->size()];
        int* cols5 = new int[d->constraintFirstDesignMatrixTransposedColPtr->size()];
        int* rows5 = new int[d->constraintFirstDesignMatrixTransposedRowInd->size()];
        vector<TReal>::const_iterator iter5 = d->constraintFirstDesignMatrixTransposedValues->begin();
        vector<int>::const_iterator iterrows5 = d->constraintFirstDesignMatrixTransposedRowInd->begin();
        i = 0;
        while (iter5 != d->constraintFirstDesignMatrixTransposedValues->end())
        {
            rows5[i] = *iterrows5;
            vals5[i++] = *iter5;

            iter5++;
            iterrows5++;
        }
        d->constraintFirstDesignMatrixTransposedValues->clear();
        d->constraintFirstDesignMatrixTransposedRowInd->clear();

        i = 0;
        vector<int>::const_iterator itercols5 = d->constraintFirstDesignMatrixTransposedColPtr->begin();
        while (itercols5 != d->constraintFirstDesignMatrixTransposedColPtr->end())
        {
            cols5[i++] = *itercols5;

            itercols5++;
        }
        d->constraintFirstDesignMatrixTransposedColPtr->clear();
		TSparseMatrixAdapt* CTrans = new TSparseMatrixAdapt(d->fNbUnk, d->fNbTotalCnstr, vals5, rows5, cols5);
		TSparseMatrixAdapt* C2 = CTrans->transposed();
		delete CTrans;

		for (i = 0; i < C2->columnPointers()[C2->columnsCount()]; i++)
		{
			C2->rowIndices()[i] += d->fNbCnstr;
		}

		d->fCnstrFirstDesignMtrx = new TSparseMatrixAdapt(d->fNbTotalCnstr, d->fNbUnk,
			vals4, rows4, cols4);

		TSparseMatrixAdapt* temp = d->fCnstrFirstDesignMtrx->add(*C2);
		delete d->fCnstrFirstDesignMtrx;
		delete C2;
		d->fCnstrFirstDesignMtrx = temp;
	}
}

TDenseMatrix toEigen(const TSparseMatrixAdapt* a) {
	
	TDenseMatrix m(a->rows, a->cols);
	
	for (int i = 0; i < a->cols; i++)
	{
		for (int j = a->colptr[i]; j < a->colptr[i + 1]; j++)
		{
			m(a->rowind[j], i) = a->vals[j];
		}
	}
	
	return m;
}


TVector toEigen(const TColumnVector* a) {
	TVector v(a->dimension());

	for (int i = 0; i < a->dimension(); i++)
		v(i) = (*a)(i);

	return v;
}

//
//TSparseMatrix* TLSInputMatricesAdapter::getFirstDgnMtrx() 
//{
//	if (d->firstDesignMatrixEigen == nullptr)
//		d->firstDesignMatrixEigen = toEigen(d->firstDesignMatrix);
//	return d->firstDesignMatrixEigen.get();
//}

#endif

/*
	Equations and nomenclature are taken from the 'white book':

	THE METHOD OF LEAST SQUARES
	  D. E. WELLS
	  E. J. KRAKIWSKY
	May 1971
	Lecture Notes 18
	Department of Geodesy and Geomatics Engineering
	University of New Brunswick
	P.O. Box 4400
	Fredericton, N .B.
	Canada
	E3B 5A3

	The reference is avaliable somewhere under G:\Support\SurveyingEng\Computing\Software
	as 
	TheMethodOfLeastSquares.pdf
	at the time of writing.

	The concrete nomenclature summarized:

	e: Number of equations
	n: Number of observations
	u: Number of unknowns
	
	A:     First design matrix
	B:     Second design matrix containing the conditions
	P:      Weight matrix
	X0:     Initial approximation to solution vector
	X:      Increment to the solution (estimated by the least-squares-process)
	Xtotal: The solution vector (X0+X)
	L:      Observation vector
	V:      Residuals vector
	W:      Misclosure vector
	Ltotal: Total observation vector (L+V)

	Two boolean flags exist:

	Combinded  Free
	    0        0   Standard parametric method
		0        1   Constrained parametric method
		1        0   Parametric + Condition equations
		1        1   Parametric + Condition equations + Constraint equations



*/


TLSInputMatricesAdapter::TLSInputMatricesAdapter() :
	d(new D) {
}
TLSInputMatricesAdapter::~TLSInputMatricesAdapter() {
	delete d;
}


struct TLSParametricMtdComputerAdapter::D {
	D() {}

	~D() {}
};


struct TLSResultsMatricesAdapter::D {
	TDenseMatrix Cxx; // Variance-Covariance matrix
	TVector residuals;
	TVector solution;
	TVector varObs;
	TReal s02; // sigma zero aposteriori squared

	D() {}
	~D() {}
};


TLSResultsMatricesAdapter::TLSResultsMatricesAdapter(UEOIndices) {
	d = new D;
	fs0apost = false;
}

TLSResultsMatricesAdapter::~TLSResultsMatricesAdapter() {
	delete d;
}

bool TLSParametricMtdComputerAdapter::computeResults(TLSInputMatricesAdapter* im, TLSResultsMatricesAdapter* rm, 
													 bool isCombinedCase, bool isFreeNetworkOrECHO, bool hasConstraints, bool calcVarObs) {

    bool result(false);

	TDenseMatrix N;
	auto A1(std::move(toEigen(im->d->firstDesignMatrix)));
	auto A2(std::move(toEigen(im->d->fCnstrFirstDesignMtrx)));
	auto Bt(std::move(toEigen(im->d->secondDesignMatrixTransposed)));
	auto P (std::move(toEigen(im->d->weightMatrix).asDiagonal()));
	auto W (std::move(toEigen(im->d->fMisclosureVector)));
	
	const auto& B   (Bt.transpose());
	const auto& A1t (A1.transpose());

	// Keep exactly one lu-decomposition for all the inversions, 
	// delete when not needed anymore.
	Eigen::FullPivLU<TDenseMatrix> lu(P);
	const TDenseMatrix& Pi(lu.inverse()); // inverse weight matrix

	bool useCnstr(isFreeNetworkOrECHO||hasConstraints);
	
#if 0
	int br, bc; // Big matrix rows and cols
	if (useCnstr) 
	{ // Adds constraints to unknowns (A2-matrix)
		// See eqn (9.41), p. 145
		br = A1.cols() + A2.rows();
		bc = A1.cols() + A2.rows();
		N.resize(br, bc);
		W.conservativeResize(br); // Keep the values of the miscosure vector when growing it

		if (isCombinedCase) 
		{ // Uses the B-Matrix (observations and unknowns not separable)
			lu = new Eigen::FullPivLU<TDenseMatrix>(B*Pi*Bt);
			N << A1t*lu->inverse()*A1,   A2.transpose(), 
				        A2,              ZEROS(A2.rows(), A2.rows());
			delete lu;
		}
		else
		{ // No condition equations
			// B = I in this case, inv(I*inv(P)*I) gives P
			N << A1t*P*A1, A2.transpose(), 
				     A2,    ZEROS(A2.rows(), A2.rows());
		}
	}
	else 
	{ // Adds no A2-matrix
		br = A1.cols() + A1.rows();
		bc = A1.cols() + A1.rows();
		N.resize(br, bc);
		W.conservativeResize(br); // Keep the values of the miscosure vector when growing it

		if (isCombinedCase) 
		{	// Uses the B-Matrix (observations and unknowns not separable)
			N << -B*Pi*Bt,    A1, 
				    A1t,    ZEROS( A1.cols(), A1.cols());

		}
		else
		{ // no condition equations: this is the most simple, parametric case
			N <<    Pi,    A1, 
				    A1t,   ZEROS( A1.cols(), A1.cols());
		}
	}

	
	// Invert the normal matrix
	lu = new Eigen::FullPivLU<TDenseMatrix>(N);
	if (! lu->isInvertible()) {
		fError = "Cannot invert normal matrix";
		delete lu;
		return false;

	}
#else
	TVector dX, dL; // dL := incremental residuals
	TReal redundancy; // leave uninitialized to let the compiler discover if a branch is missed

	// Just hack down the eqns from the book
	//if (useCnstr) 
	//{ // Adds constraints to unknowns (A2-matrix)
	//	// See eqn (9.41), p. 145
	//	if (isCombinedCase) 
	//	{ // Uses the B-Matrix (observations and unknowns not separable)

	//	}
	//	else
	//	{ // No condition equations
	//	
	//	}
	//}
	//else 
	{ // Adds no A2-matrix
		if (isCombinedCase) 
		{	// Uses the B-Matrix (observations and unknowns not separable)
			Eigen::FullPivLU<TDenseMatrix> lu2(B*Pi*Bt);
			const TDenseMatrix& Qi(lu2.inverse());
			Eigen::FullPivLU<TDenseMatrix> lu3(A1t*Qi*A1);
			const TDenseMatrix& Ri(lu3.inverse());

			dX = -Ri*A1t*Qi*W; // (7.27a)
			dL = -Pi*Bt *Qi*W; // (7.29)

			rm->d->Cxx = Ri; // (7.36b)

			// redundancy: observations + conditions - unknowns
			redundancy = A1.rows() + B.rows() - A1.cols();
		}
		else
		{ // no condition equations: this is the most simple, parametric case
			Eigen::FullPivLU<TDenseMatrix> lu3(A1t*P*A1);
			const TDenseMatrix& Ri(lu3.inverse());

			dX = -Ri*A1t*P*W; // (7.28)
			dL = -A1*dX + W;  // (7.28)

			rm->d->Cxx = Ri;  // (7.37b)

			// redundancy: observations - unknowns
			redundancy = A1.rows() - A1.cols();
		}
	}

#endif
	
	// calculate the a-posteriori variance factor
	rm->d->s02 = (redundancy > 0.0) ?
					    (dL.transpose()*P*dL)(0) / redundancy :
						1.0;

	// Turn the cofactor matrix into the covariance matrix
	rm->d->Cxx *= rm->d->s02;

	if (calcVarObs)
	{
		rm->d->varObs = (A1 * rm->d->Cxx * A1t).diagonal();
	}

	// calculate the sigma zero limits
	TReal s0PostUpLimit = LITERAL(0.0);
	TReal s0PostLoLimit = LITERAL(0.0);
	
	const TReal& sigmaZero2(rm->d->s02);
	if(redundancy > 0.0)
	{
        TReal chiUp = deviates_chi_sq(LITERAL(0.975),redundancy);
        TReal chiLow = deviates_chi_sq(1-LITERAL(0.975),redundancy);

		//Limits
		s0PostUpLimit = sqrtq(chiUp/redundancy);
		s0PostLoLimit = sqrtq(chiLow/redundancy);

		if ((sqrtq(sigmaZero2) < s0PostUpLimit) && (sqrtq(sigmaZero2) > s0PostLoLimit))
		{
			rm->fs0apost = false;
		}
	}
	
	return true;
}


TReal TLSResultsMatricesAdapter::getSigmaZero2() const {
	return d->s02;
}

TReal TLSResultsMatricesAdapter::getUnknownsCovarMtrxElmt(int row, int column) const {
	// multiply the variance factor on the fly
	return d->Cxx(row, column) * d->s02;
}

int TLSResultsMatricesAdapter::getUnkCovarMtrxRowsCount() const {
	return d->Cxx.rows();
}

int TLSResultsMatricesAdapter::getResidualsVctrDimension() const {
	return d->residuals.rows();
}

TReal TLSResultsMatricesAdapter::getResidualsVctrElmt(int row) const {
	return d->residuals(row);
}

int TLSResultsMatricesAdapter::getSolutionVctrDimension() const {
	return d->solution.rows();
}

TReal TLSResultsMatricesAdapter::getSolutionVctrElmt(int idx) const {
	return d->solution(idx);
}

TVector* TLSResultsMatricesAdapter::getVarObs() {
	return &(d->varObs);
}