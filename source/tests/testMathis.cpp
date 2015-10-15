#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <MeasurementReaders.h> 
#include <TXYH2CCS.h>
#include <TLGCApp.h>
#include "TLGCCalculation.h"


namespace tut
{
    struct test_Mathis{};
    typedef test_group<test_Mathis> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test of LGC Mathis file");
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

		//It is working, but takes time, uncomment if you want to see results
	/*	TLGCApp proj( getCurrentDirectory()+"\\MATHISCURRENT\\Mathis\\MathisLGC_V2_20150827-105549.lgc2",
					  getCurrentDirectory()+"\\MATHISCURRENT\\Mathis\\MathisLGC_V2_20150827-105549.out");
		bool processOK = proj.exec();
		ensure (processOK);*/

	}
}