////////////////////////////////////////////////////////////////////
// TResultsFileWriter.cpp :Implementation file
// Write an LGC output file
// Creates a file from the calculation results and sends the appropriate messages
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////////
 
#include <ctime>
#include "TLGCData.h"
#include "TSeparatedFormatTStream.h"
#include "TResultsFileWriter.h"
#include "TPointConverter.h"
#include "TLGCApp.h"
#include "TLOR2LOR.h"
#include "TXYH2CCS.h"
#include "TFRAMEWriter.h"
#include "TSpatialStatus.h"
 
#ifdef PRNCSV
ofstream fcsv;
map<string,vector<TLSCalcPosVectorParam::ErrorEllipsoid>> elldata;
#endif
 
 
/////////////////////////////////////////////////////////////////////
 
//ClassImp(TResultsFileWriter)
 
 
//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
 
// Can point to a writer that prints a header for an ellipsoid, an ellipse or a line
typedef void (*HeaderPrintFunc)(TAStreamFormatter* stream, int namewdith, int coordwidth);
 
//static void writeNoHeader(TAStreamFormatter*, int , int) { }
 
// Can point to a writer that prints the axes of an ellipsoid, an ellipse or a line after a header
//typedef void (*EllPrintFunc)(TAStreamFormatter* stream, LSPosVecConstIter pt, int namewdith, int coordwidth, int coorprecision, const std::string& sep);
 
 
/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TResultsFileWriter::TResultsFileWriter() :
TAFileWriter()
{//default constructor    
    withEllipses = false;
 
#ifdef PRNCSV
    fcsv.open("c:/temp/testell.csv");
#endif
}
 
 
TResultsFileWriter::TResultsFileWriter(TAStreamFormatter* stream, const TLGCData* project) :
TAFileWriter(stream, project)
{
    
#ifdef PRNCSV
    fcsv.open("c:/temp/testell.csv");
#endif
}
 
 
TResultsFileWriter::~TResultsFileWriter()
{//destructor
#ifdef PRNCSV
    fcsv << "\"NAME\";" << "\"LX\";" << "\"LY\";" << "\"LZ\";" <<endl;
    for (auto i(elldata.begin()); i != elldata.end(); i++) {
        for (auto j(i->second.begin()); j != i->second.end(); j++)
            fcsv << i->first << ";" << j->lx << ";" << j->ly << ";" << j->lz << endl;
        fcsv << endl;
    }
#endif
}
 
 
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void    TResultsFileWriter::writeFile(const string error)
{//write error messages from project
    writeTitle();
    writeError(error);
}
 
