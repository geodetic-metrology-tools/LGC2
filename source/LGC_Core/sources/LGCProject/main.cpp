// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <variant>

#include <Logger.hpp>

#include "CliParser.h"
#include "TLGCApp.h"

int main(int argc, char *argv[])
{
	const auto outcome = parseCommandLine(argc, argv);
	if (const auto *earlyExit = std::get_if<CliParseResult>(&outcome))
	{
		(earlyExit->exitCode == 0 ? std::cout : std::cerr) << earlyExit->message;
		return earlyExit->exitCode;
	}
	const CliSettings &settings = std::get<CliSettings>(outcome);

	//
	// Main code for executing the whole calculations
	//
	try
	{
		logInfo() << "This Log File contains more detailed data when application launched in Debug mode with -D option!";
		logInfo() << "See User Guide: https://confluence.cern.ch/display/SUS/LGC2+User+Guide";
		logInfo() << "Starting the calculations...";
		TLGCApp lgc(settings.inputFilePath, settings.outputFilePath, settings.nMaxIterations);
		Behavior b = lgc.exec();
		return (bool)b ? 0 : b.code();
	}
	catch (const std::runtime_error &ex)
	{
		logFatal() << "LGC calculation problem\nException: " << ex.what();
		return 1;
	}
}
