#include <iostream>
#include "TLGCApp.h"
#include "Utils.h"
#include "TFileLogger.h"

#ifdef __linux__
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <fcntl.h>
#else
	#include <windows.h>

#endif

#include <chrono>
#include <thread>

//Create the output file
void createOutputFile(std::string outputFilePath);

/// Change the path directory path -/ become \ (for windows)
std::string changePathDir(std::string fPath);

/// Return the directory path - until the last \ or /
std::string getPathDir(std::string fPath);

/// Return the file path - until the last .
std::string getPathFile(std::string fPath);

int main( int argc,  char *argv[]){

	std::string inputFilePath;
	std::string inputFilename;
	std::string outputFilePath;

#ifdef __linux__
	const std::string logFilePath = getCurrentDirectory()+slash+"LOGFile.log";
#else
	const std::string logFilePath = "C:\\temp\\LOGFile.log";
#endif

	//std::this_thread::sleep_for(std::chrono::milliseconds(8000));

	if (argc == 1)
	{
		//create a log file to write the error
		TFileLogger logFile(logFilePath, "LGC log file");
		auto& errorMessages(logFile);
		errorMessages.writeReportHeader("Launch LGC:");
		errorMessages << TFileLogger::e_logType::LOG_ERROR << "No argument, LGC needs at least the project filepath after the -i flag.";
	}
	else
	{
		for (int i = 0; i < argc; i++)
		{
			if (argv[i][0] == '-')
			{
				switch (argv[i][1])
				{
					// run command line interface
				case 'i':
				case 'I':
				{
					if (!argv[i + 1]) { // -I/-i option used, but the input file path was not specified
						break;
					}

					inputFilename = argv[i + 1];

					// Look if absolute path is used
#ifdef __linux__    
					if (inputFilename.substr(0, 1).compare(slash) != 0)
						inputFilePath = getCurrentDirectory() + slash + inputFilename;
					else
						inputFilePath = inputFilename;

#else
					inputFilename = changePathDir(inputFilename);

					if (inputFilename.substr(0, 3).compare("C:\\") == 0 || inputFilename.substr(0, 3).compare("c:\\") == 0 || inputFilename.substr(0, 1).compare("\\") == 0)
						inputFilePath = inputFilename;
					else
						inputFilePath = getCurrentDirectory() + slash + inputFilename;
#endif

					break;
				}

				case 'o':
				case 'O':
				{
					if (!argv[i + 1]) { // -O/-o option used, but the output file path was not specified
						break;
					}

					outputFilePath = argv[i + 1];

					// Look if absolute path is used
#ifdef __linux__    
					if (outputFilePath.substr(0, 1).compare(slash) != 0 )
						outputFilePath = getPathDir(inputFilePath) + slash + argv[i + 1];

#else
					outputFilePath = changePathDir(outputFilePath);

					if (outputFilePath.substr(0, 3).compare("C:\\") != 0 && outputFilePath.substr(0, 1).compare("\\") != 0)
						outputFilePath = getCurrentDirectory() + slash + outputFilePath;

#endif

					break;
				}
				}
			}
		}



		if (inputFilename == "" || outputFilePath == ""){
			if (inputFilePath == "")
			{
				//create a log file to write the error
				TFileLogger logFile(logFilePath, "LGC log file");
				auto& errorMessages(logFile);
				errorMessages.writeReportHeader("Launch LGC:");
				errorMessages << TFileLogger::e_logType::LOG_ERROR << "Error, the input file is not found. Give the path after -i flag";
				return 1;
			}
			else if (outputFilePath == "")
				outputFilePath = getPathFile(inputFilePath) + ".res";
		}

		createOutputFile(outputFilePath);

		try
		{
			TLGCApp lgc(inputFilePath, outputFilePath);
			return lgc.exec();
		}
		catch (const std::runtime_error& ex)
		{
			cout << ex.what() << endl << endl;
		}
	}


	return 1;
}

void createOutputFile(std::string outFilePath)
{
	std::string outputFileDirectory = outFilePath.substr(0, outFilePath.find_last_of('\\'));
	//If does not exist, needs to be created
#ifdef __linux__
	struct stat st = { 0 };
	if (stat(outputFileDirectory.c_str(), &st) == -1)
		if (creat(outputFileDirectory.c_str(),S_IRWXU|S_IRWXG) == -1) {
			std::cout << "Output directory does not exist and could not be created" << endl;
		}
#else
	CreateFile(outputFileDirectory.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD ftyp = GetFileAttributesA(outputFileDirectory.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES){ //This means that the directory still does not exist, using default one
		std::cout << "Output directory does not exist and could not be created";
	}

#endif
}

std::string changePathDir(std::string fPath)
{
	int found = 1;
	while (found < fPath.length())
	{
		found = fPath.find_first_of("/");
		if (found == std::string::npos)
			return fPath;
		else
		{
			fPath = fPath.substr(0, found) + "\\" + fPath.substr(found + 1, fPath.length());
			found += 1;
		}
	}
	return fPath;
}

std::string getPathDir(std::string fPath)
{
	std::size_t found = fPath.find_last_of(slash);
	return fPath.substr(0, found);
}

std::string getPathFile(std::string fPath)
{
	std::size_t found = fPath.find_last_of(".");
	return fPath.substr(0, found);
}
