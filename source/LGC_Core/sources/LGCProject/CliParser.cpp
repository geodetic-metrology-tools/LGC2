// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CliParser.h"

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <set>
#include <system_error>

#include <ConsoleLogHandler.hpp>
#include <FileLogHandler.hpp>
#include <Logger.hpp>

#include "Defaults.h"
#include "FileUtils.h"
#include "TFileLogger.h" // Will be obsolete soon
#include "TLGCApp.h"
#include "Version.h"

const std::vector<OptionSpec> &optionSpecs()
{
	static const std::vector<OptionSpec> specs = {
		{OptionId::Input, {"-i", "-I"}, "<file>", false, "Input file (required)"},
		{OptionId::Output, {"-o", "-O"}, "<file>", false, "Output file (default: <input>.res)"},
		{OptionId::MaxIterations, {"-n", "-N"}, "<int>", false, "Maximum number of iterations (default: " + std::to_string(MAX_ITERATIONS) + ")"},
		{OptionId::Debug, {"-d", "-D"}, "[<file>]", true, "Enable debug logging, optionally to <file>"},
		{OptionId::Version, {"-V", "--version"}, "", false, "Print version information and exit"},
		{OptionId::Help, {"-h", "-H", "--help"}, "", false, "Print this help message and exit"},
	};
	return specs;
}

const OptionSpec *findSpec(std::string_view arg)
{
	for (const auto &spec : optionSpecs())
		if (std::find(spec.spellings.begin(), spec.spellings.end(), arg) != spec.spellings.end())
			return &spec;
	return nullptr;
}

bool optionTakesValue(std::string_view arg)
{
	const OptionSpec *spec = findSpec(arg);
	return spec != nullptr && !spec->argHint.empty();
}

bool optionValueIsOptional(std::string_view arg)
{
	const OptionSpec *spec = findSpec(arg);
	return spec != nullptr && spec->optionalValue;
}

namespace
{
bool isRequestFor(std::string_view arg, OptionId id)
{
	const OptionSpec *spec = findSpec(arg);
	return spec != nullptr && spec->id == id;
}
} // namespace

bool isHelpRequest(std::string_view arg)
{
	return isRequestFor(arg, OptionId::Help);
}

bool isVersionRequest(std::string_view arg)
{
	return isRequestFor(arg, OptionId::Version);
}

bool looksLikeFlag(std::string_view token)
{
	return token.size() >= 2 && token[0] == '-';
}

std::string optionLeftColumn(const OptionSpec &spec)
{
	std::string left;
	for (const auto &spelling : spec.spellings)
	{
		if (!left.empty())
			left += ", ";
		left += spelling;
	}
	if (!spec.argHint.empty())
		left += std::string(" ") + std::string(spec.argHint);
	return left;
}

std::string buildOptionsHelpText()
{
	size_t width = 0;
	for (const auto &spec : optionSpecs())
		width = std::max(width, optionLeftColumn(spec).size());

	std::string text = "Options:\n";
	for (const auto &spec : optionSpecs())
	{
		const std::string left = optionLeftColumn(spec);
		text += "  " + left + std::string(width - left.size() + 2, ' ') + spec.description + '\n';
	}
	return text;
}

std::string buildUsageLine()
{
	std::string line;
	for (const auto &spec : optionSpecs())
	{
		if (!line.empty())
			line += ' ';
		std::string token(spec.spellings.front());
		if (!spec.argHint.empty())
			token += std::string(" ") + std::string(spec.argHint);
		line += (spec.id == OptionId::Input) ? token : "[" + token + "]";
	}
	return line;
}

namespace
{
std::string versionLine()
{
	return "LGC2 " + getLGCVersion() + '\n';
}
} // namespace

std::optional<CliParseResult> checkHelpOrVersion(int argc, char *const argv[])
{
	// -h always wins over -V, regardless of argv order; -v is usually reserved for 'verbose'.
	// argc > 0: guards against argc == 0, which some non-shell launchers can produce.
	if (argc > 0 && std::any_of(argv + 1, argv + argc, isHelpRequest))
	{
		std::string message = versionLine() + "\nUsage: LGC " + buildUsageLine() + "\n\n" + buildOptionsHelpText();
		return CliParseResult{std::move(message), 0};
	}
	if (argc > 0 && std::any_of(argv + 1, argv + argc, isVersionRequest))
	{
		// GNU standard: --version's first line must be "ProgramName Version", followed by copyright/warranty.
		std::string message = versionLine() + TLGCApp::getCopyright() + '\n' +
			R"(This is free software; see the source for copying conditions. There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
)";
		return CliParseResult{std::move(message), 0};
	}
	return std::nullopt;
}

