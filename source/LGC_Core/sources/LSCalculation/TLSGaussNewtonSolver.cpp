#include <TLSGaussNewtonSolver.h>
#include <Logger.hpp>
#include <iostream>


TLSGaussNewtonSolver::TLSGaussNewtonSolver(std::shared_ptr<TLSEvaluator> evaluator) : fEvaluator( evaluator )
{
}

Eigen::VectorXd TLSGaussNewtonSolver::solve()
{
	// Gauss Newton with armijo stepsize regularization
	Eigen::VectorXd parameterIterate = fEvaluator->getEstParams();
	Eigen::VectorXd grad = getGradient(parameterIterate);
	Eigen::VectorXd direction(fEvaluator->dimensions.UIndex);
	direction.setConstant(1);


	
	int itIdx = 0;
	while (direction.cwiseAbs().maxCoeff() > 1e-6 && itIdx < 50)
	{
		// compute the search direction
		direction = getGNDirection(parameterIterate, false);
		// compute the gradient along this direction. Needed for the armijo linesearch
		grad = getGradient(parameterIterate);
		// compute the residual and the weighted objective to compare the real descent vs the gradient predicted descent in the armijo linesearch method
		Eigen::VectorXd residual = fEvaluator->getResidual();
		double sigma0 = residual.transpose() * fEvaluator->getPv() * residual;
		double stepsize = backtrackingArmijoStepsize(sigma0, parameterIterate, direction);
		//double stepsize = 1;

		// do the regularized step
		parameterIterate += stepsize * direction;

		// stepsize=armijo stepsize
		if (itIdx % 20 == 0)
		{
			printf(" %4s %10s %10s %10s %6s  \n", "It", "sigma", "|grad|", "|dx|", "stepsize");
		}
		printf(" %4i %4.4e %4.4e %4.4e %4.4f  \n", itIdx, sigma0, grad.norm(), direction.norm(), stepsize);

		itIdx++;
	}
	return parameterIterate;
}


Eigen::VectorXd TLSGaussNewtonSolver::getGNDirection(Eigen::VectorXd parameter, bool useScaling)
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
	invN1 = Pv ;

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
	//TVector solutionExt;
	//(nbUnk + nbCnstr);
	

	// solve the system
	TVector solution;
	// 
	// scale the columns of NBig
	if (useScaling == true)
	{
		Eigen::MatrixXd NBigdense = NBig.toDense();
		int nCol = NBig.cols();
		Eigen::VectorXd colScale(nCol);
		for (int j = 0; j < nCol; j++)
		{
			double colNorm = NBigdense.col(j).norm();
			if (colNorm < 1e-16)
			{
				throw std::runtime_error("NBig has a zero-column, so no full rank!");
			}
			colScale(j) = 1 / colNorm;
		}

		// scale NBig
		//  define diagonal matrix
		Eigen::MatrixXd scaleMat(nCol, nCol);
		scaleMat.setZero();
		scaleMat = colScale.asDiagonal();
		// use qr decomposition
		// solve scaled system NBig * Scale * sol = W
		// and then rescale solution trueSol = Scale * sol
		Eigen::SparseMatrix<double> NBig_scaled = (NBigdense * scaleMat).sparseView();
		// Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qr(NBig);
		Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qr(NBig_scaled);
		solution = qr.solve(-VBig);
		// rescale solution
		solution = scaleMat * solution;
	}
	else
	{
		// solve directly without scaling
		Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qr(NBig);
		solution = qr.solve(-VBig);
	}

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

