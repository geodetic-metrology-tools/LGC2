/*SURVEYLIB VERSION !!!!!!!!!!!!!!!!!!!!!!!*/

//TLSInputSparseMatrices.h : implementation file
// class for input matrices as defined for survey purposes
// and for the least squares solving algorithm
/***DEBUG*///
#include <iostream>
#include <iomanip>
using namespace std;

#include "TLSInputSparseMatrices.h"


/////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////////
TLSInputSparseMatrices::TLSInputSparseMatrices()
{//Constructor
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

//	fS0APrioriScaleFactor = 1;
}


TLSInputSparseMatrices::~TLSInputSparseMatrices()
{//Destructor
	delete firstDesignMatrixTransposedValues;
	delete firstDesignMatrixTransposedColPtr;
	delete firstDesignMatrixTransposedRowInd;
	delete secondDesignMatrixTransposedValues;
	delete secondDesignMatrixTransposedColPtr;
	delete secondDesignMatrixTransposedRowInd;
	delete weightMatrixValues;
	delete fMisclosureVector;

	if (constraintFirstDesignMatrixValues != NULL)
	{
		delete constraintFirstDesignMatrixValues;
		delete constraintFirstDesignMatrixColPtr;
		delete constraintFirstDesignMatrixRowInd;
		delete constraintFirstDesignMatrixTransposedValues;
		delete constraintFirstDesignMatrixTransposedColPtr;
		delete constraintFirstDesignMatrixTransposedRowInd;
		delete fCnstrFirstDesignMtrx;
		delete fCnstrMisclosureVector;
	}
}



/////////////////////////////////////////////////////////////////////////////////
//SET FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
void TLSInputSparseMatrices::setDimensions(int unknowns, int equations, int observations,  int cnstrObs)
{//sets the dimensions of the matrices

	fNbUnk = unknowns;
	fNbObs = observations; // number of observations + constraint observations
	fNbEqn = equations;
	fNbCnstrObs = cnstrObs;

	firstDesignMatrixTransposedColPtr->reserve(equations + 1);
	secondDesignMatrixTransposedColPtr->reserve(equations + 1);

	clearMatrices();
	fMisclosureVector = new TColumnVector(fNbObs);
}


void TLSInputSparseMatrices::setDimensions(int unknowns, int equations, int observations, int nbCnstrObs, int constraints, int offsetContraints)
{//sets the dimensions of the matrices

	fNbUnk = unknowns;
	fNbObs = observations; // number of observations + constraint observations
	fNbEqn = equations;
	fNbCnstr = constraints;
	fNbTotalCnstr = constraints + offsetContraints;
	fNbCnstrObs = nbCnstrObs;

	firstDesignMatrixTransposedColPtr->reserve(equations + 1);
	secondDesignMatrixTransposedColPtr->reserve(equations + 1);

	clearMatrices();
	fMisclosureVector = new TColumnVector(fNbObs);
	fCnstrMisclosureVector = new TColumnVector(fNbTotalCnstr);

	constraintFirstDesignMatrixValues = new vector<TReal>();
	constraintFirstDesignMatrixColPtr = new vector<int>();
	constraintFirstDesignMatrixRowInd = new vector<int>();

	constraintFirstDesignMatrixTransposedValues = new vector<TReal>();
	constraintFirstDesignMatrixTransposedColPtr = new vector<int>();
	constraintFirstDesignMatrixTransposedRowInd = new vector<int>();
	
	constraintFirstDesignMatrixColPtr->reserve(fNbUnk);
	constraintFirstDesignMatrixTransposedColPtr->reserve(fNbTotalCnstr);
}



/*
void TLSInputSparseMatrices::setS0APrioriScaleFactor(TReal scalefac)
{//sets the scale factor for the S0 a priori
	fS0APrioriScaleFactor = scalefac;
}*/

