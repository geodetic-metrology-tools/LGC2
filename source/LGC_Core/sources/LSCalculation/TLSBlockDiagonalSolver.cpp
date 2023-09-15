#include "TLSBlockDiagonalSolver.h"

TLSBlockDiagonalSolver::TLSBlockDiagonalSolver(std::shared_ptr<TLSEvaluator> evaluator) : fEvaluator(evaluator)
{
}

void TLSBlockDiagonalSolver::solve()
{
	
	// compute the block diagonal decomposition
	Eigen::SparseMatrix<double> A_global = fEvaluator->getA();
	//plotSparsity(A_global);
	int nRows = A_global.rows();
	// get rowOrdering
	std::vector<int> rowOrder = getRowOrdering(A_global);
	// permute A according to roworder
	Eigen::PermutationMatrix<Eigen::Dynamic, Eigen::Dynamic> rowOrderPermutation(nRows);
	for (int i = 0; i < nRows; ++i)
	{
		rowOrderPermutation.indices()(rowOrder[i]) = i;
	}
	Eigen::SparseMatrix<double> A_global_ordered = rowOrderPermutation * A_global;
//	std::cout << "sparsity original matrix" << std::endl;
//	plotSparsity(A_global);
//	std::cout << "sparsity row ordered matrix" << std::endl;
//	plotSparsity(A_global_ordered);
	// succesively eliminate equations that do not increase the rank. goal is to get a square rank = nRows  submatrix
	//std::vector<int> chosenRows = findFullRankSubMatrix(A_global_ordered);
	std::vector<int> chosenRows = findFullRankSubMatrixWithQR(A_global_ordered);

	Eigen::SparseMatrix<double>  reducedReorderedA = maskRows(chosenRows, A_global_ordered);

	//Eigen::SparseMatrix<double> AFinal = reducedReorderedA.sparseView();
	Eigen::SparseMatrix<double> AFinal = reducedReorderedA;
	//std::cout << "sparsity row-ordered and reduced matrix" << std::endl;
	//plotSparsity(AFinal);

	// find maximum matching
	// create bipartite Graph encoding equation-parameter incidence
	BipGraph G(AFinal);
	// get the maximum matching and the associated fine Dulmage decomposition
	vector<std::pair<std::set<int>, std::set<int>>> fineDM_reducedReordered = G.getFineDulmage();


}

void TLSBlockDiagonalSolver::prepareDecomposition()
{
}
