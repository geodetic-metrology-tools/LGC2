#include <TLSRobustSolver.h>
#include <Logger.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include "UEOIndices.h"

// osqp-eigen
#include "OsqpEigen/OsqpEigen.h"



TLSRobustSolver::TLSRobustSolver(std::shared_ptr<TLSEvaluator> evaluator) : fEvaluator( evaluator )
{
}

void TLSRobustSolver::solve()
{
	Eigen::VectorXd iniVal = fEvaluator->getEstParams();
	int nPar = iniVal.rows();

	// rudimentary full step GN huber iterations

	Eigen::VectorXd currentIter = iniVal;
	Eigen::VectorXd step = iniVal;
	double stepnorm = 1;
	int nIter = 0;
	int maxIter=100;
	while (stepnorm>1e-7 && nIter<maxIter)
	{
		Eigen::VectorXd huberStep = computeStep(currentIter);
		step = huberStep.topRows(nPar);
		stepnorm = step.norm();
		currentIter += step;		
		std::cout << "huber stepsize = " << step.norm() << std::endl;
		nIter++;
	}




	//Eigen::VectorXd result = computeStep(iniVal);
	std::cout << "Huber solution for gamma =" << gamma << std::endl << currentIter << std::endl;
}

Eigen::VectorXd TLSRobustSolver::computeStep(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	// set up the linearized quadratic program and solve it with osqp
	// lambda = Huber threshold where L2 becomes L1


	// assemble QP data:
	// variables = y=(dx,V,W,U)
	// min 0.5*y^THy + g^T y  st.
	// 0<=F(x_k)+Adx+BV<=0
	// 0<=Sv V-W+U<=+inf			
	// -inf<=V-W-U<=0
	// 0<=C(x_k)+C_kdx<=0
	// 
	// with H=diag(0,0,I,0), Sv=sqrt(Pv)
	// g=lambda*(0,0,1^T,0)

	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;
	int nCnstr = indices.CIndex;

	int nVariables = nPar + 3 * nObs;
	int nConstraints = nEqn + 2 * nObs + nCnstr;

	Eigen::SparseMatrix<double> constraintMatrix(nConstraints, nVariables);
	Eigen::VectorXd lb(nConstraints), ub(nConstraints);
	lb.setZero(), ub.setZero();
	setHuberConstraints(par, constraintMatrix, lb, ub);
	Eigen::SparseMatrix<double> hessian(nVariables, nVariables);
	Eigen::VectorXd gradient(nVariables);
	gradient.setZero();
	setHuberObjective(par, hessian, gradient);


    // instantiate the OSQP solver
    OsqpEigen::Solver solver;
	// no output printing
//	solver.settings()->setVerbosity(false);
	solver.data()->setNumberOfVariables(nVariables);
	solver.data()->setNumberOfConstraints(nConstraints);
	if (!solver.data()->setHessianMatrix(hessian))
	{
		throw std::runtime_error("failed to set osqp problem data.");
	}
	if (!solver.data()->setGradient(gradient))
	{
		throw std::runtime_error("failed to set osqp problem data.");
	}
	if (!solver.data()->setLinearConstraintsMatrix(constraintMatrix))
	{
		throw std::runtime_error("failed to set osqp problem data.");
	}
	if (!solver.data()->setLowerBound(lb))
	{
		throw std::runtime_error("failed to set osqp problem data.");
	}
	if (!solver.data()->setUpperBound(ub))
	{
		throw std::runtime_error("failed to set osqp problem data.");
	}


	// initialize and solve
	if (!solver.initSolver())
	{
		throw std::runtime_error("initialization of solver failed");
	}
	if (!solver.solve())
	{
		throw std::runtime_error("OSQP solve failed.");
	}

	// get solution

	Eigen::VectorXd result = solver.getSolution();

	return result;
}

