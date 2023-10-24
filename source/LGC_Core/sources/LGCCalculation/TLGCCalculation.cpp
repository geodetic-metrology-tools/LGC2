#include "TLGCCalculation.h"

#include <Logger.hpp>
#include <TLSAlgorithm.h>

#include "TDataAnalyzer.h"
#include "TLSAllfixed.h"
#include <TLSAlgorithm.h>
#include <TLSEvaluator.h>
#include <TLSDerivativeTester.h>
#include <TLSGaussNewtonSolver.h>
#include <TLSRobustSolver.h>
#include <TLSLMSolver.h>
#include "TVAbstractAlgorithm.h"
#include "TLSResultsMatrices.h"
#include <Logger.hpp>
#include <TLSEvaluator.h>
#include <TLSGraph.h>
#include "Serializer_json.hpp"
#include <fstream>
#include <Timer.h>
#include <algorithm>


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
			//	TLSDerivativeTester tester(fData);
		
			// test different globalization methods
			try
			{
				TLSEvaluator evaluator(fData);
				std::shared_ptr<TLSEvaluator> evalPtr = std::make_shared<TLSEvaluator>(evaluator);
				TLSRobustSolver huber(evalPtr);
				huber.solve();
				// big gamma means its basically L2
				huber.setGamma(10000);
				huber.solve();
			//	testGlobalizationMethods();
			}
			catch (const std::exception &e)
			{
				// Code to handle the exception
				std::cerr << "An exception occurred: " << e.what() << std::endl;
				exit(0);
			}
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

