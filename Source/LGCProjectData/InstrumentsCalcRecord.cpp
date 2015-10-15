// InstrumentsCalcRecord.cpp
//
//
// Patterns:
//
// 
// Copyright 2007-2008 CERN TS/SU. All rights reserved. 
// Author M. Jones
//////////////////////////////////////////////////////////////////////

#include "OffsetToVerticalLineROM.h"
#include "InstrumentsCalcRecord.h"










#include <list>
//using namespace std;

#include "TOffsetToLineOrPlaneMeasurement.h"

class OffsetToSpatialLineROM
{ 
public:

	OffsetToSpatialLineROM(const TSpatialPoint* firstPointOnLine, const TSpatialPoint* secondPointOnLine);

	~OffsetToSpatialLineROM()
	{
		list<const TOffsetToLineOrPlaneMeasurement*>::iterator iter = offsetToSpatialLineMeasurements.begin();

		while (iter != offsetToSpatialLineMeasurements.end())
		{
			delete *iter;
			iter++;
		}
	}

	const TSpatialPoint* getFirstPointOnLine() const { return firstPointOnLine; }
	const TSpatialPoint* getSecondPointOnLine() const { return secondPointOnLine; }

	const list<const TOffsetToLineOrPlaneMeasurement*>& getMeasurements() const { return offsetToSpatialLineMeasurements; }

	void addMeasurement(const TOffsetToLineOrPlaneMeasurement* meas) { offsetToSpatialLineMeasurements.push_back(meas); }

private:

	const TSpatialPoint* firstPointOnLine;
	const TSpatialPoint* secondPointOnLine;
	list<const TOffsetToLineOrPlaneMeasurement*> offsetToSpatialLineMeasurements;

};