void TLSInputSparseMatrices::clearMatrices()
{
	if (firstDesignMatrixTransposed != NULL)
	{
		delete secondDesignMatrixTransposed;
		delete firstDesignMatrixTransposed;
		delete weightMatrix;

		firstDesignMatrixTransposed = NULL;
		secondDesignMatrixTransposed = NULL;
		weightMatrix = NULL;
	}

	if (fCnstrFirstDesignMtrx != NULL)
	{
		delete fCnstrFirstDesignMtrx;
		fCnstrFirstDesignMtrx = NULL;
	}

	if (weightMatrixInverted != NULL)
	{
		delete weightMatrixInverted;
		weightMatrixInverted = NULL;
	}

	if (bTimesWInvTimesBTransInverted != NULL)
	{
		delete bTimesWInvTimesBTransInverted;
		bTimesWInvTimesBTransInverted = NULL;
	}

	if (constraintFirstDesignMatrixValues != NULL)
	{
		delete constraintFirstDesignMatrixValues;
		delete constraintFirstDesignMatrixColPtr;
		delete constraintFirstDesignMatrixRowInd;

		constraintFirstDesignMatrixValues = NULL;
		constraintFirstDesignMatrixColPtr = NULL;
		constraintFirstDesignMatrixRowInd = NULL;
		
		delete constraintFirstDesignMatrixTransposedValues;
		delete constraintFirstDesignMatrixTransposedColPtr;
		delete constraintFirstDesignMatrixTransposedRowInd;

		constraintFirstDesignMatrixTransposedValues = NULL;
		constraintFirstDesignMatrixTransposedColPtr = NULL;
		constraintFirstDesignMatrixTransposedRowInd = NULL;
	}

	if (fMisclosureVector != NULL)
	{
		delete fMisclosureVector;
        fMisclosureVector = NULL;
	}
	if (fCnstrMisclosureVector != NULL)
	{
		delete fCnstrMisclosureVector;
        fCnstrMisclosureVector = NULL;
	}
}


bool TLSInputSparseMatrices::setFirstDgnMtrxTransElement(MatrixIndex row, TReal coeff)
{//sets an element of the first design matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		firstDesignMatrixTransposedValues->push_back(coeff);
		firstDesignMatrixTransposedRowInd->push_back(row);
	}

	return successfullySet;
}


bool TLSInputSparseMatrices::setSecondDgnMtrxTransElement(MatrixIndex row, TReal coeff)
{//sets an element of the second design matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		secondDesignMatrixTransposedValues->push_back(coeff);
		secondDesignMatrixTransposedRowInd->push_back(row);
	}

	return successfullySet;
}


bool TLSInputSparseMatrices::setMisclosureVectorElement(MatrixIndex row, TReal coeff)
{//sets an element of the misclosure vector
	bool successfullySet = true;
	if (row <= fNbEqn)
		(*fMisclosureVector)(row) = (*fMisclosureVector)(row) + coeff;
	else
		successfullySet = false;

	return successfullySet;
}


bool TLSInputSparseMatrices::setWeightMtrxElement(TReal coeff)
{//sets en element of the weight matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		weightMatrixValues->push_back(coeff);
	}

	return successfullySet;
}


bool TLSInputSparseMatrices::setCnstrFirstDgnMtrxElement(MatrixIndex row, TReal coeff)
{//sets an element of the constraint first design matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		constraintFirstDesignMatrixValues->push_back(coeff);
		constraintFirstDesignMatrixRowInd->push_back(row);
	}
	return successfullySet;
}


bool TLSInputSparseMatrices::setCnstrFirstDgnMtrxTransposedElement(MatrixIndex row, TReal coeff)
{//sets an element of the constraint first design matrix
	bool successfullySet = true;
	if (coeff != 0)
	{
		constraintFirstDesignMatrixTransposedValues->push_back(coeff);
		constraintFirstDesignMatrixTransposedRowInd->push_back(row);
	}
	return successfullySet;
}


bool TLSInputSparseMatrices::setCnstrMisclosureVectorElement(MatrixIndex row, TReal coeff)
{//sets an element of the constraint misclosure vector
	bool successfullySet = true;
	if (row <= fNbTotalCnstr)
	{
		(*fCnstrMisclosureVector)(row) = (*fCnstrMisclosureVector)(row) + coeff;
	}
	else
	{
		successfullySet = false;
	}
	return successfullySet;
}

void TLSInputSparseMatrices::setNewColumn()
{
	firstDesignMatrixTransposedColPtr->push_back(firstDesignMatrixTransposedValues->size());
	secondDesignMatrixTransposedColPtr->push_back(secondDesignMatrixTransposedValues->size());
}

