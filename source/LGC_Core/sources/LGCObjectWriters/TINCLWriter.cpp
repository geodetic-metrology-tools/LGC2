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
	(*stream) << endl;

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
	(*stream) << endl;
}

//------------------ Result data---------------------------------------------------------------------------
void TINCLWriter::writeINCLYResults(const TINCLYROM &inclyrom)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int obsIdWidth = getObsIdWidth();
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	int anglePrecision = getAnglePrecision();
	std::string TABs = stream->getCurrSpaceExtended(2);

	// write header
	(*stream) << endl;
	(*stream) << TABs << "INCLY" << endl;

	// data summury
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kINCLY), (int)inclyrom.measINCLY.size());
	writeINCLResultsHeader(); // write the title line for the observations

	for (auto const &ItINCLY : inclyrom.measINCLY)
	{
		(*stream) << TABs;
		// write TARGET POSITION
		(*stream).writeStringLeft(nameWidth, ItINCLY.targetPos->getName());

		// write the observed angle
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.getAngle().getGonsValue()); // Output value in gradians [gon], stored in [rad]

		// write the sigma angle
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCombinedAngle.getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the estimated angle
		(*stream).writeDouble(obsWidth, anglePrecision, (ItINCLY.getAngle() + ItINCLY.getAngleResidual()).getGonsValue()); // Output value in gradians [gon], stored in [rad]

		// write the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the residual/sigma
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getRadiansValue() / ItINCLY.target.sigmaCombinedAngle.getRadiansValue()); // Output value unitless

		// write the scale ID
		(*stream).writeString(nameWidth, ItINCLY.target.ID);

		// write the OBSE
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaAngl.getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the PPM
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaPpm.getMicroRadiansValue()); // Output value in microradians [urad], stored in [rad]

		// write the AC
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.target.angleCorrectionValue.getGonsValue()); // Output value in gon [gon], stored in [rad]

		// write the ACSE
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCorrectionValue.getSignedCCValue()); // Output value in cc [cc], stored in [rad]

		// write the RF
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.target.refAngleCorrectionValue.getGonsValue()); // Output value in gon [gon], stored in [rad]

		// write the RFSE
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.refSigmaCorrectionValue.getSignedCCValue()); // Output value in cc [cc], stored in [rad]
	
		// Write the observation identifier
		(*stream).writeString(obsIdWidth, ItINCLY.obsID);

		(*stream) << endl;
	}
	(*stream) << endl;
}

/*
 * ROLLY Results Data Writer
 * 
 * Generates comprehensive output reports for ROLLY inclinometer measurements after
 * least squares adjustment. This function formats and outputs measurement results,
 * residuals, quality indicators, and correction parameters in a structured tabular format.
 * 
 * The output includes:
 * - Observed vs. estimated angles with residuals
 * - Quality metrics (sigma values, residual/sigma ratios)
 * - Correction parameters (AC, RF) and their uncertainties
 * - Instrument parameters (OBSE, PPM) for measurement quality assessment
 * 
 * @param rollyrom: ROLLY Round of Measurements object containing all measurement data,
 *                   residuals, and quality parameters for a complete measurement session
 * 
 * @note This function follows the same output format as INCLY measurements for consistency.
 *       All angular values are converted from internal radians to appropriate output units
 *       (gon for angles, cc for uncertainties, microRadians for PPM).
 */
