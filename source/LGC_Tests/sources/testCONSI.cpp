// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include "testCONSI.h"

namespace tut
{
struct test_CONSI
{
	test_CONSI() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
	std::shared_ptr<TLGCData> projTest;
	TReader reader;
};
typedef test_group<test_CONSI> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test consistency checker");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	set_test_name("Testing CONSI keyword recognition");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/CONSI_keyword.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(Consi::CONSI_keyword_active);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	ensure_equals("Consistency check activation successful", projTest->getConfig().consCheck.isActive(), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing a well defined configuration");
	TReader reader(projTest);
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/CONSI_good_config.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(Consi::CONSI_good_config);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	ensure_equals("Consistency check activation successful", projTest->getConfig().consCheck.isActive(), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing a bad configuration with unidentifiable objects");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/CONSI_bad_config.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(Consi::CONSI_bad_config);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Configuration should be identified as problematic.", succesCalc.code(), Behavior::BehaviorCode::ERR_inputData);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing neighbor computation based on sparsity pattern");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/CONSI_neighbor.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(Consi::CONSI_ConnectedComponentsTest);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);

	TLSInputMatricesFiller matrFiller(&projTest->getTree(), projTest->getConfig().referential, *projTest.get());
	TLSInputMatrices im;
	TDataAnalyzer analyzer(*projTest);
	analyzer.dataConsistent();
	im.initMatrices(projTest->fUEOIndices);
	bool fillSuccess = matrFiller.fillMatrices(projTest.get(), &im);
	TLSConsCheck consCheck(*projTest.get(), im);

	ensure_equals("Configuration should be identified as problematic", consCheck.getResultStatus(), false);

	// Test if all three objects are recognized as connected group.
	// This can only be seen using the sparsity pattern because the derivative of all measurements with respect to the z coordinate of P2 is 0 (evaluated at the provisional values)
	std::set<int> realGroup({0, 1, 2});
	std::set<int> firstConnectedGroup = consCheck.getConnectedNullspaceGroup(0);
	bool allGood = (realGroup == firstConnectedGroup);
	ensure_equals("Connections between objects not computed correct. ", true, allGood);
}

}; // namespace tut
