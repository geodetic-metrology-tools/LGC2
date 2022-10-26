#include <iostream>
#include "TLGCApp.h"
#include "FileUtils.h"
#include <FileLogHandler.hpp>
#include <ConsoleLogHandler.hpp>
#include <Logger.hpp>
#include "Defaults.h"
#include "TMonitor.h"
#include <chrono>

#include "TFileLogger.h"  // Will be obsolete soon
using namespace std::chrono;


int main(int argc, char *argv[])
{
    auto start = high_resolution_clock::now();
    TMonitor mockup;
    for (int i=0;i<10000;i++){
        mockup.adjust();
        std::cout << "Fras iteration " << i << std::endl;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << "Elapsed time (ms): " << duration.count() << std::endl;

//
//	// ********  WILL BE OBSOLETE SOON !!!  WILL BE OBSOLETE SOON !!!   *********
//#ifdef __linux__
//	const std::string logFilePath2 = svlTools::getCurrentDirectory() + slash + "LOGFile.log";
//#else
//	const std::string logFilePath2 = "C:\\temp\\LOGFile.log";
//#endif
//	TFileLogger logFile(logFilePath2, "LGC log file");
//	// **************************************************************************
//
//	bool bChangeLogFile = true;
//
//	std::string inputFilePath, outputFilePath, logFilePath;
//	int nMaxIterations = MAX_ITERATIONS;
//
//	// Default Log file (it can be changed by the user with the -D option)
//	logFilePath = svlTools::getCurrentDirectory() + slash + "DefaultLogFileLGC.log";
//
//	// Creates the Logger mechanism (here log to file and console)
//	// IMPORTANT: Use the macros logDebug(), logInfo(), etc everywhere in the project !
//	FileLogHandler *pLogFileHandler = new FileLogHandler(logFilePath);
//	pLogFileHandler->setThreshold(LogMessage::Type::INFO);
//	Logger::getLogger().addHandlers(pLogFileHandler,	new ConsoleLogHandler());
//
//	if (argc == 1)
//	{
//		logFatal() << "Launch LGC: No argument, LGC needs at least the project filepath after the -i flag.";
//	}
//	else
//	{
//		for (int i = 0; i < argc; i++)
//		{
//			if (argv[i][0] == '-')
//			{
//				switch (argv[i][1])
//				{
//					// run command line interface
//				case 'i':
//				case 'I':
//				{
//					if (!argv[i + 1]) {
//						logFatal() << "Launch LGC: I/-i option used, but the input file path was not specified.";
//						break;
//					}
//
//					// Look if absolute path is used
//					inputFilePath = svlTools::getPathFileName(argv[i + 1]);
//					break;
//				}
//
//				case 'o':
//				case 'O':
//				{
//					if (!argv[i + 1]) {
//						logFatal() << "Launch LGC: -O/-o option used, but the output file path was not specified.";
//						break;
//					}
//
//					// Look if absolute path is used
//					outputFilePath = svlTools::getPathFileName(argv[i + 1]);
//					break;
//				}
//
//				case 'n':
//				case 'N':
//				{
//					if (!argv[i + 1]) {
//						logFatal() << "Launch LGC: -N/-n option used, but the maximal number of iterations was not specified.";
//						break;
//					}
//					try {
//						nMaxIterations = std::stoi(argv[i + 1]);
//					}
//					catch (std::invalid_argument& e) {
//						// if no conversion could be performed
//						logFatal() << "Launch LGC: -N/-n option used, but the maximal number of iterations is not correctly defined.\nException caught: " << e.what();
//					}
//					break;
//				}
//
//				// Executes the program with DEBUG level messages stored in a LOG file
//				// This log file is by default defined in the temporary folder, or given as an argument following -D option
//				case 'd':
//				case 'D':
//				{
//					// Decreases the Log level to DEBUG level
//					pLogFileHandler->setThreshold(LogMessage::Type::DEBUG);
//
//					// Changes the log file attached to this handler
//					if (argv[i + 1]) 
//					{
//						logFilePath = svlTools::getPathFileName(argv[i + 1]);
//						pLogFileHandler->setLogFile(logFilePath);
//						bChangeLogFile = false;
//					}
//
//					break;
//				}
//
//				default:
//					break;
//				}
//			}
//		}
//
//
//		if (inputFilePath.empty())
//		{
//			logFatal() << "Launch LGC: Error, the input file is not found. Give the path after -i flag.";
//			return 1;
//		}
//		else if (outputFilePath.empty())
//			// Output file becomes the input filename with the ".res" extension
//			outputFilePath = svlTools::getFilePathWithoutExtension(inputFilePath) + ".res";
//
//		svlTools::createOutputFile(outputFilePath);
//
//		// Changes the log file attached to this handler
//		// Log file becomes the input filename with the ".log2" extension
//		if (bChangeLogFile)
//		{
//			logFilePath = svlTools::getFilePathWithoutExtension(outputFilePath) + ".log2";
//			pLogFileHandler->setLogFile(logFilePath);
//		}
//
//		//
//		// Main code for executing the whole calculations
//		//
//		try
//		{
//			logInfo() << "This Log File contains more detailed data when application launched in Debug mode with -D option!";
//			logInfo() << "See User Guide: https://readthedocs.web.cern.ch/display/SUS/LGC2+User+Guide";
//			logInfo() << "Starting the calculations...";
//			TLGCApp lgc(inputFilePath, outputFilePath, nMaxIterations);
//			Behavior b = lgc.exec();
//			return (bool) b ? 0 : b.code();
//		}
//		catch (const std::runtime_error& ex)
//		{
//			logFatal() << "LGC calculation problem\nException: " << ex.what();
//		}
//	}
//
	return 1;
}
