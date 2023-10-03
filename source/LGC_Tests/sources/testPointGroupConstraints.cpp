#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
#include <TLGCData.h>
#include <TReader.h>

#include "LGCPointConstraintGroup.h"
#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "TLSConsistencyCheck.h"
#include "TLSInputMatrices.h"
#include "TLSInputMatricesFiller.h"
#include "testPointGroupConstraints.h"

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
	set_test_name("Testing point group constraint definition 1");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(pointConstraintTest::testfile1);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLSInputMatricesFiller matrFiller(&projTest->getTree(), projTest->getConfig().referential, *projTest.get());
	TLSInputMatrices im;

	// create a group of points with constraint
	std::set<std::string> affectedPoints = {"p0", "p3"};
	LGCPointConstraintGroup testGroup(*projTest);
	testGroup.setAffectedPoints(affectedPoints);
	constraintSignature signature{1, 0, 0, 0, 0, 0, 0};
	// no result with scale constraint
	// constraintSignature signature{1, 1, 1, 1, 1, 1, 1};
	testGroup.setConstraintSignature(signature);
	std::list<LGCPointConstraintGroup> &pointGroups = projTest->getPointGroups();
	pointGroups.push_back(testGroup);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing point group constraint definition 2");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	// more complicated testfile, with 7 DOF frame with free point inside frame
	std::stringstream infiler(pointConstraintTest::testfile2);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLSInputMatricesFiller matrFiller(&projTest->getTree(), projTest->getConfig().referential, *projTest.get());
	TLSInputMatrices im;

	// create a group of points with constraint
	std::set<std::string> affectedPoints = {"p0", "p1", "p2", "p3", "p4"};
	LGCPointConstraintGroup testGroup(*projTest);
	testGroup.setAffectedPoints(affectedPoints);
	constraintSignature signature{1, 0, 0, 0, 0, 0, 0};
	// constraintSignature signature{1, 1, 1, 1, 1, 1, 0};
	testGroup.setConstraintSignature(signature);
	std::list<LGCPointConstraintGroup> &pointGroups = projTest->getPointGroups();
	pointGroups.push_back(testGroup);

	// TLGCCalculation calcul(projTest);
	// std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	// Behavior succesCalc = calcul.computeResults(fileWriter);
	// ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
}
template<>
template<>
void object::test<3>()
{
	set_test_name("Testing rotation constraint");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	// more complicated testfile, with 7 DOF frame with free point inside frame
	std::stringstream infiler(pointConstraintTest::rotation_testfile);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLSInputMatricesFiller matrFiller(&projTest->getTree(), projTest->getConfig().referential, *projTest.get());
	TLSInputMatrices im;

	// create a group of points with constraint
	std::set<std::string> affectedPoints = {"px", "py", "pz"};
	LGCPointConstraintGroup testGroup(*projTest);
	testGroup.setAffectedPoints(affectedPoints);
	// blocking x momentum
	constraintSignature signature{0, 0, 0, 1, 0, 0, 0};
	testGroup.setConstraintSignature(signature);
	std::list<LGCPointConstraintGroup> &pointGroups = projTest->getPointGroups();
	pointGroups.push_back(testGroup);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const TLGCData &dataset = calcul.getData();
	TAdjustableHelmertTransformation frame = dataset.locateNode("rotatingFrame").node->data.get()->frame;

	ensure_equals("rx should not have changed", frame.getEstRotation(0), frame.getProvRotation(0), 1e-6);

	// TPositionVector px_orig = dataset.getPoints().getObject("px").getProvisionalValue();
	// TPositionVector py_orig = dataset.getPoints().getObject("py").getProvisionalValue();
	// TPositionVector pz_orig = dataset.getPoints().getObject("pz").getProvisionalValue();
	// TPositionVector px_afterComp = dataset.getPoints().getObject("px").getEstimatedValue();
	// TPositionVector py_afterComp = dataset.getPoints().getObject("py").getEstimatedValue();
	// TPositionVector pz_afterComp = dataset.getPoints().getObject("pz").getEstimatedValue();
	// ensure_equals("px should not have changed", (px_orig - px_afterComp).length(), 0, 1e-9);
	// ensure_equals("py should not have changed", (py_orig - py_afterComp).length(), 0, 1e-9);
	// ensure_equals("pz should not have changed", (pz_orig - pz_afterComp).length(), 0, 1e-9);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing translation constraint");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	// more complicated testfile, with 7 DOF frame with free point inside frame
	std::stringstream infiler(pointConstraintTest::translation_testfile);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLSInputMatricesFiller matrFiller(&projTest->getTree(), projTest->getConfig().referential, *projTest.get());
	TLSInputMatrices im;

	// create a group of points with constraint
	std::set<std::string> affectedPoints = {"px", "py", "pz"};
	LGCPointConstraintGroup testGroup(*projTest);
	testGroup.setAffectedPoints(affectedPoints);
	// blocking x cog
	constraintSignature signature{1, 0, 0, 0, 0, 0, 0};
	testGroup.setConstraintSignature(signature);
	std::list<LGCPointConstraintGroup> &pointGroups = projTest->getPointGroups();
	pointGroups.push_back(testGroup);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();
	TAdjustableHelmertTransformation frame = dataset.locateNode("translatingFrame").node->data.get()->frame;

	ensure_equals("tx should not have changed", frame.getEstTranslation(0), frame.getProvTranslation(0), 1e-6);
}

template<>
template<>
void object::test<5>()
{
	set_test_name("Testing constraint detection");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	// more complicated testfile, with 7 DOF frame with free point inside frame
	std::stringstream infiler(pointConstraintTest::constraintDetection_testfile);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	TLSInputMatricesFiller matrFiller(&projTest->getTree(), projTest->getConfig().referential, *projTest.get());
	TLSInputMatrices im;
	TDataAnalyzer analyzer(*projTest);
	analyzer.dataConsistent();
	im.initMatrices(projTest->fUEOIndices);
	bool fillSuccess = matrFiller.fillMatrices(projTest.get(), true, &im);
	TLSConsCheck consCheck(*projTest.get(), im);
	consCheck.computeNecessaryLIBRConstraints();

}

}; // namespace tut
