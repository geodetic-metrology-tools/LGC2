// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "tests_2_TSTN_OLOC.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

namespace tut
{
    struct test_complex{};
    typedef test_group<test_complex> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Testing input files with 2 total stations (TSTN)");
}

namespace tut
{	
	// Test Ang Zen & Dist, with z in the same plane
	template<>
	template<>
	void object::test<1>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing param case in ROOT, setup4");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		std::stringstream infiler(TestROOT::Param_setup4);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 0.0  , 1e-8);


        TReal ST1_V0 = dataset.getAngles().begin()->getEstimatedValue().getGonsValue();
		ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 390, 1e-8);

        TReal ST2_V0 = (++dataset.getAngles().begin())->getEstimatedValue().getGonsValue();
		ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);
	}

	// Test Plr3D, with z in the same plane
	template<>
	template<>
	void object::test<2>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing plr3D case in ROOT, setup4");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		std::stringstream infiler(TestROOT::PLR3D_setup4);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 0.0  , 1e-8);


        TReal ST1_V0 = dataset.getAngles().begin()->getEstimatedValue().getGonsValue();
        ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 200, 1e-8);

        TReal ST2_V0 = (++dataset.getAngles().begin())->getEstimatedValue().getGonsValue();
        ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);

	}


	// Test Ang Zen & Dist, with z in a different plane
	template<>
	template<>
	void object::test<3>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing param case in ROOT, setup4");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		std::stringstream infiler(TestROOT::Param_setup4_different_z);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);


		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 1.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 1.0  , 1e-8);


        TReal ST1_V0 = dataset.getAngles().begin()->getEstimatedValue().getGonsValue();
		ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 390, 1e-8);

        TReal ST2_V0 = (++dataset.getAngles().begin())->getEstimatedValue().getGonsValue();
		ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);
	}

	// Test Plr3D, with z in a different plane
	template<>
	template<>
	void object::test<4>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing plr3D case in ROOT, setup4");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		std::stringstream infiler(TestROOT::PLR3D_setup4_different_z);
		r.read(infiler);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 1.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 1.0  , 1e-8);


        TReal ST1_V0 = dataset.getAngles().begin()->getEstimatedValue().getGonsValue();
        ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 390, 1e-8);

        TReal ST2_V0 = (++dataset.getAngles().begin())->getEstimatedValue().getGonsValue();
        ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);
	}

}
