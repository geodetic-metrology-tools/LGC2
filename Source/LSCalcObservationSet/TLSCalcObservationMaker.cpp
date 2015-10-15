////////////////////////////////////////////////////////////////////
// TLSCalcObservationMaker.cpp :Implementation file
// Creates the Least Squares calculation data set from the observations
//
// Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////


#include <list>
using namespace std;

#include "OffsetToSpatialLineROM.h"
#include "OffsetToVerticalLineROM.h"

#include "TRefSystemFactory.h"
#include "TLSCalcObservationMaker.h"
#include "TheodoliteStation.h"
#include "lsalgo/TALSCalcParameter.h"

#include "EDMStation.h"
#include "GyroscopeStation.h"
#include "LevelStation.h"

#include "TLSCalcOffsetToSpatialLineConstraint.h"

#include "TLGCObsLSContributionGenerator.h"
//#include "TPointToPointVector.h"

string intToString(int inInt)
{
    stringstream ss;
    string s;
    ss << inInt;
    s = ss.str();
    return s;
}

// Processes the observations
bool TLSCalcObservationMaker::processData(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	processTheodoliteStations(data, lsobs);
	processEDMStations(data, lsobs);
	processGyroscopeStations(data, lsobs);
	processLevelStations(data, lsobs);

	processROMs(data, lsobs);
	
	processConstraints(data, lsobs);

	return fError == "";
}

void TLSCalcObservationMaker::processConstraints(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	processRadOffCnstrObs(data, lsobs);

	if (data.getOrieCnstrDimension() > 0)
	{
		processOrieCnstrObs(data, lsobs);
	}
}

void TLSCalcObservationMaker::processROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	processOffsetToVerticalPlaneROMs(data, lsobs);
	processOffsetToVerticalLineROMs(data, lsobs);
	processOffsetToSpatialLineROMs(data, lsobs);

	processVerticalDistanceROMs(data, lsobs);
}

// the vertical line is represented as a point on it ("origin") and the vertical vector, parallel to it ("unit")
void TLSCalcObservationMaker::processOffsetToVerticalLineROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
#if 0
	auto offsetToVerticalLineROMs = data.getWorkingROMs()->getOffsetToVerticalLineROMs();
	auto i = offsetToVerticalLineROMs.begin();

	int count = 0;

	while (i != offsetToVerticalLineROMs.end())
	{
		const list<const TOffsetToLineOrPlaneMeasurement*> rom = (*i)->getMeasurements();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator first = rom.begin();

		LSPosVecIter origin;
		LSParaStatus status;
		if ((*i)->getPlombKnownPoint() == NULL)
		{
			status.first = TALSCalcParameter::kVariable;
			status.second = TALSCalcParameter::kVariable;
			status.third = TALSCalcParameter::kFixed;

			TPositionVector firstPointPosition = get3DPosVec((*first)->getTargetPoint());
			
			origin = lsobs.push_back_uniquePosVec(
				TLSCalcPosVectorParam(firstPointPosition, status,
					(*first)->getTargetPoint()->getPtName() + "plomb " + intToString(count++)));
		}
		else
		{
			origin = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*i)->getPlombKnownPoint()),
					convertPVStatus((*i)->getPlombKnownPoint()->getPosition().getObjectStatus()),
					(*i)->getPlombKnownPoint()->getName().getName()));
		}

		status.first = TALSCalcParameter::kFixed;
		status.second = TALSCalcParameter::kFixed;
		status.third = TALSCalcParameter::kFixed;
		
		LSFreeVecIter unit = lsobs.push_back_uniqueFreeVec(
			TLSCalcFreeVectorParam(
				TFreeVector(0, 0, 1, TCoordSysFactory::k3DCartesian),
				status,
				""));

		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator iter = rom.begin();
		while (iter != rom.end())
		{
			LSPosVecIter targetPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));
			
			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getScale()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getScale()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getScale()->getDistanceCorrectionUnknownNumber())));

			lsobs.push_backLSOffsetToVerLine(
				TLSCalcOffsetToVerticalLineObservation(
					*iter, targetPos, origin, origin, unit, lengthCst));

			iter++;
		}

		i++;
	}
#else
	throw std::logic_error("ECVE not implemented yet");
#endif
}

