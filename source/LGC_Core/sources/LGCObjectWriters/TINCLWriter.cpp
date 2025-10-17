// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TINCLWriter.h"

#include <Global.h>
#include <TAMeas.h>

#include "LGCAdjustablePoint.h"
#include "RoundOfMeasurements.h"
#include "TAStreamFormatter.h"
#include "TObservationFormat.h"

TINCLWriter::TINCLWriter(TAStreamFormatter &stream, bool /*hist*/) : TObservationWriter(stream)
{
}

TINCLWriter::~TINCLWriter()
{
}

//------------------ Result header---------------------------------------------------------------------------
void TINCLWriter::writeINCLResultsHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(2);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "POSITION"); // Position of the scale
	(*stream).writeString(obsWidth, "OBSERVE"); // mesured offset
	(*stream).writeString(obsResWidth, "SIGMA"); // sigma DIST
	(*stream).writeString(obsWidth, "CALCULE"); // estimated offset
	(*stream).writeString(obsResWidth, "RESIDU"); // residual
	(*stream).writeString(obsResWidth, "RES/SIG"); // residual/sigma
	(*stream).writeString(nameWidth, "INCL ID"); // inclinometer ID
	(*stream).writeString(obsResWidth, "OBSE"); // OBSE value
	(*stream).writeString(obsResWidth, "PPM"); // PPM value
	(*stream).writeString(obsWidth, "AC"); // AC value
	(*stream).writeString(obsResWidth, "ACSE"); // ACSE Value
	(*stream).writeString(obsWidth, "RF"); // RF value
	(*stream).writeString(obsResWidth, "RFSE"); // RFSE value
	if (obsIdWidth != 0) (*stream).writeString(obsIdWidth, "ID"); // Observation identifier
	(*stream) << "\n";

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, ""); // POSITION
	(*stream).writeString(obsWidth, "(GON)"); // OBSERVED VALUE
	(*stream).writeString(obsResWidth, "(CC)"); // sigma observed value
	(*stream).writeString(obsWidth, "(GON)"); // estimated offset
	(*stream).writeString(obsResWidth, "(CC)"); // residual
	(*stream).writeString(obsResWidth, ""); // residual/sigma
	(*stream).writeString(nameWidth, ""); // inclinometer ID
	(*stream).writeString(obsResWidth, "(CC)"); // OBSE
	(*stream).writeString(obsResWidth, ""); // PPM
	(*stream).writeString(obsWidth, "(GON)"); // AC
	(*stream).writeString(obsResWidth, "(CC)"); // ACSE
	(*stream).writeString(obsWidth, "(GON)"); // RF
	(*stream).writeString(obsResWidth, "(CC)"); // RFSE
	(*stream) << "\n";
}

//------------------ Result data---------------------------------------------------------------------------
void TINCLWriter::writeINCLYResults(const TINCLYROM &inclyrom)
{
	writeINCLResultsHelper(inclyrom.measINCLY, "INCLY", TALGCObjectWriter::kINCLY);
}

void TINCLWriter::writeROLLYResults(const TROLLYROM &rollyrom)
{
	writeINCLResultsHelper(rollyrom.measROLLY, "ROLLY", TALGCObjectWriter::kROLLY);
}

/*
 * Common INCL Results Helper
 * 
 * Unified template function that handles both INCLY and ROLLY results output.
 * Eliminates code duplication by accepting the measurement list and type-specific parameters.
 * 
 * @param measurements: List of measurements (INCLY or ROLLY)
 * @param sectionTitle: Title to write in output ("INCLY" or "ROLLY")
 * @param obsType: Observation type identifier for descriptions
 */
