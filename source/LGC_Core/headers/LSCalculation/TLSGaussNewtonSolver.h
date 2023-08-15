/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_GAUSSNEWTONSOLVER
#define TLS_GAUSSNEWTONSOLVER


//LGC
#include <TLSEvaluator.h>

#include <Eigen/Dense>


/*!
\ingroup GaussNewtonSolver
\brief Gauss Newton method
*/
class TLSGaussNewtonSolver
{
public:
	TLSGaussNewtonSolver(std::shared_ptr<TLSEvaluator> evaluator);
	//~TLSGaussNewtonSolver();
	// apply GN solver
	Eigen::VectorXd solve();

private:
	std::shared_ptr<TLSEvaluator> fEvaluator;
	Eigen::VectorXd getGNDirection(Eigen::VectorXd);
	Eigen::VectorXd getGradient(Eigen::VectorXd);
	// compute armijo stepsize via backtracking
	double backtrackingArmijoStepsize(double sigma0 , Eigen::VectorXd x0, Eigen::VectorXd direction);


};



#endif
