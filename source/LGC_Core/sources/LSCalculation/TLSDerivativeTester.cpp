#include <TLSDerivativeTester.h>
#include <Logger.hpp>

TLSDerivativeTester::TLSDerivativeTester(std::shared_ptr<TLGCData> data) : fEvaluator( TLSEvaluator(data) )
{
	// trigger A matrix test object
	bool passed = false;
	passed = testFirstDesignMatrix();

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
	std::cout << "norm(Jac-finiteDiffJac)=" << (computedJacobian- finiteDifferenceJacobian).norm() << std::endl;

	return false;

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
	double smallNumber = 1e-6;
	
	for (int i = 0; i < nParam; i++)
	{
		Eigen::VectorXd jacCol(nObs);
		jacCol.setZero();
		// go slightly in e_i direction
		Eigen::VectorXd pertVect = vec;
		pertVect(i) += smallNumber;
		// evaluate
		Eigen::VectorXd miscPert = fEvaluator.evaluateMisclosure(pertVect);
		// compute he finite diff Jacobian
		jacCol = (miscPert - miscBase) / smallNumber;
		// write it ibn the finite diff Jacobian
		finiteDiffJacobian.col(i) = jacCol;
	}

	return finiteDiffJacobian;
}
