#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
#include <TLGCData.h>
#include <TReader.h>

#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSConsistencyCheck.h"
#include "TLSInputMatrices.h"
#include "TLSInputMatricesFiller.h"
#include "testPointGroupConstraints.h"
#include "LGCPointConstraintGroup.h"

namespace tut
{
struct test_pointGroupConstraints
{
	test_pointGroupConstraints() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
	std::shared_ptr<TLGCData> projTest;
	TReader reader;
};
typedef test_group<test_pointGroupConstraints> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test point group constraints");
}

namespace tut
{

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing point group constraint definition");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(pointConstraintTest::group_definition);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLSInputMatricesFiller matrFiller(&projTest->getTree(), projTest->getConfig().referential, *projTest.get());
	TLSInputMatrices im;
	TDataAnalyzer analyzer(*projTest);
	analyzer.dataConsistent();

	// create a group of points with constraint
	std::set<std::string> affectedPoints = {"p0", "p3"};
	LGCPointConstraintGroup testGroup(*projTest);
	testGroup.setAffectedPoints(affectedPoints);
	constraintSignature signature{1, 0, 0, 0, 0, 0, 0};
	testGroup.setConstraintSignature(signature);
	std::list<LGCPointConstraintGroup> &pointGroups = projTest->getPointGroups();
	pointGroups.push_back(testGroup);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

}

}; // namespace tut
