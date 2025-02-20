#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
// #include <TLGCData.h>
// #include <TReader.h>

#include "TLSEvaluator.h"
#include "TLSInputMatrices.h"
#include "testEvaluator.h"
// testing on some input files
#include "testMixingObservation.h"

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
}

}; // namespace tut
