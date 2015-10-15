//TLSCalcPolarObservation.cpp

#include "TLSCalcPolarObservation.h"


TLSCalcPolarObservation::TLSCalcPolarObservation(const PolarMeasurement* pMeas,LSOrientIter v0,
			LSPosVecIter st,LSPosVecIter tg,LSOrientIter cte, LSLengthIter hi,TLength hp,
			LSLengthIter cst, const TheodoliteStation* theoStation) :
	V0(v0),	station(st), target(tg), constant(cte), instrumentHeight(hi), targetHeight(hp), lengthConstant(cst)
{
	tStation = theoStation;

	horizontalAngleMeasurement = new THorAngleMeasurement(pMeas->getObservationID(), pMeas->getTargetPoint(),
		pMeas->getTarget(), new TAngle(*(pMeas->getAngleValue())), new TAngle(*(pMeas->getAngleSigma())), new TLength(*(pMeas->getTargetCenteringSigma())));
	zenithDistanceMeasurement = new TZenithDistMeasurement(pMeas->getObservationID(), pMeas->getTargetPoint(),
		pMeas->getTarget(), new TAngle(*(pMeas->getZenithDistanceValue())), new TAngle(*(pMeas->getZenithDistanceSigma())),
		new TLength(*(pMeas->getTargetHeight())), new TLength(*(pMeas->getTargetHeightSigma())), new TLength(*(pMeas->getTargetCenteringSigma())));
	spatialDistanceMeasurement = new TSpatialDistMeasurement<TheodoliteTarget>(pMeas->getObservationID(),
		pMeas->getTargetPoint(),
		pMeas->getTarget(), new TLength(*(pMeas->getSpatialDistanceValue())), new TLength(*(pMeas->getSpatialDistanceSigma())), new TLength(*(pMeas->getPPM())),
		new TLength(*(pMeas->getTargetHeight())), new TLength(*(pMeas->getTargetHeightSigma())), new TLength(*(pMeas->getTargetCenteringSigma())));

	horizontalAngleObservation = new TLSCalcHorAngleObservation(horizontalAngleMeasurement, v0, st, tg, cte, theoStation);
	zenithDistanceObservation = new TLSCalcZenithDistObservation(zenithDistanceMeasurement, hi, hp, st, tg, theoStation);
	spatialDistanceObservation = new TLSCalcSpatialDistObservation<TheodoliteTarget>(spatialDistanceMeasurement, hi, hp, cst, st, tg, theoStation);
}

TLSCalcPolarObservation::TLSCalcPolarObservation(const TLSCalcPolarObservation& other) :
	V0(other.V0), station(other.station), target(other.target), constant(other.constant),
	instrumentHeight(other.instrumentHeight), targetHeight(other.targetHeight), lengthConstant(other.lengthConstant)	
{
	horizontalAngleMeasurement = new THorAngleMeasurement(other.horizontalAngleMeasurement->getObservationID(),
		other.horizontalAngleMeasurement->getTargetPoint(), other.horizontalAngleMeasurement->getTarget(),
		new TAngle(*other.horizontalAngleMeasurement->getObservedValue()), new TAngle(*other.horizontalAngleMeasurement->getSigma()),
		new TLength(*other.horizontalAngleMeasurement->getTargetCenteringSigma()));
	zenithDistanceMeasurement = new TZenithDistMeasurement(other.zenithDistanceMeasurement->getObservationID(),
		other.zenithDistanceMeasurement->getTargetPoint(), other.zenithDistanceMeasurement->getTarget(),
		new TAngle(*other.zenithDistanceMeasurement->getObservedValue()), new TAngle(*other.zenithDistanceMeasurement->getSigma()),
		new TLength(*other.zenithDistanceMeasurement->getTargetHeight()), new TLength(*other.zenithDistanceMeasurement->getTargetHeightSigma()),
		new TLength(*other.zenithDistanceMeasurement->getTargetCenteringSigma()));
	spatialDistanceMeasurement = new TSpatialDistMeasurement<TheodoliteTarget>(other.spatialDistanceMeasurement->getObservationID(),
		other.spatialDistanceMeasurement->getTargetPoint(),
		other.spatialDistanceMeasurement->getTarget(), new TLength(*(other.spatialDistanceMeasurement->getObservedValue())),
		new TLength(*(other.spatialDistanceMeasurement->getSigma())), new TLength(*(other.spatialDistanceMeasurement->getPPM())),
		new TLength(*(other.spatialDistanceMeasurement->getTargetHeight())), new TLength(*(other.spatialDistanceMeasurement->getTargetHeightSigma())), new TLength(*(other.spatialDistanceMeasurement->getTargetCenteringSigma())));

	tStation = other.tStation;

	horizontalAngleObservation = new TLSCalcHorAngleObservation(horizontalAngleMeasurement, V0, station, target, constant, tStation);
	zenithDistanceObservation = new TLSCalcZenithDistObservation(zenithDistanceMeasurement, instrumentHeight, targetHeight, station, target, tStation);
	spatialDistanceObservation = new TLSCalcSpatialDistObservation<TheodoliteTarget>(spatialDistanceMeasurement, instrumentHeight, targetHeight, lengthConstant, station, target, tStation);
}

TLSCalcPolarObservation TLSCalcPolarObservation::operator= (const TLSCalcPolarObservation& other)
{
	return TLSCalcPolarObservation(other);
}

TLSCalcPolarObservation::~TLSCalcPolarObservation()
{
	if (horizontalAngleObservation != NULL)
	{
		delete horizontalAngleObservation;
		delete zenithDistanceObservation;
		delete spatialDistanceObservation;
	}
}
