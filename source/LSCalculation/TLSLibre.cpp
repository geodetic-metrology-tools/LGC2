#include "TLSLibre.h"
#include "TLGCData.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLSParametricMtdComputer.h"
#include "TLSCombinedMtdComputer.h"
#include "TLSWeightedUnkMtdComputer.h"
#include "TLSCnstMtdComputer.h"
#include "TLSInputMatricesFiller.h"

TLSLibre::TLSLibre(TLGCData& data, int maxIter) :fPointTransformer(&data.getTree(), data.getConfig().referential),
fLibrCnstrGenerator(fPointTransformer, data)
{}

bool TLSLibre::run(TLGCData& data, int fMaxIterations)
{
	std::unique_ptr<TALSComputer> computer;
	std::unique_ptr<TLSInputMatricesFiller> matrFiller(new TLSInputMatricesFiller(&data.getTree(), data.getConfig().referential));
	std::unique_ptr<TLSInputMatrices> inputMtr(new TLSInputMatrices());
	std::unique_ptr<TLSResultsMatrices> resultMatrices(new TLSResultsMatrices(data.fUEOIndices));
	extractor = std::make_unique<TLSResultsMatricesExtractor>(&data);

	// identify the constraints necessary, create them
	fLibrCnstrGenerator.initCnstrIdentifier(data);
	data.setNumberOfConstraints(fLibrCnstrGenerator.getNumberOfConstraint());
	
	//choose LS algorithm
	if (fLibrCnstrGenerator.getNumberOfConstraint()!=0)
		computer.reset(new TLSCnstMtdComputer());
	else
	{
		if (data.isCombinedCaseUsed())
			computer.reset(new TLSCombinedMtdComputer());
		else if (data.hasStandDeviations())
			computer.reset(new TLSWeightedUnkMtdComputer());
		else
			computer.reset(new TLSParametricMtdComputer());
	}

	//run calculation
	bool computationIsOK = iterate2Solution(data, matrFiller.get(), inputMtr.get(), computer.get(), resultMatrices.get(), fMaxIterations, data.getConfig().outPrecision.convCrit);


	return computationIsOK;

}

bool	TLSLibre::iterate2Solution(TLGCData& data,
	TLSInputMatricesFiller* matrFiller,
	TLSInputMatrices* inputMtr,
	TALSComputer* computer,
	TLSResultsMatrices* resultMatrices,
	int fMaxIterations,
	TReal convCrit){


	bool lastIteration = false;
	fNumberOfIterations = 0;

	TFileLogger& fileLog = data.getFileLogger();

	// Iterate to find solution
	while (!lastIteration && fNumberOfIterations < fMaxIterations)
	{
		bool fillOK = false;
		if (fNumberOfIterations == 0)//First iteration, fill also the weight unknown matrix.
			fillOK = matrFiller->fillMatrices(&data, true, inputMtr);
		else//In the following iteration the weight matrix remains unchanged, no need to be filled with the same values again.
			fillOK = matrFiller->fillMatrices(&data, false, inputMtr);

		

		//fill part of the free constraints
		fLibrCnstrGenerator.processFreeCnstr(*inputMtr);
		inputMtr->saveMatricesToFile(100 + fNumberOfIterations);

		if (fillOK)
		{
			// compute solution 
			bool computationOK = computer->computeResults(inputMtr, resultMatrices);

			if (computationOK)
			{
				bool extractOK = false;
				extractOK = extractor->extractResults(*resultMatrices, convCrit, fLibrCnstrGenerator);
				if (extractOK)
					lastIteration = extractor->lastIteration();
				else{
					fileLog << "Problem in LS matrices extraction.\n";
					return false; //Error during extraction, errors written out already, STOP the calculation.	
				}
			}
			else
			{
				//Write errors which occured in computer of LS methos
				fileLog << "Problem with LS computation\n";
				return false;
			}
		}
		else
		{
			fileLog << "Matrices filling was not successful.\n";
			return false;
		}
		fNumberOfIterations++;
	}

	// need to look if the convergence criteria is exeded (lastiteration=true) or not (lastiteration=false) at the maximal iteration
	if (fNumberOfIterations == fMaxIterations && !lastIteration)
	{
		fileLog << "The calculation is not converging \n";
		return false; //Error during the calculation, errors written out already, STOP the calculation.	
	}
	else
	{
		computeVarCovarAndReliability(&data, inputMtr, computer, resultMatrices);
		return true;
	}
}