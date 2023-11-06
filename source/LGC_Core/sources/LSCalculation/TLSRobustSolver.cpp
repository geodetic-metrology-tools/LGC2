#include <TLSRobustSolver.h>
#include <Logger.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unsupported/Eigen/SparseExtra>
#include "UEOIndices.h"

//// osqp-eigen
//#include "OsqpEigen/OsqpEigen.h"



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
		Eigen::VectorXd huberStep = computeHuberStep(currentIter);
		step = huberStep.topRows(nPar);
		stepnorm = step.norm();
		currentIter += step;		
		std::cout << "huber stepsize = " << step.norm() << std::endl;
		nIter++;
	}




	//Eigen::VectorXd result = computeStep(iniVal);
	std::cout << "Huber solution for gamma =" << gamma << std::endl << currentIter << std::endl;
}

Eigen::VectorXd TLSRobustSolver::computeHuberStep(Eigen::VectorXd par)
{
	fEvaluator->setParameters(par);
	// set up the linearized quadratic program and solve it with osqp
	// lambda = Huber threshold where L2 becomes L1


	// assemble QP data:
	// variables = y=(dx,V,W,R,S)
	// min 0.5*y^THy + g^T y  st.
	// 0<=F(x_k)+Adx+BV<=0
	// 0<=Sv V-W-R+S<=0			
	// 0<=R,S
	// 
	// with H=diag(0,0,I,0,0), Sv=sqrt(Pv)
	// g=lambda*(0,0,0,I,I)

	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;
	int nCnstr = indices.CIndex;

	int nVariables = nPar + 4 * nObs;
	int nConstraints = nEqn + nCnstr + 3 * nObs;

	Eigen::SparseMatrix<double> constraintMatrix;
	Eigen::VectorXd lb, ub;
	lb.setZero(), ub.setZero();
	//setOSQPFormatL1Constraint(par, constraintMatrix, lb, ub);
	//setOSQPFormatHuberConstraint(par, constraintMatrix, lb, ub);
	std::cout << constraintMatrix.rows() << " "<<constraintMatrix.cols()  << std::endl;
	Eigen::SparseMatrix<double> hessian;
	Eigen::VectorXd gradient;
	gradient.setZero();
	setHuberObjective(par, hessian, gradient);
	//setL1Objective(hessian, gradient);


//    // instantiate the OSQP solver
//    OsqpEigen::Solver solver;
//	// no output printing
////	solver.settings()->setVerbosity(false);
//	solver.settings()->setScaling(false);
//	solver.settings()->setScaledTerimination(true);
//	solver.settings()->setPolish(true);
//	solver.data()->setNumberOfVariables(nVariables);
//	solver.data()->setNumberOfConstraints(nConstraints);
//	if (!solver.data()->setHessianMatrix(hessian))
//	{
//		throw std::runtime_error("failed to set osqp problem data.");
//	}
//	if (!solver.data()->setGradient(gradient))
//	{
//		throw std::runtime_error("failed to set osqp problem data.");
//	}
//	if (!solver.data()->setLinearConstraintsMatrix(constraintMatrix))
//	{
//		throw std::runtime_error("failed to set osqp problem data.");
//	}
//	if (!solver.data()->setLowerBound(lb))
//	{
//		throw std::runtime_error("failed to set osqp problem data.");
//	}
//	if (!solver.data()->setUpperBound(ub))
//	{
//		throw std::runtime_error("failed to set osqp problem data.");
//	}
//
//
//	// initialize and solve
//	if (!solver.initSolver())
//	{
//		throw std::runtime_error("initialization of solver failed");
//	}
//	if (!solver.solve())
//	{
//		throw std::runtime_error("OSQP solve failed.");
//	}
//
//	// get solution
//
//	Eigen::VectorXd result = solver.getSolution();
	Eigen::VectorXd result;
	return result;
}

