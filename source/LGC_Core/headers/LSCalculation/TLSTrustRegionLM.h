
/*  tls_trust_region_lm.hpp
 *  -----------------------------------------------------------
 *  Trust-Region Levenberg–Marquardt solver
 *  (C++17 – Eigen ≥ 3.3)
 *  ----------------------------------------------------------- */
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
		int maxIterations = 100;
		double deltaMax = 1e+4;
		double tolGradient = 1.0e-6;
		double tolRelStep = 1.0e-9;
		double tolRelObj = 1.0e-10;
		double lambdaInit = 1e-15; /*  < 0 → triggers heuristic    */
		double lambdaInc = 10.0; /*  > 1               */
		double lambdaDec = 0.1; /*  0 < … < 1         */
		double deltaInc = 2.0; /*  > 1               */
		double deltaDec = 0.25; /*  0 < … < 1         */
		//double trRadiusInit = 1000;
		double rhoThreshold = 0.1;
		bool showInfo = true;
		bool relativeLambda = false;
	};

	/*----------------------------- results ------------------------------*/
	enum class Status
	{
		SuccessGradient,
		SuccessStep,
		SuccessObjective,
		MaxIterations,
		LinearSolverFailure
	};

	struct Result
	{
		Eigen::VectorXd x;
		Eigen::VectorXd mult;
		int iterations;
		Status status;
		double finalCost; /* ½‖r‖²                        */
		double sigma0Aposteriori;
	};

	/*------------------------- constructor ------------------------------*/
	explicit TLSTrustRegionLM(std::shared_ptr<TLSEvaluator> evaluator, const Config &cfg = Config()) : fEvaluator(std::move(evaluator)), mCfg(cfg)
	{
		if (!fEvaluator)
			throw std::invalid_argument("TLSTrustRegionLM: evaluator is null");
	}

	Result solve(const Eigen::VectorXd &x0)
	{
		UEOIndices indices = fEvaluator->getIndices();
		Eigen::VectorXd xCurr = x0;
		Eigen::VectorXd xTrial = xCurr;
		Eigen::VectorXd dx = 1e+12 * Eigen::VectorXd::Ones(xCurr.size());
		Eigen::VectorXd mult = Eigen::VectorXd::Zero(indices.CIndex);
		Eigen::VectorXd newMult = mult;
		double delta = mCfg.deltaMax;
		double lmDamping = mCfg.lambdaInit;
		Eigen::VectorXd penaltyDiag = Eigen::VectorXd::Ones(xCurr.size());

		int degreeOfFreedom = indices.EIndex - indices.UIndex + indices.CIndex;
		
		// do initial evaluation
		fEvaluator->setParameters(xCurr);
		fEvaluator->evaluate();
		TLSInputMatrices modelEval_xCurr = fEvaluator->getInputMatricesCopy();
		TLSInputMatrices modelEval_xTrial = modelEval_xCurr;

		lmDamping = computeInitialDamping(modelEval_xCurr);
		
		double fCurr = 1e+12;
		double fPrev = fCurr;
		Status status;

		for (int k = 0; k < mCfg.maxIterations; ++k)
		{
			fCurr = modelEval_xCurr.getObj();
			if (CheckConvergence(dx, mult, modelEval_xCurr, fPrev, fCurr, status))
			{
				double sigma0Aposteriori = sqrt(2 * fCurr / degreeOfFreedom);
				return Result{xCurr, mult, k, status, fCurr, sigma0Aposteriori};
			}
			if (!SolveKKTStep(modelEval_xCurr, lmDamping, penaltyDiag, dx, newMult))
			{
				delta *= mCfg.deltaDec;
				lmDamping *= mCfg.lambdaInc;
				continue;
			}
			std::cout << "|dx|=" << dx.norm() << "    f=" << fCurr << "    lambda=" << lmDamping << std::endl;

			ClipStep(dx, mult, delta);

			Eigen::VectorXd xTrial = xCurr + dx;
			fEvaluator->setParameters(xTrial);
			fEvaluator->evaluate();
			modelEval_xTrial = fEvaluator->getInputMatricesCopy();

			double fTrial = modelEval_xTrial.getObj();

			double predictedObjective = PredictedObjective(modelEval_xTrial, dx);
			double predictedReduction = fCurr-predictedObjective ;
			double actualReduction = fCurr - fTrial;

			double rho = actualReduction / predictedReduction;
			// TODO: check constraint violation
			//bool accept = (rho > mCfg.rhoThreshold) && (actualReduction < 0);
			bool accept = checkStepAndUpdateTrustRegion(lmDamping, delta, rho, dx, actualReduction);
			std::cout << accept << std::endl;
			if (accept)
			{
				xCurr += dx;
				mult = newMult;
				fPrev = fCurr;
				// save one evaluation
				modelEval_xCurr = modelEval_xTrial;
			}

		}
		double sigma0Aposteriori = sqrt(2 * fCurr / degreeOfFreedom);
		return Result{xCurr, mult, mCfg.maxIterations, Status::MaxIterations, fCurr, sigma0Aposteriori};
	}

