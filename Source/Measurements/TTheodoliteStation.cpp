// 
// TTheodoliteStation.cpp : implementation file
//
// Class for a theodolite instrument station

#include "THorAngleROM.h"
#include "TSpatialPosition.h"
#include "TLength.h"
//#include "TWorkingStations.h"
#include "TVSpatialPtListener.h"

#include "TTheodoliteStation.h"


//////////////////////
// default constructor 
//////////////////////
TTheodoliteStation::TTheodoliteStation()
:TAInstrumentStation(), fLastHAngleSeriesNbr(0), fLastZDistSeriesNbr(0),fLastGyroOrieSeriesNbr(0),
fInstrumentHeight(0), fAngleConst(){
	
	fInstHeightStatus = TAMeasurement::kFixed;
	fDistStAssociatedForSpaDist = false;
	fDistStAssociatedForOffset = false;
}

//////////////////////////////////////////
// constructor setting the stationed point
//////////////////////////////////////////
TTheodoliteStation::TTheodoliteStation(TSpatialPointName iterSetupPt)
:TAInstrumentStation(iterSetupPt), fLastHAngleSeriesNbr(0), fLastZDistSeriesNbr(0),fLastGyroOrieSeriesNbr(0),
fInstrumentHeight(0), fAngleConst(){
	
	fInstHeightStatus = TAMeasurement::kFixed;
	fDistStAssociatedForSpaDist = false;
	fDistStAssociatedForOffset = false;
}

//////////////////
//copy constructor
//////////////////
TTheodoliteStation::TTheodoliteStation(const TTheodoliteStation& source)
: TAInstrumentStation(source){

	*this = source;
}

/////////////
// destructor
/////////////
TTheodoliteStation::~TTheodoliteStation(){

}


////////////////////////////
// copy assignement operator
////////////////////////////
TTheodoliteStation& TTheodoliteStation::operator=(const TTheodoliteStation& source){

	if (this != &source)
	{
		fInstrumentHeight = source.fInstrumentHeight;
		fInstHeightStatus = source.fInstHeightStatus;
		fInstrumentNumber = source.fInstrumentNumber;
		fSetup = source.fSetup;
		fStationedPoint = source.fStationedPoint;
		fMadeAngleROMs = source.fMadeAngleROMs;
		fMadeZDistROMs = source.fMadeZDistROMs;
		fMadeGyroOrieROMs = source.fMadeGyroOrieROMs;
		fLastHAngleSeriesNbr = source.fLastHAngleSeriesNbr;
		fLastZDistSeriesNbr = source.fLastZDistSeriesNbr;
		fLastGyroOrieSeriesNbr = source.fLastGyroOrieSeriesNbr;
		fAngleConst = source.fAngleConst;
		fDistStAssociatedForSpaDist = source.fDistStAssociatedForSpaDist;
		fDistStAssociatedForOffset = source.fDistStAssociatedForOffset;

	}

	return (*this);
}


/////////////////////////////////////////////
// returns a non-const pointer to this object
/////////////////////////////////////////////
TTheodoliteStation* TTheodoliteStation::getPointer() const{

	return const_cast<TTheodoliteStation*>(this);
}


////////////////////////////////
// returns the instrument height
////////////////////////////////
TLength TTheodoliteStation::getInstrumentHeight() const
{
	return fInstrumentHeight;
}


////////////////////////////////////////////////
// returns the status of the instrument's height
////////////////////////////////////////////////
TAMeasurement::ECalcStatus TTheodoliteStation::getInstHeightStatus() const
{
	return fInstHeightStatus;
}


///////////////////////////////////////
// returns measurement's angle constant
///////////////////////////////////////
TAngleConstants TTheodoliteStation::getAngleConst() const
{
	return fAngleConst;
}





////////////////////////////////
// sets the instrument height
////////////////////////////////
void TTheodoliteStation::setInstrumentHeight(const TLength height)
{
	fInstrumentHeight = height;
	return;
}

/////////////////////////////////////////////////////
// sets measurement's angle constant (if still = LITERAL(0.0))
/////////////////////////////////////////////////////
void TTheodoliteStation::ifNotDoneSetAngleConst(const TAngleConstants angConst)
{
	if (fAngleConst.getValue().getGonsValue() == LITERAL(0.0))
		fAngleConst = angConst;
	return;
}

///////////////////////////////////////
// returns measurement's angle constant
///////////////////////////////////////
void TTheodoliteStation::setAngleConstName(const string name)
{
	fAngleConst.setConstantName(name);
	return;
}


