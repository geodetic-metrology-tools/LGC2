#include "TLSGaussNewtonSolver.h"
#include <TLSGaussNewtonSolver.h>
#include <Logger.hpp>
#include <iostream>

TLSGaussNewtonSolver::TLSGaussNewtonSolver(std::shared_ptr<TLGCData> data) : fEvaluator( TLSEvaluator(data) )
{
}

Eigen::VectorXd TLSGaussNewtonSolver::solve()
{
	// simple full step Gauss Newton
	Eigen::VectorXd parameterIterate = fEvaluator.getEstParams();
	Eigen::VectorXd grad = getGradient(parameterIterate);
	Eigen::VectorXd direction(fEvaluator.dimensions.UIndex);
	direction.setConstant(1);
	
	int itIdx = 0;
	//while (direction.norm()>1e-6)
	//while ((direction.cwiseAbs().maxCoeff() > 1e-6)|| grad.norm()>1e-2)
	while (direction.cwiseAbs().maxCoeff() > 1e-6 && itIdx < 500)
	{
		direction = getGNDirection(parameterIterate);
		grad = getGradient(parameterIterate);

		Eigen::VectorXd residual = fEvaluator.getResidual();
		double sigma0 = residual.transpose() * *fEvaluator.getPv() * residual;
		double stepsize = backtrackingArmijoStepsize(sigma0, parameterIterate, direction);
		//double stepsize = 1;

		// do full step
		//parameterIterate += direction;
		// or multiply by stepsize
		parameterIterate += stepsize * direction;

	//	std::cout << "current sigma " << sigma0 << std::endl;
	//	//std::cout << "GN solve method: full step GN norm = " << direction.norm() << std::endl;
	//	std::cout << "current gradient norm = " << grad.norm() << std::endl;
	//	std::cout << "Armijo backtracking stepsize = " << stepsize << std::endl;
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
	fEvaluator.setParameters(parameter);
	// compute dx, as in TLSUNiversalMtdComputer class
	const TSparseMatrix &A = *fEvaluator.getA();
	const TVector &W = fEvaluator.getMisclosure();
	//im->getMisclosureVctr(); // W : Misclosures vector ("fermetures")
	const TSparseMatrix &A2 = *fEvaluator.getA2();
	//*im->getCnstrFirstDgnMtrx(); // A2 : First design matrix part related to constraints only
	const TVector &W2 = fEvaluator.getConstraintMisclosure();
	//im->getCnstrMisclosureVctr(); // W2 : Misclosures vector part related to constraints only

	int nbUnk = fEvaluator.dimensions.UIndex;
	int nbEq = fEvaluator.dimensions.EIndex;
	int nbCnstr = fEvaluator.dimensions.CIndex;


	// set invN1
	TSparseMatrix invN1(nbEq, nbEq);
	// for now assume B is always block diagonal
	//if (im->getSecondDgnBlockDiagStatus())
	//{
		//if (!(im->getWeightMtrx()) || !(im->getSecondDgnBlockDiagInvMtrx()))
		//{
		//	throw std::runtime_error("Some of the design matrices are not initialized!");
		//}
		const TSparseMatrix &Pv = *fEvaluator.getPv();
		const TSparseMatrix &invB = *fEvaluator.getBinv();
		invN1 = invB.transpose() * Pv * invB;
	//}
//	else
//	{ // if B is not block diagonal, invN1 needs to be computed via an explicit inversion
//	  // B*invPv*BT is symmetric and positive definite so LDLT can be used
//		if (!(im->getWeightInvMtrx()) || !(im->getSecondDgnMtrx()))
//		{
//			throw std::runtime_error("Some of the design matrices are not initialized!");
//		}
//		const TSparseMatrix &B = *im->getSecondDgnMtrx();
//		const TSparseMatrix &InvPv = *im->getWeightInvMtrx();
//		if (!TSparseUtils::inverse(B * InvPv * B.transpose(), invN1, true))
//		{
//			logCritical() << "Matrix B*inv(Pv)*transpose(B) could not be inverted!";
//			return false;
//		}
//		// copy invN1 to resultmatrices only if B is not block diagonal, otherwise it is not needed in the calcRes method
//		rm->setInvN1Matrix(invN1);
//	}

	// Calculate Normal matrix N2 = At * inv( B * inv(P) * Bt ) * A , matrix dimensions (u,u)
	// and re-sets this new matrix to the main TLSResultsMatrices object.
	TSparseMatrix N2(nbUnk, nbUnk);
	N2 = A.transpose() * invN1 * A;

	// construct NBig = (N2, A2t
	//                   A2, 0  )
	TSparseMatrix NBig(nbUnk + nbCnstr, nbUnk + nbCnstr);
	std::vector<TTriplet> coeffs;
	coeffs.reserve(N2.nonZeros() + 2 * A2.nonZeros());

	// Fill in the N2 part
	for (int k = 0; k < N2.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(N2, k); it; ++it)
			coeffs.push_back(TTriplet(it.row(), it.col(), it.value()));
	}

	// Fill the A2 and A2T
	for (int k = 0; k < A2.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(A2, k); it; ++it)
		{
			coeffs.push_back(TTriplet(it.row() + N2.rows(), it.col(), it.value())); // A2
			coeffs.push_back(TTriplet(it.col(), it.row() + N2.cols(), it.value())); // A2T
		}
	}
	NBig.setFromTriplets(coeffs.begin(), coeffs.end());

	// Extended vector: appends W2 (constraints misclosures) to the calculated At*inv(N1)*W  vector
	TVector VBig(nbUnk + nbCnstr);
	VBig << A.transpose() * invN1 * W, W2;

	// Calculates solution NBig * X = -VBig and keeps only the part corresponding to adjusted parameters
	TVector solutionExt(nbUnk + nbCnstr);

	// use Cholesky decomposition if nbCnstr=0, otherwise SparseLU as positive definiteness of NBig may be violated
	if (!TSparseUtils::solveUnique(NBig, -VBig, solutionExt, (nbCnstr == 0), (nbCnstr == 0), true))
	{
		logCritical() << "No solution could be found when solving equation system: Nbig * dX = -VBig (extended matrices with conditions)";
	}

	TVector solution(nbUnk);
	// we do not need the Lagrange multipliers
	solution = solutionExt.head(nbUnk);
	return solution;

	// Copies the matrices into the members of the TResultsMatrices object