// the spatial line is represented as a point on the line ("ptOnLine") and a unit vector ("unit")
void TLSCalcObservationMaker::processOffsetToSpatialLineROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
#if 0
	auto offsetToSpatialLineROMs = data.getWorkingROMs()->getOffsetToSpatialLineROMs();
	auto i = offsetToSpatialLineROMs.begin();

	while (i != offsetToSpatialLineROMs.end())
	{
		const list<const TOffsetToLineOrPlaneMeasurement*> rom = (*i)->getMeasurements();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator first = rom.begin();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator second = rom.begin();
		second++;

		LSParaStatus status;
		
		TPositionVector firstPointPosition = get3DPosVec((*first)->getTargetPoint());
		TPositionVector secondPointPosition = get3DPosVec((*second)->getTargetPoint());

		LSPosVecIter origin;
		LSPosVecIter ptOnLine;
		LSFreeVecIter unit;
		LSPosVecIter secondPtOnLine;
		if ((*i)->getFirstPointOnLine() == NULL)
		{
			status.first = TALSCalcParameter::kVariable;
			status.second = TALSCalcParameter::kVariable;
			status.third = TALSCalcParameter::kVariable;

			ptOnLine = lsobs.push_back_uniquePosVec(
				TLSCalcPosVectorParam(firstPointPosition, status, ""));

			TFreeVector u = secondPointPosition - firstPointPosition;
			TReal dx = secondPointPosition.getX().getMetresValue() - firstPointPosition.getX().getMetresValue();
			TReal dy = secondPointPosition.getY().getMetresValue() - firstPointPosition.getY().getMetresValue();
			TReal dz = secondPointPosition.getZ().getMetresValue() - firstPointPosition.getZ().getMetresValue();
			TReal len = sqrtq(dx * dx + dy * dy + dz * dz);
			u *= 1 / len;
			unit = lsobs.push_back_uniqueFreeVec(
				TLSCalcFreeVectorParam(u, status, ""));

			status.first = TALSCalcParameter::kFixed;
			status.second = TALSCalcParameter::kFixed;
			status.third = TALSCalcParameter::kFixed;

			origin = lsobs.push_back_uniquePosVec(
				TLSCalcPosVectorParam(firstPointPosition, status, ""));
			
			lsobs.push_backLSOffSpaLineCnstr(
				TLSCalcOffsetToSpatialLineConstraint(
					origin, unit, ptOnLine));

			secondPtOnLine = lsobs.endPV();
		}
		else
		{
			TPositionVector f = get3DPosVec((*i)->getFirstPointOnLine());
			TPositionVector s = get3DPosVec((*i)->getSecondPointOnLine());
			ptOnLine = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					f,
					convertPVStatus((*i)->getFirstPointOnLine()->getPosition().getObjectStatus()),
					(*i)->getFirstPointOnLine()->getName().getName()));
			
			secondPtOnLine = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					s,
					convertPVStatus((*i)->getSecondPointOnLine()->getPosition().getObjectStatus()),
					(*i)->getSecondPointOnLine()->getName().getName()));

			origin = ptOnLine;

			status.first = TALSCalcParameter::kFixed;
			status.second = TALSCalcParameter::kFixed;
			status.third = TALSCalcParameter::kFixed;

			TReal dx = s.getX().getMetresValue() - f.getX().getMetresValue();
			TReal dy = s.getY().getMetresValue() - f.getY().getMetresValue();
			TReal dz = s.getZ().getMetresValue() - f.getZ().getMetresValue();
			TReal len = sqrtq(dx * dx + dy * dy + dz * dz);
			TFreeVector u = s - f;
			u *= 1 / len;
			unit = lsobs.push_back_uniqueFreeVec(TLSCalcFreeVectorParam(u, status, "two points"));
		}

		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator iter = rom.begin();
		while (iter != rom.end())
		{
			LSPosVecIter targetPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));
			
			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getScale()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getScale()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getScale()->getDistanceCorrectionUnknownNumber())));

			lsobs.push_backLSOffsetToSpaLine(
				TLSCalcOffsetToSpatialLineObservation(*iter, targetPos, origin, ptOnLine, unit, lengthCst, secondPtOnLine));

			iter++;
		}

		i++;
	}
#else
	throw std::logic_error("ECSP not implemented yet");
#endif
}

