// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <ctime>
#include <string>

#include <TLGCApp.h>
#include <TLGCData.h>
#include <TSimulationOutputFileWriter.h>

#include "TAStreamFormatter.h"
#include "TFRAMEWriter.h"

//////////////////////////////////////////////////////////////////////
// Definitions and Initialisations
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// constructor / destructor
/////////////////////////////////////////////////////////////////////////////
TSimulationOutputFileWriter::TSimulationOutputFileWriter() : TResultsFileWriter()
{ // default constructor
}

TSimulationOutputFileWriter::TSimulationOutputFileWriter(TAStreamFormatter *stream, const TLGCData *project) : TResultsFileWriter(stream, project)
{ // constructor
}

TSimulationOutputFileWriter::~TSimulationOutputFileWriter()
{ // destructor
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEMBER PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Write the first part of the LGC simulation results file for the given project
void TSimulationOutputFileWriter::writeFileBegin()
{
	// Limited just number of points and unknowns for points for now
	this->initObsListNumber();

	// Snapshot elapsed time so writeTitle() shows the computation time for the first iteration
	TLGCApp::updateProcessingElapsedSeconds();

	// write the header title of the results file
	this->writeTitle();

	// write the input data summary
	this->writeDataSummary();

	// Summary from the first simulation, like number of iterations
	this->writeCalcDataSummary();
}

void TSimulationOutputFileWriter::writeSimSummary(TLGCData &project, int numberOfSim)
{
	// write simulation header
	this->writeSimHeader(project, numberOfSim);

	TFRAMEWriter frameWriter(*getStream(), &project);

	// Tteration through the tree nodes
	for (TDataTreeIterator itTree = project.getTree().begin(); itTree != project.getTree().end(); itTree++)
	{
		frameWriter.writeFRAMESimu(itTree); // Writes the simulation summary
	}
}

void TSimulationOutputFileWriter::writeLastSimResult(TLGCData &project, int numberOfSim)
{
	// write simulation header
	this->writeSimHeader(project, numberOfSim);

	TFRAMEWriter frameWriter(*getStream(), fProjectData);

	// Tteration through the tree nodes
	for (TDataTreeIterator itTree = fProjectData->getTree().begin(); itTree != fProjectData->getTree().end(); itTree++)
	{
		frameWriter.writeFRAMEAll(itTree); // Write all the information (also the simulated observed values) for the last iteration
	}
}

// Write the header for a given simulation
void TSimulationOutputFileWriter::writeSimHeader(TLGCData &data, int i)
{
	TAStreamFormatter *stream = getStream();
	std::string separator = getSeparator();

	// write title
	(*stream) << "SIMULATION NUMERO" << separator << i << endl
			  << "*********************" << endl
			  << endl
			  << endl
			  << " DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !" << endl
			  << endl
			  << endl;

	// write sigma0 a posteriori
	writeSigmaAPosteriori(data);
}

void TSimulationOutputFileWriter::writeSimTableDescription(const std::string &projTitle, const std::string &objectType, int i)
{
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter *stream = getStream();
	std::string origSepar = stream->getSeparator();
	stream->setSeparator("");
	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();

	(*stream) << endl << endl;
	(*stream) << "*********************************************************************************************************************************** " << endl;
	(*stream) << endl << endl << endl;

	// simulation summary
	(*stream) << "RESUME APRES : " << i << " SIMULATIONS" << endl;

	TLGCApp::updateProcessingElapsedSeconds();
	(*stream) << "#CALCUL DU " << TLGCApp::getStartProcessingTimestamp() << ". PROCESSING ELAPSED SECONDS ";
	(*stream).writeDouble(0, 7, TLGCApp::getProcessingElapsedSeconds());
	(*stream) << "\n" << "\n" << "\n";
	// write title
	(*stream) << projTitle << endl;
	(*stream) << "*********************************************************************************************************************************** " << endl;
	(*stream) << endl << endl;

	// header
	(*stream).writeString(nameCoordWidth, objectType);
	(*stream).writeString(9 * (coordResWidth + 1) + 44, "ECARTS-TYPES DES " + objectType);
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth + 1 + 9 * (coordResWidth + 1) + 44, "(SIG ZERO = 1         ");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth + 1 + 9 * (coordResWidth + 1) + 44, " R = SIGMA/ECART-TYPE)");
	(*stream) << endl << endl;

	// Nom
	(*stream).writeStringLeft(nameCoordWidth, "NOM");
	(*stream).writeStringLeft(nameCoordWidth, "FRAME");

	// X offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DX ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DX ");

	// Y offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DY ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DY ");

	// Z offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DZ ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DZ ");

	// sigma X
	(*stream).writeString(2, "");
	(*stream).writeString(coordResWidth, "SX ");

	// sigma Y
	(*stream).writeString(coordResWidth, "SY ");

	// sigma Z
	(*stream).writeString(coordResWidth, "SZ ");
	(*stream) << endl;

	// units
	// space for name
	(*stream).writeStringLeft(nameCoordWidth, "");
	(*stream).writeString(nameCoordWidth, "");

	// X, Y, Z offset units
	for (int j = 0; j < 3; j++)
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "(MM)");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "(MM)");
	}

	(*stream).writeString(2, "");

	// sigma X, Y, Z units
	for (int j = 0; j < 3; j++)
	{
		(*stream).writeString(coordResWidth, "(MM)");
	}
	(*stream) << endl;

	stream->setSeparator(origSepar);
}

