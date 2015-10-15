/*SURVEYLIB VERSION !!!!!!!!!!!!!!!!!!!!!!!*/

//TLSInputMatrices.h : implementation file
// class for input matrices as defined for survey purposes
// and for the least squares solving algorithm
/***DEBUG*///
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "TLSInputMatrices.h"


/////////////////////////////////////////////////////////////////////////////////
//CONSTRUCTOR / DESTRUCTOR
/////////////////////////////////////////////////////////////////////////////////
TLSInputMatrices::TLSInputMatrices()
{//Constructor
	
	firstDesignMatrix = nullptr;
	secondDesignMatrix = nullptr;
	weightMatrix = nullptr;
	weightInvMatrix = nullptr;
	weightUnkMatrix = nullptr;
	fCnstrFirstDesignMtrx = nullptr;
	
	fMisclosureVector = nullptr;
	fCnstrMisclosureVector = nullptr;

	fNbUnk = 0;
	fNbEqn = 0;
	fNbObs = 0;
	fNbCnstr = 0;
	fNbCnstrObs = 0;
}


TLSInputMatrices::~TLSInputMatrices()
{//Destructor
	clearMatrices();
}



/////////////////////////////////////////////////////////////////////////////////
//SET FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
void TLSInputMatrices::setDimensions(int unknowns, int equations, int observations,  int cnstrObs)
{//sets the dimensions of the matrices

	fNbUnk = unknowns;
	fNbObs = observations; // number of observations + constraint observations
	fNbEqn = equations;
	fNbCnstrObs = cnstrObs;

	clearMatrices();
	fMisclosureVector = new TVector(fNbEqn);

	firstDesignMatrix = new TSparseMatrix(equations, unknowns);
	secondDesignMatrix = new TSparseMatrix(equations, observations /*+ cnstrObs*/);
	weightMatrix = new TSparseMatrix(observations /*+ cnstrObs*/, observations /*+ cnstrObs*/);
	weightInvMatrix = new TSparseMatrix(observations /*+ cnstrObs*/, observations /*+ cnstrObs*/);
	weightUnkMatrix = new TSparseMatrix(unknowns, unknowns);
}


void TLSInputMatrices::setDimensions(int unknowns, int equations, int observations, int nbCnstrObs, int constraints)
{//sets the dimensions of the matrices

	fNbUnk = unknowns;
	fNbObs = observations; // number of observations + constraint observations
	fNbEqn = equations;
	fNbCnstr = constraints;
	fNbCnstrObs = nbCnstrObs;

	clearMatrices();
	fMisclosureVector = new TVector(fNbEqn);
	fCnstrMisclosureVector = new TVector(constraints);

	firstDesignMatrix = new TSparseMatrix(equations, unknowns);
	secondDesignMatrix = new TSparseMatrix(equations, observations /*+ cnstrObs*/);
	weightMatrix = new TSparseMatrix(observations /*+ cnstrObs*/, observations /*+ cnstrObs*/);
	weightInvMatrix = new TSparseMatrix(observations /*+ cnstrObs*/, observations /*+ cnstrObs*/);
	weightUnkMatrix = new TSparseMatrix(unknowns, unknowns);
	fCnstrFirstDesignMtrx = new	TSparseMatrix(constraints, unknowns);
}

void TLSInputMatrices::clearMatrices()
{
	// TODO: gets deleted externally, change to internal deletion
	
	if (firstDesignMatrix != nullptr) {
		delete firstDesignMatrix;
		firstDesignMatrix = nullptr;
	}
	if (secondDesignMatrix != nullptr) {
		delete secondDesignMatrix;
		secondDesignMatrix = nullptr;
	}
	if (weightMatrix != nullptr) {
		delete weightMatrix;
		weightMatrix = nullptr;
	}
	if (weightInvMatrix != nullptr) {
		delete weightInvMatrix;
		weightInvMatrix = nullptr;
	}
	if (weightUnkMatrix != nullptr) {
		delete weightUnkMatrix;
		weightUnkMatrix = nullptr;
	}
	if (fMisclosureVector != nullptr) {
		delete fMisclosureVector;
		fMisclosureVector = nullptr;
	}
	if (fCnstrFirstDesignMtrx != nullptr) {
		delete fCnstrFirstDesignMtrx;
		fCnstrFirstDesignMtrx = nullptr;
	}
	if (fCnstrMisclosureVector != nullptr) {
		delete fCnstrMisclosureVector;
		fCnstrMisclosureVector = nullptr;
	}
	
}


