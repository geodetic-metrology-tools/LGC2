#include <algorithm>
#include <iostream>

#include <Logger.hpp>
#include <TLSGaussNewtonSolver.h>
#include <TLSGraph.h>

#include "QuantileFunctions.h"

TLSGaussNewtonSolver::TLSGaussNewtonSolver(std::shared_ptr<TLSEvaluator> evaluator) : fEvaluator(evaluator)
{
	resetOptions();
}

GNresult TLSGaussNewtonSolver::solve()
{
	// Gauss Newton with armijo stepsize regularization
	Eigen::VectorXd parameterIterate = fEvaluator->getEstParams();
	Eigen::VectorXd grad = fEvaluator->getGradient();
	Eigen::VectorXd direction(fEvaluator->dimensions.UIndex);
	direction.setConstant(1);

	std::string headerLine(100, '~');
	std::cout << headerLine << std::endl;
	std::cout << "Starting Gauss-Newton iterations. Dimensions: n_par = " << fEvaluator->currentMask.parameterIndices.size()
			  << ", n_eq = " << fEvaluator->currentMask.equationIndices.size() << std::endl;
	std::cout << "Regularizations used: ";
	if (fConfig.useArmijo)
	{
		std::cout << "Armijo . ";
	}
	if (fConfig.useLM)
	{
		std::cout << "LevenbergMarquardt regularization .";
	}
	std::cout << std::endl;

	int itIdx = 0;
	bool stepsizeCrit = false;
	bool maxIterReached = false;
	double sigma0 = 1;
	double stepsize = 1;
	double currentLambda = 0;
	if (fConfig.useLM)
	{
		currentLambda = 1;
	}
	Eigen::VectorXd residual;
	while (stepsizeCrit == false && maxIterReached == false)
	{
		fEvaluator->setParameters(parameterIterate);
		// compute the search direction
		TSparseMatrix J = fEvaluator->getWeightedResidualJacobian();
		Eigen::VectorXd r = fEvaluator->getWeightedResidual();
		//  compute the gradient along this direction. Needed for the armijo linesearch
		// grad = getGradient(parameterIterate);
		grad = fEvaluator->getGradient();
		// compute the current objective to compare the gradient predicted descent in the search direction with the true descent.
		sigma0 = fEvaluator->getObjective();
		residual = fEvaluator->getResidual();

		if (fConfig.useLM)
		{
			direction = lmStep(parameterIterate, currentLambda);
		}
		else
		{
			// direction = getGNDirection(parameterIterate, 0);
			direction = getGNDirection(r, J, currentLambda);
			if (direction.norm() > 1e+12)
			{
				std::cout << "Computed direction too big, stopping iterations" << std::endl;
				break;
			}
		}

		if (fConfig.useArmijo)
		{
			stepsize = 1;
			stepsize = backtrackingArmijoStepsize(sigma0, parameterIterate, direction);

			// apply the armijo stepsize
			direction *= stepsize;
		}
		parameterIterate += direction;
		// parameterIterate += stepsize * direction;

		if (fConfig.plotLevel == 2)
		{
			// stepsize=armijo stepsize
			if (itIdx % 20 == 0)
			{
				printf(" %4s %15s %10s %10s %6s %10s  \n", "It", "sigma", "|grad|", "|dx|", "stepsize", "LM lambda");
			}
			printf(" %4i %4.9e %4.4e %4.4e %4.4f  %4.4e\n", itIdx, sigma0, grad.norm(), direction.norm(), stepsize, currentLambda);
		}

		itIdx++;
		stepsizeCrit = (direction.cwiseAbs().maxCoeff() < fConfig.terminationTol);
		// stepsizeCrit = (stepsize * direction.cwiseAbs().maxCoeff() < fConfig.terminationTol);
		maxIterReached = (itIdx >= fConfig.maxIter);
	}

	// compute expected sigmas limits
	int d = fEvaluator->dimensions.EIndex - direction.rows();

	double chiUp = deviates_chi_sq_0975(d);
	double chiLow = deviates_chi_sq_0025(d);
	// Limits
	double s0PostUpLimit = sqrtq(chiUp / d);
	double s0PostLoLimit = sqrtq(chiLow / d);
	double sigma0Aposteriori = sqrtq(sigma0);
	if (d != 0)
	{
		sigma0Aposteriori *= sqrtq(1.0 / d);
	}
	bool isInLimits = (sigma0Aposteriori < s0PostUpLimit) && (sigma0Aposteriori > s0PostLoLimit);

	// write the result structure
	GNresult result;
	result.nIterations = itIdx;
	result.solution = parameterIterate;
	result.residual = residual;
	result.objective = sigma0;
	result.sigma0Aposteriori = sigma0Aposteriori;
	result.success = stepsizeCrit && (!maxIterReached);
	result.isInLimits = isInLimits;

	// final plot
	if (fConfig.plotLevel > 0)
	{
		std::cout << "Final Results" << std::endl;
		std::cout << "Success Status = " << result.success << std::endl;
		printf(" %4s %15s %10s %10s %6s  \n", "It", "sigma", "|grad|", "|dx|", "stepsize");
		printf(" %4i %4.9e %4.4e %4.4e %4.4f  \n", itIdx - 1, sigma0, grad.norm(), direction.norm(), stepsize);
		std::cout << headerLine << std::endl;
	}

	return result;
}

