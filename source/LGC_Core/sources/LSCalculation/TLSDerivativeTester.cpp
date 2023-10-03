#include <TLSDerivativeTester.h>
#include <Logger.hpp>
#include <iostream>

TLSDerivativeTester::TLSDerivativeTester(std::shared_ptr<TLGCData> data) : fEvaluator( TLSEvaluator(data) )
{
	// test the first design matrix
	bool resultA = testFirstDesignMatrix();
	// test the first constraint design matrix
	bool resultA2 = testFirstConstraintDesignMatrix();
	bool resultWeightedResidual = testWeightedResidualDerivative();
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
	std::cout << "~~~~~~~~~~ Testing first Design Matrix (A) ~~~~~~~~~~~~~~~" << std::endl;
	// get value that is currently set in the TLGCData structure, if called at the beginning, this will be the provisional values
	Eigen::VectorXd prov = fEvaluator.getEstParams();

	// evaluate A matrix according to inputMatrixFiller, contributionGenerator etc..
	//const TSparseMatrix* computedJacobian 
	fEvaluator.setParameters(prov);
	Eigen::SparseMatrix<double> test =  fEvaluator.getA();
	TDenseMatrix test2 = test.toDense();
	TDenseMatrix computedJacobian = fEvaluator.getA().toDense();
	
	// evaluate A matrix according to finite differences applied to the misclosure vector
	Eigen::MatrixXd finiteDifferenceJacobian= computeFiniteDifferenceJacobian(prov, false);
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
				//if ((relDiff > relTol)&&(absDiff>1e-9))
				if (absDiff>1e-2)
				{
					problemDetected = true;
					message << "OIdx = " << std::setw(4) << obs << ", UIdx = " << std::setw(4) << par << ", finDiffJac =" << std::setprecision(8) << std::setw(16)
							<< finiteDifferenceJacobian(obs, par) << ", LGCJac = " << std::setw(16) << computedJacobian(obs, par) << " , absDiff = " << std::setw(12)
							<< absDiff << std::endl; 
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

bool TLSDerivativeTester::testFirstConstraintDesignMatrix()
{
	std::cout << "~~~~~~~~~~ Testing first Constraint Design Matrix (A2) ~~~~~~~~~~~~~~~" << std::endl;
	// get value that is currently set in the TLGCData structure, if called at the beginning, this will be the provisional values
	Eigen::VectorXd prov = fEvaluator.getEstParams();

	// evaluate A matrix according to inputMatrixFiller, contributionGenerator etc..
	//const TSparseMatrix* computedJacobian 
	fEvaluator.setParameters(prov);
	Eigen::SparseMatrix<double> test =  fEvaluator.getA2();
	TDenseMatrix test2 = test.toDense();
	TDenseMatrix computedJacobian = fEvaluator.getA2().toDense();
	
	// evaluate A matrix according to finite differences applied to the misclosure vector
	Eigen::MatrixXd finiteDifferenceJacobian= computeFiniteDifferenceJacobian(prov, true);

	// std::cout << "findiff= " << std::endl << finiteDifferenceJacobian << std::endl;
	// std::cout << "compJac= " << std::endl << computedJacobian << std::endl;

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
		for (int cIdx = 0; cIdx < fEvaluator.dimensions.CIndex; cIdx++)
		{
			double absDiff = fabs(difference(cIdx, par));
		//	if (absDiff > tolerance)
			{
				double absSum = sqrt(pow(computedJacobian(cIdx, par),2) + pow(finiteDifferenceJacobian(cIdx, par),2));
				double relDiff = absDiff / absSum;
				//if ((relDiff > relTol)&&(absDiff>1e-9))
				if (absDiff>1e-2)
				{
					problemDetected = true;
					message << "CIdx = " << std::setw(4) << cIdx << ", UIdx = " << std::setw(4) << par << ", finDiffJac =" << std::setprecision(8) << std::setw(16)
							<< finiteDifferenceJacobian(cIdx, par) << ", LGCJac = " << std::setw(16) << computedJacobian(cIdx, par) << " , absDiff = " << std::setw(12)
							<< absDiff << std::endl; 
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

bool TLSDerivativeTester::testWeightedResidualDerivative()
{
	// weighted residual = sqrt(P_v) * (F(x)-L)
	// computed derivative (as P_v is assumed to be parameter independant):
	// sqrt(P_v)*J_F(x)
	
	// get value that is currently set in the TLGCData structure, if called at the beginning, this will be the provisional values
	Eigen::VectorXd prov = fEvaluator.getEstParams();

	fEvaluator.setParameters(prov);
	Eigen::SparseMatrix<double> computedJacobian = fEvaluator.getSqrtPv() * fEvaluator.getA();

	// compute the finite difference Jacobian
	Eigen::VectorXd baseValue = fEvaluator.getWeightedResidual();
	
	int nParam = fEvaluator.dimensions.UIndex;
	int nObs = fEvaluator.dimensions.OIndex;

	// initialize Jacobian that will be filled with finite differences
	Eigen::MatrixXd finiteDiffJacobian(nObs, nParam);
	finiteDiffJacobian.setZero();
	Eigen::VectorXd baseVector = fEvaluator.getEstParams();
	
	for (int i = 0; i < nParam; i++)
	{
		Eigen::VectorXd jacCol(nObs);
		jacCol.setZero();
		// go slightly in e_i direction
		Eigen::VectorXd pertVect = baseVector;
		pertVect(i) += dx;
		// evaluate
		fEvaluator.setParameters(pertVect);
		Eigen::VectorXd weightedResidualPert = fEvaluator.getWeightedResidual();
		// compute he finite diff Jacobian
		jacCol = (weightedResidualPert - baseValue) / dx;
		// write it ibn the finite diff Jacobian
		finiteDiffJacobian.col(i) = jacCol;
	}

	std::cout << "checking weighted residual (sqrt(Pv(x))*res(x)) derivative: |J_finDiff-J_computed| = " << (computedJacobian.toDense() - finiteDiffJacobian).norm() << std::endl;


	return false;
}

Eigen::MatrixXd TLSDerivativeTester::computeFiniteDifferenceJacobian(Eigen::VectorXd vec, bool Cnstr)
{
	// evaluate misclosure at basepoint
	fEvaluator.setParameters(vec);
	Eigen::VectorXd miscBase;
	if (Cnstr)
	{
		miscBase = fEvaluator.getConstraintMisclosure();
	}
	else
	{
		miscBase = fEvaluator.getMisclosure();
	}
	// evaluate A matrix at basepoint
	Eigen::MatrixXd ABase;
	if (Cnstr)
	{
		ABase = fEvaluator.getA2().toDense();
	}
	else
	{
		ABase = fEvaluator.getA().toDense();
	}

	int nParam = fEvaluator.dimensions.UIndex;
	int nObs = fEvaluator.dimensions.OIndex;
	int nCnstr = fEvaluator.dimensions.CIndex;

	// initialize Jacobian that will be filled with finite differences
	int rowDim;
	if (Cnstr)
	{
		rowDim = nCnstr;
	}
	else
	{
		rowDim = nObs;
	}
	Eigen::MatrixXd finiteDiffJacobian(rowDim, nParam);
	finiteDiffJacobian.setZero();

	for (int i = 0; i < nParam; i++)
	{
		Eigen::VectorXd jacCol(rowDim);
		jacCol.setZero();
		// go slightly in e_i direction
		Eigen::VectorXd pertVect = vec;
		pertVect(i) += dx;
		// evaluate
		fEvaluator.setParameters(pertVect);
		Eigen::VectorXd miscPert = fEvaluator.getMisclosure();
		if (Cnstr)
		{
			miscPert = fEvaluator.getConstraintMisclosure();
		}
		else
		{
			miscPert = fEvaluator.getMisclosure();
		}
		// compute he finite diff Jacobian
		jacCol = (miscPert - miscBase) / dx;
		// write it ibn the finite diff Jacobian
		finiteDiffJacobian.col(i) = jacCol;
	}

	return finiteDiffJacobian;
}