InstrumentsCalcRecord::InstrumentsCalcRecord(LSCalcDataSet* calcDataSet, TLGCDataSet* dataSet)
{
	// Theodolite observations
	list<TheodoliteStation*> theos = dataSet->getWorkingStations()->getTheodoliteStations();
	list<TheodoliteStation*>::iterator theoIter = theos.begin();

	while (theoIter != theos.end())
	{
		string name = (*theoIter)->getInstrument()->getInstrumentID();
		hash_map<string, TLGCObsSummary*>* obsSummaries;
		hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator temp = theodolitesMap.find(name);
		if (temp == theodolitesMap.end())
		{
			obsSummaries = new hash_map<string, TLGCObsSummary*>();
			theodolitesMap[name] = obsSummaries;
		}
		else
		{
			obsSummaries = temp->second;
		}

		list<TheodoliteStationROM*> roms = (*theoIter)->getTheodoliteStationROMs();
		list<TheodoliteStationROM*>::iterator iter = roms.begin();

		while (iter != roms.end())
		{
			// Horizontal Angles
			list <const HorizontalAngleROM*> haROMs = (*iter)->getHorizontalAngleROMs();
			list <const HorizontalAngleROM*>::iterator haROMIter = haROMs.begin();

			TLGCObsSummary* summary;
			if (haROMs.size() != 0)
			{
				hash_map<string, TLGCObsSummary*>::iterator angl = obsSummaries->find("ANGL");

				if (angl == obsSummaries->end())
				{
					summary = new TLGCObsSummary();
					(*obsSummaries)["ANGL"] = summary;
					summary->defineAngleObservation();
				}
				else
				{
					summary = angl->second;
				}
				
				while (haROMIter != haROMs.end())
				{
					list<const THorAngleMeasurement*> meas = (*haROMIter)->getMeasurements();
					list<const THorAngleMeasurement*>::iterator i = meas.begin();

					while (i != meas.end())
					{
						LSHorAngIter measurement = calcDataSet->getCalcHorAngByID((*i)->getObservationID());

						summary->addNewResidual(measurement->getResidue().getSignedCCValue());

						i++;
					}

					haROMIter++;
				}
			}
			
			// Zenith Distances
			list <const ZenithDistanceROM*> zdROMs = (*iter)->getZenithDistanceROMs();
			list <const ZenithDistanceROM*>::iterator zdROMIter = zdROMs.begin();

			if (zdROMs.size() != 0)
			{
				string keyword;
				if ((*theoIter)->getInstrumentHeightVariable())
				{
					keyword = "ZENI";
				}
				else
				{
					keyword = "ZENH";
				}

				hash_map<string, TLGCObsSummary*>::iterator zend = obsSummaries->find(keyword);

				if (zend == obsSummaries->end())
				{
					summary = new TLGCObsSummary();
					(*obsSummaries)[keyword] = summary;
					summary->defineAngleObservation();
				}
				else
				{
					summary = zend->second;
				}
				
				while (zdROMIter != zdROMs.end())
				{
					list<const TZenithDistMeasurement*> meas = (*zdROMIter)->getMeasurements();
					list<const TZenithDistMeasurement*>::iterator i = meas.begin();

					while (i != meas.end())
					{
						LSZenDistIter measurement = calcDataSet->getCalcZenDistByID((*i)->getObservationID());

						summary->addNewResidual(measurement->getResidue().getSignedCCValue());

						i++;
					}

					zdROMIter++;
				}
			}
			
			// Spatial Distances
			list <const SpatialDistanceROM*> sdROMs = (*iter)->getSpatialDistanceROMs();
			list <const SpatialDistanceROM*>::iterator sdROMIter = sdROMs.begin();

			if (sdROMs.size() != 0)
			{
				string keyword;
				if ((*theoIter)->getInstrumentHeightVariable())
				{
					keyword = "DTHE";
				}
				else
				{
					keyword = "DMES";
				}

				hash_map<string, TLGCObsSummary*>::iterator drdl = obsSummaries->find(keyword);

				if (drdl == obsSummaries->end())
				{
					summary = new TLGCObsSummary();
					(*obsSummaries)[keyword] = summary;
					summary->defineLengthObservation();
				}
				else
				{
					summary = drdl->second;
				}
				
				while (sdROMIter != sdROMs.end())
				{
					list<const TSpatialDistMeasurement<TheodoliteTarget>*> meas = (*sdROMIter)->getMeasurements();
					list<const TSpatialDistMeasurement<TheodoliteTarget>*>::iterator i = meas.begin();

					while (i != meas.end())
					{
						LSSpaDistIter measurement = calcDataSet->getCalcSpaDistByID((*i)->getObservationID());

						summary->addNewResidual(measurement->getResidue().getMMetresValue());

						i++;
					}

					sdROMIter++;
				}
			}
			
			// Horizontal Distances
			list <const HorizontalDistanceROM*> hdROMs = (*iter)->getHorizontalDistanceROMs();
			list <const HorizontalDistanceROM*>::iterator hdROMIter = hdROMs.begin();

			if (hdROMs.size() != 0)
			{
				hash_map<string, TLGCObsSummary*>::iterator dhor = obsSummaries->find("DHOR");

				if (dhor == obsSummaries->end())
				{
					summary = new TLGCObsSummary();
					(*obsSummaries)["DHOR"] = summary;
					summary->defineLengthObservation();
				}
				else
				{
					summary = dhor->second;
				}
				
				while (hdROMIter != hdROMs.end())
				{
					list<const THorizontalDistMeas<TheodoliteTarget>*> meas = (*hdROMIter)->getMeasurements();
					list<const THorizontalDistMeas<TheodoliteTarget>*>::iterator i = meas.begin();

					while (i != meas.end())
					{
						LSHorDistIter measurement = calcDataSet->getCalcHorDistByID((*i)->getObservationID());

						summary->addNewResidual(measurement->getResidue().getMMetresValue());

						i++;
					}

					hdROMIter++;
				}
			}
			
			// Offset To Theo Planes
			list <const OffsetToTheodolitePlaneROM*> otpROMs = (*iter)->getOffsetToTheodolitePlaneROMs();
			list <const OffsetToTheodolitePlaneROM*>::iterator otpROMIter = otpROMs.begin();

			if (otpROMs.size() != 0)
			{				
				while (otpROMIter != otpROMs.end())
				{
					list<const TOffsetToTheoPlaneMeasurement*> meas = (*otpROMIter)->getMeasurements();
					list<const TOffsetToTheoPlaneMeasurement*>::iterator i = meas.begin();

					while (i != meas.end())
					{
						string scaleID = (*i)->getScale()->getInstrumentID();

						hash_map<string, TLGCObsSummary*>* scaleObsSummaries;
						temp = scalesMap.find(scaleID);
						if (temp == scalesMap.end())
						{
							scaleObsSummaries = new hash_map<string, TLGCObsSummary*>();
							scalesMap[scaleID] = scaleObsSummaries;
						}
						else
						{
							scaleObsSummaries = temp->second;
						}

						hash_map<string, TLGCObsSummary*>::iterator ecth = scaleObsSummaries->find("ECTH");

						TLGCObsSummary* scaleSummary;
						if (ecth == scaleObsSummaries->end())
						{
							scaleSummary = new TLGCObsSummary();
							(*scaleObsSummaries)["ECTH"] = scaleSummary;
							scaleSummary->defineLengthObservation();
						}
						else
						{
							scaleSummary = ecth->second;
						}

						LSOffsetToTheoPlaneIter measurement = calcDataSet->getOffTheoPlaneByID((*i)->getObservationID());

						scaleSummary->addNewResidual(measurement->getResidue().getMMetresValue());

						i++;
					}

					otpROMIter++;
				}
			}
			
			// Polar 3D
			list <const PolarROM*> p3dROMs = (*iter)->getPolarROMs();
			list <const PolarROM*>::iterator p3dROMIter = p3dROMs.begin();

			if (p3dROMs.size() != 0)
			{
				if (obsSummaries->find("ANGL") == obsSummaries->end())
				{
					(*obsSummaries)["ANGL"] = new TLGCObsSummary();
					(*obsSummaries)["ANGL"]->defineAngleObservation();
				}				
				if (obsSummaries->find("ZENH") == obsSummaries->end())
				{
                    (*obsSummaries)["ZENH"] = new TLGCObsSummary();
                    (*obsSummaries)["ZENH"]->defineAngleObservation();
                }
				if (obsSummaries->find("DMES") == obsSummaries->end())
				{
                    (*obsSummaries)["DMES"] = new TLGCObsSummary();
                    (*obsSummaries)["DMES"]->defineLengthObservation();
                }
				
				while (p3dROMIter != p3dROMs.end())
				{
					list<const PolarMeasurement*> meas = (*p3dROMIter)->getMeasurements();
					list<const PolarMeasurement*>::iterator i = meas.begin();

					while (i != meas.end())
					{
						LSPolarIter polar = calcDataSet->getCalcPolarByID((*i)->getObservationID());

						(*obsSummaries)["ANGL"]->addNewResidual(polar->getHorizontalAngleObservation()->getResidue().getSignedCCValue());
						(*obsSummaries)["ZENH"]->addNewResidual(polar->getZenithDistanceObservation()->getResidue().getSignedCCValue());
						(*obsSummaries)["DMES"]->addNewResidual(polar->getSpatialDistanceObservation()->getResidue().getMMetresValue());

						i++;
					}

					p3dROMIter++;
				}
			}

			iter++;
		}

		theoIter++;
	}

	// EDM Distances
	list<EDMStation*> edms = dataSet->getWorkingStations()->getEDMStations();
	list<EDMStation*>::iterator edmIter = edms.begin();

	while (edmIter != edms.end())
	{
		string name = (*edmIter)->getInstrument()->getInstrumentID();

		hash_map<string, TLGCObsSummary*>* obsSummaries;
		hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator temp = edmsMap.find(name);
		if (temp == edmsMap.end())
		{
			obsSummaries = new hash_map<string, TLGCObsSummary*>();
			edmsMap[name] = obsSummaries;
		}
		else
		{
			obsSummaries = temp->second;
		}
		
		string keyword;
		if ((*edmIter)->getInstrumentHeightVariable())
		{
			keyword = "DTHE";
		}
		else
		{
			keyword = "DMES";
		}

		hash_map<string, TLGCObsSummary*>::iterator dspt = obsSummaries->find(keyword);

		TLGCObsSummary* summary;
		if (dspt == obsSummaries->end())
		{
			summary = new TLGCObsSummary();
			(*obsSummaries)[keyword] = summary;
			summary->defineLengthObservation();
		}
		else
		{
			summary = dspt->second;
		}

		list<const TSpatialDistMeasurement<EDMTarget>*> meas = (*edmIter)->getSpatialDistanceROM()->getMeasurements();
		list<const TSpatialDistMeasurement<EDMTarget>*>::iterator i = meas.begin();

		while (i != meas.end())
		{			
			LSEDMSpaDistIter measurement = calcDataSet->getCalcEDMSpaDistByID((*i)->getObservationID());

			summary->addNewResidual(measurement->getResidue().getMMetresValue());

			i++;
		}

		edmIter++;
	}

	// Offset To Vertical Line
	auto l = dataSet->getWorkingROMs()->getOffsetToVerticalLineROMs();
	auto iter = l.begin();

	while (iter != l.end())
	{
		list<const TOffsetToLineOrPlaneMeasurement*> meass = (*iter)->getMeasurements();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator obsIt = meass.begin();

		while(obsIt != meass.end())
		{			
			string name = (*obsIt)->getScale()->getInstrumentID();

			hash_map<string, TLGCObsSummary*>* obsSummaries;
			hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator temp = scalesMap.find(name);
			if (temp == scalesMap.end())
			{
				obsSummaries = new hash_map<string, TLGCObsSummary*>();
				scalesMap[name] = obsSummaries;
			}
			else
			{
				obsSummaries = temp->second;
			}

			hash_map<string, TLGCObsSummary*>::iterator ecve = obsSummaries->find("ECVE");

			TLGCObsSummary* summary;
			if (ecve == obsSummaries->end())
			{
				summary = new TLGCObsSummary();
				(*obsSummaries)["ECVE"] = summary;
				summary->defineLengthObservation();
			}
			else
			{
				summary = ecve->second;
			}
			
			LSOffsetToVerLineIter measurement = calcDataSet->getOffVerLineByID((*obsIt)->getObservationID());

			summary->addNewResidual(measurement->getResidue().getMMetresValue());

			obsIt++;
		}

		iter++;
	}

	// Offset To Spatial Line
	auto spaLines = dataSet->getWorkingROMs()->getOffsetToSpatialLineROMs();
	auto spaLinesIter = spaLines.begin();

	while (spaLinesIter != spaLines.end())
	{
		list<const TOffsetToLineOrPlaneMeasurement*> meass = (*spaLinesIter)->getMeasurements();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator obsIt = meass.begin();

		while (obsIt != meass.end())
		{			
			string name = (*obsIt)->getScale()->getInstrumentID();

			hash_map<string, TLGCObsSummary*>* obsSummaries;
			hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator temp = scalesMap.find(name);
			if (temp == scalesMap.end())
			{
				obsSummaries = new hash_map<string, TLGCObsSummary*>();
				scalesMap[name] = obsSummaries;
			}
			else
			{
				obsSummaries = temp->second;
			}

			hash_map<string, TLGCObsSummary*>::iterator ecsp = obsSummaries->find("ECSP");

			TLGCObsSummary* summary;
			if (ecsp == obsSummaries->end())
			{
				summary = new TLGCObsSummary();
				(*obsSummaries)["ECSP"] = summary;
				summary->defineLengthObservation();
			}
			else
			{
				summary = ecsp->second;
			}
			
			LSOffsetToSpaLineIter measurement = calcDataSet->getOffSpaLineByID((*obsIt)->getObservationID());

			summary->addNewResidual(measurement->getResidue().getMMetresValue());

			obsIt++;
		}

		spaLinesIter++;
	}

	// Offset To Vertical Plane
	list<const OffsetToLineOrPlaneROM*> verPlanes = dataSet->getWorkingROMs()->getOffsetToVerticalPlaneROMs();
	list<const OffsetToLineOrPlaneROM*>::const_iterator verPlaneIters = verPlanes.begin();

	while (verPlaneIters != verPlanes.end())
	{
		list<const TOffsetToLineOrPlaneMeasurement*> meass = (*verPlaneIters)->getMeasurements();
		list<const TOffsetToLineOrPlaneMeasurement*>::const_iterator obsIt = meass.begin();

		while (obsIt != meass.end())
		{			
			string name = (*obsIt)->getScale()->getInstrumentID();

			hash_map<string, TLGCObsSummary*>* obsSummaries;
			hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator temp = scalesMap.find(name);
			if (temp == scalesMap.end())
			{
				obsSummaries = new hash_map<string, TLGCObsSummary*>();
				scalesMap[name] = obsSummaries;
			}
			else
			{
				obsSummaries = temp->second;
			}

			hash_map<string, TLGCObsSummary*>::iterator echo = obsSummaries->find("ECHO");

			TLGCObsSummary* summary;
			if (echo == obsSummaries->end())
			{
				summary = new TLGCObsSummary();
				(*obsSummaries)["ECHO"] = summary;
				summary->defineLengthObservation();
			}
			else
			{
				summary = echo->second;
			}
			
			LSOffsetToVerPlaneIter measurement = calcDataSet->getOffVerPlaneByID((*obsIt)->getObservationID());

			summary->addNewResidual(measurement->getResidue().getMMetresValue());

			obsIt++;
		}

		verPlaneIters++;
	}

	// Gyro measurements
	list<GyroscopeStation*> gyros = dataSet->getWorkingStations()->getGyroscopeStations();
	list<GyroscopeStation*>::const_iterator gyrosIter = gyros.begin();

	while (gyrosIter != gyros.end())
	{
		string name = (*gyrosIter)->getGyroscope()->getInstrumentID();

		TLGCObsSummary* summary;
		hash_map<string, TLGCObsSummary*>::iterator temp = gyroscopesMap.find(name);
		if (temp == gyroscopesMap.end())
		{
			summary = new TLGCObsSummary();
			gyroscopesMap[name] = summary;
		}
		else
		{
			summary = temp->second;
		}

		list<const THorAngleMeasurement*> meass = (*gyrosIter)->getGyroscopeROM()->getMeasurements();
		list<const THorAngleMeasurement*>::const_iterator obsIt = meass.begin();

		while(obsIt != meass.end())
		{			
			LSGyroOrieIter measurement = calcDataSet->getGyroOrieByID((*obsIt)->getObservationID());

			summary->addNewResidual(measurement->getResidue().getSignedCCValue());

			obsIt++;
		}

		gyrosIter++;
	}

	// Level measurements
	list<const VerticalDistanceROM*> vdROMs = dataSet->getWorkingROMs()->getVerticalDistanceROMs();
	list<const VerticalDistanceROM*>::const_iterator vdIter = vdROMs.begin();

	while (vdIter != vdROMs.end())
	{
		string name = (*vdIter)->getLevel()->getInstrumentID();

		hash_map<string, TLGCObsSummary*>* obsSummaries;
		hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator temp = levelsMap.find(name);
		if (temp == levelsMap.end())
		{
			obsSummaries = new hash_map<string, TLGCObsSummary*>();
			levelsMap[name] = obsSummaries;
		}
		else
		{
			obsSummaries = temp->second;
		}
		
		hash_map<string, TLGCObsSummary*>::iterator dver = obsSummaries->find("DVER");

		TLGCObsSummary* summary;
		if (dver == obsSummaries->end())
		{
			summary = new TLGCObsSummary();
			(*obsSummaries)["DVER"] = summary;
			summary->defineLengthObservation();
		}
		else
		{
			summary = dver->second;
		}

		list<const TVerticalDistMeasurement*> meass = (*vdIter)->getMeasurements();
		list<const TVerticalDistMeasurement*>::const_iterator obsIt = meass.begin();

		while (obsIt != meass.end())
		{
			LSVertDistIter measurement = calcDataSet->getVertDistByID((*obsIt)->getObservationID());

			summary->addNewResidual(measurement->getResidue().getMMetresValue());

			obsIt++;
		}

		vdIter++;
	}

	const list<LevelStation*> lss = dataSet->getWorkingStations()->getLevelStations();
	list<LevelStation*>::const_iterator lsIter = lss.begin();

	while (lsIter != lss.end())
	{
		string name = (*lsIter)->getInstrument()->getInstrumentID();

		hash_map<string, TLGCObsSummary*>* obsSummaries;
		hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator temp = levelsMap.find(name);
		if (temp == levelsMap.end())
		{
			obsSummaries = new hash_map<string, TLGCObsSummary*>();
			levelsMap[name] = obsSummaries;
		}
		else
		{
			obsSummaries = temp->second;
		}
		
		hash_map<string, TLGCObsSummary*>::iterator dlev = obsSummaries->find("DLEV");

		TLGCObsSummary* summary;
		if (dlev == obsSummaries->end())
		{
			summary = new TLGCObsSummary();
			(*obsSummaries)["DLEV"] = summary;
			summary->defineLengthObservation();
		}
		else
		{
			summary = dlev->second;
		}

		const list<const LevelMeasurement*> meass = (*lsIter)->getVerticalDistanceROM()->getMeasurements();
		list<const LevelMeasurement*>::const_iterator obsIt = meass.begin();

		const list<const THorizontalDistMeas<Staff>*> hdMeass = (*lsIter)->getHorizontalDistanceROM()->getMeasurements();
		list<const THorizontalDistMeas<Staff>*>::const_iterator hdObsIt = hdMeass.begin();

		TLGCObsSummary* dhorSummary = NULL;

		bool hasDistance = (*obsIt)->hasDistanceMeasurement();
		if (hasDistance)
		{
			hash_map<string, TLGCObsSummary*>::iterator dhor = obsSummaries->find("DHOR");

			if (dhor == obsSummaries->end())
			{
				dhorSummary = new TLGCObsSummary();
				(*obsSummaries)["DHOR"] = dhorSummary;
				dhorSummary->defineLengthObservation();
			}
			else
			{
				dhorSummary = dhor->second;
			}
		}

		while (obsIt != meass.end())
		{
			LSLevelIter measurement = calcDataSet->getLevelObsByID((*obsIt)->getObservationID());

			summary->addNewResidual(measurement->getResidue().getMMetresValue());

			if (hasDistance)
			{
				LSLevelHorDistIter m = calcDataSet->getCalcLevelHorDistByID((*hdObsIt)->getObservationID());

				dhorSummary->addNewResidual(measurement->getResidue().getMMetresValue());

				hdObsIt++;
			}

			obsIt++;
		}

		lsIter++;
	}
}