// the plane is represented by its normal ("planeUnit") and distance from a known point ("distanceFromOriginUnknown")
void TLSCalcObservationMaker::processOffsetToVerticalPlaneROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	const list<const OffsetToLineOrPlaneROM*> offsetToVerticalPlaneROMs = data.getWorkingROMs()->getOffsetToVerticalPlaneROMs();
	list<const OffsetToLineOrPlaneROM*>::const_iterator i = offsetToVerticalPlaneROMs.begin();

	int count = 0;

	while (i != offsetToVerticalPlaneROMs.end())
	{
		const list<const TOffsetToLineOrPlaneMeasurement*> rom = (*i)->getMeasurements();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator first = rom.begin();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator second = rom.begin();
		second++;

		TPositionVector firstPointPosition = get3DPosVec((*first)->getTargetPoint());
		TPositionVector secondPointPosition = get3DPosVec((*second)->getTargetPoint());

		TFreeVector secondMinusFirst = secondPointPosition - firstPointPosition;
		TPositionVector originPoint = firstPointPosition + (secondMinusFirst * LITERAL(0.5));

		// finding the plane's normal
		TLength dx = TLength(secondPointPosition.getX().getMetresValue() -
					firstPointPosition.getX().getMetresValue());
		TLength dy = TLength(secondPointPosition.getY().getMetresValue() -
					 firstPointPosition.getY().getMetresValue());

		TReal len = sqrtq(dx.getMetresValue() * dx.getMetresValue() + dy.getMetresValue() * dy.getMetresValue());

		TFreeVector uv(-dy.getMetresValue() / len, dx.getMetresValue() / len, 0, TCoordSysFactory::k3DCartesian);

		LSParaStatus status;
		status.first = TALSCalcParameter::kVariable;
		status.second = TALSCalcParameter::kVariable;
		status.third = TALSCalcParameter::kVariable;
		LSFreeVecIter planeUnit = lsobs.push_back_uniqueFreeVec(
			TLSCalcFreeVectorParam(uv, status, ""));
		
		LSParaStatus status1;
		status1.first = TALSCalcParameter::kFixed;
		status1.second = TALSCalcParameter::kFixed;
		status1.third = TALSCalcParameter::kFixed;
		LSPosVecIter origin = lsobs.push_back_uniquePosVec(
			TLSCalcPosVectorParam(originPoint, status1,
				(*first)->getTargetPoint()->getPtName() + (*second)->getTargetPoint()->getPtName() +
					intToString(count++)));

		LSLengthIter distanceFromOriginUnknown = lsobs.push_back_uniqueLength(
			TLSCalcLengthParam(TLength(0), TALSCalcParameter::kVariable, ""));

		lsobs.push_backLSOffVerPlaneCnstr(TLSCalcOffsetToVerticalPlaneConstraint(planeUnit, origin));

		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator iter = rom.begin();
		while (iter != rom.end())
		{
			LSPosVecIter targetPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));
			
			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getScale()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getScale()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getScale()->getDistanceCorrectionUnknownNumber())));

			lsobs.push_backLSOffsetToVerPlane(
				TLSCalcOffsetToVerPlaneObservation(
					*iter, targetPos, origin, planeUnit, lengthCst, distanceFromOriginUnknown));

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processEDMStations(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	list<EDMStation*> edmStation = data.getWorkingStations()->getEDMStations();
	list<EDMStation*>::iterator i = edmStation.begin();

	while (i != edmStation.end())
	{
		const EDMSpatialDistanceROM* rom = (*i)->getSpatialDistanceROM();

		list<const TSpatialDistMeasurement<EDMTarget>*>::const_iterator iter = rom->getMeasurements().begin();
		
		// Addition of the instrument's height to the list and returns an iterator
		LSLengthIter lengthI = lsobs.push_backLength(
			TLSCalcLengthParam(
				*((*i)->getInstrumentHeight()),
				convertLengthStatus((*i)->getInstrumentHeightVariable() ? TAMeasurement::kVariable : TAMeasurement::kFixed),
				intToString((*i)->getInstrumentHeightUnknownNumber())));
		
		LSPosVecIter stPos = lsobs.push_backPosVec(
			TLSCalcPosVectorParam(
				get3DPosVec((*i)->getStationedPoint()),
				convertPVStatus((*i)->getStationedPoint()->getPosition().getObjectStatus()),
				(*i)->getStationedPoint()->getName().getName()));

		while (iter != rom->getMeasurements().end())
		{
			TLength lengthP = *((*iter)->getTargetHeight());

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getTarget()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getTarget()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getTarget()->getDistanceCorrectionUnknownNumber())));

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			// Addition of the new ls calc spatial dist observation to the list
			lsobs.push_backLSEDMSpaDist(
				TLSCalcSpatialDistObservation<EDMTarget>(*iter,lengthI,lengthP,lengthCst,stPos,tgPos, *i));

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processLevelStations(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	list<LevelStation*> levelStation = data.getWorkingStations()->getLevelStations();
	list<LevelStation*>::iterator i = levelStation.begin();
	
	while (i != levelStation.end())
	{
		const ObservationROM<LevelMeasurement>* rom = (*i)->getVerticalDistanceROM();
		list<const LevelMeasurement*>::const_iterator iter = rom->getMeasurements().begin();
		
		const ObservationROM<THorizontalDistMeas<Staff> >* hdROM = (*i)->getHorizontalDistanceROM();
		list<const THorizontalDistMeas<Staff>*>::const_iterator hdIter = hdROM->getMeasurements().begin();

		LSPosVecIter stPos = lsobs.push_backPosVec(
			TLSCalcPosVectorParam(
				get3DPosVec((*i)->getStationedPoint()),
				convertPVStatus((*i)->getStationedPoint()->getPosition().getObjectStatus()),
				(*i)->getStationedPoint()->getName().getName()));
		
		while (iter != rom->getMeasurements().end())
		{
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			LSLengthIter cte = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getStaff()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getStaff()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getStaff()->getDistanceCorrectionUnknownNumber())));

			lsobs.push_backLSLevelObs(TLSCalcLevelObservation(*iter, stPos, tgPos, cte));

			TReal hdSigma = (*hdIter)->getSigma()->getMetresValue() + ((*hdIter)->getObservedValue()->getKMetresValue() * (*hdIter)->getPPM()->getMetresValue());
			TReal dcSigma = (*hdIter)->getTarget()->getDistanceCorrectionSigma()->getMetresValue();
			TLength sigma(hdSigma * hdSigma + dcSigma * dcSigma);

			lsobs.push_backLSLevelHorDist(TLSCalcHorDistObservation<Staff>(*hdIter, stPos, tgPos, cte, sigma));

			iter++;
			if ((*iter)->hasDistanceMeasurement())
			{
				hdIter++;
			}
		}

		i++;
	}
}