void TLSGaussNewtonSolver::setOption(std::string option, bool flag)
{
	if (option == "useArmijo")
	{
		fConfig.useArmijo = flag;
		return;
	}
	if (option == "useLM")
	{
		fConfig.useLM = flag;
		return;
	}

	throw std::runtime_error("Option " + option + " with argument type bool does not exist.");
}

void TLSGaussNewtonSolver::setOption(std::string option, double value)
{
	if (option == "terminationTol")
	{
		fConfig.terminationTol = value;
		return;
	}

	throw std::runtime_error("Option " + option + " with argument type double does not exist.");
}

void TLSGaussNewtonSolver::setOption(std::string option, int value)
{
	if (option == "maxIter")
	{
		fConfig.maxIter = int(value);
		return;
	}

	if (option == "plotLevel")
	{
		fConfig.plotLevel = value;
		return;
	}

	throw std::runtime_error("Option " + option + " with argument type int does not exist.");
}
void TLSGaussNewtonSolver::resetOptions()
{
	// set the default values
	// default = full step GN, no Armijo, no LM regularization, 100 iterations
	fConfig.plotLevel = 1;
	fConfig.maxIter = 100;
	fConfig.useArmijo = false;
	fConfig.useLM = false;
	fConfig.LMpenalty = 1e-2;
	fConfig.terminationTol = 1e-6;
}

Eigen::VectorXd TLSGaussNewtonSolver::getGNDirection(Eigen::VectorXd r, TSparseMatrix &J, double LMLambda)
{
	int nx = fEvaluator->dimensions.UIndex;
	int ne = fEvaluator->dimensions.EIndex;

	TSparseMatrix Nregularized = J.transpose() * J;
	for (int j = 0; j < nx; j++)
	{
		Nregularized.coeffRef(j, j) *= (LMLambda + 1);
	}
	// Ndiag.diagonal() = N.diagonal();
	// Eigen::MatrixXd Ndiag = Eigen::MatrixXd::Identity(nx, nx);
	// Eigen::MatrixXd Nregularized=N+LMLambda *Ndiag;
	//  if (LMLambda > 0)
	//  {
	//  	TSparseMatrix Nregularized = N + LMLambda * Ndiag;
	//  }
	Eigen::MatrixXd Ndense = Nregularized.toDense();
	Eigen::VectorXd b = -J.transpose() * r;
	// solve Nreg * dx = b to compute dx
	// Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> decomp(Nregularized);
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> decomp(Ndense.sparseView());
	if (decomp.info() != Eigen::Success)
	{
		throw std::logic_error("Decomposition during Gauss Newton step failed.");
	}

	return decomp.solve(b);
	// return Nregularized.fullPivHouseholderQr().solve(b);

	// alternative: use qr decomposition
	///    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> qrSolver;
	///    Eigen::MatrixXd Jaugmented = Eigen::MatrixXd::Zero(ne + nx, nx);
	///    Jaugmented.topRows(ne) = J;
	///    Eigen::MatrixXd diagPenalty= Eigen::MatrixXd::Zero(nx, nx);
	///    Eigen::MatrixXd N = J.transpose() * J;
	///    for (int j = 0; j < nx; j++)
	///    {
	///    	diagPenalty.diagonal()[j] = sqrt(N.diagonal()[j]);
	///    }

	///    Jaugmented.bottomRows(nx) = sqrt(LMLambda) * diagPenalty;
	///    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>> qrSolver;
	///    qrSolver.compute(Jaugmented.sparseView());
	///    Eigen::VectorXd bAugmented = Eigen::VectorXd::Zero(ne + nx);
	///    bAugmented.topRows(ne) = -r;
	///    //return Jaugmented.fullPivHouseholderQr().solve(bAugmented);
	///    return qrSolver.solve(bAugmented);
}

