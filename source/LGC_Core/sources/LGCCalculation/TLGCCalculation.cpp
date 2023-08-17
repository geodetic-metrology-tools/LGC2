#include "TLGCCalculation.h"
#include "TDataAnalyzer.h"
#include "TLSSimulation.h"
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

//////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTOR
//////////////////////////////////////////////////////////////////////
TLGCCalculation::TLGCCalculation(std::shared_ptr<TLGCData> dat, int maxIterations) : fData(dat), fMaxIterations(maxIterations), fResultsMtr(nullptr)
{}

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
	try{
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
	   		auxEval.setParameters(provPar);
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
	catch (std::exception& e)
	{
		fData->getFileLogger() << TFileLogger::e_logType::LOG_ERROR << e.what();
		successCalculation += Behavior(Behavior::BehaviorCode::ERR_results, L"Problem during computation.");
	}

    // If the calculation succeeded, initialise the observation summaries:
    if(successCalculation)
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
	Eigen::SparseMatrix<double> A = evalPtr->getA();
	Eigen::MatrixXd A_dense = A.toDense();

	// create bipartite Graph encoding equation-parameter incidence
	BipGraph G(A);
	// get the maximum matching and the associated fine Dulmage decomposition
	vector<std::pair<std::set<int>, std::set<int>>> fineDM = G.getFineDulmage();
	vector<int> orderedPIdx;
	vector<int> orderedEIdx;

	// print information on the strongly connected components
	int count = 1;
	for (auto comp : fineDM)
	{
		set<int> eqComp = comp.first;
		set<int> parComp = comp.second;
		std::cout << "Component " << count << " of size " << eqComp.size() << std::endl;
		std::cout << "par Idx: ";
		for (auto pIdx : parComp)
		{
			orderedPIdx.push_back(pIdx - 1);
			std::cout << pIdx << " , ";
		};
		std::cout << std::endl;
		std::cout << "eqn Idx: ";
		for (auto eqIdx : eqComp)
		{
			orderedEIdx.push_back(eqIdx - 1);
			std::cout << eqIdx << " , ";
		};
		std::cout << std::endl;
		count++;
	}

	// // comparing the sparsity patterns
	// Eigen::MatrixXd test = A_dense(orderedEIdx, orderedPIdx);
	// Eigen::SparseMatrix<double> test_sparse = test.sparseView();
	// std::cout << "Sparsity Pattern original A matrix:" << std::endl;
	// // std::cout << A.toDense() << std ::endl;
	// plotSparsity(A);
	// std::cout << "Sparsity Pattern reduced and reordered A matrix:" << std::endl;
	// plotSparsity(test_sparse);

	// create a sequence of well defined subproblems of increasing dimension and dolve them using the masking method with the Gn solver
	
	// loop over the components of the DM decomposition, following the topological ordering beginning from the block that depends on the least number of parameters
	// set (or alternatively add them) the parameter mask to the parameters of the block, the equation mask to the equations of the block
	// solve the problem

	// perturb initial value to avoid singular matrices
//	TVector iniVal = evalPtr->getEstParams(false);
//	Eigen::VectorXd randVal(iniVal.rows());
//	randVal.setRandom();
//	//randVal *= 1e-2;
//	randVal *= 0;
//	iniVal += randVal;
//	evalPtr->setParameters(iniVal, false);

	evalPtr->currentMask.equationIndices.clear();
	evalPtr->currentMask.parameterIndices.clear();
	int blockNumber = 0;
	for (auto compIt = fineDM.rbegin(); compIt != fineDM.rend(); ++compIt)
	{
		blockNumber++;
		// get the eq and par indices
		std::vector<int> eqIndices, parIndices;
		for (auto eqIdx : compIt->first)
		{
			eqIndices.push_back(eqIdx - 1);
		}
		for (auto parIdx : compIt->second)
		{
			parIndices.push_back(parIdx - 1);
		}
		//   // use them for the mask
	 	//   // OPTION 1: reset mask,only solve with equations and parameters corresponding to current component
	 	//   evalPtr->currentMask.equationIndices = eqIndices;
	 	//   evalPtr->currentMask.parameterIndices = parIndices;
	  	// OPTION 2: 
	  	// gradually increase set of active parameters and equations
	  	for (auto eqIdx : eqIndices)
	  	{
	  		evalPtr->currentMask.equationIndices.push_back(eqIdx);
	  	}
	  	for (auto parIdx : parIndices)
	  	{
	  		evalPtr->currentMask.parameterIndices.push_back(parIdx);
	  	}
		// use the gn solver to solve the corrsponding subproblem
		std::cout << "Solving block number " << blockNumber << " of size " << evalPtr->currentMask.parameterIndices.size() << std::endl;
		gnSolver.solve();
	}
	// remove the mask and solve again
	evalPtr->unmask();
	// final solve
	gnSolver.solve();

}