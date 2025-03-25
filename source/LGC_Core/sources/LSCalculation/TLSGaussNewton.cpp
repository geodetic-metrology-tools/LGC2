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
TVector TLSGaussNewton::solve(Eigen::VectorXd initial)
{
	TVector result(indices.UIndex);
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
	
	double lmPenalty = 1e-4;
	//double lmPenalty = 0;
	
	// outer loop
	int outerIt = 0;
	while (dx.norm() > 1e-6 && (currentObjective>1e-16))
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
			acceptLMStep = decideAcceptance(currentObjective, predictedObjective, trialObjective);
			std::cout << " . Step accepted: " << acceptLMStep << std::endl;
			
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



	result = currentIterate;
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
//	std::cout << "cObj=" << currObj << std::endl;
//	std::cout << "pObj=" << predObj << std::endl;
//	std::cout << "tObj=" << trialObj << std::endl;
//	std::cout << "pRed=" << predictedDecrease << std::endl;
//	std::cout << "aRed=" << actualDecrease << std::endl;
	if (trialObj < 1e-16)
		return true;
	if (actualDecrease < 0)
		accept &= false;
	if (predictedDecrease < 0)
		throw std::runtime_error("predicted objective increased! We need decrease");
	double acceptanceFactor = 0.01;
	if (actualDecrease > acceptanceFactor * predictedDecrease)
		accept &= true;
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
	//std::cout << "N2=" <<std::endl<< N2.toDense() << std::endl;
	// apply the LM regularization. Add lmCoefficient * diagonal entry to the diagonal (this corresponds to a variable scaling)
	for (int j = 0; j < indices.UIndex; j++)
	{
		N2regularized.coeffRef(j, j) *= lmCoefficient + 1;
	}

	//std::cout << "N2reg=" << std::endl << N2regularized.toDense() << std::endl;
	TSparseMatrix NBig(indices.UIndex + indices.CIndex, indices.UIndex + indices.CIndex);
	std::vector<TTriplet> coeffs;
	coeffs.reserve(N2regularized.nonZeros() + 2 * C.nonZeros());

	// Fill in the N2regularized part
	for (int k = 0; k < N2regularized.outerSize(); ++k)
	{
		for (TSparseMatrix::InnerIterator it(N2regularized, k); it; ++it)
			coeffs.push_back(TTriplet(it.row(), it.col(), it.value()));
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

