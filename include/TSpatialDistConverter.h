////////////////////////////////////////////////////////////////////
// TSpatialDistConverter
/*!
Write Spatial Distance Observations for an output LGC file
and send the appropriate messages

Patterns:

 
Copyright 2003-2008 M.Jones, CERN, EST/SU. All rights reserved.
*/
////////////////////////////////////////////////////////////////////////////////////

#ifndef SU_TSPA_DIST_CONVERTER
#define SU_TSPA_DIST_CONVERTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////
// Forward declarations
#include "TLGCProject.h"
#include "TLengthObsConverter.h"
#include "TSpatialDistMeasurement.h"
#include "TheodoliteTarget.h"

//Class definition
class  TSpatialDistConverter : public TLengthObsConverter  
{
	
public:

	TSpatialDistConverter(TAStreamFormatter& stream)  : TLengthObsConverter(stream) { }

	template <typename T, typename Instrument>
	TSpatialDistMeasurement<T>* readObservation(TLGCProject* project, const T* currentTarget, Instrument* instr, int& i)
	{
		string targetPoint;
		TLength* obs = new TLength();

		*fStream >> targetPoint;
		fStream->setLengthUnits(TLength::kMetres);
		*fStream >> *obs;

		PointIterator p = project->getDataSet()->getWorkingPoints()->getPoint(targetPoint);
		if (p == project->getDataSet()->getWorkingPoints()->getPointsEndIterator())
		{
			char lineNum[10];
			_itoa(i, lineNum, 10);
			project->setError(project->getError() + "Line: " + lineNum + " - Unknown point " + targetPoint + "\n");
			return NULL;
		}
		TSpatialPoint* point = &*p;

		string targetID = readOptional("TRGT");

		const T* target;
		if (targetID == "")
		{
			target = currentTarget;
		}
		else
		{
			target = instr->getTargetNamed(targetID);

			if (target == NULL)
			{
				char lineNum[10];
				_itoa(i, lineNum, 10);
				project->setError(project->getError() + "Line: " + lineNum + " - Unknown Spatial Distance target " + targetID + "\n");
				return NULL;
			}
		}

		const TLength* sigma = readOptionalValue(TLength::kMillimetres, "OBSE", target->getDistanceSigma());
		const TLength* ppmE = readOptionalValue(TLength::kMillimetres, "PPM", target->getPPM());
		const TLength* targetHeight = readOptionalValue(TLength::kMetres, "TH", target->getTargetHeight());
		const TLength* targetHeightSigma = readOptionalValue(TLength::kMillimetres, "THSE", target->getTargetHeightSigma());
		const TLength* targetCenteringSigma = readOptionalValue(TLength::kMillimetres, "TCSE", target->getTargetCenteringSigma());
	
		//read measurement comments, dB identifier, and measurement constant
		int id=-1;
		string comdb="";
		string EOLComments = "";
		char lineNum[9];
		string iLine, line, error;
		string headCommentLine = "";
		// set error message if there was a problem reading the measurement
		if (fStream->getError() != "")
		{
			_itoa(i, lineNum, 10);
			iLine = lineNum;
			line = "line number " + iLine + " : ";
			error = project->getError();
			error=error+ line + fStream->getError() + '\n';
			project->setError(error);
			fStream->setError("");
		}
		
		while (!(fStream->peek() == '\n'))
		{
			fStream->skipWhiteSpace();
			switch (fStream->peek())
			{
				case '$':
					readDBComments(	lineNum, iLine, line,
									i,
									id, comdb, EOLComments,
									project);
					break;

				case '#':
					readP100Comment(EOLComments);
					break;

				case '%':
					readP100Comment(EOLComments);
					break;

				default:
					// other characters to read on this line correspond to an error
					_itoa(i, lineNum, 10);
					iLine = lineNum;
					line = "line number " + iLine + " : ";
					error = project->getError();
					error=error+line+ "Too much information to read" + '\n';
					project->setError(error);
					while (!(fStream->peek() == '\n'))
					{
						fStream->readChar();
					}
					break;
			}
			fStream->skipWhiteSpace();
		}

		TSpatialDistMeasurement<T>* newMeas = new TSpatialDistMeasurement<T>(project->getDataSet()->getObservationsCount()++,
			point, target, obs, sigma, ppmE, targetHeight, targetHeightSigma, targetCenteringSigma);

		setComments(newMeas, headCommentLine, comdb);

		fStream->readLine();

		return newMeas;
	}