void TLSRobustSolver::setHuberObjective(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &hessian, Eigen::VectorXd &gradient)
{
	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;

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
	hessian.resize(nPar + 4 * nObs, nPar + 4 * nObs);

	// identity block	
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(nPar + nObs + k, nPar + nObs + k, +1.0));
	}
	hessian.setFromTriplets(triplets.begin(), triplets.end());
	hessian.makeCompressed();
	Eigen::MatrixXd test = hessian.toDense();
	//std::cout << "huber Hessians matrix" << std::endl << test << std::endl;

	// gradient
	// the gradient is set to the value where the L2 switches to L1 in the Huber objective
	// so it has roughly the magnitude of the sigmas
	//Eigen::VectorXd diag = Sv.diagonal();
	//Eigen::VectorXd sigmas = diag.cwiseInverse();
	Eigen::VectorXd ones(2 * nObs);
	ones.setOnes();
		//	std::cout << sigmas << std::endl;
	Eigen::VectorXd result(nPar + 4 * nObs);
	result.setZero();
	//double gamma = 1;
	result.middleRows(nPar + 2 * nObs, 2 * nObs) = gamma * ones;

	gradient.resize(nPar + 4 * nObs);

	gradient = result;
	if (dumpMat)
	{
		Eigen::saveMarket(hessian, "hessian.mtx");
		Eigen::saveMarketDense(gradient, "gradient.mtx");
	}

}

void TLSRobustSolver::setHuberEqualityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &b)
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
	int nnzEqBlock= nnzA + nnzA2 + nnzB + 1 * nnzP + 3 * nObs;

	// Equality Constraints in matrix form
	// [A  , B, 0 ,0, 0] [dx] = -F(x_k)
	// [C_k, 0, 0, 0, 0] [V ] = -C(x_k)
	// [0  ,Sv,-I,-I, I] [W ] = 0
	// setting the size
	block.resize(nEqn + nCnstr + nObs, nPar + 4 * nObs);
	b.resize(nEqn + nCnstr + nObs);

	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzEqBlock);
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
	// C_k
	for (int k = 0; k < A2.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(A2, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn, it.col(), it.value()));
	}
	// row 3
	// Sv
	for (int k = 0; k < Sv.outerSize(); k++)
	{
		for (TSparseMatrix::InnerIterator it(Sv, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn+nCnstr, it.col() + nPar, it.value()));
	}
	// the three identity matrices
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(nEqn + nCnstr + k, nPar + nObs + k, -1.0));
		triplets.push_back(TTriplet(nEqn + nCnstr + k, nPar + 2 * nObs + k, -1.0));
		triplets.push_back(TTriplet(nEqn + nCnstr + k, nPar + 3 * nObs + k, +1.0));
	}
	block.setFromTriplets(triplets.begin(), triplets.end());
	block.makeCompressed();

	double BIG = 1e+6;
	// the bounds
	// row 1
	b.topRows(nEqn) = -W;
	// row 2
	b.middleRows(nEqn, nCnstr) = -W2;
	// row 3
	b.middleRows(nEqn + nCnstr, nObs).setZero();

}

void TLSRobustSolver::setHuberInequalityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &lb, Eigen::VectorXd &ub)
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


	int nnzIneqBlock = 2 * nObs;

	// Inequality Constraints in matrix form
	// 0       <= [0  , 0, 0, I, 0] [R ] <= +inf
	// 0       <= [0  , 0, 0, 0, I] [S ] <= +inf
	block.resize(2 * nObs, nPar + 4 * nObs);
	lb.resize(2 * nObs);
	ub.resize(2 * nObs);
	
	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzIneqBlock);
	// row 1+1
	// each one identity matrix
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(k, nPar + 2 * nObs + k, +1.0));
		triplets.push_back(TTriplet(nObs + k, nPar + 3 * nObs + k, +1.0));
	}
	block.setFromTriplets(triplets.begin(), triplets.end());
	block.makeCompressed();

	// the lower and upper bound
	// row 1+2
	lb.setZero();
	//ub.setConstant(OsqpEigen::INFTY);
}

