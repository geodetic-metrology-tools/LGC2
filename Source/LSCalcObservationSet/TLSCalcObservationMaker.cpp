////////////////////////////////////////////////////////////////////
// TLSCalcObservationMaker.cpp :Implementation file
// Creates the Least Squares calculation data set from the observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////


#include <list>

#include "TRefSystemFactory.h"
#include "TLSCalcObservationMaker.h"

// for simulations
//#include <time.h>


///////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR
///////////////////////////////////////

// Default constructor
TLSCalcObservationMaker::TLSCalcObservationMaker() {

	fError = "";
}

// Constructor
TLSCalcObservationMaker::TLSCalcObservationMaker(bool) {

	fError = "";
}

// Destructor
TLSCalcObservationMaker::~TLSCalcObservationMaker() {

}

// Processes the observations
bool TLSCalcObservationMaker::processData(const ObservationSet& obs,LSCalcDataSet& lsobs) {

	bool processed = false;
	//processes horizontal angle observations
	processHorAngleObs(obs,lsobs);
	//processes zenithal angle observations
	processZenDistObs(obs,lsobs);
	//processes spatial distance observations
	processSpaDistObs(obs,lsobs);
	//processes horizontal distance observations
	processHorDistObs(obs,lsobs);
	//processes vertical distance observations
	// Both cases, DLEV(1) and DVER(0)
	processVertDistObs(obs,lsobs,0);
	processVertDistObs(obs,lsobs,1);

	//processes offset to vertical line observations */
	processOffsetToVerLineObs(obs,lsobs);
	//processes offset to spatial line observations */
	processOffsetToSpaLineObs(obs,lsobs);
	//processes offset to vertical plane observations */
	processOffsetToVerPlaneObs(obs,lsobs);
	//processes offset to theodolite plane observations */
	processOffsetToTheoPlaneObs(obs,lsobs);

	//processes gyro. orientation observations */
	processGyroOrieObs(obs,lsobs);

	processRadOffCnstrObs(obs,lsobs);
	processOrieCnstrObs(obs,lsobs);

	if (fError == "")
		processed = true;
	else
		cout << fError << endl;

	return processed;
}






/////////////////////////////////////////////////////////////////////////////////
//PROCESS PRIVATE FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////
	
// Processing of hor. ang. observations leading to the creation of LSCalc hor ang. observations and associated parameters
void TLSCalcObservationMaker::processHorAngleObs(const ObservationSet& obs,LSCalcDataSet& lsobs) {

	HorAngObsConstIter iterB = obs.beginHorAng();
	HorAngObsConstIter  iterE = obs.endHorAng();

	while (iterB!=iterE) {
		if (iterB->isActive()) {
			 
			// Addition of the orientation to the list ( if not already inserted ) and returns an iterator
			LSOrientIter orientation = lsobs.push_backOrient(TLSCalcOrientationParam(iterB->getRomV0(),convertV0Status(iterB->getRomV0Status()),iterB->getRomV0Name()));
			// check of the push_back method's validity
			if ( orientation == lsobs.endOrient() ) {
				string error("Orientation (Hor.Ang.Observation): ");
				error += iterB->getRomV0Name();
				error += " was not inserted \n";
				fError += error;
			}

			// Addition of the stationned position to the list ( if not already inserted ) and returns an iterator		
			
			LSPosVecIter stPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));	
			// check of the push_back method's validity
			if ( ( stPos == lsobs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Stationed position (Hor.Ang.Observation): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}
			
			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetPosVec(),convertPVStatus(iterB->getTargetPosStatus()),iterB->getTargetPosName()));
			// check of the push_back method's validity
			if ( ( tgPos == lsobs.endPV() ) || ( tgPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Target position (Hor.Ang.Observation): ");
				error += iterB->getTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the angle measurement cte (if not inserted) 
			TLSCalcOrientationParam orieParam;
			orieParam.setProvisionalValue(TAngle(LITERAL(0.0)),TAngle(LITERAL(0.0)),iterB->getConstantValue());
			orieParam.setStatus(convertV0Status(iterB->getConstantStatus()));
			orieParam.setName(iterB->getConstantName());
			LSOrientIter cte = lsobs.push_backOrient(orieParam);
			// check of the push_back method's validity
			if ( cte == lsobs.endOrient() ) {
				string error("Angle constant (Hor.Ang.Observation): ");
				error += iterB->getConstantName();
				error += " was not inserted \n";
				fError += error;
			}
		

			// Addition of the new ls calc hor ang observation to the list 
			LSHorAngIter lsHaIt = lsobs.push_backLSHorAng(TLSCalcHorAngleObservation(iterB,orientation,stPos,tgPos, cte));
			// check of the push_back method's validity
			if ( lsHaIt == lsobs.endLSHorAng() ) {
				string error("New LSCalc horizontal angle observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target position: ";
				error += iterB->getTargetPosName();
				error += " was not inserted\n";
				fError += error;
			}
			
		}
		iterB++;
	}

}

