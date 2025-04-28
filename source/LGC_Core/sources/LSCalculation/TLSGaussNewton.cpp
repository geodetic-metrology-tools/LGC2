#include <algorithm>
#include <iostream>

#include <Logger.hpp>
#include <TLSGaussNewton.h>

#include "QuantileFunctions.h"

TLSGaussNewton::TLSGaussNewton(std::shared_ptr<TLSEvaluator> evaluator) : fEvaluator(evaluator), indices(evaluator->getIndices())
{
	//resetOptions();
}

//GNresult TLSGaussNewton::solve(Eigen::VectorXd initial)
GNResult TLSGaussNewton::solve(Eigen::VectorXd initial)
{
	int maxIter = 100;

	double currentObjective = 1e+12;
	double predictedObjective = 1e+12;
	double trialObjective = 1e+12;
	
	TVector currentResidual(indices.OIndex);
	TVector predictedResidual(indices.OIndex);
	TVector trialResidual(indices.OIndex);

	// pseudo code outline
	TVector currentIterate = initial;
	TVector dx(indices.UIndex);
	dx.setConstant(1);
	TVector dxTrial(indices.UIndex);
	dxTrial.setZero();
	TVector trialIterate(indices.UIndex);
	// evaluate at current iterate
	fEvaluator->setParameters(currentIterate);
	fEvaluator->evaluate();
	TSparseMatrix& A_current = fEvaluator->getAMatrix();
	// for now assume B is invertable
	TSparseMatrix &invB_current = fEvaluator->getInvBMatrix();
	TVector &W_current = fEvaluator->getMisclosure();
	TSparseMatrix &Pv_current = fEvaluator->getPMatrix();
	TSparseMatrix &C_current = fEvaluator->getA2Matrix();
	TVector &W2_current = fEvaluator->getConstraintMisclosure();

	// compute current objective
	currentObjective = computeObjective(A_current, invB_current,Pv_current, W_current, dxTrial);
	// mark the dxTrial nonzero
//	dxTrial.setConstant(1);
	
	double lmPenalty = 1e-6;
	//double lmPenalty = 0;
	
	// outer loop
	int outerIt = 0;
	while (dx.norm() > 1e-6 && (currentObjective > 1e-12) && outerIt < maxIter)
	{
		// prepare the ingredients that are needed for each inner loop step:
		// the normal matrix (before regularization), the right hand side of the normal equation system
		TSparseMatrix &invN1_current = getInvN1Matrix(invB_current, Pv_current);
		TSparseMatrix &N2_current = getN2Matrix(A_current, invN1_current);
		TVector rhs_current(indices.UIndex + indices.CIndex);
		rhs_current << A_current.transpose()*invN1_current*W_current, W2_current;
		outerIt++;
		std::cout << "outer iteration " << outerIt << " current objective " << currentObjective << std::endl;
		//rhs_current.topRows(indices.UIndex)=W_current		
		// inner loop
		bool acceptLMStep = false;
		int innerIt = 0;
		while (!acceptLMStep)
		{
			innerIt++;
			std::cout << "		inner iteration " << innerIt;
			dxTrial = computeRegularizedStep(N2_current, C_current, rhs_current, lmPenalty);
			trialIterate = currentIterate + dxTrial;

			std::cout << " with LM factor "<< lmPenalty << " => proposed |dx_inner|=" << dxTrial.norm();

			// compute predicted objective
			predictedObjective = computeObjective(A_current, invB_current, Pv_current, W_current, dxTrial);

			// set and evaluate at new iterate to compute the actual trial objective
			fEvaluator->setParameters(trialIterate);
			fEvaluator->evaluate();
			//
			TSparseMatrix &A_trial = fEvaluator->getAMatrix();
			TSparseMatrix &invB_trial = fEvaluator->getInvBMatrix();
			TVector &W_trial = fEvaluator->getMisclosure();
			TVector &W2_trial = fEvaluator->getConstraintMisclosure();
			TSparseMatrix &Pv_trial = fEvaluator->getPMatrix();
			TSparseMatrix &C_trial = fEvaluator->getA2Matrix();
			// TVector &W2_trial= fEvaluator->getConstraintMisclosure();
			trialObjective = computeObjective(A_trial, invB_trial, Pv_trial, W_trial);
			// sufficient decreaseor step is already small
			acceptLMStep = decideAcceptance(currentObjective, predictedObjective, trialObjective) || dxTrial.norm()<1e-6;
			std::cout << " step accecpted: " << acceptLMStep << std::endl;

			
			if (acceptLMStep){
				// decrease lm penalty
				lmPenalty /= 10;
				// update data needed for the next outer iteration
				invB_current = invB_trial;
				Pv_current = Pv_trial;
				A_current = A_trial;
				W_current = W_trial;
				W2_current = W2_trial;
				dx = dxTrial;
				// do the actual step
				currentIterate += dx;
				currentObjective = trialObjective;
			}
			else
			{
				lmPenalty *= 2;
			}
		}
	}


	GNResult result;
	if ((dx.norm() < 1e-6) && (outerIt < maxIter))
		result.success = true;
	result.iniVal = initial;
	result.objective = currentObjective;
	result.solution = currentIterate;
	result.nIterations = outerIt;

	// compute expected sigmas limits
	int d = indices.EIndex - indices.UIndex + indices.CIndex;
	result.sigma0Aposteriori = sqrt(currentObjective / d);

	limits fisherLim = {0, 0};
	if (d > 0)
	{
		double chiUp = deviates_chi_sq_0975(d);
		double chiLow = deviates_chi_sq_0025(d);

		// Limits
		fisherLim.s0PostUpLimit = sqrtq(chiUp / d);
		fisherLim.s0PostLoLimit = sqrtq(chiLow / d);
	}

	result.isInLimits = (result.sigma0Aposteriori < fisherLim.s0PostUpLimit) && (result.sigma0Aposteriori > fisherLim.s0PostLoLimit);


	return result;
}

