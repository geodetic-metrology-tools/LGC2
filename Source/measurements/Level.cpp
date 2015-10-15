// 
// Level.cpp : implementation file
//
// Class for an EDM station

#include "Level.h"

Level::~Level()
{
	hash_map<string, const Staff*>::iterator iter = staffs.begin();

	while (iter != staffs.end())
	{
		delete iter->second;
		iter++;
	}
}

void Level::addStaff(const Staff* staff, bool def)
{
	staffs[staff->getTargetID()] = staff;
	if (def)
	{
		defaultStaff = staff;
	}
}






























//////////////////////////////////////////////////
// responds to an additional readyToActivate Child
//////////////////////////////////////////////////
/*void TTheodolite::childReadyToActivate(int measCount, int eqCount, int unkCount){

	fReadyToActivateEqns += eqCount;
//	fStationedPoint->updatePotentialEqCount(eqCount);

	if (fReadyToActivateEqns >= 
//		(fStationedPoint->getVariableDimension()-fStationedPoint->getPotentialEqCount())){

		fUsedInCalc = true;

		activateReadyChildren();
	}
//}

///////////////////////////////////////////
// activates the ready to activate children
///////////////////////////////////////////
void TTheodolite::activateReadyChildren() {

	HorAngROMIterator iter = fMadeAngleROMs.begin();
	HorAngROMIterator iterEnd = fMadeAngleROMs.end();

	while (iter != iterEnd){
		iter->parentActivated();
		if (iter->readyToActivate())
			iter->activateReadyChildren();
		iter++;
	}
}
		
////////////////////////////////////////////////////////////////////////////
// tells the children the station has a sufficient total number of equations
////////////////////////////////////////////////////////////////////////////
void TTheodolite::sendEnoughTotalEqns() {

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

//////////////////////////////////////////////////////////
// tells the ROM children that the station is disactivated
//////////////////////////////////////////////////////////
void TTheodolite::sendDisactivatedToChildren() {
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

/////////////////////////////////////////////////////////
// tells the ROM children that the station is Activated
/////////////////////////////////////////////////////////
void TTheodolite::sendActivatedToChildren() {
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