void TLSInputSparseMatrices::setConstraintNewColumn()
{
	constraintFirstDesignMatrixColPtr->push_back(constraintFirstDesignMatrixValues->size());
}

void TLSInputSparseMatrices::setConstraintTransposedNewColumn()
{
	constraintFirstDesignMatrixTransposedColPtr->push_back(constraintFirstDesignMatrixTransposedValues->size());
}

void TLSInputSparseMatrices::finishedFillingMatrices()
{
	TReal* vals = new TReal[firstDesignMatrixTransposedValues->size()];
	int* cols = new int[firstDesignMatrixTransposedColPtr->size()];
	int* rows = new int[firstDesignMatrixTransposedRowInd->size()];
	vector<TReal>::const_iterator iter = firstDesignMatrixTransposedValues->begin();
	vector<int>::const_iterator iterRows = firstDesignMatrixTransposedRowInd->begin();
	int i = 0;
	while (iter != firstDesignMatrixTransposedValues->end())
	{
		rows[i] = *iterRows;
		vals[i++] = *iter;

		iter++;
		iterRows++;
	}
	firstDesignMatrixTransposedValues->clear();
	firstDesignMatrixTransposedRowInd->clear();

	i = 0;
	vector<int>::const_iterator iterCols = firstDesignMatrixTransposedColPtr->begin();
	while (iterCols != firstDesignMatrixTransposedColPtr->end())
	{
		cols[i++] = *iterCols;

		iterCols++;
	}
	firstDesignMatrixTransposedColPtr->clear();

	TReal* vals2 = new TReal[secondDesignMatrixTransposedValues->size()];
	int* cols2 = new int[secondDesignMatrixTransposedColPtr->size()];
	int* rows2 = new int[secondDesignMatrixTransposedRowInd->size()];

	vector<TReal>::const_iterator iter2 = secondDesignMatrixTransposedValues->begin();
	vector<int>::const_iterator iterRows2 = secondDesignMatrixTransposedRowInd->begin();
	i = 0;
	while (iter2 != secondDesignMatrixTransposedValues->end())
	{
		rows2[i] = *iterRows2;
		vals2[i++] = *iter2;

		iter2++;
		iterRows2++;
	}
	secondDesignMatrixTransposedValues->clear();
	secondDesignMatrixTransposedRowInd->clear();

	i = 0;
	vector<int>::const_iterator iterCols2 = secondDesignMatrixTransposedColPtr->begin();
	while (iterCols2 != secondDesignMatrixTransposedColPtr->end())
	{
		cols2[i++] = *iterCols2;

		iterCols2++;
	}
	secondDesignMatrixTransposedColPtr->clear();

	firstDesignMatrixTransposed = new TSparseMatrix(fNbUnk, fNbEqn,
		vals, rows, cols);
	secondDesignMatrixTransposed = new TSparseMatrix(fNbObs, fNbEqn,
		vals2, rows2, cols2);
	int* rowinds = new int[fNbObs + 1];
	for (i = 0; i <= fNbObs; i++)
	{
		rowinds[i] = i;
	}

	TReal* vals3 = new TReal[weightMatrixValues->size()];
	vector<TReal>::const_iterator iter3 = weightMatrixValues->begin();
	i = 0;
	while (iter3 != weightMatrixValues->end())
	{
		vals3[i++] = *iter3;

		iter3++;
	}
	weightMatrixValues->clear();

	weightMatrix = new TSparseMatrix(fNbEqn, fNbEqn,
		vals3, rowinds, rowinds);

	if (fNbTotalCnstr != 0)
	{
		while (constraintFirstDesignMatrixColPtr->size() <= fNbUnk)
		{
			setConstraintNewColumn();
		}
		while (constraintFirstDesignMatrixTransposedColPtr->size() <= fNbTotalCnstr)
		{
			setConstraintTransposedNewColumn();
		}

        TReal* vals4 = new TReal[constraintFirstDesignMatrixValues->size()];
        int* cols4 = new int[constraintFirstDesignMatrixColPtr->size()];
        int* rows4 = new int[constraintFirstDesignMatrixRowInd->size()];
        vector<TReal>::const_iterator iter4 = constraintFirstDesignMatrixValues->begin();
        vector<int>::const_iterator iterRows4 = constraintFirstDesignMatrixRowInd->begin();
        i = 0;
        while (iter4 != constraintFirstDesignMatrixValues->end())
        {
            rows4[i] = *iterRows4;
            vals4[i++] = *iter4;

            iter4++;
            iterRows4++;
        }
        constraintFirstDesignMatrixValues->clear();
        constraintFirstDesignMatrixRowInd->clear();

        i = 0;
        vector<int>::const_iterator iterCols4 = constraintFirstDesignMatrixColPtr->begin();
        while (iterCols4 != constraintFirstDesignMatrixColPtr->end())
        {
            cols4[i++] = *iterCols4;

            iterCols4++;
        }
        constraintFirstDesignMatrixColPtr->clear();		

        TReal* vals5 = new TReal[constraintFirstDesignMatrixTransposedValues->size()];
        int* cols5 = new int[constraintFirstDesignMatrixTransposedColPtr->size()];
        int* rows5 = new int[constraintFirstDesignMatrixTransposedRowInd->size()];
        vector<TReal>::const_iterator iter5 = constraintFirstDesignMatrixTransposedValues->begin();
        vector<int>::const_iterator iterrows5 = constraintFirstDesignMatrixTransposedRowInd->begin();
        i = 0;
        while (iter5 != constraintFirstDesignMatrixTransposedValues->end())
        {
            rows5[i] = *iterrows5;
            vals5[i++] = *iter5;

            iter5++;
            iterrows5++;
        }
        constraintFirstDesignMatrixTransposedValues->clear();
        constraintFirstDesignMatrixTransposedRowInd->clear();

        i = 0;
        vector<int>::const_iterator itercols5 = constraintFirstDesignMatrixTransposedColPtr->begin();
        while (itercols5 != constraintFirstDesignMatrixTransposedColPtr->end())
        {
            cols5[i++] = *itercols5;

            itercols5++;
        }
        constraintFirstDesignMatrixTransposedColPtr->clear();
		TSparseMatrix* CTrans = new TSparseMatrix(fNbUnk, fNbTotalCnstr, vals5, rows5, cols5);
		TSparseMatrix* C2 = CTrans->transposed();
		delete CTrans;

		for (i = 0; i < C2->columnPointers()[C2->columnsCount()]; i++)
		{
			C2->rowIndices()[i] += fNbCnstr;
		}

		fCnstrFirstDesignMtrx = new TSparseMatrix(fNbTotalCnstr, fNbUnk,
			vals4, rows4, cols4);

		TSparseMatrix* temp = fCnstrFirstDesignMtrx->add(*C2);
		delete fCnstrFirstDesignMtrx;
		delete C2;
		fCnstrFirstDesignMtrx = temp;
	}
}

