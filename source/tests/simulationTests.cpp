#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include "Utils.h"
#include <TLGCData.h>
#include <readers/TReader.h>
#include "TLGCCalculation.h"
#include <TLGCApp.h>

namespace tut
{
    struct test_simulation{};
    typedef test_group<test_simulation> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Testing SIMU option");
}

namespace tut
{	
	/*Only made for checking the output file, unit tests to be added*/
	template<>
	template<>
	void object::test<1>()
	{
		
		set_test_name("Simu test 1");
		TLGCApp proj(getCurrentDirectory()+ slash + "test_files" + slash + "SimulationTests" + slash + "simu2.inp", 
					 getCurrentDirectory()+ slash + "test_files" + slash + "SimulationTests" + slash + "simu2.out");
			
		proj.exec();

		// Tests to come - check result files
		// ... 
	}

	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Simu test 2 frame");
		TLGCApp proj(getCurrentDirectory()+ slash + "test_files" + slash + "SimulationTests" + slash + "simu2Frame.inp",
					 getCurrentDirectory()+ slash + "test_files" + slash + "SimulationTests" + slash + "simu2Frame.out");
			
		proj.exec();

		// Tests to come - check result files
		// ... 
	}
}