void TINCLWriter::writeROLLYResults(const TROLLYROM &rollyrom)
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
	(*stream) << endl;
	(*stream) << TABs << "ROLLY" << endl;

	// Write observation summary and column headers
	this->writeObsTitle(TABs + this->getObsDescriptionFR(TALGCObjectWriter::kROLLY), (int)rollyrom.measROLLY.size());
	writeINCLResultsHeader(); // Use same header format as INCLY for consistency

	// Process each ROLLY measurement in the round
	for (auto const &ItROLLY : rollyrom.measROLLY)
	{
		(*stream) << TABs;  // Apply consistent indentation
		
		// Column 1: TARGET POSITION - Name of the measured target point
		(*stream).writeStringLeft(nameWidth, ItROLLY.targetPos->getName());

		// Column 2: OBSERVED ANGLE - Raw measured angle converted from radians to gon
		(*stream).writeDouble(obsWidth, anglePrecision, ItROLLY.getAngle().getGonsValue());

		// Column 3: SIGMA ANGLE - Combined uncertainty (OBSE + PPM) in centesimal seconds [cc]
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.target.sigmaCombinedAngle.getSignedCCValue());

		// Column 4: ESTIMATED ANGLE - Adjusted angle (observed + residual) in gon
		(*stream).writeDouble(obsWidth, anglePrecision, (ItROLLY.getAngle() + ItROLLY.getAngleResidual()).getGonsValue());

		// Column 5: RESIDUAL - Difference between observed and estimated angles in cc
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.getAngleResidual().getSignedCCValue());

		// Column 6: RESIDUAL/SIGMA - Quality indicator (residual normalized by uncertainty), unitless
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.getAngleResidual().getRadiansValue() / ItROLLY.target.sigmaCombinedAngle.getRadiansValue());

		// Column 7: SCALE ID - Identifier for the measurement scale/frame
		(*stream).writeString(nameWidth, ItROLLY.target.ID);

		// Column 8: OBSE - Observation standard error in cc (instrument precision)
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.target.sigmaAngl.getSignedCCValue());

		// Column 9: PPM - Parts per million uncertainty in microRadians (scale factor uncertainty)
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.target.sigmaPpm.getMicroRadiansValue());

		// Column 10: AC - Angle correction value in gon (systematic correction applied)
		(*stream).writeDouble(obsWidth, anglePrecision, ItROLLY.target.angleCorrectionValue.getGonsValue());

		// Column 11: ACSE - Standard error of angle correction in cc (uncertainty in correction)
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.target.sigmaCorrectionValue.getSignedCCValue());

		// Column 12: RF - Reference angle correction in gon (reference system correction)
		(*stream).writeDouble(obsWidth, anglePrecision, ItROLLY.target.refAngleCorrectionValue.getGonsValue());

		// Column 13: RFSE - Standard error of reference correction in cc (uncertainty in reference correction)
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.target.refSigmaCorrectionValue.getSignedCCValue());
	
		// Column 14: OBSERVATION ID - Unique identifier for this measurement
		(*stream).writeString(obsIdWidth, ItROLLY.obsID);

		// End of measurement row
		(*stream) << endl;
	}
	
	// Add final spacing for readability
	(*stream) << endl;
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
	(*stream) << TABs << "ROLLY" << endl;

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
	(*stream) << TABs << "INCLY" << endl;

	writeAngleResultsSummary(inclyrom.getINCLYObsSummary(), stream->getCurrSpaceExtended(2));
}

//------------------ Reliability header----------------------------------------------------------------------
void TINCLWriter::writeINCLYReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("FRAME", "STATION", "", "OBSERVATION", "GON", "CC");
	return;
}

//------------------ ROLLY Reliability header----------------------------------------------------------------------
void TINCLWriter::writeROLLYReliabilityHeader()
{
	this->TObservationWriter::writeReliabilityHeader("FRAME", "STATION", "", "OBSERVATION", "GON", "CC");
	return;
}

//------------------ Reliability data----------------------------------------------------------------------
void TINCLWriter::writeINCLYReliabilityData(const TINCLYROM &inclyrom, const TLGCStatistic &stat, const std::list<TINCLY> &measINCLY)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsWidth = getObsWidth();
	int obsResWidth = getObsResWidth();
	int anglePrecision = getAnglePrecision();
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);

	// For each INCLY measurement of the station
	for (auto const &ItINCLY : measINCLY)
	{
		// Observation index to take the right value in the statistic vector
		int index = ItINCLY.getFirstObservationIndex();

		// Get the frame name
		(*stream).writeStringLeft(nameWidth, inclyrom.positionInTree.node->data->frame.getName());

		// get Station point
		(*stream).writeStringLeft(nameWidth, ItINCLY.targetPos->getName());
		// get Point 3
		(*stream).writeStringLeft(nameWidth, "");

		// get the observed angle
		(*stream).writeDouble(obsWidth, anglePrecision, ItINCLY.getAngle().getGonsValue());
		// get the standard deviation
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.target.sigmaCombinedAngle.getSignedCCValue());
		// get the residual
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY.getAngleResidual().getSignedCCValue());

		writeReliabilityMM(index, stat);
	}
	return;
}

