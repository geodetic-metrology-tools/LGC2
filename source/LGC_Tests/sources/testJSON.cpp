#include <tut/tut.hpp>

#include <Behavior.h>
#include <TLGCData.h>
#include <TReader.h>

#include "TLGCCalculation.h"

namespace tut
{
struct test_JSON
{
	test_JSON() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
	std::shared_ptr<TLGCData> projTest;
	TReader reader;
	std::string JSON_input_header = "*TITR\nTesting JSON keyword\n*OLOC\n";
	std::string JSON_input_rest = R"(*INSTR
*POLAR TS1 T1 0.0 0.1 0.1 0
T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0
*CALA
STN 100 100 0
REF  0 0 0
*POIN
PT  100 1000.5 50  
*TSTN STN TS1 IHFIX
*V0 ACST 200.0
*PLR3D
REF  50 100  141.421356237 
PT  100 100 100 
*END
)";
};
typedef test_group<test_JSON> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test JSON generation");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	set_test_name("Testing JSON keyword recognition");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/JSON_keyword.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(JSON_input_header + "*JSON\n" + JSON_input_rest);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	ensure_equals("JSON check activation succesful", projTest->getConfig().json.isActive(), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing JSON keyword absence");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/JSON_keyword.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(JSON_input_header + JSON_input_rest);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	ensure_equals("JSON check activation failed succesfully", projTest->getConfig().json.isActive(), false);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
}

}; // namespace tut
