#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
// #include <TLGCData.h>
// #include <TReader.h>

#include "TLSEvaluator.h"
#include "TLSGaussNewton.h"
#include "TLSInputMatrices.h"
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
	Eigen::VectorXd testVect = myEvaluator.getEstParams();
	bool succes = myEvaluator.testParameterSetAndGet();
	ensure_equals("Problem with setter/getter of evaluator object.", succes, true);

	myEvaluator.testEvaluate();

}
template<>
template<>
void object::test<2>()
{
	set_test_name("Testing GNSolver");
	//std::stringstream infiler(MixObs::TSTN_FrameContribTest);
	std::stringstream infiler(pointWithSigma::computation1);
	TLSEvaluator myEvaluator(infiler);
	TLSGaussNewton mySolver(std::make_shared<TLSEvaluator>(myEvaluator));
	Eigen::VectorXd testVect = myEvaluator.getEstParams();

	mySolver.solve(testVect);

}

}; // namespace tut