double TLSGaussNewtonSolver::backtrackingArmijoStepsize(double sigma0, Eigen::VectorXd x0, Eigen::VectorXd direction)
{
	// |f(x0)|^2 = sigma0, search direction 'direction'
	// armijo parameters
	// tau: reduction in case Armijo–Goldstein condition is not satisfied
	double tau = 0.5;
	// c: Armijo–Goldstein condition: real descent vs descent predicted by gradient
	double c = 0.25;
	// fEvaluator
	double expectedDescent = fEvaluator->getGradient().dot(direction);
	double alpha = 1;
	Eigen::VectorXd trialParameter = x0 + alpha * direction;
	fEvaluator->setParameters(trialParameter);
	// Eigen::VectorXd trialResidual = fEvaluator->getResidual();
	// double trialSigma = trialResidual.transpose() * fEvaluator->getPv() * trialResidual;
	double trialSigma = fEvaluator->getObjective();

	double realDescent = trialSigma - sigma0;
	// testing armijo goldstein descent condition (real descent has to be at least stepsize * c * full step expected descent )
	while (c * alpha * expectedDescent < realDescent && alpha > 1e-1)
	{
		// reduce stepsize
		alpha *= tau;
		trialParameter = x0 + alpha * direction;
		fEvaluator->setParameters(trialParameter);
		// trialResidual = fEvaluator->getResidual();
		trialSigma = fEvaluator->getObjective();
		realDescent = trialSigma - sigma0;
	}
	return alpha;
}

Eigen::VectorXd TLSGaussNewtonSolver::lmStep(Eigen::VectorXd p, double &lambda)
{
	double Lup = 11;
	double Ldown = 9;
	double eps4 = 0.1;
	fEvaluator->setParameters(p);
	Eigen::VectorXd weightedRes = fEvaluator->getWeightedResidual();
	TSparseMatrix weightedResJac = fEvaluator->getWeightedResidualJacobian();
	Eigen::VectorXd step(fEvaluator->dimensions.UIndex);
	double rho = 0;
	while (rho < eps4)
	{
		// compute L step urrent lambda
		step = getGNDirection(weightedRes, weightedResJac, lambda);
		Eigen::VectorXd pUpdated = p + step;
		fEvaluator->setParameters(pUpdated);
		Eigen::VectorXd updatedWeightedRes = fEvaluator->getWeightedResidual();
		// real descent
		double nominator = pow2(weightedRes.norm()) - pow2(updatedWeightedRes.norm());
		// expected descent
		double denominator = fabs(pow2(weightedRes.norm()) - pow2((weightedRes + weightedResJac * step).norm()));
		rho = nominator / denominator;
		if (rho < eps4)
		{
			// increase the lm regularization
			lambda *= Lup;
			// std::cout << "lambda increased " << lambda << std::endl;
			double maxL = 1e+7;
			if (lambda > maxL)
			{
				lambda = maxL;
				// break;
			}
		}
		else
		{ // decrease the LM reg
			lambda *= (1 / Ldown);
			lambda = std::max(lambda, 1e-7);
			// std::cout << "lambda decreased " << lambda << std::endl;
			//  do step
			// break;
		}
	}

	return step;
}

Eigen::SparseMatrix<double> TLSGaussNewtonSolver::getDiagonalLMScaleFactor(Eigen::SparseMatrix<double> &M)
{
	int colDim = M.cols(), rowDim = M.rows();
	if (colDim != rowDim)
	{
		throw std::runtime_error("Levenberg Marquardt Scalefactor needs to be computed with a square matrix.");
	}
	Eigen::SparseMatrix<double> result(colDim, colDim);
	Eigen::VectorXd diagonal = M.diagonal();
	for (int j = 0; j < colDim; j++)
	{
		result.coeffRef(j, j) = diagonal(j);
	}
	return result;
}

void GNresult::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("solution", solution);
	obj.addProperty("residual", residual);
	obj.addProperty("objective", objective);
	obj.addProperty("Iterations", nIterations);
	obj.addProperty("isInLimits", isInLimits);
	obj.addProperty("sigma0Aposteriori", sigma0Aposteriori);
}

void solverConfig::serialize(ObjectSerializer &obj) const
{
	obj.addProperty("plotLevel", plotLevel);
	obj.addProperty("useArmijo", useArmijo);
	obj.addProperty("useLM", useLM);
	obj.addProperty("LMpenalty", LMpenalty);
	obj.addProperty("maxIter", maxIter);
	obj.addProperty("terminationTol", terminationTol);
}
