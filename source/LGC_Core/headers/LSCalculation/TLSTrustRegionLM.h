
/*  tls_trust_region_lm.hpp
 *  -----------------------------------------------------------
 *  Trust-Region Levenberg–Marquardt solver
 *  (C++17 – Eigen ≥ 3.3)
 *  ----------------------------------------------------------- */
#ifndef TLS_TRUST_REGION_LM_HPP
#define TLS_TRUST_REGION_LM_HPP

#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <TLSEvaluator.h>
#include <iomanip>                  /* for nice column formatting */
#include "QuantileFunctions.h"


#include <Eigen/Core>
#include <Eigen/Sparse>
#include <Eigen/SparseCholesky>

 /*=========================================================================*/
/*  Solver class                                                           */
/*=========================================================================*/
class TLSTrustRegionLM
{
public:
	/*--------------------------- configuration --------------------------*/
	struct Config
	{
		std::size_t maxIterations = 1000;
		double deltaMax = 1e+3;
		double tolGradient = 1.0e-6;
		double tolStep = 1.0e-12;
		double tolObjective = 1.0e-10;
		double lambdaInit = -1.0; /*  < 0 → heuristic    */
		double lambdaInc = 10.0; /*  > 1               */
		double lambdaDec = 0.1; /*  0 < … < 1         */
		double deltaInc = 2.0; /*  > 1               */
		double deltaDec = 0.25; /*  0 < … < 1         */
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
		std::size_t iterations;
		Status status;
		double finalCost; /* ½‖r‖²                        */
		double sigma0APosteriori;
		bool isInLimits;

	};

	/*------------------------- constructor ------------------------------*/
	explicit TLSTrustRegionLM(std::shared_ptr<TLSEvaluator> evaluator, const Config &cfg = Config()) : fEvaluator(std::move(evaluator)), mCfg(cfg)
	{
		if (!fEvaluator)
			throw std::invalid_argument("TLSTrustRegionLM: evaluator is null");
	}

