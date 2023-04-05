#include <TLSDerivativeTester.h>
#include <Logger.hpp>
#include <iostream>

TLSDerivativeTester::TLSDerivativeTester(std::shared_ptr<TLGCData> data) : fEvaluator( TLSEvaluator(data) )
{
	// test the first design matrix
	bool result = testFirstDesignMatrix();
	// if (result)
	// {
	// 	std::cout << "Finite differences test for first design matrix passed." << std::endl;
	// }
	// else
	// {
	// 	std::cout << "Finite differences test for first design matrix failed." << std::endl;
	// }
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

	// reset the parameters
	fEvaluator.setParameters(prov);
	// generate error messages for each entry where the derivatives don't match (wrt to a given tolerance)
	Eigen::MatrixXd difference = (computedJacobian - finiteDifferenceJacobian);
	//double tolerance = 100 * dx;
	// only consider errors above certain relative tolerance
	double relTol = 0.5;
	std::stringstream message;
	bool problemDetected = false;
	for (int par = 0; par < fEvaluator.dimensions.UIndex; par++)
	{
		for (int obs = 0; obs < fEvaluator.dimensions.OIndex; obs++)
		{
			double absDiff = fabs(difference(obs, par));
		//	if (absDiff > tolerance)
			{
				double absSum = sqrt(pow(computedJacobian(obs, par),2) + pow(finiteDifferenceJacobian(obs, par),2));
				double relDiff = absDiff / absSum;
				if ((relDiff > relTol)&&(absDiff>1e-9))
				{
					problemDetected = true;
					message << "OIdx = " << std::setw(4) << obs << ", UIdx = " << std::setw(4) << par << ", finDiffJac =" << std::setprecision(8) << std::setw(16)
							<< finiteDifferenceJacobian(obs, par) << ", LGCJac = " << std::setw(16) << computedJacobian(obs, par) << " , absDiff = " << std::setw(12)
							<< absDiff << ", relDiff = " << std::setw(10) << relDiff << std::endl;
				}
			}
		}
	}
	if (problemDetected == true)
	{
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
