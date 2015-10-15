// 
// TDistMeasStation.cpp : implementation File
//
// Class for a distance measuring instrument station
//#include "TWorkingStations.h"

#include "TDistMeasStation.h"




//////////////////////////////////////////
//constructor / destructor
//////////////////////////////////////////
TDistMeasStation::TDistMeasStation()
: TAInstrumentStation(), fLastHorDistSeriesNbr(0),fLastSpatialDistSeriesNbr(0), fLast2DOffsetSeriesNbr(0),
fECVE(), fECSP(), fECHO(), fECTH(), fInstrumentHeight(0)
{// default constructor
	fInstHeightStatus = TAMeasurement::kFixed;
	fIsAssociatedTheoSt = false;
}


TDistMeasStation::TDistMeasStation(TSpatialPointName setupPt)
: TAInstrumentStation(setupPt), fLastHorDistSeriesNbr(0), fLast2DOffsetSeriesNbr(0), 
fLastSpatialDistSeriesNbr(0), fECVE(),  fECSP(), fECHO(), fECTH(), fInstrumentHeight(0)
{//constructor
	fInstHeightStatus = TAMeasurement::kFixed;
	fIsAssociatedTheoSt = false;
}


TDistMeasStation::~TDistMeasStation()
{//destructor
}


/////////////////////////////////////////////////////////////////////////////////////////////
//Member function
/////////////////////////////////////////////////////////////////////////////////////////////
TDistMeasStation& TDistMeasStation::operator=(const TDistMeasStation& source)
{//copy assignement operator

	//TAInstrumentStation's attributs
	fInstrumentNumber = source.fInstrumentNumber;
	fSetup = source.fSetup;
	fStationedPoint = source.fStationedPoint;

	//TDistMeasStation's attributs
	fMadeHorDistROMs = source.fMadeHorDistROMs;
	fLastHorDistSeriesNbr = source.fLastHorDistSeriesNbr;

	fMadeSpatialDistROMs = source.fMadeSpatialDistROMs;
	fLastSpatialDistSeriesNbr = source.fLastSpatialDistSeriesNbr;

	fMade2DOffsetROMs = source.fMade2DOffsetROMs;
	fLast2DOffsetSeriesNbr = source.fLast2DOffsetSeriesNbr;

	fECVE = source.fECVE;
	fECSP = source.fECSP;
	fECHO = source.fECHO;
	fECTH = source.fECTH;

	fInstrumentHeight = source.fInstrumentHeight; 
	fInstHeightStatus = source.fInstHeightStatus; 

	fAssociatedTheoSt = source.fAssociatedTheoSt;
	fIsAssociatedTheoSt = source.fIsAssociatedTheoSt;

	return (*this);
}


bool	TDistMeasStation::operator==(const TDistMeasStation& right) const
{//operator == 

	if ( (this->TAInstrumentStation::operator==(right)) &&
		 (fInstrumentHeight==right.fInstrumentHeight) &&
		 (fInstHeightStatus==right.fInstHeightStatus)	)
		return true;
	else return false;
}



TLength TDistMeasStation::getInstrumentHeight() const
{
	return fInstrumentHeight;
}

TAMeasurement::ECalcStatus TDistMeasStation::getInstHeightStatus() const
{
	return fInstHeightStatus;
}

void TDistMeasStation::setInstrumentHeight(TLength height)
{
	fInstrumentHeight = height;
	return;
}


bool	TDistMeasStation::isAssociatedTheoSt() const
{
	return fIsAssociatedTheoSt;
}

TheodStIterator	TDistMeasStation::getAssociatedTheoSt() const
{
	return fAssociatedTheoSt;
}

TheodStIterator	TDistMeasStation::setAssociatedTheoSt(const TheodStIterator theo)
{
	TheodStIterator old = fAssociatedTheoSt;
	fAssociatedTheoSt = theo;
	fIsAssociatedTheoSt = true;

	return old;
}
	







//////////////////////////////////////////////////////////////////
// sets the Unknown/Variable/Fixed status of the instrument height
//////////////////////////////////////////////////////////////////
void TDistMeasStation::setInstHeightStatus(TAMeasurement::ECalcStatus s){

	fInstHeightStatus = s;
}