namespace
{
// Argv slots consumed by the option at argv[i]: 2 if it takes argv[i + 1] as its value, else 1.
int optionStep(std::string_view arg, int argc, char *const argv[], int i)
{
	if (optionTakesValue(arg) && i + 1 < argc)
		if (!optionValueIsOptional(arg) || !looksLikeFlag(argv[i + 1]))
			return 2;
	return 1;
}
} // namespace

std::optional<CliParseResult> validateKnownOptions(int argc, char *const argv[])
{
	// Reject unrecognized options (GNU-style); skips -i/-o/-n/-d's value slot so dashed values aren't misflagged, -d's value is optional.
	std::set<OptionId> seen;
	int i = 1;
	while (i < argc)
	{
		const std::string_view arg{argv[i]};
		if (arg.size() < 2 || arg[0] != '-')
		{
			i++;
			continue;
		}

		const OptionSpec *spec = findSpec(arg);
		if (!spec)
		{
			std::string message = "LGC: invalid option -- '" + std::string(arg) + "'\n";
			if (arg == "-v")
				message += "Did you mean '-V'?\n";
			message += "Try 'LGC --help' for more information.\n";
			return CliParseResult{std::move(message), 1};
		}

		// Help/Version are pure switches already handled by checkHelpOrVersion(); repeating them is harmless, only value-carrying options are tracked here.
		if (spec->id != OptionId::Help && spec->id != OptionId::Version)
		{
			if (!seen.insert(spec->id).second)
			{
				std::string message = "LGC: option '" + std::string(arg)
					+ "' specified more than once\n"
					  "Try 'LGC --help' for more information.\n";
				return CliParseResult{std::move(message), 1};
			}
		}

		i += optionStep(arg, argc, argv, i);
	}
	return std::nullopt;
}

namespace
{
CliParseResult inputFileError(const std::string &logFilePath, const std::string &message)
{
	TFileLogger errorLog(logFilePath, "LGC output file");
	errorLog.writeReportHeader("Reading input file:");
	errorLog << TFileLogger::e_logType::LOG_ERROR << message;

	return CliParseResult{"[ERROR]: " + message + "\nError details logged to: " + logFilePath + "\n", 1};
}
} // namespace

