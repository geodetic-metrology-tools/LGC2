// 
// TWorkingStations
//
// Class for an instrument station container as defined for survey purposes
//

#include	"TWorkingStations.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TWorkingStations::TWorkingStations():
fLastDistNbr(0), fLastTheodNbr(0), fLastWPSNbr(0), fLastLevelNbr(0), fLastWTWNbr(0)
{


}


TWorkingStations::~TWorkingStations()
{

}

///////////////////
// copy constructor
///////////////////
TWorkingStations::TWorkingStations(const TWorkingStations & source){

	fLastDistNbr = source.fLastDistNbr;
	fLastTheodNbr = source.fLastTheodNbr;
	fLastWPSNbr = source.fLastWPSNbr;
	fLastLevelNbr = source.fLastLevelNbr;
	fLastDigiLevelNbr = source.fLastDigiLevelNbr;
	fDistStations = source.fDistStations;
	fTheodStations = source.fTheodStations;
	fWPSStations = source.fWPSStations;
	fLevelStations = source.fLevelStations;
	fDigiLevelStations = source.fDigiLevelStations;
	fWTWStations = source.fWTWStations;
}

//////////////////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the theodolite station stationed on the specified point
//////////////////////////////////////////////////////////////////////////////////////////
TheodStIterator TWorkingStations::getTheodStation(TSpatialPointName setupPt) {

	TheodStIterator iterSt = getTheodStBeginIterator();
	TheodStIterator iterStEnd = getTheodStEndIterator();

	bool notFound = true;

	while ((iterSt != iterStEnd) && notFound == true){

		if ((iterSt->getStationedPoint()) == setupPt)
			notFound = false;
		else
			iterSt++;
	}

	return iterSt;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the theodolite station stationed on the specified point
///////////////////////////////////////////////////////////////////////////////////////////////
TheodStConstIter TWorkingStations::getTheodStation(TSpatialPointName setupPt) const{

	TheodStConstIter iterSt = getTheodStBeginIterator();
	TheodStConstIter iterStEnd = getTheodStEndIterator();

	bool notFound = true;

	while ((iterSt != iterStEnd) && notFound == true){

		if ((iterSt->getStationedPoint()) == setupPt)
			notFound = false;
		else
			iterSt++;
	}

	return iterSt;
}


////////////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to the firts element of the theodolite stations container
////////////////////////////////////////////////////////////////////////////////////////
TheodStIterator TWorkingStations::getTheodStBeginIterator() {

	return fTheodStations.begin();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to the firts element of the theodolite stations container
/////////////////////////////////////////////////////////////////////////////////////////////
TheodStConstIter TWorkingStations::getTheodStBeginIterator() const{

	return fTheodStations.begin();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to one past the last element of the theodolite stations container
////////////////////////////////////////////////////////////////////////////////////////////////
TheodStIterator TWorkingStations::getTheodStEndIterator() {

	return fTheodStations.end();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to one past the last element of the theodolite stations container
/////////////////////////////////////////////////////////////////////////////////////////////////////
TheodStConstIter TWorkingStations::getTheodStEndIterator() const{

	return fTheodStations.end();
}

/////////////////////////////////////////////////////////////////////////////////////////
// returns an iterator pointing to the dist meas station stationed on the specified point
/////////////////////////////////////////////////////////////////////////////////////////
DistStIterator TWorkingStations::getDistStation(TSpatialPointName setupPt) {

	DistStIterator iterSt = getDistStBeginIterator();
	DistStIterator iterStEnd = getDistStEndIterator();

	bool notFound = true;

	while ((iterSt != iterStEnd) && notFound == true){

		if ((iterSt->getStationedPoint()) == setupPt)
			notFound = false;
		else
			iterSt++;
	}

	return iterSt;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// returns a const iterator pointing to the dist meas station stationed on the specified point
//////////////////////////////////////////////////////////////////////////////////////////////
DistStConstIter TWorkingStations::getDistStation(TSpatialPointName setupPt) const{

	DistStConstIter iterSt = getDistStBeginIterator();
	DistStConstIter iterStEnd = getDistStEndIterator();

	bool notFound = true;

	while ((iterSt != iterStEnd) && notFound == true){

		if ((iterSt->getStationedPoint()) == setupPt)
			notFound = false;
		else
			iterSt++;
	}

	return iterSt;
}



//////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to the first element of the dist stations container
//////////////////////////////////////////////////////////////////////////////////
DistStIterator TWorkingStations::getDistStBeginIterator() {

	return fDistStations.begin();
}

///////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to the first element of the dist stations container
///////////////////////////////////////////////////////////////////////////////////////
DistStConstIter TWorkingStations::getDistStBeginIterator() const{

	return fDistStations.begin();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to one past the last element of the dist stations container
//////////////////////////////////////////////////////////////////////////////////////////
DistStIterator TWorkingStations::getDistStEndIterator() {

	return fDistStations.end();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to one past the last element of the dist stations container
///////////////////////////////////////////////////////////////////////////////////////////////
DistStConstIter TWorkingStations::getDistStEndIterator() const{

	return fDistStations.end();
}


/////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to the first element of the WPS stations container
/////////////////////////////////////////////////////////////////////////////////
WPSStIterator TWorkingStations::getWPSStBeginIterator() {

	return fWPSStations.begin();
}

//////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to the first element of the WPS stations container
//////////////////////////////////////////////////////////////////////////////////////
WPSStConstIter TWorkingStations::getWPSStBeginIterator() const{

	return fWPSStations.begin();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to one past the last element of the WPS stations container
/////////////////////////////////////////////////////////////////////////////////////////
WPSStIterator TWorkingStations::getWPSStEndIterator() {

	return fWPSStations.end();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to one past the last element of the WPS stations container
//////////////////////////////////////////////////////////////////////////////////////////////
WPSStConstIter TWorkingStations::getWPSStEndIterator() const{

	return fWPSStations.end();
}


///////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to the first element of the Level stations container
///////////////////////////////////////////////////////////////////////////////////
LevelStIterator TWorkingStations::getLevelStBeginIterator(bool isDLEV) {

	if (isDLEV)
		return fDigiLevelStations.begin();
	else
		return fLevelStations.begin();
}

////////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to the first element of the Level stations container
////////////////////////////////////////////////////////////////////////////////////////
LevelStConstIter TWorkingStations::getLevelStBeginIterator(bool isDLEV) const{
	
	if (isDLEV)
		return fDigiLevelStations.begin();
	else
		return fLevelStations.begin();
}

///////////////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to one past the last element of the Level stations container
///////////////////////////////////////////////////////////////////////////////////////////
LevelStIterator TWorkingStations::getLevelStEndIterator(bool isDLEV) {
	
	if (isDLEV)
		return fDigiLevelStations.end();
	else
		return fLevelStations.end();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to one past the last element of the Level stations container
////////////////////////////////////////////////////////////////////////////////////////////////
LevelStConstIter TWorkingStations::getLevelStEndIterator(bool isDLEV) const{
	
	if (isDLEV)
		return fDigiLevelStations.end();
	else
		return fLevelStations.end();
}


/////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to the first element of the WTW stations container
/////////////////////////////////////////////////////////////////////////////////
WTWStIterator TWorkingStations::getWTWStBeginIterator() {

	return fWTWStations.begin();
}

//////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to the first element of the WTW stations container
//////////////////////////////////////////////////////////////////////////////////////
WTWStConstIter TWorkingStations::getWTWStBeginIterator() const{

	return fWTWStations.begin();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Return an iterator pointing to one past the last element of the WTW stations container
/////////////////////////////////////////////////////////////////////////////////////////
WTWStIterator TWorkingStations::getWTWStEndIterator() {

	return fWTWStations.end();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Return a const iterator pointing to one past the last element of the WTW stations container
//////////////////////////////////////////////////////////////////////////////////////////////
WTWStConstIter TWorkingStations::getWTWStEndIterator() const{

	return fWTWStations.end();
}




///////////////////////////////////////
// Adds a theodolite station to the set
///////////////////////////////////////
TTheodoliteStation* TWorkingStations::addTheodStation(TTheodoliteStation *ts){

	//possible attribution of a default fSetup string
	string empty("");
	if (ts->getSetup() == empty){
		ostringstream oss;
		fLastTheodNbr ++;
		oss << "TheodSt" << fLastTheodNbr;
		ts->setSetup(oss.str());
	}

	//insertion in the container
	//if (notInContainer(ts)){
		fTheodStations.push_back(*ts);
		return &(*(--fTheodStations.end()));
	//}
	//else{
	//	cerr << "Theod station not inserted : already stored\n";
	//	return 0;
	//}
	
}

///////////////////////////////////////////////////////////////
// checks if the theodolite station is already in the container
///////////////////////////////////////////////////////////////
bool TWorkingStations::notInContainer(TTheodoliteStation* thSt) const{

	TheodStConstIter iter = getTheodStBeginIterator();
	TheodStConstIter iterEnd = getTheodStEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*thSt) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}

/////////////////////////////////
// Adds an distance measuring station to the set
/////////////////////////////////
TDistMeasStation* TWorkingStations::addDistStation(TDistMeasStation *ds){

	//possible attribution of a default fSetup string
	string empty("");
	if (ds->getSetup() == empty){
		ostringstream oss;
		fLastDistNbr ++;
		oss << "DistSt" << fLastDistNbr;
		ds->setSetup(oss.str());
	}
	
	//insertion in the container
	//if (notInContainer(ds)){
		fDistStations.push_back(*ds);
		return &(*(--fDistStations.end()));
	//}
	//else{
		//cerr << "Dist. station not inserted : already in container\n";
		//return 0;
	//}
	
}

////////////////////////////////////////////////////////
// checks if the EDM station is already in the container
////////////////////////////////////////////////////////
bool TWorkingStations::notInContainer(TDistMeasStation* dSt) const{

	DistStConstIter iter = getDistStBeginIterator();
	DistStConstIter iterEnd = getDistStEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*dSt) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}

/////////////////////////////////
// Adds an WPS station to the set
/////////////////////////////////
TWPSStation* TWorkingStations::addWPSStation(TWPSStation *wpss){

	//possible attribution of a default fSetup string
	string empty("");
	if (wpss->getSetup() == empty){
		ostringstream oss;
		fLastWPSNbr ++;
		oss << "WPSSt" << fLastWPSNbr;
		wpss->setSetup(oss.str());
	}
	
	//insertion in the container
	if (notInContainer(wpss)){
		fWPSStations.push_back(*wpss);
		return &(*(--fWPSStations.end()));
	}
	else{
		cerr << "WPS station not inserted : already in container\n";
		return 0;
	}
	
}

////////////////////////////////////////////////////////
// checks if the WPS station is already in the container
////////////////////////////////////////////////////////
bool TWorkingStations::notInContainer(TWPSStation* wpsSt) const{

	WPSStConstIter iter = getWPSStBeginIterator();
	WPSStConstIter iterEnd = getWPSStEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*wpsSt) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}


/////////////////////////////////
// Adds an Level station to the container
/////////////////////////////////
TLevelStation* TWorkingStations::addLevelStation(TLevelStation *levelSt, bool isDLEV){

	//possible attribution of a default fSetup string
	string empty("");
	if (levelSt->getSetup() == empty){
		ostringstream oss;
		fLastLevelNbr ++;
		oss << "LevelSt" << fLastLevelNbr;
		levelSt->setSetup(oss.str());
	}
	
	//insertion in the container
	//if (notInContainer(levelSt)){
		LevelStContainer &levelcontainer =  isDLEV ? 
											fDigiLevelStations : 
											fLevelStations;
		levelcontainer.push_back(*levelSt);
		return &(*(--levelcontainer.end()));
	//}
	//else{
	//	cerr << "Level station not inserted : already in container\n";
	//	return 0;
	//}
	
}

////////////////////////////////////////////////////////
// checks if the Level station is already in the container
////////////////////////////////////////////////////////
bool TWorkingStations::notInContainer(TLevelStation* levelSt, bool isDLEV) const{

	LevelStConstIter iter = getLevelStBeginIterator(isDLEV);
	LevelStConstIter iterEnd = getLevelStEndIterator(isDLEV);

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*levelSt) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}

///////////////////////////////////////
// Adds an WTW station to the container
///////////////////////////////////////
TWTWDistStation* TWorkingStations::addWTWStation(TWTWDistStation *wtwSt){

	//possible attribution of a default fSetup string
	string empty("");
	if (wtwSt->getSetup() == empty){
		ostringstream oss;
		fLastWTWNbr ++;
		oss << "WTWSt" << fLastWTWNbr;
		wtwSt->setSetup(oss.str());
	}
	
	//insertion in the container
	if (notInContainer(wtwSt)){
		fWTWStations.push_back(*wtwSt);
		return &(*(--fWTWStations.end()));
	}
	else{
		cerr << "WTW station not inserted : already in container\n";
		return 0;
	}
	
}

////////////////////////////////////////////////////////
// checks if the WTW station is already in the container
////////////////////////////////////////////////////////
bool TWorkingStations::notInContainer(TWTWDistStation* wtwSt) const{

	WTWStConstIter iter = getWTWStBeginIterator();
	WTWStConstIter iterEnd = getWTWStEndIterator();

	bool notPresent = true;

	while (iter != iterEnd){

		if ((*wtwSt) == (*iter)){
			notPresent = false;
			iter  =iterEnd;
		}
		else
			iter ++;
	}

	return notPresent;
}



//////////////////////////////////////////////////
// Return the number of stored theodolite stations
//////////////////////////////////////////////////
int TWorkingStations::numberOfTheodStations(){

	return fTheodStations.size();
}


///////////////////////////////////////////
// Return the number of stored dist. meas. stations
///////////////////////////////////////////
int TWorkingStations::numberOfDistMeasStations(){

	return fDistStations.size();
}

///////////////////////////////////////////
// Return the number of stored wps stations
///////////////////////////////////////////
int TWorkingStations::numberOfWPSStations(){

	return fWPSStations.size();
}

/////////////////////////////////////////////
// Return the number of stored Level stations
/////////////////////////////////////////////
int TWorkingStations::numberOfLevelStations(bool isDLEV){

	if (isDLEV)
		return fDigiLevelStations.size();
	else
		return fLevelStations.size();
}

///////////////////////////////////////////
// Return the number of stored WTW stations
///////////////////////////////////////////
int TWorkingStations::numberOfWTWStations(){

	return fWTWStations.size();
}




