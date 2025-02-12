/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_EVALUATOR
#define TLS_EVALUATOR

// SURVEYLIB
#include <Behavior.h>
#include <TALSComputer.h>
// LGC
#include <memory>

#include <Eigen/Dense>

#include <TLSConsistencyCheck.h>
#include <TLSResultsMatricesExtractor.h>
#include <TVAbstractAlgorithm.h>

#include "TLSInputMatricesFiller.h"

// class TLSInputMatricesFiller;
// class TLSResultsMatrices;

/*!
\ingroup Evaluator
\brief For evaluating the mathematical models at arbitrary values
original LS problem: min |PV|^2 s.t. F(x,L+V)=0 (& C(x)=0)
for iterative GN method in LGC, inputMatricFiller evaluates
- "A-matrix" A = dF/dx(x,L)
- constraint A martrix A2 = dC/dx(x)
- "Misclosure" W = F(x,L)(=F(x)-L for the relevant parametric case)
- constraint misclosure C = F(x)
As the residual V can be interpreted as function of the parameter x (we only have the "parametric" case F(x)-(L+V)=0) we also implement the residual function
- residual r(x)=V(x)=W (from 0=F(x)-(L+V)=W-V)
*/

struct maskData
{
	std::vector<int> parameterIndices;
	std::vector<int> equationIndices;
};
class TLSEvaluator
{
public:
	TLSEvaluator(std::shared_ptr<TLGCData> data);

	~TLSEvaluator();

	Eigen::VectorXd getMisclosure(bool useMask = true);
	Eigen::VectorXd getConstraintMisclosure(bool useMask = true);
	// using the relation W+Bv=0, assuming B is invertible
	Eigen::VectorXd getResidual(bool useMask = true);
	Eigen::VectorXd getWeightedResidual(bool useMask = true);
	double getObjective(bool useMask = true);

	void setParameters(Eigen::VectorXd para, bool useMask = true);
	// first design matrix
	const TSparseMatrix getA(bool useMask = true);
	// first design matrix
	const TSparseMatrix getB(bool useMask = true);
	//// constraint first design matrix
	const TSparseMatrix getA2(bool useMask = true);
	// weights
	const TSparseMatrix getPv(bool useMask = true);
	const TSparseMatrix getSqrtPv(bool useMask = true);

	Eigen::VectorXd getEstParams(bool useMask = true);
	UEOIndices dimensions;
	void testSetterAndGetter();
	bool testSetterEffect();
	// maskData vectors of indices of active parameters and active equations
	maskData currentMask;
	// reset the mask to default: consider all equations and all parameters
	void unmask();

private:
	// a copy of data for manipulating parameter and observation values.
	std::shared_ptr<TLGCData> fData;
	// TLGCData fData;
	TLSInputMatricesFiller *fMatFiller;
	TLSInputMatrices *iMat;
	// update iMat objects by evaluating at current parameter/observation
	bool evaluate();
	// indicating that iMat object corresponds to evaluation at current parameter. reset to false in any setParam method call
	bool isUptoDate = false;

	// helper methods for setting and getting current parameter and observation values

	// Parameter related
	// setter helpers
	void setPointParams(Eigen::VectorXd para);
	void setAngleParams(Eigen::VectorXd para);
	void setPlaneParams(Eigen::VectorXd para);
	void setLengthParams(Eigen::VectorXd para);
	void setTransformationParams(Eigen::VectorXd para);
	void setLineParams(Eigen::VectorXd para);

	// getter helpers
	void getPointParams(Eigen::VectorXd &para);
	void getAngleParams(Eigen::VectorXd &para);
	void getPlaneParams(Eigen::VectorXd &para);
	void getLengthParams(Eigen::VectorXd &para);
	void getTransformationParams(Eigen::VectorXd &para);
	void getLineParams(Eigen::VectorXd &para);

	// Observation related
	void setObservations(Eigen::VectorXd L);
	void getObservations(Eigen::VectorXd &L);
};

#endif