//void TLSRobustSolver::setOSQPFormatHuberConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &constraintMat, Eigen::VectorXd &lb, Eigen::VectorXd &ub)
//{
//	Eigen::SparseMatrix<double> eqBlock, ineqBlock;
//	Eigen::VectorXd bEq, lbIneq, ubIneq;
//	setHuberEqualityConstraint(par, eqBlock, bEq);
//	setHuberInequalityConstraint(par, ineqBlock, lbIneq, ubIneq);
//	if (dumpMat)
//	{
//		Eigen::saveMarket(eqBlock, "eqBlock.mtx");
//		Eigen::saveMarketDense(bEq, "bEq.mtx");
//		Eigen::saveMarket(ineqBlock, "ineqBlock.mtx");
//		Eigen::saveMarketDense(lbIneq, "lbIneq.mtx");
//		Eigen::saveMarketDense(ubIneq, "ubIneq.mtx");
//	}
//	int eqRows = eqBlock.rows();
//	int ineqRows = ineqBlock.rows();
//	constraintMat.resize(eqRows + ineqRows, eqBlock.cols());
//	// stack both blocks to form the combined constraints
//	std::vector<Eigen::Triplet<double>> triplets;
//	triplets.reserve(eqBlock.nonZeros()+ineqBlock.nonZeros());
//	// Set the top equality block.
//	for (int i = 0; i < eqBlock.outerSize(); ++i)
//	{
//		for (Eigen::SparseMatrix<double>::InnerIterator it(eqBlock, i); it; ++it)
//		{
//			constraintMat.insert(it.row(), it.col()) = it.value();
//		}
//	}
//
//	// Set the bottom inequality block.
//	for (int i = 0; i < ineqBlock.outerSize(); ++i)
//	{
//		for (Eigen::SparseMatrix<double>::InnerIterator it(ineqBlock, i); it; ++it)
//		{
//			constraintMat.insert(it.row() + eqRows, it.col()) = it.value();
//		}
//	}
//
//	lb.resize(eqRows + ineqRows);
//	ub.resize(eqRows + ineqRows);
//	// set the bounds
//	// equality
//	lb.topRows(eqRows) = bEq;
//	ub.topRows(eqRows) = bEq;
//	//inequality
//	lb.bottomRows(ineqRows) = lbIneq;
//	ub.bottomRows(ineqRows) = ubIneq;
//
//
//}

void TLSRobustSolver::setL1Objective(Eigen::SparseMatrix<double> &hessian, Eigen::VectorXd &gradient)
{
	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;

	// Hessian is zero matrix, L1 is formulated as LP
	int nnzHessian = 0;
	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzHessian);
	hessian.resize(nPar + 2 * nObs, nPar + 2 * nObs);
	hessian.setFromTriplets(triplets.begin(), triplets.end());
	hessian.makeCompressed();

	// gradient
	Eigen::VectorXd ones(nObs);
	ones.setOnes();
	Eigen::VectorXd result(nPar + 2 * nObs);
	result.setZero();
	//double gamma = 1;
	result.middleRows(nPar, nObs) = ones;

	gradient.resize(nPar + 2 * nObs);

	gradient = result;
	if (dumpMat)
	{
		Eigen::saveMarket(hessian, "hessian.mtx");
		Eigen::saveMarketDense(gradient, "gradient.mtx");
	}

}

void TLSRobustSolver::setL1EqualityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &b)
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

	// Misclosure Vectors .. for the bounds
	Eigen::VectorXd W = fEvaluator->getMisclosure();
	Eigen::VectorXd W2 = fEvaluator->getConstraintMisclosure();


	int nnzA(A.nonZeros()), nnzB(B.nonZeros()), nnzA2(A2.nonZeros());
	int nnzEqBlock = nnzA + nnzA2 + nnzB;

	// Equality Constraints in matrix form
	// [A  , B, 0] [dx] = -F(x_k)
	// [C_k, 0, 0] [V ] = -C(x_k)
	// setting the size
	block.resize(nEqn + nCnstr, nPar + 2 * nObs);
	b.resize(nEqn + nCnstr);

	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzEqBlock);
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
	// C_k
	for (int k = 0; k < A2.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(A2, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn, it.col(), it.value()));
	}
	block.setFromTriplets(triplets.begin(), triplets.end());
	block.makeCompressed();

	double BIG = 1e+6;
	// the bounds
	// row 1
	b.topRows(nEqn) = -W;
	// row 2
	b.middleRows(nEqn, nCnstr) = -W2;

}

