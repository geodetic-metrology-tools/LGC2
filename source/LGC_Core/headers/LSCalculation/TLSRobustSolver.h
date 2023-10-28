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
	void setGamma(double t) { gamma = t; }


private:
	std::shared_ptr<TLSEvaluator> fEvaluator;

	// Huber estimator related
	Eigen::VectorXd computeHuberStep(Eigen::VectorXd par);
	void setHuberObjective(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &hessian, Eigen::VectorXd &gradient);
	// equality constraint block of the form Ax=b
	void setHuberEqualityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &b);
	// inequality constraint block of the form lb=<Ax<=ub
	void setHuberInequalityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &lb, Eigen::VectorXd &ub);
	// combine eq and ineq constraints for osqp
	void setOSQPFormatHuberConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &constraintMat, Eigen::VectorXd &lb, Eigen::VectorXd &ub);

	// Huber threshold (residual vs sigma from where L1 begins)
	double gamma = 1;

	// flag for dumping matrices for debugging in python
	bool dumpMat = true;

	// L1 estimator related
	//Eigen::VectorXd computeL1Step(Eigen::VectorXd par);
	void setL1Objective(Eigen::SparseMatrix<double> &hessian, Eigen::VectorXd &gradient);
	// equality constraint block of the form Ax=b
	void setL1EqualityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &b);
	// inequality constraint block of the form lb=<Ax<=ub
	void setL1InequalityConstraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &block, Eigen::VectorXd &lb, Eigen::VectorXd &ub);
	// combine eq and ineq constraints for osqp
	void setOSQPFormatL1Constraint(const Eigen::VectorXd par, Eigen::SparseMatrix<double> &constraintMat, Eigen::VectorXd &lb, Eigen::VectorXd &ub);


};



#endif
