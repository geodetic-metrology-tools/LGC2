// TLGCProject.cpp
//
// A LGC project (manages the data storing and the calculation) 
//
// Patterns:
//
// 
// Copyright 2000-2008 M. Jones. CERN TS/SU. All rights reserved.
//////////////////////////////////////////////////////////////////////



//For ROOT//////////////////////////////////////////////////////
//#include	"TROOT.h"
//
// other forward declarations
#include <stdexcept>

#include "TLGCApplication.h"
#include "TLGCFileReader.h"
#include "TObservationMaker.h"

#include "TMillimetresFilter.h"
#include "TMetresFilter.h"
#include "TKilometresFilter.h"
#include "TGonsFilter.h"
#include "TRadiansFilter.h"
#include "T100MicroGonsFilter.h"
#include "TDMSFilter.h"
#include "T3DCartesianFilter.h"
#include "T2DPlusHFilter.h"
#include "TGeodeticFilter.h"
#include "TRefSystemFactory.h"
#include "TCoordSysFactory.h"

#include  "TLGCProject.h"
////////////////////////////////////////////////////////////////


//ClassImp(TLGCProject)


//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////
string TLGCProject::fTitle = "";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TLGCProject::TLGCProject()
{	// default constructor
	TPtrWrapper<TLGCDataSet> fPtrData(new TLGCDataSet());
	TPtrWrapper<ObservationSet> fPtrObs(new ObservationSet());
	TPtrWrapper<LSCalcDataSet> fPtrCalcData(new LSCalcDataSet());
	fData = fPtrData;
	fObs = fPtrObs;
	fCalcData = fPtrCalcData;
	fOutOptions = new TLGCOutputOptions();
	fParams = new TLGCCalcParams();
}


///////////////////
// copy constructor
///////////////////
TLGCProject::TLGCProject( TLGCProject& source)
{
	*this =source;
}

