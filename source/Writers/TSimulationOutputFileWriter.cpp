////////////////////////////////////////////////////////////////////
// TSimulationOutputFileWriter.cpp :Implementation file
// Write an LGC output file for the results of a simulation calculation
// Creates a file from the calculation results and sends the appropriate messages
/////////////////////////////////////////////////////////////////////


#include	<ctime>
#include	"TSimulationOutputFileWriter.h"
#include	"TLGCData.h"
#include	"TAStreamFormatter.h"
#include	"TFRAMEWriter.h"


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TSimulationOutputFileWriter::TSimulationOutputFileWriter() : TResultsFileWriter()
{//default constructor
}


TSimulationOutputFileWriter::TSimulationOutputFileWriter(TAStreamFormatter* stream, const TLGCData* project) : TResultsFileWriter(stream, project)
{//constructor
}


TSimulationOutputFileWriter::~TSimulationOutputFileWriter()
{//destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////


// Write the first part of the LGC simulation results file for the given project
void	TSimulationOutputFileWriter::writeFileBegin()
{
	//Limited just number of points and unknowns for points for now
	this->initObsListNumber();	
	
	//write the header title of the results file
	this->writeTitle();

	// write the input data summary
	this->writeDataSummary();

	//Summary from the first simulation, like number of iterations
	this->writeCalcDataSummary();
}

void	TSimulationOutputFileWriter::writeSimSummary(TLGCData &project, int numberOfSim)
{
	//write simulation header
	this->writeSimHeader(project, numberOfSim);

	TFRAMEWriter frameWriter(*getStream(), &project);

	//Tteration through the tree nodes
	for (TDataTreeIterator itTree = project.getTree().begin(); itTree != project.getTree().end(); itTree++){
		frameWriter.writeFRAMESimu(itTree);	//Writes the simulation summary
	}
}


void	TSimulationOutputFileWriter::writeLastSimResult(TLGCData &project, int numberOfSim)
{
	//write simulation header
	this->writeSimHeader(project, numberOfSim);

	TFRAMEWriter frameWriter(*getStream(), fProjectData);

	//Tteration through the tree nodes
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++){	
		frameWriter.writeFRAMEAll(itTree);	//Write all the information (also the simulated observed values) for the last iteration
	}
}

//Write the header for a given simulation
void	TSimulationOutputFileWriter::writeSimHeader(TLGCData &data, int i)
{
	TAStreamFormatter* stream = getStream();
	string separator = getSeparator();
	
	//write title
	(*stream)<<"SIMULATION NUMERO"<<separator<< i <<endl<<
		"*********************"<<endl<<endl<<endl<<
		" DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !"<<endl<<endl<<endl;
	
	//write sigma0 a posteriori
	writeSigmaAPosteriori(data);
}

void TSimulationOutputFileWriter::writeSimTableDescription(const string &projTitle, const string &objectType, int i){
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter* stream = getStream();
	string origSepar = stream->getSeparator();
	stream->setSeparator("");
	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();

	(*stream)<<endl<<endl;
 	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<endl<<endl<<endl;

	//simulation summary
	(*stream)<<"RESUME APRES : " << i	<< " SIMULATIONS" <<endl;

	//write date and time
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
	(*stream)<<tmpbuf<<endl<<endl<<endl;

	//write title
	(*stream)<<projTitle<<endl;
	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<endl<<endl;


	//header
	(*stream).writeString(nameCoordWidth, objectType);
	(*stream).writeString(9*(coordResWidth+1)+44,"ECARTS-TYPES DES " + objectType);
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth+1+9*(coordResWidth+1)+44,"(SIG ZERO = 1         ");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth+1+9*(coordResWidth+1)+44," R = SIGMA/ECART-TYPE)");
	(*stream) << endl << endl;

	
	//Nom
	(*stream).writeStringLeft(nameCoordWidth,"NOM");
	(*stream).writeStringLeft(nameCoordWidth,"FRAME");

	//X offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DX ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DX ");

	//Y offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DY ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DY ");

	//Z offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DZ ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DZ ");
	
	//sigma X
	(*stream).writeString(2,"");
	(*stream).writeString(coordResWidth,"SX ");

	//sigma Y
	(*stream).writeString(coordResWidth,"SY ");

	//sigma Z
	(*stream).writeString(coordResWidth,"SZ ");
	(*stream) << endl;
	
	
	//units
	//space for name
	(*stream).writeStringLeft(nameCoordWidth,"");
	(*stream).writeString(nameCoordWidth,"");

	//X, Y, Z offset units
	for(int j = 0; j<3; j++)
	{
		(*stream).writeString(5,"");
		(*stream).writeString(coordResWidth,"(MM)");
		(*stream).writeString(7,"");
		(*stream).writeString(coordResWidth,"(MM)");
	}

	(*stream).writeString(2,"");

	//sigma X, Y, Z units
	for(int j = 0; j<3; j++)
	{
		(*stream).writeString(coordResWidth,"(MM)");
	}
	(*stream) << endl;

	stream->setSeparator(origSepar);
}