private:
	bool CheckConvergence(const Eigen::VectorXd &dx, const Eigen::VectorXd &mult, const TLSInputMatrices &input, double fPrev, double fCurr, Status &statusOut)
	{
		// 1. Check gradient norm (KKT stationarity)
		Eigen::VectorXd fGrad = input.getObjGrad();
		Eigen::VectorXd cGrad = input.getCnstrFirstDgnMtrx().transpose() * mult;
		Eigen::VectorXd grad = fGrad + cGrad;

		if (grad.norm() <= mCfg.tolGradient)
		{
			statusOut = Status::SuccessGradient;
			return true;
		}

		Eigen::VectorXd residual = input.getV();
		// 2. Check step size (relative)
		double relStep = dx.norm() / (1.0 + residual.size());
		//if (dx.norm() <= mCfg.tolRelStep * (1.0 + residual.norm()))
		if (dx.norm() <= 1e-6)
		{
			statusOut = Status::SuccessStep;
			return true;
		}

		// 3. Check relative objective change
		double absObjDiff = std::abs(fPrev - fCurr);
		if (absObjDiff <= mCfg.tolRelObj * fPrev)
		{
			statusOut = Status::SuccessObjective;
			return true;
		}

		// No convergence yet
		return false;
	}

double computeInitialDamping(const TLSInputMatrices &input)
	{
		const auto &A = input.getFirstDgnMtrx();
		const auto &invB = input.getSecondDgnBlockDiagInvMtrx();
		const auto &P = input.getWeightMtrx();
		const auto &invW = input.getWeightInvMtrx();
		const auto &Ac = input.getCnstrFirstDgnMtrx();
		const auto &W = input.getMisclosureVctr();
		const auto &Wc = input.getCnstrMisclosureVctr();

		const int n = A.cols(); // number of parameters

		TSparseMatrix invN1 = invB.transpose() * P * invB;
		TSparseMatrix N2 = A.transpose() * invN1 * A;
		Eigen::VectorXd diag = N2.diagonal();

		double maxDiag = (diag.size() > 0) ? diag.cwiseAbs().maxCoeff() : 1.0;
		return 1.0e-10 * (maxDiag > 1.0 ? maxDiag : 1.0);
	}

	bool SolveKKTStep(const TLSInputMatrices &input, double lmDamping, const Eigen::VectorXd &penaltyDiag, Eigen::VectorXd &dx, Eigen::VectorXd &mult)
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
		// construct NBig = (N2, Act
		//                   Ac, 0  )
		TSparseMatrix NBig(n + nc, n + nc);
		std::vector<TTriplet> coeffs;
		coeffs.reserve(N2.nonZeros() + 2 * Ac.nonZeros());

		// Fill in the N2 part
		for (int k = 0; k < N2.outerSize(); ++k)
		{
			for (TSparseMatrix::InnerIterator it(N2, k); it; ++it)
			{
				/* add damping on the diagonal */

				// apply LM damping
				if (it.row() == it.col())
				{
					double val = 0;
					if (mCfg.relativeLambda)
						val = it.value() * (1 + lmDamping);
					else
						val = it.value() + lmDamping;
					coeffs.push_back(TTriplet(it.row(), it.col(), val));
				}
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

		// RHS: [-AT * invN1 * W; -Wc]
		Eigen::VectorXd rhs(n + nc);
		rhs.head(n) = -A.transpose() * invN1 * W;
		rhs.tail(nc) = -Wc;

		Eigen::VectorXd sol;
		// use Cholesky decomposition if nbCnstr=0, otherwise SparseLU as positive definiteness of NBig may be violated
		if (!TSparseUtils::solveUnique(NBig, rhs, sol, (nc == 0), (nc == 0)))
		{
			logCritical() << "No solution could be found when solving equation system: Nbig * dX = -VBig (extended matrices with conditions)";
			return false;
		}
		dx = sol.head(n);
		mult = sol.tail(nc);

		return true;
	}

	void ClipStep(Eigen::VectorXd &dx, Eigen::VectorXd &mult, double delta)
	{
		double normDx = dx.norm();
		if (normDx > delta && normDx > 0.0)
		{
			double alpha = delta / normDx;
			std::cout << "clipping factor " << alpha << std::endl;
			dx *= alpha;
			mult *= alpha;
		}
	}
	double PredictedObjective(const TLSInputMatrices &input, const Eigen::VectorXd &dx)
	{
		const Eigen::VectorXd &g = input.getObjGrad();
		const auto &A = input.getFirstDgnMtrx();
		const auto &W = input.getMisclosureVctr();
		const auto &invB = input.getSecondDgnBlockDiagInvMtrx();
		const auto &P = input.getWeightMtrx();
		Eigen::VectorXd predictedResidual = invB * (A * dx + W);
		double predictedObjective = 0.5 * predictedResidual.dot(P * predictedResidual);

		return predictedObjective;
	}

	bool checkStepAndUpdateTrustRegion(double &lambda, double &delta, const double rho, const Eigen::VectorXd &dx, const double actualReduction)
	{
		double dxNorm = dx.norm();
		const bool nearBoundary = std::abs(dxNorm - delta) < 1e-12;
		const bool accept = (rho > mCfg.rhoThreshold) && (actualReduction > 0);
		//const bool accept = (actualReduction < 0);
		// lambda update
		if (accept)
			lambda = (mCfg.lambdaDec * lambda > 1.0e-10) ? mCfg.lambdaDec * lambda : 1.0e-10;
		else
			lambda *= mCfg.lambdaInc;

						/* radius update */
		if (rho < 0.25)
			// poor agreement: shrink trust region radius
			delta = (delta * mCfg.deltaDec > 1.0e-0) ? delta * mCfg.deltaDec : 1.0e-0;
		else if (rho > 0.75 && nearBoundary)
			delta = (delta * mCfg.deltaInc < mCfg.deltaMax) ? delta * mCfg.deltaInc : mCfg.deltaMax;

		return accept;
	}


	void UpdateTrustRegion(double &lambda, double &delta, double rho, const Eigen::VectorXd &dx)
	{
		const double lambdaMin = 1e-10;
		const double dxNorm = dx.norm();
		const bool nearBoundary = std::abs(dxNorm - delta) < 1e-12;

		// Case 1: bad model agreement → shrink trust region, increase damping
		if (rho < 0.25)
		{
			delta = std::max(delta * mCfg.deltaDec, 1e-12);
			lambda *= mCfg.lambdaInc;
		}
		// Case 2: very good agreement + step at boundary → expand region
		else if (rho > 0.75 && nearBoundary)
		{
			delta = std::min(delta * mCfg.deltaInc, mCfg.deltaMax);
			lambda = std::max(lambda * mCfg.lambdaDec, lambdaMin);
		}
	}

	std::shared_ptr<TLSEvaluator> fEvaluator;
	Config mCfg;
};
#endif /* TLS_TRUST_REGION_LM_HPP */