double TLSGaussNewton::computeObjective(TSparseMatrix &A, TSparseMatrix &invB, TSparseMatrix &Pv, TVector &W, TVector &dxTrial)
{
	// A dx + B V +W=0
	// compute V^T Pv V
	// assume B is invertible
	TVector V = -invB * (W + A * dxTrial);
	double objective = V.transpose() * Pv * V;
	return objective;
}

double TLSGaussNewton::computeObjective(TSparseMatrix &A, TSparseMatrix &invB, TSparseMatrix &Pv, TVector &W)
{
	TVector dxTrialDefault(indices.UIndex);
	dxTrialDefault.setZero();
	return computeObjective(A, invB, Pv, W, dxTrialDefault);
}

TVector TLSGaussNewton::computeRegularizedStep(TSparseMatrix &N2, TSparseMatrix &C, TVector &rhs, double lmPenalty)
{
	// assemble the regularized normal matrix with constraints
	TSparseMatrix NBigRegularized = getNBig(N2, C, lmPenalty);
	TVector solutionExt(indices.UIndex + indices.CIndex);
	solutionExt.setZero();
	bool solveSuccess = TSparseUtils::solveUnique(NBigRegularized, -rhs, solutionExt, (indices.CIndex == 0), (indices.CIndex == 0));
//	std::cout << "lin system sol" << std::endl << solutionExt << std::endl;
	if (!solveSuccess)
	{
		throw std::runtime_error("problem during step computation.");
	}
	TVector solution = solutionExt.topRows(indices.UIndex);
	return solution;
}

bool TLSGaussNewton::decideAcceptance(double currObj, double predObj, double trialObj)
{
	bool accept = true;
	double predictedDecrease = currObj - predObj;
	double actualDecrease = currObj - trialObj;
	std::cout << "current, predicted,trial " << currObj << " , " << predObj << " , " << trialObj << std::endl;
	std::cout << std::endl << "actual decrease /predictedDecrease=" << actualDecrease / predictedDecrease << std::endl;
	// checks for early acceptance
	// accept if the trial objective is close to zero or the predicted decrease is not significant anymore
	if ((trialObj < 1e-12) || (fabs(predictedDecrease) < 1e-4))
	{
		return true;
	}
	// do not allow actual increase
	if (actualDecrease < 0)
	{
		return false;
	}
	// raise error if linearized model objective increases (theoretically impossible, if this is the case there is a numeriocal problem with the linear system)
	if (predictedDecrease < -1e-12)
	{
		throw std::runtime_error("predicted objective increased! We need decrease");
	}

	// the actual acceptance check for sufficient objectivedecrease
	double acceptanceFactor = 0.1;
	if (actualDecrease < acceptanceFactor * predictedDecrease)
	{
		return false;
	}
	return accept;
}

TSparseMatrix TLSGaussNewton::getInvN1Matrix(TSparseMatrix &invB, TSparseMatrix &Pv)
{
	TSparseMatrix invN1 = invB.transpose() * Pv * invB;
	return invN1;
}

TSparseMatrix TLSGaussNewton::getN2Matrix(TSparseMatrix &A, TSparseMatrix &invN1)
{
	// TSparseMatrix N2(indices.UIndex, indices.UIndex);
	TSparseMatrix N2 = A.transpose() * invN1 * A;
	return N2;
}

TSparseMatrix TLSGaussNewton::getNBig(TSparseMatrix &N2, TSparseMatrix &C, double lmCoefficient)
{
	TSparseMatrix N2regularized = N2;
	
	bool useScaledPenalty = true;

	TSparseMatrix NBig(indices.UIndex + indices.CIndex, indices.UIndex + indices.CIndex);
	std::vector<TTriplet> coeffs;
	coeffs.reserve(N2regularized.nonZeros() + 2 * C.nonZeros());

	// Fill in the N2regularized part
	for (int k = 0; k < N2regularized.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(N2regularized, k); it; ++it)
		{
			double value = it.value();
			if (it.row()==it.col()){
				// add penalty diagonal term, either scaled according to J^T J or unscaled
				if (useScaledPenalty)
					value *= (1 + lmCoefficient);
				else
					value += lmCoefficient;
			}
			coeffs.push_back(TTriplet(it.row(), it.col(), value));
		}
	}

	// Fill the C and CT, if no constraints, nothing should happen
	for (int k = 0; k < C.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(C, k); it; ++it)
		{
			coeffs.push_back(TTriplet(it.row() + N2regularized.rows(), it.col(), it.value())); // C
			coeffs.push_back(TTriplet(it.col(), it.row() + N2regularized.cols(), it.value())); // CT
		}
	}
	NBig.setFromTriplets(coeffs.begin(), coeffs.end());

	return NBig;

}

