// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TLSTrustRegionLM.h"

/*
 *  -----------------------------------------------------------
 *  Trust-Region Levenberg–Marquardt solver
 *  ----------------------------------------------------------- */

#include <cmath>
#include <cstddef>
#include <iomanip> /* for nice column formatting */
#include <limits>
#include <memory>
#include <stdexcept>

#include <TLSEvaluator.h>
#include <TLSTrustRegionLM.h>
#include <TSparseMatrix.h>

/*------------------------- constructor ------------------------------*/
TLSTrustRegionLM::TLSTrustRegionLM(std::shared_ptr<TLSEvaluator> evaluator, const Config &cfg) : fEvaluator(std::move(evaluator)), mCfg(cfg)
{
	if (!fEvaluator)
		throw std::invalid_argument("TLSTrustRegionLM: evaluator is null");
}

TLSTrustRegionLM::Result TLSTrustRegionLM::solve(const Eigen::VectorXd &x0)
{
	UEOIndices indices = fEvaluator->getIndices();
	Eigen::VectorXd xCurr = x0;
	Eigen::VectorXd xTrial = xCurr;
	const double nonConvergedStepSize = 1e+12;
	Eigen::VectorXd dx = nonConvergedStepSize * Eigen::VectorXd::Ones(xCurr.size());
	Eigen::VectorXd mult = Eigen::VectorXd::Zero(indices.CIndex);
	Eigen::VectorXd newMult = mult;
	double delta = mCfg.deltaMax;
	double lmDamping = mCfg.lambdaInit;

	int degreeOfFreedom = indices.EIndex - indices.UIndex + indices.CIndex;
	int degreeOfFreedomConvention = (degreeOfFreedom == 0) ? 1 : degreeOfFreedom;

	// do initial evaluation
	fEvaluator->setParameters(xCurr);
	fEvaluator->evaluate();
	TLSInputMatrices modelEval_xCurr = fEvaluator->getInputMatricesCopy();
	TLSInputMatrices modelEval_xTrial = modelEval_xCurr;


	// current objective
	double fCurr = 1e+12;
	// current constraint norm
	double cCurr = 1e+12;
	Status status;
	bool accept = false;
	double sigma0Aposteriori = -1;

	for (int k = 0; k < mCfg.maxIterations; ++k)
	{
		fCurr = getObjective(modelEval_xCurr);
		cCurr = getConstraintViolation(modelEval_xCurr);
		if (accept && checkConvergence(dx, status))
		{
			sigma0Aposteriori = sqrt(2 * fCurr / degreeOfFreedomConvention);
			return Result{xCurr, k, status, fCurr, sigma0Aposteriori};
		}
		if (!solveKKTStep(modelEval_xCurr, lmDamping, dx, newMult))
		{
			return Result{xCurr, k, Status::LinearSolverFailure, fCurr, sigma0Aposteriori};
		}

		clipStep(dx, mult, delta);
		

		xTrial = xCurr + dx;
		fEvaluator->setParameters(xTrial);
		fEvaluator->evaluate();
		modelEval_xTrial = fEvaluator->getInputMatricesCopy();

		double fTrial = getObjective(modelEval_xTrial);
		double cTrial = getConstraintViolation(modelEval_xTrial);

		double predictedObjective = getPredictedObjective(modelEval_xCurr, dx);
		double predictedReduction = fCurr - predictedObjective;
		double actualReduction = fCurr - fTrial;

		double rho = actualReduction / predictedReduction;
		// check predicted vs actual reduction and adapt trust region radius. also accept step if constraint improved
		accept = checkStepAndUpdateTrustRegion(lmDamping, delta, predictedReduction, dx, actualReduction) || (cTrial < cCurr);
		if (accept)
		{
			xCurr = xTrial;
			mult = newMult;
			// to save one evaluation, reuse the trial evaluation
			modelEval_xCurr = std::move(modelEval_xTrial);
		}
		sigma0Aposteriori = sqrt(2 * fCurr / degreeOfFreedomConvention);

		formatLine(k, dx, sigma0Aposteriori, lmDamping, predictedReduction, actualReduction, rho, accept, 50);
	}
	// if we arrive here, the algorithm didnt converge after maxIter iterations
	return Result{xCurr, mCfg.maxIterations, Status::MaxIterations, fCurr, sigma0Aposteriori};
}