bool TLSInputMatrices::setFirstDgnMtrxElement(MatrixIndex row, MatrixIndex column, TReal coeff)
{
	try {
	if (0 <= row && row < fNbEqn && 0 <= column && column < fNbUnk)
		firstDesignMatrix->coeffRef(row,column) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}


bool TLSInputMatrices::setSecondDgnMtrxElement(MatrixIndex row, MatrixIndex column, TReal coeff)
{
	try {
	if (0 <= row && row < fNbEqn && 0 <= column && column < fNbObs)
		secondDesignMatrix->insert(row,column) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}


bool TLSInputMatrices::setMisclosureVectorElement(MatrixIndex row, TReal coeff)
{
	try {
		(*fMisclosureVector)(row) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}


bool TLSInputMatrices::setWeightMtrxElement(MatrixIndex row, MatrixIndex column, TReal coeff)
{
	try {
	if (0 <= row && row < fNbObs && 0 <= column && column < fNbObs)
		weightMatrix->insert(row,column) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}

bool TLSInputMatrices::setWeightInvMtrxElement(MatrixIndex row, MatrixIndex column, TReal coeff)
{
	try {
	if (0 <= row && row < fNbObs && 0 <= column && column < fNbObs)
		weightInvMatrix->insert(row,column) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}

bool TLSInputMatrices::setWeightUnkMtrxElement(MatrixIndex row, MatrixIndex column, TReal coeff)
{
	try {
	if (0 <= row && row < fNbUnk && 0 <= column && column < fNbUnk)
		weightUnkMatrix->insert(row,column) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}

bool TLSInputMatrices::setCnstrFirstDgnMtrxElement(MatrixIndex row, MatrixIndex column, TReal coeff)
{
	try {
	if (0 <= row && row < fNbCnstr && 0 <= column && column < fNbUnk)
		fCnstrFirstDesignMtrx->insert(row,column) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}


bool TLSInputMatrices::setCnstrMisclosureVectorElement(MatrixIndex row, TReal coeff)
{
	try {
		(*fCnstrMisclosureVector)(row) = coeff;
	} catch(...) {
		return false;
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////
//ACCESS METHOD FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
const TSparseMatrix* TLSInputMatrices::getFirstDgnMtrx() const
{//returns a reference to the first dgn matrix
	return firstDesignMatrix;
}

const TSparseMatrix* TLSInputMatrices::getSecondDgnMtrx() const
{//returns a reference to the first dgn matrix
	return secondDesignMatrix;
}

const TSparseMatrix* TLSInputMatrices::getWeightMtrx() const
{
	return weightMatrix;
}

const TSparseMatrix* TLSInputMatrices::getWeightInvMtrx() const
{
	return weightInvMatrix;
}

const TSparseMatrix* TLSInputMatrices::getWeightUnkMtrx() const
{
	return weightUnkMatrix;
}

const TVector& TLSInputMatrices::getMisclosureVctr() const
{// returns a reference to the misclosure vector
	return *fMisclosureVector;
}


const TSparseMatrix* TLSInputMatrices::getCnstrFirstDgnMtrx() const
{//returns a reference to the constraint first dgn matrix
	return fCnstrFirstDesignMtrx;
}


const TVector& TLSInputMatrices::getCnstrMisclosureVctr() const
{// returns a reference to the constraint misclosure vector
	return *fCnstrMisclosureVector;
}

int TLSInputMatrices::getNbrUnknowns() const
{
	return fNbUnk;
}

int TLSInputMatrices::getNbrEquations() const
{
	return fNbEqn;
}

int TLSInputMatrices::getNbrObservations() const
{
	return fNbObs;
}

//zero or 1
int TLSInputMatrices::getNbrConstraintObs() const
{
	return fNbCnstrObs;
}

//zero or 1
int TLSInputMatrices::getNbrConstraints() const
{
	return fNbCnstr;
}


///////////////////////////////////////////////////////////////////////////////
//DEBUG METHOD : saves the content of the matrices to a text file
///////////////////////////////////////////////////////////////////////////////
void TLSInputMatrices::saveMatricesToFile(int nbIter) const{

	// TODO: fix
	std::ostringstream oss;
	oss << "C:\\temp\\inputMatrices" << nbIter << ".txt";
	std::string fileName = oss.str();

	std::ofstream of(fileName.c_str(), std::ios::out);
	if (!of){
		std::cerr << "Impossible d'ouvrir le fichier C:\\temp\\inputMatrices.txt" << '\n';
		std::exit (1);
	}

	of << std::setprecision(9);

	of << "Number of Unknowns : " << fNbUnk << std::endl;
	of << "Number of Observations : " << fNbObs << std::endl;
	of << "Number of Equations : " << fNbEqn << std::endl << std::endl;

	of << "***********************" << std::endl;
	of << "* FIRST DESIGN MATRIX *" << std::endl;
	of << "***********************" << std::endl << std::endl;
	
	of << *firstDesignMatrix;
	of << std::endl << std::endl;


	of << "*********************" << std::endl;
	of << "* MISCLOSURE VECTOR *" << std::endl;
	of << "*********************" << std::endl << std::endl;

	of << *fMisclosureVector;
	of << std::endl << std::endl;


	if (fCnstrFirstDesignMtrx != 0)
	{
		of << "**********************************" << std::endl;
		of << "* CONSTRAINT FIRST DESIGN MATRIX *" << std::endl;
		of << "**********************************" << std::endl << std::endl;
		
		of << fCnstrFirstDesignMtrx;
		of << std::endl << std::endl;
	}


	if (fCnstrMisclosureVector != 0)
	{
		of << "********************************" << std::endl;
		of << "* CONSTRAINT MISCLOSURE VECTOR *" << std::endl;
		of << "********************************" << std::endl << std::endl;
		
		of << fCnstrMisclosureVector;
		of << std::endl << std::endl;
	}


	of << "*****************" << std::endl;
	of << "* WEIGHT MATRIX *" << std::endl;
	of << "*****************" << std::endl << std::endl;
	
	of << *weightMatrix;
	of << std::endl << std::endl;



	of << "************************" << std::endl;
	of << "* SECOND DESIGN MATRIX *" << std::endl;
	of << "************************" << std::endl << std::endl;

	of << *secondDesignMatrix;
	of << std::endl << std::endl;

	of.close();
}


/////////////////////////////////////////////////////////////////////////////////
//END
/////////////////////////////////////////////////////////////////////////////////