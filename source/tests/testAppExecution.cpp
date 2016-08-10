#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include "Utils.h"
#include <TLGCData.h>
#include <TReader.h>
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
		//Test if the whole application runs
		using namespace LGC;

		set_test_name("Testing TLGCApp class and nam file reader");
		
        //TLGCApp proj( getCurrentDirectory()+ slash + "test_files" + slash + "SimulationTests" + slash + "LIBR_SIMU.lgc2", 
		//			    getCurrentDirectory()+ slash + "test_files" + slash + "SimulationTests" + slash + "LIBR_SIMU.out");
		//ensure_equals( "Execution should work", proj.exec(), true);
	}

	template<>
	template<>
	void object::test<2>()
	{
		//Test if the whole application runs
		using namespace LGC;
		set_test_name("Testing TLGCApp class and nam file reader");

		//TLGCApp proj(getCurrentDirectory() + slash + "test_files" + slash + "Calc" + slash + "testCovar.lgc2",
		//	getCurrentDirectory() + slash + "test_files" + slash + "Calc" + slash + "testCovar.res");
		//
		//ensure_equals("Execution should work", proj.exec(), true);
		
		// re initialise static object allfixed param to false
		//TAdjustablePoint::setAllFixedParam(false);

	}

};
