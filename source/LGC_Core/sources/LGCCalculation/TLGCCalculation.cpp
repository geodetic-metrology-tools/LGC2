#include "TLGCCalculation.h"

#include <Logger.hpp>
#include <TLSAlgorithm.h>

#include "TDataAnalyzer.h"
#include "TLSAllfixed.h"
#include <TLSAlgorithm.h>
#include <TLSEvaluator.h>
#include <TLSDerivativeTester.h>
#include <TLSGaussNewtonSolver.h>
#include <TLSLMSolver.h>
#include "TVAbstractAlgorithm.h"
#include "TLSResultsMatrices.h"
#include <Logger.hpp>
#include <TLSEvaluator.h>
#include <TLSGraph.h>
#include "TLSSimulation.h"
#include "TVAbstractAlgorithm.h"

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLGCCalculation::TLGCCalculation(std::shared_ptr<TLGCData> dat, int maxIterations) : fData(dat), fMaxIterations(maxIterations), fResultsMtr(nullptr)
{
}

///////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
///////////////////////////////////////////////////////////////////////////
Behavior TLGCCalculation::computeResults(std::shared_ptr<TSimulationOutputFileWriter> fileWriter)
{
	std::unique_ptr<TVAbstractAlgorithm> algorithm;
	Behavior successCalculation;


	/*Class for analyzing the data.*/
	TDataAnalyzer analyzer(*fData.get());
	// Checks whether the data are consistent, assign unknown indices and initialize uninitialized objects (points, lines, planes), reference poiints for certain observations, etc.
	if (!analyzer.dataConsistent())
	{
		// throw std::runtime_error("Data are not consistent, see the output file: " + fData->getFileLogger().getOutputFileLocation() + " for more information.");
		return Behavior(Behavior::BehaviorCode::ERR_inputData, L"Data are not consistent, see the log file for more information.");
	}
	try
	{
		// Iteration through the points
		for (auto it(fData->getPoints().begin()); it != fData->getPoints().end(); ++it)
		{
			it->transformProvisionalCoordinates(fData.get());
		}

		algorithm.reset(new TLSAlgorithm(*fData.get()));
	   	{
	   		// only now the constraint dimensions are set.
			// testing derivatives
	   		TLSDerivativeTester tester(fData);
	   		TLSEvaluator auxEval(fData);
	   		Eigen::VectorXd provPar = auxEval.getEstParams();
	   		TLSEvaluator evaluator(fData);
	   		std::shared_ptr<TLSEvaluator> evalPtr = std::make_shared<TLSEvaluator>(evaluator);

	   		// plan: Gauss Newton solver with armijo backtracking
	   		TLSGaussNewtonSolver gnObject(evalPtr);

	   		// only use armijo gn if there are variables and there is no constraint
	   		if (fData.get()->fUEOIndices.UIndex > 0 && fData.get()->fUEOIndices.CIndex == 0 )
	   		{
				// experiment with the dulmage decomposition
				computeDulmageSequence();
				// solve the problem using Gauss Newton with stepsize regularization
	   			Eigen::VectorXd solution = gnObject.solve();
	   		}
	   		// reset parameters - to not interfere with usual LGC calculation
	   		//auxEval.setParameters(provPar);
	   	}

		if (fData->getConfig().sim.isActive())
			algorithm.reset(new TLSSimulation(*fData.get(), fMaxIterations, fileWriter));
		else if (fData->getConfig().allfixed.isActive())
			algorithm.reset(new TLSAllfixed(*fData.get(), fMaxIterations));

		successCalculation = algorithm->run(*fData.get(), fMaxIterations);
		// repeat test after convergence	
		// TLSDerivativeTester tester(fData);
		if (successCalculation)
		{
			fResultsMtr = algorithm->resultMatrices;

			// Iteration through the points
			for (auto it(fData->getPoints().begin()); it != fData->getPoints().end(); ++it)
			{
				it->setCovarianceMatrixInRoot(fData.get());
				it->transformEstimatedCoordinates(fData.get());
				it->changeProvValueToCCS(fData.get());
			}
		}
	}
	catch (std::exception &e)
	{
		fData->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << e.what();
		successCalculation += Behavior(Behavior::BehaviorCode::ERR_results, L"Problem during computation.");
	}

	// If the calculation succeeded, initialise the observation summaries:
	if (successCalculation)
		initialiseObsSummaries();

	return successCalculation;
}

