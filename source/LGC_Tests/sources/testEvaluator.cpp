// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>

#include "TLSEvaluator.h"
#include "TLSTrustRegionLM.h"
#include "TLSInputMatrices.h"
#include "TLSTrustRegionLM.h"
#include "testEvaluator.h"
// testing on some input files
#include "testMixingObservation.h"
#include "testPointWithSigma.h"

namespace tut
{
struct test_Evaluator
{
	// nothing
};
typedef test_group<test_Evaluator> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test Evaluator");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	set_test_name("Testing Evaluator");
	std::stringstream infiler(MixObs::TSTN_FrameContribTest);
	TLSEvaluator myEvaluator(infiler);
	Eigen::VectorXd provVal = myEvaluator.getEstParams();
	bool succes = myEvaluator.testParameterSetAndGet();
	ensure_equals("Problem with setter/getter of evaluator object.", succes, true);

	// test consistency of evaluator (with its derivatives)
	myEvaluator.setParameters(provVal);
	bool evalSuccess = myEvaluator.evaluate();
	ensure_equals("evaluate() method should succeed.", evalSuccess, true);
	Eigen::MatrixXd testJac = myEvaluator.getAMatrix();
	Eigen::MatrixXd finDiffJac = myEvaluator.getFiniteDifferenceA();
	Eigen::MatrixXd difference = testJac - finDiffJac;
	ensure("finite difference Jacobian and symbolic Jacobian should roughly be equal.", difference.norm() < 1e-5);
}
template<>
template<>
void object::test<2>()
{
	set_test_name("Testing Levenberg Marquardt Solver");
	std::stringstream infiler(pointWithSigma::computation1);
	std::shared_ptr<TLSEvaluator> myEvaluator = std::make_shared<TLSEvaluator>(infiler);
	TLSTrustRegionLM::Config defaultConfig;
	TLSTrustRegionLM mySolver(myEvaluator, defaultConfig);
	Eigen::VectorXd provVal = myEvaluator->getEstParams();
	TLSTrustRegionLM::Result result = mySolver.solve(provVal);
	// also solve with LGC
	myEvaluator->setParameters(provVal);
	TVector lgcSol;
	bool lgcSuccess = myEvaluator->tryLGCSolve(lgcSol);

	ensure_equals("Lgc solve method should have succeeded.", lgcSuccess, true);
	ensure("LGC solution and regularized Gauss Newton solution should be equal.", (lgcSol - result.sol).norm() < 1e-8);

}
template<>
template<>
void object::test<3>()
{
	set_test_name("Testing Evaluator over every adjustable object type");
	// Evaluator::allAdjustableTypes carries at least one free parameter of every adjustable
	// object type, see the comments inside the input itself.
	std::stringstream infiler(Evaluator::allAdjustableTypes);

	TLSEvaluator myEvaluator(infiler);
	const TLGCData &data = myEvaluator.getData();

	// Guard the coverage of the test: without a free parameter of a given type the round
	// trip below would silently stop testing that type.
	ensure("Test must hold a free point.", data.getPoints().numUnknowns() > 0);
	ensure("Test must hold a free angle.", data.getAngles().numUnknowns() > 0);
	ensure("Test must hold a free length.", data.getLength().numUnknowns() > 0);
	ensure("Test must hold a free plane.", data.getPlanes().numUnknowns() > 0);
	ensure("Test must hold a free sag element.", data.getSags().numUnknowns() > 0);
	int frameUnknowns = 0;
	for (auto it(data.getTree().begin()); it != data.getTree().end(); ++it)
	{
		const auto &trafo(it.node->data.get()->frame);
		if (!trafo.isFixed())
			frameUnknowns += trafo.getNumUnkn();
	}
	ensure_equals("Test must hold a frame with all 7 helmert parameters free.", frameUnknowns, 7);

	// Every unknown index must survive setParameters() followed by getEstParams().
	ensure_equals("Problem with setter/getter of evaluator object.", myEvaluator.testParameterSetAndGet(), true);
}

}; // namespace tut