void TLSRobustSolver::setL1InequalityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &lb, Eigen::VectorXd &ub)
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


	int nnzIneqBlock = 2 * nObs;

	// Inequality Constraints in matrix form
	// -inf    <= [0  , Sv, -I]  <= 0
	// 0       <= [0  , Sv,  I]  <= +inf
	block.resize(2 * nObs, nPar + 2 * nObs);
	
	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzIneqBlock);
	// row 1+2
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(k, nPar + nObs + k, -1.0));
		triplets.push_back(TTriplet(k + nObs, nPar + nObs + k, -1.0));
	}
	// Sv
	for (int k = 0; k < Sv.outerSize(); k++)
	{
		for (TSparseMatrix::InnerIterator it(Sv, k); it; ++it)
		{
			triplets.push_back(TTriplet(it.row(), it.col() + nPar, it.value()));
			triplets.push_back(TTriplet(it.row() + nObs, it.col() + nPar, it.value()));
		}
	}

	block.setFromTriplets(triplets.begin(), triplets.end());
	block.makeCompressed();

	// the lower and upper bound
	// row 1+2
	lb.resize(2 * nObs);
	ub.resize(2 * nObs);

	lb.setZero();
	ub.setZero();
	//lb.topRows(nObs).setConstant(-OsqpEigen::INFTY);
	//lb.bottomRows(nObs).setConstant(0);
	//ub.topRows(nObs).setConstant(0);
	//ub.bottomRows(nObs).setConstant(OsqpEigen::INFTY);
}


//void TLSRobustSolver::setOSQPFormatL1Constraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &constraintMat, Eigen::VectorXd &lb, Eigen::VectorXd &ub)
//{
//	Eigen::SparseMatrix<double> eqBlock, ineqBlock;
//	Eigen::VectorXd bEq, lbIneq, ubIneq;
//	setL1EqualityConstraint(par, eqBlock, bEq);
//	setL1InequalityConstraint(par, ineqBlock, lbIneq, ubIneq);
//	if (dumpMat)
//	{
//		Eigen::saveMarket(eqBlock, "eqBlock.mtx");
//		Eigen::saveMarketDense(bEq, "bEq.mtx");
//		Eigen::saveMarket(ineqBlock, "ineqBlock.mtx");
//		Eigen::saveMarketDense(lbIneq, "lbIneq.mtx");
//		Eigen::saveMarketDense(ubIneq, "ubIneq.mtx");
//	}
//	int eqRows = eqBlock.rows();
//	int ineqRows = ineqBlock.rows();
//	constraintMat.resize(eqRows + ineqRows, eqBlock.cols());
//	// stack both blocks to form the combined constraints
//	std::vector<Eigen::Triplet<double>> triplets;
//	triplets.reserve(eqBlock.nonZeros()+ineqBlock.nonZeros());
//	// Set the top equality block.
//	for (int i = 0; i < eqBlock.outerSize(); ++i)
//	{
//		for (Eigen::SparseMatrix<double>::InnerIterator it(eqBlock, i); it; ++it)
//		{
//			constraintMat.insert(it.row(), it.col()) = it.value();
//		}
//	}
//
//	// Set the bottom inequality block.
//	for (int i = 0; i < ineqBlock.outerSize(); ++i)
//	{
//		for (Eigen::SparseMatrix<double>::InnerIterator it(ineqBlock, i); it; ++it)
//		{
//			constraintMat.insert(it.row() + eqRows, it.col()) = it.value();
//		}
//	}
//
//	lb.resize(eqRows + ineqRows);
//	ub.resize(eqRows + ineqRows);
//	// set the bounds
//	// equality
//	lb.topRows(eqRows) = bEq;
//	ub.topRows(eqRows) = bEq;
//	//inequality
//	lb.bottomRows(ineqRows) = lbIneq;
//	ub.bottomRows(ineqRows) = ubIneq;
//
//
//}
