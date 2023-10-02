/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_DERIVATIVETESTER
#define TLS_DERIVATIVETESTER


//LGC
#include <TLSEvaluator.h>

#include <Eigen/Dense>


/*!
\ingroup DerivativeTester
\brief For testing the derivatives (A matrix) using finite differences applied to misclosure evaluations
*/
class TLSDerivativeTester 
{

public:
	TLSDerivativeTester(std::shared_ptr<TLGCData> data);

	bool testFirstDesignMatrix();
	bool testFirstConstraintDesignMatrix();
	bool testWeightedResidualDerivative();

private:
	
	TLSEvaluator fEvaluator;
	
	// with Cnstr=false it will give the first design matrix, with cnstr=true it will retutn the constraint first design matrix
	Eigen::MatrixXd computeFiniteDifferenceJacobian(Eigen::VectorXd vec, bool Cnstr=false);

	// used for finite differences
	double dx = 1e-6;


};

#endif
