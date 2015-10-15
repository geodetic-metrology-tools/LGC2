///////////////////////////////////////////////////////////////
//test observation Maker
//01/2003
///////////////////////////////////////////////////////////////

//#include <iostream>
//#include <string>
//using namespace std;

#include "TLGCApplication.h"
#include "TLGCProject.h"
#include "TObservationMaker.h"
#include "TLSCalcObservationMaker.h"
#include "TLGCFileWriter.h"
#include "TFileParameters.h"

void main()
{
	const char * LGCFile;
	LGCFile = "C:\\workspace\\LGC\\exempleclic.inp";

	// settings for Data parameters
	TDataParameters dp;
	dp.setRefFrame(TDataParameters::kCCS);
	dp.setCoordSys(TCoordSysFactory::k3DCartesian);
	dp.setAngUnits(TAngle::kGons);
	dp.setLenUnits(TLength::kMetres);
	dp.setAnglePrecision(TObservationFormat::k10Microgons);
	dp.setLengthPrecision(TObservationFormat::k10Micrometres);
	dp.setCoordPrecision(TPointFormat::k10Millimetres);

	// settings for file parameters
	TFileParameters fp;
	fp.setFileName(LGCFile);

	// creating an (empty) application and a project
	TLGCProject project;

	// use of load function
	project.load(fp);

	//data set
	TLGCDataSet* data = new TLGCDataSet();
	data =project.getDataSet();

	//Observation set
	ObservationSet obs;

	//test du maker
	TObservationMaker maker;

	bool B = maker.processData(*data, obs);
	cout<<"process result : "<<B<<endl<<endl;

	string errorMessage = maker.getError();
	cout<<"error : "<<errorMessage<<endl;

	//ls calc observation
	LSCalcDataSet dataset; 
	TLSCalcObservationMaker LSMaker;
	LSMaker.processData(obs,dataset);









	TFileParameters fileParam;
	string outputFile = "C:\\workspace\\LGC\\out.txt";
	fileParam.setFileName(outputFile);
	data->setFileParams(fileParam);
	//writer
	TLGCFileWriter writer;

	writer.writeFile(data, &dataset);

	cout<<writer.getError()<<endl;


	return;
	

};