void TSimulationOutputFileWriter::writeSimFRAMEDescription(){
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter* stream = getStream();
	string origSepar = stream->getSeparator();
	stream->setSeparator("");
	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();

	(*stream)<<endl<<endl;
 	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<endl<<endl<<endl;

	//header
	(*stream).writeString(nameCoordWidth, "FRAME");
	(*stream).writeString(9*(coordResWidth+1)+44,"ECARTS-TYPES");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth+1+9*(coordResWidth+1)+44,"(SIG ZERO = 1         ");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth+1+9*(coordResWidth+1)+44," R = SIGMA/ECART-TYPE)");
	(*stream) << endl << endl;

	
	//Nom
	(*stream).writeStringLeft(nameCoordWidth,"FRAME");

	//TX offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DTX ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DTX ");

	//TY offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DTY ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DTY ");

	//TZ offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DTZ ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DTZ ");

	//RX offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DRX ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DRX ");

	//RY offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DRY ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DRY ");

	//RZ offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DRZ ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DRZ ");

	//SCL offset 
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"DSC ");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"DSC ");
	
	//sigma TX
	(*stream).writeString(2,"");
	(*stream).writeString(coordResWidth,"STX ");

	//sigma TY
	(*stream).writeString(coordResWidth,"STY ");

	//sigma TZ
	(*stream).writeString(coordResWidth,"STZ ");

	//sigma RX
	(*stream).writeString(coordResWidth,"SRX ");

	//sigma RY
	(*stream).writeString(coordResWidth,"SRY ");

	//sigma RZ
	(*stream).writeString(coordResWidth,"SRZ ");
	
	//sigma SCL
	(*stream).writeString(coordResWidth,"SSC ");
	(*stream) << endl;
	
	//units
	//space for name
	(*stream).writeString(nameCoordWidth,"");

	//translation offset units
	for(int j = 0; j<3; j++)
	{
		(*stream).writeString(5,"");
		(*stream).writeString(coordResWidth,"(MM)");
		(*stream).writeString(7,"");
		(*stream).writeString(coordResWidth,"(MM)");
	}

	//rotation offset units
	for(int j = 3; j<6; j++)
	{
		(*stream).writeString(5,"");
		(*stream).writeString(coordResWidth,"(CC)");
		(*stream).writeString(7,"");
		(*stream).writeString(coordResWidth,"(CC)");
	}

	//Scale offset
	(*stream).writeString(5,"");
	(*stream).writeString(coordResWidth,"(MM)");
	(*stream).writeString(7,"");
	(*stream).writeString(coordResWidth,"(MM)");

	(*stream).writeString(2,"");

	//sigma translation units
	for(int j = 0; j<3; j++)
	{
		(*stream).writeString(coordResWidth,"(MM)");
	}

	//sigma rotaion units
	for(int j = 0; j<3; j++)
	{
		(*stream).writeString(coordResWidth,"(CC)");
	}
	
	//scale sigma
	(*stream).writeString(coordResWidth,"(MM)");
	(*stream) << endl;

	stream->setSeparator(origSepar);
}
////////////////////////////////////////////////////////////
//STATISTIC
////////////////////////////////////////////////////////////
void	TSimulationOutputFileWriter::writeSimPointsSummary(const string &projTitle, const std::list<TSimPointSummary>& dataSum, int numbOfSimu)
{
	writeSimTableDescription(projTitle, "POINTS", numbOfSimu);

	for (auto& pointSummary : dataSum){
		writeSimPointData(pointSummary, numbOfSimu);
	}

}