void TSimulationOutputFileWriter::writeSimFRAMEDescription()
{
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter *stream = getStream();
	std::string origSepar = stream->getSeparator();
	stream->setSeparator("");
	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();

	(*stream) << endl << endl;
	(*stream) << "*********************************************************************************************************************************** " << endl;
	(*stream) << endl << endl << endl;

	// header
	(*stream).writeString(nameCoordWidth, "FRAME");
	(*stream).writeString(9 * (coordResWidth + 1) + 44, "ECARTS-TYPES");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth + 1 + 9 * (coordResWidth + 1) + 44, "(SIG ZERO = 1         ");
	(*stream) << endl;
	(*stream).writeString(nameCoordWidth + 1 + 9 * (coordResWidth + 1) + 44, " R = SIGMA/ECART-TYPE)");
	(*stream) << endl << endl;

	// Nom
	(*stream).writeStringLeft(nameCoordWidth, "FRAME");

	// TX offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DTX ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DTX ");

	// TY offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DTY ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DTY ");

	// TZ offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DTZ ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DTZ ");

	// RX offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DRX ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DRX ");

	// RY offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DRY ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DRY ");

	// RZ offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DRZ ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DRZ ");

	// SCL offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "DSC ");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "DSC ");

	// sigma TX
	(*stream).writeString(2, "");
	(*stream).writeString(coordResWidth, "STX ");

	// sigma TY
	(*stream).writeString(coordResWidth, "STY ");

	// sigma TZ
	(*stream).writeString(coordResWidth, "STZ ");

	// sigma RX
	(*stream).writeString(coordResWidth, "SRX ");

	// sigma RY
	(*stream).writeString(coordResWidth, "SRY ");

	// sigma RZ
	(*stream).writeString(coordResWidth, "SRZ ");

	// sigma SCL
	(*stream).writeString(coordResWidth, "SSC ");
	(*stream) << endl;

	// units
	// space for name
	(*stream).writeString(nameCoordWidth, "");

	// translation offset units
	for (int j = 0; j < 3; j++)
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "(MM)");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "(MM)");
	}

	// rotation offset units
	for (int j = 3; j < 6; j++)
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "(CC)");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "(CC)");
	}

	// Scale offset
	(*stream).writeString(5, "");
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream).writeString(7, "");
	(*stream).writeString(coordResWidth, "(MM)");

	(*stream).writeString(2, "");

	// sigma translation units
	for (int j = 0; j < 3; j++)
	{
		(*stream).writeString(coordResWidth, "(MM)");
	}

	// sigma rotaion units
	for (int j = 0; j < 3; j++)
	{
		(*stream).writeString(coordResWidth, "(CC)");
	}

	// scale sigma
	(*stream).writeString(coordResWidth, "(MM)");
	(*stream) << endl;

	stream->setSeparator(origSepar);
}

void TSimulationOutputFileWriter::writeRelErrorHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = std::max(getObsResWidth(), 9);

	// write header
	(*stream) << endl << endl << endl;
	(*stream) << "ERREURS RELATIVES " << endl;
	(*stream) << "*******************" << endl << endl;

	//////////////////////////////////////////////////////////////
	// line1
	//  point 1 & 2 & frame
	(*stream).writeStringLeft(nameWidth, "POINT 1");
	(*stream).writeStringLeft(nameWidth, "POINT 2");
	(*stream).writeStringLeft(nameWidth, "Frame  ");
	(*stream).writeStringLeft(nameWidth, "");
	// Sigmas
	(*stream).writeString(obsResWidth, "SIGMA L");
	(*stream).writeString(obsResWidth, "SIGMA G");
	(*stream).writeString(obsResWidth, "SIGMA R");
	(*stream).writeString(obsResWidth, "SIGMA Z");
	(*stream).writeString(obsResWidth, "SIGMA V");
	(*stream) << endl;

	//////////////////////////////////////////////////////////////
	// line2
	//  units
	(*stream).writeString(nameWidth, " ");
	(*stream).writeString(nameWidth, " ");
	(*stream).writeString(nameWidth, " ");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(MM)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream) << endl << endl;

	return;
}

