///////////////////////////////////////////////////////////////
//test observation Maker
//01/2003
///////////////////////////////////////////////////////////////

//#include <iostream>
//#include <string>
//using namespace std;

//#include "TLGCApplication.h"
#include "TLGCProject.h"

/*#include "TSpatialPoint.h"
#include "TSpatialPosition.h"
#include "TRefSystemFactory.h"
#include "TCoordSysFactory.h"
#include "TTheodoliteStation.h"
#include "THorAngleROM.h"
#include "TLGCDataSet.h"*/
#include "TObservationMaker.h"

void main()
{
////////////////////////////////////////////////////////////////////////////////////////////////
//Premier test du maker
////////////////////////////////////////////////////////////////////////////////////////////////
/*
	//creation des points
	TSpatialPointName nameStation ("S");
	TVReferenceFrame* ccs = (TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS)); 
	real x, y, z;
	x = 1000;
	y = 5000;
	z = 200;
	TSpatialPosition* posStation = new TSpatialPosition(ccs, x, y, z, TCoordSysFactory::k3DCartesian);
	TSpatialPoint* pointStation = new TSpatialPoint( nameStation, posStation);

	TSpatialPointName nameTargetA ("A");
	TSpatialPosition* posTargetA = new TSpatialPosition(ccs);
	TSpatialPoint* targetA = new TSpatialPoint(nameTargetA, posTargetA);
	
	TSpatialPointName nameTargetB ("B");
	TSpatialPosition* posTargetB = new TSpatialPosition(ccs);
	TSpatialPoint* targetB = new TSpatialPoint(nameTargetB, posTargetB);

	TSpatialPointName nameTargetC ("C");
	TSpatialPosition* posTargetC = new TSpatialPosition(ccs);
	TSpatialPoint* targetC = new TSpatialPoint(nameTargetC, posTargetC);

	//creation des constantes d angle
	TAngle a (1);
	TAngleConstants* csteA = new TAngleConstants(a, TAMeasurement::kFixed);

	//creation de mesures d'angle
	TAngle obsAngleA (0);
	TAngle sigmaA (LITERAL(0.001));
	THorAngleMeasurement* HAA = new THorAngleMeasurement(nameTargetA, obsAngleA, sigmaA);

	TAngle obsAngleB;
	obsAngleB.setRadiansValue(LITERAL(1.56));
	TAngle sigmaB (LITERAL(0.002));
	THorAngleMeasurement* HAB = new THorAngleMeasurement(nameTargetB, obsAngleB, sigmaB);

	TAngle obsAngleC;
	obsAngleC.setRadiansValue(LITERAL(2.39));
	TAngle sigmaC (LITERAL(0.003));
	THorAngleMeasurement* HAC = new THorAngleMeasurement(nameTargetC, obsAngleC, sigmaC);

	
	//creation du data set
	TLGCDataSet* data = new TLGCDataSet();
	data->getWorkingPoints()->addPoint(pointStation);
	data->getWorkingPoints()->addPoint(targetA);
	data->getWorkingPoints()->addPoint(targetB);
	data->getWorkingPoints()->addPoint(targetC);
	data->getWorkingConstants()->addAngleConstant(csteA);

	//creation d'une HAROM
	THorAngleROM* HARom = new THorAngleROM();
	HARom->addHorAngle(HAA);
	HARom->addHorAngle(HAB);
	HARom->addHorAngle(HAC);
	
	TTheodoliteStation* theoStation =  new TTheodoliteStation(nameStation);
	theoStation->addHorAngleROM(HARom);	
	theoStation->ifNotDoneSetAngleConst(*csteA);
	data->getWorkingStations()->addTheodStation(theoStation);

	//test du maker
	ObservationSet obs;
	TObservationMaker maker;

	maker.processData(*data, obs);

	string errorMessage = maker.getError();
	cout<<"error : "<<errorMessage<<endl;

	HorAngObsIter beg =obs.beginHorAng();
	HorAngObsIter end =obs.endHorAng();

	while(beg != end)
	{	
		cout<<"Station name : "<< beg->getStationedPosName()<<endl;
		cout<<"Target name : "<< beg->getTargetPosName()<<endl<<endl;

		cout<<"angle value : "<< beg->getHorAngleValue().getRadiansValue()<<endl;
		cout<<"angle status : "<< beg->getHorAngleValue().getStatus()<<endl<<endl;

		cout<<"sigma value : "<< beg->getSigmaValue().getRadiansValue()<<endl;
		cout<<"sigma status : "<< beg->getSigmaValue().getStatus()<<endl<<endl;

		cout<<"ROM V0 name : "<< beg->getRomV0Name()<<endl;
		cout<<"ROM V0 value : "<< beg->getRomV0().kappa.getRadiansValue()<<endl;
		cout<<"ROM V0 status : "<< beg->getRomV0Status()<<endl<<endl<<endl;

		beg++;
	}
*/

///////////////////////////////////////////////////////////////////////////////////////////////
//Test depuis le file Reader
///////////////////////////////////////////////////////////////////////////////////////////////

	const char * LGCFile;
	LGCFile = "C:\\workspace\\LGC\\hihptest3.inp";

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


	//////////////////////////////////////////////////////////////////////////////
	//Horizontal Angle
	HorAngObsIter beg =obs.beginHorAng();
	HorAngObsIter end =obs.endHorAng();

	cout<<"TEST HORIZONTAL ANGLE"<<endl<<endl;

	while(beg != end)
	{	
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< beg->getStationedPosName()<<endl;
		cout<<"	Station X : "<<beg->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<beg->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<beg->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<beg->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<beg->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<beg->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<beg->getStationedPosStatus()<<endl;

		cout<<"Target :"<<endl;
		cout<<"	Target name : "<< beg->getTargetPosName()<<endl;
		cout<<"	Target X : "<<beg->getTargetPosition().getX().getMetresValue()<<",	numeric status : "<<beg->getTargetPosition().getX().getStatus()<<endl;
		cout<<"	Target Y : "<<beg->getTargetPosition().getY().getMetresValue()<<",	numeric status : "<<beg->getTargetPosition().getY().getStatus()<<endl;
		cout<<"	Target Z : "<<beg->getTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<beg->getTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Target spatial status : "<<beg->getTargetPosStatus()<<endl;
	
		cout<<"Horizontal Angle :"<<endl;
		cout.precision(8);
		cout<<"	angle value : "<< beg->getHorAngleValue().getGonsValue()<<",	numeric status : "<< beg->getHorAngleValue().getStatus()<<endl;
	
		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< beg->getSigmaValue().getGonsValue()<<",	numeric status : "<< beg->getSigmaValue().getStatus()<<endl;
		
		cout<<"ROM :"<<endl;
		cout<<"	ROM V0 name : "<< beg->getRomV0Name()<<endl;
		cout<<"	ROM V0 kappa value : "<< beg->getRomV0().kappa.getGonsValue()<<endl;
		cout<<"	ROM V0 calc status : "<< beg->getRomV0Status()<<endl<<endl<<endl;

		beg++;
	}

	cout<<"error : "<<maker.getError()<<endl;

	//////////////////////////////////////////////////////////////////////////////
	//distance zenithal
	ZenDistObsIter begDZ =obs.beginZenDist();
	ZenDistObsIter endDZ =obs.endZenDist();

	cout<<"TEST ZENITHAL DISTANCE"<<endl<<endl;

	while(begDZ != endDZ)
	{	

		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begDZ->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begDZ->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begDZ->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begDZ->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begDZ->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begDZ->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begDZ->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begDZ->getStationedPosStatus()<<endl;
		cout<<"	Station height : "<< begDZ->getInstrumentHeight().getMetresValue()<<",	numeric status : "<< begDZ->getInstrumentHeight().getStatus()<<endl;
		cout<<"	Station height calc status: "<< begDZ->getInstHeightStatus()<<endl;
		cout<<"	Station height name: "<< begDZ->getIHeightName()<<endl;
	
		cout<<"Target :"<<endl;
		cout<<"	Target name : "<< begDZ->getTargetPosName()<<endl;
		cout<<"	Target X : "<<begDZ->getTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begDZ->getTargetPosition().getX().getStatus()<<endl;
		cout<<"	Target Y : "<<begDZ->getTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begDZ->getTargetPosition().getY().getStatus()<<endl;
		cout<<"	Target Z : "<<begDZ->getTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begDZ->getTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Target spatial status : "<<begDZ->getTargetPosStatus()<<endl;
		cout<<"	Target height : "<< begDZ->getPrismHeight().getMetresValue()<<",	numeric status : "<< begDZ->getPrismHeight().getStatus()<<endl;
		cout<<"	Target height  calc status: "<< begDZ->getPrismHeightStatus()<<endl;
		cout<<"	Target height  name: "<< begDZ->getPHeightName()<<endl;
		
		cout<<"Zenithal distance :"<<endl;
		cout.precision(8);
		cout<<"	angle value : "<< begDZ->getZenAngleValue().getGonsValue()<<",	numeric status : "<< begDZ->getZenAngleValue().getStatus()<<endl;
	
		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begDZ->getSigmaValue().getGonsValue()<<",	numeric status : "<< begDZ->getSigmaValue().getStatus()<<endl<<endl<<endl;


		begDZ++;
	}

	cout<<"error : "<<maker.getError()<<endl;

	//////////////////////////////////////////////////////////////////////////////
	//vertical distance 
	VertDistObsIter begVD =obs.beginVertDist();
	VertDistObsIter endVD =obs.endVertDist();

	cout<<"TEST VERTICAL DISTANCE"<<endl<<endl;

	while(begVD != endVD)
	{
		cout<<"Reference :"<<endl;
		cout<<"	Reference name : "<< begVD->getRefPosName()<<endl;
		cout<<"	Reference X : "<<begVD->getRefPosition().getX().getMetresValue()<<",	numeric status : "<<begVD->getRefPosition().getX().getStatus()<<endl;
		cout<<"	Reference Y : "<<begVD->getRefPosition().getY().getMetresValue()<<",	numeric status : "<<begVD->getRefPosition().getY().getStatus()<<endl;
		cout<<"	Reference Z : "<<begVD->getRefPosition().getZ().getMetresValue()<<",	numeric status : "<<begVD->getRefPosition().getZ().getStatus()<<endl;
		cout<<"	Reference spatial status : "<<begVD->getRefPosStatus()<<endl;
	
		cout<<"Target :"<<endl;
		cout<<"	Target name : "<< begVD->getTargetPosName()<<endl;
		cout<<"	Target X : "<<begVD->getTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begVD->getTargetPosition().getX().getStatus()<<endl;
		cout<<"	Target Y : "<<begVD->getTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begVD->getTargetPosition().getY().getStatus()<<endl;
		cout<<"	Target Z : "<<begVD->getTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begVD->getTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Target spatial status : "<<begVD->getTargetPosStatus()<<endl;
	
		cout<<"Vertical distance :"<<endl;
		cout.precision(8);
		cout<<"	dist value : "<< begVD->getVertDistValue().getMetresValue()<<",	numeric status : "<< begVD->getVertDistValue().getStatus()<<endl;
	
		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begVD->getSigmaValue().getMMetresValue()<<",	numeric status : "<< begVD->getSigmaValue().getStatus()<<endl<<endl<<endl;

		begVD++;
	}

	cout<<"error : "<<maker.getError()<<endl;

	//////////////////////////////////////////////////////////////////////////////
	//spatial distance 
	SpaDistObsIter begSD =obs.beginSpaDist();
	SpaDistObsIter endSD =obs.endSpaDist();

	cout<<"TEST SPATIAL DISTANCE"<<endl<<endl;

	while(begSD != endSD)
	{	
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begSD->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begSD->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begSD->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begSD->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begSD->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begSD->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begSD->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begSD->getStationedPosStatus()<<endl;
		cout<<"	Station height : "<< begSD->getInstrumentHeight().getMetresValue()<<",	numeric status : "<<begSD->getInstrumentHeight().getStatus()<<endl;
		cout<<"	Station height calc status: "<< begSD->getInstHeightStatus()<<endl;
		cout<<"	Station height name: "<< begSD->getIHeightName()<<endl;
	
		cout<<"Target :"<<endl;
		cout<<"	Target name : "<< begSD->getTargetPosName()<<endl;
		cout<<"	Target X : "<<begSD->getTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begSD->getTargetPosition().getX().getStatus()<<endl;
		cout<<"	Target Y : "<<begSD->getTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begSD->getTargetPosition().getY().getStatus()<<endl;
		cout<<"	Target Z : "<<begSD->getTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begSD->getTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Target spatial status : "<<begSD->getTargetPosStatus()<<endl;
		cout<<"	Target height : "<< begSD->getPrismHeight().getMetresValue()<<",	numeric status : "<<begSD->getPrismHeight().getStatus()<<endl;
		cout<<"	Target height  calc status: "<< begSD->getPrismHeightStatus()<<endl;
		cout<<"	Target height  name: "<< begSD->getPHeightName()<<endl;
		
		cout<<"Spatial distance :"<<endl;
		cout.precision(8);
		cout<<"	dist value : "<< begSD->getSpatialDistValue().getMetresValue()<<",	numeric status : "<< begSD->getSpatialDistValue().getStatus()<<endl;
	
		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begSD->getSigmaValue().getMMetresValue()<<",	numeric status : "<< begSD->getSigmaValue().getStatus()<<endl;
	
		cout<<"Sigma ppm :"<<endl;
		cout<<"	sigma ppm value : "<< begSD->getSigmaPpmValue().getMMetresValue()<<",	numeric status : "<< begSD->getSigmaPpmValue().getStatus()<<endl;
	
		cout<<"Constant :"<<endl;
		cout<<"	cte name : "<< begSD->getConstantName()<<endl;
		cout<<"	cte value : "<< begSD->getConstantValue().getMetresValue()<<",	numeric status : "<< begSD->getConstantValue().getStatus()<<endl;
		cout<<"	cte calc status : "<< begSD->getConstantStatus()<<endl<<endl<<endl;

		begSD++;
	}

	cout<<"error : "<<maker.getError()<<endl;


	//////////////////////////////////////////////////////////////////////////////
	//horizontal distance
	HorDistObsIter begHD =obs.beginHorDist();
	HorDistObsIter endHD =obs.endHorDist();

	cout<<"TEST HORIZONTAL DISTANCE"<<endl<<endl;

	while(begHD != endHD)
	{
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begHD->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begHD->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begHD->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begHD->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begHD->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begHD->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begHD->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begHD->getStationedPosStatus()<<endl;

		cout<<"Target :"<<endl;
		cout<<"	Target name : "<< begHD->getTargetPosName()<<endl;
		cout<<"	Target X : "<<begHD->getTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begHD->getTargetPosition().getX().getStatus()<<endl;
		cout<<"	Target Y : "<<begHD->getTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begHD->getTargetPosition().getY().getStatus()<<endl;
		cout<<"	Target Z : "<<begHD->getTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begHD->getTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Target spatial status : "<<begHD->getTargetPosStatus()<<endl;

		cout<<"Horizontal distance :"<<endl;
		cout.precision(8);
		cout<<"	dist value : "<< begHD->getHorDistValue().getMetresValue()<<",	numeric status : "<< begHD->getHorDistValue().getStatus()<<endl;

		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begHD->getSigmaValue().getMMetresValue()<<",	numeric status : "<< begHD->getSigmaValue().getStatus()<<endl;

		cout<<"Sigma ppm :"<<endl;
		cout<<"	sigma ppm value : "<< begHD->getSigmaPpmValue().getMMetresValue()<<",	numeric status : "<< begHD->getSigmaPpmValue().getStatus()<<endl<<endl<<endl;

		begHD++;
	}

	cout<<"error : "<<maker.getError()<<endl;


	//////////////////////////////////////////////////////////////////////////////
	//offset To Vertical Line
	OffsetToVerLineObsIter begECVE =obs.beginOffsetToVerLine();
	OffsetToVerLineObsIter endECVE =obs.endOffsetToVerLine();

	cout<<"TEST ECVE (OFFSET TO VERTICAL LINE)"<<endl<<endl;

	while(begECVE != endECVE)
	{
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begECVE->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begECVE->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begECVE->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begECVE->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begECVE->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begECVE->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begECVE->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begECVE->getStationedPosStatus()<<endl;

		cout<<"Target :"<<endl;
		cout<<"	Target name : "<< begECVE->getTargetPosName()<<endl;
		cout<<"	Target X : "<<begECVE->getTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begECVE->getTargetPosition().getX().getStatus()<<endl;
		cout<<"	Target Y : "<<begECVE->getTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begECVE->getTargetPosition().getY().getStatus()<<endl;
		cout<<"	Target Z : "<<begECVE->getTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begECVE->getTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Target spatial status : "<<begECVE->getTargetPosStatus()<<endl;

		cout<<"Offset :"<<endl;
		cout.precision(8);
		cout<<"	dist value : "<< begECVE->getOffsetToVerLineValue().getMetresValue()<<",	numeric status : "<< begECVE->getOffsetToVerLineValue().getStatus()<<endl;

		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begECVE->getSigmaValue().getMMetresValue()<<",	numeric status : "<< begECVE->getSigmaValue().getStatus()<<endl<<endl<<endl;

		begECVE++;
	}

	cout<<"error : "<<maker.getError()<<endl;



	//////////////////////////////////////////////////////////////////////////////
	//offset To Spatial Line
	OffsetToSpaLineObsIter begECSP =obs.beginOffsetToSpaLine();
	OffsetToSpaLineObsIter endECSP =obs.endOffsetToSpaLine();

	cout<<"TEST ECSP (OFFSET TO SPATIAL LINE)"<<endl<<endl;

	while(begECSP != endECSP)
	{
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begECSP->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begECSP->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begECSP->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begECSP->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begECSP->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begECSP->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begECSP->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begECSP->getStationedPosStatus()<<endl;

		cout<<"Target :"<<endl;
		cout<<"	First Pt Target name : "<< begECSP->getFirstTargetPosName()<<endl;
		cout<<"	First Pt Target X : "<<begECSP->getFirstTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begECSP->getFirstTargetPosition().getX().getStatus()<<endl;
		cout<<"	First Pt Target Y : "<<begECSP->getFirstTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begECSP->getFirstTargetPosition().getY().getStatus()<<endl;
		cout<<"	First Pt Target Z : "<<begECSP->getFirstTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begECSP->getFirstTargetPosition().getZ().getStatus()<<endl;
		cout<<"	First Pt Target spatial status : "<<begECSP->getFirstTargetPosStatus()<<endl;

		cout<<"	Second Pt Target name : "<< begECSP->getSecondTargetPosName()<<endl;
		cout<<"	Second Pt Target X : "<<begECSP->getSecondTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begECSP->getSecondTargetPosition().getX().getStatus()<<endl;
		cout<<"	Second Pt Target Y : "<<begECSP->getSecondTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begECSP->getSecondTargetPosition().getY().getStatus()<<endl;
		cout<<"	Second Pt Target Z : "<<begECSP->getSecondTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begECSP->getSecondTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Second Pt Target spatial status : "<<begECSP->getSecondTargetPosStatus()<<endl;

		cout<<"Offset :"<<endl;
		cout.precision(8);
		cout<<"	dist value : "<< begECSP->getOffsetToSpaLineValue().getMetresValue()<<",	numeric status : "<< begECSP->getOffsetToSpaLineValue().getStatus()<<endl;

		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begECSP->getSigmaValue().getMMetresValue()<<",	numeric status : "<< begECSP->getSigmaValue().getStatus()<<endl<<endl<<endl;

		begECSP++;
	}

	cout<<"error : "<<maker.getError()<<endl;


	//////////////////////////////////////////////////////////////////////////////
	//offset To Vertical Plane
	OffsetToVerPlaneObsIter begECHO =obs.beginOffsetToVerPlane();
	OffsetToVerPlaneObsIter endECHO =obs.endOffsetToVerPlane();

	cout<<"TEST ECHO (OFFSET TO VERTICAL PLANE)"<<endl<<endl;

	while(begECHO != endECHO)
	{
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begECHO->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begECHO->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begECHO->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begECHO->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begECHO->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begECHO->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begECHO->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begECHO->getStationedPosStatus()<<endl;

		cout<<"Target :"<<endl;
		cout<<"	First Pt Target name : "<< begECHO->getFirstTargetPosName()<<endl;
		cout<<"	First Pt Target X : "<<begECHO->getFirstTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begECHO->getFirstTargetPosition().getX().getStatus()<<endl;
		cout<<"	First Pt Target Y : "<<begECHO->getFirstTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begECHO->getFirstTargetPosition().getY().getStatus()<<endl;
		cout<<"	First Pt Target Z : "<<begECHO->getFirstTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begECHO->getFirstTargetPosition().getZ().getStatus()<<endl;
		cout<<"	First Pt Target spatial status : "<<begECHO->getFirstTargetPosStatus()<<endl;

		cout<<"	Second Pt Target name : "<< begECHO->getSecondTargetPosName()<<endl;
		cout<<"	Second Pt Target X : "<<begECHO->getSecondTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begECHO->getSecondTargetPosition().getX().getStatus()<<endl;
		cout<<"	Second Pt Target Y : "<<begECHO->getSecondTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begECHO->getSecondTargetPosition().getY().getStatus()<<endl;
		cout<<"	Second Pt Target Z : "<<begECHO->getSecondTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begECHO->getSecondTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Second Pt Target spatial status : "<<begECHO->getSecondTargetPosStatus()<<endl;

		cout<<"Offset :"<<endl;
		cout.precision(8);
		cout<<"	dist value : "<< begECHO->getOffsetToVerPlaneValue().getMetresValue()<<",	numeric status : "<< begECHO->getOffsetToVerPlaneValue().getStatus()<<endl;

		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begECHO->getSigmaValue().getMMetresValue()<<",	numeric status : "<< begECHO->getSigmaValue().getStatus()<<endl<<endl<<endl;

		begECHO++;
	}

	cout<<"error : "<<maker.getError()<<endl;


	//////////////////////////////////////////////////////////////////////////////
	//offset To Theodolite Plane
	OffsetToTheoPlaneObsIter begECTH =obs.beginOffsetToTheoPlane();
	OffsetToTheoPlaneObsIter endECTH =obs.endOffsetToTheoPlane();

	cout<<"TEST ECTH (OFFSET TO THEODOLITE PLANE)"<<endl<<endl;

	while(begECTH != endECTH)
	{
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begECTH->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begECTH->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begECTH->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begECTH->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begECTH->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begECTH->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begECTH->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begECTH->getStationedPosStatus()<<endl;

		cout<<"Target :"<<endl;
		cout<<"	First Pt Target name : "<< begECTH->getTargetTheoPosName()<<endl;
		cout<<"	First Pt Target X : "<<begECTH->getTargetTheoPosition().getX().getMetresValue()<<",	numeric status : "<<begECTH->getTargetTheoPosition().getX().getStatus()<<endl;
		cout<<"	First Pt Target Y : "<<begECTH->getTargetTheoPosition().getY().getMetresValue()<<",	numeric status : "<<begECTH->getTargetTheoPosition().getY().getStatus()<<endl;
		cout<<"	First Pt Target Z : "<<begECTH->getTargetTheoPosition().getZ().getMetresValue()<<",	numeric status : "<<begECTH->getTargetTheoPosition().getZ().getStatus()<<endl;
		cout<<"	First Pt Target spatial status : "<<begECTH->getTargetTheoPosStatus()<<endl;

		cout<<"	Angle : "<< begECTH->getTargetAngle().getGonsValue()<<",	numeric status : "<<begECTH->getTargetAngle().getStatus()<<endl;

		cout<<"Offset :"<<endl;
		cout.precision(8);
		cout<<"	dist value : "<< begECTH->getOffsetToTheoPlaneValue().getMetresValue()<<",	numeric status : "<< begECTH->getOffsetToTheoPlaneValue().getStatus()<<endl;

		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begECTH->getSigmaValue().getMMetresValue()<<",	numeric status : "<< begECTH->getSigmaValue().getStatus()<<endl<<endl<<endl;

		begECTH++;
	}

	cout<<"error : "<<maker.getError()<<endl;


	//////////////////////////////////////////////////////////////////////////////
	//Orientation
	GyroOrieObsIter begORIE =obs.beginGyroOrie();
	GyroOrieObsIter endORIE =obs.endGyroOrie();

	cout<<"TEST ORIE (ORIENTATION)"<<endl<<endl;

	while(begORIE != endORIE)
	{
		cout<<"Station :"<<endl;
		cout<<"	Station name : "<< begORIE->getStationedPosName()<<endl;
		cout<<"	Station X : "<<begORIE->getStationedPosition().getX().getMetresValue()<<",	numeric status : "<<begORIE->getStationedPosition().getX().getStatus()<<endl;
		cout<<"	Station Y : "<<begORIE->getStationedPosition().getY().getMetresValue()<<",	numeric status : "<<begORIE->getStationedPosition().getY().getStatus()<<endl;
		cout<<"	Station Z : "<<begORIE->getStationedPosition().getZ().getMetresValue()<<",	numeric status : "<<begORIE->getStationedPosition().getZ().getStatus()<<endl;
		cout<<"	Station spatial status : "<<begORIE->getStationedPosStatus()<<endl;

		cout<<"Target :"<<endl;
		cout<<"	Target name : "<< begORIE->getTargetPosName()<<endl;
		cout<<"	Target X : "<<begORIE->getTargetPosition().getX().getMetresValue()<<",	numeric status : "<<begORIE->getTargetPosition().getX().getStatus()<<endl;
		cout<<"	Target Y : "<<begORIE->getTargetPosition().getY().getMetresValue()<<",	numeric status : "<<begORIE->getTargetPosition().getY().getStatus()<<endl;
		cout<<"	Target Z : "<<begORIE->getTargetPosition().getZ().getMetresValue()<<",	numeric status : "<<begORIE->getTargetPosition().getZ().getStatus()<<endl;
		cout<<"	Target spatial status : "<<begORIE->getTargetPosStatus()<<endl;
	
		cout<<"Horizontal Angle :"<<endl;
		cout.precision(8);
		cout<<"	angle value : "<< begORIE->getGyroOrieValue().getGonsValue()<<",	numeric status : "<< begORIE->getGyroOrieValue().getStatus()<<endl;
	
		cout<<"Sigma :"<<endl;
		cout<<"	sigma value : "<< begORIE->getSigmaValue().getGonsValue()<<",	numeric status : "<< begORIE->getSigmaValue().getStatus()<<endl;
		
		begORIE++;
	}

	cout<<"error : "<<maker.getError()<<endl;


	return;
	

};