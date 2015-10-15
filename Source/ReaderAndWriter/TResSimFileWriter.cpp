////////////////////////////////////////////////////////////////////
// TResSimFileWriter.cpp :Implementation file
// Write an LGC output file for the results of a simulation calculation
// Creates a file from the calculation results and sends the appropriate messages
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// other forward declarations
#include	<ctime>
#include	"TLGCApplication.h"
#include	"TLGCProject.h"
#include	"TLGCDataSet.h"
#include	"LSCalcDataSet.h"
#include	"TSeparatedFormatTStream.h"
#include	"TPointFormat.h"
#include	"TResSimFileWriter.h"
#include	"TSpatialStatus.h"
#include	"TLGCSimResults.h"

#include	"TPointConverter.h"
#include	"THorAngleConverter.h"
#include	"TZenithDistConverter.h"
#include	"TGyroOrientationConverter.h"
#include	"TVertDistConverter.h"
#include	"THorDistConverter.h"
#include	"TSpatialDistConverter.h"
#include	"TOffsetToVerLineConverter.h"
#include	"TOffsetToVerPlaneConverter.h"
#include	"TOffsetToSpaLineConverter.h"
#include	"TOffsetToTheoPlaneConverter.h"
#include	"TRadialOffsetCnstrConverter.h"


/////////////////////////////////////////////////////////////////////

//ClassImp(TResSimFileWriter)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TResSimFileWriter::TResSimFileWriter() : TResultsFileWriter()
{//default constructor
}


TResSimFileWriter::TResSimFileWriter(TAStreamFormatter* stream, const TLGCProject* project) : TResultsFileWriter(stream, project)
{//constructor
}


