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
	tut::skip();
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
	tut::skip();
	set_test_name("Testing reading OLOC");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readOLOC.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestCUSTOMGEOID::OLOC_PARSE);
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
void object::test<3>()
{
	tut::skip();
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
	std::cout << projTest->getPoints().getObject("P0").getEstimatedHeightInRoot() << "\n Test XYZ\n";

	std::cout << projTest->getPoints().getObject("TestXYZ").getEstimatedValue().getX() << "\n";
	std::cout << projTest->getPoints().getObject("TestXYZ").getEstimatedValue().getY() << "\n";
	std::cout << projTest->getPoints().getObject("TestXYZ").getEstimatedValue().getZ() << "\n";
	std::cout << projTest->getPoints().getObject("TestXYZ").getEstimatedHeightInRoot() << "\n";

	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<4>()
{
	tut::skip();

	set_test_name("Simple TSTN observation");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	//std::stringstream infiler(TestCUSTOMGEOID::RS2K_TSTN);
	std::stringstream infiler(TestCUSTOMGEOID::CUSTOMGEOID_TSTN);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	auto H4E = projTest->getPoints().getObject("H4.XBPF.22716.E");
	auto H4S = projTest->getPoints().getObject("H4.XBPF.22716.S");

	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getX() << " (" << H4E.getDXValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getY() << " (" << H4E.getDYValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getZ() << " (" << H4E.getDZValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedHeightInRoot() << "\n";
	std::cout << std::setprecision(12) << "dist with provisional value: " << H4E.getEstimatedValue().dist(H4E.getProvisionalValue()) << "\n";

	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getX() << " (" << H4E.getDXValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getY() << " (" << H4E.getDYValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getZ() << " (" << H4E.getDZValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedHeightInRoot() << "\n";
	std::cout << std::setprecision(12) << "dist with provisional value H4S: " << H4S.getEstimatedValue().dist(H4S.getProvisionalValue()) << "\n";


	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<5>()
{
	tut::skip();
	set_test_name("Simple TSTN observation");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestCUSTOMGEOID::RS2K_TSTN);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	auto H4E = projTest->getPoints().getObject("H4.XBPF.22716.E");
	auto H4S = projTest->getPoints().getObject("H4.XBPF.22716.S");

	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getX() << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getY() << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getZ() << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedHeightInRoot() << "\n";
	std::cout << std::setprecision(12) << "dist with provisional value H42: " << H4E.getEstimatedValue().dist(H4E.getProvisionalValue()) << "\n";

	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getX() << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getY() << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getZ() << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedHeightInRoot() << "\n";
	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<6>()
{
	tut::skip();
	set_test_name("Simple TSTN observation");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	// std::stringstream infiler(TestCUSTOMGEOID::RS2K_TSTN);
	std::stringstream infiler(TestCUSTOMGEOID::CUSTOMGEOID_TSTN_2);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	auto H4E = projTest->getPoints().getObject("H4.XBPF.22716.E");
	auto H4S = projTest->getPoints().getObject("H4.XBPF.22716.S");

	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getX() << "(" << H4E.getDXValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getY() << "(" << H4E.getDYValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getZ() << "(" << H4E.getDZValue() << ")" << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedHeightInRoot() << "\n";
	std::cout << std::setprecision(12) << "dist with provisional value: " << H4E.getEstimatedValue().dist(H4E.getProvisionalValue()) << "\n";

	// std::cout << std::setprecision(12) << H4S.getEstimatedValue().getX() << "\n";
	// std::cout << std::setprecision(12) << H4S.getEstimatedValue().getY() << "\n";
	// std::cout << std::setprecision(12) << H4S.getEstimatedValue().getZ() << "\n";
	// std::cout << std::setprecision(12) << H4S.getEstimatedHeightInRoot() << "\n";

	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Simple TSTN observation");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestCUSTOMGEOID::SPHE_SIMPLE_TSTN);
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	auto H4E = projTest->getPoints().getObject("H4.XBPF.22716.E");
	auto H4S = projTest->getPoints().getObject("H4.XBPF.22716.S");
	
	TAReferenceFrame *CGRFs(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCGRFSphere));
	TAReferenceFrame *CCS(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));

	TSpatialPosition h4sSP(CGRFs, H4S.getEstimatedValue());
	TSpatialPosition h4eSP(CGRFs, H4E.getEstimatedValue());

	h4sSP.transform(CCS);
	h4eSP.transform(CCS);

	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
	// Comparison with coordinates computed using production LGC (v2.11)
	ensure_equals("H4S CCS X coordinate", h4sSP.getCoordinates(TCoordSysFactory::k3DCartesian).getX(), 759.239832, 1e-6);
	ensure_equals("H4S CCS Y coordinate", h4sSP.getCoordinates(TCoordSysFactory::k3DCartesian).getY(), 5300.365627, 1e-6);
	ensure_equals("H4S CCS Z coordinate", h4sSP.getCoordinates(TCoordSysFactory::k3DCartesian).getZ(), 2447.945446, 1e-6);
	ensure_equals("H4E CCS X coordinate", h4eSP.getCoordinates(TCoordSysFactory::k3DCartesian).getX(), 759.119636, 1e-6);
	ensure_equals("H4E CCS Y coordinate", h4eSP.getCoordinates(TCoordSysFactory::k3DCartesian).getY(), 5300.002693, 1e-6);
	ensure_equals("H4E CCS Z coordinate", h4eSP.getCoordinates(TCoordSysFactory::k3DCartesian).getZ(), 2448.022985, 1e-6);

}

} // namespace tut