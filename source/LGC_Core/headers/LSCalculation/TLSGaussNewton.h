/*
© Copyright CERN 2000-2025. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_GAUSSNEWTON
#define TLS_GAUSSNEWTON

// LGC
#include <Eigen/Dense>

#include <TLSEvaluator.h>

///*!
//\ingroup GaussNewton
//\brief Gauss Newton method
//*/
struct GNresult// : public Serializable
{
	Eigen::VectorXd solution;
	Eigen::VectorXd residual;
	double objective = -1;
	int nIterations = 0;
	bool success = false;
	bool isInLimits = false;
	double sigma0Aposteriori;
	//void serialize(ObjectSerializer &obj) const;

	//	GNresult() = default;
};
//struct solverConfig : public Serializable
//{
//	// plotLevel
//	// 0 -> show no information
//	// 1 -> show information at end of iterations
//	// 2 -> show information after each step
//	int plotLevel = 0;
//	bool useArmijo = false;
//	bool useLM = false;
//	double LMpenalty = 0;
//	int maxIter = 0;
//	double terminationTol = 0;
//	std::vector<int> penalizedIndices;
//	virtual void serialize(ObjectSerializer &obj) const override;
//	//
//	solverConfig(int plotLevel, bool useArmijo, bool useLM, double LMpenalty, int maxIter, double terminationTol) :
//		plotLevel(plotLevel), useArmijo(useArmijo), useLM(useLM), LMpenalty(LMpenalty), maxIter(maxIter), terminationTol(terminationTol){};
//
//	solverConfig() = default;
//};

class TLSGaussNewton // public Serializable
{
public:
	TLSGaussNewton(std::shared_ptr<TLSEvaluator> evaluator);
	//~TLSGaussNewton();
	// apply GN solver
	TVector solve(TVector initial);

private:
	double computeObjective(TSparseMatrix &A, TSparseMatrix &invB, TSparseMatrix &Pv, TVector &W, TVector &dxTrial);
	double computeObjective(TSparseMatrix &A, TSparseMatrix &invB, TSparseMatrix &Pv, TVector &W);
	TVector computeRegularizedStep(TSparseMatrix &N2, TSparseMatrix &C, TVector &rhs, double lmPenalty);

	bool decideAcceptance(double currObj, double predObj, double trialObj);

	TSparseMatrix getInvN1Matrix(TSparseMatrix &invB, TSparseMatrix &Pv);
	TSparseMatrix getN2Matrix(TSparseMatrix &A, TSparseMatrix &invN1);
	TSparseMatrix getNBig(TSparseMatrix &N2, TSparseMatrix &C, double lmCoefficient);
	std::shared_ptr<TLSEvaluator> fEvaluator;
	UEOIndices indices;
};

#endif