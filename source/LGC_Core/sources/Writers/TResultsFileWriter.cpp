////////////////////////////////////////////////////////////////////
// TResultsFileWriter.cpp :Implementation file
// Write an LGC output file
// Creates a file from the calculation results and sends the appropriate messages
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////////
 
#include <ctime>

//LGC Core
#include <TLGCData.h>
#include "TResultsFileWriter.h"
#include "TLGCApp.h"
#include "TLOR2LOR.h"
#include "TFRAMEWriter.h"
#include "TLibrCnstrGenerator.h"

//SurveyLib
#include "TSeparatedFormatTStream.h"
#include "TPointConverter.h"
#include <TObservationWriter.h>
#include "TSpatialStatus.h"
#include "TXYH2CCS.h"
 
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
void    TResultsFileWriter::writeFile(const std::string error)
{//write error messages from project
    writeTitle();
    writeError(error);
}
 
// Write the LGC results for the given project
void    TResultsFileWriter::writeFile()
{
	TAStreamFormatter* stream = getStream();
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

	if (!getDataSet()->getConfig().erelPairs.empty())
	{
		this->writeRelErrorHeader();
		this->writeRelErrorResults(*getDataSet());
	}

	//FIN DE FICHIER
	(*stream) << "*** FIN DE FICHIER ***";
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
    (*stream)<<endl;
 
    //write software id.
    (*stream)<<(TLGCApp::getProgId())<<endl;
 
    //write software copyright
    (*stream)<<(TLGCApp::getCopyright())<<endl;
 
    //write title
    (*stream)<<"*********************************************************************************************************************************** "<<endl;
    (*stream)<<(fProjectData->getConfig().title)<<endl;
    (*stream)<<endl;
 
    //write time
	char tmpbuf[128];
	time_t ltime;
    time(&ltime);
	struct tm *today;
#if defined(__linux__) || defined(__APPLE__)
	tzset();
#else
	_tzset();
#endif
	today = localtime( &ltime );
	std::string essai = ctime( &ltime );
    strftime( tmpbuf, 128,"CALCUL DU %d %B %Y %X", today );
    (*stream)<<tmpbuf<<endl;
    (*stream)<<"*********************************************************************************************************************************** "<<endl<<endl;
 
}
 
 
// Write a summary of the LGC input data
void    TResultsFileWriter::writeDataSummary()
{
    TAStreamFormatter* stream = getStream();
 
	(*stream) << "DATA SET -  INFO GENERAL:" << endl << endl;
    (*stream)<<"\tFRAMES :" << int(fProjectData->getNumberOfFrames()) << endl << endl;
 
    if(fProjectData->getPoints().numObjects()>0)
       writeAdjustableObjGeneralInfo("POINTS", (int)fProjectData->getPoints().numObjects(), (int)fProjectData->getPoints().numUnknowns());
    if(fProjectData->getLines().numObjects()>0)
       writeAdjustableObjGeneralInfo("LIGNES", (int)fProjectData->getLines().numObjects(), (int)fProjectData->getLines().numUnknowns());
 
    if(fProjectData->getPlanes().numObjects()>0)
       writeAdjustableObjGeneralInfo("PLANS", (int)fProjectData->getPlanes().numObjects(), (int)fProjectData->getPlanes().numUnknowns());
 
    if(fProjectData->getAngles().numObjects()>0)
       writeAdjustableObjGeneralInfo("ANGLES", (int)fProjectData->getAngles().numObjects(), (int)fProjectData->getAngles().numUnknowns());
 
    if(fProjectData->getLength().numObjects()>0)
       writeAdjustableObjGeneralInfo("DISTANCES", (int)fProjectData->getLength().numObjects(), (int)fProjectData->getLength().numUnknowns());

 
    int fNumFixedPoint = fProjectData->getPointsDimension(TSpatialStatus::kCala);
    int fNumVxyzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVxyz);
    int fNumVyzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVyz);
    int fNumVxzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVxz);
    int fNumVxyPoint = fProjectData->getPointsDimension(TSpatialStatus::kVxy);
    int fNumVxPoint = fProjectData->getPointsDimension(TSpatialStatus::kVx);
    int fNumVyPoint = fProjectData->getPointsDimension(TSpatialStatus::kVy);
    int fNumVzPoint = fProjectData->getPointsDimension(TSpatialStatus::kVz);
 
	(*stream) << "POINTS : " << endl << endl;
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
	int fNumECDIR = fProjectData->getMeasurementDimension(TMeasurementsGlobal::kECDIR);
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
 
	(*stream) << endl << "MESURES :" << endl << endl;
    if(fNumHorAng != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kANGL), fNumHorAng);    
 
    if(fNumZenDist != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kZEND), fNumZenDist);    
 
    if(fNumDistMeas != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kDIST), fNumDistMeas);    
    
    if(fNumPLR3D != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kPLR3D), fNumPLR3D);

	if (fNumECDIR != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kECDIR), fNumECDIR);
	
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

    if(fNumORIE != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kORIE), fNumORIE);
 
    if(fNumPDOR != 0)
        writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kPDOR), fNumPDOR);

	if (fNumRADI != 0)
		writeMeasDataSummary(TObservationWriter::getObsDescriptionFR(TALGCObjectWriter::kRADI), fNumRADI);
 
 
    //FIN DE FICHIER
    (*stream)<<endl<<"*** STATISTIQUES ***";
    (*stream)<<endl<<endl;
}
 
 
// write the summary for the number of points of the given description  
void    TResultsFileWriter::writeAdjustableObjGeneralInfo(const std::string adjObjName, const int numAdj, const int numUnkn)  
{
    TAStreamFormatter &stream = getStreamRef();
    stream<<"\t"<< adjObjName << " : " <<numAdj << endl;
    stream<<"\t Inconuues introduites " << adjObjName << ": "<<numUnkn << endl<<endl;
}
 
 
// write the summary for the number of points of the given description  
void    TResultsFileWriter::writePointDataSummary(const std::string description, const int numPts)
{
    TAStreamFormatter &stream = getStreamRef();
 
    stream<<"	LECTURE DES POINTS " << description<<" : "<<numPts<<endl;
    return;
}
 
 
// Write a summary of the LGC calculation data parameters
void    TResultsFileWriter::writeCalcDataSummary()
{
    TAStreamFormatter &stream = getStreamRef();
 
    //NOMBRE D'OBSERVATIONS
    stream<<"	NOMBRE D'OBSERVATIONS =  "<< fProjectData->fUEOIndices.OIndex;
	if (fProjectData->getConfig().pdor.isActive())
	{
		stream << "	(PDOR INCLUS - ATTENTION, PREMIER CALA DEFINI DANS LE ROOT UTILISE";
	}
    stream<<endl;
 
    //NOMBRE D'INCONNUES
    stream<<"	NOMBRE D'INCONNUES =     "<< fProjectData->fUEOIndices.UIndex;
    stream<<endl;
 
    // NUMBER OF ITERATIONS
    stream<<"	NUMBER OF ITERATIONS =     "<<fProjectData->getNumberOfLSIterations();
    stream<<endl<<endl;
 
    //RESEAU COMPLETEMENT LIBRE ?
	if (fProjectData->getConfig().libre.isActive())
	{
		if (fProjectData->getNumberOfConstraints() != 0)
		{
			stream << "             ************************************";
			stream << endl;
			stream << "             *                                  *";
			stream << endl;
			stream << "             *     RESEAU COMPLETEMENT LIBRE    *";
			stream << endl;
			stream << "             *                                  *";
			stream << endl;
			stream << "             ************************************";
			stream << endl << endl;

			stream << "             LES PARAMETRES QUI MANQUENT SONT :";
			stream << endl << endl;

			TPointTransformer fPointTransformer(& fProjectData->getTree(), fProjectData->getConfig().referential);
			TLibrCnstrGenerator fCnstrGenerator(fPointTransformer, *fProjectData);
			fCnstrGenerator.initCnstrIdentifier(*fProjectData);
			struct isFreeCnstr cnstr = fCnstrGenerator.getCnstIdentifier();
			if (cnstr.dx == 1)
			{
				stream << "             ORIGINE D'AXE X";
				stream << endl;
			}
			if (cnstr.dy == 1)
			{
				stream << "             ORIGINE D'AXE Y";
				stream << endl;
			}
			if (cnstr.dz == 1)
			{
				stream << "             ORIGINE D'AXE Z";
				stream << endl;
			}
			if (cnstr.rx == 1)
			{
				stream << "             ORIENTATION DANS LE PLAN YZ";
				stream << endl;
			}
			if (cnstr.ry == 1)
			{
				stream << "             ORIENTATION DANS LE PLAN XZ";
				stream << endl;
			}
			if (cnstr.rz == 1)
			{
				stream << "             ORIENTATION DANS LE PLAN XY";
				stream << endl;
			}
			if (cnstr.k == 1)
			{
				stream << "             ECHELLE";
				stream << endl;
			}

			stream << endl << endl << endl << endl;

		}
		else
		{
			stream << "             ERREUR : OPTION *LIBR CHOISIE SANS JUSTIFICATION";
			stream << endl << endl << endl << endl << endl;
		}
	}
 
    return;
}
 
 
void    TResultsFileWriter::writeSigmaAPosteriori()
{
    TAStreamFormatter* stream = getStream();
	std::string separator = getSeparator();
 
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
        (*stream)<<endl;
 
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
	std::string separator = getSeparator();
 
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
        (*stream)<<endl;
 
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
void    TResultsFileWriter::writeMeasDataSummary(const std::string description, const int numObs)
{
    TAStreamFormatter &stream = getStreamRef();
 
    //insure impose de mettre les endl en en retour de ligne....
    stream<<"	LECTURE DES " << description<<" : "<<numObs<<endl;
 
    return;
}

void	TResultsFileWriter::writeRelErrorHeader()
{
	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	int					obsResWidth = std::max(getObsResWidth(), 9);

	// write header
	(*stream) << endl << endl << endl;
	(*stream) << "ERREURS RELATIVES " << endl;
	(*stream) << "*******************" << endl << endl;

	//////////////////////////////////////////////////////////////
	//line1
	// point 1 & 2
	(*stream).writeStringLeft(nameWidth, "POINT 1");
	(*stream).writeStringLeft(nameWidth, "POINT 2");
	// Sigmas
	(*stream).writeString(obsResWidth, "SIGMA L");
	(*stream).writeString(obsResWidth, "SIGMA G");
	(*stream).writeString(obsResWidth, "SIGMA R");
	(*stream).writeString(obsResWidth, "SIGMA Z");
	(*stream).writeString(obsResWidth, "SIGMA V");
	(*stream) << endl;

	//////////////////////////////////////////////////////////////
	//line2
	// units
	(*stream).writeString(nameWidth, " ");
	(*stream).writeString(nameWidth, " ");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream) << endl << endl;

	return;
}

void	TResultsFileWriter::writeRelErrorResults(const TLGCData& data)
{

	TAStreamFormatter*	stream = getStream();
	int					nameWidth = getNameWidth();
	std::string				separator = getSeparator();

	for (auto& ptPairIt : data.getRelError())
	{
		// write points name

		(*stream).writeStringLeft(nameWidth, ptPairIt.getPoint1Name());
		(*stream).writeStringLeft(nameWidth, ptPairIt.getPoint2Name());
		// sets the values format:
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setPrecisionFormat(getLengthPrecision());
		stream->setWidthFormat(std::max(getObsResWidth(), 9));

		//sigma L
		(*stream) << right << ptPairIt.getSigmaL() << separator;

		//sigma G			
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << ptPairIt.getSigmaG() << separator;

		//sigma R
		stream->setPrecisionFormat(getLengthPrecision());
		(*stream) << right << ptPairIt.getSigmaR() << separator;

		//sigma Z
		(*stream) << right << ptPairIt.getSigmaZ() << separator;

		//sigma V
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << ptPairIt.getSigmaV() << separator << endl;
		//(*stream).setDataSpacing();
	}

	return;
}