void TLSCalcObservationMaker::processGyroscopeStations(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	list<GyroscopeStation*> gyroStation = data.getWorkingStations()->getGyroscopeStations();
	list<GyroscopeStation*>::iterator i = gyroStation.begin();

	while (i != gyroStation.end())
	{
		const TGyroOrientationROM* rom = (*i)->getGyroscopeROM();
		list<const THorAngleMeasurement*>::const_iterator iter = rom->getMeasurements().begin();
		
		LSPosVecIter stPos = lsobs.push_backPosVec(
			TLSCalcPosVectorParam(
				get3DPosVec((*i)->getStationedPoint()),
				convertPVStatus((*i)->getStationedPoint()->getPosition().getObjectStatus()),
				(*i)->getStationedPoint()->getName().getName()));
		
		TLSCalcOrientationParam angleCst;
		angleCst.setProvisionalValue(TAngle(LITERAL(0.0)),TAngle(LITERAL(0.0)),(*i)->getAngleConstant()->getValue());
		angleCst.setStatus(convertV0Status((*i)->getAngleConstant()->getStatus()));
		LSOrientIter cte = lsobs.push_back_uniqueOrient(angleCst);
		
		while (iter != rom->getMeasurements().end())
		{
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			// Addition of the new ls calc gyro observation to the list 
			lsobs.push_backLSGyroOrie(
				TLSCalcGyroOrientationObservation(*iter, stPos, tgPos, cte, *i));			

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processTheodoliteStations(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	list<TheodoliteStation*> theoStation = data.getWorkingStations()->getTheodoliteStations();
	list<TheodoliteStation*>::const_iterator iter = theoStation.begin();

	while (iter != theoStation.end())
	{
		// Addition of the instrument's height to the list and returns an iterator
		LSLengthIter lengthI = lsobs.push_backLength(
			TLSCalcLengthParam(
				*((*iter)->getInstrumentHeight()),
				convertLengthStatus((*iter)->getInstrumentHeightVariable() ? TAMeasurement::kVariable : TAMeasurement::kFixed),
				intToString((*iter)->getInstrumentHeightUnknownNumber())));
		
		LSPosVecIter stPos = lsobs.push_backPosVec(
			TLSCalcPosVectorParam(
				get3DPosVec((*iter)->getStationedPoint()),
				convertPVStatus((*iter)->getStationedPoint()->getPosition().getObjectStatus()),
				(*iter)->getStationedPoint()->getName().getName()));

		// Addition of the angle measurement cte (if not inserted) 
		TLSCalcOrientationParam orieParam;
		orieParam.setProvisionalValue(TAngle(LITERAL(0.0)),TAngle(LITERAL(0.0)),(*iter)->getInstrument()->getAngleConst()->getValue());
		orieParam.setStatus(convertV0Status((*iter)->getInstrument()->getAngleConst()->getStatus()));
		LSOrientIter cte = lsobs.push_back_uniqueOrient(orieParam);

		processTheoStationROMs(lsobs, *iter, lengthI, stPos, cte);
		iter++;
	}
}

void TLSCalcObservationMaker::processTheoStationROMs(LSCalcDataSet& lsobs, const TheodoliteStation* theoStation, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const LSOrientIter& cte)
{
	list<TheodoliteStationROM*>::const_iterator iter = theoStation->getTheodoliteStationROMs().begin();

	while (iter != theoStation->getTheodoliteStationROMs().end())
	{
		Angles v0;
		v0.kappa = TAngle(0);
		v0.omega = TAngle(0);
		v0.phi = TAngle(0);

		bool hasV0 = (*iter)->getHorizontalAngleROMs().size() > 0 ||
			(*iter)->getOffsetToTheodolitePlaneROMs().size() > 0 ||
			(*iter)->getPolarROMs().size() > 0;

		LSParaStatus status;
		if (hasV0)
		{
			if (theoStation->getRot3D())
			{
				status.first = TALSCalcParameter::kVariable;
				status.second = TALSCalcParameter::kVariable;
			}
			else
			{
				status.first = TALSCalcParameter::kFixed;
				status.second = TALSCalcParameter::kFixed;
			}
			status.third = TALSCalcParameter::kVariable;
		}
		else
		{
			status.first = TALSCalcParameter::kFixed;
			status.second = TALSCalcParameter::kFixed;
			status.third = TALSCalcParameter::kFixed;
		}

		LSOrientIter orientation = lsobs.push_back_uniqueOrient(
			TLSCalcOrientationParam(v0,	status, ""));

		list<const HorizontalAngleROM*> haROMs = (*iter)->getHorizontalAngleROMs();
		processHAROMs(lsobs, haROMs, orientation, stPos, cte, theoStation);
		
		list<const ZenithDistanceROM*> zdROMs = (*iter)->getZenithDistanceROMs();
		processZDROMs(lsobs, zdROMs, lengthI, stPos, theoStation);
		
		list<const SpatialDistanceROM*> sdROMs = (*iter)->getSpatialDistanceROMs();
		processSDROMs(lsobs, sdROMs, lengthI, stPos, theoStation);
		
		list<const HorizontalDistanceROM*> hdROMs = (*iter)->getHorizontalDistanceROMs();
		processHDROMs(lsobs, hdROMs, stPos, theoStation);
		
		list<const OffsetToTheodolitePlaneROM*> otpROMs = (*iter)->getOffsetToTheodolitePlaneROMs();
		processOTPROMs(lsobs, otpROMs, orientation, stPos, theoStation);
		
		list<const PolarROM*> p3dROMs = (*iter)->getPolarROMs();
		processP3DROMs(lsobs, p3dROMs, orientation, lengthI, stPos, cte, theoStation);

		iter++;
	}
}

void TLSCalcObservationMaker::processHAROMs(LSCalcDataSet& lsobs, const list<const HorizontalAngleROM*>& roms, const LSOrientIter& orientation, const LSPosVecIter& stPos, const LSOrientIter& cte, const TheodoliteStation* theoStation)
{
	list<const HorizontalAngleROM*>::const_iterator i = roms.begin();

	while (i != roms.end())
	{
		list<const THorAngleMeasurement*>::const_iterator iter = (*i)->getMeasurements().begin();

		while (iter != (*i)->getMeasurements().end())
		{
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			// Addition of the new ls calc hor ang observation to the list 
			LSHorAngIter lsHaIt = lsobs.push_backLSHorAng(
				TLSCalcHorAngleObservation(*iter, orientation, stPos, tgPos, cte, theoStation));			

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processZDROMs(LSCalcDataSet& lsobs, const list<const ZenithDistanceROM*>& roms, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const TheodoliteStation* theoStation)
{
	list<const ZenithDistanceROM*>::const_iterator i = roms.begin();

	while (i != roms.end())
	{
		list<const TZenithDistMeasurement*>::const_iterator iter = (*i)->getMeasurements().begin();

		while (iter != (*i)->getMeasurements().end())
		{
			TLength lengthP = *((*iter)->getTargetHeight());

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			// Addition of the new ls calc zen dist observation to the list
			LSZenDistIter lsZdIt = lsobs.push_backLSZenDist(
				TLSCalcZenithDistObservation(*iter,lengthI,lengthP,stPos,tgPos, theoStation));

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processSDROMs(LSCalcDataSet& lsobs, const list<const SpatialDistanceROM*>& roms, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const TheodoliteStation* theoStation)
{
	list<const SpatialDistanceROM*>::const_iterator i = roms.begin();

	while (i != roms.end())
	{
		list<const TSpatialDistMeasurement<TheodoliteTarget>*>::const_iterator iter = (*i)->getMeasurements().begin();

		while (iter != (*i)->getMeasurements().end())
		{
			TLength lengthP = *((*iter)->getTargetHeight());

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getTarget()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getTarget()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getTarget()->getDistanceCorrectionUnknownNumber())));

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			// Addition of the new ls calc spatial dist observation to the list
			LSSpaDistIter lsSdIt = lsobs.push_backLSSpaDist(
				TLSCalcSpatialDistObservation<TheodoliteTarget>(*iter,lengthI,lengthP,lengthCst,stPos,tgPos, theoStation));

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processHDROMs(LSCalcDataSet& lsobs, const list<const HorizontalDistanceROM*>& roms, const LSPosVecIter& stPos, const TheodoliteStation* theoStation)
{
	list<const HorizontalDistanceROM*>::const_iterator i = roms.begin();

	const TPositionVector& st = stPos->getEstimatedValue();

	while (i != roms.end())
	{
		list<const THorizontalDistMeas<TheodoliteTarget>*>::const_iterator iter = (*i)->getMeasurements().begin();

		while (iter != (*i)->getMeasurements().end())
		{
			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getTarget()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getTarget()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getTarget()->getDistanceCorrectionUnknownNumber())));

			// Addition of the target position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			TReal hdSigma = (*iter)->getSigma()->getMetresValue() + ((*iter)->getObservedValue()->getKMetresValue() * (*iter)->getPPM()->getMetresValue());
			TReal tcSigma = (*iter)->getTargetCenteringSigma()->getMetresValue();
			TReal icSigma = theoStation->getInstrumentCenteringSigma()->getMetresValue();
			TReal dcSigma = (*iter)->getTarget()->getDistanceCorrectionSigma()->getMetresValue();
			TLength sigma(sqrtq(hdSigma * hdSigma + tcSigma * tcSigma + icSigma * icSigma + dcSigma * dcSigma));

			// Addition of the new ls calc hor dist observation to the list
			LSHorDistIter lsHdIt = lsobs.push_backLSHorDist(
				TLSCalcHorDistObservation<TheodoliteTarget>(*iter,stPos,tgPos,lengthCst, sigma));

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processVerticalDistanceROMs(const TLGCDataSet& data,LSCalcDataSet& lsobs)
{
	const list<const VerticalDistanceROM*> verticalDistanceROMs = data.getWorkingROMs()->getVerticalDistanceROMs();
	list<const VerticalDistanceROM*>::const_iterator i = verticalDistanceROMs.begin();

	while (i != verticalDistanceROMs.end())
	{
		const list<const TVerticalDistMeasurement*> rom = (*i)->getMeasurements();
		list<const TVerticalDistMeasurement*>::const_iterator iter = rom.begin();

		while (iter != rom.end())
		{
			LSPosVecIter tg1Pos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));
			
			LSPosVecIter tg2Pos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getSecondTargetPoint()),
					convertPVStatus((*iter)->getSecondTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getSecondTargetPoint()->getName().getName()));

			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getFirstStaff()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getFirstStaff()->getDistanceCorrection()->getStatus()),
					(*iter)->getFirstStaff()->getTargetID()));

			LSVertDistIter lsVdIt = lsobs.push_backLSVertDist(
				TLSCalcVertDistObservation(*iter, tg1Pos, tg2Pos,lengthCst));

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processOTPROMs(LSCalcDataSet& lsobs, const list<const OffsetToTheodolitePlaneROM*>& roms, const LSOrientIter& orientation, const LSPosVecIter& stPos, const TheodoliteStation* theoStation)
{
	list<const OffsetToTheodolitePlaneROM*>::const_iterator i = roms.begin();

	while (i != roms.end())
	{
		list<const TOffsetToTheoPlaneMeasurement*>::const_iterator iter = (*i)->getMeasurements().begin();

		while (iter != (*i)->getMeasurements().end())
		{
			// Addition of the first target point position to the list ( if not already inserted ) and returns an iterator
			LSPosVecIter theoPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));

			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getScale()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getScale()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getScale()->getDistanceCorrectionUnknownNumber())));

			// Addition of the new ls calc offset observation to the list
			LSOffsetToTheoPlaneIter lsOffsetIt = lsobs.push_backLSOffsetToTheoPlane(
				TLSCalcOffsetToTheoPlaneObservation(*iter,theoPos,stPos, orientation, lengthCst, theoStation));

			iter++;
		}

		i++;
	}
}

