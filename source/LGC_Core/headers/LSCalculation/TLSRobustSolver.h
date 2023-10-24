/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef TLS_ROBUSTSOLVER
#define TLS_ROBUSTSOLVER


//LGC
#include <TLSEvaluator.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>


/*!
\ingroup RobustSolver
\brief L1 and Huber using OSQP 
*/


class TLSRobustSolver //public Serializable
{
public:
	TLSRobustSolver(std::shared_ptr<TLSEvaluator> evaluator);
	//~TLSRobustSolver();
	// apply GN solver
	// Huber Estimator
	void solve();


private:
	std::shared_ptr<TLSEvaluator> fEvaluator;
	Eigen::VectorXd computeStep(Eigen::VectorXd par);
	void setHuberConstraints(Eigen::VectorXd par, Eigen::SparseMatrix<double> &constraintMat, Eigen::VectorXd &lb, Eigen::VectorXd &ub);
	void setHuberObjective(Eigen::VectorXd par, Eigen::SparseMatrix<double> &hessian, Eigen::VectorXd &gradient);

};



#endif
