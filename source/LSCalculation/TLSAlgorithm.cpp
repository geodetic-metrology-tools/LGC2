#include "TLSAlgorithm.h"
#include "TLSInputMatricesFiller.h"
#include "TLSParametricMtdComputer.h"
#include "TLSCombinedMtdComputer.h"
#include "TLSWeightedUnkMtdComputer.h"
#include "TLSCnstMtdComputer.h"


TLSAlgorithm::TLSAlgorithm(TLGCData& data)
	: fNumberOfIterations(0)
	, fS0APosterioriVariances(false)
	,fPointTransformer(&data.getTree(), data.getConfig().referential)
	,fLibrCnstrGenerator(fPointTransformer, data)
{
	delete resultMatrices;
	resultMatrices = new TLSResultsMatrices(data.fUEOIndices);
}

bool TLSAlgorithm::run(TLGCData& data, int fMaxIterations)
{
	std::unique_ptr<TALSComputer> computer;
	std::unique_ptr<TLSInputMatricesFiller> matrFiller(new TLSInputMatricesFiller(&data.getTree(), data.getConfig().referential));
	std::unique_ptr<TLSInputMatrices> inputMtr(new TLSInputMatrices());
	//std::unique_ptr<TLSResultsMatrices> resultMatrices(new TLSResultsMatrices(data.fUEOIndices));

	extractor = std::make_unique<TLSResultsMatricesExtractor>(&data);
	
	// identify the constraints necessary, create them
	if (data.getConfig().libre.isActive())
	{
		fLibrCnstrGenerator.initCnstrIdentifier(data);
		data.setNumberOfConstraints(fLibrCnstrGenerator.getNumberOfConstraint());
	}


	//choose LS algorithm
	if (fLibrCnstrGenerator.getNumberOfConstraint() != 0)
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

	bool computationIsOK = iterate2Solution(data, matrFiller.get(), inputMtr.get(), computer.get(), fMaxIterations, data.getConfig().outPrecision.convCrit);

	return computationIsOK;
}


bool	TLSAlgorithm::iterate2Solution(TLGCData& data,
	TLSInputMatricesFiller* matrFiller,
	TLSInputMatrices* inputMtr,
	TALSComputer* computer,
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
		if (data.getConfig().libre.isActive())
		{
			fLibrCnstrGenerator.processFreeCnstr(*inputMtr);
			inputMtr->saveMatricesToFile(100 + fNumberOfIterations);
		}

		if (fillOK)
		{
			// compute solution 
			bool computationOK = computer->computeResults(inputMtr, resultMatrices);

			if (computationOK)
			{
				bool extractOK = false; 
				if (data.getConfig().libre.isActive())
					extractOK = extractor->extractResults(*resultMatrices, convCrit, fLibrCnstrGenerator);
				else
					extractOK = extractor->extractResults(*resultMatrices, convCrit);

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
		computeVarCovarAndReliability(&data, inputMtr, computer);
		return true;
	}
}

void	TLSAlgorithm::computeVarCovarAndReliability(TLGCData* data,
	TLSInputMatrices* inputMtr,
	TALSComputer* computer)
{
	computer->calcResiduAndVarCovMatrice(inputMtr, resultMatrices);
	extractor->extractResiduals(*resultMatrices);

	TReal S0 = sqrt(resultMatrices->getSigmaZero2());
	// Apply S02 only if APRI is not used and S02 is outside the limits
	if (!data->getConfig().useApriori.isActive() && (S0>resultMatrices->getSigmaZeroUpLimit() || S0<resultMatrices->getSigmaZeroLowLimit()))
	{
		TSparseMatrix* UnkCovar = resultMatrices->getUnkCovarMtrx();
		(*UnkCovar) *= resultMatrices->getSigmaZero2();
		resultMatrices->setUnkCovarMtrx(UnkCovar);
	}

	extractor->extractVarCovarParams(*resultMatrices);
	data->setNumberOfLSIterations(fNumberOfIterations);

	/* Store calculated SIGMA A POSTERIORI in TLGCData and limits for it */
	data->setS0APosteriori(S0);
	data->setChiS0Limits(resultMatrices->getSigmaZeroLowLimit(), resultMatrices->getSigmaZeroUpLimit());


	if ((data->fUEOIndices.UIndex != 0) && data->getConfig().faut.isActive())
	{
		//re-initialize the statictic vectors
		data->getStatistics().initialiseStatVector(inputMtr);
		TReal alpha = data->getConfig().faut.alpha / 100;
		TReal beta = data->getConfig().faut.beta / 100;

		//compute statistics (Z, W, T, G, NABLA and DELTY)
		data->getStatistics().calcReliabilityVector(alpha, beta, inputMtr, resultMatrices);

		//calcRelErrorParams( &extractor);
		//ellips erros
	}
	else
	{//pas de probleme de convergence dans un calcul sans inconnues
		//calcDS.setConvergenceResult(lastIteration);
	}
}