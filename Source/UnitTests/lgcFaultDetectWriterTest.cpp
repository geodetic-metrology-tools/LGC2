///////////////////////////////////////////////////////////////
//test
//03/2003
///////////////////////////////////////////////////////////////

//#include <iostream>
//#include <string>
//using namespace std;

#include "TAStreamFormatter.h"
#include "TSeparatedFormatTStream.h"
#include "TLGCFileReader.h"
#include "TLGCDataSet.h"
#include "TLGCProject.h"
#include "TLGCApplication.h"

// includes for ObsMkr
#include "TObservationMaker.h"

// includes for LSCalcObsMkr
#include "TLSCalcObservationMaker.h"

// includes for input matrices filling
#include "TLSInputMatricesFiller.h"
#include "TLSLocalSysContribGenFactory.h"
#include "TLSGeoSysContribGenFactory.h"
#include "TLSInputMatrices.h"


// includes for calculation
#include "TLSParametricMtdComputer.h"
#include "TLSResultsMatricesExtractor.h"
#include "TLSResultsMatrices.h"

// includes for writer
#include "TLGCFaultDetectionFileWriter.h"
#include "TLGCFileWriter.h"

#include "LSCalcDataSet.h"
#include	<cmath>
#include	<nag.h>
#include	<nagg01.h>


void main()
{
/*	static NagError fail;
	fail.print = false;
	real wmax = nag_deviates_normal(Nag_UpperTail,LITERAL(0.1),&fail);

	cout << wmax << endl;*/

	//////////////////////////////////////////////////////////////
	//Read a input LGCFilevv with the load function of TLGCProject
	//////////////////////////////////////////////////////////////

	const char * LGCFile;
	LGCFile = "C:\\Temp\\optimisation\\fault\\ALLOBS1.inp";

	// settings for file parameters
	TFileParameters fp;
	fp.setFileName(LGCFile);

	cout << fp.getPath() << "\\" << fp.getName() << endl;
	cout << fp.defined() << endl;

	// creating an (empty) application and a project
	//TLGCApplication application;
	TLGCProject project;

	project.load(fp);

	// Observations and ls calc observations making & Computation
	project.getResultsView();

	cout << "display the chi-test upper limit: " << project.getLSCalcDataSet()->getChiUpLimit() << endl;
	cout << "display the chi-test lower limit: " << project.getLSCalcDataSet()->getChiLoLimit() << endl;

	// write output file
	cout << "saving results" << endl;
	TLGCFileWriter output;
	string pathN = "C:\\Temp\\optimisation\\fault\\output.txt";
	TFileParameters filePara;
	filePara.setFileName(pathN);
	project.getDataSet()->setFileParams(filePara);
	output.writeFile(project.getDataSet(),project.getLSCalcDataSet());
	
	// write reliability file
	bool reliability = project.getCalcParams()->isFaultDetectToBeSaved();
	if (reliability) {
		cout << "saving reliability parameters" << endl;
		TLGCFaultDetectionFileWriter fault(project.getCalcParams());
		string outputFile = "C:\\Temp\\optimisation\\fault\\fault.txt";
		TFileParameters fileParam;
		fileParam.setFileName(outputFile);
		project.getDataSet()->setFileParams(fileParam);
		fault.writeResults(project.getDataSet(),project.getLSCalcDataSet());
		}
	else
		cout << "Keyword recognition didn't work" << endl;
	
	cout << project.getError() << endl;
	return;
}
