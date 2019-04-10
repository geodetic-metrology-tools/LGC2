#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>

#include <TReader.h>

namespace tut
{
    test_runner_singleton runner;
}

int main(int argc, char** argv)
{
    tut::reporter reporter;
    tut::runner.get().set_callback(&reporter);

	try {
		if (argc == 2) {
			tut::runner.get().run_tests(argv[1]);
		}
		else {
			tut::runner.get().run_tests();
		}
	} catch (std::exception& e) {
		std::cout << "Test error: " << e.what() << std::endl;
	}

    return !reporter.all_ok();
}