void TLGCCalculation::testGlobalizationMethods()
{
	TLSEvaluator evaluator(fData);
	std::shared_ptr<TLSEvaluator> evalPtr = std::make_shared<TLSEvaluator>(evaluator);

	TVector provisionalVal = evalPtr->getEstParams(false);
	// Throw away any apriori knowledge in form of provisional values
	Eigen::VectorXd iniVal = provisionalVal;
	Eigen::VectorXd pertVal = provisionalVal;
	// // set values randomly
	// pertVal.setRandom();
	//	iniVal += 1e+1*pertVal;
	// set values linearly spaced
	for (int j = 0; j < iniVal.rows(); j++)
	{
		iniVal(j) = 50 + 10 * double(j + 1) / double(iniVal.rows());
	}
	evalPtr->setParameters(iniVal, false);
	
	// create Gauss Newton solver instance
	TLSGaussNewtonSolver gnObject(evalPtr);

	// solverConfigs to test
	solverConfig fullStepGN = {2, false, false, 0, 100, 1e-6};
	solverConfig armijoGN = {2, true, false, 0, 100, 1e-6};
	solverConfig LMregGN = {2, false, true, 1e-14, 100, 1e-6};
	solverConfig LMandArmijoregGN = {2, true, true, 1e-14, 100, 1e-6};
	
	std::vector<solverConfig> testConfigs = {fullStepGN, armijoGN, LMregGN, LMandArmijoregGN};
	//std::vector<solverConfig> testConfigs = { fullStepGN, LMregGN, LMandArmijoregGN};
	//std::vector<solverConfig> testConfigs = { LMandArmijoregGN};
	//std::vector<solverConfig> testConfigs = {armijoGN};
	//std::vector<solverConfig> testConfigs = {};


	for (auto config : testConfigs)
	{
		jsonSerializerObject ser;
		SerializerObject::SerializationHelper serobj = ser.getSerializationHelper();
		// set initial value
		evalPtr->setParameters(iniVal, false);
		// set config
		gnObject.setConfig(config);
		// try solution
		GNresult result;
		try
		{
			result = gnObject.solve();
		}
		catch (...)
		{
			std::cout << "GN solution failed" << std::endl;
		}
		serobj.addProperty("File", fData.get()->getFileLogger().getOutputFileLocation());
		serobj.addProperty("Result", result);
		serobj.addProperty("Configuration", config);
		//std::cout << ser.getStringRepresentation() << std::endl;
		std::ofstream outputFile("../studyResults.txt", std::ios::app);
		outputFile << ser.getStringRepresentation() << "\n";
		outputFile.close();
		if (result.success == true)
		{
		//	break;
		}
	}
	
	// reset initial value
	evalPtr->setParameters(iniVal, false);
 	// try to solve the problem via a Dulmage Mendelsohn sequence
 	std::cout << "Sequence of subproblems according to strongly connected components" << std::endl;
	computeDulmageSequence();
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
	// make sure the matrix on which the fine dulmage decomposition is performed has full rank
	std::vector<int> chosenRows = findFullRankSubMatrixWithQR(A_global_ordered);

	Eigen::SparseMatrix<double>  reducedReorderedA = maskRows(chosenRows, A_global_ordered);

	Eigen::SparseMatrix<double> AFinal = reducedReorderedA;

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
		blockSizes.push_back(eqComp.size());
		for (auto pIdx : parComp)
		{
			orderedPIdx.push_back(pIdx - 1);
			parCompReal.insert(pIdx - 1);
		};
		for (auto eqIdx : eqComp)
		{
			orderedEIdx.push_back(rowOrder[chosenRows[eqIdx - 1]]);
			eqCompReal.insert(rowOrder[chosenRows[eqIdx - 1]]);
		};
		fineDM_reducedRealIndices.push_back(std::make_pair(eqCompReal, parCompReal));
		count++;
	}
	//  comparing the sparsity patterns original vs dulmage decomp
	// 	Eigen::SparseMatrix<double> test_sparse = maskRows(orderedEIdx, maskColumns(orderedPIdx, A_global));
	// 	std::cout << "Sparsity Pattern original A matrix:" << std::endl;
	// 	plotSparsity(A_global);
	// 	std::cout << "Sparsity Pattern reduced and reordered A matrix:" << std::endl;
	// 	plotSparsity(test_sparse, blockSizes);


	// iterate through the components and solve problemns of increasing size
	evalPtr->currentMask.equationIndices.clear();
	evalPtr->currentMask.parameterIndices.clear();
	int blockNumber = 0;

	// this parameter controls the minimum block size for which we attempt to solve the problem. This is to avoid too much small subproblems which could take a longer time.
	//int minBlockSize = evalPtr->dimensions.UIndex / 5.0;
	int minBlockSize = std::min((double) evalPtr->dimensions.UIndex / 5.0, 50.0);
	int newParametersSinceLastSolve = 0;

	for (auto compIt = fineDM_reducedRealIndices.rbegin(); compIt != fineDM_reducedRealIndices.rend(); ++compIt)
	{
		blockNumber++;
		// get the eq and par indices
		auto eqns = compIt->first;
		auto pars = compIt->second;
		std::vector<int> eqIndices(eqns.begin(), eqns.end()), parIndices(pars.begin(), pars.end());
		// use them for the mask
		//   // OPTION 1: reset mask,only solve with equations and parameters corresponding to current component
	// 	evalPtr->currentMask.equationIndices = eqIndices;
	// 	evalPtr->currentMask.parameterIndices = parIndices;
		// OPTION 2:
		// gradually increase set of active parameters and equations
		evalPtr->currentMask.parameterIndices = concatenate(evalPtr->currentMask.parameterIndices, parIndices);
		// check what is the size of the parameters that are added since last solve
		newParametersSinceLastSolve += parIndices.size();
		evalPtr->currentMask.equationIndices = concatenate(evalPtr->currentMask.equationIndices, eqIndices);

		// alternative use all equations associated
		evalPtr->currentMask.equationIndices = getAssociatedEquations(evalPtr->currentMask.parameterIndices, A_global);



		// make rudimentary consistency check before solve
		// because it can happen that the dulmage decomposition which is based on the structural rank (only based on sparsity pattern) overestimates the rank of the matrix
		// this can happen when for example a point is measured by two ANGL measurements from the same station, the corresponding 2x2 block (eq x pars) will have no zeros but the columns are linearly dependant
		// in theory this should be avoided by the findFullRankSubmatrix which is performed before the computation of the strongly connected components
		Eigen::SparseMatrix<double> A = evalPtr->getA(true);
		
	//  	std::cout << "sparsity pattern of current  masked matrix" << std::endl;
	//  	plotSparsity(A);
		
		Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::NaturalOrdering<int>> qrSolver;
		qrSolver.compute(A);
		
		GNresult blockResult;
		Eigen::VectorXd previousGuess = evalPtr->getEstParams(true);
	
		// solver configurations
		solverConfig fullStepGN = {2, false, false, 0, 100, 1e-6};
		solverConfig armijoGN = {2, true, false, 0, 100, 1e-6};
		solverConfig LMregGN = {2, false, true, 1e-12, 50, 1e-6};
		solverConfig LMandArmijoregGN = {2, true, true, 1e-12, 100, 1e-6};


		// check if a solve attempt should be done (enough new parameters added?)
		if (newParametersSinceLastSolve >= minBlockSize)
		{
			// reset the newly added parameter counter
			newParametersSinceLastSolve = 0;
			if (qrSolver.info() == Eigen::Success)
			{
				std::cout << "Trying to solve block number " << blockNumber << " of size " << evalPtr->currentMask.parameterIndices.size() << std::endl;
				int rank = qrSolver.rank();
				if (rank != A.cols())
				{
					std::cout << "A matrix has rank " << rank << " but there are " << A.cols() << " columns, so A has not full column rank." << std::endl;
					//
					// in the rank deficient case only use LM regularized solvers to guarantee psoitive definiteness
					std::vector<solverConfig> robustConfigs = {LMregGN, LMandArmijoregGN};
					for (auto config : robustConfigs)
					{
						gnSolver.setConfig(config);
						try
						{
							blockResult = gnSolver.solve();
						}
						catch (...)
						{
							blockResult.success = false;
						}	
						if (blockResult.success == true || blockResult.objective < 1e-4)
						{
							break;
						}
					}
				}
				else
				{
					// only solve if full rank
					// use the gn solver to solve the corrsponding subproblem
					std::cout << "A matrix has rank " << rank << ". There are " << A.cols() << " columns and " << A.rows() << " equations. Solve will start." << std::endl;
					// try to solve the subblock with different solver settings, starting with armijo linesearch: full GN, going to the more robust ones
					// solverConfigs to test
					// first try LM
					std::vector<solverConfig> blockSolverConfigs = {LMregGN, armijoGN, LMandArmijoregGN};

					for (auto config : blockSolverConfigs)
					{
						gnSolver.setConfig(config);
						try
						{
							blockResult = gnSolver.solve();
						}
						catch (...)
						{
							blockResult.success = false;
						}
						if (blockResult.success == true || blockResult.objective < 1e-4)
						{
							break;
						}
					}
				}
			}
			else
			{
				throw std::runtime_error("Decomposition failed.");
			}
			Eigen::VectorXd newGuess = evalPtr->getEstParams(true);
			// std::cout << newGuess - previousGuess << std::endl;

			// check if block solve failed
			if (blockResult.success || blockResult.objective < 1e-4)
			{
				// reset the newly added parameter counter
				//newParametersSinceLastSolve = 0;
			}
			else
			{
				// if solve was not successful, reject the block iterations
				evalPtr->setParameters(previousGuess, true);
			}
		}
	}
	// remove the mask and solve again
	evalPtr->unmask();
	// final solve
	gnSolver.setOption("useLM", true);
	gnSolver.solve();
	// switch to full step to confrim solution
	gnSolver.resetOptions();
	gnSolver.solve();

}
vector<int> getAssociatedEquations(vector<int> parIdx, TSparseMatrix A)
{
	int colDim = A.cols();
	int rowDim = A.rows();
	vector<int> associatedEqIndices;
	// loop through equations ~ rows of matrix A
	for (int rowIdx = 0; rowIdx < rowDim; rowIdx++)
	{
		bool rowDependsOnSomeParameter = false;
		// check if row/eq only depends on parameters in parIdx
		bool isAssociated = true;
		for (int colIdx = 0; colIdx < colDim; colIdx++)
		{
			for (Eigen::SparseMatrix<double>::InnerIterator it(A, colIdx); it; ++it)
			{
				if (it.row() == rowIdx)
				{
					rowDependsOnSomeParameter = true;
					// the row depends on parameter colIdx
					// check if the colIdx is in the allowed set
					if (std::count(parIdx.begin(), parIdx.end(), colIdx) == 0)
					{
						//not allowed
						isAssociated = false;
					}
				}
			}
			if (isAssociated == false)
			{
				// row does also depend on other parameters
				break;
			}
		}
		if (isAssociated && rowDependsOnSomeParameter)
		{
			associatedEqIndices.push_back(rowIdx);
		}
	}
	return associatedEqIndices;
}



