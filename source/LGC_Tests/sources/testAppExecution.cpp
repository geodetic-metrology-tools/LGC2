// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <tut/tut.hpp>

#include <StringManager.h>
#include <TLGCData.h>
#include <TReader.h>
#include "TLGCApp.h"
#include "testNonTSTNmeas.h"
#include "TLGCCalculation.h"
#include "Utils.h"

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

		TLGCApp proj(getCurrentDirectory() + slash + "test_files" + slash + "OBSXYZ.lgc",
			getCurrentDirectory() + slash + "test_files" + slash + "OBSXYZ.res");

		ensure_equals("Calculation successful", proj.exec().code(), Behavior::BehaviorCode::ERR_noError);

		
		// re initialise static object allfixed param to false
		//TAdjustablePoint::setAllFixedParam(false);
	}

};