template<typename MeasurementList>
void TINCLWriter::writeINCLResultsHelper(const MeasurementList& measurements, const char* sectionTitle, TALGCObjectWriter::ELGCObservations obsType)
{
	// Get output stream and configure formatting parameters
	TAStreamFormatter *stream = getStream();
	
	// Configure column widths for consistent tabular output
	int nameWidth = getNameWidth();           // Width for target names and IDs
	int obsWidth = getObsWidth();             // Width for observed/estimated values
	int obsResWidth = getObsResWidth();       // Width for residual and uncertainty values
	int obsIdWidth = getObsIdWidth();         // Width for observation identifiers
	
	// Configure precision for different value types
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);  // Precision for residuals (reduced by 4 for readability)
	int anglePrecision = getAnglePrecision();                               // Precision for main angle values
	
	// Get indentation for consistent formatting
	std::string TABs = stream->getCurrSpaceExtended(2);

	// Write section header
	(*stream) << "\n";
	(*stream) << TABs << sectionTitle << "\n";

	// Write observation summary and column headers
	this->writeObsTitle(TABs + this->getObsDescriptionFR(obsType), (int)measurements.size());
	writeINCLResultsHeader(); // Write the title line for the observations

	// Process each measurement in the list
	for (auto const &measurement : measurements)
	{
		(*stream) << TABs;  // Apply consistent indentation
		
		// Column 1: TARGET POSITION - Name of the measured target point
		(*stream).writeStringLeft(nameWidth, measurement.targetPos->getName());

		// Column 2: OBSERVED ANGLE - Raw measured angle converted from radians to gon
		(*stream).writeDouble(obsWidth, anglePrecision, measurement.getAngle().getGonsValue());

		// Column 3: SIGMA ANGLE - Combined uncertainty (OBSE + PPM) in centesimal seconds [cc]
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.target.sigmaCombinedAngle.getSignedCCValue());

		// Column 4: ESTIMATED ANGLE - Adjusted angle (observed + residual) in gon
		(*stream).writeDouble(obsWidth, anglePrecision, (measurement.getAngle() + measurement.getAngleResidual()).getGonsValue());

		// Column 5: RESIDUAL - Difference between observed and estimated angles in cc
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.getAngleResidual().getSignedCCValue());

		// Column 6: RESIDUAL/SIGMA - Quality indicator (residual normalized by uncertainty), unitless
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.getAngleResidual().getRadiansValue() / measurement.target.sigmaCombinedAngle.getRadiansValue());

		// Column 7: SCALE ID - Identifier for the measurement scale/frame
		(*stream).writeString(nameWidth, measurement.target.ID);

		// Column 8: OBSE - Observation standard error in cc (instrument precision)
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.target.sigmaAngl.getSignedCCValue());

		// Column 9: PPM - Parts per million uncertainty in microRadians (scale factor uncertainty)
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.target.sigmaPpm.getMicroRadiansValue());

		// Column 10: AC - Angle correction value in gon (systematic correction applied)
		(*stream).writeDouble(obsWidth, anglePrecision, measurement.target.angleCorrectionValue.getGonsValue());

		// Column 11: ACSE - Standard error of angle correction in cc (uncertainty in correction)
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.target.sigmaCorrectionValue.getSignedCCValue());

		// Column 12: RF - Reference angle correction in gon (reference system correction)
		(*stream).writeDouble(obsWidth, anglePrecision, measurement.target.refAngleCorrectionValue.getGonsValue());

		// Column 13: RFSE - Standard error of reference correction in cc (uncertainty in reference correction)
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.target.refSigmaCorrectionValue.getSignedCCValue());
	
		// Column 14: OBSERVATION ID - Unique identifier for this measurement
		(*stream).writeString(obsIdWidth, measurement.obsID);

		// End of measurement row
		(*stream) << "\n";
	}
	
	// Add final spacing for readability
	(*stream) << "\n";
}

/*
 * ROLLY Simulation Results Writer
 * 
 * Generates simulation output reports for ROLLY inclinometer measurements. This function
 * is used during simulation runs to output predicted measurement values, quality metrics,
 * and statistical summaries without performing actual least squares adjustment.
 * 
 * The simulation output provides:
 * - Predicted measurement values based on current parameter estimates
 * - Quality indicators and uncertainty projections
 * - Statistical summaries for simulation validation and analysis
 * - Comparison data for simulation vs. actual measurement scenarios
 * 
 * @param rollyrom: ROLLY Round of Measurements object containing simulation data,
 *                   predicted values, and quality parameters for simulation analysis
 * 
 * @note This function is distinct from writeROLLYResults() as it outputs simulation
 *       predictions rather than actual measurement results. It uses the same formatting
 *       structure for consistency but focuses on simulated measurement scenarios.
 */
