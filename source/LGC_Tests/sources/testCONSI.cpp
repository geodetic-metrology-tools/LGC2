#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testCONSI.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

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
}

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
		ensure_equals("Consistency check activation succesful", projTest->getConfig().consCheck.isActive(), true);

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
		ensure_equals("Consistency check activation succesful", projTest->getConfig().consCheck.isActive(), true);

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
		ensure_equals("Problematic measurement configuration detected successfully", succesCalc.code(), Behavior::BehaviorCode::ERR_consistencyCheck);
	}
};
