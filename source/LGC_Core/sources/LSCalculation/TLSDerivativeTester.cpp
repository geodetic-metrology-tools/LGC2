#include <TLSDerivativeTester.h>
#include <Logger.hpp>
#include <iostream>

TLSDerivativeTester::TLSDerivativeTester(std::shared_ptr<TLGCData> data) : fEvaluator( TLSEvaluator(data) )
{

}

bool TLSDerivativeTester::testFirstDesignMatrix()
{
	// get value that is currently set in the TLGCData structure, if called at the beginning, this will be the provisional values
	Eigen::VectorXd prov = fEvaluator.getEstParams();

	// evaluate A matrix according to inputMatrixFiller, contributionGenerator etc..
	Eigen::MatrixXd computedJacobian = fEvaluator.evaluateA(prov);
	
	// evaluate A matrix according to finite differences applied to the misclosure vector
	Eigen::MatrixXd finiteDifferenceJacobian= computeFiniteDifferenceJacobian(prov);
	// compare to A matrix
	//std::cout << "norm(Jac-finiteDiffJac)=" << (computedJacobian- finiteDifferenceJacobian).norm() << std::endl;

	// generate error messages for each entry where the derivatives don't match (wrt to a given tolerance)
	Eigen::MatrixXd difference = (computedJacobian - finiteDifferenceJacobian);
	double tolerance = 10 * dx;
	std::stringstream header;
	header << "Derivative Test results: " << std::endl;
	header << "(obs_idx, Parameter_idx), (expected value (computed via fin diff), actual value), absoluteDifference" << std::endl;
	std::stringstream message;
	bool problemDetected = false;
	for (int par = 0; par < fEvaluator.dimensions.UIndex; par++)
	{
		for (int obs = 0; obs < fEvaluator.dimensions.OIndex; obs++)
		{
			if (fabs(difference(obs, par)) > tolerance)
			{
				problemDetected = true;
				message << "(" << obs << "," << par << "), (" << std::setprecision(9) << finiteDifferenceJacobian(obs, par) << "," << computedJacobian(obs, par) << "), "
						  << fabs(difference(obs, par)) << std::endl;
			}
		}
	}
	if (problemDetected == true)
	{
		std::cout << header.str();
		std::cout << message.str();
	}

	return !problemDetected;
}

Eigen::MatrixXd TLSDerivativeTester::computeFiniteDifferenceJacobian(Eigen::VectorXd vec)
{
	// evaluate misclosure at basepoint
	Eigen::VectorXd miscBase = fEvaluator.evaluateMisclosure(vec);
	// evaluate A matrix at basepoint
	Eigen::MatrixXd ABase = fEvaluator.evaluateA(vec);

	int nParam = fEvaluator.dimensions.UIndex;
	int nObs = fEvaluator.dimensions.OIndex;

	// initialize Jacobian that will be filled with finite differences
	Eigen::MatrixXd finiteDiffJacobian(nObs, nParam);
	finiteDiffJacobian.setZero();
	
	for (int i = 0; i < nParam; i++)
	{
		Eigen::VectorXd jacCol(nObs);
		jacCol.setZero();
		// go slightly in e_i direction
		Eigen::VectorXd pertVect = vec;
		pertVect(i) += dx;
		// evaluate
		Eigen::VectorXd miscPert = fEvaluator.evaluateMisclosure(pertVect);
		// compute he finite diff Jacobian
		jacCol = (miscPert - miscBase) / dx;
		// write it ibn the finite diff Jacobian
		finiteDiffJacobian.col(i) = jacCol;
	}

	return finiteDiffJacobian;
}