bool TLSTrustRegionLM::checkConvergence(const Eigen::VectorXd &dx, Status &statusOut)
{
	if (dx.lpNorm<Eigen::Infinity>() <= mCfg.tolStep)
	{
		statusOut = Status::SuccessStep;
		return true;
	}
	return false;
}


bool TLSTrustRegionLM::solveKKTStep(const TLSInputMatrices &input, double lmDamping, Eigen::VectorXd &dx, Eigen::VectorXd &mult)
{
	const auto &A = input.getFirstDgnMtrx();
	const auto &invB = input.getSecondDgnBlockDiagInvMtrx();
	const auto &P = input.getWeightMtrx();
	const auto &invW = input.getWeightInvMtrx();
	const auto &Ac = input.getCnstrFirstDgnMtrx();
	const auto &W = input.getMisclosureVctr();
	const auto &Wc = input.getCnstrMisclosureVctr();

	const int n = A.cols(); // number of parameters
	const int nc = Ac.rows(); // number of constraints

	TSparseMatrix invN1 = invB.transpose() * P * invB;
	TSparseMatrix N2 = A.transpose() * invN1 * A;
	TSparseMatrix NBig = constructNBigRegularized(N2, Ac, lmDamping);


	// RHS: [-AT * invN1 * W; -Wc]
	Eigen::VectorXd rhs(n + nc);
	rhs.head(n) = -A.transpose() * invN1 * W;
	rhs.tail(nc) = -Wc;

	Eigen::VectorXd sol;
	// use Cholesky decomposition if nbCnstr=0, otherwise SparseLU as positive definiteness of NBig may be violated
	if (!TSparseUtils::solveUnique(NBig, rhs, sol, (nc == 0)))
	{
		logCritical() << "No solution could be found when solving equation system: Nbig * dX = -VBig (extended matrices with conditions)";
		return false;
	}
	dx = sol.head(n);
	mult = sol.tail(nc);

	return true;
}

TSparseMatrix TLSTrustRegionLM::constructNBigRegularized(const TSparseMatrix &N2, const TSparseMatrix &Ac, double lmDamping)
{
	// construct NBig = (N2, Act
	//                   Ac, 0  )
	const int n = Ac.cols(); // number of parameters
	const int nc = Ac.rows(); // number of constraints
	TSparseMatrix NBig(n + nc, n + nc);
	std::vector<TTriplet> coeffs;
	coeffs.reserve(N2.nonZeros() + 2 * Ac.nonZeros());

	// Fill in the N2 part
	for (int k = 0; k < N2.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(N2, k); it; ++it)
		{
			// apply LM damping on the diagonal
			if (it.row() == it.col())
				coeffs.push_back(TTriplet(it.row(), it.col(), it.value() + lmDamping));
			else
				coeffs.push_back(TTriplet(it.row(), it.col(), it.value()));
		}
	}

	// Fill the Ac and AcT
	for (int k = 0; k < Ac.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(Ac, k); it; ++it)
		{
			coeffs.push_back(TTriplet(it.row() + N2.rows(), it.col(), it.value())); // A2
			coeffs.push_back(TTriplet(it.col(), it.row() + N2.cols(), it.value())); // A2T
		}
	}
	NBig.setFromTriplets(coeffs.begin(), coeffs.end());
	return NBig;
}

void TLSTrustRegionLM::clipStep(Eigen::VectorXd &dx, Eigen::VectorXd &mult, double delta)
{
	double normDx = dx.norm();
	if (normDx > delta && normDx > 0.0)
	{
		double alpha = delta / normDx;
		dx *= alpha;
		mult *= alpha;
	}
}
double TLSTrustRegionLM::getPredictedObjective(const TLSInputMatrices &input, const Eigen::VectorXd &dx)
{
	const auto &A = input.getFirstDgnMtrx();
	const auto &W = input.getMisclosureVctr();
	const auto &invB = input.getSecondDgnBlockDiagInvMtrx();
	const auto &P = input.getWeightMtrx();
	Eigen::VectorXd predictedResidual = invB * (A * dx + W);
	double predictedObjective = 0.5 * predictedResidual.dot(P * predictedResidual);

	return predictedObjective;
}