void TLSRobustSolver::setHuberConstraints(Eigen::VectorXd par, Eigen::SparseMatrix<double> &constraintMat, Eigen::VectorXd &lb, Eigen::VectorXd &ub)
{
	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;
	int nCnstr = indices.CIndex;

	fEvaluator->setParameters(par);
	// matrices: first design, weight, etc
	Eigen::SparseMatrix<double> A = fEvaluator->getA();
	Eigen::SparseMatrix<double> A2 = fEvaluator->getA2();
	// implement getB!!
	Eigen::SparseMatrix<double> B = fEvaluator->getB();
	Eigen::SparseMatrix<double> P = fEvaluator->getPv();
	// square root assuming P is diagonal
	Eigen::SparseMatrix<double> Sv = P.cwiseSqrt();

	// Misclosure Vectors .. for the bounds
	Eigen::VectorXd W = fEvaluator->getMisclosure();
	Eigen::VectorXd W2 = fEvaluator->getConstraintMisclosure();


	int nnzA(A.nonZeros()), nnzB(B.nonZeros()), nnzA2(A2.nonZeros()), nnzP(P.nonZeros());
	int nnzHuber = nnzA + nnzA2 + nnzB + 2 * nnzP + 4 * nObs;

	// Constraints in matrix form
	// -F(x_k) <= [A  , B, 0 ,0] [dx] <= -F(x_k)
	// 0       <= [0  ,Sv,-I, I] [V ] <= +inf
	// -inf    <= [0  ,Sv,-I,-I] [W ] <= 0
	// -C(x_k) <= [C_k, 0, 0, 0] [U ] <= -C(x_k)
	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzHuber);
	// row 1
	// A
	for (int k = 0; k < A.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(A, k); it; ++it)
			triplets.push_back(TTriplet(it.row(), it.col(), it.value()));
	}
	// B
	for (int k = 0; k < B.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(B, k); it; ++it)
			triplets.push_back(TTriplet(it.row(), it.col() + nPar, it.value()));
	}
	// row 2
	// Sv
	for (int k = 0; k < Sv.outerSize(); k++)
	{
		for (TSparseMatrix::InnerIterator it(Sv, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn, it.col() + nPar, it.value()));
	}
	// the two identity matrices
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(nEqn + k, nPar+nObs + k, -1.0));
		triplets.push_back(TTriplet(nEqn + k, nPar+2 * nObs + k, +1.0));
	}
	// row 3
	// Sv
	for (int k = 0; k < Sv.outerSize(); k++)
	{
		for (TSparseMatrix::InnerIterator it(Sv, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn + nObs, it.col() + nPar, it.value()));
	}
	// the two identity matrices
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(nEqn + nObs + k,nPar+ nObs + k, -1.0));
		triplets.push_back(TTriplet(nEqn + nObs + k, nPar+2 * nObs + k, -1.0));
	}
	// row 4
	// C_k
	for (int k = 0; k < A2.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(A2, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn + 2 * nObs, it.col(), it.value()));
	}

	constraintMat.setFromTriplets(triplets.begin(), triplets.end());

	double BIG = 1e+6;
	// the bounds
	// row 1
	lb.topRows(nEqn) = -W;
	ub.topRows(nEqn) = -W;
	// row 2
	lb.middleRows(nEqn, nObs).setZero();
	ub.middleRows(nEqn, nObs).setConstant(OsqpEigen::INFTY);
	// row 3
	lb.middleRows(nEqn + nObs, nObs).setConstant(-OsqpEigen::INFTY);
	ub.middleRows(nEqn + nObs, nObs).setZero();
	// row 4
	lb.middleRows(nEqn + 2 * nObs, nCnstr) = -W2;
	ub.middleRows(nEqn + 2 * nObs, nCnstr) = -W2;

}

void TLSRobustSolver::setHuberObjective(Eigen::VectorXd par, Eigen::SparseMatrix<double> &hessian, Eigen::VectorXd &gradient)
{
	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;
	int nCnstr = indices.CIndex;

	fEvaluator->setParameters(par);
	// matrices: first design, weight, etc
	Eigen::SparseMatrix<double> P = fEvaluator->getPv();
	// square root assuming P is diagonal
	Eigen::SparseMatrix<double> Sv = P.cwiseSqrt();
	
	int nnzP(P.nonZeros());
	int nnzHessian = nObs;
	// H=diag(0,0,I,0)
	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzHessian);

	// identity block	
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(nPar + nObs + k, nPar + nObs + k, +1.0));
	}
	hessian.setFromTriplets(triplets.begin(), triplets.end());
	hessian.makeCompressed();
	Eigen::MatrixXd test = hessian.toDense();

	// gradient
	// the gradient is set to the value where the L2 switches to L1 in the Huber objective
	// so it has roughly the magnitude of the sigmas
	//Eigen::VectorXd diag = Sv.diagonal();
	//Eigen::VectorXd sigmas = diag.cwiseInverse();
	Eigen::VectorXd ones(nObs);
	ones.setOnes();
		//	std::cout << sigmas << std::endl;
	Eigen::VectorXd result(nPar + 3 * nObs);
	result.setZero();
	//double gamma = 1;
	result.middleRows(nPar + 2*nObs, nObs) = gamma*ones;


	gradient = result;

}



