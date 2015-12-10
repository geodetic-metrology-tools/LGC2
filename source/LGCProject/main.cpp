#include <iostream>
#include "TLGCApp.h"

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

int main( int argc,  char *argv[]){

	//std::this_thread::sleep_for(std::chrono::seconds(10));

	std::string inputFilePath;
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
					if(!argv[i+1]) { // -I/-i option used, but the input file path was not specified
						std::cout << "Input file was not specified"<< endl; 
						break;
					}

					inputFilePath = argv[i+1]; 
					break;
				}

				case 'o':
				case 'O':
				{
					if (!argv[i + 1]) { // -O/-o option used, but the output file path was not specified
						std::cout << "Output file was not specified" << endl;
						break;
					}
					outputFilePath = argv[i + 1];  //store the output file path, if there is something ot read
					std::cout << outputFilePath << endl;
					break;
				}
			}
		}
	}



	if(inputFilePath == "" || outputFilePath == ""){ 
		if (inputFilePath == "")
			throw runtime_error("Error, the input file is not found");
		else if (outputFilePath == "")
			outputFilePath = inputFilePath.substr(0, inputFilePath.length() - 4) + "out";
	}
		
	std::cout << outputFilePath << endl;
	createOutputFile(outputFilePath);

	try{
		std::cout << inputFilePath << endl;
		std::cout << outputFilePath << endl;
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
		if (touch(outputFileDirectory.c_str(), 0700) == -1) {
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