/////////////////////////////////////////
// adds a round of hor distance measurements
/////////////////////////////////////////
THorizontalDistROM* TDistMeasStation::addHorDistROM(THorizontalDistROM * hdrom){

	//possible definition of the rom's fSeries
	string empty("");
	if ((hdrom->getSeries()) == empty){

		fLastHorDistSeriesNbr++;
		ostringstream oss;
		oss << "HorDistROM" << fLastHorDistSeriesNbr;
		hdrom->setSeries(oss.str());
	}

	if (notInContainer(hdrom)){
		fMadeHorDistROMs.push_back(*hdrom);
		return &(*(-- fMadeHorDistROMs.end()));
	}
	else {
		cerr << "HorDistROM not inserted : already stored" << endl;
		return 0;
	}
}

/////////////////////////////////////////////////////
// checks if the dist ROM is already in the container
/////////////////////////////////////////////////////
bool TDistMeasStation::notInContainer(THorizontalDistROM* hdr) const{

	HorDistROMConstIter iter = getHorDistROMBeginIterator();
	HorDistROMConstIter iterEnd = getHorDistROMEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*hdr) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}

////////////////////////////////////////////////////////////////
// returns an iterator pointing to the first dist ROM of the set
////////////////////////////////////////////////////////////////
HorDistROMIterator TDistMeasStation::getHorDistROMBeginIterator() {

	return fMadeHorDistROMs.begin();
}

//////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the first dist ROM of the set
//////////////////////////////////////////////////////////////////////
HorDistROMConstIter TDistMeasStation::getHorDistROMBeginIterator() const{

	return fMadeHorDistROMs.begin();
}

////////////////////////////////////////////////////////////////
// returns an iterator pointing to the end dist ROM of the set
////////////////////////////////////////////////////////////////
HorDistROMIterator TDistMeasStation::getHorDistROMEndIterator() {

	return fMadeHorDistROMs.end();
}

//////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the end dist ROM of the set
//////////////////////////////////////////////////////////////////////
HorDistROMConstIter TDistMeasStation::getHorDistROMEndIterator() const{

	return fMadeHorDistROMs.end();
}