//	rm->setNormalMatrix(NBig);
//	rm->setSolutionVect(solution);

}

Eigen::VectorXd TLSGaussNewtonSolver::getGradient(Eigen::VectorXd parameter)
{
	fEvaluator.setParameters(parameter);
	Eigen::VectorXd grad(fEvaluator.dimensions.UIndex);
	Eigen::VectorXd misclosure = fEvaluator.getMisclosure();
	const TSparseMatrix &Pv = *fEvaluator.getPv();
	const TSparseMatrix &A = *fEvaluator.getA();
	const TSparseMatrix &invB = *fEvaluator.getBinv();
	Eigen::VectorXd residual = fEvaluator.getResidual();

	grad = -(invB * A).transpose() * Pv * residual;

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
	fEvaluator.setParameters(trialParameter);
	Eigen::VectorXd trialResidual = fEvaluator.getResidual();
	double trialSigma = trialResidual.transpose() * *fEvaluator.getPv() * trialResidual;
	double realDescent = trialSigma - sigma0;
	// testing armijo goldstein descent condition (real descent has to be at least stepsize * c * full step expected descent )
	while (c * alpha* expectedDescent < realDescent && alpha>0.1)
	{
		// reduce stepsize
		alpha *= tau;
		trialParameter = x0 + alpha * direction;
		fEvaluator.setParameters(trialParameter);
		trialResidual = fEvaluator.getResidual();
		trialSigma = trialResidual.transpose() * *fEvaluator.getPv() * trialResidual;
		realDescent = trialSigma - sigma0;
	}
	return alpha;
}