	/*--------------------------- solver ---------------------------------*/
	Result solve(const Eigen::VectorXd &x0)
	{	
		// compute expected sigmas limits
		int d = fEvaluator->getIndices().EIndex - fEvaluator->getIndices().UIndex;

		limits fisherLim = {0, 0};
		if (d > 0)
		{
			double chiUp = deviates_chi_sq_0975(d);
			double chiLow = deviates_chi_sq_0025(d);
			// Limits
			fisherLim.s0PostUpLimit = sqrtq(chiUp / d);
			fisherLim.s0PostLoLimit = sqrtq(chiLow / d);
		}




		Status status = Status::MaxIterations;
		Eigen::VectorXd p;
		/* --- initial parameter vector --- */
		Eigen::VectorXd x = x0;
		fEvaluator->setParameters(x);
		fEvaluator->evaluate();
		Eigen::VectorXd             v  = fEvaluator->getMisclosure();  // size m
		Eigen::SparseMatrix<double> A = fEvaluator->getAMatrix(); // m×n
		Eigen::SparseMatrix<double> W = fEvaluator->getPMatrix(); // m×m SPD


		/* --- initial residual & Jacobian --- */
	//	Eigen::VectorXd r = fEvaluator->getResidual();

		const std::size_t n = static_cast<std::size_t>(x.size());
		double f = 0.5 * v.dot(W * v);

		/* --- initial damping λ --- */
		double lambda = mCfg.lambdaInit;
		if (lambda <= 0.0)
		{
			if (mCfg.relativeLambda)
				lambda = 1e-3;
			else
			{
				Eigen::SparseMatrix<double> H = A.transpose() *W*A;
				Eigen::VectorXd diag = H.diagonal();
				double maxDiag = (diag.size() > 0) ? diag.cwiseAbs().maxCoeff() : 1.0;
				lambda = 1.0e-3 * (maxDiag > 1.0 ? maxDiag : 1.0);
			}
		}

		/* --- trust-region radius --- */
		double delta = mCfg.deltaMax;

		/* ==================== main loop =================== */
		std::size_t k = 0;
		for (; k < mCfg.maxIterations; ++k)
		{
			/* gradient */
			Eigen::VectorXd g = A.transpose() * (W * v);
			if (g.cwiseAbs().maxCoeff() <= mCfg.tolGradient)
			{
				status = Status::SuccessGradient;
				break; /* SuccessGradient */
			}

			///*---------------- initial data ----------------*/
			//Eigen::VectorXd v = fEvaluator->getMisclosure(); // un-weighted
			//Eigen::SparseMatrix<double> A = fEvaluator->getAMatrix(); // Jacobian
			//TSparseMatrix W = fEvaluator->getPMatrix(); // diag W

			f = 0.5 * v.dot(W * v);


			/* -------- new step computation ----------------*/
			Eigen::VectorXd p;
			if (!computeLMStep(A, v, W, lambda, p))
			{
				status = Status::LinearSolverFailure;
				break;
			}


		//////	/* (JᵀJ + λ I) p = −g  or relative lambda */
		//////	Eigen::SparseMatrix<double> A = J.transpose() * J;
		//////	if (mCfg.relativeLambda)
		//////	{
		//////		for (int i = 0; i < static_cast<int>(n); ++i)
		//////			A.coeffRef(i, i) *= (1+lambda);
		//////	}
		//////	else
		//////	{
		//////		for (int i = 0; i < static_cast<int>(n); ++i)
		//////			A.coeffRef(i, i) += lambda;
		//////	}

		//////	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
		//////	solver.compute(A);
		//////	if (solver.info() != Eigen::Success)
		//////	{
		//////		status = Status::LinearSolverFailure;
		//////		break;
		//////		//return {x, k, Status::LinearSolverFailure, f};
		//////	}

		//////	//Eigen::VectorXd p = solver.solve(-g);
		//////	p = solver.solve(-g);
		//////	if (solver.info() != Eigen::Success)
		//////	{
		//////		status = Status::LinearSolverFailure;
		//////		break;
		//////		//return {x, k, Status::LinearSolverFailure, f};
		//////	}

			/* trust-region clipping */
			double pNorm = p.norm();
			if (pNorm > delta)
			{
				p *= delta / pNorm;
				pNorm = delta;
			}	
			double gNorm = g.norm();
			if (mCfg.showInfo)
			{
				if (k % 10 == 0)
				{
					std::cout << std::setw(4) << "Iter" << std::setw(12) << "|dx|" << std::setw(12) << "TR Rad" << std::setw(12) << "lambda" << std::setw(14) << "2*f"
							  << std::setw(14) << "sqrt(2*f/dof)" << std::setw(14) << "|grad|" << std::endl;
				}

				std::cout << std::setw(4) << k << std::setw(12) << pNorm << std::setw(12) << delta << std::setw(12) << lambda << std::setw(14) << 2*f << std::setw(14)
						  << sqrt(2*f / d) << std::setw(14) << gNorm << std::endl;
			}

			if (pNorm <= mCfg.tolStep * (1.0 + x.norm()))
			{
				status = Status::SuccessStep;
				break; /* SuccessStep */
			}

			/* evaluate trial point */
			TVector vTrial;
			TSparseMatrix ATrial, WTrial;
			Eigen::VectorXd xTrial = x + p;
			fEvaluator->setParameters(xTrial);
			fEvaluator->evaluate();
			vTrial = fEvaluator->getMisclosure();
			ATrial = fEvaluator->getAMatrix();
			WTrial = fEvaluator->getPMatrix();

			double fTrial = 0.5 * vTrial.dot(WTrial * vTrial);

			///* predicted reduction */
			//Eigen::VectorXd Jp = J * p;
			//double predicted = -(g.dot(p) + 0.5 * Jp.squaredNorm());
			//double rho = (f - fTrial) / predicted;
			//double fOld = f;

			/* ---------- predicted reduction ----------------------------------- *
			 *  m(p) = f + gᵀp + ½ pᵀ H p   with
			 *        g = Aᵀ W v ,   H = Aᵀ W A
			 *  We compute ½ pᵀ H p  via ½ (Ap)ᵀ W (Ap)
			 *-------------------------------------------------------------------*/
			Eigen::VectorXd Ap = A * p; // m-vector
			double modelSecond = 0.5 * Ap.dot(W * Ap); // ½ pᵀ H p
			double predicted = -(g.dot(p) + modelSecond);

			double rho = (f - fTrial) / predicted; // agreement ratio
			double fOld = f;

			/* ---------- accept / reject --------------------------------------- */
			bool accept = rho > 0.0;
			if (accept)
			{
				/* promote trial quantities */
				x = std::move(xTrial);
				v = std::move(vTrial);
				A = std::move(ATrial);
				W = std::move(WTrial);
				f = fTrial;

				/* damp-parameter update */
				lambda = (mCfg.lambdaDec * lambda > 1.0e-10) ? mCfg.lambdaDec * lambda : 1.0e-10;
			}
			else
			{
				lambda *= mCfg.lambdaInc;
			}



					/* radius update */
			if (rho < 0.25)
			{
				delta = (delta * mCfg.deltaDec > 1.0e-12) ? delta * mCfg.deltaDec : 1.0e-12;
			}
			else if (rho > 0.75 && std::abs(pNorm - delta) <= 1.0e-12)
			{
				delta = (delta * mCfg.deltaInc < mCfg.deltaMax) ? delta * mCfg.deltaInc : mCfg.deltaMax;
			}

			/* objective convergence */
			if (accept && std::abs(fOld - fTrial) <= mCfg.tolObjective * fOld)
			{
				status = Status::SuccessObjective;
				break; /* SuccessObjective */

			}
		}

		/* --- exit classification --- */
		//Status status = Status::MaxIterations;
		if (k < mCfg.maxIterations)
		{
			Eigen::VectorXd g = A.transpose() * W * v;
		//	if (g.cwiseAbs().maxCoeff() <= mCfg.tolGradient)
		//	{
		//		status = Status::SuccessGradient;
		//	}
		//	else if (std::abs(f) <= mCfg.tolObjective)
		//	{
		//		status = Status::SuccessObjective;
		//	}
		//	else if (p.norm())
		//	{
		//		status = Status::SuccessStep;
		//	}
			if (mCfg.showInfo)
			{
				std::cout << std::setw(4) << k << std::setw(12) << p.norm() << std::setw(12) << delta << std::setw(12) << lambda << std::setw(14) << 2*f << std::setw(14)
						  << sqrt(2*f/d) << std::setw(14) << g.norm() << std::endl;
				std::cout << "\n>>> Termination: " << statusMessage(status) << "  |  iterations = " << k << "  |  final f = " << f << std::endl;
			}
		}

		// compute expected sigmas limits
		double currentObjective = 2 * f;
		double sigma0APosteriori = sqrt(currentObjective / d);

		bool isInLimits = (sigma0APosteriori < fisherLim.s0PostUpLimit) && (sigma0APosteriori > fisherLim.s0PostLoLimit);
		std::cout << "S0Post" << sigma0APosteriori<< std::endl;
		std::cout << "inLimits " << isInLimits << std::endl;

		return {x, k, status, f, sigma0APosteriori, isInLimits};
	}

