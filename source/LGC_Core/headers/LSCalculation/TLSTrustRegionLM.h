/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TLS_TRUST_REGION_LM_HPP
#define TLS_TRUST_REGION_LM_HPP

#include <cmath>
#include <cstddef>
#include <iomanip> /* for nice column formatting */
#include <limits>
#include <memory>
#include <stdexcept>

#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>

#include <TLSEvaluator.h>
#include <TSparseMatrix.h>

#include "QuantileFunctions.h"

/*=========================================================================*/
/*  Solver class                                                           */
/*=========================================================================*/
class TLSTrustRegionLM
{
public:
	/*--------------------------- configuration --------------------------*/
	struct Config
	{
		int maxIterations = 500;
		// trust region radius bounds
		double deltaMax = 1e+4;
		double deltaMin = 1e+0;
		// termination crit
		double tolStep = 1.0e-7;
		// initial lambda (penalty), increase and decrease factors
		double lambdaInit = 1e-5;
		double lambdaInc = 10.0;
		double lambdaDec = 0.1;
		double lambdaMin = 1e-10;
		// trust region increase/decrease factors
		double deltaInc = 2.0;
		double deltaDec = 0.25;
		// rho= actual / predicted decrease acceptance threshold
		double rhoThreshold = 0.1;
	};

	/*----------------------------- results ------------------------------*/
	enum class Status
	{
		SuccessStep,
		MaxIterations,
		LinearSolverFailure
	};

	struct Result
	{
		// primal solution
		Eigen::VectorXd sol;
		int iterations;
		Status status;
		// final weighted residual value 0.5 * ||r(x)||^2
		double finalCost;
		// degree of freedom normalized version, LGC language 	sqrt(||r(x)||^2 / dof);
		double sigma0Aposteriori;
	};

	/*------------------------- constructor ------------------------------*/
	explicit TLSTrustRegionLM(std::shared_ptr<TLSEvaluator> evaluator, const Config &cfg);

	Result solve(const Eigen::VectorXd &x0);

private:
	bool checkConvergence(const Eigen::VectorXd &dx, Status &statusOut);
	// solving the linear least square problem with the levenberg marquardt penalty factor, can be seen as linear Karush Kuhn Tucker system https://en.wikipedia.org/wiki/Karush%E2%80%93Kuhn%E2%80%93Tucker_conditions
	bool solveKKTStep(const TLSInputMatrices &input, double lmDamping, Eigen::VectorXd &dx, Eigen::VectorXd &mult);
	// set up the normal matrix with constraints
	TSparseMatrix constructNBigRegularized(const TSparseMatrix &N1, const TSparseMatrix &Ac, double lmDamping);
	// clipping step to fit inside the trust region
	void clipStep(Eigen::VectorXd &dx, Eigen::VectorXd &mult, double delta);
	double getPredictedObjective(const TLSInputMatrices &input, const Eigen::VectorXd &dx);
	// check quality of step and adapt trust region radius if necessary
	bool checkStepAndUpdateTrustRegion(double &lambda, double &delta, const double predictedReduction, const Eigen::VectorXd &dx, const double actualReduction);
	// format information on current iterate
	void formatLine(int iter, const Eigen::VectorXd &dx, double sqrtF, double lmDamping, double predRed, double actRed, double rho, bool accept, int headerEvery) const;
	
	// helper functions LGC specific: assuming B is invertible
	double getObjective(const TLSInputMatrices &input) const;
	Eigen::VectorXd getV(const TLSInputMatrices &input) const;

	double getConstraintViolation(const TLSInputMatrices &input) const;

	std::shared_ptr<TLSEvaluator> fEvaluator;
	Config mCfg;
};
#endif
