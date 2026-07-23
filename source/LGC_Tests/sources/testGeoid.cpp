// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testGeoid.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

namespace tut
{
    struct test_CustomGeoid{};
    typedef test_group<test_CustomGeoid> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Custom Geoid tests");
}

namespace tut
{	
template<>
template<>
void object::test<1>()
{
	// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system, 4 iterations

	std::shared_ptr<TLGCData> projTest(new TLGCData);

	set_test_name("Testing H to Z transformation (RS2K)");
	TReader r(projTest);
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputGeoidRS2K.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestROOT::transform_h2z_rs2k);
	r.read(infiler);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const TLGCData &dataset = calcul.getData();

	TPositionVector PT = dataset.getPoints().getObject("P0").getEstimatedValue();
	ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2000.00000, 1e-5);
	ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 2097.79265, 1e-5);
	ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 2433.66000, 1e-5);

	TPositionVector Test1 = dataset.getPoints().getObject("Test1").getEstimatedValue();
	ensure_equals("Pt x coordinate should match", Test1.getX().getMetresValue(), 63.34159, 1e-5);
	ensure_equals("Pt y coordinate should match", Test1.getY().getMetresValue(), 5271.95242, 1e-5);
	ensure_equals("Pt z coordinate should match", Test1.getZ().getMetresValue(), 2452.49768, 1e-5);

	TPositionVector TestXYZ = dataset.getPoints().getObject("TestXYZ").getEstimatedValue();
	ensure_equals("Pt x coordinate should match", TestXYZ.getX().getMetresValue(), 759.11954, 1e-5);
	ensure_equals("Pt y coordinate should match", TestXYZ.getY().getMetresValue(), 5300.00270, 1e-5);
	ensure_equals("Pt z coordinate should match", TestXYZ.getZ().getMetresValue(), 2448.94777, 1e-5);
}

template<>
template<>
void object::test<2>()
{
	// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system, 4 iterations

	std::shared_ptr<TLGCData> projTest(new TLGCData);

	set_test_name("Testing H to Z transformation (Custom geoid)");
	TReader r(projTest);
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestROOT::transform_h2z_customG);
	r.read(infiler);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const TLGCData &dataset = calcul.getData();

	TPositionVector PT = dataset.getPoints().getObject("P0").getEstimatedValue();
	ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2000.00000, 1e-5);
	ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 2097.79265, 1e-5);
	ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 2433.66000, 1e-5);

	TPositionVector Test1 = dataset.getPoints().getObject("Test1").getEstimatedValue();
	ensure_equals("Pt x coordinate should match", Test1.getX().getMetresValue(), 63.34159, 1e-5);
	ensure_equals("Pt y coordinate should match", Test1.getY().getMetresValue(), 5271.95242, 1e-5);
	ensure_equals("Pt z coordinate should match", Test1.getZ().getMetresValue(), 2452.49768, 1e-5);

	TPositionVector TestXYZ = dataset.getPoints().getObject("TestXYZ").getEstimatedValue();
	ensure_equals("Pt x coordinate should match", TestXYZ.getX().getMetresValue(), 759.11954, 1e-5);
	ensure_equals("Pt y coordinate should match", TestXYZ.getY().getMetresValue(), 5300.00270, 1e-5);
	ensure_equals("Pt z coordinate should match", TestXYZ.getZ().getMetresValue(), 2448.94777, 1e-5);
}
}
