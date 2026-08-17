// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <fstream>
#include <regex>

#include <tut/tut.hpp>
#include <tut/tut_macros.hpp>

#include <CliParser.h>
#include <Defaults.h>
#include <Logger.hpp>
#include <Version.h>

namespace tut
{
struct test_CliParser
{
};
typedef test_group<test_CliParser> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test of CLI option parsing helpers");

// Creates a temp file with dummy content under the system temp dir; caller removes it.
std::string makeTempInputFile(const std::string &filename)
{
	const std::string path = (std::filesystem::temp_directory_path() / filename).string();
	std::filesystem::remove(path);
	std::ofstream(path) << "dummy content\n";
	return path;
}

// RAII guard: clears Logger handlers on entry and exit, so parseCommandLine()'s side
// effects never leak into the next test — even if an ensure() throws in between.
struct ScopedLoggerReset
{
	ScopedLoggerReset() { Logger::getLogger().clearHandlers(); }
	~ScopedLoggerReset() { Logger::getLogger().clearHandlers(); }
};
} // namespace

namespace tut
{
template<>
template<>
void object::test<1>()
{
	set_test_name("optionTakesValue distinguishes value-taking flags from switches");
	ensure("-i takes a value", optionTakesValue("-i"));
	ensure("-d takes an optional value", optionTakesValue("-d"));
	ensure("-h takes no value", !optionTakesValue("-h"));
	ensure("-V takes no value", !optionTakesValue("-V"));
	ensure("unknown option takes no value", !optionTakesValue("-v"));
}

template<>
template<>
void object::test<2>()
{
	set_test_name("buildOptionsHelpText lists every option's description, aligned");
	const std::string text = buildOptionsHelpText();

	size_t firstDescriptionColumn = std::string::npos;
	for (const auto &spec : optionSpecs())
	{
		ensure("description for '" + spec.description + "' is present", text.find(spec.description) != std::string::npos);

		const size_t column = text.find(spec.description) - text.rfind('\n', text.find(spec.description));
		if (firstDescriptionColumn == std::string::npos)
			firstDescriptionColumn = column;
		ensure_equals("all descriptions start at the same column", column, firstDescriptionColumn);
	}
}

template<>
template<>
void object::test<3>()
{
	set_test_name("isHelpRequest/isVersionRequest agree with the Help/Version spellings");
	ensure("-h is a help request", isHelpRequest("-h"));
	ensure("--help is a help request", isHelpRequest("--help"));
	ensure("-V is not a help request", !isHelpRequest("-V"));

	ensure("-V is a version request", isVersionRequest("-V"));
	ensure("--version is a version request", isVersionRequest("--version"));
	ensure("-h is not a version request", !isVersionRequest("-h"));
	ensure("lowercase -v is not a version request", !isVersionRequest("-v"));
}

template<>
template<>
void object::test<4>()
{
	set_test_name("buildUsageLine marks Input as required and everything else optional");
	const std::string usage = buildUsageLine();

	ensure("required -i is not bracketed", usage.find("-i <file>") != std::string::npos);
	ensure("optional -o is bracketed", usage.find("[-o <file>]") != std::string::npos);
	ensure("optional -n is bracketed", usage.find("[-n <int>]") != std::string::npos);
	ensure("optional -d is bracketed", usage.find("[-d [<file>]]") != std::string::npos);
	ensure("optional -V is bracketed", usage.find("[-V]") != std::string::npos);
	ensure("optional -h is bracketed", usage.find("[-h]") != std::string::npos);
}

template<>
template<>
void object::test<5>()
{
	set_test_name("optionValueIsOptional distinguishes -d's optional value from required ones");
	ensure("-d's value is optional", optionValueIsOptional("-d"));
	ensure("-i's value is required", !optionValueIsOptional("-i"));
	ensure("-o's value is required", !optionValueIsOptional("-o"));
	ensure("-n's value is required", !optionValueIsOptional("-n"));
	ensure("-h takes no value, so not optional either", !optionValueIsOptional("-h"));
	ensure("unknown option is not optional", !optionValueIsOptional("-v"));
}

template<>
template<>
void object::test<6>()
{
	set_test_name("getLGCVersion() matches vMAJOR.MINOR.<free> format");
	const std::regex versionFormat(R"(v\d+\.\d+\..+)");

	const std::string version = getLGCVersion();
	ensure("version '" + version + "' matches expected format", std::regex_match(version, versionFormat));

	// Regression: these must NOT match.
	for (const std::string &invalid : {"v.a.b", "v1..2", "v2.1a.0", "v2.1.", "vA.1.0", "v2a.1.0"})
		ensure("'" + invalid + "' must not match", !std::regex_match(invalid, versionFormat));
}

template<>
template<>
void object::test<7>()
{
	set_test_name("checkHelpOrVersion: -h wins over -V regardless of order; no early exit otherwise");

	char prog[] = "LGC";
	char optH[] = "-h";
	char optV[] = "-V";
	char optBogus[] = "--bogus";

	{
		char *argv[] = {prog, optV, optH};
		const auto result = checkHelpOrVersion(3, argv);
		ensure("(-V -h) is a help/version early exit", result.has_value());
		ensure("(-V -h): help wins, message mentions Usage", result->message.find("Usage:") != std::string::npos);
	}
	{
		char *argv[] = {prog, optH, optV};
		const auto result = checkHelpOrVersion(3, argv);
		ensure("(-h -V) is a help/version early exit", result.has_value());
		ensure("(-h -V): help wins, message mentions Usage", result->message.find("Usage:") != std::string::npos);
	}
	{
		char *argv[] = {prog, optBogus};
		const auto result = checkHelpOrVersion(2, argv);
		ensure("no -h/-V present -> no early exit", !result.has_value());
	}
	{
		const auto result = checkHelpOrVersion(0, nullptr);
		ensure("argc == 0 is safe and returns no early exit", !result.has_value());
	}
}

template<>
template<>
void object::test<8>()
{
	set_test_name("validateKnownOptions: dash-prefixed values and -d's optional value are not misflagged");

	char prog[] = "LGC";
	char optI[] = "-i";
	char dashedValue[] = "-myfile.lgc2";
	char optD[] = "-d";
	char optX[] = "-x";
	char optV[] = "-v";

	{
		char *argv[] = {prog, optI, dashedValue};
		const auto result = validateKnownOptions(3, argv);
		ensure("dash-prefixed value after -i is not misflagged", !result.has_value());
	}
	{
		char *argv[] = {prog, optD, optX};
		const auto result = validateKnownOptions(3, argv);
		ensure("-d -x: -x must still be rejected", result.has_value());
		ensure("message names the offending option", result->message.find("-x") != std::string::npos);
	}
	{
		char *argv[] = {prog, optV};
		const auto result = validateKnownOptions(2, argv);
		ensure("-v is rejected with a hint", result.has_value());
		ensure("hint mentions -V", result->message.find("-V") != std::string::npos);
	}
	{
		const auto result = validateKnownOptions(0, nullptr);
		ensure("argc == 0 is safe", !result.has_value());
	}
}

template<>
template<>
void object::test<9>()
{
	set_test_name("parseCommandLine resolves CliSettings on success, given a real input file");

	const std::string tmpInput = makeTempInputFile("lgc_test_parseCommandLine_input.lgc2");

	char prog[] = "LGC";
	std::vector<char> inputArgBuf(tmpInput.begin(), tmpInput.end());
	inputArgBuf.push_back('\0');
	char optI[] = "-i";
	char optN[] = "-n";
	char nValue[] = "7";

	char *argv[] = {prog, optI, inputArgBuf.data(), optN, nValue};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(5, argv);

	const CliSettings *settings = std::get_if<CliSettings>(&outcome);
	ensure("parseCommandLine succeeds for a real, existing input file", settings != nullptr);
	if (settings)
	{
		ensure("resolved input file path points at the same file", std::filesystem::equivalent(settings->inputFilePath, tmpInput));
		ensure_equals("resolved nMaxIterations", settings->nMaxIterations, 7);
	}

	std::filesystem::remove(tmpInput);
}

template<>
template<>
void object::test<10>()
{
	set_test_name("parseCommandLine: no arguments at all is a CliParseResult error");

	char prog[] = "LGC";
	char *argv[] = {prog};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(1, argv);

	const CliParseResult *result = std::get_if<CliParseResult>(&outcome);
	ensure("no arguments -> early exit", result != nullptr);
	if (result)
	{
		ensure_equals("exit code", result->exitCode, 1);
		ensure("message mentions no arguments provided", result->message.find("No arguments provided") != std::string::npos);
	}
}

template<>
template<>
void object::test<11>()
{
	set_test_name("parseCommandLine: -i with a missing value is a CliParseResult error");

	char prog[] = "LGC";
	char optI[] = "-i";
	// Trailing nullptr mimics the real argv[argc]==nullptr guarantee the legacy
	// code (unconditional `if (!argv[i + 1])`) relies on, since a hand-built
	// array otherwise has no element there at all.
	char *argv[] = {prog, optI, nullptr};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(2, argv);

	const CliParseResult *result = std::get_if<CliParseResult>(&outcome);
	ensure("-i with no value -> early exit", result != nullptr);
	if (result)
	{
		ensure_equals("exit code", result->exitCode, 1);
		ensure("message mentions missing file path", result->message.find("Input file path not provided") != std::string::npos);
	}
}

template<>
template<>
void object::test<12>()
{
	set_test_name("parseCommandLine: -i pointing at a nonexistent file is a CliParseResult error");

	char prog[] = "LGC";
	char optI[] = "-i";
	char missingFile[] = "does_not_exist_lgc_test.lgc2";
	char *argv[] = {prog, optI, missingFile};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(3, argv);

	const CliParseResult *result = std::get_if<CliParseResult>(&outcome);
	ensure("-i on a nonexistent file -> early exit", result != nullptr);
	if (result)
	{
		ensure_equals("exit code", result->exitCode, 1);
		ensure("message mentions file does not exist", result->message.find("does not exist") != std::string::npos);
	}
}

template<>
template<>
void object::test<13>()
{
	set_test_name("parseCommandLine: -i never given is a CliParseResult error");

	char prog[] = "LGC";
	char optN[] = "-n";
	char nValue[] = "5";
	char *argv[] = {prog, optN, nValue};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(3, argv);

	const CliParseResult *result = std::get_if<CliParseResult>(&outcome);
	ensure("no -i -> early exit", result != nullptr);
	if (result)
	{
		ensure_equals("exit code", result->exitCode, 1);
		ensure("message mentions -i not specified", result->message.find("\"-i\" not specified") != std::string::npos);
	}
}

template<>
template<>
void object::test<14>()
{
	set_test_name("parseCommandLine: bad/missing -o and -n values are non-fatal, not CliParseResult errors");

	const std::string tmpInput = makeTempInputFile("lgc_test_parseCommandLine_badopts.lgc2");
	char prog[] = "LGC";
	std::vector<char> inputArgBuf(tmpInput.begin(), tmpInput.end());
	inputArgBuf.push_back('\0');
	char optI[] = "-i";
	char optO[] = "-o";
	char optN[] = "-n";
	char badNValue[] = "abc";

	{
		// -o with a missing value: non-fatal, still resolves CliSettings.
		char *argv[] = {prog, optI, inputArgBuf.data(), optO, nullptr};
		ScopedLoggerReset resetLogger;
		const auto outcome = parseCommandLine(4, argv);
		ensure("-o missing value does not error out", std::get_if<CliSettings>(&outcome) != nullptr);
	}
	{
		// -n with a missing value: non-fatal, still resolves CliSettings.
		char *argv[] = {prog, optI, inputArgBuf.data(), optN, nullptr};
		ScopedLoggerReset resetLogger;
		const auto outcome = parseCommandLine(4, argv);
		ensure("-n missing value does not error out", std::get_if<CliSettings>(&outcome) != nullptr);
	}
	{
		// -n with a non-numeric value: non-fatal (caught internally), still resolves CliSettings,
		// and nMaxIterations keeps its default rather than being partially updated.
		char *argv[] = {prog, optI, inputArgBuf.data(), optN, badNValue};
		ScopedLoggerReset resetLogger;
		const auto outcome = parseCommandLine(5, argv);
		const CliSettings *settings = std::get_if<CliSettings>(&outcome);
		ensure("-n non-numeric value does not error out", settings != nullptr);
		if (settings)
			ensure_equals("nMaxIterations keeps its default", settings->nMaxIterations, MAX_ITERATIONS);
	}
	{
		// -n with a partially-numeric value ("100abc"): std::stoi used to silently accept the
		// leading "100" and drop the rest. Must now be rejected like a fully non-numeric value.
		char partialNValue[] = "100abc";
		char *argv[] = {prog, optI, inputArgBuf.data(), optN, partialNValue};
		ScopedLoggerReset resetLogger;
		const auto outcome = parseCommandLine(5, argv);
		const CliSettings *settings = std::get_if<CliSettings>(&outcome);
		ensure("-n partially-numeric value does not error out", settings != nullptr);
		if (settings)
			ensure_equals("nMaxIterations keeps its default for a partially-numeric value", settings->nMaxIterations, MAX_ITERATIONS);
	}
	{
		// -n with a non-positive value: must be rejected, not accepted as a valid iteration count.
		char zeroNValue[] = "0";
		char negativeNValue[] = "-5";
		{
			char *argv[] = {prog, optI, inputArgBuf.data(), optN, zeroNValue};
			ScopedLoggerReset resetLogger;
			const auto outcome = parseCommandLine(5, argv);
			const CliSettings *settings = std::get_if<CliSettings>(&outcome);
			ensure("-n 0 does not error out", settings != nullptr);
			if (settings)
				ensure_equals("nMaxIterations keeps its default for -n 0", settings->nMaxIterations, MAX_ITERATIONS);
		}
		{
			char *argv[] = {prog, optI, inputArgBuf.data(), optN, negativeNValue};
			ScopedLoggerReset resetLogger;
			const auto outcome = parseCommandLine(5, argv);
			const CliSettings *settings = std::get_if<CliSettings>(&outcome);
			ensure("-n -5 does not error out", settings != nullptr);
			if (settings)
				ensure_equals("nMaxIterations keeps its default for -n -5", settings->nMaxIterations, MAX_ITERATIONS);
		}
	}

	std::filesystem::remove(tmpInput);
}

template<>
template<>
void object::test<15>()
{
	set_test_name("parseCommandLine: -d with an explicit log path is reflected in CliSettings");

	const std::string tmpInput = makeTempInputFile("lgc_test_parseCommandLine_dlog.lgc2");
	const std::string tmpLog = (std::filesystem::temp_directory_path() / "lgc_test_parseCommandLine_d.log").string();

	char prog[] = "LGC";
	std::vector<char> inputArgBuf(tmpInput.begin(), tmpInput.end());
	inputArgBuf.push_back('\0');
	std::vector<char> logArgBuf(tmpLog.begin(), tmpLog.end());
	logArgBuf.push_back('\0');
	char optI[] = "-i";
	char optD[] = "-d";

	char *argv[] = {prog, optI, inputArgBuf.data(), optD, logArgBuf.data()};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(5, argv);

	const CliSettings *settings = std::get_if<CliSettings>(&outcome);
	ensure("parseCommandLine succeeds", settings != nullptr);
	if (settings)
		// Compared as paths, not via std::filesystem::equivalent(): the log file
		// is only recorded here, not created (that happens lazily on first write).
		ensure("logFilePath uses -d's explicit path", std::filesystem::path(settings->logFilePath) == std::filesystem::path(tmpLog));

	std::filesystem::remove(tmpInput);
	std::filesystem::remove(tmpLog);
}

template<>
template<>
void object::test<16>()
{
	set_test_name("parseCommandLine: bare -d immediately followed by another flag doesn't swallow it as -d's value");

	const std::string tmpInput = makeTempInputFile("lgc_test_parseCommandLine_dedge.lgc2");

	char prog[] = "LGC";
	std::vector<char> inputArgBuf(tmpInput.begin(), tmpInput.end());
	inputArgBuf.push_back('\0');
	char optI[] = "-i";
	char optD[] = "-d";
	char optO[] = "-o";
	// Trailing nullptr: after skipping -o as -d's value, the second pass still
	// reaches -o's own (missing-value) case, which reads argv[i + 1] unconditionally.
	char *argv[] = {prog, optI, inputArgBuf.data(), optD, optO, nullptr};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(5, argv);

	const CliSettings *settings = std::get_if<CliSettings>(&outcome);
	ensure("parseCommandLine succeeds (bad -o is only a non-fatal warning)", settings != nullptr);
	if (settings)
		ensure("logFilePath was not clobbered to '-o'", settings->logFilePath.find("-o") == std::string::npos);

	std::filesystem::remove(tmpInput);
}

template<>
template<>
void object::test<17>()
{
	set_test_name("parseCommandLine: a dash-prefixed -i value is never re-examined as -n/-o by a later pass");

	// The value must itself be the literal argv token (not just contain a dash somewhere in an
	// absolute path), so it has to be a relative filename resolved against the current directory.
	const std::filesystem::path originalCwd = std::filesystem::current_path();
	const std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
	const std::string dashInputName = "-nasty_lgc_test_parseCommandLine.lgc2";
	const std::string tmpInput = makeTempInputFile(dashInputName);
	std::filesystem::current_path(tmpDir);

	char prog[] = "LGC";
	char optI[] = "-i";
	std::vector<char> inputArgBuf(dashInputName.begin(), dashInputName.end());
	inputArgBuf.push_back('\0');
	char optN[] = "-n";
	char nValue[] = "9";
	char optO[] = "-o";
	char realOutput[] = "lgc_test_parseCommandLine_realOutput.res";

	char *argv[] = {prog, optI, inputArgBuf.data(), optN, nValue, optO, realOutput};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(7, argv);

	std::filesystem::current_path(originalCwd);
	std::filesystem::remove(tmpInput);

	const CliSettings *settings = std::get_if<CliSettings>(&outcome);
	ensure("parseCommandLine succeeds despite a dash-prefixed -i value", settings != nullptr);
	if (settings)
	{
		ensure_equals("the real -n value is honored, not swallowed by the dash-prefixed -i value", settings->nMaxIterations, 9);
		ensure("the real -o value is honored, not swallowed", settings->outputFilePath.find("lgc_test_parseCommandLine_realOutput.res") != std::string::npos);
	}
}

template<>
template<>
void object::test<18>()
{
	set_test_name("parseCommandLine: bare -d as the very last argument doesn't crash and keeps the default log file");

	const std::string tmpInput = makeTempInputFile("lgc_test_parseCommandLine_dlast.lgc2");

	char prog[] = "LGC";
	std::vector<char> inputArgBuf(tmpInput.begin(), tmpInput.end());
	inputArgBuf.push_back('\0');
	char optI[] = "-i";
	char optD[] = "-d";
	char *argv[] = {prog, optI, inputArgBuf.data(), optD};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(4, argv);

	const CliSettings *settings = std::get_if<CliSettings>(&outcome);
	ensure("parseCommandLine succeeds with a trailing bare -d", settings != nullptr);
	if (settings)
		ensure("logFilePath keeps the default .log2 path, not clobbered by a missing -d value", settings->logFilePath.find(".log2") != std::string::npos);

	std::filesystem::remove(tmpInput);
}

template<>
template<>
void object::test<19>()
{
	set_test_name("validateKnownOptions: a repeated option (same or different spelling) is rejected");

	char prog[] = "LGC";
	char optI[] = "-i";
	char optICaps[] = "-I";
	char file1[] = "file1.lgc2";
	char file2[] = "file2.lgc2";
	char optH[] = "-h";

	{
		char *argv[] = {prog, optI, file1, optI, file2};
		const auto result = validateKnownOptions(5, argv);
		ensure("repeated -i is rejected", result.has_value());
		if (result)
			ensure("message names the repeated option", result->message.find("-i") != std::string::npos);
	}
	{
		// -i and -I are different spellings of the same OptionId: tracking must be by id, not spelling.
		char *argv[] = {prog, optI, file1, optICaps, file2};
		const auto result = validateKnownOptions(5, argv);
		ensure("repeated Input option under a different spelling is rejected", result.has_value());
	}
	{
		// Help/Version are pure switches; repeating them is harmless and not flagged here.
		char *argv[] = {prog, optH, optH};
		const auto result = validateKnownOptions(3, argv);
		ensure("repeated -h is not rejected", !result.has_value());
	}
}

template<>
template<>
void object::test<20>()
{
	set_test_name("parseCommandLine: a repeated -i is a CliParseResult error, not a silent first-wins");

	const std::string tmpInput1 = makeTempInputFile("lgc_test_parseCommandLine_dup1.lgc2");
	const std::string tmpInput2 = makeTempInputFile("lgc_test_parseCommandLine_dup2.lgc2");

	char prog[] = "LGC";
	char optI[] = "-i";
	std::vector<char> input1Buf(tmpInput1.begin(), tmpInput1.end());
	input1Buf.push_back('\0');
	std::vector<char> input2Buf(tmpInput2.begin(), tmpInput2.end());
	input2Buf.push_back('\0');
	char *argv[] = {prog, optI, input1Buf.data(), optI, input2Buf.data()};

	ScopedLoggerReset resetLogger;
	const auto outcome = parseCommandLine(5, argv);

	const CliParseResult *result = std::get_if<CliParseResult>(&outcome);
	ensure("repeated -i -> early exit", result != nullptr);
	if (result)
	{
		ensure_equals("exit code", result->exitCode, 1);
		ensure("message mentions the option is specified more than once", result->message.find("more than once") != std::string::npos);
	}

	std::filesystem::remove(tmpInput1);
	std::filesystem::remove(tmpInput2);
}
} // namespace tut
