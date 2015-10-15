#ifndef _LS_MATRICES_ADAPTER_
#define _LS_MATRICES_ADAPTER_

#include <string>
#include <Quad.h>
#include <TSparseMatrix.h>
#include <lsalgo/UEOIndices.h>

class TLSParametricMtdComputerAdapter;

class TLSInputMatricesAdapter {
public:
	friend class TLSParametricMtdComputerAdapter;

	TLSInputMatricesAdapter();
	~TLSInputMatricesAdapter();
	void setDimensions(int nbUnknowns, int nbEquations, int nbObservations, int nbCnstrObs);

	void setDimensions(int nbUnknowns, int nbEquations, int nbCnstrObs, 
		               int nbObservations, int nbConstraints, int offsetContraints);

	void setNewColumn();
	void setConstraintNewColumn();
	void setConstraintTransposedNewColumn();
	void finishedFillingMatrices();
	void clearMatrices();
	
	//!Sets a coefficient of the first design matrix
	bool setFirstDgnMtrxTransElement(int row, TReal coefficient);
	//!Sets a coefficient of the second design matrix
	bool setSecondDgnMtrxTransElement(int row, TReal coefficient);
    //!Sets a coefficient of the misclosure vector
    bool setMisclosureVectorElement(int row, TReal coeff);
    //!Sets a coefficient of the weight matrix
    bool setWeightMtrxElement(TReal coefficient);
	
    //!Sets a coefficient of the constraint first design matrix
    bool setCnstrFirstDgnMtrxElement(int row, TReal coefficient);
    //!Sets a coefficient of the constraint misclosure vector
    bool setCnstrMisclosureVectorElement(int row, TReal coeff);
    //!Sets a coefficient of the constraint first design matrix
    bool setCnstrFirstDgnMtrxTransposedElement(int column, TReal coefficient);

private:
	struct D;
	D* d;
};

class TLSResultsMatricesAdapter {
public:
	friend class TLSParametricMtdComputerAdapter;

	TLSResultsMatricesAdapter(UEOIndices ueoi);
	~TLSResultsMatricesAdapter();
	
	void				setS0APosterioriVariances(bool b) {
		fs0apost = b;
	}

	/*!@return the boolean indicating if sigma zero should be applied or not */
	bool				S0APosterioriVariances() const {
		return fs0apost;
	}

	/*!@return  the squared sigma zero */
	TReal				getSigmaZero2() const;

	TReal getUnknownsCovarMtrxElmt(int row, int column) const;

	/*!@return The size of the covar matrix*/
	int getUnkCovarMtrxRowsCount() const;

	int getResidualsVctrDimension() const;
	/*!@return an element of the residuals vector
	@param row the desired element's index*/
	TReal	getResidualsVctrElmt(int row) const;

	TReal getUnkCovarMtrxElement(int row, int column) const {
		return getUnknownsCovarMtrxElmt(row, column);
	}

	int getSolutionVctrDimension() const;
	TReal getSolutionVctrElmt(int idx) const;

	TVector* getVarObs();

private:
	struct D;
	D* d;

	// from legacy, gets set and read. Purpose not clear.
	bool fs0apost;
};

class TLSParametricMtdComputerAdapter {
public:
	//!Computes the results matrices
	/*!@param im a pointer to the LS input matrices*/
	bool computeResults(TLSInputMatricesAdapter* im, TLSResultsMatricesAdapter* rm, bool isCombinedCase, 
		                bool isFreeNetworkOrECHO, bool hasConstraints, bool calcVarObs);

	const std::string& getError() {return fError;}
	
private:
	struct D;
	D* d;

	std::string fError;
};

#endif