void TSimulationOutputFileWriter::writeRelErrorResults(const TLGCData &data)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	std::string separator = getSeparator();

	ERELPointStat reducedEREL = calculateStatForERELPoint(data.getRelError().points);

	// for (auto& ptPairIt : reducedEREL.MaxErel /*data.getRelError()*/)
	for (int ptPairIt = 0; ptPairIt < reducedEREL.MaxErel.size(); ptPairIt++)
	{
		// LINE 1 : MIN
		// write points name
		(*stream).writeStringLeft(nameWidth, reducedEREL.MaxErel.at(ptPairIt).getPoint1());
		(*stream).writeStringLeft(nameWidth, reducedEREL.MaxErel.at(ptPairIt).getPoint2());
		(*stream).writeStringLeft(nameWidth, reducedEREL.MaxErel.at(ptPairIt).getDestinationFrame());
		(*stream).writeStringLeft(nameWidth, "MIN");
		// sets the values format:
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setAngleUnits(TAngle::k100MicroGons);
		stream->setPrecisionFormat(getLengthPrecision());
		stream->setWidthFormat(std::max(getObsResWidth(), 9));

		// sigma L
		(*stream) << right << reducedEREL.MinErel.at(ptPairIt).getSigmaL() << separator;

		// sigma G
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << reducedEREL.MinErel.at(ptPairIt).getSigmaG() << separator;

		// sigma R
		stream->setPrecisionFormat(getLengthPrecision());
		(*stream) << right << reducedEREL.MinErel.at(ptPairIt).getSigmaR() << separator;

		// sigma Z
		(*stream) << right << reducedEREL.MinErel.at(ptPairIt).getSigmaZ() << separator;

		// sigma V
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << reducedEREL.MinErel.at(ptPairIt).getSigmaV() << separator << endl;
		//(*stream).setDataSpacing();

		// LINE 2: MAX
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "MAX");
		// sigma L
		(*stream) << right << reducedEREL.MaxErel.at(ptPairIt).getSigmaL() << separator;

		// sigma G
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << reducedEREL.MaxErel.at(ptPairIt).getSigmaG() << separator;

		// sigma R
		stream->setPrecisionFormat(getLengthPrecision());
		(*stream) << right << reducedEREL.MaxErel.at(ptPairIt).getSigmaR() << separator;

		// sigma Z
		(*stream) << right << reducedEREL.MaxErel.at(ptPairIt).getSigmaZ() << separator;

		// sigma V
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << reducedEREL.MaxErel.at(ptPairIt).getSigmaV() << separator << endl;

		// LINE 3: MEAN
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "");
		(*stream).writeStringLeft(nameWidth, "MEAN");
		// sigma L
		(*stream) << right << reducedEREL.MeanErel.at(ptPairIt).getSigmaL() << separator;

		// sigma G
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << reducedEREL.MeanErel.at(ptPairIt).getSigmaG() << separator;

		// sigma R
		stream->setPrecisionFormat(getLengthPrecision());
		(*stream) << right << reducedEREL.MeanErel.at(ptPairIt).getSigmaR() << separator;

		// sigma Z
		(*stream) << right << reducedEREL.MeanErel.at(ptPairIt).getSigmaZ() << separator;

		// sigma V
		stream->setPrecisionFormat(getAnglePrecision());
		(*stream) << right << reducedEREL.MeanErel.at(ptPairIt).getSigmaV() << separator << endl;
	}

	return;
}
void TSimulationOutputFileWriter::writeRelErrorFrameHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = std::max(getObsResWidth(), 9);

	// write header
	(*stream) << endl << endl << endl;
	(*stream) << "ERREURS RELATIVES FRAMES" << endl;
	(*stream) << "*******************" << endl << endl;

	return;
}
void TSimulationOutputFileWriter::writeRelErrorFrameResults(const TLGCData &data)
{
	TAStreamFormatter *stream = getStream();
	// write frame names
	int nameWidth = getNameWidth();
	for (auto relFrame : data.getRelError().frames)
	{
		std::string relFrameName = "[" + relFrame.getFromFrame() + "->" + relFrame.getToFrame() + "]";
		(*stream) << "Relative Transformation:";
		(*stream).writeStringLeft(nameWidth, relFrameName);
		(*stream) << endl;
		TFRAMEWriter aux(*stream, &data);
		aux.writeFRAMEDefinition(relFrame.getResult());
	}
	return;
}

