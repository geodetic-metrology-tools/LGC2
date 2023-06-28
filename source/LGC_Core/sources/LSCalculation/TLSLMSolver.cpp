#include "TLSLMSolver.h"
#include <TLSLMSolver.h>
#include <Logger.hpp>
#include <iostream>

#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>




TLSLMSolver::TLSLMSolver(std::shared_ptr<TLGCData> data) : fEvaluator( TLSEvaluator(data) )
{
}


Eigen::VectorXd TLSLMSolver::solveLM()
{
	struct my_functor : Functor<double>
	{
		my_functor() : Functor<double>(1,1) {}
		int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec) const
		{
			// example function from https://edms.cern.ch/document/2852335/1
			// this example has a extremely small convergence region when solved with LGC's normal full step GN - like [-1.2,2.8]
			fvec(0) = acos(x(0) / pow(1 + x(0) * x(0), 0.5)) - PI / 4;
			// fvec(0) = 10.0 * pow(x(0) + 3.0, 2) + pow(x(1) - 5.0, 2);
			// fvec(1) = 0;

			return 0;
		}
	};

	// test eigen levenberg marquardt
	Eigen::VectorXd x(1);
	x(0) = 500.0;
	std::cout << "x: " << x << std::endl;

	my_functor functor;
	Eigen::NumericalDiff<my_functor> numDiff(functor);
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<my_functor>, double> lm(numDiff);
	lm.parameters.maxfev = 2000;
	lm.parameters.xtol = 1.0e-10;
	//std::cout << lm.parameters.maxfev << std::endl;

	int ret = lm.minimize(x);
	std::cout << lm.iter << std::endl;
	std::cout << ret << std::endl;

	std::cout << "x that minimizes the function: " << x << std::endl;

	return x;
}

