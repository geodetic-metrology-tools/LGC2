// TLGCApplication.cpp
//
/** Class for a LGC application : 
file reading, interaction with LGCproject, results writing */
//
// Patterns:
//
// 
// Copyright 2000-2008 M.Jones CERN TS/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
//#include "QFile.h"
#include <sstream>
#include <string>
using namespace std;

#define SUCCESS 0;
#define INIT_FAILURE 1;
#define FAILURE 2;

#include  "SurveryLibVersion.h"
#include  "LGCVersion.h"
#include  "TStreamFormatterFactory.h"
#include  "T3DLocalRefFrame.h"
#include  "TSeparatedFormatTStream.h"
#include  "TFixedColFormatTStream.h"
#include  "TLGCApplication.h"
//#include  "TLGCFileWriter.h"
#include  "TLGCSimResults.h"
//#include  "TLGCSimulatedObsFileWriter.h"
#include  "TPunchFileWriter.h"
#include  "TMesFileWriter.h"
#include  "TRadFileWriter.h"
#include  "TCooFileWriter.h"
#include  "TResultsFileWriter.h"
#include  "TFautFileWriter.h"
#include  "TResSimFileWriter.h"
#include  "TSimFileWriter.h"
#include  "TDataParameters.h"
#include  "TDefAFileWriter.h"

////////////////////////////////////////////////////////////////


//ClassImp(TLGCApplication)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
const string TLGCApplication::fCopyright = "Copyright 2003-2013, CERN SU. All rights reserved.";
	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//////////////////////
// Default constructor
//////////////////////
TLGCApplication::TLGCApplication(int argc, char** argv)
:fProject(0)
{
	fProgramError = false;
	fUseGUI = false;

	fStream = 0;

	fArgc = argc;
	fArgv = argv;

	//fNumOfSimToDo = 0;

	getCommandLine();
}

