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
#include "TLGCApp.h"
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

	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getX() << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getY() << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedValue().getZ() << "\n";
	std::cout << std::setprecision(12) << H4E.getEstimatedHeightInRoot() << "\n";
	std::cout << std::setprecision(12) << "dist with provisional value: " << H4E.getEstimatedValue().dist(H4E.getProvisionalValue()) << "\n";

	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getX() << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getY() << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedValue().getZ() << "\n";
	std::cout << std::setprecision(12) << H4S.getEstimatedHeightInRoot() << "\n";

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
	std::cout << std::setprecision(12) << "dist with provisional value: " << H4E.getEstimatedValue().dist(H4E.getProvisionalValue()) << "\n";

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
	//
	tut::skip();
	set_test_name("Simple TSTN observation");
	std::cout << "\n\n\n\ Simple test LAMBERT" <<std::endl;
	std::string filePath = "C:\\Users\\bweyer\\cernbox\\Documents\\Development\\LGC\\SimpleTest\\";
	TLGCApp proj(filePath + "CUSTOMGEOID_SIMPLE_TSTN_LAMBERT.lgc", filePath + "CUSTOMGEOID_SIMPLE_TSTN_LAMBERT.res");
	ensure_equals("Calculation successful", proj.exec().code(), Behavior::BehaviorCode::ERR_noError);

	TLGCApp projRound(filePath + "CUSTOMGEOID_SIMPLE_TSTN_LAMBERT_round.lgc", filePath + "CUSTOMGEOID_SIMPLE_TSTN_LAMBERT_round.res");
	ensure_equals("Calculation successful", projRound.exec().code(), Behavior::BehaviorCode::ERR_noError);


	std::ifstream infiler(filePath + "CUSTOMGEOID_SIMPLE_TSTN_LAMBERT.lgc");
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);

	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);

}

template<>
template<>

void object::test<9>()
{
	tut::skip();
	set_test_name("Test with North Area Network");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::string filePath = "C:\\Users\\bweyer\\cernbox\\Documents\\Development\\LGC\\TestNorthArea\\test\\";

	TLGCApp proj3(filePath + "..\\260202DEP_33003_Network_VS Magnet_Check_Lambert93.lgc", filePath + "..\\260202DEP_33003_Network_VS Magnet_Check_Lambert93.res");
	ensure_equals("Calculation successful", proj3.exec().code(), Behavior::BehaviorCode::ERR_noError);

	TLGCApp proj2(filePath + "32840_final_insert_modif_simplifiedLamb.lgc", filePath + "32840_final_insert_modif_simplifiedLamb.res");
	ensure_equals("Calculation successful", proj2.exec().code(), Behavior::BehaviorCode::ERR_noError);

	TLGCApp proj(filePath + "32840_final_insert_Lambert93_optimized.lgc", filePath + "32840_final_insert_Lambert93_optimized.res");
	ensure_equals("Calculation successful", proj.exec().code(), Behavior::BehaviorCode::ERR_noError);


	std::ifstream infiler(filePath + "32840_final_insert_Lambert93.lgc");
	ensure_equals("Reading Successful", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior successCalc = calcul.computeResults(fileWriter);



	ensure_equals("Calculation should be done", successCalc.code(), Behavior::BehaviorCode::ERR_noError);
	// Comparison with coordinates computed using production LGC (v2.11)
}

template<>
template<>

void object::test<10>()
{
	tut::skip();
	set_test_name("Test Levelling LHC");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::string filePath = "C:\\Users\\bweyer\\cernbox\\Documents\\Development\\LGC\\TestLHC_DLEV\\";

	TLGCApp proj3(filePath + "32955_no_dhor_Lambert.lgc", filePath + "32955_no_dhor_Lambert.res");
	//TLGCApp proj3(filePath + "32955_Lambert.lgc", filePath + "32955_Lambert.res");
	ensure_equals("Calculation successful", proj3.exec().code(), Behavior::BehaviorCode::ERR_noError);
}
template<>
template<>

void object::test<11>()
{
	set_test_name("Test LHC Gabin dataset");

	projTest->getFileLogger().setOutputfileLocation("C:/Temp/readCustomGeoid.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::string filePath = "C:\\Users\\bweyer\\cernbox\\Documents\\Development\\LGC\\TestLHCGabin\\";

	TLGCApp proj(filePath + "\\gyro\\31326_2025-02_GYRO_LHC-Arc23_Lambert.lgc", filePath + "\\gyro\\31326_2025-02_GYRO_LHC-Arc23_Lambert.res");
	ensure_equals("Calculation successful", proj.exec().code(), Behavior::BehaviorCode::ERR_noError);

	TLGCApp proj3(filePath + "31323-geode_Lambert_RAF.lgc", filePath + "31323-geode_Lambert_RAF.res");
	ensure_equals("Calculation successful", proj3.exec().code(), Behavior::BehaviorCode::ERR_noError);


}

} // namespace tut