void TINCLWriter::writeROLLYSIMUResults(const TROLLYROM &rollyrom)
{
	// Get output stream and configure formatting parameters
	TAStreamFormatter *stream = getStream();
	
	// Get separator character for consistent formatting (typically space or tab)
	std::string separator = getSeparator();
	
	// Configure indentation for simulation output formatting
	std::string TABs = stream->getCurrSpaceExtended(1);

	// Write observation title with measurement count for simulation context
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kROLLY), (int)rollyrom.measROLLY.size());
	
	// Write simulation section header
	(*stream) << TABs << "ROLLY" << "\n";

	// Generate comprehensive simulation results summary with enhanced indentation
	// This includes predicted values, quality metrics, and statistical summaries
	writeAngleResultsSummary(rollyrom.getROLLYObsSummary(), stream->getCurrSpaceExtended(2));
}

//------------------ Simu data--------------------------------------------------------------------------
void TINCLWriter::writeINCLYSIMUResults(const TINCLYROM &inclyrom)
{
	TAStreamFormatter *stream = getStream();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kINCLY), (int)inclyrom.measINCLY.size());
	(*stream) << TABs << "INCLY" << "\n";

	writeAngleResultsSummary(inclyrom.getINCLYObsSummary(), stream->getCurrSpaceExtended(2));
}

//------------------ INCL Reliability header (common for INCLY and ROLLY)----------------------------------------------------------------------
void TINCLWriter::writeINCLReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("FRAME", "STATION", "", "OBSERVATION", "GON", "CC");
	return;
}

//------------------ Reliability data----------------------------------------------------------------------
void TINCLWriter::writeINCLYReliabilityData(const TINCLYROM &inclyrom, const TLGCStatistic &stat, const std::list<TINCLY> &measINCLY)
{
	writeINCLReliabilityDataHelper(inclyrom, stat, measINCLY);
}


void TINCLWriter::writeROLLYReliabilityData(const TROLLYROM &rollyrom, const TLGCStatistic &stat, const std::list<TROLLY> &measROLLY)
{
	writeINCLReliabilityDataHelper(rollyrom, stat, measROLLY);
}

/*
 * Common INCL Reliability Data Helper
 * 
 * Unified template function that handles both INCLY and ROLLY reliability data output.

 * @param rom: Round of measurements object (TINCLYROM or TROLLYROM)
 * @param stat: LGC Statistics object containing reliability metrics and quality indicators
 * @param measurements: List of measurements (INCLY or ROLLY) to process for reliability analysis
 */
template<typename ROMType, typename MeasurementList>
void TINCLWriter::writeINCLReliabilityDataHelper(const ROMType& rom, const TLGCStatistic& stat, const MeasurementList& measurements)
{
	// Get output stream and configure formatting parameters
	TAStreamFormatter *stream = getStream();
	
	// Configure column widths for consistent tabular output
	int nameWidth = getNameWidth();           // Width for frame names, station points, and identifiers
	int obsWidth = getObsWidth();             // Width for observed angle values
	int obsResWidth = getObsResWidth();       // Width for uncertainty and residual values
	
	// Configure precision for different value types
	int anglePrecision = getAnglePrecision();                               // Precision for main angle values
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);  // Precision for residuals (reduced by 4 for readability)

	// Process each measurement in the list for reliability analysis
	for (auto const &measurement : measurements)
	{
		// Get observation index to retrieve corresponding statistical values from the statistic vector
		int index = measurement.getFirstObservationIndex();

		// Column 1: FRAME NAME - Name of the frame containing the measurement
		(*stream).writeStringLeft(nameWidth, rom.positionInTree.node->data->frame.getName());

		// Column 2: STATION POINT - Name of the measurement station/target point
		(*stream).writeStringLeft(nameWidth, measurement.targetPos->getName());
		
		// Column 3: POINT 3 - Reserved for future use (currently empty)
		(*stream).writeStringLeft(nameWidth, "");

		// Column 4: OBSERVED ANGLE - Raw measured angle converted from radians to gon
		(*stream).writeDouble(obsWidth, anglePrecision, measurement.getAngle().getGonsValue());
		
		// Column 5: STANDARD DEVIATION - Combined uncertainty (OBSE + PPM) in centesimal seconds [cc]
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.target.sigmaCombinedAngle.getSignedCCValue());
		
		// Column 6: RESIDUAL - Difference between observed and estimated angles in cc
		(*stream).writeDouble(obsResWidth, angleResPrecision, measurement.getAngleResidual().getSignedCCValue());

		// Generate M-estimator reliability metrics for this observation
		// This includes statistical quality indicators, outlier detection metrics,
		// and reliability scores for measurement validation
		writeReliabilityMM(index, stat);
	}
}