InstrumentsCalcRecord::~InstrumentsCalcRecord()
{
	hash_map<string, hash_map<string, TLGCObsSummary*>* >::iterator tMapIter = theodolitesMap.begin();

	while (tMapIter != theodolitesMap.end())
	{
		hash_map<string, TLGCObsSummary*>::iterator i = tMapIter->second->begin();

		while (i != tMapIter->second->end())
		{
			delete i->second;

			i++;
		}

		delete tMapIter->second;

		tMapIter++;
	}

	hash_map<string, hash_map<string, TLGCObsSummary*>* >::iterator eMapIter = edmsMap.begin();

	while (eMapIter != edmsMap.end())
	{
		hash_map<string, TLGCObsSummary*>::iterator i = eMapIter->second->begin();

		while (i != eMapIter->second->end())
		{
			delete i->second;

			i++;
		}

		delete eMapIter->second;

		eMapIter++;
	}

	hash_map<string, hash_map<string, TLGCObsSummary*>* >::iterator sMapIter = scalesMap.begin();

	while (sMapIter != scalesMap.end())
	{
		hash_map<string, TLGCObsSummary*>::iterator i = sMapIter->second->begin();

		while (i != sMapIter->second->end())
		{
			delete i->second;

			i++;
		}

		delete sMapIter->second;

		sMapIter++;
	}

	hash_map<string, TLGCObsSummary*>::iterator gMapIter = gyroscopesMap.begin();

	while (gMapIter != gyroscopesMap.end())
	{
		delete gMapIter->second;

		gMapIter++;
	}

	hash_map<string, hash_map<string, TLGCObsSummary*>* >::iterator lMapIter = levelsMap.begin();

	while (lMapIter != levelsMap.end())
	{
		hash_map<string, TLGCObsSummary*>::iterator i = lMapIter->second->begin();

		while (i != lMapIter->second->end())
		{
			delete i->second;

			i++;
		}

		delete lMapIter->second;

		lMapIter++;
	}
}

hash_map<string, TLGCObsSummary*>* InstrumentsCalcRecord::getTheodoliteObservationSummary(const string& instrumentID)
{
	return theodolitesMap[instrumentID];
}

hash_map<string, TLGCObsSummary*>* InstrumentsCalcRecord::getEDMObservationSummary(const string& instrumentID)
{
	return edmsMap[instrumentID];
}

hash_map<string, TLGCObsSummary*>* InstrumentsCalcRecord::getScaleObservationSummary(const string& instrumentID)
{
	hash_map<string, hash_map<string, TLGCObsSummary*>*>::iterator i = scalesMap.find(instrumentID);
	if (i != scalesMap.end())
	{
		return i->second;
	}
	return NULL;
}

TLGCObsSummary* InstrumentsCalcRecord::getGyroscopeObservationSummary(const string& instrumentID)
{
	return gyroscopesMap[instrumentID];
}


hash_map<string, TLGCObsSummary*>* InstrumentsCalcRecord::getLevelObservationSummary(const string& instrumentID)
{
	return levelsMap[instrumentID];
}

