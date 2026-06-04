// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "testPOLAR.h"

namespace tut
{
struct test_POLAR
{
	test_POLAR() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

	void runV0Test(const std::string &inputData, const std::string &outputFile, TReal expectedV0Value)
	{
		projTest->getFileLogger().setOutputfileLocation(outputFile);
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(inputData);

		bool successReading = r.read(infiler);
		ensure_equals("Reading Successful", successReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior successCalc = calcul.computeResults(fileWriter);

		ensure_equals("Calculation successful", successCalc.code(), Behavior::BehaviorCode::ERR_noError);

		// Validate V0 result
		TDataTree tree = projTest->getTree();
		TDataTreeIterator frameIt = tree.begin();
		auto romIt = frameIt.node->data->measurements.fTSTN.begin();
		ensure_equals("V0 check", romIt->get()->roms.begin()->get()->v0->getEstimatedValue().getRadiansValue(), expectedV0Value, 1e-12);
	}

	std::shared_ptr<TLGCData> projTest;
	TReader r;
};
typedef test_group<test_POLAR> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("POLAR tests");
}

namespace tut
{

//------------------------------------------ V0  -------------------------------------------------//

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing V0 initialization: case 1 ALLFIXED");
	runV0Test(TestPOLAR::POLAR_V0_INIT_0, "C:/Temp/POLAR_V0_Init_0.txt", 3.1176296599306217);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing V0 initialization: case 1 CALA");
	runV0Test(TestPOLAR::POLAR_V0_INIT_1, "C:/Temp/POLAR_V0_Init_1.txt", 3.1176296599306217);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing V0 initialization: case 1 PLR3D");
	runV0Test(TestPOLAR::POLAR_V0_INIT_2, "C:/Temp/POLAR_V0_Init_2.txt", 3.1176296599306217);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing V0 initialization: case 1 PLR3D and ANGL");
	runV0Test(TestPOLAR::POLAR_V0_INIT_3, "C:/Temp/POLAR_V0_Init_3.txt", 3.1176296599306217);
}

template<>
template<>
void object::test<5>()
{
	set_test_name("Testing V0 initialization: case 2 ALLFIXED");
	runV0Test(TestPOLAR::POLAR_V0_INIT_4, "C:/Temp/POLAR_V0_Init_4.txt", 3.126594665218494);
}

template<>
template<>
void object::test<6>()
{
	set_test_name("Testing ROT3D near gimbal-lock: relies on station-orientation pre-init");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/POLAR_ROT3D_Init.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestPOLAR::POLAR_ROT3D_NEAR_GIMBAL_LOCK);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	ensure_equals("Calculation successful", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure("Sigma0 a posteriori small", projTest->getS0APosteriori() < 1e-3);
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Testing ROT3D with non-PLR3D observations is rejected");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/POLAR_ROT3D_non_PLR3D.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestPOLAR::POLAR_ROT3D_NON_PLR3D);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	ensure_equals("Calculation should be rejected with ERR_inputData", successCalc.code(), Behavior::BehaviorCode::ERR_inputData);
}
} // namespace tut