ERELPointStat TSimulationOutputFileWriter::calculateStatForERELPoint(std::vector<TLSCalcRelativeErrorPoint> ERELdata)
{
	ERELPointStat statForErel;
	std::vector<int> numOfPair;

	for (auto &ptPairIt : ERELdata)
	{
		if (statForErel.MaxErel.empty() && statForErel.MinErel.empty() && statForErel.MeanErel.empty())
		{
			statForErel.MaxErel.push_back(ptPairIt);
			statForErel.MinErel.push_back(ptPairIt);
			statForErel.MeanErel.push_back(ptPairIt);
			numOfPair.push_back(1);
		}
		else
		{
			bool saved = false;
			// find if pair is already saved
			for (int itInStat = 0; itInStat < statForErel.MaxErel.size(); itInStat++)
			{
				// pair is already save
				if (statForErel.MinErel.at(itInStat).getPoint1() == ptPairIt.getPoint1() && statForErel.MinErel.at(itInStat).getPoint2() == ptPairIt.getPoint2()
					&& statForErel.MinErel.at(itInStat).getDestinationFrame() == ptPairIt.getDestinationFrame())
				{
					numOfPair.at(itInStat) += 1;

					// update mean
					statForErel.MeanErel.at(itInStat).setSigmaG(
						TAngle((statForErel.MeanErel.at(itInStat).getSigmaG().getRadiansValue() * (numOfPair.at(itInStat) - 1) + ptPairIt.getSigmaG().getRadiansValue())
								* 1 / numOfPair.at(itInStat),
							TAngle::EUnits::kRadians));
					statForErel.MeanErel.at(itInStat).setSigmaV(
						TAngle((statForErel.MeanErel.at(itInStat).getSigmaV().getRadiansValue() * (numOfPair.at(itInStat) - 1) + ptPairIt.getSigmaV().getRadiansValue())
								* 1 / numOfPair.at(itInStat),
							TAngle::EUnits::kRadians));
					statForErel.MeanErel.at(itInStat).setSigmaL(TLength(
						(statForErel.MeanErel.at(itInStat).getSigmaL().getMetresValue() * (numOfPair.at(itInStat) - 1) + ptPairIt.getSigmaL()) * 1 / numOfPair.at(itInStat)));
					statForErel.MeanErel.at(itInStat).setSigmaR(TLength(
						(statForErel.MeanErel.at(itInStat).getSigmaR().getMetresValue() * (numOfPair.at(itInStat) - 1) + ptPairIt.getSigmaR()) * 1 / numOfPair.at(itInStat)));
					statForErel.MeanErel.at(itInStat).setSigmaZ(TLength(
						(statForErel.MeanErel.at(itInStat).getSigmaZ().getMetresValue() * (numOfPair.at(itInStat) - 1) + ptPairIt.getSigmaZ()) * 1 / numOfPair.at(itInStat)));

					// update min if necessary
					if (ptPairIt.getSigmaG() < statForErel.MinErel.at(itInStat).getSigmaG())
						statForErel.MinErel.at(itInStat).setSigmaG(ptPairIt.getSigmaG());
					if (ptPairIt.getSigmaL() < statForErel.MinErel.at(itInStat).getSigmaL())
						statForErel.MinErel.at(itInStat).setSigmaL(ptPairIt.getSigmaL());
					if (ptPairIt.getSigmaR() < statForErel.MinErel.at(itInStat).getSigmaR())
						statForErel.MinErel.at(itInStat).setSigmaR(ptPairIt.getSigmaR());
					if (ptPairIt.getSigmaZ() < statForErel.MinErel.at(itInStat).getSigmaZ())
						statForErel.MinErel.at(itInStat).setSigmaZ(ptPairIt.getSigmaZ());
					if (ptPairIt.getSigmaV() < statForErel.MinErel.at(itInStat).getSigmaV())
						statForErel.MinErel.at(itInStat).setSigmaV(ptPairIt.getSigmaV());

					// update max if necessary
					if (ptPairIt.getSigmaG() > statForErel.MaxErel.at(itInStat).getSigmaG())
						statForErel.MaxErel.at(itInStat).setSigmaG(ptPairIt.getSigmaG());
					if (ptPairIt.getSigmaL() > statForErel.MaxErel.at(itInStat).getSigmaL())
						statForErel.MaxErel.at(itInStat).setSigmaL(ptPairIt.getSigmaL());
					if (ptPairIt.getSigmaR() > statForErel.MaxErel.at(itInStat).getSigmaR())
						statForErel.MaxErel.at(itInStat).setSigmaR(ptPairIt.getSigmaR());
					if (ptPairIt.getSigmaZ() > statForErel.MaxErel.at(itInStat).getSigmaZ())
						statForErel.MaxErel.at(itInStat).setSigmaZ(ptPairIt.getSigmaZ());
					if (ptPairIt.getSigmaV() > statForErel.MaxErel.at(itInStat).getSigmaV())
						statForErel.MaxErel.at(itInStat).setSigmaV(ptPairIt.getSigmaV());
					saved = true;
				}
			}
			// if not, save it
			if (!saved)
			{
				statForErel.MaxErel.push_back(ptPairIt);
				statForErel.MinErel.push_back(ptPairIt);
				statForErel.MeanErel.push_back(ptPairIt);
				numOfPair.push_back(1);
			}
		}
	}
	return statForErel;
}
////////////////////////////////////////////////////////////
// STATISTIC
////////////////////////////////////////////////////////////
void TSimulationOutputFileWriter::writeSimPointsSummary(const std::string &projTitle, const std::list<TSimPointSummary> &dataSum, int numbOfSimu)
{
	writeSimTableDescription(projTitle, "POINTS", numbOfSimu);

	for (auto &pointSummary : dataSum)
	{
		writeSimPointData(pointSummary, numbOfSimu);
	}
}