void TSimulationOutputFileWriter::writeSimFramesSummary(const std::list<TSimFrameSummary>& dataSum, int numbOfSimu){
	writeSimFRAMEDescription();

	for (auto& frameSummary : dataSum){
		writeSimFRAMEData(frameSummary, numbOfSimu);
	}
}

void	TSimulationOutputFileWriter::writeSimPointData(const TSimPointSummary& simPt, const int i)
{//write point's data
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter* stream = getStream();
	string origSepar = stream->getSeparator();
	stream->setSeparator("");
	string separator = stream->getSeparator();;

	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();
	int coordResPrecision = this->getCoordErrorPrecision();

	// Table data line 1
	//write name
	(*stream).writeStringLeft(nameCoordWidth,simPt.getAdjustablePoint()->getName());
	std::stringstream result;
	const std::vector<int>& ID = simPt.getAdjustablePoint()->getFrameTreePosition().node->data.get()->ID;
	std::copy(ID.begin(), ID.end(), std::ostream_iterator<int>(result));
	(*stream).writeStringLeft(nameCoordWidth,simPt.getAdjustablePoint()->getFrameTreePosition().node->data.get()->frame.getName() + " (" + result.str() + ")");

	//get delta from LGC simulations' results
	TFreeVector deltaMoy = simPt.getSumRes() * (LITERAL(1.0)/i);
	TFreeVector deltaMax = simPt.getMaxRes();
	TFreeVector deltaMin = simPt.getMinRes();
	TFreeVector deltaSigma = simPt.getSumRes2();

	TReal sigmadx(sqrtq((deltaSigma.getX().getMMetresValue()
		-powq(deltaMoy.getX().getMMetresValue()*i,2)/i)*(LITERAL(1.0)/i)));
	TReal sigmady(sqrtq((deltaSigma.getY().getMMetresValue()
			-powq(deltaMoy.getY().getMMetresValue()*i,2)/i)*(LITERAL(1.0)/i)));
	TReal sigmadz(sqrtq((deltaSigma.getZ().getMMetresValue()
			-powq(deltaMoy.getZ().getMMetresValue()*i,2)/i)*(LITERAL(1.0)/i)));
	TReal sx(simPt.getAdjustablePoint()->getXEstPrecision().getMMetresValue());
	TReal sy(simPt.getAdjustablePoint()->getYEstPrecision().getMMetresValue());
	TReal sz(simPt.getAdjustablePoint()->getZEstPrecision().getMMetresValue());

	stream->setWidthFormat(coordResWidth);
	stream->setPrecisionFormat(coordResPrecision);

	int precisionMM = coordResPrecision > 3 ? (coordResPrecision - 3) : 0; //This is because the values are outputted in [mm] for the TLength the precision is cut for a factor of 3.
																		 // but we output 'double', we need to do it here!
	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		(*stream).writeString(5,"MAX");
		writeDouble(coordResWidth, precisionMM, deltaMax.getX().getMMetresValue());
		(*stream).writeString(7,"MOYEN");
		writeDouble(coordResWidth, precisionMM, deltaMoy.getX().getMMetresValue());
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}


	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		(*stream).writeString(5,"MAX");
		writeDouble(coordResWidth, precisionMM, deltaMax.getY().getMMetresValue());
		(*stream).writeString(7,"MOYEN");
		writeDouble(coordResWidth, precisionMM, deltaMoy.getY().getMMetresValue());
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}


	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		(*stream).writeString(5,"MAX");
		writeDouble(coordResWidth, precisionMM, deltaMax.getZ().getMMetresValue());
		(*stream).writeString(7,"MOYEN");
		writeDouble(coordResWidth, precisionMM, deltaMoy.getZ().getMMetresValue());
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	(*stream).writeString(2,"");
	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		writeDouble(coordResWidth, precisionMM, sx);
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		writeDouble(coordResWidth, precisionMM, sy);
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		writeDouble(coordResWidth, precisionMM, sz);
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}
	(*stream) << endl;


	// Table data line 2
	(*stream).writeStringLeft(nameCoordWidth, "");
	(*stream).writeString(nameCoordWidth,"");

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		(*stream).writeString(5,"MIN");
		writeDouble(coordResWidth, precisionMM, deltaMin.getX().getMMetresValue());
		(*stream).writeString(7,"SIGMA");
		writeDouble(coordResWidth, precisionMM, sigmadx);
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	
	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		(*stream).writeString(5,"MIN");
		writeDouble(coordResWidth, precisionMM, deltaMin.getY().getMMetresValue());
		(*stream).writeString(7,"SIGMA");
		writeDouble(coordResWidth, precisionMM, sigmady);
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}


	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		(*stream).writeString(5,"MIN");
		writeDouble(coordResWidth, precisionMM, deltaMin.getZ().getMMetresValue());
		(*stream).writeString(7,"SIGMA");
		writeDouble(coordResWidth, precisionMM, sigmadz);
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	(*stream).writeString(2,"R");
	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		writeDouble(coordResWidth, precisionMM, sigmadx/sx); //This is unitless, but we want the same precision as for [mm]
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		writeDouble(coordResWidth, precisionMM, sigmady/sy); //This is unitless, but we want the same precision as for [mm]
	}
	else
	{
		(*stream).writeString(coordResWidth, ""); 
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		writeDouble(coordResWidth, precisionMM, sigmadz/sz);	//This is unitless, but we want the same precision as for [mm]
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}
	(*stream) << endl;


	// Table data line 3
	//write overal sigma
	(*stream).writeString(nameCoordWidth, "OVERALL SIGMA =");
	(*stream) <<  (TReal) (sqrtq(powq(sigmadx,2)
		+powq(sigmady,2)+powq(sigmadz,2))) << separator;
	(*stream) << endl << endl;

	stream->setSeparator(origSepar);

}

