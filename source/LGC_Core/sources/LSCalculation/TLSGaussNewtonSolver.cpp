#include <TLSGaussNewtonSolver.h>
#include <Logger.hpp>
#include "QuantileFunctions.h"
#include <iostream>


TLSGaussNewtonSolver::TLSGaussNewtonSolver(std::shared_ptr<TLSEvaluator> evaluator) : fEvaluator( evaluator )
{
	resetOptions();
}

GNresult TLSGaussNewtonSolver::solve()
{
	// Gauss Newton with armijo stepsize regularization
	Eigen::VectorXd parameterIterate = fEvaluator->getEstParams();
	Eigen::VectorXd grad = getGradient(parameterIterate);
	Eigen::VectorXd direction(fEvaluator->dimensions.UIndex);
	direction.setConstant(1);

	std::string headerLine(100, '~');
	std::cout << headerLine << std::endl;
	std::cout << "Starting Gauss-Newton iterations" << std::endl;
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
	double sigma0=1;
	double stepsize = 1;
	Eigen::VectorXd residual;
	while (stepsizeCrit == false && maxIterReached == false)
	{
		// compute the search direction
		direction = getGNDirection(parameterIterate);
		// compute the gradient along this direction. Needed for the armijo linesearch
		grad = getGradient(parameterIterate);
		// compute the residual and the weighted objective to compare the real descent vs the gradient predicted descent in the armijo linesearch method
		residual = fEvaluator->getResidual();
		sigma0 = residual.transpose() * fEvaluator->getPv() * residual;
		stepsize = 1;
		if (fConfig.useArmijo)
		{
			stepsize = backtrackingArmijoStepsize(sigma0, parameterIterate, direction);
		}

		// do the regularized step
		parameterIterate += stepsize * direction;

		if (fConfig.plotLevel == 2)
		{
			// stepsize=armijo stepsize
			if (itIdx % 20 == 0)
			{
				printf(" %4s %10s %10s %10s %6s  \n", "It", "sigma", "|grad|", "|dx|", "stepsize");
			}
			printf(" %4i %4.4e %4.4e %4.4e %4.4f  \n", itIdx, sigma0, grad.norm(), direction.norm(), stepsize);
		}
		if (direction.norm() > 1e+12)
		{
			std::cout << "Step too big, stopping iterations" << std::endl;
			break;
		}

		itIdx++;
		stepsizeCrit = (direction.cwiseAbs().maxCoeff() < fConfig.terminationTol);
		maxIterReached =  (itIdx >= fConfig.maxIter);
	}


	// compute expected sigmas limits
	int d = residual.rows() - direction.rows();
	double chiUp = deviates_chi_sq_0975(d);
	double chiLow = deviates_chi_sq_0025(d);
	// Limits
	double s0PostUpLimit = sqrtq(chiUp / d);
	double s0PostLoLimit = sqrtq(chiLow / d);
	double sigma0Aposteriori = sqrtq(sigma0 / d);
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
		printf(" %4s %10s %10s %10s %6s  \n", "It", "sigma", "|grad|", "|dx|", "stepsize");
		printf(" %4i %4.4e %4.4e %4.4e %4.4f  \n", itIdx-1, sigma0, grad.norm(), direction.norm(), stepsize);
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


Eigen::VectorXd TLSGaussNewtonSolver::getGNDirection(Eigen::VectorXd parameter)
{
	fEvaluator->setParameters(parameter);
	// compute dx, as in TLSUniversalMtdComputer class
	const TSparseMatrix &A = fEvaluator->getA();

	const TVector &W = fEvaluator->getMisclosure();

	int nbUnk = fEvaluator->dimensions.UIndex;
	int nbEq = fEvaluator->dimensions.EIndex;
	// assume nbConstr=0;
	int nbCnstr = fEvaluator->dimensions.CIndex;
	if (nbCnstr != 0)
	{
		throw std::runtime_error("Armijo Gauss Newton only implemented without constraints.");
	}

	// set invN1
	TSparseMatrix invN1;
	const TSparseMatrix &Pv = fEvaluator->getPv();
	invN1 = Pv;

	// Calculate Normal matrix N2 = At * inv( B * inv(P) * Bt ) * A , matrix dimensions (u,u)
	TSparseMatrix N2;
	N2 = A.transpose() * invN1 * A;

	// construct NBig = (N2, A2t
	//                   A2, 0  )
	TSparseMatrix NBig;
	// no constraints
	NBig = N2;
	TVector VBig;
	VBig = A.transpose() * invN1 * W;

	// Calculates solution NBig * X = -VBig and keeps only the part corresponding to adjusted parameters

	// solve the system
	TVector solution;
	// solve directly without scaling
	double scaleFactor = fConfig.LMpenalty;
	if (fConfig.useLM)
	{
		// std::cout << std::endl << NBig << std::endl;
		NBig += scaleFactor * getDiagonalLMScaleFactor(NBig);
		// std::cout << std::endl << NBig << std::endl;
	}
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> decomp(NBig);
	solution = decomp.solve(-VBig);

	//	// test solution quality, how exact is NBig*sol = -VBig solved?
	//	double solutionQuality = (NBig * solution + VBig).norm();
	//	if (solutionQuality > 1e+3)
	//	{
	//		std::cout << "linear subsystem not solved accurately |NBig * solution + VBig| = " << solutionQuality << std::endl;
	//	}

	return solution;
}

Eigen::VectorXd TLSGaussNewtonSolver::getGradient(Eigen::VectorXd parameter)
{
	fEvaluator->setParameters(parameter);
	Eigen::VectorXd grad(fEvaluator->dimensions.UIndex);
	Eigen::VectorXd misclosure = fEvaluator->getMisclosure();
	const TSparseMatrix &Pv = fEvaluator->getPv();
	const TSparseMatrix &A = fEvaluator->getA();
	Eigen::VectorXd residual = fEvaluator->getResidual();

	grad = A.transpose() * Pv * residual;

	return grad;
}

double TLSGaussNewtonSolver::backtrackingArmijoStepsize(double  sigma0, Eigen::VectorXd x0, Eigen::VectorXd direction)
{
	// |f(x0)|^2 = sigma0, search direction 'direction'
	// armijo parameters
	// tau: reduction in case Armijo–Goldstein condition is not satisfied
	double tau = 0.5;
	// c: Armijo–Goldstein condition: real descent vs descent predicted by gradient
	double c = 0.5;
	//fEvaluator
	double expectedDescent = getGradient(x0).dot(direction);
	double alpha = 1;
	Eigen::VectorXd trialParameter = x0 + alpha * direction;
	fEvaluator->setParameters(trialParameter);
	Eigen::VectorXd trialResidual = fEvaluator->getResidual();
	double trialSigma = trialResidual.transpose() * fEvaluator->getPv() * trialResidual;
	double realDescent = trialSigma - sigma0;
	// testing armijo goldstein descent condition (real descent has to be at least stepsize * c * full step expected descent )
	while (c * alpha* expectedDescent < realDescent && alpha>0.01)
	{
		// reduce stepsize
		alpha *= tau;
		trialParameter = x0 + alpha * direction;
		fEvaluator->setParameters(trialParameter);
		trialResidual = fEvaluator->getResidual();
		trialSigma = trialResidual.transpose() * fEvaluator->getPv() * trialResidual;
		realDescent = trialSigma - sigma0;
	}
	return alpha;
}

Eigen::SparseMatrix<double> TLSGaussNewtonSolver::getDiagonalLMScaleFactor(Eigen::SparseMatrix<double>& M)
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

void GNresult::serialize(SerializerObject::SerializationHelper &obj) const
{
	obj.addProperty("solution", solution);
	obj.addProperty("residual", residual);
	obj.addProperty("objective", objective);
	obj.addProperty("Iterations",nIterations);
	obj.addProperty("isInLimits", isInLimits);
	obj.addProperty("sigma0Aposteriori", sigma0Aposteriori);
}

//void solverConfig::serialize(SerializerObject::SerializationHelper &obj) const
//{ 
//	obj.addProperty("plotLevel", plotLevel);
//	obj.addProperty("useArmijo", useArmijo);
//	obj.addProperty("useLM", useLM);
//	obj.addProperty("LMpenalty", LMpenalty);
//	obj.addProperty("maxIter", maxIter);
//	obj.addProperty("terminationTol", terminationTol);   	
//}

