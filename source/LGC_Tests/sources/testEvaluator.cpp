#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
// #include <TLGCData.h>
// #include <TReader.h>

#include "TLSEvaluator.h"
<<<<<<< HEAD
=======
#include "TLSGaussNewton.h"
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40
#include "TLSInputMatrices.h"
#include "testEvaluator.h"
// testing on some input files
#include "testMixingObservation.h"
<<<<<<< HEAD
=======
#include "testPointWithSigma.h"
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40

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
	Eigen::VectorXd testVect = myEvaluator.getEstParams();
	bool succes = myEvaluator.testParameterSetAndGet();
	ensure_equals("Problem with setter/getter of evaluator object.", succes, true);

	myEvaluator.testEvaluate();

}
<<<<<<< HEAD
=======
template<>
template<>
void object::test<2>()
{
	set_test_name("Testing GNSolver");
	//std::stringstream infiler(MixObs::TSTN_FrameContribTest);
	std::stringstream infiler(pointWithSigma::computation1);
	TLSEvaluator myEvaluator(infiler);
	TLSGaussNewton mySolver(std::make_shared<TLSEvaluator>(myEvaluator));
	Eigen::VectorXd provVal = myEvaluator.getEstParams();
	GNResult result = mySolver.solve(provVal);
	// also solve with LGC
	myEvaluator.setParameters(provVal);
	TVector lgcSol;
	bool lgcSuccess = myEvaluator.tryLGCSolve(lgcSol);

	ensure_equals("Lgc solve method should have succeeded.", lgcSuccess, true);
	ensure_equals("Regularized Gauss Newton solve method should have succeeded.", result.success, true);
	ensure("LGC solution and regularized Gauss Newton solution should be equal.", (lgcSol - result.solution).norm() < 1e-8);

	// 

}
>>>>>>> 46fadf621b75dbbcbf43a2f4b38160e8acfdad40

}; // namespace tut