void TLSCalcObservationMaker::processP3DROMs(LSCalcDataSet& lsobs, const list<const PolarROM*>& roms, const LSOrientIter& orientation, const LSLengthIter& lengthI, const LSPosVecIter& stPos, const LSOrientIter& cte, const TheodoliteStation* theoStation)
{
	list<const PolarROM*>::const_iterator i = roms.begin();

	while (i != roms.end())
	{
		list<const PolarMeasurement*>::const_iterator iter = (*i)->getMeasurements().begin();

		while (iter != (*i)->getMeasurements().end())
		{
			LSPosVecIter tgPos = lsobs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec((*iter)->getTargetPoint()),
					convertPVStatus((*iter)->getTargetPoint()->getPosition().getObjectStatus()),
					(*iter)->getTargetPoint()->getName().getName()));
			
			// Addition of the instrument's constant to the list (if not already inserted) and returns an iterator
			LSLengthIter lengthCst = lsobs.push_backLength(
				TLSCalcLengthParam(
					(*iter)->getTarget()->getDistanceCorrection()->getValue(),
					convertLengthStatus((*iter)->getTarget()->getDistanceCorrection()->getStatus()),
					intToString((*iter)->getTarget()->getDistanceCorrectionUnknownNumber())));

			lsobs.push_backLSPolar(TLSCalcPolarObservation(
				*iter, orientation, stPos, tgPos, cte, lengthI, *((*iter)->getTargetHeight()), lengthCst, theoStation));

			iter++;
		}

		i++;
	}
}