void TSimulationOutputFileWriter::writeSimFramesSummary(const std::list<TSimFrameSummary> &dataSum, int numbOfSimu)
{
	writeSimFRAMEDescription();

	for (auto &frameSummary : dataSum)
	{
		writeSimFRAMEData(frameSummary, numbOfSimu);
	}
}

void TSimulationOutputFileWriter::writeSimPointData(const TSimPointSummary &simPt, const int i)
{ // write point's data
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter *stream = getStream();
	std::string origSepar = stream->getSeparator();
	stream->setSeparator("");
	std::string separator = stream->getSeparator();
	;

	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();
	int coordResPrecision = this->getCoordErrorPrecision();

	// Table data line 1
	// write name
	(*stream).writeStringLeft(nameCoordWidth, simPt.getAdjustablePoint()->getName());
	(*stream).writeStringLeft(nameCoordWidth, simPt.getAdjustablePoint()->getFrameTreePosition().node->data.get()->frame.getName());

	// get delta from LGC simulations' results
	TFreeVector deltaMoy = simPt.getSumRes() * (LITERAL(1.0) / i);
	TFreeVector deltaMax = simPt.getMaxRes();
	TFreeVector deltaMin = simPt.getMinRes();
	TFreeVector deltaSigma = simPt.getSumRes2();

	TReal sigmadx(sqrtq(deltaSigma.getX() * (LITERAL(1.0) / i) - powq(deltaMoy.getX().getMetresValue(), 2)) * M2MM);
	TReal sigmady(sqrtq(deltaSigma.getY() * (LITERAL(1.0) / i) - powq(deltaMoy.getY().getMetresValue(), 2)) * M2MM);
	TReal sigmadz(sqrtq(deltaSigma.getZ() * (LITERAL(1.0) / i) - powq(deltaMoy.getZ().getMetresValue(), 2)) * M2MM);
	TReal sx(simPt.getAdjustablePoint()->getXEstPrecision().getMMetresValue());
	TReal sy(simPt.getAdjustablePoint()->getYEstPrecision().getMMetresValue());
	TReal sz(simPt.getAdjustablePoint()->getZEstPrecision().getMMetresValue());

	stream->setWidthFormat(coordResWidth);
	stream->setPrecisionFormat(coordResPrecision);

	int precisionMM = coordResPrecision > 3 ? (coordResPrecision - 3) : 0; // This is because the values are outputted in [mm] for the TLength the precision is cut for a factor of 3.
																		   //  but we output 'double', we need to do it here!
	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		(*stream).writeString(5, "MAX");
		writeDouble(coordResWidth, precisionMM, deltaMax.getX().getMMetresValue());
		(*stream).writeString(7, "MOYEN");
		writeDouble(coordResWidth, precisionMM, deltaMoy.getX().getMMetresValue());
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "*");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "*");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		(*stream).writeString(5, "MAX");
		writeDouble(coordResWidth, precisionMM, deltaMax.getY().getMMetresValue());
		(*stream).writeString(7, "MOYEN");
		writeDouble(coordResWidth, precisionMM, deltaMoy.getY().getMMetresValue());
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "*");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "*");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		(*stream).writeString(5, "MAX");
		writeDouble(coordResWidth, precisionMM, deltaMax.getZ().getMMetresValue());
		(*stream).writeString(7, "MOYEN");
		writeDouble(coordResWidth, precisionMM, deltaMoy.getZ().getMMetresValue());
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "*");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "*");
	}

	(*stream).writeString(2, "");
	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		writeDouble(coordResWidth, precisionMM, sx);
	}
	else
	{
		(*stream).writeString(coordResWidth, "*");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		writeDouble(coordResWidth, precisionMM, sy);
	}
	else
	{
		(*stream).writeString(coordResWidth, "*");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		writeDouble(coordResWidth, precisionMM, sz);
	}
	else
	{
		(*stream).writeString(coordResWidth, "*");
	}
	(*stream) << endl;

	// Table data line 2
	(*stream).writeStringLeft(nameCoordWidth, "");

	std::stringstream result;
	const std::vector<int> &ID = simPt.getAdjustablePoint()->getFrameTreePosition().node->data.get()->ID;
	std::copy(ID.begin(), ID.end(), std::ostream_iterator<int>(result));

	(*stream).writeStringLeft(nameCoordWidth, "(" + result.str() + ")");

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		(*stream).writeString(5, "MIN");
		writeDouble(coordResWidth, precisionMM, deltaMin.getX().getMMetresValue());
		(*stream).writeString(7, "SIGMA");
		writeDouble(coordResWidth, precisionMM, sigmadx);
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		(*stream).writeString(5, "MIN");
		writeDouble(coordResWidth, precisionMM, deltaMin.getY().getMMetresValue());
		(*stream).writeString(7, "SIGMA");
		writeDouble(coordResWidth, precisionMM, sigmady);
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		(*stream).writeString(5, "MIN");
		writeDouble(coordResWidth, precisionMM, deltaMin.getZ().getMMetresValue());
		(*stream).writeString(7, "SIGMA");
		writeDouble(coordResWidth, precisionMM, sigmadz);
	}
	else
	{
		(*stream).writeString(5, "");
		(*stream).writeString(coordResWidth, "");
		(*stream).writeString(7, "");
		(*stream).writeString(coordResWidth, "");
	}

	(*stream).writeString(2, "R");
	if (!simPt.getAdjustablePoint()->isCoordinateFixed(0))
	{
		writeDouble(coordResWidth, precisionMM, sigmadx / sx); // This is unitless, but we want the same precision as for [mm]
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(1))
	{
		writeDouble(coordResWidth, precisionMM, sigmady / sy); // This is unitless, but we want the same precision as for [mm]
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}

	if (!simPt.getAdjustablePoint()->isCoordinateFixed(2))
	{
		writeDouble(coordResWidth, precisionMM, sigmadz / sz); // This is unitless, but we want the same precision as for [mm]
	}
	else
	{
		(*stream).writeString(coordResWidth, "");
	}
	(*stream) << endl;

	// Table data line 3
	// write overal sigma
	(*stream).writeString(nameCoordWidth, "OVERALL SIGMA =");
	(*stream) << (TReal)(sqrtq(powq(sigmadx, 2) + powq(sigmady, 2) + powq(sigmadz, 2))) << separator;
	(*stream) << endl << endl;

	stream->setSeparator(origSepar);
}

