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
	//TVector ini(fEvaluator.dimensions.UIndex);
	TVector ini = Eigen::VectorXd::Random(fEvaluator.dimensions.UIndex);
	ini = fEvaluator.getEstParams();

	// fEvaluator.setParameters(ini);
	TVector result(fEvaluator.dimensions.OIndex);

	 Functor<double> myFunctor(fEvaluator);
//	 // finite difference version
//	 myFunctor(ini, result);
//	 //std::cout <<sa result << std::endl;
//
//	 Eigen::NumericalDiff<Functor<double>> numDiff(myFunctor);
//	 Eigen::LevenbergMarquardt<Eigen::NumericalDiff<Functor<double>>, double> lmNumDiff(numDiff);
//	 // reset initial value
//	 TVector sol = ini;
//	 int retNumdiff = lmNumDiff.minimize(sol);
//	 std::cout << "Function evaluations " << lmNumDiff.nfev << std::endl;
//	 std::cout << "Jacobian evaluations " << lmNumDiff.njev << std::endl;
//
//	 std::cout << "fnorm=" << lmNumDiff.fnorm << std::endl;



	// in case we have the Jcobian
	 Eigen::LevenbergMarquardt<Functor<double>> lm(myFunctor);
	 lm.parameters.maxfev = 2000;
	 lm.parameters.xtol = 1.0e-10;
	 //lm.parameters.gtol = 1e-20;

	 Eigen::VectorXd sol = ini;
	 //std::cout << sol << std::endl;
	 int ret = lm.minimize(sol);
	 std::cout << "Function evaluations " << lm.nfev << std::endl;
	 std::cout << "Jacobian evaluations " << lm.njev << std::endl;
	 std::cout << "fnorm=" << lm.fnorm << std::endl;

//	 //test deriv
//	 double testx = 5;
//	 Eigen::VectorXd x(1);
//	 x << testx;
//	 Eigen::VectorXd xeps(1);
//	 double myEps = 1e-6;
//	 xeps << testx + myEps;
//	 Eigen::VectorXd fx(1);
//	 Eigen::VectorXd fxeps(1);
//	 myFunctor(xeps, fxeps);
//	 myFunctor(x, fx);
//	 Eigen::MatrixXd dfDiff = (fxeps - fx) / myEps;
//	 Eigen::MatrixXd df(1, 1);
//	 myFunctor.df(x, df);
//	 std::cout << "findiff="<< dfDiff << std::endl;
//	 std::cout << "jacobi="<< df << std::endl;


	// std::cout << "sol=" << sol << std::endl;



	return sol;
}

