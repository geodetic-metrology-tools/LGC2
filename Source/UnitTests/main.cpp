#include <tut/tut.hpp>
#include <tut_reporter.h>

#include <FileTester.h>


namespace tut
{
    test_runner_singleton runner;
}

int main()
{
	try {
		FileTester::runTestFiles();
	} catch (std::exception& e) {
		std::cout << "Error in LGC test files: " << e.what();
	}

	return 0;

    tut::reporter reporter;
    tut::runner.get().set_callback(&reporter);

    tut::runner.get().run_tests();

    return !reporter.all_ok();
}