// Processing of zen.dist. observations leading to the creation of LSCalc zen.dist. observations and associated parameters
void TLSCalcObservationMaker::processZenDistObs(const ObservationSet& obs,LSCalcDataSet& lsobs) {
	
	ZenDistObsConstIter iterB = obs.beginZenDist();
	ZenDistObsConstIter iterE = obs.endZenDist();

	while (iterB!=iterE) {
		if (iterB->isActive()) {

			// Addition of the prism's height to the list (if not already inserted) and returns an iterator
/*			LSLengthIter lengthP = lsobs.push_backLength(TLSCalcLengthParam(iterB->getPrismHeight(),convertLengthStatus(iterB->getPrismHeightStatus()),iterB->getPHeightName()));
			// checks that the prism height status is set to fixed + check of the push_back method's validity
			if ( ( lengthP == lsobs.endLength() ) || ( lengthP->getStatus() == TALSCalcParameter::kVariable ) ) {
				string error("Prism Height (Zenith Dist. Observations): ");
				error += iterB->getPHeightName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}*/
			TLength lengthP = iterB->getPrismHeight();
		

			// Addition of the instrument's height to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthI = lsobs.push_backLength(TLSCalcLengthParam(iterB->getInstrumentHeight(),convertLengthStatus(iterB->getInstHeightStatus()),iterB->getIHeightName()));
			// check of the push_back method's validity
			if ( lengthI == lsobs.endLength() ) {
				string error("Instrument heigth (Zenith Dist. Observations): ");
				error += iterB->getIHeightName();
				error += " was not inserted \n";
				fError += error;
			}
			// Addition of the stationned position to the list ( if not already inserted ) and returns an iterator		
			LSPosVecIter stPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsobs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Stationed position (Zenith Dist. Observations): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetPosVec(),convertPVStatus(iterB->getTargetPosStatus()),iterB->getTargetPosName()));
			// check of the push_back method's validity
			if ( ( tgPos == lsobs.endPV() ) || ( tgPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Target position (Zenith Dist. Observations): ");
				error += iterB->getTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the new ls calc zen dist observation to the list
			LSZenDistIter lsZdIt = lsobs.push_backLSZenDist(TLSCalcZenithDistObservation(iterB,lengthI,lengthP,stPos,tgPos));
			// check of the push_back method's validity
			if ( lsZdIt == lsobs.endLSZenDist() ) {
				string error("New LSCalc zenith distance observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target position: ";
				error += iterB->getTargetPosName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;	
	}
}

// Processing of spatial dist. observations leading to the creation of LSCalc spatial dist. observations and associated parameters
void	TLSCalcObservationMaker::processSpaDistObs(const ObservationSet& obs,LSCalcDataSet& lsobs) {
	
	SpaDistObsConstIter iterB = obs.beginSpaDist();
	SpaDistObsConstIter iterE = obs.endSpaDist();

	while (iterB!=iterE) {
		if (iterB->isActive()) {

			// Addition of the prism's height to the list (if not already inserted) and returns an iterator
/*			LSLengthIter lengthP = lsobs.push_backLength(TLSCalcLengthParam(iterB->getPrismHeight(),convertLengthStatus(iterB->getPrismHeightStatus()),iterB->getPHeightName()));
			// checks that the prism height status is set to fixed + check of the push_back method's validity
			if ( ( lengthP == lsobs.endLength() ) || ( lengthP->getStatus() == TALSCalcParameter::kVariable ) ) {
				string error("Prism Height (Spatial Dist. Observations): ");
				error += iterB->getPHeightName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}*/
			TLength lengthP = iterB->getPrismHeight();


			// Addition of the instrument's height to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthI = lsobs.push_backLength(TLSCalcLengthParam(iterB->getInstrumentHeight(),convertLengthStatus(iterB->getInstHeightStatus()),iterB->getIHeightName()));
			// check of the push_back method's validity
			if ( lengthI == lsobs.endLength() ) {
				string error("Instrument constant (Spatial Dist. Observations): ");
				error += iterB->getIHeightName();
				error += " was not inserted \n";
				fError += error;
			}

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(TLSCalcLengthParam(iterB->getConstantValue(),convertLengthStatus(iterB->getConstantStatus()),iterB->getConstantName()));
			// check of the push_back method's validity
			if ( lengthCst == lsobs.endLength() ) {
				string error("The constant (Spatial Dist. Observations): ");
				error += iterB->getConstantName();
				error += " was not inserted\n";
				fError += error;
			}


			// Addition of the stationned position to the list ( if not already inserted ) and return of an iterator		
			LSPosVecIter stPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsobs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Stationed position (Spatial Dist. Observations): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetPosVec(),convertPVStatus(iterB->getTargetPosStatus()),iterB->getTargetPosName()));
			// check of the push_back method's validity
			if ( ( tgPos == lsobs.endPV() ) || ( tgPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Target position (Spatial Dist. Observations): ");
				error += iterB->getTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}


			// Addition of the new ls calc spatial dist observation to the list
			LSSpaDistIter lsSdIt = lsobs.push_backLSSpaDist(TLSCalcSpatialDistObservation(iterB,lengthI,lengthP,lengthCst,stPos,tgPos));
			// check of the push_back method's validity
			if ( lsSdIt == lsobs.endLSSpaDist() ) {
				string error("New LSCalc spatial distance observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target position: ";
				error += iterB->getTargetPosName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}


// Processing of horizontal dist. observations leading to the creation of LSCalc horizontal dist. observations and associated parameters
void	TLSCalcObservationMaker::processHorDistObs(const ObservationSet& obs,LSCalcDataSet& lsobs) {
	
	HorDistObsConstIter iterB = obs.beginHorDist();
	HorDistObsConstIter iterE = obs.endHorDist();

	while (iterB!=iterE) {
		if (iterB->isActive()) {

			// Addition of the stationned position to the list ( if not already inserted ) and returns an iterator		
			LSPosVecIter stPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsobs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Stationed position (Horizontal Dist. Observations): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetPosVec(),convertPVStatus(iterB->getTargetPosStatus()),iterB->getTargetPosName()));
			// check of the push_back method's validity
			if ( ( tgPos == lsobs.endPV() ) || ( tgPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Target position (Horizontal Dist. Observations): ");
				error += iterB->getTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(TLSCalcLengthParam(iterB->getConstantValue(),convertLengthStatus(iterB->getConstantStatus()),iterB->getConstantName()));
			// check of the push_back method's validity
			if ( lengthCst == lsobs.endLength() ) {
				string error("The constant (Hor. Dist. Observations): ");
				error += iterB->getConstantName();
				error += " was not inserted\n";
				fError += error;
			}

			// Addition of the new ls calc hor dist observation to the list
			LSHorDistIter lsHdIt = lsobs.push_backLSHorDist(TLSCalcHorDistObservation(iterB,stPos,tgPos,lengthCst));
			// check of the push_back method's validity
			if ( lsHdIt == lsobs.endLSHorDist() ) {
				string error("New LSCalc horizontal distance observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target position: ";
				error += iterB->getTargetPosName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}

// Processing of vertical dist. observations leading to the creation of LSCalc vertical dist. observations and associated parameters
void	TLSCalcObservationMaker::processVertDistObs(const ObservationSet& obs,LSCalcDataSet& lsobs, bool isDLEV) {
	
	VertDistObsConstIter iterB = obs.beginVertDist(isDLEV);
	VertDistObsConstIter iterE = obs.endVertDist(isDLEV);

	while (iterB!=iterE) {
		if (iterB->isActive()) {

			// Addition of the reference position to the list ( if not already inserted ) and returns an iterator		
			LSPosVecIter refPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DRefPosVec(),convertPVStatus(iterB->getRefPosStatus()),iterB->getRefPosName()));
			if ( ( refPos == lsobs.endPV() ) || ( refPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Reference position (Vertical Dist. Observations): ");
				error += iterB->getRefPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator		
			LSPosVecIter tgPos = lsobs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetPosVec(),convertPVStatus(iterB->getTargetPosStatus()),iterB->getTargetPosName()));
			if ( ( tgPos == lsobs.endPV() ) || ( tgPos->getXStatus() == TALSCalcParameter::kNull ) ) {
				string error("Target position (Vertical Dist. Observations): ");
				error += iterB->getTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(TLSCalcLengthParam(iterB->getConstantValue(),convertLengthStatus(iterB->getConstantStatus()),iterB->getConstantName()));
			// check of the push_back method's validity
			if ( lengthCst == lsobs.endLength() ) {
				string error("The constant (Vert. Dist. Observations): ");
				error += iterB->getConstantName();
				error += " was not inserted\n";
				fError += error;
			}

			// Addition of the new ls calc hor dist observation to the list
			LSVertDistIter lsVdIt = lsobs.push_backLSVertDist(TLSCalcVertDistObservation(iterB,refPos,tgPos,lengthCst), isDLEV);
			// check of the push_back method's validity
			if ( lsVdIt == lsobs.endLSVertDist(isDLEV) ) {
				string error("New LSCalc vertical distance observation with reference position: ");
				error += iterB->getRefPosName();
				error += " to Target position: ";
				error += iterB->getTargetPosName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}


void	TLSCalcObservationMaker::processOffsetToVerLineObs(const ObservationSet& obs,LSCalcDataSet& lsObs)
{/* Processing of offset (ECVE) observations leading to the creation of
	LSCalc offset to vertical line observations and associated parameters*/

	
	OffsetToVerLineObsConstIter iterB = obs.beginOffsetToVerLine();
	OffsetToVerLineObsConstIter iterE = obs.endOffsetToVerLine();

	while (iterB!=iterE)
	{
		if (iterB->isActive())
		{

			// Addition of the stationned position to the list ( if not already inserted )	
			LSPosVecIter stPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsObs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Stationed position (Offset To Vertical Line Observations): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetPosVec(),convertPVStatus(iterB->getTargetPosStatus()),iterB->getTargetPosName()));
			// check of the push_back method's validity
			if ( ( tgPos == lsObs.endPV() ) || ( tgPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Target position (Offset To Vertival Line Observations): ");
				error += iterB->getTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsObs.push_backLength(TLSCalcLengthParam(iterB->getConstantValue(),convertLengthStatus(iterB->getConstantStatus()),iterB->getConstantName()));
			// check of the push_back method's validity
			if ( lengthCst == lsObs.endLength() )
			{
				string error("The constant (Vert. Dist. Observations): ");
				error += iterB->getConstantName();
				error += " was not inserted\n";
				fError += error;
			}

			// Addition of the new ls calc offset observation to the list
			LSOffsetToVerLineIter lsOffsetIt = lsObs.push_backLSOffsetToVerLine(TLSCalcOffsetToVerLineObservation(iterB,stPos,tgPos, lengthCst));
			// check of the push_back method's validity
			if ( lsOffsetIt == lsObs.endLSOffsetToVerLine() )
			{
				string error("New LSCalc offset to vertical line observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target line: ";
				error += iterB->getTargetLineName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}


void	TLSCalcObservationMaker::processOffsetToSpaLineObs(const ObservationSet& obs,LSCalcDataSet& lsObs)
{/* Processing of offset (ECSP) observations leading to the creation of
	LSCalc offset to spatial line observations and associated parameters*/

	
	OffsetToSpaLineObsConstIter iterB = obs.beginOffsetToSpaLine();
	OffsetToSpaLineObsConstIter iterE = obs.endOffsetToSpaLine();

	while (iterB!=iterE)
	{
		if (iterB->isActive())
		{

			// Addition of the stationned position to the list ( if not already inserted )	
			LSPosVecIter stPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsObs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Stationed position (Offset To Spatial Line Observations): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the first target point position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter firstTgPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DFirstTargetPosVec(),convertPVStatus(iterB->getFirstTargetPosStatus()),iterB->getFirstTargetPosName()));
			// check of the push_back method's validity
			if ( ( firstTgPos == lsObs.endPV() ) || ( firstTgPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("First point used to defined the target position (Offset To Spatial Line Observations): ");
				error += iterB->getFirstTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the second target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter secondTgPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DSecondTargetPosVec(),convertPVStatus(iterB->getSecondTargetPosStatus()),iterB->getSecondTargetPosName()));
			// check of the push_back method's validity
			if ( ( secondTgPos == lsObs.endPV() ) || ( secondTgPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Second point used to define the target position (Offset To Spatial Line Observations): ");
				error += iterB->getSecondTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsObs.push_backLength(TLSCalcLengthParam(iterB->getConstantValue(),convertLengthStatus(iterB->getConstantStatus()),iterB->getConstantName()));
			// check of the push_back method's validity
			if ( lengthCst == lsObs.endLength() )
			{
				string error("The constant (Vert. Dist. Observations): ");
				error += iterB->getConstantName();
				error += " was not inserted\n";
				fError += error;
			}

			// Addition of the new ls calc offset observation to the list
			LSOffsetToSpaLineIter lsOffsetIt = lsObs.push_backLSOffsetToSpaLine(TLSCalcOffsetToSpaLineObservation(iterB,stPos,firstTgPos, secondTgPos, lengthCst));
			// check of the push_back method's validity
			if ( lsOffsetIt == lsObs.endLSOffsetToSpaLine() )
			{
				string error("New LSCalc offset to spatial line observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target line: ";
				error += iterB->getTargetLineName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}


void	TLSCalcObservationMaker::processOffsetToVerPlaneObs(const ObservationSet& obs,LSCalcDataSet& lsObs)
{/* Processing of offset (ECHO) observations leading to the creation of
	LSCalc offset to vertical plane observations and associated parameters*/

	
	OffsetToVerPlaneObsConstIter iterB = obs.beginOffsetToVerPlane();
	OffsetToVerPlaneObsConstIter iterE = obs.endOffsetToVerPlane();

	while (iterB!=iterE)
	{
		if (iterB->isActive())
		{

			// Addition of the stationned position to the list ( if not already inserted )	
			LSPosVecIter stPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsObs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Stationed position (Offset To Vertical Plane Observations): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the first target point position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter firstTgPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DFirstTargetPosVec(),convertPVStatus(iterB->getFirstTargetPosStatus()),iterB->getFirstTargetPosName()));
			// check of the push_back method's validity
			if ( ( firstTgPos == lsObs.endPV() ) || ( firstTgPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("First point used to defined the target position (Offset To Spatial Line Observations): ");
				error += iterB->getFirstTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the second target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter secondTgPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DSecondTargetPosVec(),convertPVStatus(iterB->getSecondTargetPosStatus()),iterB->getSecondTargetPosName()));
			// check of the push_back method's validity
			if ( ( secondTgPos == lsObs.endPV() ) || ( secondTgPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Second point used to define the target position (Offset To Spatial Line Observations): ");
				error += iterB->getSecondTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsObs.push_backLength(TLSCalcLengthParam(iterB->getConstantValue(),convertLengthStatus(iterB->getConstantStatus()),iterB->getConstantName()));
			// check of the push_back method's validity
			if ( lengthCst == lsObs.endLength() )
			{
				string error("The constant (Vert. Dist. Observations): ");
				error += iterB->getConstantName();
				error += " was not inserted\n";
				fError += error;
			}

			// Addition of the new ls calc offset observation to the list
			LSOffsetToVerPlaneIter lsOffsetIt = lsObs.push_backLSOffsetToVerPlane(TLSCalcOffsetToVerPlaneObservation(iterB,stPos,firstTgPos, secondTgPos, lengthCst));
			// check of the push_back method's validity
			if ( lsOffsetIt == lsObs.endLSOffsetToVerPlane() )
			{
				string error("New LSCalc offset to vertical plane observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target plane: ";
				error += iterB->getTargetPlaneName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}


void	TLSCalcObservationMaker::processOffsetToTheoPlaneObs(const ObservationSet& obs,LSCalcDataSet& lsObs)
{/* Processing of offset (ECTH) observations leading to the creation of
	LSCalc offset to theodolite plane observations and associated parameters*/

	
	OffsetToTheoPlaneObsConstIter iterB = obs.beginOffsetToTheoPlane();
	OffsetToTheoPlaneObsConstIter iterE = obs.endOffsetToTheoPlane();

	while (iterB!=iterE)
	{
		if (iterB->isActive())
		{

			// Addition of the stationned position to the list ( if not already inserted )	
			LSPosVecIter stPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsObs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Stationed position (Offset To Vertical Plane Observations): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the first target point position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter theoPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetStPosVec(),convertPVStatus(iterB->getTargetTheoPosStatus()),iterB->getTargetTheoPosName()));
			// check of the push_back method's validity
			if ( ( theoPos == lsObs.endPV() ) || ( theoPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("First point used to defined the target position (Offset To Spatial Line Observations): ");
				error += iterB->getTargetTheoPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the orientation to the list ( if not already inserted ) and returns an iterator
			LSOrientIter orientation = lsObs.push_backOrient(TLSCalcOrientationParam(iterB->getTheoV0(),convertV0Status(iterB->getTheoV0Status()),iterB->getTheoV0Name()));
			// check of the push_back method's validity
			if ( orientation == lsObs.endOrient() )
			{
				string error("Target Orientation (Offset to Theodolite Plane Observation): ");
				error += iterB->getTheoV0Name();
				error += " was not inserted \n";
				fError += error;
			}

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsObs.push_backLength(TLSCalcLengthParam(iterB->getConstantValue(),convertLengthStatus(iterB->getConstantStatus()),iterB->getConstantName()));
			// check of the push_back method's validity
			if ( lengthCst == lsObs.endLength() )
			{
				string error("The constant (Vert. Dist. Observations): ");
				error += iterB->getConstantName();
				error += " was not inserted\n";
				fError += error;
			}

			// Addition of the new ls calc offset observation to the list
			LSOffsetToTheoPlaneIter lsOffsetIt = lsObs.push_backLSOffsetToTheoPlane(TLSCalcOffsetToTheoPlaneObservation(iterB,stPos,theoPos, orientation, lengthCst));
			// check of the push_back method's validity
			if ( lsOffsetIt == lsObs.endLSOffsetToTheoPlane() )
			{
				string error("New LSCalc offset to vertical plane observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target plane: ";
				error += iterB->getTargetPlaneName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}



void TLSCalcObservationMaker::processGyroOrieObs(const ObservationSet& obs,LSCalcDataSet& lsObs)
{// Processing of hor. ang. observations leading to the creation of LSCalc hor ang. observations and associated parameters

	GyroOrieObsConstIter iterB = obs.beginGyroOrie();
	GyroOrieObsConstIter  iterE = obs.endGyroOrie();

	while (iterB!=iterE)
	{
		if (iterB->isActive())
		{
			// Addition of the stationned position to the list ( if not already inserted ) and returns an iterator		
			LSPosVecIter stPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DStationPosVec(),convertPVStatus(iterB->getStationedPosStatus()),iterB->getStationedPosName()));	
			// check of the push_back method's validity
			if ( ( stPos == lsObs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Stationed position (Orientation Observation): ");
				error += iterB->getStationedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}
			
			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DTargetPosVec(),convertPVStatus(iterB->getTargetPosStatus()),iterB->getTargetPosName()));
			// check of the push_back method's validity
			if ( ( tgPos == lsObs.endPV() ) || ( tgPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Target position (Orientation Observation): ");
				error += iterB->getTargetPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the angle measurement cte (if not inserted) 
			TLSCalcOrientationParam orieParam;
			orieParam.setProvisionalValue(TAngle(LITERAL(0.0)),TAngle(LITERAL(0.0)),iterB->getConstantValue());
			orieParam.setStatus(convertV0Status(iterB->getConstantStatus()));
			orieParam.setName(iterB->getConstantName());
			LSOrientIter cte = lsObs.push_backOrient(orieParam);
			// check of the push_back method's validity
			if ( cte == lsObs.endOrient() )
			{
				string error("Angle constant (Orientation Observation): ");
				error += iterB->getConstantName();
				error += " was not inserted \n";
				fError += error;
			}
			
			// Addition of the new ls calc orientation observation to the list 
			LSGyroOrieIter lsOrieIt = lsObs.push_backLSGyroOrie(TLSCalcGyroOrientationObservation(iterB, stPos, tgPos, cte));
			// check of the push_back method's validity
			if ( lsOrieIt == lsObs.endLSGyroOrie() )
			{
				string error("New LSCalc orientation observation from Stationed position: ");
				error += iterB->getStationedPosName();
				error += " to Target position: ";
				error += iterB->getTargetPosName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}

void	TLSCalcObservationMaker::processRadOffCnstrObs(const ObservationSet& obs,LSCalcDataSet& lsObs)
{/* Processing of offset (ECTH) observations leading to the creation of
	LSCalc offset to theodolite plane observations and associated parameters*/

	
	RadOffCnstrObsConstIter iterB = obs.beginRadOffCnstr();
	RadOffCnstrObsConstIter iterE = obs.endRadOffCnstr();

	while (iterB!=iterE)
	{
		if (iterB->isActive())
		{

			// Addition of the point position to the list ( if not already inserted )	
			LSPosVecIter stPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DPosVec(),convertPVStatus(iterB->getPosStatus()),iterB->getPosName()));
			// check of the push_back method's validity
			if ( ( stPos == lsObs.endPV() ) || ( stPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Radial constraint on position (Radial offset constraint Observations): ");
				error += iterB->getPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the new ls calc offset observation to the list
			LSRadOffCnstrIter lsOffsetIt = lsObs.push_backLSRadOffCnstr(TLSCalcRadialOffsetCnstrObservation(iterB,stPos));
			// check of the push_back method's validity
			if ( lsOffsetIt == lsObs.endLSRadOffCnstr() )
			{
				string error("New LSCalc radial offset constraint on position: ");
				error += iterB->getPosName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}


void	TLSCalcObservationMaker::processOrieCnstrObs(const ObservationSet& obs,LSCalcDataSet& lsObs)
{/* Processing of offset (ECTH) observations leading to the creation of
	LSCalc offset to theodolite plane observations and associated parameters*/

	
	OrieCnstrObsConstIter iterB = obs.beginOrieCnstr();
	OrieCnstrObsConstIter iterE = obs.endOrieCnstr();

	while (iterB!=iterE)
	{
		if (iterB->isActive())
		{

			// Addition of the point position to the list ( if not already inserted )	
			LSPosVecIter fixedPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DFixedPosVec(),convertPVStatus(iterB->getFixedPosStatus()),iterB->getFixedPosName()));
			// check of the push_back method's validity
			if ( ( fixedPos == lsObs.endPV() ) || ( fixedPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Orientation constraint on fixed position (Orientation constraint Observations): ");
				error += iterB->getFixedPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the point position to the list ( if not already inserted )	
			LSPosVecIter refPos = lsObs.push_backPosVec(TLSCalcPosVectorParam(iterB->get3DRefPosVec(),convertPVStatus(iterB->getRefPosStatus()),iterB->getRefPosName()));
			// check of the push_back method's validity
			if ( ( refPos == lsObs.endPV() ) || ( refPos->getXStatus() == TALSCalcParameter::kNull ) )
			{
				string error("Orientation constraint on reference position (Orientation constraint Observations): ");
				error += iterB->getRefPosName();
				error += " was not correctly inserted or status was wrongly set\n";
				fError += error;
			}

			// Addition of the new ls calc offset observation to the list
			LSOrieCnstrIter lsOffsetIt = lsObs.push_backLSOrieCnstr(TLSCalcOrientationCnstrObservation(iterB, fixedPos, refPos));
			// check of the push_back method's validity
			if ( lsOffsetIt == lsObs.endLSOrieCnstr() )
			{
				string error("New LSCalc orientation constraint between position: ");
				error += iterB->getFixedPosName();
				error += ", an position : ";
				error += iterB->getRefPosName();
				error += " was not inserted\n";
				fError += error;
			}
		}
		iterB++;
	}
}


//////////////////////////////////////////////////////////////////////////////////
//FUNCTION CONVERT
//////////////////////////////////////////////////////////////////////////////////

// Converts length's status to ls calc length's status 
TALSCalcParameter::ELSStatus	TLSCalcObservationMaker::convertLengthStatus(TAMeasurement::ECalcStatus ecs) {

	// Length's status
	TALSCalcParameter::ELSStatus lStatus;

	// Conversion
	if ( ecs == TAMeasurement::kFixed )
		lStatus = TALSCalcParameter::kFixed;
	else
		lStatus = TALSCalcParameter::kVariable;
	
	return lStatus;
}

// Converts the original v0 status to an orientation's struct LSParaStatus 
struct LSParaStatus		TLSCalcObservationMaker::convertV0Status(TAMeasurement::ECalcStatus ecs) {

	// Orientation's struct status
	struct LSParaStatus oStatus;
		
	// Preparation of orientation's status
	oStatus.first = TALSCalcParameter::kFixed;
	oStatus.second = TALSCalcParameter::kFixed;
	if (ecs == TAMeasurement::kFixed)
		oStatus.third = TALSCalcParameter::kFixed;
	else
		oStatus.third = TALSCalcParameter::kVariable;

	return oStatus;
}

// Converts the original spatial point status to a position vector's struct LSParaStatus
struct LSParaStatus		TLSCalcObservationMaker::convertPVStatus(TSpatialStatus::ESpatialStatus ess) {

	// Position vector's struct status: calage by default
	struct LSParaStatus pvStatus(TALSCalcParameter::kFixed);

	switch (ess) {
		case TSpatialStatus::kPosNull: 
		case TSpatialStatus::kUnknown: 
			fError = "the status of the position is wrongly set\n";
			pvStatus.setAll(TALSCalcParameter::kNull);
		break;
		case TSpatialStatus::kCala: // nothing to do, all fixed by default
		break;
		case TSpatialStatus::kVx:
			pvStatus.first = TALSCalcParameter::kVariable;
		break;
		case TSpatialStatus::kVy:
			pvStatus.second = TALSCalcParameter::kVariable;
		break;
		case TSpatialStatus::kVz:
			pvStatus.third = TALSCalcParameter::kVariable;
		break;
		case TSpatialStatus::kVxy:
			pvStatus.first  = TALSCalcParameter::kVariable;
			pvStatus.second = TALSCalcParameter::kVariable;
		break;
		case TSpatialStatus::kVxz:
			pvStatus.first = TALSCalcParameter::kVariable;
			pvStatus.third = TALSCalcParameter::kVariable;
		break;
		case TSpatialStatus::kVyz:
			pvStatus.second = TALSCalcParameter::kVariable;
			pvStatus.third  = TALSCalcParameter::kVariable;
		break;
		case TSpatialStatus::kVxyz:
			pvStatus.first  = TALSCalcParameter::kVariable;
			pvStatus.second = TALSCalcParameter::kVariable;
			pvStatus.third  = TALSCalcParameter::kVariable;
		break;

	}

	return pvStatus;	
}

/////////////////////////////////////////////////////////////////////////////
//END 
/////////////////////////////////////////////////////////////////////////////