TResSimFileWriter::~TResSimFileWriter()
{//destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////


// Write the LGC simulation results for the given project
void	TResSimFileWriter::writeFile(TLGCProject &project)
{
	LSCalcDataSet* lsds = project.getLSCalcDataSet();
	TLGCDataSet *ds = project.getDataSet();

	this->init(ds, lsds, project.getError());
	this->initObsListNumber(ds);
	
	//write the header title of the results file
	this->writeTitle();
	// write the input data summary
	this->writeDataSummary(project);


	//either write the results or write the project error messages
	if(getError()=="")
	{
		// write the calculation data parameters
		this->writeCalcDataSummary();

		// write the summary data for the simulations
		//writer->writeSimPartFile(data, lsCalcDS, numOfMadeSim+1);
		this->writeSimSummaries(project);

		// write the standard output for the last simulation calculation
		this->writeLastSimResult(project);
		
		// write the final summary for the statistics on the estimated 
		// point coordinates
		//writer->writeSimSummary(*simRes, (int)project->getCalcParams()->getNumOfSim());
		this->writeSimPointsSummary(project.getTitle(), *(project.getLSCalcDataSet()), (int)project.getCalcParams()->getNumOfSim());


		// write relative errors (EREL)
		this->writeRelativeErrors();
		
	}
	else
	{
		this->writeError();
	}
	return;
}


void	TResSimFileWriter::writeSimSummaries(TLGCProject &project)
{//Write each simulation summary -for all but the last simulation, if there no errors in the project

	int i;
	string separator = getSeparator();
	CalcRecordIter calcRec    = project.getLSCalcDataSet()->beginSimCalcRec();
	CalcRecordIter calcRecEnd = project.getLSCalcDataSet()->endSimCalcRec();
	int penultimate = (project.getCalcParams()->getNumOfSim()) - 1;

	for( i=0; calcRec != calcRecEnd && i < penultimate; i++)
	{
		//write simulation header
		this->writeSimHeader(*calcRec, i+1);

		//Write Points
		this->writePointsResults(project.getCalcParams()->getRefSurfaceName(), calcRec->beginPoint(), calcRec->endPoint());

		//Write Observation summaries
		ObsSummaryIter iter = calcRec->beginObsSumm();
		ObsSummaryIter iterEnd = calcRec->endObsSumm();

		// iterate through the list of observation summaries
		while( iter != iterEnd)
		{
			// write the title line for the observations
			this->writeObsTitle(getObsDescriptionFR(iter->first), iter->second.getNumberOfObs());
			
			// check if observations are of an angle or length type
			// and call the appropriate method to output the observation summary
			if( iter->second.isAngleType() )
			{
				this->writeAngleObsSummary(iter->second, getObsDescriptionFR(iter->first));
			}
			else
			{
				this->writeLengthObsSummary(iter->second, getObsDescriptionFR(iter->first));
			}

			// increment the iterator
			iter++;
		}

		calcRec++;
	}

	return;
}



//Write the full lgc results for the last simulation calculation
void	TResSimFileWriter::writeLastSimResult(TLGCProject &project)
{
	LSCalcDataSet	*lsds = project.getLSCalcDataSet();
	CalcRecordIter calcRec = lsds->endSimCalcRec();

	// Nothing to do here (empty vector)
	if (calcRec != lsds->beginSimCalcRec()) 
	{
		// move the iterator to the last record
		 calcRec--;

		// write simulation header
		this->writeSimHeader(*calcRec, project.getCalcParams()->getNumOfSim());

		//Write the calculation results for the points
		//pass the begin and end iterators to the list of calculated position
		//vectors in the LSCalcDataSet
		this->writePointsResults( project.getCalcParams()->getRefSurfaceName(), lsds->beginPV(), lsds->endPV());
	}

	// Write Observations Results
	this->writeObsResults( *lsds );

	return;
}


//Write the header for a given simulation
void	TResSimFileWriter::writeSimHeader(TLGCCalcRecord &calcRec, int i)
{
	TAStreamFormatter* stream = getStream();
	string separator = getSeparator();
	
	//write title
	(*stream)<<"SIMULATION NUMERO"<<separator<< i <<endl<<
		"*********************"<<endl<<endl<<endl<<
		" DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !"<<endl<<endl<<endl;
	
	//write sigma0 a posteriori
	writeSigmaAPosteriori(calcRec);

	return;
}



////////////////////////////////////////////////////////////
//STATISTIC
////////////////////////////////////////////////////////////
void	TResSimFileWriter::writeSimPointsSummary(const string &projTitle, const LSCalcDataSet& lsds, int i)
{
	TAStreamFormatter* stream = getStream();
	string separator = getSeparator();
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
	_tzset();
    today = localtime( &ltime );
	string essai = ctime( &ltime );
	strftime( tmpbuf, 128,"CALCUL DU %d %B %Y %X", today );
	(*stream)<<tmpbuf<<endl<<endl<<endl;

	//write title
	(*stream)<<projTitle<<endl;
	(*stream)<<"*********************************************************************************************************************************** "<<endl;
	(*stream)<<endl<<endl;


	//header
	(*stream).writeString(nameCoordWidth,"POINTS");
	(*stream).writeString(9*(coordResWidth+1)+44,"ECARTS-TYPES DES POINTS");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth+1+9*(coordResWidth+1)+44,"(SIG ZERO = 1         ");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth+1+9*(coordResWidth+1)+44," R = SIGMA/ECART-TYPE)");
	(*stream) << endl << endl;

	
	//Nom
	(*stream).writeString(nameCoordWidth,"NOM");

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


	PointConstIter		pointIterator, endPointIterator;
	SimPointConstIter	simPt, endSimPt;
	endSimPt			= lsds.endSimResPoint();
	pointIterator		= getDataSet()->getWorkingPoints()->getPointsBeginIterator();
	endPointIterator	= getDataSet()->getWorkingPoints()->getPointsEndIterator();

	while(pointIterator != endPointIterator)
	{
		// iterate through the input point data and check that the point is not a fixed point
		TSpatialStatus::ESpatialStatus status = pointIterator->getPosition().getObjectStatus();
		if(status != TSpatialStatus::kCala)
		{
			string nameApp, nameCalc, name;
			simPt = lsds.beginSimResPoint();
			nameApp = pointIterator->getName().getName();
			nameCalc = simPt->getPosVec()->getName();
		
			// iterate through the list of calculated point data and use
			// the point names to find the corresponding calculated point
			// in order to write the simulated point summaries in the same order 
			// as the lsit of points in the input file
			bool stop = true;
			while(nameApp != nameCalc && stop)
			{
				// calculated point not found yet so move on to next point
				simPt++;
				if(simPt == endSimPt)
				{
					// reached end of point list so flag to stop search
					stop = false;
				}
				else
				{
					// get the current point name for comparison with the data point
					nameCalc = simPt->getPosVec()->getName();
				}
			}

			// not necessary!??
			name = pointIterator->getName().getName();

			// data point not in the lsit of calculated points
			if(nameApp != nameCalc)
			{
				// is this the right error message ???
				string error = "Les coordonnées du point, " + nameApp + ", ne peuvent pas être déterminées.";
				setError(error);
			}
			else
			{
				if (getError()=="")
				{
					//write results
					writeSimPointData(simPt, i);
				}
			}
		}
		pointIterator++;
	}

	(*stream)<<endl<<endl<<endl;

	return;
}