//////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the ROM having the specified fSeries string
//////////////////////////////////////////////////////////////////////////////
HorDistROMIterator TDistMeasStation::getHorDistROM(string series) {

	HorDistROMIterator iter = fMadeHorDistROMs.begin();
	bool notFound = true;
	while (notFound && (iter!=fMadeHorDistROMs.end())){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}

////////////////////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the ROM having the specified fSeries string
////////////////////////////////////////////////////////////////////////////////////
HorDistROMConstIter TDistMeasStation::getHorDistROM(string series) const{

	HorDistROMConstIter iter = fMadeHorDistROMs.begin();
	bool notFound = true;
	while (notFound && (iter!=fMadeHorDistROMs.end())){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}


/////////////////////////////////////////
// adds a round of hor distance measurements
/////////////////////////////////////////
TSpatialDistROM* TDistMeasStation::addSpatialDistROM(TSpatialDistROM * sdrom){

	//possible definition of the rom's fSeries
	string empty("");
	if ((sdrom->getSeries()) == empty){

		fLastSpatialDistSeriesNbr++;
		ostringstream oss;
		oss << "SpatialDistROM" << fLastSpatialDistSeriesNbr;
		sdrom->setSeries(oss.str());
	}

	if (notInContainer(sdrom)){
		fMadeSpatialDistROMs.push_back(*sdrom);
		return &(*(-- fMadeSpatialDistROMs.end()));
	}
	else {
		cerr << "SpatialDistROM not inserted : already stored" << endl;
		return 0;
	}
}

/////////////////////////////////////////////////////
// checks if the dist ROM is already in the container
/////////////////////////////////////////////////////
bool TDistMeasStation::notInContainer(TSpatialDistROM* sdr) const{

	SpatialDistROMConstIter iter = getSpatialDistROMBeginIterator();
	SpatialDistROMConstIter iterEnd = getSpatialDistROMEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*sdr) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}

////////////////////////////////////////////////////////////////
// returns an iterator pointing to the first dist ROM of the set
////////////////////////////////////////////////////////////////
SpatialDistROMIterator TDistMeasStation::getSpatialDistROMBeginIterator() {

	return fMadeSpatialDistROMs.begin();
}

//////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the first dist ROM of the set
//////////////////////////////////////////////////////////////////////
SpatialDistROMConstIter TDistMeasStation::getSpatialDistROMBeginIterator() const{

	return fMadeSpatialDistROMs.begin();
}

////////////////////////////////////////////////////////////////
// returns an iterator pointing to the end dist ROM of the set
////////////////////////////////////////////////////////////////
SpatialDistROMIterator TDistMeasStation::getSpatialDistROMEndIterator() {

	return fMadeSpatialDistROMs.end();
}

//////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the end dist ROM of the set
//////////////////////////////////////////////////////////////////////
SpatialDistROMConstIter TDistMeasStation::getSpatialDistROMEndIterator() const{

	return fMadeSpatialDistROMs.end();
}


//////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the ROM having the specified fSeries string
//////////////////////////////////////////////////////////////////////////////
SpatialDistROMIterator TDistMeasStation::getSpatialDistROM(string series) {

	SpatialDistROMIterator iter = fMadeSpatialDistROMs.begin();
	bool notFound = true;
	while (notFound && (iter!=fMadeSpatialDistROMs.end())){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}

////////////////////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the ROM having the specified fSeries string
////////////////////////////////////////////////////////////////////////////////////
SpatialDistROMConstIter TDistMeasStation::getSpatialDistROM(string series) const{

	SpatialDistROMConstIter iter = fMadeSpatialDistROMs.begin();
	bool notFound = true;
	while (notFound && (iter!=fMadeSpatialDistROMs.end())){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}

/////////////////////////////////////////
// adds a round of 2D offset measurements
/////////////////////////////////////////
T2DOffsetROM* TDistMeasStation::add2DOffsetROM(T2DOffsetROM* orom){

	//possible definition of the rom's fSeries
	string empty("");
	if ((orom->getSeries()) == empty){

		fLast2DOffsetSeriesNbr++;
		ostringstream oss;
		oss << "2DOffsetROM" << fLast2DOffsetSeriesNbr;
		orom->setSeries(oss.str());
	}

	if (notInContainer(orom)){
		fMade2DOffsetROMs.push_back(*orom);
		return &(*(-- fMade2DOffsetROMs.end()));
	}
	else {
		cerr << "2DOffsetROM not inserted : already stored" << endl;
		return 0;
	}
}

//////////////////////////////////////////////////////////
// checks if the 2D offset ROM is already in the container
//////////////////////////////////////////////////////////
bool TDistMeasStation::notInContainer(T2DOffsetROM* orm) const {

	Off2DROMConstIter iter = get2DOffsetROMBeginIterator();
	Off2DROMConstIter iterEnd = get2DOffsetROMEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*orm) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
		{
			 
		}
			iter ++;
	}

	return notPresent;
}

////////////////////////////////////////////////////////////////
// returns an iterator pointing to the first dist ROM of the set
////////////////////////////////////////////////////////////////
Off2DROMIterator TDistMeasStation::get2DOffsetROMBeginIterator() {

	return fMade2DOffsetROMs.begin();
}

//////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the first dist ROM of the set
/////////////////////////////////////////////////////////////////////
Off2DROMConstIter TDistMeasStation::get2DOffsetROMBeginIterator() const{

	return fMade2DOffsetROMs.begin();
}

//////////////////////////////////////////////////////////////
// returns an iterator pointing to the end dist ROM of the set
//////////////////////////////////////////////////////////////
Off2DROMIterator TDistMeasStation::get2DOffsetROMEndIterator() {

	return fMade2DOffsetROMs.end();
}

////////////////////////////////////////////////////////////////////
// returns an const iterator pointing to the end dist ROM of the set
////////////////////////////////////////////////////////////////////
Off2DROMConstIter TDistMeasStation::get2DOffsetROMEndIterator() const{

	return fMade2DOffsetROMs.end();
}


//////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the ROM having the specified fSeries string
//////////////////////////////////////////////////////////////////////////////
Off2DROMIterator TDistMeasStation::get2DOffsetROM(string series) {

	Off2DROMIterator iter = fMade2DOffsetROMs.begin();
	bool notFound = true;
	while (notFound && (iter!=fMade2DOffsetROMs.end())){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}


//////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the ROM having the specified fSeries string
//////////////////////////////////////////////////////////////////////////////
Off2DROMConstIter TDistMeasStation::get2DOffsetROM(string series) const{

	Off2DROMConstIter iter = fMade2DOffsetROMs.begin();
	bool notFound = true;
	while (notFound && (iter!=fMade2DOffsetROMs.end())){
		if (iter->getSeries() == series)
			notFound = false;
		else
			iter ++;
	}
	return iter;
}


///////////////////////////////////////////////////////////////////////////////////
//ECVE (offset to ver. line) measurements related
///////////////////////////////////////////////////////////////////////////////////
OffsetToVerLineMeasIterator TDistMeasStation::getOffsetToVerLineMeasBeginIterator()
{//returns an iterator pointing to the first dist ROM of the set
	return fECVE.begin();
}


OffsetToVerLineMeasConstIter TDistMeasStation::getOffsetToVerLineMeasBeginIterator() const
{// returns an const iterator pointing to the first dist ROM of the set
	return fECVE.begin();
}


OffsetToVerLineMeasIterator TDistMeasStation::getOffsetToVerLineMeasEndIterator()
{// returns an iterator pointing to the end dist ROM of the set
	return fECVE.end();
}


OffsetToVerLineMeasConstIter TDistMeasStation::getOffsetToVerLineMeasEndIterator() const
{// returns an const iterator pointing to the end dist ROM of the set
	return fECVE.end();
}


TOffsetToVerLineMeasurement* TDistMeasStation::addOffsetToVerLineMeas(TOffsetToVerLineMeasurement* meas)
{// adds a radial offset constraint to the station

	if (notInContainer(meas))
	{
		fECVE.push_back(*meas);
		return &(*(-- fECVE.end()));
	}
	else
	{
		cerr << "Offset not inserted : already stored" << endl;
		return 0;
	}
}


bool TDistMeasStation::notInContainer(TOffsetToVerLineMeasurement* meas) const
{// checks if the radial offset constraint is already in the container
	OffsetToVerLineMeasConstIter iter = getOffsetToVerLineMeasBeginIterator(); 
	OffsetToVerLineMeasConstIter iterEnd = getOffsetToVerLineMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd)
	{
		if ((*meas) == (*iter))
		{
			notPresent = false;
			iter  =iterEnd;
		}
		else
		{
			iter ++;
		}
	}

	return notPresent;
}


///////////////////////////////////////////////////////////////////////////////////
//ECSP (offset to spa. line) measurements related
///////////////////////////////////////////////////////////////////////////////////
OffsetToSpaLineMeasIterator TDistMeasStation::getOffsetToSpaLineMeasBeginIterator()
{//returns an iterator pointing to the first dist ROM of the set
	return fECSP.begin();
}


OffsetToSpaLineMeasConstIter TDistMeasStation::getOffsetToSpaLineMeasBeginIterator() const
{// returns an const iterator pointing to the first dist ROM of the set
	return fECSP.begin();
}


OffsetToSpaLineMeasIterator TDistMeasStation::getOffsetToSpaLineMeasEndIterator()
{// returns an iterator pointing to the end dist ROM of the set
	return fECSP.end();
}


OffsetToSpaLineMeasConstIter TDistMeasStation::getOffsetToSpaLineMeasEndIterator() const
{// returns an const iterator pointing to the end dist ROM of the set
	return fECSP.end();
}


TOffsetToSpaLineMeasurement* TDistMeasStation::addOffsetToSpaLineMeas(TOffsetToSpaLineMeasurement* meas)
{// adds a radial offset constraint to the station

	if (notInContainer(meas))
	{
		fECSP.push_back(*meas);
		return &(*(-- fECSP.end()));
	}
	else
	{
		cerr << "Offset not inserted : already stored" << endl;
		return 0;
	}
}


bool TDistMeasStation::notInContainer(TOffsetToSpaLineMeasurement* meas) const
{// checks if the radial offset constraint is already in the container
	OffsetToSpaLineMeasConstIter iter = getOffsetToSpaLineMeasBeginIterator(); 
	OffsetToSpaLineMeasConstIter iterEnd = getOffsetToSpaLineMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd)
	{
		if ((*meas) == (*iter))
		{
			notPresent = false;
			iter  =iterEnd;
		}
		else
		{
			iter ++;
		}
	}

	return notPresent;
}