//////////////////////////////////////////////////////////////////
// sets the Unknown/Variable/Fixed status of the instrument height
//////////////////////////////////////////////////////////////////
void TTheodoliteStation::setInstHeightStatus(const TAMeasurement::ECalcStatus s){

	fInstHeightStatus = s;
}


bool TTheodoliteStation::setAssociatedDistStForSpaDist()
{/*turn the boolean attribute for corresponding distance
 measurement station to true*/
	fDistStAssociatedForSpaDist = true;
	return fDistStAssociatedForSpaDist;
}


bool TTheodoliteStation::setAssociatedDistStForOffset()
{/*turn the boolean attribute for corresponding distance
 measurement station to true*/
	fDistStAssociatedForOffset = true;
	return fDistStAssociatedForOffset;
}

	
bool	TTheodoliteStation::getAssociatedDistStForSpaDist() const
{//get the boolean indicating a corresponding TDistStation
	return fDistStAssociatedForSpaDist;
}


bool	TTheodoliteStation::getAssociatedDistStForOffset() const
{//get the boolean indicating a corresponding TDistStation
	return fDistStAssociatedForOffset;
}


/////////////////////////////////////////
// adds a round of hor angle measurements
/////////////////////////////////////////
THorAngleROM* TTheodoliteStation::addHorAngleROM(THorAngleROM * harom){

	//possible definition of the rom's fSeries
	string empty("");
	if ((harom->getSeries()) == empty){

		fLastHAngleSeriesNbr ++;
		ostringstream oss;
		oss << "TheodHAngROM" << fLastHAngleSeriesNbr;
		harom->setSeries(oss.str());
	}

	if (notInContainer(harom)){

		//insertion in the container
		fMadeAngleROMs.push_back(*harom);
		return &(*(-- fMadeAngleROMs.end()));
	}
	else{
		cerr << "HAROM not inserted : already stored\n";
		return 0;
	}
}




/////////////////////////////////////////////////////////////////
// checks if the horizontal angle ROM is already in the container
/////////////////////////////////////////////////////////////////
bool TTheodoliteStation::notInContainer(THorAngleROM* har) const{

	HorAngROMConstIter iter = getHorAngROMBeginIterator();
	HorAngROMConstIter iterEnd = getHorAngROMEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*har) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}




////////////////////////////////////////////////
// returns an iterator on the first HorAngle ROM
////////////////////////////////////////////////
HorAngROMIterator TTheodoliteStation::getHorAngROMBeginIterator() {

	return fMadeAngleROMs.begin();
}

/////////////////////////////////////////////////////
// returns a const iterator on the first HorAngle ROM
/////////////////////////////////////////////////////
HorAngROMConstIter TTheodoliteStation::getHorAngROMBeginIterator() const{

	return fMadeAngleROMs.begin();
}



////////////////////////////////////////////////////////////////
// returns an iterator one past the last HorAngle ROM of the set
////////////////////////////////////////////////////////////////
HorAngROMIterator TTheodoliteStation::getHorAngROMEndIterator() {

	return fMadeAngleROMs.end();
}

/////////////////////////////////////////////////////////////////////
// returns a const iterator one past the last HorAngle ROM of the set
/////////////////////////////////////////////////////////////////////
HorAngROMConstIter TTheodoliteStation::getHorAngROMEndIterator() const{

	return fMadeAngleROMs.end();
}