void	TResSimFileWriter::writeSimPointData(SimPointConstIter simPt, const int i)
{//write point's data
	TAStreamFormatter* stream = getStream();
	string separator = getSeparator();
	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();
	int coordResPrecision = this->getCoordErrorPrecision();
	
	//stock unit	
	TLength::EUnits unit = stream->getLengthUnits();

	// Table data line 1
	//write name
	(*stream).writeString(nameCoordWidth,simPt->getPosVec()->getName());

	//get delta from LGC simulations' results
	TFreeVector deltaMoy = simPt->getSumRes() * (LITERAL(1.0)/i);
	TFreeVector deltaMax = simPt->getMaxRes();
	TFreeVector deltaMin = simPt->getMinRes();
	TFreeVector deltaSigma = simPt->getSumRes2();

	TLength sigmadx(sqrtq((deltaSigma.getX().getMetresValue()
			-powq(deltaMoy.getX().getMetresValue()*i,2)/i)*(LITERAL(1.0)/i)));
	TLength sigmady(sqrtq((deltaSigma.getY().getMetresValue()
			-powq(deltaMoy.getY().getMetresValue()*i,2)/i)*(LITERAL(1.0)/i)));
	TLength sigmadz(sqrtq((deltaSigma.getZ().getMetresValue()
			-powq(deltaMoy.getZ().getMetresValue()*i,2)/i)*(LITERAL(1.0)/i)));
	TLength sx(simPt->getPosVec()->getEstimatedPrecision().getX());
	TLength sy(simPt->getPosVec()->getEstimatedPrecision().getY());
	TLength sz(simPt->getPosVec()->getEstimatedPrecision().getZ());


	int oldFormat = stream->setWidthFormat(6);
	stream->setLengthUnits(TLength::kMillimetres);
	stream->setWidthFormat(coordResWidth);
	stream->setPrecisionFormat(coordResPrecision);

	if(simPt->getPosVec()->getXStatus() == TALSCalcParameter::kVariable)
	{
		(*stream).writeString(5,"MAX");
		(*stream) << deltaMax.getX() << separator;
		(*stream).writeString(7,"MOYEN");
		(*stream) << deltaMoy.getX() << separator;
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}


	if(simPt->getPosVec()->getYStatus() == TALSCalcParameter::kVariable)
	{
		(*stream).writeString(5,"MAX");
		(*stream) << deltaMax.getY() << separator;
		(*stream).writeString(7,"MOYEN");
		(*stream) << deltaMoy.getY() << separator;
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}


	if(simPt->getPosVec()->getZStatus() == TALSCalcParameter::kVariable)
	{
		(*stream).writeString(5,"MAX");
		(*stream) << deltaMax.getZ() << separator;
		(*stream).writeString(7,"MOYEN");
		(*stream) << deltaMoy.getZ() << separator;
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	(*stream).writeString(2,"");
	if(simPt->getPosVec()->getXStatus() == TALSCalcParameter::kVariable)
	{
		(*stream) << sx << separator;
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if(simPt->getPosVec()->getYStatus() == TALSCalcParameter::kVariable)
	{
		(*stream) << sy << separator;
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if(simPt->getPosVec()->getZStatus() == TALSCalcParameter::kVariable)
	{
		(*stream) << sz << separator;
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}
	(*stream) << endl;


	// Table data line 2
	(*stream).writeString(nameCoordWidth, "");

	if(simPt->getPosVec()->getXStatus() == TALSCalcParameter::kVariable)
	{
		(*stream).writeString(5,"MIN");
		(*stream) << deltaMin.getX() << separator;
		(*stream).writeString(7,"SIGMA");
		(*stream) << sigmadx << separator;
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	
	if(simPt->getPosVec()->getYStatus() == TALSCalcParameter::kVariable)
	{
		(*stream).writeString(5,"MIN");
		(*stream) << deltaMin.getY() << separator;
		(*stream).writeString(7,"SIGMA");
		(*stream) << sigmady << separator;
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}


	if(simPt->getPosVec()->getZStatus() == TALSCalcParameter::kVariable)
	{
		(*stream).writeString(5,"MIN");
		(*stream) << deltaMin.getZ() << separator;
		(*stream).writeString(7,"SIGMA");
		(*stream) << sigmadz << separator;
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	(*stream).writeString(2,"R");
	stream->setPrecisionFormat(max(0, coordResPrecision-3));
	if(simPt->getPosVec()->getXStatus() == TALSCalcParameter::kVariable)
	{
		(*stream) << sigmadx/sx << separator;
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if(simPt->getPosVec()->getYStatus() == TALSCalcParameter::kVariable)
	{
		(*stream) << sigmady/sy << separator;
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if(simPt->getPosVec()->getZStatus() == TALSCalcParameter::kVariable)
	{
		(*stream) << sigmadz/sz << separator;
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}
	(*stream) << endl;


	// Table data line 3
	//write overal sigma
	(*stream).writeString(nameCoordWidth, "OVERALL SIGMA =");
	(*stream) <<  (TReal) (sqrtq(powq(sigmadx.getMMetresValue(),2)
		+powq(sigmady.getMMetresValue(),2)+powq(sigmadz.getMMetresValue(),2))) << separator;
	(*stream) << endl;

	if (fDataSet->isSpaceBetweenData())  (*stream) << endl;

	stream->setWidthFormat(oldFormat);

	//initialise Unit
	stream->setLengthUnits(unit);

	return;
}




////////////////////////////////////////////////////////
//END
////////////////////////////////////////////////////////