void	TSimulationOutputFileWriter::writeSimFRAMEData(const TSimFrameSummary& simFr, const int i)
{
	//writeframe's data
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter* stream = getStream();
	string origSepar = stream->getSeparator();
	stream->setSeparator("");
	string separator = stream->getSeparator();;

	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();
	int coordResPrecision = this->getCoordErrorPrecision();

	// Table data line 1
	//write name
	(*stream).writeStringLeft(nameCoordWidth,simFr.getAdjustableTransformation()->getName());

	//get delta from LGC simulations' results
	TransformParameters deltaMoy = simFr.getSumRes();
	deltaMoy.kappa *= (LITERAL(1.0)/i);
	deltaMoy.phi *= (LITERAL(1.0)/i);
	deltaMoy.omega *= (LITERAL(1.0)/i);
	deltaMoy.tX *= (LITERAL(1.0)/i);
	deltaMoy.tY *= (LITERAL(1.0)/i);
	deltaMoy.tZ *= (LITERAL(1.0)/i);
	deltaMoy.scale *= (LITERAL(1.0)/i);

	TransformParameters deltaMax = simFr.getMaxRes();
	TransformParameters deltaMin = simFr.getMinRes();
	TransformParametersSquare deltaSigma = simFr.getSumRes2();

	TReal sigmadtx(sqrtq((deltaSigma.tX.getMMetresValue() - powq(deltaMoy.tX.getMMetresValue()*i,2)/i) *( LITERAL(1.0)/i)));
	TReal sigmadty(sqrtq((deltaSigma.tY.getMMetresValue() - powq(deltaMoy.tY.getMMetresValue()*i,2)/i) * (LITERAL(1.0) / i)));
	TReal sigmadtz(sqrtq((deltaSigma.tZ.getMMetresValue() - powq(deltaMoy.tZ.getMMetresValue()*i,2)/i) * (LITERAL(1.0)/i)));
	TReal sigmadrx(sqrtq((deltaSigma.omega - powq(deltaMoy.omega.getRadiansValue() *i, 2) / i) * (LITERAL(1.0) / i)));  // sigma are in rad, need to convert in CC
	TReal sigmadry(sqrtq((deltaSigma.phi - powq(deltaMoy.phi.getRadiansValue()*i, 2) / i) * (LITERAL(1.0) / i)));       // sigma are in rad, need to convert in CC
	TReal sigmadrz(sqrtq((deltaSigma.kappa - powq(deltaMoy.kappa.getRadiansValue()*i, 2) / i) * (LITERAL(1.0) / i)));   // sigma are in rad, need to convert in CC
	TReal sigmadscl(sqrtq((deltaSigma.scale*M2MM - powq(deltaMoy.scale*M2MM*i,2)/i) * (LITERAL(1.0)/i)));

	TReal stx(simFr.getAdjustableTransformation()->getEstimatedPrecisionTransl(0).getMMetresValue()); //Because sigmadtX are in M
	TReal sty(simFr.getAdjustableTransformation()->getEstimatedPrecisionTransl(1).getMMetresValue());
	TReal stz(simFr.getAdjustableTransformation()->getEstimatedPrecisionTransl(2).getMMetresValue());
	TReal srx(simFr.getAdjustableTransformation()->getEstimatedPrecisionRot(0).getSignedCCValue());
	TReal sry(simFr.getAdjustableTransformation()->getEstimatedPrecisionRot(1).getSignedCCValue());
	TReal srz(simFr.getAdjustableTransformation()->getEstimatedPrecisionRot(2).getSignedCCValue());

	stream->setWidthFormat(coordResWidth);
	stream->setPrecisionFormat(coordResPrecision);

	int precisionMM = coordResPrecision > 5 ? (coordResPrecision - 5) : 0; //This is because the values are outputted in [mm] 
	if(!simFr.getAdjustableTransformation()->isFixed())
	{

		//line 1
		//D translation
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			(*stream).writeString(5,"MAX");
			writeDouble(coordResWidth, precisionMM, deltaMax.tX.getMMetresValue());
			(*stream).writeString(7,"MOYEN");
			writeDouble(coordResWidth, precisionMM, deltaMoy.tX.getMMetresValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			(*stream).writeString(5,"MAX");
			writeDouble(coordResWidth, precisionMM, deltaMax.tY.getMMetresValue());
			(*stream).writeString(7,"MOYEN");
			writeDouble(coordResWidth, precisionMM, deltaMoy.tY.getMMetresValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			(*stream).writeString(5,"MAX");
			writeDouble(coordResWidth, precisionMM, deltaMax.tZ.getMMetresValue());
			(*stream).writeString(7,"MOYEN");
			writeDouble(coordResWidth, precisionMM, deltaMoy.tZ.getMMetresValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		//D rotations
		if(!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			(*stream).writeString(5,"MAX");
			writeDouble(coordResWidth, precisionMM, deltaMax.omega.getSignedCCValue());
			(*stream).writeString(7,"MOYEN");
			writeDouble(coordResWidth, precisionMM, deltaMoy.omega.getSignedCCValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			(*stream).writeString(5,"MAX");
			writeDouble(coordResWidth, precisionMM, deltaMax.phi.getSignedCCValue());
			(*stream).writeString(7,"MOYEN");
			writeDouble(coordResWidth, precisionMM, deltaMoy.phi.getSignedCCValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			(*stream).writeString(5,"MAX");
			writeDouble(coordResWidth, precisionMM, deltaMax.kappa.getSignedCCValue());
			(*stream).writeString(7,"MOYEN");
			writeDouble(coordResWidth, precisionMM, deltaMoy.kappa.getSignedCCValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		//D scale
		if(!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			(*stream).writeString(5,"MAX");
			writeDouble(coordResWidth, precisionMM, deltaMax.scale * M2MM);
			(*stream).writeString(7,"MOYEN");
			writeDouble(coordResWidth, precisionMM, deltaMoy.scale * M2MM);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		//sigma
		(*stream).writeString(2,"");
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			writeDouble(coordResWidth, precisionMM, stx);
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			writeDouble(coordResWidth, precisionMM, sty); 
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			writeDouble(coordResWidth, precisionMM, stz); 
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			writeDouble(coordResWidth, precisionMM, srx); 
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			writeDouble(coordResWidth, precisionMM, sry); 
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			writeDouble(coordResWidth, precisionMM, srz); 
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			writeDouble(coordResWidth, precisionMM, simFr.getAdjustableTransformation()->getEstimatedPrecisionScale() * M2MM); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}

		(*stream) << endl;

		//line 2
		(*stream).writeStringLeft(nameCoordWidth,"");
		//D translation
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			(*stream).writeString(5,"MIN");
			writeDouble(coordResWidth, precisionMM, deltaMin.tX.getMMetresValue() );
			(*stream).writeString(7,"SIGMA");
			writeDouble(coordResWidth, precisionMM, sigmadtx);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			(*stream).writeString(5,"MIN");
			writeDouble(coordResWidth, precisionMM, deltaMin.tY.getMMetresValue());
			(*stream).writeString(7,"SIGMA");
			writeDouble(coordResWidth, precisionMM, sigmadty);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			(*stream).writeString(5,"MIN");
			writeDouble(coordResWidth, precisionMM, deltaMin.tZ.getMMetresValue());
			(*stream).writeString(7,"SIGMA");
			writeDouble(coordResWidth, precisionMM, sigmadtz);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		//D rotations
		if(!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			(*stream).writeString(5,"MIN");
			writeDouble(coordResWidth, precisionMM, deltaMin.omega.getSignedCCValue());
			(*stream).writeString(7,"SIGMA");
			writeDouble(coordResWidth, precisionMM, sigmadrx * RAD2CC);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			(*stream).writeString(5,"MIN");
			writeDouble(coordResWidth, precisionMM, deltaMin.phi.getSignedCCValue());
			(*stream).writeString(7,"SIGMA");
			writeDouble(coordResWidth, precisionMM, sigmadry * RAD2CC);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}
		
		if(!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			(*stream).writeString(5,"MIN");
			writeDouble(coordResWidth, precisionMM, deltaMin.kappa.getSignedCCValue());
			(*stream).writeString(7,"SIGMA");
			writeDouble(coordResWidth, precisionMM, sigmadrz * RAD2CC);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		//D scale
		if(!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			(*stream).writeString(5,"MIN");
			writeDouble(coordResWidth, precisionMM, deltaMin.scale * M2MM);
			(*stream).writeString(7,"SIGMA");
			writeDouble(coordResWidth, precisionMM, sigmadscl * M2MM);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		//sigma
		(*stream).writeString(2,"R");
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			writeDouble(coordResWidth, precisionMM, sigmadtx/stx); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			writeDouble(coordResWidth, precisionMM, sigmadty/sty); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			writeDouble(coordResWidth, precisionMM, sigmadtz/stz); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			writeDouble(coordResWidth, precisionMM, sigmadrx * RAD2CC / srx); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			writeDouble(coordResWidth, precisionMM, sigmadry * RAD2CC / sry); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			writeDouble(coordResWidth, precisionMM, sigmadrz * RAD2CC / srz); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if(!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			writeDouble(coordResWidth, precisionMM, sigmadscl / simFr.getAdjustableTransformation()->getEstimatedPrecisionScale()); //This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}

		(*stream) << endl;

		//line 3
		//write overal sigma
		(*stream).writeString(nameCoordWidth, "OVERALL SIGMA =");
		(*stream) <<  (TReal) (sqrtq(powq(sigmadtx,2)+powq(sigmadty,2)+powq(sigmadtz,2)
			+ powq(sigmadrx,2)+powq(sigmadry,2)+powq(sigmadrz,2)+powq(sigmadscl*M2MM,2))) << separator;
		(*stream) << endl << endl;

		stream->setSeparator(origSepar);
	}
}

////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////