///////////////////////////////////////////////////////////////////////////////////
//ECHO (offset to ver. Plane) measurements related
///////////////////////////////////////////////////////////////////////////////////
OffsetToVerPlaneMeasIterator TDistMeasStation::getOffsetToVerPlaneMeasBeginIterator()
{//returns an iterator pointing to the first dist ROM of the set
	return fECHO.begin();
}


OffsetToVerPlaneMeasConstIter TDistMeasStation::getOffsetToVerPlaneMeasBeginIterator() const
{// returns an const iterator pointing to the first dist ROM of the set
	return fECHO.begin();
}


OffsetToVerPlaneMeasIterator TDistMeasStation::getOffsetToVerPlaneMeasEndIterator()
{// returns an iterator pointing to the end dist ROM of the set
	return fECHO.end();
}


OffsetToVerPlaneMeasConstIter TDistMeasStation::getOffsetToVerPlaneMeasEndIterator() const
{// returns an const iterator pointing to the end dist ROM of the set
	return fECHO.end();
}


TOffsetToVerPlaneMeasurement* TDistMeasStation::addOffsetToVerPlaneMeas(TOffsetToVerPlaneMeasurement* meas)
{// adds a radial offset constraint to the station

	if (notInContainer(meas))
	{
		fECHO.push_back(*meas);
		return &(*(-- fECHO.end()));
	}
	else
	{
		cerr << "Offset not inserted : already stored" << endl;
		return 0;
	}
}