void TSimulationOutputFileWriter::writeSimFRAMEData(const TSimFrameSummary &simFr, const int i)
{
	// writeframe's data
	/*We do not want any separator, therefore re-set and in the end of the function returned back to the original setting*/
	TAStreamFormatter *stream = getStream();
	std::string origSepar = stream->getSeparator();
	stream->setSeparator("");
	std::string separator = stream->getSeparator();
	;

	int nameCoordWidth = getNameWidth();
	int coordResWidth = this->getCoordResWidth();
	int coordResPrecision = this->getCoordErrorPrecision();

	// Table data line 1
	// write name
	(*stream).writeStringLeft(nameCoordWidth, simFr.getAdjustableTransformation()->getName());

	// get delta from LGC simulations' results
	TransformParameters deltaMoy = simFr.getSumRes();
	deltaMoy.kappa *= (LITERAL(1.0) / i);
	deltaMoy.phi *= (LITERAL(1.0) / i);
	deltaMoy.omega *= (LITERAL(1.0) / i);
	deltaMoy.tX *= (LITERAL(1.0) / i);
	deltaMoy.tY *= (LITERAL(1.0) / i);
	deltaMoy.tZ *= (LITERAL(1.0) / i);
	deltaMoy.scale *= (LITERAL(1.0) / i);

	TransformParameters deltaMax = simFr.getMaxRes();
	TransformParameters deltaMin = simFr.getMinRes();
	TransformParametersSquare deltaSigma = simFr.getSumRes2();

	TReal sigmadtx(sqrtq((deltaSigma.tX.getMMetresValue() - powq(deltaMoy.tX.getMMetresValue() * i, 2) / i) * (LITERAL(1.0) / i)));
	TReal sigmadty(sqrtq((deltaSigma.tY.getMMetresValue() - powq(deltaMoy.tY.getMMetresValue() * i, 2) / i) * (LITERAL(1.0) / i)));
	TReal sigmadtz(sqrtq((deltaSigma.tZ.getMMetresValue() - powq(deltaMoy.tZ.getMMetresValue() * i, 2) / i) * (LITERAL(1.0) / i)));
	TReal sigmadrx(sqrtq((deltaSigma.omega - powq(deltaMoy.omega.getRadiansValue() * i, 2) / i) * (LITERAL(1.0) / i))); // sigma are in rad, need to convert in CC
	TReal sigmadry(sqrtq((deltaSigma.phi - powq(deltaMoy.phi.getRadiansValue() * i, 2) / i) * (LITERAL(1.0) / i))); // sigma are in rad, need to convert in CC
	TReal sigmadrz(sqrtq((deltaSigma.kappa - powq(deltaMoy.kappa.getRadiansValue() * i, 2) / i) * (LITERAL(1.0) / i))); // sigma are in rad, need to convert in CC
	TReal sigmadscl(sqrtq((deltaSigma.scale * M2MM - powq(deltaMoy.scale * M2MM * i, 2) / i) * (LITERAL(1.0) / i)));

	TReal stx(simFr.getAdjustableTransformation()->getEstimatedPrecisionTransl(0).getMMetresValue()); // Because sigmadtX are in M
	TReal sty(simFr.getAdjustableTransformation()->getEstimatedPrecisionTransl(1).getMMetresValue());
	TReal stz(simFr.getAdjustableTransformation()->getEstimatedPrecisionTransl(2).getMMetresValue());
	TReal srx(simFr.getAdjustableTransformation()->getEstimatedPrecisionRot(0).getSignedCCValue());
	TReal sry(simFr.getAdjustableTransformation()->getEstimatedPrecisionRot(1).getSignedCCValue());
	TReal srz(simFr.getAdjustableTransformation()->getEstimatedPrecisionRot(2).getSignedCCValue());

	stream->setWidthFormat(coordResWidth);
	stream->setPrecisionFormat(coordResPrecision);

	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	int lengthResPrecision = std::max(getLengthResidualPrecision() - 3, 0);

	if (!simFr.getAdjustableTransformation()->isFixed())
	{
		// line 1
		// D translation
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			(*stream).writeString(5, "MAX");
			writeDouble(coordResWidth, lengthResPrecision, deltaMax.tX.getMMetresValue());
			(*stream).writeString(7, "MOYEN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMoy.tX.getMMetresValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "*");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "*");
		}

		if (!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			(*stream).writeString(5, "MAX");
			writeDouble(coordResWidth, lengthResPrecision, deltaMax.tY.getMMetresValue());
			(*stream).writeString(7, "MOYEN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMoy.tY.getMMetresValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "*");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "*");
		}

		if (!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			(*stream).writeString(5, "MAX");
			writeDouble(coordResWidth, lengthResPrecision, deltaMax.tZ.getMMetresValue());
			(*stream).writeString(7, "MOYEN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMoy.tZ.getMMetresValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "*");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "*");
		}

		// D rotations
		if (!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			(*stream).writeString(5, "MAX");
			writeDouble(coordResWidth, angleResPrecision, deltaMax.omega.getSignedCCValue());
			(*stream).writeString(7, "MOYEN");
			writeDouble(coordResWidth, angleResPrecision, deltaMoy.omega.getSignedCCValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "*");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "*");
		}

		if (!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			(*stream).writeString(5, "MAX");
			writeDouble(coordResWidth, angleResPrecision, deltaMax.phi.getSignedCCValue());
			(*stream).writeString(7, "MOYEN");
			writeDouble(coordResWidth, angleResPrecision, deltaMoy.phi.getSignedCCValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "*");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "*");
		}

		if (!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			(*stream).writeString(5, "MAX");
			writeDouble(coordResWidth, angleResPrecision, deltaMax.kappa.getSignedCCValue());
			(*stream).writeString(7, "MOYEN");
			writeDouble(coordResWidth, angleResPrecision, deltaMoy.kappa.getSignedCCValue());
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "*");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "*");
		}

		// D scale
		if (!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			(*stream).writeString(5, "MAX");
			writeDouble(coordResWidth, lengthResPrecision, deltaMax.scale * M2MM);
			(*stream).writeString(7, "MOYEN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMoy.scale * M2MM);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "*");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "*");
		}

		// sigma
		(*stream).writeString(2, "");
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			writeDouble(coordResWidth, lengthResPrecision, stx);
		}
		else
		{
			(*stream).writeString(coordResWidth, "*");
		}
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			writeDouble(coordResWidth, lengthResPrecision, sty);
		}
		else
		{
			(*stream).writeString(coordResWidth, "*");
		}
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			writeDouble(coordResWidth, lengthResPrecision, stz);
		}
		else
		{
			(*stream).writeString(coordResWidth, "*");
		}
		if (!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			writeDouble(coordResWidth, angleResPrecision, srx);
		}
		else
		{
			(*stream).writeString(coordResWidth, "*");
		}
		if (!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			writeDouble(coordResWidth, angleResPrecision, sry);
		}
		else
		{
			(*stream).writeString(coordResWidth, "*");
		}
		if (!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			writeDouble(coordResWidth, angleResPrecision, srz);
		}
		else
		{
			(*stream).writeString(coordResWidth, "*");
		}
		if (!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			writeDouble(coordResWidth, lengthResPrecision, simFr.getAdjustableTransformation()->getEstimatedPrecisionScale() * M2MM); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "*");
		}

		(*stream) << endl;

		// line 2
		(*stream).writeStringLeft(nameCoordWidth, "");
		// D translation
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			(*stream).writeString(5, "MIN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMin.tX.getMMetresValue());
			(*stream).writeString(7, "SIGMA");
			writeDouble(coordResWidth, lengthResPrecision, sigmadtx);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		if (!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			(*stream).writeString(5, "MIN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMin.tY.getMMetresValue());
			(*stream).writeString(7, "SIGMA");
			writeDouble(coordResWidth, lengthResPrecision, sigmadty);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		if (!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			(*stream).writeString(5, "MIN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMin.tZ.getMMetresValue());
			(*stream).writeString(7, "SIGMA");
			writeDouble(coordResWidth, lengthResPrecision, sigmadtz);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		// D rotations
		if (!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			(*stream).writeString(5, "MIN");
			writeDouble(coordResWidth, angleResPrecision, deltaMin.omega.getSignedCCValue());
			(*stream).writeString(7, "SIGMA");
			writeDouble(coordResWidth, angleResPrecision, sigmadrx * RAD2CC);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		if (!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			(*stream).writeString(5, "MIN");
			writeDouble(coordResWidth, angleResPrecision, deltaMin.phi.getSignedCCValue());
			(*stream).writeString(7, "SIGMA");
			writeDouble(coordResWidth, angleResPrecision, sigmadry * RAD2CC);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		if (!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			(*stream).writeString(5, "MIN");
			writeDouble(coordResWidth, angleResPrecision, deltaMin.kappa.getSignedCCValue());
			(*stream).writeString(7, "SIGMA");
			writeDouble(coordResWidth, angleResPrecision, sigmadrz * RAD2CC);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		// D scale
		if (!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			(*stream).writeString(5, "MIN");
			writeDouble(coordResWidth, lengthResPrecision, deltaMin.scale * M2MM);
			(*stream).writeString(7, "SIGMA");
			writeDouble(coordResWidth, lengthResPrecision, sigmadscl * M2MM);
		}
		else
		{
			(*stream).writeString(5, "");
			(*stream).writeString(coordResWidth, "");
			(*stream).writeString(7, "");
			(*stream).writeString(coordResWidth, "");
		}

		// sigma
		(*stream).writeString(2, "R");
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(0))
		{
			writeDouble(coordResWidth, lengthResPrecision, sigmadtx / stx); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(1))
		{
			writeDouble(coordResWidth, lengthResPrecision, sigmadty / sty); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if (!simFr.getAdjustableTransformation()->isTranslationFixed(2))
		{
			writeDouble(coordResWidth, lengthResPrecision, sigmadtz / stz); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if (!simFr.getAdjustableTransformation()->isRotationFixed(0))
		{
			writeDouble(coordResWidth, angleResPrecision, sigmadrx * RAD2CC / srx); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if (!simFr.getAdjustableTransformation()->isRotationFixed(1))
		{
			writeDouble(coordResWidth, angleResPrecision, sigmadry * RAD2CC / sry); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if (!simFr.getAdjustableTransformation()->isRotationFixed(2))
		{
			writeDouble(coordResWidth, angleResPrecision, sigmadrz * RAD2CC / srz); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}
		if (!simFr.getAdjustableTransformation()->isScaleFixed())
		{
			writeDouble(coordResWidth, lengthResPrecision,
				sigmadscl / simFr.getAdjustableTransformation()->getEstimatedPrecisionScale()); // This is unitless, but we want the same precision as for [mm]
		}
		else
		{
			(*stream).writeString(coordResWidth, "");
		}

		(*stream) << endl;

		// line 3
		// write overal sigma
		(*stream).writeString(nameCoordWidth, "OVERALL SIGMA =");
		(*stream)
			<< (TReal)(sqrtq(powq(sigmadtx, 2) + powq(sigmadty, 2) + powq(sigmadtz, 2) + powq(sigmadrx, 2) + powq(sigmadry, 2) + powq(sigmadrz, 2) + powq(sigmadscl * M2MM, 2)))
			<< separator;
		(*stream) << endl << endl;

		stream->setSeparator(origSepar);
	}
}

////////////////////////////////////////////////////////
// END
////////////////////////////////////////////////////////