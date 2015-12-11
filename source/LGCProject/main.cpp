#include <iostream>
#include "TLGCApp.h"
#include "Utils.h"

#ifdef __linux__
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#else
	#include <windows.h>
#endif

#include <thread>
#include <chrono>

void createOutputFile(std::string outputFilePath);
std::string getPathDir(std::string fPath);
std::string getPathFile(std::string fPath);

int main( int argc,  char *argv[]){

	std::string inputFilePath;
	std::string inputFilename;
	std::string outputFilePath;
	

	for (int i=0 ; i<argc ; i++)
	{
		if(argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
				// run command line interface
				case 'i':
				case 'I':
				{
					if (!argv[i + 1]) { // -I/-i option used, but the input file path was not specified
						std::cout << "Input file was not specified" << endl;
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

					if (inputFilename.substr(0, 3).compare("C:\\") == 0 || inputFilename.substr(0, 1).compare("\\") == 0)
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
						std::cout << "Output file was not specified" << endl;
						break;
					}

					outputFilePath = argv[i + 1];

					// Look if absolute path is used
					#ifdef __linux__    
					if (outputFilePath.substr(0, 1).compare(slash) != 0 )
						outputFilePath = getPath(inputFilePath) + argv[i + 1];

					#else
					if (outputFilePath.substr(0, 3).compare("C:\\") != 0 && outputFilePath.substr(0, 1).compare("\\") != 0)
						outputFilePath = getPathDir(inputFilePath) + argv[i + 1];

					#endif

					break;
				}
			}
		}
	}



	if (inputFilename == "" || outputFilePath == ""){
		if (inputFilePath == "")
			throw runtime_error("Error, the input file is not found");
		else if (outputFilePath == "")
			outputFilePath = getPathFile(inputFilePath) + "out";
			
	}
		
	createOutputFile(outputFilePath);

	try{
		TLGCApp lgc(inputFilePath, outputFilePath);
		return lgc.exec();	
	}
	catch (std::exception& ex){
		cout << ex.what()<<endl<<endl;		
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
		if (creat(outputFileDirectory.c_str(),00007) == -1) {
			std::cout << "Output directory does not exist and could not be created" << endl;
		}
#else
	CreateFile(outputFileDirectory.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD ftyp = GetFileAttributesA(outputFileDirectory.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES){ //This means that the directory still does not exist, using default one
		throw runtime_error("Output directory does not exist and could not be created");
	}

#endif
}

std::string getPathDir(std::string fPath)
{
	std::size_t found = fPath.find_last_of("/\\");
	return fPath.substr(0, found);
}

std::string getPathFile(std::string fPath)
{
	std::size_t found = fPath.find_last_of(".");
	return fPath.substr(0, found);
}