bool TDistMeasStation::notInContainer(TOffsetToVerPlaneMeasurement* meas) const
{// checks if the radial offset constraint is already in the container
	OffsetToVerPlaneMeasConstIter iter = getOffsetToVerPlaneMeasBeginIterator(); 
	OffsetToVerPlaneMeasConstIter iterEnd = getOffsetToVerPlaneMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd)
	{
		if ((*meas) == (*iter))
		{
			notPresent = false;
			iter  =iterEnd;
		}
		else
		{
			iter ++;
		}
	}

	return notPresent;
}


///////////////////////////////////////////////////////////////////////////////////
//ECTH (offset to theo. Plane) measurements related
///////////////////////////////////////////////////////////////////////////////////
OffsetToTheoPlaneMeasIterator TDistMeasStation::getOffsetToTheoPlaneMeasBeginIterator()
{//returns an iterator pointing to the first dist ROM of the set
	return fECTH.begin();
}


OffsetToTheoPlaneMeasConstIter TDistMeasStation::getOffsetToTheoPlaneMeasBeginIterator() const
{// returns an const iterator pointing to the first dist ROM of the set
	return fECTH.begin();
}


OffsetToTheoPlaneMeasIterator TDistMeasStation::getOffsetToTheoPlaneMeasEndIterator()
{// returns an iterator pointing to the end dist ROM of the set
	return fECTH.end();
}


OffsetToTheoPlaneMeasConstIter TDistMeasStation::getOffsetToTheoPlaneMeasEndIterator() const
{// returns an const iterator pointing to the end dist ROM of the set
	return fECTH.end();
}


TOffsetToTheoPlaneMeasurement* TDistMeasStation::addOffsetToTheoPlaneMeas(TOffsetToTheoPlaneMeasurement* meas)
{// adds a radial offset constraint to the station

	if (notInContainer(meas))
	{
		fECTH.push_back(*meas);
		return &(*(-- fECTH.end()));
	}
	else
	{
		cerr << "Offset not inserted : already stored" << endl;
		return 0;
	}
}


bool TDistMeasStation::notInContainer(TOffsetToTheoPlaneMeasurement* meas) const
{// checks if the radial offset constraint is already in the container
	OffsetToTheoPlaneMeasConstIter iter = getOffsetToTheoPlaneMeasBeginIterator(); 
	OffsetToTheoPlaneMeasConstIter iterEnd = getOffsetToTheoPlaneMeasEndIterator();

	bool notPresent = true;

	while (iter != iterEnd)
	{
		if ((*meas) == (*iter))
		{
			notPresent = false;
			iter  =iterEnd;
		}
		else
		{
			iter ++;
		}
	}

	return notPresent;
}




//////////////////////////////////////////////////////////////////////////////////
//end
//////////////////////////////////////////////////////////////////////////////////