	/* ---------------- public data member (as requested) ---------------- */
	std::shared_ptr<TLSEvaluator> fEvaluator;

private:
	Config mCfg;
	static const std::string statusMessage(TLSTrustRegionLM::Status s)
	{
		switch (s)
		{
		case Status::SuccessGradient:
			return "gradient-norm tolerance reached";
		case Status::SuccessStep:
			return "step-size tolerance reached";
		case Status::SuccessObjective:
			return "objective-change tolerance reached";
		case Status::LinearSolverFailure:
			return "numerical factorisation failed";
		case Status::MaxIterations:
			return "maximum iterations exceeded";
		default:
			return "unknown status";
		}
	}

	/*--------------------------------------------------------------------
 *  Solve   (Aᵀ W A + λ R) p = -Aᵀ W v
 *  ‣ W   given as its diagonal  w   (size m)
 *  ‣ R   = I            if !mCfg.relativeLambda
 *        = diag(Aᵀ W A) if  mCfg.relativeLambda
 *-------------------------------------------------------------------*/
bool computeLMStep(const Eigen::SparseMatrix<double>& A,
                   const Eigen::VectorXd&             v,
                   const TSparseMatrix&             W,
                   double                             lambda,
                   Eigen::VectorXd&                   p) const
{
    /* gradient g = Aᵀ W v  */
    Eigen::VectorXd g = A.transpose() * (W * v);

    /* Hessian H = Aᵀ W A   */
    Eigen::SparseMatrix<double> H = A.transpose() * (W * A);

    /* add damping on the diagonal */
    if (mCfg.relativeLambda)
    {
        Eigen::VectorXd dH = H.diagonal();
        for (int i = 0; i < H.rows(); ++i)
            H.coeffRef(i,i) += lambda * dH(i);
    }
    else
    {
        for (int i = 0; i < H.rows(); ++i)
            H.coeffRef(i,i) += lambda;
    }

    /* sparse LDLᵀ */
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    solver.compute(H);
    if (solver.info() != Eigen::Success) return false;

    p = solver.solve(-g);
    return solver.info() == Eigen::Success;
}


};

#endif /* TLS_TRUST_REGION_LM_HPP */