void TLGCCalculation::initialiseObsSummaries(){

    // Iterate the whole tree and initialise the
    // observation summaries in each node:
    for(auto &node : fData->getTree())
        node->measurements.initialiseObsSummaries();
}

void TLGCCalculation::computeDulmageSequence(){
	// create a Evaluater pointer
	TLSEvaluator auxEval(fData);
	std::shared_ptr<TLSEvaluator> evalPtr = std::make_shared<TLSEvaluator>(auxEval);
	// use it to create a Gauss Newton solver object
	TLSGaussNewtonSolver gnSolver(evalPtr);

	// compute the Dulmage-Mendelsohn decomposition using the A matrix sparsity pattern
	Eigen::SparseMatrix<double> A_global = evalPtr->getA();
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
	std::vector<int> chosenRows = findFullRankSubMatrix(A_global_ordered);
	Eigen::MatrixXd reducedReorderedA = A_global_ordered.toDense()(chosenRows, Eigen::indexing::all);

	Eigen::SparseMatrix<double> AFinal = reducedReorderedA.sparseView();
	//std::cout << "sparsity row-ordered and reduced matrix" << std::endl;
	//plotSparsity(AFinal);

	// find maximum matching
	// create bipartite Graph encoding equation-parameter incidence
	BipGraph G(AFinal);
	// get the maximum matching and the associated fine Dulmage decomposition
	vector<std::pair<std::set<int>, std::set<int>>> fineDM_reducedReordered = G.getFineDulmage();


	// prepare vector containing the original indices
	vector<std::pair<std::set<int>, std::set<int>>> fineDM_reducedRealIndices;

	vector<int> orderedPIdx;
	vector<int> orderedEIdx;
	vector<int> blockSizes;

	// print information on the strongly connected components
	// transform indices back to original matrix (reversing row-reordering, full rank row eliminiation)
	int count = 1;
	for (auto comp : fineDM_reducedReordered)
	{
		set<int> eqComp = comp.first;
		set<int> parComp = comp.second;
		set<int> eqCompReal;
		set<int> parCompReal;
		std::cout << "Component " << count << " of size " << eqComp.size() << std::endl;
		blockSizes.push_back(eqComp.size());
		std::cout << "par Idx: ";
		for (auto pIdx : parComp)
		{
			orderedPIdx.push_back(pIdx - 1);
			parCompReal.insert(pIdx - 1);
			std::cout << pIdx - 1 << " , ";
		};
		std::cout << std::endl;
		std::cout << "eqn Idx: ";
		for (auto eqIdx : eqComp)
		{
			orderedEIdx.push_back(rowOrder[chosenRows[eqIdx - 1]]);
			eqCompReal.insert(rowOrder[chosenRows[eqIdx - 1]]);
			std::cout << rowOrder[chosenRows[eqIdx - 1]] << " , ";
		};
		fineDM_reducedRealIndices.push_back(std::make_pair(eqCompReal, parCompReal));
		std::cout << std::endl;
		count++;
	}
	//  comparing the sparsity patterns
	Eigen::MatrixXd A_global_dense = A_global.toDense();
	Eigen::MatrixXd test = A_global_dense(orderedEIdx, orderedPIdx);
	Eigen::SparseMatrix<double> test_sparse = test.sparseView();
	//std::cout << "Sparsity Pattern original A matrix:" << std::endl;
	//plotSparsity(A_global);
	std::cout << "Sparsity Pattern reduced and reordered A matrix:" << std::endl;
	plotSparsity(test_sparse, blockSizes);

	//	// perturb initial value to avoid singular matrices
	// TVector iniVal = evalPtr->getEstParams(false);
	// Eigen::VectorXd randVal(iniVal.rows());
	// std::cout << iniVal << std::endl;
	// randVal.setRandom();
	// randVal *= 1e+0;
	// iniVal= randVal.cwiseProduct(iniVal);
	// //randVal *= 0;
	// iniVal += randVal;
	// evalPtr->setParameters(iniVal, false);


	// iterate through the components and solve problemns of increasing size
	evalPtr->currentMask.equationIndices.clear();
	evalPtr->currentMask.parameterIndices.clear();
	int blockNumber = 0;
	for (auto compIt = fineDM_reducedRealIndices.rbegin(); compIt != fineDM_reducedRealIndices.rend(); ++compIt)
	{
		blockNumber++;
		// get the eq and par indices
		std::vector<int> eqIndices, parIndices;
		for (auto eqIdx : compIt->first)
		{
			eqIndices.push_back(eqIdx);
		}
		for (auto parIdx : compIt->second)
		{
			parIndices.push_back(parIdx);
		}
		//   // use them for the mask
		//   // OPTION 1: reset mask,only solve with equations and parameters corresponding to current component
		//   evalPtr->currentMask.equationIndices = eqIndices;
		//   evalPtr->currentMask.parameterIndices = parIndices;
		// OPTION 2:
		// gradually increase set of active parameters and equations
		for (auto parIdx : parIndices)
		{
			evalPtr->currentMask.parameterIndices.push_back(parIdx);
		}
		for (auto eqIdx : eqIndices)
		{
			evalPtr->currentMask.equationIndices.push_back(eqIdx);
		}

		// alternative use all eqautions associated
	//	evalPtr->currentMask.equationIndices = getAssociatedEquations(evalPtr->currentMask.parameterIndices, A_global);

		// make rudimentary consistency check before solve
		// because it can happen that the dulmage decomposition which is based on the structural rank (only based on sparsity pattern) overestimates the rank of the matrix
		// this can happen when for example a point is measured by two ANGL measurements from the same station, the corresponding 2x2 block (eq x pars) will have no zeros but the columns are linearly dependant
		Eigen::SparseMatrix<double> A = evalPtr->getA(true);
		std::cout << "sparsity pattern of current  masked matrix" << std::endl;
		plotSparsity(A);
		Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qrSolver;
		qrSolver.compute(A);

		if (qrSolver.info() == Eigen::Success)
		{
			std::cout << "Trying to solve block number " << blockNumber << " of size " << evalPtr->currentMask.parameterIndices.size() << std::endl;
			int rank = qrSolver.rank();
			// std::cout << "Current A block: " << std::endl << A.toDense() << std::endl;
			if (rank != A.cols())
			{
				std::cout << "A matrix has rank " << rank << " but there are " << A.cols() << " columns, so A has not full column rank." << std::endl;
			}
			else
			{
				// only solve if full rank
				// use the gn solver to solve the corrsponding subproblem
				std::cout << "A matrix has rank " << rank << " and there are " << A.cols() << " columns, solve will start." << std::endl;
				gnSolver.solve();
			}
		}
		else
		{
			throw std::runtime_error("Decomposition failed.");
		}
	}
	// remove the mask and solve again
	evalPtr->unmask();
	// final solve
	gnSolver.solve();




	int k = 0;
	true;
	//	
//	vector<int> orderedPIdx;
//
//	Eigen::MatrixXd A_dense = A_global.toDense();
//
//	// create bipartite Graph encoding equation-parameter incidence
//	BipGraph G(A_global);
//	// get the maximum matching and the associated fine Dulmage decomposition
//	vector<std::pair<std::set<int>, std::set<int>>> fineDM = G.getFineDulmage();
//	vector<int> orderedPIdx;
//	vector<int> orderedEIdx;
//
//	// print information on the strongly connected components
//	int count = 1;
//	for (auto comp : fineDM)
//	{
//		set<int> eqComp = comp.first;
//		set<int> parComp = comp.second;
//		std::cout << "Component " << count << " of size " << eqComp.size() << std::endl;
//		std::cout << "par Idx: ";
//		for (auto pIdx : parComp)
//		{
//			orderedPIdx.push_back(pIdx - 1);
//			std::cout << pIdx << " , ";
//		};
//		std::cout << std::endl;
//		std::cout << "eqn Idx: ";
//		for (auto eqIdx : eqComp)
//		{
//			orderedEIdx.push_back(eqIdx - 1);
//			std::cout << eqIdx << " , ";
//		};
//		std::cout << std::endl;
//		count++;
//	}
//
//	//  comparing the sparsity patterns
//	Eigen::MatrixXd test = A_dense(orderedEIdx, orderedPIdx);
//	Eigen::SparseMatrix<double> test_sparse = test.sparseView();
//	std::cout << "Sparsity Pattern original A matrix:" << std::endl;
//	// std::cout << A.toDense() << std ::endl;
//	plotSparsity(A_global);
//	std::cout << "Sparsity Pattern reduced and reordered A matrix:" << std::endl;
//	plotSparsity(test_sparse);
//
//	// find a subset of row indices such that the reduced A matrix has full rank (essentially a maximum matching with real full rank opposed to only structural full rank)
//	// order rows with respect to number of nonzeros
//
//	std::vector<int> chosenRows = findFullRankSubMatrix(A_global);
//	Eigen::SparseMatrix<double> intermediateA = A_dense(chosenRows, Eigen::indexing::all).sparseView();
//	//	plotSparsity(intermediateA);
//	// create bipartite Graph encoding equation-parameter incidence
//	BipGraph intermediateG(intermediateA);
//	// check the fine dulmage decomposition of the reduced matrix
//	vector<std::pair<std::set<int>, std::set<int>>> intermediateFineDM = intermediateG.getFineDulmage();
//
//	// check if the diagonal blocks of the dulmage decomposition are of full rank
//	std::cout << "full A matrix has dimensions " << A_dense.fullPivHouseholderQr().rank() << std::endl;
//	for (auto compIt = fineDM.rbegin(); compIt != fineDM.rend(); ++compIt)
//	{
//		std::vector<int> eqIndices, parIndices;
//		for (auto eqIdx : compIt->first)
//		{
//			eqIndices.push_back(eqIdx - 1);
//		}
//		for (auto parIdx : compIt->second)
//		{
//			parIndices.push_back(parIdx - 1);
//		}
//		Eigen::MatrixXd diagBlock = A_dense(eqIndices, parIndices);
//		int blockSize = parIndices.size();
//		int blockRank = diagBlock.fullPivHouseholderQr().rank();
//		std::cout << "Block has dimensions " << blockSize << " and rank " << blockRank;
//		if (blockRank < blockSize)
//		{
//			std::cout << " Block is rank deficient.";
//		}
//		std::cout << std::endl;
//	}
//
//	// create a sequence of well defined subproblems of increasing dimension and solve them using the masking method with the Gn solver
//
//	// loop over the components of the DM decomposition, following the topological ordering beginning from the block that depends on the least number of parameters
//	// set (or alternatively add them) the parameter mask to the parameters of the block, the equation mask to the equations of the block
//	// solve the problem
//
//	// perturb initial value to avoid singular matrices
//	//	TVector iniVal = evalPtr->getEstParams(false);
//	//	Eigen::VectorXd randVal(iniVal.rows());
//	//	randVal.setRandom();
//	//	//randVal *= 1e-2;
//	//	randVal *= 0;
//	//	iniVal += randVal;
//	//	evalPtr->setParameters(iniVal, false);
//
//	evalPtr->currentMask.equationIndices.clear();
//	evalPtr->currentMask.parameterIndices.clear();
//	int blockNumber = 0;
//	for (auto compIt = fineDM.rbegin(); compIt != fineDM.rend(); ++compIt)
//	{
//		blockNumber++;
//		// get the eq and par indices
//		std::vector<int> eqIndices, parIndices;
//		for (auto eqIdx : compIt->first)
//		{
//			eqIndices.push_back(eqIdx - 1);
//		}
//		for (auto parIdx : compIt->second)
//		{
//			parIndices.push_back(parIdx - 1);
//		}
//		//   // use them for the mask
//		//   // OPTION 1: reset mask,only solve with equations and parameters corresponding to current component
//		//   evalPtr->currentMask.equationIndices = eqIndices;
//		//   evalPtr->currentMask.parameterIndices = parIndices;
//		// OPTION 2:
//		// gradually increase set of active parameters and equations
//		for (auto parIdx : parIndices)
//		{
//			evalPtr->currentMask.parameterIndices.push_back(parIdx);
//		}
//		// for (auto eqIdx : eqIndices)
//		//{
//		//	evalPtr->currentMask.equationIndices.push_back(eqIdx);
//		// }
//
//		// alternative use all eqautions associated
//		evalPtr->currentMask.equationIndices = getAssociatedEquations(evalPtr->currentMask.parameterIndices, A_global);
//
//		// make rudimentary consistency check before solve
//		// because it can happen that the dulmage decomposition which is based on the structural rank (only based on sparsity pattern) overestimates the rank of the matrix
//		// this can happen when for example a point is measured by two ANGL measurements from the same station, the corresponding 2x2 block (eq x pars) will have no zeros but the columns are linearly dependant
//		Eigen::SparseMatrix<double> A = evalPtr->getA(true);
//		Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qrSolver;
//		qrSolver.compute(A);
//
//		if (qrSolver.info() == Eigen::Success)
//		{
//			std::cout << "Trying to solve block number " << blockNumber << " of size " << evalPtr->currentMask.parameterIndices.size() << std::endl;
//			int rank = qrSolver.rank();
//			// std::cout << "Current A block: " << std::endl << A.toDense() << std::endl;
//			if (rank != A.cols())
//			{
//				std::cout << "A matrix has rank " << rank << " but there are " << A.cols() << " columns, so A has not full column rank." << std::endl;
//			}
//			else
//			{
//				// only solve if full rank
//				// use the gn solver to solve the corrsponding subproblem
//				std::cout << "A matrix has rank " << rank << " and there are " << A.cols() << " columns, solve will start." << std::endl;
//				gnSolver.solve();
//			}
//		}
//		else
//		{
//			throw std::runtime_error("Decomposition failed.");
//		}
//	}
//	// remove the mask and solve again
//	evalPtr->unmask();
//	// final solve
//	gnSolver.solve();
}

vector<int> getAssociatedEquations(vector<int> parIdx, TSparseMatrix A)
{
	int coldim = A.cols();
	int rowDim = A.rows();
	vector<int> associatedEqIndices;
	// loop through equations ~ rows of matrix A
	for (int rowIdx = 0; rowIdx < rowDim; rowIdx++)
	{
		bool isAssociated = true;
		// check if row/eq only depends on parameters in parIdx
		for (Eigen::SparseMatrix<double>::InnerIterator it(A, rowIdx); it; ++it)
		{
			// test if parameter is in set
			if (std::count(parIdx.begin(), parIdx.end(), it.col()) == 0)
			{
				isAssociated = false;
				break;
			}
			//std::cout << "Row: " << it.row() << " Column: " << it.col() << " Value: " << it.value() << std::endl;
		}
		if (isAssociated)
		{
			associatedEqIndices.push_back(rowIdx);
		}
	}
	return associatedEqIndices;
}
