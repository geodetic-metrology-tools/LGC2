#pragma once
#include <TLSEvaluator.h>
#include <TLSDerivativeTester.h>
#include <TLSGaussNewtonSolver.h>
#include <Logger.hpp>
#include <TLSGraph.h>

class TLSBlockDiagonalSolver {
public:
	TLSBlockDiagonalSolver(std::shared_ptr<TLSEvaluator> evaluator);
	void solve();

private:
	std::shared_ptr<TLSEvaluator> fEvaluator;
	void prepareDecomposition();
};