std::variant<CliSettings, CliParseResult> parseCommandLine(int argc, char *argv[])
{
	// Three passes, not one: checkHelpOrVersion()/validateKnownOptions() are pure and independently testable; this loop below is where side effects happen.
	if (auto earlyExit = checkHelpOrVersion(argc, argv))
		return *earlyExit;
	if (auto earlyExit = validateKnownOptions(argc, argv))
		return *earlyExit;

	// ********  WILL BE OBSOLETE SOON !!!  WILL BE OBSOLETE SOON !!!   *********
#ifdef __linux__
	const std::string logFilePath2 = svlTools::getCurrentDirectory() + slash + "LOGFile.log";
#else
	const std::string logFilePath2 = "C:\\temp\\LOGFile.log";
#endif
	TFileLogger logFile(logFilePath2, "LGC log file");
	// **************************************************************************

	bool useDefaultOutputLog = true; // true: derive the log path from -o/the input file; false: -d gave an explicit path

	std::string inputFilePath, outputFilePath, logFilePath;
	int nMaxIterations = MAX_ITERATIONS;

	// Default Log file (it can be changed by the user with the -D option)
	logFilePath = svlTools::getCurrentDirectory() + slash + "DefaultLogFileLGC.log";

	// Creates the Logger mechanism (here log to file and console)
	// IMPORTANT: Use the macros logDebug(), logInfo(), etc everywhere in the project !
	FileLogHandler *pLogFileHandler = new FileLogHandler(logFilePath);
	pLogFileHandler->setThreshold(LogMessage::Type::INFO);
	Logger::getLogger().addHandlers(pLogFileHandler, new ConsoleLogHandler());

	// Early return: No arguments provided at all
	if (argc == 1)
		return inputFileError(logFilePath, "No arguments provided. At least input file argument \"-i\" must be specified\nUse -h or --help for usage information.");

	// Single pass over argv (mirrors validateKnownOptions()'s skip-logic): a value consumed by one option is never re-examined as a different option, unlike two independent scans would.
	// validateKnownOptions() already rejected duplicates, so each OptionId appears at most once here; the empty-check below is just a harmless invariant guard.
	int i = 1;
	while (i < argc)
	{
		const std::string_view arg{argv[i]};
		const OptionSpec *spec = findSpec(arg);

		const int step = optionStep(arg, argc, argv, i); // 2 if the value slot is also consumed
		const bool hasValue = (step == 2);

		if (spec)
		{
			switch (spec->id)
			{
			case OptionId::Input:
				if (inputFilePath.empty())
				{
					if (!hasValue)
						return inputFileError(logFilePath, "Input file path not provided");

					// Look if absolute path is used
					inputFilePath = svlTools::getPathFileName(argv[i + 1]);

					// Set the log file early (.log) so error messages land in the expected file
					if (useDefaultOutputLog && !inputFilePath.empty())
					{
						std::string potentialOutputPath = svlTools::getFilePathWithoutExtension(inputFilePath) + ".res";
						logFilePath = svlTools::getFilePathWithoutExtension(potentialOutputPath) + ".log";
						std::filesystem::remove(logFilePath.c_str());
						pLogFileHandler->setLogFile(logFilePath);
						// Don't set useDefaultOutputLog = false here, so it can be changed to .log2 for successful processing
					}

					{
						std::error_code ec;
						if (!std::filesystem::is_regular_file(inputFilePath, ec))
						{
							// "Not found" isn't a real error on some stdlibs (incl. MSVC's); only surface ec's reason for genuine errors, e.g. permission denied.
							// ec.message() isn't guaranteed UTF-8 on Windows (ANSI codepage); report the portable, always-ASCII error code instead.
							const bool isPlainNotFound = !ec || ec == std::errc::no_such_file_or_directory;
							const std::string reason = isPlainNotFound ? "" : (" (system error " + std::to_string(ec.value()) + ")");
							return inputFileError(logFilePath, "Input file does not exist in the path provided" + reason);
						}
					}
				}
				break;

			case OptionId::Output:
				if (!hasValue)
					logFatal() << "Launch LGC: -O/-o option used, but the output file path was not specified.";
				else
				{
					// Look if absolute path is used
					outputFilePath = svlTools::getPathFileName(argv[i + 1]);
				}
				break;

			case OptionId::MaxIterations:
				if (!hasValue)
					logFatal() << "Launch LGC: -N/-n option used, but the maximal number of iterations was not specified.";
				else
				{
					const std::string_view valueArg{argv[i + 1]};
					int parsed = 0;
					const auto result = std::from_chars(valueArg.data(), valueArg.data() + valueArg.size(), parsed);
					if (result.ec != std::errc() || result.ptr != valueArg.data() + valueArg.size() || parsed <= 0)
						logFatal() << "Launch LGC: -N/-n option used, but the maximal number of iterations is not correctly defined.";
					else
						nMaxIterations = parsed;
				}
				break;

			// -d switches to DEBUG-level logging, optionally to a given log file (defaults to the temp folder)
			case OptionId::Debug:
				pLogFileHandler->setThreshold(LogMessage::Type::DEBUG);

				// Only switch the log file if a real path follows it, not another flag
				if (hasValue)
				{
					logFilePath = svlTools::getPathFileName(argv[i + 1]);
					useDefaultOutputLog = false;
				}
				break;

			default:
				break; // Help/Version can't reach here: checkHelpOrVersion() already returned early.
			}
		}

		i += step;
	}

	// Check if required parameter was provided
	if (inputFilePath.empty())
		return inputFileError(logFilePath, "Input file argument \"-i\" not specified");

	if (outputFilePath.empty())
		// Output file becomes the input filename with the ".res" extension
		outputFilePath = svlTools::getFilePathWithoutExtension(inputFilePath) + ".res";

	svlTools::createOutputFile(outputFilePath);

	// Successful run: switch the log file to <output>.log2
	if (useDefaultOutputLog)
	{
		logFilePath = svlTools::getFilePathWithoutExtension(outputFilePath) + ".log2";
	}
	std::filesystem::remove(logFilePath.c_str());
	pLogFileHandler->setLogFile(logFilePath);

	return CliSettings{inputFilePath, outputFilePath, nMaxIterations, logFilePath};
}
