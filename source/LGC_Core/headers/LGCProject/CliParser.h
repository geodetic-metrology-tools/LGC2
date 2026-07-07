/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGCCLIPARSER_H_
#define _LGCCLIPARSER_H_

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

/// Stable identity for an option, independent of its spelling(s).
enum class OptionId
{
	Input,
	Output,
	MaxIterations,
	Debug,
	Version,
	Help
};

/*!
	\ingroup LGCProject
	Single source of truth for LGC's option table: both the -h help text and the
	invalid-option check derive from this.
*/
struct OptionSpec
{
	OptionId id;
	std::vector<std::string_view> spellings;
	std::string_view argHint; // "" if the flag takes no value
	bool optionalValue; // true if the value may be omitted (e.g. -d's log file)
	std::string description;
};

/// All recognized command-line options.
const std::vector<OptionSpec> &optionSpecs();

/// Returns the OptionSpec matching arg, or nullptr if arg is not a known spelling.
const OptionSpec *findSpec(std::string_view arg);

/// True if the option matching arg expects a following value.
bool optionTakesValue(std::string_view arg);

/// True if the option matching arg's value may be omitted (e.g. -d's log file).
bool optionValueIsOptional(std::string_view arg);

/// True if arg is one of the Help option's spellings.
bool isHelpRequest(std::string_view arg);

/// True if arg is one of the Version option's spellings.
bool isVersionRequest(std::string_view arg);

/// True if token looks like a flag (starts with '-' and has at least 2 characters).
bool looksLikeFlag(std::string_view token);

/// Builds the left-hand ("-i, -I <file>") column text for a single option.
std::string optionLeftColumn(const OptionSpec &spec);

/// Builds the full, column-aligned "Options:\n ..." help text block.
std::string buildOptionsHelpText();

/// Builds the "-i <file> [-o <file>] ..." Usage synopsis from optionSpecs().
std::string buildUsageLine();

/// Resolved settings for a command line that parsed successfully; ready to construct/run TLGCApp.
struct CliSettings
{
	std::string inputFilePath;
	std::string outputFilePath;
	int nMaxIterations;
	std::string logFilePath;
};

/// Outcome of an early command-line exit: text to print, and the process exit code.
struct CliParseResult
{
	std::string message; // fully formatted, newline-terminated, ready to print as-is
	int exitCode; // 0 -> print message to std::cout; nonzero -> std::cerr
};

/// Pure (no I/O): -h/-V detection. -h wins over -V regardless of argv order.
std::optional<CliParseResult> checkHelpOrVersion(int argc, char *const argv[]);

/// Pure (no I/O): GNU-style unknown-option rejection, plus rejects an option repeated under any spelling (e.g. "-i a -i b" or "-i a -I b").
std::optional<CliParseResult> validateKnownOptions(int argc, char *const argv[]);

/// Parses argv fully, with side effects; returns CliSettings on success, or a CliParseResult for main() to print and return on any early exit.
std::variant<CliSettings, CliParseResult> parseCommandLine(int argc, char *argv[]);

#endif
