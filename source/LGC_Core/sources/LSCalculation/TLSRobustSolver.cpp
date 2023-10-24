#include <TLSRobustSolver.h>
#include <Logger.hpp>
#include <iostream>
#include <algorithm>
#include <vector>
#include "UEOIndices.h"


TLSRobustSolver::TLSRobustSolver(std::shared_ptr<TLSEvaluator> evaluator) : fEvaluator( evaluator )
{
}

void TLSRobustSolver::solve()
{
	Eigen::VectorXd iniVal = fEvaluator->getEstParams();




}

Eigen::VectorXd TLSRobustSolver::computeStep(Eigen::VectorXd par)
{
	// set up the linearized quadratic program and solve it with osqp
	// lambda = Huber threshold where L2 becomes L1

	// matrices: first design, weight, etc
	Eigen::SparseMatrix<double> A = fEvaluator->getA();
	Eigen::SparseMatrix<double> A2 = fEvaluator->getA2();
	// implement getB!!
	//Eigen::SparseMatrix<double> B = fEvaluator->getB();
	Eigen::SparseMatrix<double> P = fEvaluator->getPv();

	// vectors: misclosures
	Eigen::VectorXd W = fEvaluator->getMisclosure();
	Eigen::VectorXd W2 = fEvaluator->getConstraintMisclosure();

	// assemble QP data:
	// variables = y=(dx,V,W,U)
	// min 0.5*y^THy + g^T y  st.
	// 0<=F(x_k)+Adx+BV<=0
	// 0<=V-U+W<=+inf			
	// -inf<=V-U-W<=0
	// 0<=C(x_k)+C_kdx<=0
	// 
	// with H=diag(0,Sv,0,I), Sv=sqrt(Pv)
	// g=(0,0,lambda^T,0)

	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;
	int nCnstr = indices.CIndex;


	Eigen::SparseMatrix<double> constraintMatrix(nEqn + 3 * nObs, nPar + 2 * nObs + nCnstr);
	Eigen::VectorXd lb(nEqn+ 3 * nObs), ub(nEqn+ 3 * nObs);
	lb.setZero(), ub.setZero();
	setHuberConstraints(par, constraintMatrix, lb, ub);
	Eigen::SparseMatrix<double> hessian(nPar + 2 * nObs + nCnstr, nPar + 2 * nObs + nCnstr);
	Eigen::VectorXd gradient(nPar + 2 * nObs + nCnstr);
	gradient.setZero();
	setHuberObjective(par, hessian, gradient);
	





	return Eigen::VectorXd();
}

void TLSRobustSolver::setHuberConstraints(Eigen::VectorXd par, Eigen::SparseMatrix<double> &constraintMat, Eigen::VectorXd &lb, Eigen::VectorXd &ub)
{
	UEOIndices indices = fEvaluator->dimensions;

	int nPar = indices.UIndex;
	int nObs = indices.OIndex;
	int nEqn = indices.EIndex;
	int nCnstr = indices.CIndex;

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
	int nnzHuber = nnzA + nnzA2 + 2 * nnzP + 4 * nObs;

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
	for (int k = 0; k < Sv.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(Sv, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn, it.col()+nPar, it.value()));
	}
	// the two identity matrices
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(nEqn+k,nObs+k,-1.0));
		triplets.push_back(TTriplet(nEqn+k,2*nObs+k,+1.0));
	}
	// row 3
	// Sv
	for (int k = 0; k < Sv.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(Sv, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn+nObs, it.col()+nPar, it.value()));
	}
	// the two identity matrices
	for (int k = 0; k < nObs; k++)
	{
		triplets.push_back(TTriplet(nEqn+nObs+k,nObs+k,-1.0));
		triplets.push_back(TTriplet(nEqn+nObs+k,2*nObs+k,-1.0));
	}
	// row 4
	// C_k
	for (int k = 0; k < A2.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(A2, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nEqn+2*nObs, it.col(), it.value()));
	}

	constraintMat.setFromTriplets(triplets.begin(), triplets.end());

	// the bounds
	// row 1
	lb.topRows(nEqn) = -W;
	ub.topRows(nEqn) = -W;
	// row 2
	lb.middleRows(nEqn, nObs).setZero();
	ub.middleRows(nEqn, nObs).setConstant(Eigen::Infinity);
	// row 3
	lb.middleRows(nEqn + nObs, nObs).setConstant(-Eigen::Infinity);
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

	// matrices: first design, weight, etc
	Eigen::SparseMatrix<double> P = fEvaluator->getPv();
	// square root assuming P is diagonal
	Eigen::SparseMatrix<double> Sv = P.cwiseSqrt();
	
	int nnzP(P.nonZeros());
	int nnzHessian = nnzP + nObs;
	// H=diag(0,Sv,0,I)
	std::vector<Eigen::Triplet<double>> triplets;
	triplets.reserve(nnzHessian);

	// Sv block
	for (int k = 0; k < Sv.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(Sv, k); it; ++it)
			triplets.push_back(TTriplet(it.row() + nPar, it.col() + nPar, it.value()));
	}
	// lower right identity block	
	for (int k = 0; k < nObs; ++k)
	{
		triplets.push_back(TTriplet(nPar + 2 * nObs + k, nPar + 2 * nObs + k, +1.0));	
	}
	hessian.setFromTriplets(triplets.begin(), triplets.end());

	// gradient
	// the gradient is set to the value where the L2 switches to L1 in the Huber objective
	// so it has roughly the magnitude of the sigmas
	Eigen::VectorXd diag = Sv.diagonal();
	Eigen::VectorXd sigmas = diag.cwiseInverse();

	// set the border between L1 and L2 to 2 sigma --> residuals over 2 sigma will be penalized via L1
	gradient = 2 * sigmas;

}



