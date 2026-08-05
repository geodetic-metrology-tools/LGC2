// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <TLGCData.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "testCustomGeoid.h"

namespace tut
{
struct test_CustomGeoid
{
	test_CustomGeoid() : projTest(std::make_shared<TLGCData>()), r(projTest) {}


	std::shared_ptr<TLGCData> projTest;
	TReader r;
};
typedef test_group<test_CustomGeoid> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test of custom geoid");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	set_test_name("Testing reading RS2K");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readRS2K.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestCUSTOMGEOID::RS2K_PARSE);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	std::cout << projTest->getPoints().getObject("P0").getEstimatedValue().getX() << "\n";
	std::cout << projTest->getPoints().getObject("P0").getEstimatedValue().getY() << "\n";
	std::cout << projTest->getPoints().getObject("P0").getEstimatedValue().getZ() << "\n";
	std::cout << projTest->getPoints().getObject("P0").getEstimatedHeightInRoot() << "\n";

	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing reading CUSTOM Geoid path");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestCUSTOMGEOID::CUSTOMGEOID_PARSE);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);


	std::cout << projTest->getPoints().getObject("P0").getEstimatedValue().getX() << "\n";
	std::cout << projTest->getPoints().getObject("P0").getEstimatedValue().getY() << "\n";
	std::cout << projTest->getPoints().getObject("P0").getEstimatedValue().getZ() << "\n";
	std::cout << projTest->getPoints().getObject("P0").getEstimatedHeightInRoot() << "\n";

	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
}
} // namespace tut