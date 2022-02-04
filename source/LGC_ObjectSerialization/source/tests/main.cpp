#include <tut/tut.hpp>
#include <tut/tut_main.hpp>
#include <tut/tut_reporter.hpp>

namespace tut
{
test_runner_singleton runner;
}

/**
 * Main function.
 *
 * You can run the program with "-h" or "--help" option to know more about it.
 */
int main(int argc, char **argv)
{
	tut::reporter reporter;
	tut::runner.get().set_callback(&reporter);

	try
	{
		tut::tut_main(argc, argv);
	}
	catch (const tut::tut_error &ex)
	{
		std::cerr << "TUT error: " << ex.what() << std::endl;
	}

	return !reporter.all_ok();
}
