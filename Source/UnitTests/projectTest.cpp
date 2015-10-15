///////////////////////////////////////////////////////////////
//test
//01/2003
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
#include "TLGCSimulatedObsFileWriter.h"
//#include "TLSCalcDataSet.h"
#include "TGeodeticRefFrame.h"


void main()
{
	
//////////////////////////////////////////////////////////////
//Read a input LGCFilevv with the load function of TLGCProject
//////////////////////////////////////////////////////////////

	const char * LGCFile;
	LGCFile = "C:\\temp\\allobs.inp";

	// settings for Data parameters
	/*TDataParameters dp;
	dp.setRefFrame(TDataParameters::kCCS);
	dp.setCoordSys(TCoordSysFactory::k3DCartesian);
	dp.setAngUnits(TAngle::kGons);
	dp.setLenUnits(TLength::kMetres);
	dp.setAnglePrecision(6);
	dp.setLengthPrecision(5);
	dp.setCoordPrecision(TPointFormat::k10Millimetres);*/

	// settings for file parameters
	TFileParameters fp;
	fp.setFileName(LGCFile);

	cout << fp.getPath() << "\\" << fp.getName() << endl;
	cout << fp.defined() << endl;


	// creating an (empty) application and a project
	//TLGCApplication application;
	TLGCProject project;

	// use of load function
	project.load(fp);


	
	// print the title
	cout << "Title :\n" << TLGCProject::getTitle() << endl;
	cout << "Output Options : GEODE : " << project.getOutputOptions()->usingGeodeFiles() << endl;
	cout << "                 coord. precision : " << project.getDataSet()->getDataParams().getCoordPrecision() << endl;
	cout << "                 dist. precision : " << project.getDataSet()->getDataParams().getLengthPrecision() << endl;
	cout << "                 angle. precision : " << project.getDataSet()->getDataParams().getAnglePrecision() << endl;
	cout << "Calculation Parameters : Sigma A Priori : " << project.getCalcParams()->usingSigmaAPriori() << endl;
	cout << "                         Convergence Criteria : " << project.getCalcParams()->getConvCriteria() << endl;



	// output of points, comments,...
	PointIterator i, j;

	i=project.getDataSet()->getWorkingPoints()->getPointsBeginIterator();
	j=project.getDataSet()->getWorkingPoints()->getPointsEndIterator();

/*	while(i!=j)
	{
		TSpatialStatus::ESpatialStatus status = i->getPosition().getObjectStatus();
		string name=i->getPtName();
		real X =i->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian).getX().getMetresValue();
		real Y =i->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian).getY().getMetresValue();
		real Z =i->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian).getZ().getMetresValue();
		real dist=i->getDist();
		int id=i->getName().getId();
		string com=i->getName().getComment();
		string head=i->getHeaderComment();
		string eol = i->getEOLComment();
		cout.precision(10);
		cout<<status<<endl;
		cout<<name<<", "<<X<<", "<<Y<<", "<<Z<<endl;
		cout<<"dist: "<<dist<<", id: "<<id<<", com: "<<com<<endl;
		cout<<"head: "<<head<<endl;
		cout<<"eol: "<<eol<<endl<<endl;
		i++;
	}


	// output of measurements
	// horizontal angles
	TheodStIterator iter, iterEnd;
	iter = project.getDataSet()->getWorkingStations()->getTheodStBeginIterator();
	iterEnd = project.getDataSet()->getWorkingStations()->getTheodStEndIterator();
	cout << "Horizontal Angles : " << endl;


	while (iter != iterEnd)
	{	
		cout << iter->getSetup() << endl;

		HorAngROMIterator ROMIter, ROMIterEnd;
		ROMIter = iter->getHorAngROMBeginIterator();
		ROMIterEnd = iter->getHorAngROMEndIterator();
		int i= 1;

		while (ROMIter != ROMIterEnd)
		{
			cout << "ROM n." << i << endl;

			HorAngMeasIterator MeasIter, MeasIterEnd;
			MeasIter = ROMIter->getHorAngMeasBeginIterator();
			MeasIterEnd = ROMIter->getHorAngMeasEndIterator();

			while (MeasIter != MeasIterEnd)
			{
				cout << "Header comment :"   << MeasIter->getHeaderComment() << endl
					<< iter->getStationedPoint().getName() << "   " << MeasIter->getTargetPoint().getName() << "   "<<
					MeasIter->getAngleValue().getGonsValue() << endl
					<< "sigma : " << MeasIter->getSigma().getGonsValue() 
					<< "   constant : "<< iter->getAngleConst().getValue().getGonsValue() << "(" << iter->getAngleConst().getValue().getStatus() << ")" 
					<< "   Id. & comment : " << MeasIter->getId() << "   " << MeasIter->getComment() << endl;

				MeasIter++;
			}

			ROMIter++;
			i++;
		}

		iter++;
	}




	// Zenithal distances
	iter = project.getDataSet()->getWorkingStations()->getTheodStBeginIterator();
	cout << "Zenithal Distances : " << endl;

	while (iter != iterEnd)
	{	
		cout << iter->getSetup() << endl;

		ZenithDistROMIterator ROMIter, ROMIterEnd;
		ROMIter = iter->getZenithDistROMBeginIterator();
		ROMIterEnd = iter->getZenithDistROMEndIterator();

		int i = 1;

		while (ROMIter != ROMIterEnd)
		{
			cout << "ROM n." << i << endl;
			ZenithDistMeasIterator MeasIter, MeasIterEnd;
			MeasIter = ROMIter->getZenithDistMeasBeginIterator();
			MeasIterEnd = ROMIter->getZenithDistMeasEndIterator();

			while (MeasIter != MeasIterEnd)
			{
				cout  << "Header comment :"   << MeasIter->getHeaderComment() << endl
					<< iter->getStationedPoint().getName() << "   " << MeasIter->getTargetPoint().getName() << "   "<<
					MeasIter->getAngleValue().getGonsValue() << endl
					<< "sigma : " << MeasIter->getSigma().getGonsValue() 
					<< "   Hi : "<<	iter->getInstrumentHeight().getMetresValue() << "(" << iter->getInstHeightStatus() << ")" 
					<< "   Hp : " << MeasIter->getPrismeHeight().getMetresValue() << "(" << MeasIter->getPrismeHeightStatus() << ")" << endl
					<< "   Id. & comment : " <<	MeasIter->getId() << "   " << MeasIter->getComment() << endl << endl;

				MeasIter++;
			}

			cout << endl;
			ROMIter++;
			i++;
		}

		iter++;
	}




	// Horizontal Distances
	DistStIterator DSiter = project.getDataSet()->getWorkingStations()->getDistStBeginIterator();
	DistStIterator DSiterEnd = project.getDataSet()->getWorkingStations()->getDistStEndIterator();
	cout << "Horizontal Distances : " << endl;

	while (DSiter != DSiterEnd)
	{	
		cout << DSiter->getSetup() << endl;

		HorDistROMIterator ROMIter, ROMIterEnd;
		ROMIter = DSiter->getHorDistROMBeginIterator();
		ROMIterEnd = DSiter->getHorDistROMEndIterator();

		int i = 1;
		
		while (ROMIter != ROMIterEnd)
		{
			cout << "ROM n." << i << endl;

			HorDistMeasIterator MeasIter, MeasIterEnd;
			MeasIter = ROMIter->getDistMeasBeginIterator();
			MeasIterEnd = ROMIter->getDistMeasEndIterator();

			while (MeasIter != MeasIterEnd)
			{
				cout  << "Header comment :"   << MeasIter->getHeaderComment() << endl
					<< DSiter->getStationedPoint().getName() << "   " << MeasIter->getTargetPoint().getName() << "   "<<
					MeasIter->getDistValue().getMetresValue() << endl
					<< "sigma : " << MeasIter->getSigma().getMMetresValue() 
					<< "   sigma ppm : " << MeasIter->getSigmaPpm().getMMetresValue() 
					<< "   constant : " << MeasIter->getMeasConst().getValue().getMetresValue() 
					<< "(" << MeasIter->getMeasConst().getStatus() << ")" 
					<< "   Id. & comment : " <<	MeasIter->getId() << "   " << MeasIter->getComment() << endl;

				MeasIter++;
			}

			cout << endl;
			ROMIter++;
			i++;
		}

		DSiter++;
	}



	// Spatial Distances
	DSiter = project.getDataSet()->getWorkingStations()->getDistStBeginIterator();
	cout << "Spatial Distances : " << endl;

	while (DSiter != DSiterEnd)
	{	
		cout << DSiter->getSetup() << endl;

		SpatialDistROMIterator ROMIter, ROMIterEnd;
		ROMIter = DSiter->getSpatialDistROMBeginIterator();
		ROMIterEnd = DSiter->getSpatialDistROMEndIterator();

		int i = 1;
		
		while (ROMIter != ROMIterEnd)
		{
			cout << "ROM n." << i << endl;

			SpatialDistMeasIterator MeasIter, MeasIterEnd;
			MeasIter = ROMIter->getDistMeasBeginIterator();
			MeasIterEnd = ROMIter->getDistMeasEndIterator();

			while (MeasIter != MeasIterEnd)
			{
				cout  << "Header comment :"   << MeasIter->getHeaderComment() << endl
					<< DSiter->getStationedPoint().getName() << "   " << MeasIter->getTargetPoint().getName() << "   "<<
					MeasIter->getDistValue().getMetresValue() << endl
					<< "sigma : " << MeasIter->getSigma().getMMetresValue() 
					<< "   sigmaPpm : " << MeasIter->getSigmaPpm().getMMetresValue()
					<< "   Hi : "<<	DSiter->getInstrumentHeight().getMetresValue() << "(" << DSiter->getInstHeightStatus() << ")" 
					<< "   Hp : " << MeasIter->getPrismHeight().getMetresValue() << "(" << MeasIter->getPrismHeightStatus() << ")" << endl
					<< "   constant" << " (" << MeasIter->getMeasConst().getConstantName() << ") : " << MeasIter->getMeasConst().getValue().getMetresValue() 
					<< "(" << MeasIter->getMeasConst().getStatus() << ")" 
					<< "   Id. & comment : " <<	MeasIter->getId() << "   " << MeasIter->getComment() << endl;

				MeasIter++;
			}

			cout << endl;
			ROMIter++;
			i++;
		}

		DSiter++;
	}


	// Vertical Distances
	LevelStIterator LSiter = project.getDataSet()->getWorkingStations()->getLevelStBeginIterator();
	LevelStIterator LSiterEnd = project.getDataSet()->getWorkingStations()->getLevelStEndIterator();
	cout << "Vertical Distances : " << endl;

	while (LSiter != LSiterEnd)
	{	
		cout << LSiter->getSetup() << endl;

		VertDistROMIterator ROMIter, ROMIterEnd;
		ROMIter = LSiter->getVertDistROMBeginIterator();
		ROMIterEnd = LSiter->getVertDistROMEndIterator();

		int i = 1;
		
		while (ROMIter != ROMIterEnd)
		{
			cout << "ROM n." << i << endl;

			VertDistMeasIterator MeasIter, MeasIterEnd;
			MeasIter = ROMIter->getVertDistMeasBeginIterator();
			MeasIterEnd = ROMIter->getVertDistMeasEndIterator();

			while (MeasIter != MeasIterEnd)
			{
				cout << "Header comment :"   << MeasIter->getHeaderComment() << endl
					<< ROMIter->getRefPtName().getName() << "   " << MeasIter->getTargetPoint().getName() << "   "<<
					MeasIter->getVDistValue().getMetresValue() << endl
					<< "sigma : " << MeasIter->getSigma().getMMetresValue() 
					<< "   constant" << " (" << MeasIter->getMeasConst().getConstantName() << ") : " << MeasIter->getMeasConst().getValue().getMetresValue() 
					<< "(" << MeasIter->getMeasConst().getStatus() << ")" << endl
					<< "   Meas. Id. & comment : " <<	MeasIter->getId() << "   " << MeasIter->getComment() << endl
					<< "   Ref. Pt. Id. : " << ROMIter->getId() << endl;

				MeasIter++;
			}

			cout << endl;
			ROMIter++;
			i++;
		}

		LSiter++;
	}



	cout << endl << endl << "Reading Errors' File : " << endl;
	cout << project.getError() << endl;


	// constant 
	DistConstantIterator cIter, cIterEnd;
	cIter = project.getDataSet()->getWorkingConstants()->getDistConstantsBeginIterator();
	cIterEnd = project.getDataSet()->getWorkingConstants()->getDistConstantsEndIterator();

	while (cIter!=cIterEnd)
	{
		cout << cIter->getConstantName() << "   " << cIter->getValue().getMetresValue() << endl;
		cIter++;
	}

	//number of measurements
	cout << "Points cala : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kCala) << endl;
	cout << "Points Vx : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kVx) << endl;
	cout << "Points Vy : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kVy) << endl;
	cout << "Points Vz : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kVz) << endl;
	cout << "Points Vxy : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kVxy) << endl;
	cout << "Points Vxz : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kVxz) << endl;
	cout << "Points Vyz : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kVyz) << endl;
	cout << "Points Vxyz : " << project.getDataSet()->getPointsDimension(TSpatialStatus::kVxyz) << endl;
	cout << "ANGL : " << project.getDataSet()->getHADimension() << endl;
	cout << "ZENI : " << project.getDataSet()->getZDDimension(TAMeasurement::kVariable) << endl;
	cout << "ZENH : " << project.getDataSet()->getZDDimension(TAMeasurement::kFixed) << endl;
	cout << "DHOR : " << project.getDataSet()->getHDDimension() << endl;
	cout << "DTHE : " << project.getDataSet()->getSDDimension(TAMeasurement::kVariable) << endl;
	cout << "DMES : " << project.getDataSet()->getSDDimension(TAMeasurement::kFixed) << endl;
	cout << "DVER : " << project.getDataSet()->getVDDimension() << endl;
	
*/


	/*
	const char* fileToWrite;
	const TLSCalcDataSet* lscDS = new TLSCalcDataSet(lgcDS);
	fileToWrite= "C:\\workspace\\LGC\\essaiecriture.lgc";
	TLGCFileWriter w (lgcDS, lscDS, fileToWrite);
	*/


	TLGCSimulatedObsFileWriter writer;
	writer.writeSimulatedObsFile(project.getDataSet());

	return;
}