TPositionVector	TLSCalcObservationMaker::get3DPosVec(const TSpatialPoint* pt) const
{
	TPositionVector posVec(pt->getPosition().getCoordinates(TCoordSysFactory::k3DCartesian));
	if (posVec.getCoordSys() != TCoordSysFactory::k3DCartesian)
	{
		TSpatialPosition* spos = new TSpatialPosition(pt->getPosition());
		spos->transform(TRefSystemFactory::getRefSystemFactory()->getRefFrame(TRefSystemFactory::kCCS));
		posVec = spos->getCoordinates(TCoordSysFactory::k3DCartesian);
		delete spos;
	}
	return posVec;
}

//////////////////////////////////////////////////////////////////////////////////
//FUNCTION CONVERT
//////////////////////////////////////////////////////////////////////////////////

// Converts length's status to ls calc length's status 
TALSCalcParameter::ELSStatus	TLSCalcObservationMaker::convertLengthStatus(TAMeasurement::ECalcStatus ecs) const {

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
struct LSParaStatus		TLSCalcObservationMaker::convertV0Status(TAMeasurement::ECalcStatus ecs) const {

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
struct LSParaStatus	TLSCalcObservationMaker::convertPVStatus(TSpatialStatus::ESpatialStatus ess) const {

	// Position vector's struct status
	struct LSParaStatus pvStatus;

	// tests if the input status is not set to kPosNull or kUnknown
	if (( ess == TSpatialStatus::kPosNull ) || ( ess == TSpatialStatus::kUnknown )) {
		//fError = "the status of the position is wrongly set\n";
		pvStatus.first = TALSCalcParameter::kNull;
		pvStatus.second = TALSCalcParameter::kNull;
		pvStatus.third = TALSCalcParameter::kNull;
	}

	// Preparation of stationed position's status
	else if ( ess == TSpatialStatus::kCala ) {
		pvStatus.first = TALSCalcParameter::kFixed;
		pvStatus.second = TALSCalcParameter::kFixed;
		pvStatus.third = TALSCalcParameter::kFixed;	}
	else if ( ess == TSpatialStatus::kVx ) {
		pvStatus.first = TALSCalcParameter::kVariable;
		pvStatus.second = TALSCalcParameter::kFixed;
		pvStatus.third = TALSCalcParameter::kFixed; }
	else if ( ess == TSpatialStatus::kVy ) {
		pvStatus.first = TALSCalcParameter::kFixed;
		pvStatus.second = TALSCalcParameter::kVariable;
		pvStatus.third = TALSCalcParameter::kFixed;	}
	else if ( ess == TSpatialStatus::kVz ) {
		pvStatus.first = TALSCalcParameter::kFixed;
		pvStatus.second = TALSCalcParameter::kFixed;
		pvStatus.third = TALSCalcParameter::kVariable;	}
	else if ( ess == TSpatialStatus::kVxy ) {
		pvStatus.first = TALSCalcParameter::kVariable;
		pvStatus.second = TALSCalcParameter::kVariable;
		pvStatus.third = TALSCalcParameter::kFixed;	}
	else if ( ess == TSpatialStatus::kVxz ) {
		pvStatus.first = TALSCalcParameter::kVariable;
		pvStatus.second = TALSCalcParameter::kFixed;
		pvStatus.third = TALSCalcParameter::kVariable;	}
	else if ( ess == TSpatialStatus::kVyz ) {
		pvStatus.first = TALSCalcParameter::kFixed;
		pvStatus.second = TALSCalcParameter::kVariable;
		pvStatus.third = TALSCalcParameter::kVariable;	}
	else { // Vxyz
		pvStatus.first = TALSCalcParameter::kVariable;
		pvStatus.second = TALSCalcParameter::kVariable;
		pvStatus.third = TALSCalcParameter::kVariable;	}

	return pvStatus;	
}

void	TLSCalcObservationMaker::processRadOffCnstrObs(const TLGCDataSet& data,LSCalcDataSet& lsObs)
{/* Processing of offset (ECTH) observations leading to the creation of
LSCalc offset to theodolite plane observations and associated parameters*/


	RadOffCnstrIterator iterB = data.getWorkingConstraints()->getRadOffCnstrBeginIterator();
	RadOffCnstrIterator iterE = data.getWorkingConstraints()->getRadOffCnstrEndIterator();

	while (iterB!=iterE)
	{
		TSpatialPoint* point = &*(data.getWorkingPoints()->getPoint(iterB->getPoint()));

		// Addition of the point position to the list ( if not already inserted )	
		LSPosVecIter stPos = lsObs.push_backPosVec(
			TLSCalcPosVectorParam(
				get3DPosVec(point),
				convertPVStatus(point->getPosition().getObjectStatus()),
				point->getName().getName()));

		// Addition of the new ls calc offset observation to the list
		LSRadOffCnstrIter lsOffsetIt = lsObs.push_backLSRadOffCnstr(TLSCalcRadialOffsetCnstrObservation(iterB,stPos));

		iterB++;
	}
}


void	TLSCalcObservationMaker::processOrieCnstrObs(const TLGCDataSet& data,LSCalcDataSet& lsObs)
{/* Processing of offset (ECTH) observations leading to the creation of
LSCalc offset to theodolite plane observations and associated parameters*/


	OrieCnstrIterator iterB = data.getWorkingConstraints()->getOrieCnstrBeginIterator();
	OrieCnstrIterator iterE = data.getWorkingConstraints()->getOrieCnstrEndIterator();
	
	PointConstIter firstPt = data.getWorkingPoints()->getPointsBeginIterator();
	PointConstIter endPoint = data.getWorkingPoints()->getPointsEndIterator();
	PointConstIter fixedPtIt;
	bool find = true;

	//search the first CALA point
	while(firstPt != endPoint && find == true)
	{
		if(firstPt->getPosition().getObjectStatus() == TSpatialStatus::kCala)
		{
			fixedPtIt = firstPt;
			find = false;
		}
		firstPt++;
	}

	//else search the first VXY point
	if(find == true)
	{
		//initialise first point for a second loop
		firstPt = data.getWorkingPoints()->getPointsBeginIterator();

		while(firstPt != endPoint && find == true)
		{
			if(firstPt->getPosition().getObjectStatus() == TSpatialStatus::kVxy)
			{
				fixedPtIt = firstPt;
				find = false;
			}
			firstPt++;
		}
	}

	//else error
	if(find == true)
	{
		fError += "Orientation Constraint not defined, no 'CALA' or 'VXY' Point defined ";
	}
	else
	{
		while (iterB!=iterE)
		{
			TSpatialPoint* point = &*(data.getWorkingPoints()->getPoint(iterB->getPoint()));

			// Addition of the point position to the list ( if not already inserted )
			LSPosVecIter fixedPos = lsObs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec(&*fixedPtIt),
					convertPVStatus(fixedPtIt->getPosition().getObjectStatus()),
					fixedPtIt->getName().getName()));

			// Addition of the point position to the list ( if not already inserted )	
			LSPosVecIter refPos = lsObs.push_backPosVec(
				TLSCalcPosVectorParam(
					get3DPosVec(point),
					convertPVStatus(point->getPosition().getObjectStatus()),
					point->getName().getName()));

			// Addition of the new ls calc offset observation to the list
			LSOrieCnstrIter lsOffsetIt = lsObs.push_backLSOrieCnstr(
				TLSCalcOrientationCnstrObservation(iterB, fixedPos, refPos));


			iterB++;
		}
	}
}
