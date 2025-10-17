// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <filesystem>
#include <iostream>

#include <ConsoleLogHandler.hpp>
#include <FileLogHandler.hpp>
#include <Logger.hpp>

#include "Defaults.h"
#include "FileUtils.h"
#include "TFileLogger.h" // Will be obsolete soon
#include "TLGCApp.h"

int main(int argc, char *argv[])
{
	// ********  WILL BE OBSOLETE SOON !!!  WILL BE OBSOLETE SOON !!!   *********
#ifdef __linux__
	const std::string logFilePath2 = svlTools::getCurrentDirectory() + slash + "LOGFile.log";
#else
	const std::string logFilePath2 = "C:\\temp\\LOGFile.log";
#endif
	TFileLogger logFile(logFilePath2, "LGC log file");
	// **************************************************************************

	bool bChangeLogFile = true;

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
	{
		// Create a proper .log file with TFileLogger format for no arguments
		TFileLogger errorLog(logFilePath, "LGC output file");
		errorLog.writeReportHeader("Reading input file:");
		errorLog << TFileLogger::e_logType::LOG_ERROR << "No arguments provided. Atleast input file argument \"-i\" must be specified";

		std::cerr << "[ERROR]: No arguments provided. Atleast input file argument \"-i\" must be specified" << '\n';
		std::cerr << "Error details logged to: " << logFilePath << '\n';
		return 1;
	}

	// First pass: Handle required parameter (-i)
	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] == '-' && (argv[i][1] == 'i' || argv[i][1] == 'I'))
		{
			if (!argv[i + 1])
			{
				// Create a proper .log file with TFileLogger format for missing file path after -i
				TFileLogger errorLog(logFilePath, "LGC output file");
				errorLog.writeReportHeader("Reading input file:");
				errorLog << TFileLogger::e_logType::LOG_ERROR << "Input file path not provided";

				std::cerr << "[ERROR]: Input file path not provided" << '\n';
				std::cerr << "Error details logged to: " << logFilePath << '\n';
				return 1;
			}

			// Look if absolute path is used
			inputFilePath = svlTools::getPathFileName(argv[i + 1]);

			// Set up log file name early for error cases - use .log for errors
			// This ensures error messages go to the expected log file
			if (bChangeLogFile && !inputFilePath.empty())
			{
				std::string potentialOutputPath = svlTools::getFilePathWithoutExtension(inputFilePath) + ".res";
				logFilePath = svlTools::getFilePathWithoutExtension(potentialOutputPath) + ".log";
				std::filesystem::remove(logFilePath.c_str());
				pLogFileHandler->setLogFile(logFilePath);
				// Don't set bChangeLogFile = false here, so it can be changed to .log2 for successful processing
			}

			// Validate input file immediately
			if (!std::filesystem::is_regular_file(inputFilePath))
			{
				// Create a proper .log file with TFileLogger format for directory path errors
				TFileLogger errorLog(logFilePath, "LGC output file");
				errorLog.writeReportHeader("Reading input file:");
				errorLog << TFileLogger::e_logType::LOG_ERROR << "Input file does not exist in the path provided";

				std::cerr << "[ERROR]: Input file does not exist in the path provided" << '\n';
				std::cerr << "Error details logged to: " << logFilePath << '\n';
				return 1;
			}

			break; // Required parameter found, exit loop
		}
	}

	// Check if required parameter was provided
	if (inputFilePath.empty())
	{
		// Create a proper .log file with TFileLogger format for missing -i argument
		TFileLogger errorLog(logFilePath, "LGC output file");
		errorLog.writeReportHeader("Reading input file:");
		errorLog << TFileLogger::e_logType::LOG_ERROR << "Input file argument \"-i\" not specified";

		std::cerr << "[ERROR]: Input file argument \"-i\" not specified" << '\n';
		std::cerr << "Error details logged to: " << logFilePath << '\n';
		return 1;
	}

	// Second pass: Handle optional parameters
	for (int i = 0; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'o':
			case 'O': {
				if (!argv[i + 1])
				{
					logFatal() << "Launch LGC: -O/-o option used, but the output file path was not specified.";
					break;
				}

				// Look if absolute path is used
				outputFilePath = svlTools::getPathFileName(argv[i + 1]);
				break;
			}

			case 'n':
			case 'N': {
				if (!argv[i + 1])
				{
					logFatal() << "Launch LGC: -N/-n option used, but the maximal number of iterations was not specified.";
					break;
				}
				try
				{
					nMaxIterations = std::stoi(argv[i + 1]);
				}
				catch (std::invalid_argument &e)
				{
					// if no conversion could be performed
					logFatal() << "Launch LGC: -N/-n option used, but the maximal number of iterations is not correctly defined.\nException caught: " << e.what();
				}
				break;
			}

			// Executes the program with DEBUG level messages stored in a LOG file
			// This log file is by default defined in the temporary folder, or given as an argument following -D option
			case 'd':
			case 'D': {
				// Decreases the Log level to DEBUG level
				pLogFileHandler->setThreshold(LogMessage::Type::DEBUG);

				// Changes the log file attached to this handler
				if (argv[i + 1])
				{
					logFilePath = svlTools::getPathFileName(argv[i + 1]);
					bChangeLogFile = false;
				}

				break;
			}

			default:
				break;
			}
		}
	}

	if (outputFilePath.empty())
		// Output file becomes the input filename with the ".res" extension
		outputFilePath = svlTools::getFilePathWithoutExtension(inputFilePath) + ".res";

	svlTools::createOutputFile(outputFilePath);

	// Changes the log file attached to this handler
	// Log file becomes the input filename with the ".log2" extension
	if (bChangeLogFile)
	{
		logFilePath = svlTools::getFilePathWithoutExtension(outputFilePath) + ".log2";
	}
	std::filesystem::remove(logFilePath.c_str());
	pLogFileHandler->setLogFile(logFilePath);

	//
	// Main code for executing the whole calculations
	//
	try
	{
		logInfo() << "This Log File contains more detailed data when application launched in Debug mode with -D option!";
		logInfo() << "See User Guide: https://confluence.cern.ch/display/SUS/LGC2+User+Guide";
		logInfo() << "Starting the calculations...";
		TLGCApp lgc(inputFilePath, outputFilePath, nMaxIterations);
		Behavior b = lgc.exec();
		return (bool)b ? 0 : b.code();
	}
	catch (const std::runtime_error &ex)
	{
		logFatal() << "LGC calculation problem\nException: " << ex.what();
		return 1;
	}
}
