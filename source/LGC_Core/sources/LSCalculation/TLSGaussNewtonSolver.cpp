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

	// make initial rudimentary consistency check
	Eigen::SparseMatrix<double> A = fEvaluator->getA(true);
    Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qrSolver;
    qrSolver.compute(A);

    if (qrSolver.info() == Eigen::Success) {
		int rank = qrSolver.rank();
		if (rank != A.cols())
		{
			std::stringstream errMessage;
			errMessage << "A matrix has rank " << rank << " but there are " << A.cols() << " columns, so A has not full column rank." << std::endl;
			throw std::runtime_error(errMessage.str());
		}
	} else {
		throw std::runtime_error("Decomposition failed.");
    }

	
	int itIdx = 0;
	while (direction.cwiseAbs().maxCoeff() > 1e-6 && itIdx < 500)
	{
		// compute the search direction
		direction = getGNDirection(parameterIterate);
		// compute the gradient along this direction. Needed for the armijo linesearch
		grad = getGradient(parameterIterate);
		// compute the residual and the weighted objective to compare the real descent vs the gradient predicted descent in the armijo linesearch method
		Eigen::VectorXd residual = fEvaluator->getResidual();
		double sigma0 = residual.transpose() * fEvaluator->getPv() * residual;
		double stepsize = backtrackingArmijoStepsize(sigma0, parameterIterate, direction);

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
	invN1 = Pv ;

	// Calculate Normal matrix N2 = At * inv( B * inv(P) * Bt ) * A , matrix dimensions (u,u)
	TSparseMatrix N2;
	N2 = A.transpose() * invN1 * A;

	// construct NBig = (N2, A2t
	//                   A2, 0  )
	TSparseMatrix NBig;
	// no constraints
	NBig = N2;
	//std::vector<TTriplet> coeffs;
	//coeffs.reserve(N2.nonZeros() + 2 * A2.nonZeros());

	//// Fill in the N2 part
	//for (int k = 0; k < N2.outerSize(); ++k)
	//{
	//	for (TSparseMatrix::InnerIterator it(N2, k); it; ++it)
	//		coeffs.push_back(TTriplet(it.row(), it.col(), it.value()));
	//}

	//NBig.setFromTriplets(coeffs.begin(), coeffs.end());

	// Extended vector: appends W2 (constraints misclosures) to the calculated At*inv(N1)*W  vector
	TVector VBig;
	//(nbUnk + nbCnstr);
	VBig = A.transpose() * invN1 * W;

	// Calculates solution NBig * X = -VBig and keeps only the part corresponding to adjusted parameters
	TVector solutionExt;
	//(nbUnk + nbCnstr);

	// use Cholesky decomposition if nbCnstr=0, otherwise SparseLU as positive definiteness of NBig may be violated
	if (!TSparseUtils::solveUnique(NBig, -VBig, solutionExt, (nbCnstr == 0), (nbCnstr == 0), true))
	{
		logCritical() << "No solution could be found when solving equation system: Nbig * dX = -VBig (extended matrices with conditions)";
	}

	//std::cout << "NBig" << std::endl << NBig.toDense() << std::endl;
	TVector solution;
	// we do not need the Lagrange multipliers
	solution = solutionExt;
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
	while (c * alpha* expectedDescent < realDescent && alpha>0.1)
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