////////////
//destructor
////////////
TLGCProject::~TLGCProject()
{
	//fData.~TPtrWrapper<TLGCDataSet>;

	if(fOutOptions != 0)
	{
		delete fOutOptions;
	}
	if(fParams != 0)
	{
		delete fParams;
	}

	TMillimetresFilter::deleteFilter();
	TMetresFilter::deleteFilter();
	TKilometresFilter::deleteFilter();
	TGonsFilter::deleteFilter();
	TRadiansFilter::deleteFilter();
	T100MicroGonsFilter::deleteFilter();
	TDMSFilter::deleteFilter();
	T3DCartesianFilter::deleteFilter();
	T2DPlusHFilter::deleteFilter();
	TGeodeticFilter::deleteFilter();
	TRefSystemFactory::getRefSystemFactory()->deleteRefSystemFactory();
	TCoordSysFactory::getCoordSysFactoryPtr()->deleteCoordSysFactory();
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////
// copy assignement operator
TLGCProject& TLGCProject::operator=( TLGCProject& rhs )
{	//copy assignment operator
	if ( this != &rhs )
	{
		// initialise the attributes
		fTitle = rhs.fTitle;
		fError = rhs.getError();
		fNameFile = rhs.fNameFile;
		fResultsFile = rhs.fResultsFile;
		fDeformFile = rhs.fDeformFile;
		fOutOptions = rhs.fOutOptions;
		fParams = rhs.fParams;
		
		// copy the point data
		// effectively a shallow copy using a wrapper around the pointer
		fData = rhs.fData;
		fObs = rhs.fObs;
		fCalcData = rhs.fCalcData;		
	}
	return *this;
}



// gets the project title
string TLGCProject::getTitle()
{
	return fTitle;
}


// get output options
TLGCOutputOptions* TLGCProject::getOutputOptions() const
{
	return fOutOptions;
}

// get output options
TLGCCalcParams* TLGCProject::getCalcParams() const
{
	return fParams;
}

// load a project
int TLGCProject::load(const TFileParameters& fp)
{
	int error;
	fData.getPtr()->setFileParams(fp);
	fInputFile = fp;

	// if the filename is defined the load can continue
	if( fp.defined() && fData.getPtr()->getDataParams().defined() )
	{
		TLGCFileReader* file = new TLGCFileReader(this);
		file->readFile();
		delete file;

		if(this->getError()=="")
		{
			error=0;
		}
		else
		{
			error=1;
		}
	}
	else
	{
		error = 2;
	}


	return error;
}

////////////////////////////////////////////////////////////////////////
// process the project data by least squares with the input measurements
////////////////////////////////////////////////////////////////////////
void TLGCProject::computeLSResults()
{
	// create a new TObservationMaker object
	TObservationMaker* obs = new TObservationMaker();

	// if the data processing options are compatible, continue with the calculation
	if( processOptionsCompatible() )
	{
		// process the input data to create the observations and continue with the 
		// calculation if no errors generated
		if (obs->processData(*(fData.getPtr()), *(fObs.getPtr())))
		{
			//create and initialise a new TLSCalculationObject
			TLSCalculation* calc = new TLSCalculation(*fParams);
			if (fData.getPtr()->getRefFrameEnumerator() != TRefSystemFactory::kLocalRefFrame)
			{// calculation taking geodetic reference datums into account
				calc->useGeodSys();
			}

			if (getOutputOptions()->isDeformationAnalysisToBeCreated())
			{
				(fCalcData.getPtr())->setForDeformationAnalysis();
			}

			try {
			// calculate the required results
			calc->computeLSResults(*(fObs.getPtr()), *(fCalcData.getPtr()), *(fData.getPtr()), fParams );
			} catch (std::bad_alloc& e) {
				fError += string("Pas assez de memoire disponible: ") + e.what();
			} catch (std::exception& e) {
				fError += string("Cannot solve: ") + e.what();
			} catch (...) {
				fError += string("Unexpected error in calculation, contact support.");
			}
	
			fError += calc->getError();
			//delete calc;
		}
		else
		{
			fError = fError + obs->getError();
		}

		if(fError == "")
		{//si pas d'erreur de lecture ou inversion matrice.... alors on regarde le critere de convergence
			if(fCalcData.getPtr()->isConvergenceOk() == false)
			{
				fError = fError + "Le calcul ne converge pas: N'arrivé pas au critere de convergence.";
				fError = fError  + '\n';
			}
		}

		//order position vector parameter result
//		fCalcData.getPtr()->orderPVList(fData.getPtr()->getWorkingPoints());
	}

	delete obs;

	return;
	
}

//////////////////////////////////////////////////////
// check if the data processing options are compatible
//////////////////////////////////////////////////////
bool TLGCProject::processOptionsCompatible()
{
	bool ok = true;

	// check the reference frame and the punch file options are compatible
	TLGCOutputOptions::EPunc pun = fOutOptions->isPunchfileToBeSaved();
	TRefSystemFactory::ERefFrame refFrame = fData.getPtr()->getRefFrameEnumerator();

	bool b = (	pun == TLGCOutputOptions::kH || pun == TLGCOutputOptions::kZH
				|| pun == TLGCOutputOptions::kHN || pun == TLGCOutputOptions::kZHN);

	if(refFrame == TRefSystemFactory::kLocalRefFrame && b)
	{
		fError = fError + "L'option PUNC choisie est incompatible avec le mot clef OLOC, la coordonnee H n'est pas definie." + '\n';
		ok = false;
	}

	if (fOutOptions->isDeformationAnalysisToBeCreated())
	{
		if (this->isSimulation())
		{
			fError += "You cannot have DEFA while running simulations!\n";
			ok = false;
		}
		int variablePointsCount = 0;
		
		TLGCDataSet *ds = fData.getPtr();
		if (ds->getPointsDimension(TSpatialStatus::kVx) != 0) variablePointsCount++;
	    if (ds->getPointsDimension(TSpatialStatus::kVy) != 0) variablePointsCount++;
	    if (ds->getPointsDimension(TSpatialStatus::kVz) != 0) variablePointsCount++;
	    if (ds->getPointsDimension(TSpatialStatus::kVxy) != 0) variablePointsCount++;
	    if (ds->getPointsDimension(TSpatialStatus::kVxz) != 0) variablePointsCount++;
	    if (ds->getPointsDimension(TSpatialStatus::kVyz) != 0) variablePointsCount++;
	    if (ds->getPointsDimension(TSpatialStatus::kVxyz) != 0) variablePointsCount++;

		if (variablePointsCount != 1)
		{
			fError += "The input file should contain exactly one kind of variable points!\n";
			ok = false;
		}

		LSPosVecConstIter begin = fCalcData.getPtr()->beginPV();
		LSPosVecConstIter end = fCalcData.getPtr()->endPV();

		while (begin != end)
		{
			if (begin->getName().length() > 8)
			{
				fError += "Point names should be less than 8 characters in lenght when using *DEFA!\n";
				ok = false;
			}

			begin++;
		}
	}

	return ok;
}

/////////////////////////////////////
// check if it is a simulation or not
/////////////////////////////////////
bool TLGCProject::isSimulation()
{
	return fParams->isSimulation();
}


////////////////////////////////////////////////
// returns a pointer to the project's LGCDataSet
////////////////////////////////////////////////
TLGCDataSet* TLGCProject::getDataSet(){

	return fData.getPtr();
}

const TLGCDataSet* TLGCProject::getDataSet() const {

	return fData.getPtr();
}

////////////////////////
// returns the name file
////////////////////////
TFileParameters TLGCProject::getNameFile() const{

	return fNameFile;
}

//////////////////////////////////////
// returns the results file parameters
//////////////////////////////////////
TFileParameters TLGCProject::getInputFileParams() const{

	return fInputFile;
}

//////////////////////////////////////
// returns the results file parameters
//////////////////////////////////////
TFileParameters TLGCProject::getResultsFileParams() const{

	return fResultsFile;
}


//////////////////////////////////////
// returns the warning messages
//////////////////////////////////////
string	TLGCProject::getWarning() const
{
	return fWarning;
}


/////////////////
// sets the title
/////////////////
void	TLGCProject::setTitle(const string title)
{
	fTitle= title;
	return;
}

////////////////////////////////
// sets the name file parameters
////////////////////////////////
bool TLGCProject::setNameFileParams(const TFileParameters& nameFile)
{
	fNameFile = nameFile;

	return true;
}
///////////////////////////////////
// sets the results file parameters
///////////////////////////////////
bool TLGCProject::setResultsFileParams(const TFileParameters& resFile)
{
	fResultsFile = resFile;

	return true;
}
///////////////////////////////////
// sets the deformation file parameters
///////////////////////////////////
bool TLGCProject::setDeformFileParams(const TFileParameters& defFile)
{
	fDeformFile = defFile;

	return true;
}

////////////////////////////////
// turn on the simulation option
////////////////////////////////
void TLGCProject::setSimulation()
{
	fParams->setSimulation();
	return;
}


////////////////////////////////
// adds a warning message
////////////////////////////////
void TLGCProject::addWarning(const string& warning)
{
	fWarning += warning + "\n";
	return;
}


///////////////////////////////////////////////////
// returns a pointer to the project's LSCalcDataSet
///////////////////////////////////////////////////
LSCalcDataSet* TLGCProject::getLSCalcDataSet(){

	return fCalcData.getPtr();
}