//------------------ Synthesis header------------------------------------------------------------------------
void TINCLWriter::writeINCLSynthesisHeader()
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	// int				obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	std::string separator = getSeparator();
	std::string TABs = stream->getCurrSpaceExtended(1);

	////////////////////////////////////////////////////////////
	// First line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "STATION"); // plane name
	(*stream).writeString(obsResWidth, "RES_MAX"); // residi max
	(*stream).writeString(obsResWidth, "RES_MIN"); // residu min
	(*stream).writeString(obsResWidth, "RES_MOY"); // residu mean
	(*stream).writeString(obsResWidth, "ECART_TYPE"); // ecart type
	(*stream) << "\n";

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");

	(*stream) << "\n";
}

//------------------ Synthesis data--------------------------------------------------------------------------

void TINCLWriter::writeINCLYResultsSynthesis(std::list<const TLGCObsSummary *> &inclysum)
{
	writeINCLResultsSynthesisHelper(inclysum);
}

void TINCLWriter::writeROLLYResultsSynthesis(std::list<const TLGCObsSummary *> &rollysum)
{
	writeINCLResultsSynthesisHelper(rollysum);
}

/*
 * Common INCL Results Synthesis Helper
 * 
 * Unified function that handles both INCLY and ROLLY results synthesis output.
 * Formats and outputs a summary table containing statistics including:
 * - Reference point names
 * - Maximum/minimum residuals in centesimal seconds (CC)
 * - Mean residuals and standard deviations
 * 
 * @param summaries: List of observation summaries containing measurement statistics
 */
void TINCLWriter::writeINCLResultsSynthesisHelper(std::list<const TLGCObsSummary *> &summaries)
{
	// Get output stream and configure formatting parameters
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();           // Width for reference point names
	int obsResWidth = getObsResWidth();       // Width for statistical values
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);  // Precision for residuals (reduced by 4 for readability)
	std::string TABs = stream->getCurrSpaceExtended(1);  // Indentation for consistent formatting

	// Process each observation summary in the list
	for (auto const &summary : summaries)
	{
		(*stream) << TABs;  // Apply consistent indentation
		
		// Column 1: REFERENCE POINT - Name/identifier of the observation point
		(*stream).writeStringLeft(nameWidth, summary->getObsText());
		
		// Column 2: MAXIMUM RESIDUAL - Largest residual value in CC
		(*stream).writeDouble(obsResWidth, angleResPrecision, summary->getResMax());
		
		// Column 3: MINIMUM RESIDUAL - Smallest residual value in CC  
		(*stream).writeDouble(obsResWidth, angleResPrecision, summary->getResMin());
		
		// Column 4: MEAN RESIDUAL - Average residual value in CC
		(*stream).writeDouble(obsResWidth, angleResPrecision, summary->getMean());
		
		// Column 5: STANDARD DEVIATION - Statistical spread of residuals in CC
		(*stream).writeDouble(obsResWidth, angleResPrecision, summary->getStdev());
		
		// End of summary row
		(*stream) << "\n";
	}
}