bool TLSTrustRegionLM::checkStepAndUpdateTrustRegion(double &lambda, double &delta, const double predictedReduction, const Eigen::VectorXd &dx, const double actualReduction)
{
	double dxNorm = dx.norm();
	const bool nearBoundary = std::abs(dxNorm - delta) < 1e-12;
	const double rho = actualReduction / predictedReduction;
	const bool objStagnation = (fabsq(predictedReduction) < 1e-12);
	const bool accept = ((rho > mCfg.rhoThreshold) && (actualReduction > 0)) || objStagnation || (dx.lpNorm<Eigen::Infinity>() <= mCfg.tolStep);
	//  lambda update
	if (accept)
		lambda = (mCfg.lambdaDec * lambda > mCfg.lambdaMin) ? mCfg.lambdaDec * lambda : mCfg.lambdaMin;
	else
		lambda *= mCfg.lambdaInc;

	/* radius update */
	if (rho < 0.25)
	{
		double decreasedDelta = delta * mCfg.deltaDec;
		// poor agreement: shrink trust region radius
		delta = (decreasedDelta > mCfg.deltaMin) ? decreasedDelta : mCfg.deltaMin;
	}
	else if (rho > 0.75 && nearBoundary)
	{
		double increasedDelta = delta * mCfg.deltaInc;
		delta = (increasedDelta < mCfg.deltaMax) ? increasedDelta : mCfg.deltaMax;
	}

	return accept;
}

void TLSTrustRegionLM::formatLine(int iter, const Eigen::VectorXd &dx, double sqrtF, double lmDamping, double pRed, double aRed, double rho, bool accept, int headerEvery) const
{
	std::ostringstream header;
	header << std::scientific << std::setprecision(4);

	if (iter % headerEvery == 0)
	{
		header << std::setw(6) << "Iter"
				<< " | " << std::setw(12) << "|dx|_inf"
				<< " | " << std::setw(12) << "|dx|_2"
				<< " | " << std::setw(15) << "sigma0"
				<< " | " << std::setw(12) << "lambda"
				<< " | " << std::setw(20) << "predicted reduction"
				<< " | " << std::setw(20) << "actual reduction"
				<< " | " << std::setw(12) << "rho=aRed/pRed"
				<< " | " << std::setw(8) << "accept" << '\n';
		std::string headerStr = header.str();
		std::string underline(headerStr.length() - 1, '-');
		logInfo() << underline;
		logInfo() << headerStr;
		logInfo() << underline;
	}

	std::ostringstream logMsg;
	logMsg << std::setw(6) << iter << " | " << std::setw(12) << dx.lpNorm<Eigen::Infinity>() << " | " << std::setw(12) << dx.norm() << " | " << std::setw(15) << sqrtF
		   << " | " << std::setw(12) << lmDamping << " | " << std::setw(20) << pRed << " | " << std::setw(20) << aRed << " | " << std::setw(12) << rho << " | "
		   << std::setw(8) << (accept ? "yes" : "no");
	logInfo() << logMsg.str();
}

double TLSTrustRegionLM::getObjective(const TLSInputMatrices &input) const
{
	Eigen::VectorXd residual = getV(input);
	return 0.5 * residual.dot(input.getWeightMtrx() * residual);
}
double TLSTrustRegionLM::getConstraintViolation(const TLSInputMatrices &input) const
{
	return (input.getCnstrMisclosureVctr()).norm();
}

Eigen::VectorXd TLSTrustRegionLM::getV(const TLSInputMatrices &input) const
{
	return -input.getSecondDgnBlockDiagInvMtrx() * input.getMisclosureVctr();
}