/*
 * ROLLY Reliability Data Writer
 * 
 * Generates reliability analysis reports for ROLLY inclinometer measurements after
 * least squares adjustment. This function outputs comprehensive quality assessment
 * data including measurement statistics, residuals, uncertainties, and reliability
 * metrics for each ROLLY measurement in a station.
 * 
 * The reliability output provides:
 * - Frame and station identification for measurement context
 * - Observed angles with their uncertainties and residuals
 * - Statistical reliability metrics (MM - M-estimator statistics)
 * - Quality indicators for measurement validation and outlier detection
 * - Data for reliability analysis and measurement quality assessment
 * 
 * @param rollyrom: ROLLY Round of Measurements object containing measurement data,
 *                   frame information, and tree position for context
 * @param stat: LGC Statistics object containing computed reliability metrics,
 *              M-estimator values, and quality indicators for each observation
 * @param measROLLY: List of ROLLY measurements to process for reliability analysis
 * 
 * @note This function is part of the reliability analysis system that helps identify
 *       potential outliers, assess measurement quality, and validate least squares
 *       adjustment results. It outputs data in a format suitable for statistical
 *       analysis and quality control procedures.
 */
void TINCLWriter::writeROLLYReliabilityData(const TROLLYROM &rollyrom, const TLGCStatistic &stat, const std::list<TROLLY> &measROLLY)
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

	// Process each ROLLY measurement in the station for reliability analysis
	for (auto const &ItROLLY : measROLLY)
	{
		// Get observation index to retrieve corresponding statistical values from the statistic vector
		int index = ItROLLY.getFirstObservationIndex();

		// Column 1: FRAME NAME - Name of the frame containing the measurement
		(*stream).writeStringLeft(nameWidth, rollyrom.positionInTree.node->data->frame.getName());

		// Column 2: STATION POINT - Name of the measurement station/target point
		(*stream).writeStringLeft(nameWidth, ItROLLY.targetPos->getName());
		
		// Column 3: POINT 3 - Reserved for future use (currently empty)
		(*stream).writeStringLeft(nameWidth, "");

		// Column 4: OBSERVED ANGLE - Raw measured angle converted from radians to gon
		(*stream).writeDouble(obsWidth, anglePrecision, ItROLLY.getAngle().getGonsValue());
		
		// Column 5: STANDARD DEVIATION - Combined uncertainty (OBSE + PPM) in centesimal seconds [cc]
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.target.sigmaCombinedAngle.getSignedCCValue());
		
		// Column 6: RESIDUAL - Difference between observed and estimated angles in cc
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY.getAngleResidual().getSignedCCValue());

		// Generate M-estimator reliability metrics for this observation
		// This includes statistical quality indicators, outlier detection metrics,
		// and reliability scores for measurement validation
		writeReliabilityMM(index, stat);
	}
	
	// Reliability analysis complete for this station
	return;
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
	(*stream) << endl;

	///////////////////////////////////////////////////////////////////////////////////
	// second line
	(*stream) << TABs;
	(*stream).writeStringLeft(nameWidth, "");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");
	(*stream).writeString(obsResWidth, "(CC)");

	(*stream) << endl;
}

//------------------ Synthesis data--------------------------------------------------------------------------

void TINCLWriter::writeINCLYResultsSynthesis(std::list<const TLGCObsSummary *> &inclysum)
{
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth();
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	std::string TABs = stream->getCurrSpaceExtended(1);

	for (auto const &ItINCLY : inclysum)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, ItINCLY->getObsText()); // Reference point
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getResMax()); // residu max
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getResMin()); // residu min
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getMean()); // residu moy
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItINCLY->getStdev()); // ecart type
		(*stream) << endl;
	}
}

/*
 * Writes synthesis results for ROLLY (roll Y-axis) measurements
 * 
 * Formats and outputs a summary table containing statistics for ROLLY measurements
 * including maximum/minimum residuals, mean, and standard deviation.
 * Values are written in centesimal seconds (CC) units.
 *
 * @param rollysum List of observation summaries containing ROLLY measurement statistics
 */
void TINCLWriter::writeROLLYResultsSynthesis(std::list<const TLGCObsSummary *> &rollysum)
{
	// Get output stream and configure formatting parameters
	TAStreamFormatter *stream = getStream();
	int nameWidth = getNameWidth();
	int obsResWidth = getObsResWidth(); 
	int angleResPrecision = std::max(getAngleResidualPrecision() - 4, 0);
	std::string TABs = stream->getCurrSpaceExtended(1);

	// Write statistics for each ROLLY observation summary
	for (auto const &ItROLLY : rollysum)
	{
		(*stream) << TABs;
		(*stream).writeStringLeft(nameWidth, ItROLLY->getObsText()); // Write reference point name
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY->getResMax()); // Maximum residual in CC
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY->getResMin()); // Minimum residual in CC  
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY->getMean());   // Mean residual in CC
		(*stream).writeDouble(obsResWidth, angleResPrecision, ItROLLY->getStdev()); // Standard deviation in CC
		(*stream) << endl;
	}
}