///////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the ROM identified by the fSeries string
///////////////////////////////////////////////////////////////////////////
HorAngROMIterator TTheodoliteStation::getHorAngROM(string series) {


	HorAngROMIterator iter = fMadeAngleROMs.begin();
	bool notFound = true;

	while ((iter != fMadeAngleROMs.end()) && notFound){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	
	return iter;
}

////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the ROM identified by the fSeries string
////////////////////////////////////////////////////////////////////////////////
HorAngROMConstIter TTheodoliteStation::getHorAngROM(string series) const{


	HorAngROMConstIter iter = fMadeAngleROMs.begin();
	bool notFound = true;

	while ((iter != fMadeAngleROMs.end()) && notFound){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	
	return iter;
}





/////////////////////////////////////////////////
// adds a round of zenithal distance measurements
/////////////////////////////////////////////////
TZenithDistROM* TTheodoliteStation::addZenithDistROM(TZenithDistROM * zdrom){

	//possible definition of the rom's fSeries
	string empty("");
	if ((zdrom->getSeries()) == empty){

		fLastZDistSeriesNbr ++;
		ostringstream oss;
		oss << "TheodZDistROM" << fLastZDistSeriesNbr;
		zdrom->setSeries(oss.str());
	}

	if (notInContainer(zdrom)){

		//insertion in the container
		fMadeZDistROMs.push_back(*zdrom);
		return &(*(-- fMadeZDistROMs.end()));
	}
	else{
		cerr << "ZDROM not inserted : already stored\n";
		return 0;
	}
}

//////////////////////////////////////////////////////////////////
// checks if the zenithal distance ROM is already in the container
//////////////////////////////////////////////////////////////////
bool TTheodoliteStation::notInContainer(TZenithDistROM* zdr) const{

	ZenithDistROMConstIter iter = getZenithDistROMBeginIterator();
	ZenithDistROMConstIter iterEnd = getZenithDistROMEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*zdr) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}


//////////////////////////////////////////////////
// returns an iterator on the first ZenithDist ROM
//////////////////////////////////////////////////
ZenithDistROMIterator TTheodoliteStation::getZenithDistROMBeginIterator() {

	return fMadeZDistROMs.begin();
}

///////////////////////////////////////////////////////
// returns a const iterator on the first ZenithDist ROM
///////////////////////////////////////////////////////
ZenithDistROMConstIter TTheodoliteStation::getZenithDistROMBeginIterator() const{

	return fMadeZDistROMs.begin();
}


//////////////////////////////////////////////////////////////////
// returns an iterator one past the last ZenithDist ROM of the set
//////////////////////////////////////////////////////////////////
ZenithDistROMIterator TTheodoliteStation::getZenithDistROMEndIterator() {

	return fMadeZDistROMs.end();
}

///////////////////////////////////////////////////////////////////////
// returns a const iterator one past the last ZenithDist ROM of the set
///////////////////////////////////////////////////////////////////////
ZenithDistROMConstIter TTheodoliteStation::getZenithDistROMEndIterator() const{

	return fMadeZDistROMs.end();
}

///////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the ROM identified by the fSeries string
///////////////////////////////////////////////////////////////////////////
ZenithDistROMIterator TTheodoliteStation::getZenithDistROM(string series) {


	ZenithDistROMIterator iter = fMadeZDistROMs.begin();
	bool notFound = true;

	while ((iter != fMadeZDistROMs.end()) && notFound){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	
	return iter;
}

////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the ROM identified by the fSeries string
////////////////////////////////////////////////////////////////////////////////
ZenithDistROMConstIter TTheodoliteStation::getZenithDistROM(string series) const{


	ZenithDistROMConstIter iter = fMadeZDistROMs.begin();
	bool notFound = true;

	while ((iter != fMadeZDistROMs.end()) && notFound){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	
	return iter;
}

////////////////////////////////////////////////////////////////////////////////////
//Gyro Orientation
////////////////////////////////////////////////////////////////////////////////////
TGyroOrientationROM* TTheodoliteStation::addGyroOrieROM(TGyroOrientationROM* orieROM)
{//adds a round of gyro measurements
	
	//possible definition of the rom's fSeries
	string empty("");
	if ((orieROM->getSeries()) == empty)
	{
		fLastGyroOrieSeriesNbr ++;
		ostringstream oss;
		oss << "TheodGyroOrieROM" << fLastGyroOrieSeriesNbr;
		orieROM->setSeries(oss.str());
	}

	if (notInContainer(orieROM))
	{//insertion in the container
		fMadeGyroOrieROMs.push_back(*orieROM);
		return &(*(-- fMadeGyroOrieROMs.end()));
	}
	else
	{
		cerr << "GyroOrientationROM not inserted : already stored\n";
		return 0;
	}
}


bool TTheodoliteStation::notInContainer(TGyroOrientationROM* gyro) const
{//checks if the gyro ROM is already in the container
	GyroOrieROMConstIter iter = getGyroOrieROMBeginIterator();
	GyroOrieROMConstIter iterEnd = getGyroOrieROMEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*gyro) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}
	return notPresent;
}


GyroOrieROMIterator TTheodoliteStation::getGyroOrieROMBeginIterator()
{//returns an iterator on the first gyro ROM
	return fMadeGyroOrieROMs.begin();
}


GyroOrieROMConstIter TTheodoliteStation::getGyroOrieROMBeginIterator() const
{//returns a const iterator on the first gyro ROM
	return fMadeGyroOrieROMs.begin();
}


GyroOrieROMIterator TTheodoliteStation::getGyroOrieROMEndIterator()
{//returns an iterator one past the last gyro ROM of the set
	return fMadeGyroOrieROMs.end();
}


GyroOrieROMConstIter TTheodoliteStation::getGyroOrieROMEndIterator() const
{//returns a const iterator one past the last gyro ROM of the set
	return fMadeGyroOrieROMs.end();
}


GyroOrieROMIterator TTheodoliteStation::getGyroOrieROM(string series)
{//returns an iterator pointing to the ROM identified by the fSeries string
	GyroOrieROMIterator iter = fMadeGyroOrieROMs.begin();
	bool notFound = true;

	while ((iter != fMadeGyroOrieROMs.end()) && notFound){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}


GyroOrieROMConstIter TTheodoliteStation::getGyroOrieROM(string series) const
{//returns a const iterator pointing to the ROM identified by the fSeries string
	GyroOrieROMConstIter iter = fMadeGyroOrieROMs.begin();
	bool notFound = true;

	while ((iter != fMadeGyroOrieROMs.end()) && notFound){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}
































//////////////////////////////////////////////////
// responds to an additional readyToActivate Child
//////////////////////////////////////////////////
/*void TTheodoliteStation::childReadyToActivate(int measCount, int eqCount, int unkCount){

	fReadyToActivateEqns += eqCount;
//	fStationedPoint->updatePotentialEqCount(eqCount);

/*	if (fReadyToActivateEqns >= 
//		(fStationedPoint->getVariableDimension()-fStationedPoint->getPotentialEqCount())){

		fUsedInCalc = true;

		activateReadyChildren();
	}*/
//}

///////////////////////////////////////////
// activates the ready to activate children
///////////////////////////////////////////
/*void TTheodoliteStation::activateReadyChildren() {

	HorAngROMIterator iter = fMadeAngleROMs.begin();
	HorAngROMIterator iterEnd = fMadeAngleROMs.end();

	while (iter != iterEnd){
		iter->parentActivated();
		if (iter->readyToActivate())
			iter->activateReadyChildren();
		iter++;
	}
}
*/		
////////////////////////////////////////////////////////////////////////////
// tells the children the station has a sufficient total number of equations
////////////////////////////////////////////////////////////////////////////
/*void TTheodoliteStation::sendEnoughTotalEqns() {

	HorAngROMIterator iter = fMadeAngleROMs.begin();
	HorAngROMIterator iterEnd = fMadeAngleROMs.end();

	while (iter != iterEnd){
		iter->parentEnoughTotalEqns();
		iter++;
	}

	TAInstrumentStation::StListnrIterator iterL = fStListeners.begin();
	TAInstrumentStation::StListnrIterator iterLEnd = fStListeners.end();

	while (iterL != iterLEnd){
		(*iterL)->stationUpdated(TVStationListener::EUpdateMessage::kEnoughTotalEqns);
		iterL++;
	}
}
*/
//////////////////////////////////////////////////////////
// tells the ROM children that the station is disactivated
//////////////////////////////////////////////////////////
/*void TTheodoliteStation::sendDisactivatedToChildren() {
	{

		HorAngROMIterator iter = fMadeAngleROMs.begin();
		HorAngROMIterator iterEnd = fMadeAngleROMs.end();

		while (iter != iterEnd){
			iter->parentDisactivated();
			iter++;
		}
	}

	{

		ZenithDistROMIterator iter = fMadeZDistROMs.begin();
		ZenithDistROMIterator iterEnd = fMadeZDistROMs.end();

		while (iter != iterEnd){
			iter->parentDisactivated();
			iter++;
		}
	}

}
*/
/////////////////////////////////////////////////////////
// tells the ROM children that the station is Activated
/////////////////////////////////////////////////////////
/*void TTheodoliteStation::sendActivatedToChildren() {
	{
		HorAngROMIterator iter = fMadeAngleROMs.begin();
		HorAngROMIterator iterEnd = fMadeAngleROMs.end();

		while (iter != iterEnd){
			iter->parentActivated();
			iter++;
		}
	}


	{

		ZenithDistROMIterator iter = fMadeZDistROMs.begin();
		ZenithDistROMIterator iterEnd = fMadeZDistROMs.end();

		while (iter != iterEnd){
			iter->parentActivated();
			iter++;
		}
	}
}
*/