	template <typename T>
	void	writeResults(TLSCalcSpatialDistObservation<T> obsIt)
	{
		TAStreamFormatter*	stream = getStream();
		int					nameWidth = getNameWidth();
		int					obsWidth = getObsWidth();
		int					obsResWidth = getObsResWidth();
		int					lengthResidualPrecision = getLengthResidualPrecision();
		int					lengthPrecision =	getLengthPrecision();
		string				separator = getSeparator();

		//write Point 2
		(*stream).writeStringLeft(nameWidth, obsIt.getTgPointName());
		//write the observed distance
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<(obsIt.getCorrectedObsDist())<<(separator);

		//write the sigma
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(obsResWidth);
		stream->setPrecisionFormat(lengthResidualPrecision);
		(*stream)<<(obsIt.getSigmaAPriori())<<(separator);

		//write the estimated distance
		stream->setLengthUnits(TLength::kMetres);
		stream->setWidthFormat(obsWidth);
		stream->setPrecisionFormat(lengthPrecision);
		(*stream)<<(obsIt.getCorrectedEstDist())<<(separator);

		//write the offset (mm) after calculation
		stream->setWidthFormat(obsResWidth);
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setPrecisionFormat(lengthResidualPrecision);
		TLength residue = obsIt.getResidue();
		(*stream)<<(residue)<<(separator);

		//write the offset / sigma (TDouble (MM))
		stream->setPrecisionFormat(2);
		(*stream)<<(obsIt.getResDivSig())<<separator;

		//write the sensitivity (TReal (MM/CM))
		(*stream)<<(obsIt.getSensitivity())<<separator;

		//write the target's height (TLength (M))
		(*stream)<<(obsIt.getHPrism())<<separator;

		//write the distance cste (TLength (MM))
		stream->setLengthUnits(TLength::kMillimetres);
		stream->setWidthFormat(obsResWidth);
		(*stream)<<(obsIt.getEstValueForVariableDistConst())<<separator;

		//write the distance cste sigma (TLength (MM))
		(*stream)<<(obsIt.getEstSigmaValueForVariableDistConst())<<separator;

		if (obsIt.hasOwnInstrumentHeight())
		{
			//write Instrument's heigth ( (M) TLength)
			(*stream)<<(obsIt.getHInstrument()->getEstimatedValue())<<separator;
		}
		*stream << endl;
		(*stream).setDataSpacing();

		return;
	}

	void	writeResultsHeader(bool hasOwnInstrumentHeight);

	template <typename T>
	void	writeReliabilityData(TLSCalcSpatialDistObservation<T>& obs)
	{
		this->TLengthObsConverter::writeReliabilityData(	obs,
															obs.getTgPointName());
		return;
	}

	void	writeReliabilityHeader();

	template <typename T>
	void	writeMesData(TLSCalcSpatialDistObservation<T> obsIt)
	{	
		TAngle gis, noV0;
		formatMesLine(	obsIt.getId(),
						obsIt.getStPointName(),
						obsIt.getTgPointName(),
						"",
						gis,
						obsIt.getCorrectedObsDist(), 
						obsIt.getCorrectedEstDist(), 
						obsIt.getResidue(),
						obsIt.getSigmaAPriori(),
						noV0, 
						obsIt.getComment());
		return;
	}


};

/*@}*/

#endif // SU_TSPA_DIST_CONVERTER