/////////////
// Destructor
/////////////
TLGCApplication::~TLGCApplication()
{
	delete fProject;
	if(fStream !=0)
	{
		delete fStream;
	}
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
//execute the application
int TLGCApplication::exec()
{
	int result = SUCCESS;

	if (!fProgramError)
	{
		if (usingGUI())
		{ //run the application GUI
			result = runGUI();
		}
		else
		{ //run the application (command line)
			result = runApp();
		}
	}
	else 
		result = INIT_FAILURE;

	return result;
}


// return the software version id string
const string TLGCApplication::getProgId()
{
	std::stringstream id;
	id << "LGC++ " << getLGCVersion() << " compiled against SurveyLib " << getSurveyLibVersion() << " on " <<   __DATE__ ;
	return id.str();
}


// return the software copyright string
const string TLGCApplication::getCopyright()
{
	return fCopyright;
}



//////////////////////////////////////////////////////////////////////
// Private Member Functions
//////////////////////////////////////////////////////////////////////
// extract name file parameters from the command line parameters
TFileParameters  TLGCApplication::getNameFileParams()
{
	TFileParameters nameFile;
	int i=1;

	// search the command line arguments for the project file names file
	// (project file)
	while (i < fArgc && !fProgramError)
	{
		if (fArgv[i][0] == '-')
		{
			switch (fArgv[i][1])
			{
				case 'f': //input file path and name
					if (argsChkOK(i,1))
					{
						fProgramError = !nameFile.setFileName( string(fArgv[i+1]) );
						++i;
					}
					break;
				default:
					break;
			}
		}
		i++;
	}

	// if the project file names file is not specified use the default value
	if(!(nameFile.defined()))
	{
		nameFile.setFileName("C:\\temp\\file.nam");
	}

	return nameFile;
}



// extract project input file parameters from the name file
TFileParameters  TLGCApplication::getInputFileParams()
{
	TFileParameters fileParam;
	fstream* fileStream = new fstream(fProject->getNameFile().getFileName().c_str(), ios_base::in);
	
	//read first two lines of the name file
	// file path and name on the first line
	string fileName = readLine(fileStream);
	// file extension on the second line
	string fileExt = readLine(fileStream);

	// store the full path and filename, and note any errors
	fProgramError = !fileParam.setFileName(fileName + "." + fileExt);

	delete fileStream;
	return fileParam;
}


// extract result file parameters from the name file
TFileParameters  TLGCApplication::getResultsFileParams()
{
	TFileParameters fileParam;
	fstream* fileStream = new fstream(fProject->getNameFile().getFileName().c_str(), ios_base::in);

	// skip the first two lines of the file
	readLine(fileStream);
	readLine(fileStream);
	//read third and fourth lines of the name file
	// file path and name on the third line
	string fileName = readLine(fileStream);
	// file extension on the fourth line
	string fileExt = readLine(fileStream);
		
	// store the full path and filename, and note any errors
	fProgramError = !fileParam.setFileName(fileName + "." + fileExt);

	delete fileStream;
	return fileParam;
}

// extract result file parameters from the name file
TFileParameters  TLGCApplication::getDeformFileParams()
{
	TFileParameters fileParam;
	fstream* fileStream = new fstream(fProject->getNameFile().getFileName().c_str(), ios_base::in);

	//read third and fourst line the name file
	// skip the first four lines of the file
	readLine(fileStream);
	readLine(fileStream);
	readLine(fileStream);
	readLine(fileStream);
	// file path and name on the fifth line
	string fileName = readLine(fileStream);
		
	fProgramError = !fileParam.setFileName(fileName);

	delete fileStream;
	return fileParam;
}


/////////////////////////////////////
// read a line from a file (file.nam)
/////////////////////////////////////
string TLGCApplication::readLine(fstream* fileStream)
{ // method similar to TAStreamFormatter::readLine()
	string result ="";
	const int buf_size=256;
	char c[buf_size];
	int pos=0;

	c[pos]= static_cast<char>(fileStream->get());

	if(c[pos]==EOF)
	{//check if there is a line
		result = "end of file";
	}
	else
	{		
		while( c[pos]!=EOF && (c[pos]!=*("\n")) && pos < buf_size)
		{//read character by character
			result +=c[pos];
			pos++;
			c[pos]=static_cast<char>(fileStream->get());
		}
		//remove the last char (eof or \n)
	}
	return result;
}


// read the project file names file
int TLGCApplication::load(const TFileParameters& fileParams)
{
	TFileParameters inputFile, resultsFile, deformFile;

	//pass the project file name file parameters to the project
	fProject->setNameFileParams(fileParams);

	//read the project file name file and pass the file parameters to the project
	inputFile = getInputFileParams();
	resultsFile = getResultsFileParams();
	fProject->setResultsFileParams(resultsFile);
	deformFile = getDeformFileParams();
	fProject->setDeformFileParams(deformFile);

	//load the input file data in the project
	int error = fProject->load(inputFile);
	
	return error;
}


// return true if the application is using a GUI
bool TLGCApplication::usingGUI()
{ // to be implemented
	return fUseGUI; 
}

// check number and type of arguments
bool TLGCApplication::argsChkOK(int currArg, int numParams)
{
	//check if there are enougth argument on the command line
	// check the overall number of argument
	if ( (currArg + numParams + 1) > fArgc )
	{
		fProgramError = true;
	}
	else
	{
		//check the number argument following the current argument
		int i = currArg + 1;
		while(i<(currArg + numParams + 1) && !fProgramError)
		{
			//check if argument is a new flag
			if(fArgv[i][0] == '-' && !isdigit(fArgv[i][1]))
			{
				fProgramError = true;
			}
			++i;
		}
	}
	return !fProgramError;
}


// get the elements of the command line
void TLGCApplication::getCommandLine()
{
	int i = 1;
	while (i < fArgc && !fProgramError)
	{
		if(fArgv[i][0] == '-')
		{
			switch (fArgv[i][1])
			{
				case 'x': //indicate the GUI is not used
					fUseGUI = false;
					break;
				default:
					break;
			}
		}
		i++;
	}
	return;
}



// run the application
int TLGCApplication::runApp()
{
	int result = FAILURE;

	//create a new project
	fProject = new TLGCProject();

	// if the project has been created succesfully
	if (fProject != 0)
	{
		//read name file and load input data
		TFileParameters nameFile = getNameFileParams();
		
		// if the file loads without problem then compute the results
		if( this->load(nameFile) == 0 )
		{
			// compute the least squares results from the project data
			fProject->computeLSResults();
			
			if( fProject->getError() == "" )
				result = SUCCESS;
		}

		// save the results
		this->saveLSResults(fProject);
	}

	return result;
}

// run the GUI
int TLGCApplication::runGUI()
{
	return FAILURE;
}


void TLGCApplication::makeSaveStream(TLGCDataSet* ds, string separator, TAStreamFormatter::ETextFormat textFormat)
{
	if(fStream !=0)
	{
		delete fStream;
	}

	TStreamFormatterFactory* formatterFactory = TStreamFormatterFactory::instance();
	fStream = formatterFactory->getFormatter(ds, textFormat, separator);

	if (ds->getDataParams().getRefFrameEnumerator() == TRefSystemFactory::kLocalRefFrame)
	{
		//points are defined in a local system
		fStream->setReferenceFrame(ds->getDataParams().getRefFrame());
	}
	else
	{
		//points are in a CERN sytem and transform in CCS
		fStream->setReferenceFrame(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		fStream->setCoordSys(TCoordSysFactory::k3DCartesian);
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
// Save all the results files requested for a standard computation project
//////////////////////////////////////////////////////////////////////////
void TLGCApplication::saveLSResults(TLGCProject *project)
{
	// write different results dependent upon the project type
	if (fProject->isSimulation())
	{
		//save the simulated LS results
		saveSimulatedLSResults(project);
	}
	else
	{//save the LS results
		// write the results file
		writeStdResultsFile(project);

		// if no errors occurred in the calculation output other requested files
		if (project->getError() == "")
		{
			// if requested save a punch file
			if(project->getOutputOptions()->isPunchfileToBeSaved() != TLGCOutputOptions::kNo)
			{
				writePunchFile(project);
			}
		
			// if requested save the Coo file
			if( fProject->getOutputOptions()->isCoofileToBeSaved() == TLGCOutputOptions::kCOO )
			{
				writeCooFile(project);
			}

			// if requested save the Mes file
			if( fProject->getOutputOptions()->isMesfileToBeSaved() == TLGCOutputOptions::kMES )
			{
				writeMesFile(project);
			}

			// if required save the rad file
			if( (fProject->getDataSet()->getRadOffCnstrDimension()) != 0 )
			{
				writeRadFile(project);
			}
			if (project->getOutputOptions()->isDeformationAnalysisToBeCreated())
			{
				writeDefAFile(project);
			}
		}
	}

	// if no errors occurred then write the results files in common
	if (project->getError() == "")
	{
		// if requested save the error analysis file (Faut)
		if (project->getCalcParams()->isFaultDetectToBeSaved())
		{
			writeFautFile(project);
		}
	}

	return;
}

void TLGCApplication::writeDefAFile(TLGCProject *project)
{
	TFileParameters params;
	string name = project->getResultsFileParams().getFileName();
	int i;
	if (!name.empty())
	{
		//find the end of the path
		i = name.find_last_of( '.' );

		// assign the filename
		name = name.substr(0, i) + ".def";
	}
	params.setFileName(name);
	TLGCDataSet* data = project->getDataSet();
	data->setFileParams(params);
	makeSaveStream(data, data->getResultsSeparator(), TAStreamFormatter::kColumnFormat);
	fStream->setSeparator("");

	TDefAFileWriter* file = new TDefAFileWriter(fStream, project);

	// if there is still no error the calculation has converged so save the results
	if (project->getError() == "")
	{//le calcul converge
		file->writeFile(*project);
	}
	else
	{//erreur ou le calcul ne converge pas		
		file->writeFile(project->getError());		
	}

	delete file;
	return;
}

////////////////////////////////////////////////////////////////////////////
// Save all the results files requested for a simulation computation project
////////////////////////////////////////////////////////////////////////////
// calculate and save results
void TLGCApplication::saveSimulatedLSResults(TLGCProject *project)
{
	// initialise variables and set up file writer
	TLGCDataSet* data = project->getDataSet();
	data->setFileParams(project->getResultsFileParams());
	makeSaveStream(data, data->getResultsSeparator(), data->getResultsFileFormat());
	TResSimFileWriter* writer = new TResSimFileWriter(fStream, project);

	// write out the different components of the output file
	writer->writeFile(*project);

	if(project->getError() == "")
	{
		// if requested save a simulated input file
		if (project->getOutputOptions()->isSimInputToBeSaved() == true)
		{
			writeSimInputFile(project);
		}
	}

	delete writer;

	return;
}


////////////////////////////////////////////////
// write the results file for a standard project
////////////////////////////////////////////////
void TLGCApplication::writeStdResultsFile(TLGCProject *project)
{
	project->getDataSet()->setFileParams(project->getResultsFileParams());
	TLGCDataSet* data = project->getDataSet();
	makeSaveStream(data, data->getResultsSeparator(), data->getResultsFileFormat());
	TResultsFileWriter* file = new TResultsFileWriter (fStream, project);

	// if there is still no error the calculation has converged so save the results
	if (project->getError() == "")
	{//le calcul converge
		file->writeFile(*project);
	}
	else
	{//erreur ou le calcul ne converge pas		
		file->writeFile(data, project->getError());		
	}

	delete file;
	return;
}


// write and save the error analysis file
void	TLGCApplication::writeFautFile(TLGCProject *project)
{
	LSCalcDataSet *lsCalcDS = project->getLSCalcDataSet();

	if (lsCalcDS->isConvergenceOk() == true
		&& lsCalcDS->getDimensions().UIndex != 0)
	{
		TFileParameters fileParam;
		
		int i;
		string name = project->getResultsFileParams().getFileName();
		if(!name.empty())
		{
			i = name.find_last_of('.');
			name = name.substr(0,i) + ".err";
		}
		fileParam.setFileName(name);
		
		TLGCDataSet* data = project->getDataSet();
		data->setFileParams(fileParam);
		makeSaveStream(data, data->getPunchSeparator(), data->getPunchFileFormat());
		TFautFileWriter* ffile = new TFautFileWriter(fStream, project);

		ffile->writeResults(*(project->getLSCalcDataSet()), *project);
		delete ffile;
	}
	return;
}

// write and save the punch file
void	TLGCApplication::writePunchFile(TLGCProject *project)
{
	TFileParameters punch;
	int  i;
	string name = project->getResultsFileParams().getFileName();
	// make sure the filename is not empty
	if( !name.empty() )
	{
		//find the end of the path
		i = name.find_last_of( '.' );

		// assign the filename
		name = name.substr(0, i) + ".pun";
	}
	punch.setFileName(name);

	TLGCDataSet* data = project->getDataSet();
	data->setFileParams(punch);
	makeSaveStream(data, data->getPunchSeparator(), data->getPunchFileFormat());

	TPunchFileWriter writer(fStream, project);
	writer.writeFile(data, project->getLSCalcDataSet());

	return;
}

// write and save the coo file
void	TLGCApplication::writeCooFile(TLGCProject *project)
{
	TFileParameters coo;
	int  i;
	string name = project->getResultsFileParams().getFileName();
	// make sure the filename is not empty
	if( !name.empty() )
	{
		//find the end of the path
		i = name.find_last_of( '.' );

		// assign the filename
		name = name.substr(0, i) + ".coo";
	}
	coo.setFileName(name);

	TLGCDataSet* data = project->getDataSet();
	data->setFileParams(coo);
	makeSaveStream(data, data->getPunchSeparator(), data->getPunchFileFormat());
	TCooFileWriter writerCoo (fStream, project);
	writerCoo.writeFile(data, project->getLSCalcDataSet());

	return;
}

// write and save the measurements file
void	TLGCApplication::writeMesFile(TLGCProject *project)
{
	TFileParameters mes;
	int  i;
	string name = project->getResultsFileParams().getFileName();
	// make sure the filename is not empty
	if( !name.empty() )
	{
		//find the end of the path
		i = name.find_last_of( '.' );

		// assign the filename
		name = name.substr(0, i) + ".mes";
	}
	mes.setFileName(name);

	TLGCDataSet* data = project->getDataSet();
	data->setFileParams(mes);
	makeSaveStream(data, data->getPunchSeparator(), data->getPunchFileFormat());
	TMesFileWriter writerMes (fStream, project);
	
	//writerMes.setOutputOptions(outOptions);
	writerMes.writeFile(data, project->getLSCalcDataSet());

	return;
}

// write and save the radial offsets file
void	TLGCApplication::writeRadFile(TLGCProject *project)
{
	TFileParameters rad;
	int  i;
	string name = project->getResultsFileParams().getFileName();
	// make sure the filename is not empty
	if( !name.empty() )
	{
		//find the end of the path
		i = name.find_last_of( '.' );

		// assign the filename
		name = name.substr(0, i) + ".rad";
	}
	rad.setFileName(name);

	TLGCDataSet* data = project->getDataSet();
	data->setFileParams(rad);
	makeSaveStream(data, data->getPunchSeparator(), data->getPunchFileFormat());
	TRadFileWriter writerRad (fStream, project);

	//writerRad.setOutputOptions(outOptions);
	writerRad.writeFile(data, project->getLSCalcDataSet());

	return;
}

// write and save the simulated input file
void	TLGCApplication::writeSimInputFile(TLGCProject *project)
{
	TFileParameters input = fProject->getInputFileParams();
	TFileParameters simOut;
	int  i;
	string name = project->getResultsFileParams().getFileName();
	// make sure the filename is not empty
	if( !name.empty() )
	{
		//find the end of the path
		i = name.find_last_of( '.' );

		// assign the filename
		name = name.substr(0, i) + ".sim";
	}
	simOut.setFileName(name);

	TLGCDataSet* data = project->getDataSet();
	data->setFileParams(simOut);
	makeSaveStream(data, "   ", data->getPunchFileFormat());
	TSimFileWriter simWriter (fStream, project);

	simWriter.writeSimulatedObsFile(data, input, project->getLSCalcDataSet());

	return;
}





	
