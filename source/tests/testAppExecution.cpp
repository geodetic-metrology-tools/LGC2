#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include "Utils.h"
#include <TLGCData.h>
#include <readers/TReader.h>
#include "TLGCApp.h"
#include "testNonTSTNmeas.h"
#include "TLGCCalculation.h"

namespace tut
{
    struct test_Writers{};
    typedef test_group<test_Writers> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test of the application execution");
}

namespace tut
{	
	template<>
	template<>
	void object::test<1>()
	{ 
		//Test if the whole application run
		using namespace LGC;

		// LGCData
		std::shared_ptr<TLGCData> projTest(new TLGCData);(new TLGCData);

		set_test_name("Testing Punch file writer");
		
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/PUNCH_FILE_WRITER_LOG.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::echo_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		
		TLGCCalculation calcul(projTest);
		bool succesCalc = calcul.computeResults(nullptr);

		ensure_equals("Calculation successful", succesCalc, true);
 	}

	template<>
	template<>
	void object::test<2>()
	{
		//Test if the whole application runs
		using namespace LGC;
		set_test_name("Testing TLGCApp class and nam file reader");

		TLGCApp proj( getCurrentDirectory()+ slash + "test_files" + slash + "Calc" + slash + "TSTNTEST.inp", 
					    getCurrentDirectory()+ slash + "test_files" + slash + "Calc" + slash + "TSTNTEST.out");
		ensure_equals( "Execution should work", proj.exec(), true);
	}

	template<>
	template<>
	void object::test<3>()
	{
		//Test if the whole application runs
		using namespace LGC;
		set_test_name("Testing TLGCApp class and nam file reader");

		TLGCApp proj( getCurrentDirectory()+ slash + "test_files" + slash + "Calc" + slash + "DVER.inp", 
					    getCurrentDirectory()+ slash + "test_files" + slash + "Calc" + slash + "DVER.out");
		ensure_equals( "Execution should work", proj.exec(), true);
	}
};