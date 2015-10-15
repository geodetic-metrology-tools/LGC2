#include <iostream>
#include "TLGCApp.h"

#ifdef __linux__
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	const std::string defaultInputFilePath = "/root/Temp/input.inp"; 
	const std::string defaultOutputFilePath = "/root/Temp/output.out";
#else
	#include <windows.h>
	const std::string defaultInputFilePath = "C:/Temp/input.inp"; 
	const std::string defaultOutputFilePath = "C:/Temp/output.out";
#endif


int main( int argc,  char *argv[]){

	std::string inputFilePath;
	std::string outputFilePath;

	bool optionUsed = false;

	for (int i=0 ; i<argc ; i++)
	{
		if(argv[i][0] == '-')
		{
			int retVal = 0;
			switch (argv[i][1])
			{
				// run command line interface
				case 'i':
				case 'I':					
				{
					if(!argv[i+1]) { // -I/-i option used, but the input file path was not specified
						std::cout << "Input file was not specified, using default path: " + defaultInputFilePath<< endl; 
						break;
					}

					inputFilePath = argv[i+1]; 

					//Possibly check if it has currect extension, exists, etc
				}
				break;

				case 'o':
				case 'O':
				{
					if(!argv[i+1]) { // -O/-o option used, but the output file path was not specified
							std::cout << "Output file was not specified, using default path: " + defaultOutputFilePath<< endl;
							break;
					}

					outputFilePath = argv[i+1];  //store the output file path, if there is something ot read

					std::string outputFileDirectory = outputFilePath.substr(0, outputFilePath.find_last_of('\\'));
					//If does not exist, needs to be created
					#ifdef __linux__
						struct stat st = {0};
						if (stat(outputFileDirectory.c_str(), &st) == -1)
							if(mkdir(outputFileDirectory.c_str(), 0700) == -1) {
								std::cout << "Output directory does not exist and could not be created, using default one: " + defaultOutputFilePath<< endl;
								outputFileDirectory = defaultOutputFilePath;
							}
					#else
						CreateDirectory(outputFileDirectory.c_str(), NULL);

						DWORD ftyp = GetFileAttributesA(outputFileDirectory.c_str());
						if (ftyp == INVALID_FILE_ATTRIBUTES){ //This means that the directory still does not exist, using default one
							std::cout << "Output directory does not exist and could not be created, using default one: " + defaultOutputFilePath<< endl;
							outputFileDirectory = defaultOutputFilePath;
						}
						//BOOST solution - requires compilation of boost
					/*
						boost::filesystem::create_directories(outputFileDirectory);

						if(!boost::filesystem::is_directory(outputFileDirectory)) //Check if it really exists, could be complete nonsense, if not use default
							outputFileDirectory = defaultOutputFilePath;
					*/
					#endif
					break;
				}
			}
		}
	}

	if(inputFilePath == "" && outputFilePath == ""){ // Use default if not argument used
		inputFilePath = defaultInputFilePath;
		outputFilePath = defaultOutputFilePath;
	}
	else if (inputFilePath == "") // If only output file path specified, set default input file path
		inputFilePath = defaultInputFilePath;
	else if (outputFilePath == "") // If only input file path specified, output file path is the same with different extension
		outputFilePath = inputFilePath.substr(0,inputFilePath.length() - 3) + "out";


	try{
		TLGCApp lgc(inputFilePath, outputFilePath);
		return lgc.exec();	
	}
	catch (std::exception& ex){
		cout << ex.what()<<endl<<endl;		
	}

	return 1;
}