////////////////////////////////////////////////////////////////////////////////
//ACCESS METHOD FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
const TSparseMatrix* TLSInputSparseMatrices::getFirstDgnMtrxTransposed() const
{//returns a reference to the first dgn matrix
	return firstDesignMatrixTransposed;
}

const TSparseMatrix* TLSInputSparseMatrices::getSecondDgnMtrxTransposed() const
{//returns a reference to the first dgn matrix
	return secondDesignMatrixTransposed;
}

const TSparseMatrix* TLSInputSparseMatrices::getWeightMtrx() const
{
	return weightMatrix;
}

const TColumnVector& TLSInputSparseMatrices::getMisclosureVctr() const
{// returns a reference to the misclosure vector
	return *fMisclosureVector;
}


const TSparseMatrix* TLSInputSparseMatrices::getCnstrFirstDgnMtrx() const
{//returns a reference to the constraint first dgn matrix
	return fCnstrFirstDesignMtrx;
}


const TColumnVector& TLSInputSparseMatrices::getCnstrMisclosureVctr() const
{// returns a reference to the constraint misclosure vector
	return *fCnstrMisclosureVector;
}

int TLSInputSparseMatrices::getNbrUnknowns() const
{
	return fNbUnk;
}

int TLSInputSparseMatrices::getNbrEquations() const
{
	return fNbEqn;
}

int TLSInputSparseMatrices::getNbrObservations() const
{
	return fNbObs;
}

//zero or 1
int TLSInputSparseMatrices::getNbrConstraintObs() const
{
	return fNbCnstrObs;
}

//zero or 1
int TLSInputSparseMatrices::getNbrConstraints() const
{
	return fNbTotalCnstr;
}



/////////////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////////////
