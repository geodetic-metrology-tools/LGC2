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
	int nParam = fEvaluator.dimensions.UIndex;
	int nObs = fEvaluator.dimensions.OIndex;

	// evaluate misclosure at basepoint
	Eigen::VectorXd miscBase = fEvaluator.evaluateMisclosure(prov);
	// evaluate A matrix at basepoint
	Eigen::MatrixXd ABase = fEvaluator.evaluateA(prov);


	// initialize Jacobian that will be filled with finite differences
	Eigen::MatrixXd finiteDiffJacobian(nObs, nParam);
	finiteDiffJacobian.setZero();
	double smallNumber = 1e-6;
	
	for (int i = 0; i < nParam; i++)
	{
		Eigen::VectorXd jacCol(nObs);
		jacCol.setZero();
		// go slightly in e_i direction
		Eigen::VectorXd pertVect = prov;
		pertVect(i) += smallNumber;
		// evaluate
		Eigen::VectorXd miscPert = fEvaluator.evaluateMisclosure(pertVect);
		// compute he finite diff Jacobian
		jacCol = (miscPert - miscBase) / smallNumber;
		// write it ibn the finite diff Jacobian
		finiteDiffJacobian.col(i) = jacCol;
	}

	// compare to A matrix
	std::cout << "norm(Jac-finiteDiffJac)=" << (ABase - finiteDiffJacobian).norm() << std::endl;

	return false;

}