// Write the LGC results for the given project
void    TResultsFileWriter::writeFile()
{
    //Limited just number of points and unknowns for points for now
    this->initObsListNumber();    
    
    //write the header title of the results file
    this->writeTitle();
 
    // write the input data summary
    this->writeDataSummary();
 
    this->writeCalcDataSummary();
 
    // write the calculation header
    this->writeSigmaAPosteriori();
 
    //Write all the information specific to a frame (points and frames)
    this->writeFramesResults();
}
 
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//INITIALISE ATTRIBUTS
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void    TResultsFileWriter::initObsListNumber()
{
    //SIZE OF POINT'S LIST
    fNumberOfPoints    = fProjectData->getPoints().numObjects();
    fNumberOfUnknOfPoints = fProjectData->getPoints().numUnknowns();
 
    return;
}
 
 
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//TITLE
///////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void    TResultsFileWriter::writeTitle()
{
    TAStreamFormatter* stream = getStream();
 
 
     (*stream)<<endl<<endl;
     (*stream)<<"                          ****               ******              *****           ***********                                     "<<endl;
    (*stream)<<"                          ****            ************        ***********        ***********                                     "<<endl;
    (*stream)<<"                          ****           *****     ****      *****    ****              ****                                     "<<endl;
    (*stream)<<"                          ****          ****        ****    ****       ****             ****                                     "<<endl;
    (*stream)<<"                          ****          ****                ****                        ****                                     "<<endl;
    (*stream)<<"                          ****          ****                ****                 ***********                                     "<<endl;
    (*stream)<<" ***********************  ****          ****      *******   ****                 ***********    ***********************             "<<endl;
    (*stream)<<" ***********************  ****          ****      *******   ****                 ****            ***********************             "<<endl;
    (*stream)<<"                          ****          ****       ******   ****       ****      ****                                            "<<endl;
    (*stream)<<"                          ***********    ****     **** **    ****     ****       ****                                            "<<endl;
    (*stream)<<"                          ***********     ***********         ***********        ***********                                     "<<endl;
    (*stream)<<"                          ***********        *****               *****           ***********                                     "<<endl<<endl<<endl<<endl;
 
    (*stream)<<"*********************************************************************************************************************************** "<<endl;
    (*stream)<<endl<<endl<<endl;
 
    //write software id.
    (*stream)<<(TLGCApp::getProgId())<<endl;
 
    //write software copyright
    (*stream)<<(TLGCApp::getCopyright())<<endl;
 
    //last compilation (now showed in progID together with surveylib version)
    //(*stream)<<"Derniere compilation : "<<__DATE__<<endl<<endl<<endl;
 
    //write title
    (*stream)<<"*********************************************************************************************************************************** "<<endl;
    (*stream)<<(fProjectData->getConfig().title)<<endl;
    (*stream)<<endl;
 
    //write time
	char tmpbuf[128];
	time_t ltime;
    time(&ltime);
	struct tm *today;
#ifdef __linux__
	tzset();
#else
	_tzset();
#endif
	today = localtime( &ltime );
    string essai = ctime( &ltime );
    strftime( tmpbuf, 128,"CALCUL DU %d %B %Y %X", today );
    (*stream)<<tmpbuf<<endl;
    (*stream)<<"*********************************************************************************************************************************** "<<endl<<endl<<endl<<endl;
 
}
 
 
// Write a summary of the LGC input data
void    TResultsFileWriter::writeDataSummary()
{
    TAStreamFormatter* stream = getStream();
 
    (*stream)<<"DATA SET - GENERAL INFO:" << endl<<endl;
    (*stream)<<"\tFRAMES defined:" << endl;
    (*stream)<<"\t \t NOMBRE =\t"<< int(fProjectData->getNumberOfFrames()) << endl << endl;
 
    if(fProjectData->getPoints().numObjects()>0)
       writeAdjustableObjGeneralInfo("POINTS", (int)fProjectData->getPoints().numObjects(), (int)fProjectData->getPoints().numUnknowns());
    if(fProjectData->getLines().numObjects()>0)
       writeAdjustableObjGeneralInfo("LINES", (int)fProjectData->getLines().numObjects(), (int)fProjectData->getLines().numUnknowns());
 
    if(fProjectData->getPlanes().numObjects()>0)
       writeAdjustableObjGeneralInfo("PLANES", (int)fProjectData->getPlanes().numObjects(), (int)fProjectData->getPlanes().numUnknowns());
 
    if(fProjectData->getAngles().numObjects()>0)
       writeAdjustableObjGeneralInfo("ANGLES", (int)fProjectData->getAngles().numObjects(), (int)fProjectData->getAngles().numUnknowns());
 
    if(fProjectData->getLength().numObjects()>0)
       writeAdjustableObjGeneralInfo("LENGTHS", (int)fProjectData->getLength().numObjects(), (int)fProjectData->getLength().numUnknowns());
 
    (*stream)<<endl<<endl;
 
    int fNumFixedPoint = fProjectData->getPointsDimension(TSpatialStatus::kCala);
    int fNumVxyzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVxyz);
    int fNumVyzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVyz);
    int fNumVxzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVxz);
    int fNumVxyPoint = fProjectData->getPointsDimension(TSpatialStatus::kVxy);
    int fNumVxPoint = fProjectData->getPointsDimension(TSpatialStatus::kVx);
    int fNumVyPoint = fProjectData->getPointsDimension(TSpatialStatus::kVy);
    int fNumVzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVz);
 
    (*stream)<<"POINTS SUMMARY:" << endl;
    //insure impose de mettre les endl en en retour de ligne....
    //LECTURE DES POINTS DE CALAGE
    if(fNumFixedPoint != 0)
        writePointDataSummary("DE CALAGE", fNumFixedPoint);
 
    //LECTURE DES POINTS VARIABLES EN XYZ
    if(fNumVxyzPoint != 0)
        writePointDataSummary("VARIABLES EN XYZ", fNumVxyzPoint);
 
    //LECTURE DES POINTS INVARIABLES EN X
    if(fNumVyzPoint != 0)
        writePointDataSummary("INVARIABLES EN X", fNumVyzPoint);
 
    //LECTURE DES POINTS INVARIABLES EN Y
    if(fNumVxzPoint != 0)
        writePointDataSummary("INVARIABLES EN Y", fNumVxzPoint);
 
    //LECTURE DES POINTS INVARIABLES EN Z
    if(fNumVxyPoint != 0)
        writePointDataSummary("INVARIABLES EN Z", fNumVxyPoint);
 
    //LECTURE DES POINTS VARIABLES EN X UNIQUEMENT
    if(fNumVxPoint != 0)
        writePointDataSummary("VARIABLES EN X UNIQUEMENT", fNumVxPoint);
 
    //LECTURE DES POINTS VARIABLES EN Y UNIQUEMENT
    if(fNumVyPoint != 0)
        writePointDataSummary("VARIABLES EN Y UNIQUEMENT", fNumVyPoint);
 
    //LECTURE DES POINTS VARIABLES EN Z UNIQUEMENT
    if(fNumVzPoint != 0)
        writePointDataSummary("VARIABLES EN Z UNIQUEMENT", fNumVzPoint);
 
 
 
    //LECTURE DES OBSERVATIONS
    int fNumHorAng = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kANGL);
    int fNumZenDist = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kZEND);
    int fNumDistMeas = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDIST);
    int fNumPLR3D = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kPLR3D);
	int fNumECTH = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECTH);
	int fNumDHOR = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDHOR);
	int fNumDSPT = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDSPT);
	int fNumUVEC = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kUVEC);
	int fNumUVD = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kUVD);
    int fNumDLEV = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDLEV);
	int fNumDVER = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kDVER);
    int fNumECHO = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECHO);
	int fNumECSP = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECSP);
	int fNumECVE = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECVE);
    int fNumORIE = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kORIE);
	int fNumPDOR = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kPDOR);
	int fNumRADI = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kRADI);
 
    (*stream)<<"MEASUREMENTS SUMMARY:" << endl;
    if(fNumHorAng != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kANGL), fNumHorAng);    
 
    if(fNumZenDist != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kZEND), fNumZenDist);    
 
    if(fNumDistMeas != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kDIST), fNumDistMeas);    
    
    if(fNumPLR3D != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kPLR3D), fNumPLR3D);

	if (fNumECTH != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kECTH), fNumECTH);

	if (fNumDHOR != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kDHOR), fNumDHOR);

	if (fNumUVEC != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kUVEC), fNumUVEC);

	if (fNumUVD != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kUVD), fNumUVD);

	if (fNumDSPT != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kDSPT), fNumDSPT);
 
    if(fNumDLEV != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kDLEV), fNumDLEV);
 
	if (fNumDVER != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kDVER), fNumDVER);
 
    if(fNumECHO != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kECHO), fNumECHO);
	
	if (fNumECSP != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kECSP), fNumECSP);
	
	if (fNumECVE != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kECVE), fNumECVE);
 
    if(fNumPDOR != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kPDOR), fNumPDOR);

	if (fNumRADI != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kRADI), fNumRADI);
 
 
    //FIN DE FICHIER
    (*stream)<<"*** FIN DE FICHIER ***";
    (*stream)<<endl<<endl;
}
 
 
// write the summary for the number of points of the given description  
void    TResultsFileWriter::writeAdjustableObjGeneralInfo(const std::string adjObjName, const int numAdj, const int numUnkn)  
{
    TAStreamFormatter &stream = getStreamRef();
    stream<<"\t"<< adjObjName << " defined:" << endl;
    stream<<"\t \t NOMBRE =\t"<<numAdj << endl << endl;
    stream<<"\tUnknowns introduced by " << adjObjName << ":"<<endl;
    stream<<"\t \t NOMBRE =\t"<<numUnkn << endl << endl;
}
 
 
// write the summary for the number of points of the given description  
void    TResultsFileWriter::writePointDataSummary(const string description, const int numPts)  
{
    TAStreamFormatter &stream = getStreamRef();
    //insure impose de mettre les endl en en retour de ligne....
 
    stream<<"            LECTURE DES POINTS " << description << endl;
    stream<<"+        OK :";
    stream<<endl<<endl;
    stream<<"            NOMBRE=    "<<numPts;
    stream<<endl<<endl<<endl;
    return;
}
 
 
// Write a summary of the LGC calculation data parameters
void    TResultsFileWriter::writeCalcDataSummary()
{
    TAStreamFormatter &stream = getStreamRef();
 
    //NOMBRE D'OBSERVATIONS
    stream<<"             NOMBRE D'OBSERVATIONS =  "<< fProjectData->fUEOIndices.OIndex;
	if (fProjectData->getConfig().pdor.isActive())
	{
		stream << " (PDOR INCLUS - ATTENTION, PREMIER CALA DEFINI DANS LE ROOT UTILISE";
	}
    stream<<endl<<endl;
 
    //NOMBRE D'INCONNUES
    stream<<"             NOMBRE D'INCONNUES =     "<< fProjectData->fUEOIndices.UIndex;
    stream<<endl<<endl;
 
    // NUMBER OF ITERATIONS
    stream<<"             NUMBER OF ITERATIONS =     "<<fProjectData->getNumberOfLSIterations();
    stream<<endl<<endl<<endl<<endl<<endl;
 
    //RESEAU COMPLETEMENT LIBRE ?
    if( fProjectData->getConfig().libre.isActive() )
        stream<<"LIBR CALCULATION IS NOT ENABLED YET, TO BE IMPLEMENTED";
 
    return;
}
 
 
void    TResultsFileWriter::writeSigmaAPosteriori()
{
    TAStreamFormatter* stream = getStream();
    string separator = getSeparator();
 
    TReal S0Aposteriori = fProjectData->getS0APosteriori();
    TReal S0LowLimit = fProjectData->getChiS0LowLimit();
    TReal S0UpLimit = fProjectData->getChiS0UpLimit();
 
    //SIGMA ZERO A POSTERIORI
	int					obsWidth = stream->getObsFormat()->getObsWidth();
	int obsPrecision = obsWidth > 3 ? (obsWidth - 6) : 0;
    stream->precision(obsPrecision);
    stream->width( stream->getObsFormat()->getObsResidualWidth() );
    (*stream)<<"SIGMA ZERO A POSTERIORI ="<<S0Aposteriori;
 
    (*stream)<<", VALEUR CRITIQUE = (";
    stream->precision(5);
    stream->width( stream->getObsFormat()->getObsResidualWidth() );
    (*stream)<<S0LowLimit<<", ";
 
    stream->precision(5);
    stream->width( stream->getObsFormat()->getObsResidualWidth() );
        (*stream)<<S0UpLimit<<")";
        (*stream)<<endl<<endl;
 
    if(fProjectData->fUEOIndices.UIndex == fProjectData->fUEOIndices.OIndex)
    {
        (*stream)<<"SOLUTION STRICTEMENT DETERMINEE: LES ECARTS-TYPES NE SONT PAS CALCULES!";
        (*stream)<<endl;
    }
    else if ((S0Aposteriori < S0UpLimit &&
        S0Aposteriori > S0LowLimit) || fProjectData->getConfig().useApriori.isActive() )  
    {
        (*stream)<<"LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)";
        (*stream)<<endl;
    }
    else
    {
        (*stream)<<"LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A POSTERIORI";
        (*stream)<<endl;
    }
 
    (*stream)<<endl;
 
    return;
}
 
 
void    TResultsFileWriter::writeSigmaAPosteriori(const TLGCData& data)
{
    TAStreamFormatter* stream = getStream();
    string separator = getSeparator();
 
    TReal S0Aposteriori = data.getS0APosteriori();
    TReal S0LowLimit = data.getChiS0LowLimit();
    TReal S0UpLimit = data.getChiS0UpLimit();
 
 
    //SIGMA ZERO A POSTERIORI
    stream->precision(5);
    stream->width( stream->getObsFormat()->getObsResidualWidth() );
    (*stream)<<"SIGMA ZERO A POSTERIORI ="<<S0Aposteriori;
 
    (*stream)<<", VALEUR CRITIQUE = (";
    stream->precision(5);
    stream->width( stream->getObsFormat()->getObsResidualWidth() );
    (*stream)<<S0LowLimit<<", ";
 
    stream->precision(5);
    stream->width( stream->getObsFormat()->getObsResidualWidth() );
        (*stream)<<S0UpLimit<<")";
        (*stream)<<endl<<endl;
 
    if(fProjectData->fUEOIndices.UIndex == fProjectData->fUEOIndices.OIndex)
    {
        (*stream)<<"SOLUTION STRICTEMENT DETERMINEE: LES ECARTS-TYPES NE SONT PAS CALCULES!";
        (*stream)<<endl;
    }
    else if ((S0Aposteriori < S0UpLimit &&
        S0Aposteriori > S0LowLimit) /*|| isSAPrioriUsed()*/)   //  || isSAPrioriUsed()  INVESTIGATE WHAT DOES THIS EXACTLY MEAN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
        (*stream)<<"LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A PRIORI (EGAL A 1)";
        (*stream)<<endl;
    }
    else
    {
        (*stream)<<"LES ECARTS-TYPES SONT CALCULES PAR RAPPORT AU SIGMA ZERO A POSTERIORI";
        (*stream)<<endl;
    }
 
    (*stream)<<endl;
 
    return;
}
 
 
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//HEADER
//////////////////////////////////////////////////////////////////////////////////////////////////////////

 
//Write the observations results
void    TResultsFileWriter::writeFramesResults()
{
    TAStreamFormatter*    stream = getStream();
    TFRAMEWriter frameWriter(*stream, fProjectData);
 
    //Tteration through the tree nodes
    for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++){    
        frameWriter.writeFRAMEAll(itTree);    //Writes  
    }
}
 
 
void TResultsFileWriter::transfUsingGeoid(TPositionVector& pv, const TRefSystemFactory::ERefFrame& rf){
        if(rf == TRefSystemFactory::ERefFrame::kCERNXYHsSphereSPS)
            TXYH2CCS::CCS2XYHs(pv);
        else if(rf == TRefSystemFactory::ERefFrame::kCernXYHg00Machine)
            TXYH2CCS::CCS2XYHg2000Machine(pv);
        else if (rf == TRefSystemFactory::ERefFrame::kCernXYHg85Machine)
            TXYH2CCS::CCS2XYHg1985Machine(pv);
}
 
 
 
// write the summary for the number of measurements of the given description
void    TResultsFileWriter::writeMeasDataSummary(const string description, const int numObs)  
{
    TAStreamFormatter &stream = getStreamRef();
 
    //insure impose de mettre les endl en en retour de ligne....
    stream<<"             LECTURE DES " << description;
    stream<<endl;
    stream<<"+        OK :";
    stream<<endl<<endl;
    stream<<"             NOMBRE=    "<<numObs;
    stream<<endl<<endl<<endl;
 
    return;
}