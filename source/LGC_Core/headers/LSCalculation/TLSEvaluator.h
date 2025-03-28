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

/*!
\ingroup Evaluator
\brief For evaluating the mathematical models at arbitrary values
original LS problem: min |PV|^2 s.t. F(x,L+V)=0 (& C(x)=0)
for iterative GN method in LGC, inputMatricFiller evaluates
- "A-matrix" A = dF/dx(x,L)
- constraint A matrix A2 = dC/dx(x)
- "Misclosure" W = F(x,L)(=F(x)-L for the relevant parametric case)
- constraint misclosure C = F(x)
As the residual V can be interpreted as function of the parameter x (we only have the "parametric" case F(x)-(L+V)=0) we also implement the residual function
- residual r(x)=V(x)=W (from 0=F(x)-(L+V)=W-V)
*/

class TLSEvaluator
{
public:
	// construct with a prepared project data
	TLSEvaluator(std::shared_ptr<TLGCData> data);
	// construct with a lgc input file
	TLSEvaluator(std::stringstream& fileStream);

	~TLSEvaluator();

	// setting and getting the parameter vector
	void setParameters(const Eigen::VectorXd &para);
	Eigen::VectorXd getEstParams();
	bool testParameterSetAndGet();
	bool testEvaluate();

<<<<<<< HEAD
=======
	// this will attempt a normal LGC solve operation
	bool tryLGCSolve(TVector& solution);

>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
	// set observations has lower priority because in LGC as of now all mathematical models are parametric
	// 	void setObservations(const Eigen::VectorXd &para);
	Eigen::VectorXd getObservations();

	// mathematical model relates parameters x with observations L and residuals V
	// general model is of the form
	// F(x,L+V)=0
	// as of today all models in LGC are parametric, which means its of the form
	// F(x,L)=Fp(x)-L
	// In general, the expression F(x,L) is called "misclosure"
	// For the iterative solution process, incremental parameter updates are computed based on the linearized linear least square problem
	// min ||Sigma^-0.5 V_k|| over (Delta x,V_k) such that F(x_k,L)+dFdx(x_k,L) Delta x + dFdL(x_k,L) V_k
	// the linearization corresponds to a first order taylor expansion of F at (x_k,L)
	// in this approach, the linearization of the model is always computed at the same value for L
	// this only results in convergence to a (x,V) that satisfies the mathematical model (F(x,L+V)=0) because the mathematical models in LGC are parametric, otherwise we would have to also compute Delta V_k steps and update the linearization points.

	// misclosure = F(x,L)
	// A matrix = dFdx(x,L)
	// B matrix = dFdL(x,L) (will be -Id for parametric models)
	// P matrix = Sigma^-1 (the weights associated to the observations).. convention such that the quadratic objective can be written as V^T P V, and P = (Sigma^-0.5)^T Sigma^-0.5  = Sigma^-1
	// constraint misclosure = C(x)
	// A2 matrix = dCdx(x)

	// todo: implement
	// triggers evaluation at the current set parameter
	bool evaluate();
	// functions are evaluated at current parameter (can be controlled by get/set)
	Eigen::VectorXd getMisclosure() const;
	TSparseMatrix getAMatrix() const;
	TSparseMatrix getBMatrix() const;
<<<<<<< HEAD
	Eigen::VectorXd getConstraintMisclosure() const;
	TSparseMatrix getA2Matrix() const;
	TSparseMatrix getPMatrix() const;
=======
	TSparseMatrix getInvBMatrix() const;
	Eigen::VectorXd getConstraintMisclosure() const;
	TSparseMatrix getA2Matrix() const;
	TSparseMatrix getPMatrix() const;
	UEOIndices getIndices() const { return fData->fUEOIndices; };
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40

private:
	// indicates tha results are ready for access
	bool isUptoDate;
	// a copy of data for manipulating parameter and observation values.
	std::shared_ptr<TLGCData> fData;
	TLSInputMatricesFiller *fMatFiller;
	TLSInputMatrices *iMat;

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

};

#endif