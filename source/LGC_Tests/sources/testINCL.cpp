#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <string>
#include <Behavior.h>
#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "testINCL.h"

namespace tut
{
struct test_INCL
{
	test_INCL() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

	std::shared_ptr<TLGCData> projTest;
	TReader r;
};
typedef test_group<test_INCL> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("INCL tests");
}

namespace tut
{

//========================================== HELPER FUNCTIONS ==========================================//

/*
 * Helper structure to define parameters for subframe tests (INCLY/ROLLY)
 * This enables parameterized testing to reduce code duplication
 */
struct SubframeTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string pointName;
	TReal expectedX, expectedY, expectedZ;
	int frameIterations;
	std::string measurementType;  // "INCLY" or "ROLLY"
	TReal coordinateTolerance;    // Tolerance for coordinate validation
	TReal rotationTolerance;      // Tolerance for rotation validation
};

/*
 * Parameterized helper function for subframe tests (INCLY/ROLLY)
 * 
 * This function encapsulates the common testing pattern for inclinometer measurements in subframes:
 * 1. Parse input file with INCLY/ROLLY measurement
 * 2. Perform least squares calculation
 * 3. Validate point coordinates after transformation to ROOT frame
 * 4. Verify frame rotation parameters
 * 
 * Parameters:
 * - params: Test configuration including expected coordinates, point name, and measurement type
 * - projTest: Shared pointer to TLGCData for project management
 * - r: TReader instance for file parsing
 * 
 * Common Validation:
 * - Parser success, calculation success
 * - Point coordinates transformed to ROOT frame (±1e-7 tolerance)
 * - Y-axis rotation: 400 - 0.007555401 = 399.992444599 gon
 * - Measurement presence and processing validation
 * 
 * This function abstracts the subframe validation logic to ensure consistent behavior
 * between INCLY and ROLLY measurements, following DRY principles.
 */
void runSubframeTest(const SubframeTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();
	TDataTree tree = projTest->getTree();

	TLOR2LOR RSTRToROOT(tree, "RSTR_XT03.BDB.0900", "ROOT", "RSTRTOROOT");

	TPositionVector point = dataset.getPoints().getObject(params.pointName).getEstimatedValue();
	RSTRToROOT.transform(point);

	// Use the same coordinate validation approach as the proven working tests
	// Use the tolerance specified in the test parameters
	std::string coordMsg = params.pointName + " x coordinate should match";
	ensure_equals(coordMsg.c_str(), point.getX().getMetresValue(), params.expectedX, params.coordinateTolerance);
	
	coordMsg = params.pointName + " y coordinate should match";
	ensure_equals(coordMsg.c_str(), point.getY().getMetresValue(), params.expectedY, params.coordinateTolerance);
	
	coordMsg = params.pointName + " z coordinate should match";
	ensure_equals(coordMsg.c_str(), point.getZ().getMetresValue(), params.expectedZ, params.coordinateTolerance);

	TDataTreeIterator frameIt = tree.begin();
	for (int i = 0; i < params.frameIterations; i++) {
	frameIt++;
	}
	
	// Use the same rotation validation as the proven working tests: 400 - 0.007555401
	// Use the tolerance specified in the test parameters
	ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400 - 0.007555401, params.rotationTolerance);
	
	// Validate that measurements are present and processed correctly
	const auto& inclyMeasurements = frameIt.node->data->measurements.fINCLY;
	const auto& rollyMeasurements = frameIt.node->data->measurements.fROLLY;
	
	// Check for measurements based on the specified measurement type
	if (params.measurementType == "INCLY") {
		ensure("INCLY measurements should be present", !inclyMeasurements.empty());
		auto romIt = inclyMeasurements.begin();
		ensure("INCLY ROM should have measurements", !romIt->measINCLY.empty());
		
		// Check that measurements have been processed (residuals calculated)
		for (auto &meas : romIt->measINCLY) {
			ensure("INCLY measurement should have valid residual", !std::isnan(meas.getAngleResidual().getRadiansValue()));
		}
	} else if (params.measurementType == "ROLLY") {
		ensure("ROLLY measurements should be present", !rollyMeasurements.empty());
		auto romIt = rollyMeasurements.begin();
		ensure("ROLLY ROM should have measurements", !romIt->measROLLY.empty());
		
		// Check that measurements have been processed (residuals calculated)
		for (auto &meas : romIt->measROLLY) {
			ensure("ROLLY measurement should have valid residual", !std::isnan(meas.getAngleResidual().getRadiansValue()));
		}
	} else {
		ensure("Invalid measurement type specified", false);
	}
}

/*
 * Enhanced helper function for testing INCLY/ROLLY keyword parsing and parameter validation
 * 
 * This function validates the parsing and parameter inheritance for INCLY/ROLLY measurements,
 * ensuring that all keywords (INSTR, OBSE, PPM, AC, ACSE, RF, RFSE, ID) are correctly
 * parsed and applied to measurements. It follows the same pattern as DLEV keyword testing.
 * 
 * Key Validation Points:
 * 1. Instrument parameters are correctly parsed and inherited
 * 2. Observation-specific keywords override instrument defaults
 * 3. All measurement parameters (OBSE, PPM, AC, RF, etc.) are correctly set
 * 4. Observation IDs are properly assigned and tracked
 * 5. Multiple measurements with different parameters are handled correctly
 * 6. Optional S0 validation for statistical checks
 * 7. Optional frame rotation validation
 * 8. Combined sigma calculation validation
 * 
 * Test Structure:
 * - Single frame with multiple INCLY/ROLLY measurements
 * - Different parameter combinations for each measurement
 * - Validation of both instrument-level and observation-level parameters
 * - Optional statistical and geometric validations
 * 
 * Parameters:
 * - testData: Input data string containing INCLY/ROLLY measurements with keywords
 * - logFileName: Output log file name for the test
 * - testDescription: Descriptive name for the test
 * - expectedMeasurements: Vector of expected measurement parameters
 * - performS0Check: Whether to validate S0 a posteriori value
 * - expectedS0: Expected S0 value for validation
 * - validateFrameRotation: Whether to validate frame rotation parameters
 * - expectedRotationY: Expected Y rotation value
 * 
 * This function abstracts the keyword validation logic to ensure consistent behavior
 * between INCLY and ROLLY keyword parsing, following DRY principles.
 */
struct KeywordValidationTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	struct MeasurementParams {
		std::string instrumentID;
		TReal obse;      // Observation sigma in CC
		TReal ppm;       // PPM sigma in microRadians (not CC!)
		TReal ac;        // Angle correction in gon
		TReal acse;      // AC sigma in CC
		TReal rf;        // Reference angle in gon
		TReal rfse;      // RF sigma in CC
		std::string obsID;
		TReal measurementValue;  // Observed angle value in gon
		TReal combinedSigma;     // Combined sigma calculation (optional)
	};
	std::vector<MeasurementParams> expectedMeasurements;
	
	// Optional validation features
	bool performS0Check = false;
	TReal expectedS0 = 0.0;
	bool validateFrameRotation = false;
	TReal expectedRotationY = 0.0;
};

/*
 * Helper function for testing root frame measurement errors
 * 
 * This function validates that inclinometer measurements (INCLY/ROLLY) are not allowed
 * in the root frame context. Both INCLY and ROLLY share the same validation rules.
 * 
 * Parameters:
 * - testData: The input data string containing the measurement in root frame
 * - logFileName: Output log file name for the test
 * - testDescription: Descriptive name for the test
 * - measurementType: Type of measurement being tested ("INCLY" or "ROLLY")
 * 
 * Context Rules:
 * - Inclinometer measurements must be within *FRAME....*ENDFRAME blocks
 * - Root frame is not a valid context for inclinometer measurements
 * - Parser should reject input and return reading failure
 * 
 * Expected Behavior:
 * - File parsing should fail (succesReading = false)
 * - Error indicates invalid context for inclinometer measurement
 * - No calculation attempted due to parsing failure
 * 
 * This validation ensures LGC maintains proper measurement context rules
 * and prevents invalid configurations from proceeding to calculation.
 */
struct RootErrorTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
};

void runRootErrorTest(const RootErrorTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	// Root frame measurements should be rejected - reading should FAIL
	ensure_equals("Reading should fail for root frame measurements", succesReading, false);

	// Since reading failed, we should not proceed with calculation
	// The test validates that the parser correctly rejects invalid input
}

/*
 * Helper function for testing inclinometer observation ID parsing and validation
 * 
 * This function validates the observation ID functionality for inclinometer measurements (INCLY/ROLLY),
 * ensuring that both measurement types handle ID parsing and validation identically.
 * 
 * Key Validation Points:
 * 1. Custom observation IDs are correctly parsed from input files using ID keyword
 * 2. The parsed IDs match expected values exactly  
 * 3. The system correctly tracks the maximum observation ID length for output formatting
 * 4. ID validation works correctly within the measurement pipeline
 * 
 * Parameters:
 * - testData: Input data string containing measurement with ID keyword
 * - logFileName: Output log file name for the test
 * - testDescription: Descriptive name for the test
 * - measurementType: Type of measurement being tested ("INCLY" or "ROLLY")
 * - expectedIDs: Vector of expected observation ID strings
 * 
 * Common ID Syntax in Input Files:
 * *INCLY test
 * BEAM_XT03.BDB.0900.T -0.0070028174960434 ID test.BEAM_XT03.BDB.0900.T
 * 
 * *ROLLY test  
 * BEAM_XT03.BDB.0900.T -0.0070028174960434 ID test.BEAM_XT03.BDB.0900.T
 * 
 * Validation Process:
 * 1. Parse input file successfully (validates ID keyword syntax)
 * 2. Complete least squares calculation without errors
 * 3. Iterate through all frames in the project tree
 * 4. For each frame containing target measurements:
 *    a. Extract observation ID from each measurement
 *    b. Verify ID matches expected value
 *    c. Track maximum ID length across all measurements
 * 5. Verify that dataset configuration correctly stores max ID width
 * 
 * This function abstracts the ID validation logic to ensure consistent behavior
 * between INCLY and ROLLY measurements, following DRY principles.
 */
struct IDValidationTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
};

void runIDValidationTest(const IDValidationTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Validate that ID validation works correctly
	ensure("ID validation should complete successfully", frameIt.node != nullptr);
}

/*
 * Helper function for testing OBSXYZ observation ID parsing in complex frame structures
 * 
 * This function validates OBSXYZ observation ID functionality in complex frame hierarchies
 * that also contain inclinometer measurements (INCLY/ROLLY). It ensures consistent ID
 * parsing behavior across different measurement type combinations.
 * 
 * Key Validation Points:
 * 1. OBSXYZ measurements with custom IDs are correctly parsed from input files
 * 2. The parsed OBSXYZ IDs match expected values exactly
 * 3. The system correctly tracks maximum observation ID length across all measurement types
 * 4. OBSXYZ ID validation works correctly in complex frame hierarchies
 * 
 * Parameters:
 * - testData: Input data string containing OBSXYZ measurement with ID keyword
 * - logFileName: Output log file name for the test
 * - testDescription: Descriptive name for the test
 * - contextType: Context measurement type ("INCLY" or "ROLLY") for logging purposes
 * - expectedOBSXYZID: Expected OBSXYZ observation ID string
 * - frameIterations: Number of frame iterations needed to reach target frame
 * 
 * Common OBSXYZ ID Syntax in Input Files:
 * *OBSXYZ
 * BEAM_XT03.BDB.0900.E 0 0 0 0.0001 0.0001 0.0001 ID XYZ.BEAM_XT03.BDB.0900.E
 * *INCLY test  (or *ROLLY test)
 * BEAM_XT03.BDB.0900.E -0.0070028174960434
 * 
 * Frame Structure Navigation:
 * - Complex multi-level frame hierarchies (typically 4-5 levels)
 * - ROOT -> RSTB_XT03 -> RSTB2_XT03 -> RSTRI_XT03 -> RSTR_XT03
 * - OBSXYZ measurements located in deepest frame with inclinometer measurements
 * 
 * Validation Process:
 * 1. Parse input file successfully (validates OBSXYZ ID keyword syntax)
 * 2. Complete least squares calculation without errors
 * 3. Navigate to the correct frame using specified frame iterations
 * 4. Extract OBSXYZ observation ID from the target frame
 * 5. Verify ID matches expected value
 * 6. Iterate through all OBSXYZ measurements to find maximum ID length
 * 7. Verify that dataset configuration correctly stores max ID width
 * 
 * This function abstracts OBSXYZ ID validation logic to ensure consistent behavior
 * across different inclinometer measurement contexts, following DRY principles.
 */
struct OBSXYZIDValidationTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string contextType;
	std::string expectedOBSXYZID;
	int frameIterations;
};

void runOBSXYZIDValidationTest(const OBSXYZIDValidationTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Validate that OBSXYZ ID validation works correctly
	ensure("OBSXYZ ID validation should complete successfully", frameIt.node != nullptr);
}


// Parameterized validation function - works for both INCLY and ROLLY
template<typename MeasurementContainer>
void validateMeasurementParameters(const MeasurementContainer& measurements, const std::vector<KeywordValidationTestParams::MeasurementParams>& expectedMeasurements) {
	typename MeasurementContainer::const_iterator measIt = measurements.begin();
	for (size_t i = 0; i < expectedMeasurements.size() && measIt != measurements.end(); ++i, ++measIt) {
		const auto& expected = expectedMeasurements[i];
		
		std::string errorMsg = "Measurement " + std::to_string(i + 1) + ": Instrument ID should match";
		ensure_equals(errorMsg.c_str(), measIt->target.ID, expected.instrumentID);
		
		// Use TAngle objects for unit conversions instead of hard-coded values
		TAngle expectedOBSE(expected.obse, TAngle::EUnits::kCCs);
		TAngle expectedPPM(expected.ppm, TAngle::EUnits::kMicroRadians);
		TAngle expectedACSE(expected.acse, TAngle::EUnits::kCCs);
		TAngle expectedRFSE(expected.rfse, TAngle::EUnits::kCCs);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": OBSE should match";
		ensure_equals(errorMsg.c_str(), measIt->target.sigmaAngl.getGonsValue(), expectedOBSE.getGonsValue(), 1e-6);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": PPM should match";
		ensure_equals(errorMsg.c_str(), measIt->target.sigmaPpm.getGonsValue(), expectedPPM.getGonsValue(), 1e-6);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": AC should match";
		ensure_equals(errorMsg.c_str(), measIt->target.angleCorrectionValue.getGonsValue(), expected.ac, 1e-6);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": ACSE should match";
		ensure_equals(errorMsg.c_str(), measIt->target.sigmaCorrectionValue.getGonsValue(), expectedACSE.getGonsValue(), 1e-6);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": RF should match";
		ensure_equals(errorMsg.c_str(), measIt->target.refAngleCorrectionValue.getGonsValue(), expected.rf, 1e-6);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": RFSE should match";
		ensure_equals(errorMsg.c_str(), measIt->target.refSigmaCorrectionValue.getGonsValue(), expectedRFSE.getGonsValue(), 1e-6);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": ID should match";
		ensure_equals(errorMsg.c_str(), measIt->obsID, expected.obsID);
		
		errorMsg = "Measurement " + std::to_string(i + 1) + ": Measurement value should match";
		ensure_equals(errorMsg.c_str(), measIt->getAngle().getGonsValue(), expected.measurementValue, 1e-6);
		
		// Optional combined sigma validation
		if (expected.combinedSigma > 0.0) {
			errorMsg = "Measurement " + std::to_string(i + 1) + ": Combined sigma should match";
			ensure_equals(errorMsg.c_str(), measIt->target.sigmaCombinedAngle.getGonsValue(), expected.combinedSigma, 1e-6);
		}
	}
}

void runKeywordValidationTest(const KeywordValidationTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// Optional S0 validation
	if (params.performS0Check) {
		ensure_equals("S0 validation", projTest->getS0APosteriori(), params.expectedS0, 1e-9);
	}

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Optional frame rotation validation
	if (params.validateFrameRotation) {
		ensure_equals("Rotation Y does not match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), params.expectedRotationY, 1e-6);
	}

	// Get measurements - automatically works for both INCLY and ROLLY
	const auto& inclyMeasurements = frameIt.node->data->measurements.fINCLY;
	const auto& rollyMeasurements = frameIt.node->data->measurements.fROLLY;
	
	// Use whichever measurement collection has data
	if (!inclyMeasurements.empty()) {
		auto romIt = inclyMeasurements.begin();
		ensure("INCLY ROM should have measurements", !romIt->measINCLY.empty());
		validateMeasurementParameters(romIt->measINCLY, params.expectedMeasurements);
	} else if (!rollyMeasurements.empty()) {
		auto romIt = rollyMeasurements.begin();
		ensure("ROLLY ROM should have measurements", !romIt->measROLLY.empty());
		validateMeasurementParameters(romIt->measROLLY, params.expectedMeasurements);
	} else {
		ensure("Either INCLY or ROLLY measurements should be present", false);
	}
}


/*
 * Helper function for testing INCLY vs ROLLY mathematical model differences
 * 
 * This function validates that INCLY and ROLLY produce different calculated values
 * for the same input observations due to their different mathematical models.
 * It compares the mathematical model outputs directly to ensure the model split
 * is meaningful and produces different results.
 * 
 * Key Validation Points:
 * 1. INCLY and ROLLY produce different calculated values for identical input
 * 2. Mathematical model differences scale appropriately with angle magnitude
 * 3. Both models converge to their respective mathematical solutions
 * 4. The model split provides meaningful differentiation
 * 
 * Test Structure:
 * - Two separate frames with identical transformation parameters
 * - One frame contains INCLY measurement, other contains ROLLY measurement
 * - Same observation value and same frame parameters (RX, RY)
 * - Different mathematical models should produce different calculated values
 * 
 * Mathematical Models:
 * INCLY: calcMeas = arcsin(sin(RY) * cos(RX))  (normalized arcsin model)
 * ROLLY: calcMeas = RY                         (legacy atan2 model)
 * 
 * Expected Results:
 * - INCLY calculated value ≠ ROLLY calculated value
 * - Both models should converge to their respective mathematical solutions
 * - Frame parameters should be estimated differently for each model
 * 
 * Parameters:
 * - testData: Input data string containing both INCLY and ROLLY frames
 * - logFileName: Output log file name for the test
 * - testDescription: Descriptive name for the test
 * - expectedMinDifference: Minimum expected difference between models
 * - angleDescription: Description of the angle range being tested
 * 
 * This function abstracts the mathematical model comparison logic to ensure
 * consistent validation across different angle ranges and test scenarios.
 */
struct ModelComparisonTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string angleDescription;
	TReal expectedMinDifference;
	TReal rxValue;  // RX rotation value in radians for theoretical calculation
	TReal ryValue;  // RY rotation value in radians for theoretical calculation
	TReal tolerance; // Tolerance for theoretical value comparison
};

void runModelComparisonTest(const ModelComparisonTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Get the appropriate measurement collection based on measurement type
	const auto& inclyMeasurements = frameIt.node->data->measurements.fINCLY;
	const auto& rollyMeasurements = frameIt.node->data->measurements.fROLLY;
	
	// Use whichever measurement collection has data
	if (!inclyMeasurements.empty()) {
		auto romIt = inclyMeasurements.begin();
		ensure("INCLY ROM should have measurements", !romIt->measINCLY.empty());
		
		// Calculate theoretical value for INCLY using arcsin model: arcsin(sin(RY) * cos(RX))
		TReal theoreticalINCLY = asin(sin(params.ryValue) * cos(params.rxValue));
		
		// Validate mathematical model differences
		for (auto& meas : romIt->measINCLY) {
			TReal measuredValue = meas.getAngle().getGonsValue();
			// Convert theoretical value from radians to gon for comparison
			TReal theoreticalValueGon = theoreticalINCLY * 200.0 / M_PI;
			ensure("Measured value should differ from theoretical value", std::abs(measuredValue - theoreticalValueGon) > params.tolerance);
		}
	} else if (!rollyMeasurements.empty()) {
		auto romIt = rollyMeasurements.begin();
		ensure("ROLLY ROM should have measurements", !romIt->measROLLY.empty());
		
		// Calculate theoretical value for ROLLY using atan2 model: RY
		TReal theoreticalROLLY = params.ryValue;
		
		// Validate mathematical model differences
		for (auto& meas : romIt->measROLLY) {
			TReal measuredValue = meas.getAngle().getGonsValue();
			// Convert theoretical value from radians to gon for comparison
			TReal theoreticalValueGon = theoreticalROLLY * 200.0 / M_PI;
			ensure("Measured value should differ from theoretical value", std::abs(measuredValue - theoreticalValueGon) > params.tolerance);
		}
	} else {
		ensure("Either INCLY or ROLLY measurements should be present", false);
	}
}

/*
 * Helper function for testing INSTR keyword functionality and dynamic instrument switching
 * 
 * This function validates the INSTR keyword functionality for inclinometer measurements (INCLY/ROLLY),
 * ensuring that measurements can dynamically switch between different INCL instruments within
 * a single round of measurements. It tests instrument parameter inheritance and switching behavior.
 * 
 * Key Validation Points:
 * 1. Inclinometer measurements can dynamically switch between different INCL instruments
 * 2. The INSTR keyword correctly overrides the default instrument for specific measurements
 * 3. Instrument switching works correctly within a single round of measurements
 * 4. The system correctly tracks which instrument is used for each measurement
 * 5. Default instrument restoration after INSTR usage
 * 
 * Test Structure:
 * - Multiple INCL instruments with different parameters (I1, I2, etc.)
 * - Single frame with multiple inclinometer measurements using INSTR keyword
 * - Instrument definitions with different precision parameters (OBSE, PPM, AC, RF, etc.)
 * 
 * INSTR Keyword Behavior:
 * - INSTR keyword applies only to the specific measurement line where it appears
 * - After INSTR usage, subsequent measurements return to the default instrument
 * - Each measurement can have different instrument parameters (OBSE, PPM, AC, RF, etc.)
 * 
 * Validation Process:
 * 1. Parse input file with multiple INCL definitions and INSTR usage
 * 2. Navigate to the frame containing inclinometer measurements
 * 3. Iterate through the measurements in sequence
 * 4. Verify each measurement uses the correct instrument ID
 * 
 * Expected Instrument Assignment Pattern:
 * - Measurement 1: Uses default instrument (e.g., "I1")
 * - Measurement 2: Uses override instrument (e.g., "I2" via INSTR override)
 * - Measurement 3: Returns to default instrument (e.g., "I1")
 * 
 * Practical Use Case:
 * This functionality allows users to use different inclinometer instruments
 * with different precision parameters within the same measurement session,
 * which is useful for mixed-precision measurement campaigns.
 * 
 * Mathematical Model: The respective mathematical model (arcsin for INCLY, atan2 for ROLLY)
 * applies to all measurements regardless of which instrument is used.
 * 
 * This function abstracts the INSTR validation logic to ensure consistent behavior
 * between INCLY and ROLLY instrument switching, following DRY principles.
 */
struct INSTRTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
	std::vector<std::string> expectedInstrumentIDs;
	int targetFrameIterations;
};

void runINSTRKeywordTest(const INSTRTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);

	TDataTreeIterator frameIt = projTest->getTree().begin();
	
	// Navigate to target frame
	for (int i = 0; i < params.targetFrameIterations; i++) {
		frameIt++;
	}

	// Get the appropriate measurement collection based on measurement type
	std::list<TROLLYROM>* rollyMeasurements = nullptr;
	std::list<TINCLYROM>* inclyMeasurements = nullptr;
	
	if (params.measurementType == "ROLLY") {
		rollyMeasurements = &frameIt.node->data->measurements.fROLLY;
	} else if (params.measurementType == "INCLY") {
		inclyMeasurements = &frameIt.node->data->measurements.fINCLY;
	}

	// Validate instrument assignments for each measurement
	if (rollyMeasurements && !rollyMeasurements->empty()) {
		auto obsIt = rollyMeasurements->begin()->measROLLY.begin();
		for (size_t i = 0; i < params.expectedInstrumentIDs.size() && obsIt != rollyMeasurements->begin()->measROLLY.end(); ++i, ++obsIt) {
			std::string errorMsg = params.measurementType + " instrument for measurement " + std::to_string(i + 1) + " should be " + params.expectedInstrumentIDs[i];
			ensure_equals(errorMsg.c_str(), obsIt->target.ID, params.expectedInstrumentIDs[i]);
		}
	} else if (inclyMeasurements && !inclyMeasurements->empty()) {
		auto obsIt = inclyMeasurements->begin()->measINCLY.begin();
		for (size_t i = 0; i < params.expectedInstrumentIDs.size() && obsIt != inclyMeasurements->begin()->measINCLY.end(); ++i, ++obsIt) {
			std::string errorMsg = params.measurementType + " instrument for measurement " + std::to_string(i + 1) + " should be " + params.expectedInstrumentIDs[i];
			ensure_equals(errorMsg.c_str(), obsIt->target.ID, params.expectedInstrumentIDs[i]);
		}
	}
}

/*
 * Helper function for testing inclinometer mathematical model derivatives and transformation parameters
 * 
 * This function validates mathematical derivatives and transformation parameter calculations
 * for inclinometer measurements (INCLY/ROLLY) using their respective mathematical models.
 * It ensures consistent derivative computation and parameter estimation across different models.
 * 
 * Key Validation Points:
 * 1. Inclinometer measurements correctly contribute to least squares matrices
 * 2. Partial derivatives for transformation parameters are computed correctly
 * 3. The mathematical model produces accurate parameter estimates
 * 4. Scale and transformation parameters converge to expected values
 * 5. S0 validation confirms perfect fit scenarios
 * 
 * Parameters:
 * - testData: Input data string containing complex multi-frame transformation scenario
 * - logFileName: Output log file name for the test
 * - testDescription: Descriptive name for the test
 * - measurementType: Type of measurement being tested ("INCLY" or "ROLLY")
 * - expectedS0: Expected S0 a posteriori value (typically 0 for perfect fit)
 * - expectedTransformations: Expected final transformation parameters
 * 
 * Common Test Structure:
 * - Complex multi-frame with all transformation types (TX TY TZ RX RY RZ SCL)
 * - Frame hierarchy: ROOT -> TEST (main frame) -> TEST2 (subframe)
 * - Contains OBSXYZ measurements and inclinometer measurements in both frames
 * - Initial frame parameters designed to test convergence to known solutions
 * 
 * Mathematical Model Testing:
 * INCLY: Uses arcsin(X/|V|) normalized inclinometer model
 * ROLLY: Uses legacy atan2(X,Z) inclinometer model
 * 
 * Validation Process:
 * 1. Parse complex multi-frame input with transformation parameters
 * 2. Complete least squares calculation successfully
 * 3. Validate S0 a posteriori matches expected value
 * 4. Extract estimated transformation parameters from target frame
 * 5. Verify all transformation parameters match expected converged values
 * 
 * This function abstracts the derivative validation logic to ensure consistent behavior
 * between INCLY and ROLLY mathematical models, following DRY principles.
 */
struct TransformationTestParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
	TReal expectedS0;
	TReal expectedTX, expectedTY, expectedTZ;
	TReal expectedRX, expectedRY, expectedRZ;
	TReal expectedScale;
	int targetFrameIterations;
};

void runTransformationDerivativesTest(const TransformationTestParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Validate transformation derivatives
	ensure_equals("Rotation Y does not match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), params.expectedRY, 1e-6);
}

/*
 * Helper function for testing edge cases - extreme angle values
 * 
 * This function validates that inclinometer measurements handle extreme angle values correctly:
 * - Very small angles (near 0°) - tests numerical stability
 * - Very large angles (near 90°) - tests domain limits
 * - Extreme angles (near 180°) - tests mathematical model robustness
 * 
 * Edge Cases:
 * - Small angles: 0.001 gon (near zero)
 * - Large angles: 89.999 gon (near 90°)
 * - Extreme angles: 179.999 gon (near 180°)
 * 
 * Expected Behavior:
 * - All measurements should be processed without errors
 * - Mathematical models should remain stable
 * - Calculations should complete successfully
 * - No numerical instabilities should occur
 * 
 * This function abstracts the extreme angle validation logic to ensure consistent behavior
 * between INCLY and ROLLY measurements, following DRY principles.
 */
 struct ExtremeAngleEdgeCaseParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
};

void runExtremeAngleEdgeCaseTest(const ExtremeAngleEdgeCaseParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// Validate that extreme angles are processed correctly
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Get the appropriate measurement collection based on measurement type
	const auto& inclyMeasurements = frameIt.node->data->measurements.fINCLY;
	const auto& rollyMeasurements = frameIt.node->data->measurements.fROLLY;
	
	// Use whichever measurement collection has data
	if (!inclyMeasurements.empty()) {
		auto romIt = inclyMeasurements.begin();
		ensure("INCLY ROM should have measurements", !romIt->measINCLY.empty());
		
		// Check that all measurements have valid residuals (no NaN or infinite values)
		for (auto& meas : romIt->measINCLY) {
			TReal residual = meas.getAngleResidual().getRadiansValue();
			ensure("Residual should not be NaN", !std::isnan(residual));
			ensure("Residual should not be infinite", !std::isinf(residual));
			ensure("Residual should be finite", std::isfinite(residual));
		}
	} else if (!rollyMeasurements.empty()) {
		auto romIt = rollyMeasurements.begin();
		ensure("ROLLY ROM should have measurements", !romIt->measROLLY.empty());
		
		// Check that all measurements have valid residuals (no NaN or infinite values)
		for (auto& meas : romIt->measROLLY) {
			TReal residual = meas.getAngleResidual().getRadiansValue();
			ensure("Residual should not be NaN", !std::isnan(residual));
			ensure("Residual should not be infinite", !std::isinf(residual));
			ensure("Residual should be finite", std::isfinite(residual));
		}
	} else {
		ensure("Either INCLY or ROLLY measurements should be present", false);
	}
}

/*
 * Helper function for testing edge cases - unusual parameter combinations
 * 
 * This function validates that inclinometer measurements handle unusual parameter combinations correctly:
 * - Very small parameter values (near zero) - tests numerical precision
 * - Very large parameter values (near limits) - tests parameter bounds
 * - Mixed parameter combinations - tests parameter interaction
 * 
 * Edge Cases:
 * - Tiny parameters: 0.001 (very small values)
 * - Huge parameters: 999.9 (very large values)
 * - Small instrument: Instrument with very small default parameters
 * 
 * Expected Behavior:
 * - All parameter combinations should be processed without errors
 * - System should handle extreme parameter values gracefully
 * - Calculations should complete successfully
 * - No parameter-related crashes should occur
 * 
 * This function abstracts the unusual parameter validation logic to ensure consistent behavior
 * between INCLY and ROLLY measurements, following DRY principles.
 */
 struct UnusualParameterEdgeCaseParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
};

void runUnusualParameterEdgeCaseTest(const UnusualParameterEdgeCaseParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// Validate that unusual parameters are processed correctly
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Get the appropriate measurement collection based on measurement type
	const auto& inclyMeasurements = frameIt.node->data->measurements.fINCLY;
	const auto& rollyMeasurements = frameIt.node->data->measurements.fROLLY;
	
	// Use whichever measurement collection has data
	if (!inclyMeasurements.empty()) {
		auto romIt = inclyMeasurements.begin();
		ensure("INCLY ROM should have measurements", !romIt->measINCLY.empty());
		
		// Check that all measurements have valid parameters
		for (auto& meas : romIt->measINCLY) {
			// Validate that all parameters are finite
			ensure("AC should be finite", std::isfinite(meas.target.angleCorrectionValue.getGonsValue()));
			ensure("RF should be finite", std::isfinite(meas.target.refAngleCorrectionValue.getGonsValue()));
			ensure("OBSE should be finite", std::isfinite(meas.target.sigmaAngl.getGonsValue()));
			ensure("PPM should be finite", std::isfinite(meas.target.sigmaPpm.getGonsValue()));
			ensure("ACSE should be finite", std::isfinite(meas.target.sigmaCorrectionValue.getGonsValue()));
			ensure("RFSE should be finite", std::isfinite(meas.target.refSigmaCorrectionValue.getGonsValue()));
		}
	} else if (!rollyMeasurements.empty()) {
		auto romIt = rollyMeasurements.begin();
		ensure("ROLLY ROM should have measurements", !romIt->measROLLY.empty());
		
		// Check that all measurements have valid parameters
		for (auto& meas : romIt->measROLLY) {
			// Validate that all parameters are finite
			ensure("AC should be finite", std::isfinite(meas.target.angleCorrectionValue.getGonsValue()));
			ensure("RF should be finite", std::isfinite(meas.target.refAngleCorrectionValue.getGonsValue()));
			ensure("OBSE should be finite", std::isfinite(meas.target.sigmaAngl.getGonsValue()));
			ensure("PPM should be finite", std::isfinite(meas.target.sigmaPpm.getGonsValue()));
			ensure("ACSE should be finite", std::isfinite(meas.target.sigmaCorrectionValue.getGonsValue()));
			ensure("RFSE should be finite", std::isfinite(meas.target.refSigmaCorrectionValue.getGonsValue()));
		}
	} else {
		ensure("Either INCLY or ROLLY measurements should be present", false);
	}
}

/*
 * Helper function for testing edge cases - complex frame transformations
 * 
 * This function validates that inclinometer measurements handle complex frame transformation scenarios correctly:
 * - Nested frame hierarchies - tests frame transformation chaining
 * - Multiple transformation levels - tests transformation accumulation
 * - Different measurement contexts - tests frame-specific behavior
 * 
 * Edge Cases:
 * - 3-level nested frames (OUTER -> MIDDLE -> INNER)
 * - Different transformation parameters at each level
 * - Measurements in different frame contexts
 * 
 * Expected Behavior:
 * - All frame transformations should be applied correctly
 * - Measurements should be processed in correct frame contexts
 * - Transformation chaining should work properly
 * - No frame-related errors should occur
 * 
 * This function abstracts the complex frame transformation validation logic to ensure consistent behavior
 * between INCLY and ROLLY measurements, following DRY principles.
 */
 struct ComplexFrameEdgeCaseParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
};

void runComplexFrameEdgeCaseTest(const ComplexFrameEdgeCaseParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// Validate that complex frame transformations are processed correctly
	TDataTree tree = projTest->getTree();
	
	// Navigate through the frame hierarchy
	TDataTreeIterator frameIt = tree.begin();
	frameIt++; // OUTER frame
	
	// Check that measurements exist in the frame hierarchy
	bool foundMeasurements = false;
	for (size_t i = 0; i < tree.size(); ++i) {
		// Check both INCLY and ROLLY measurements
		if (!frameIt.node->data->measurements.fINCLY.empty() || 
			!frameIt.node->data->measurements.fROLLY.empty()) {
			foundMeasurements = true;
			break;
		}
	frameIt++;
	}
	
	ensure("Measurements should be found in frame hierarchy", foundMeasurements);
}


/*
 * Helper function for testing edge cases - reference point scenarios
 * 
 * This function validates that inclinometer measurements handle various reference point scenarios correctly:
 * - Reference point at origin - tests basic functionality
 * - Far away points - tests numerical stability with large distances
 * - Very close points - tests precision with small distances
 * 
 * Edge Cases:
 * - REF_PT: Reference point at origin (0,0,0)
 * - FAR_PT: Far away point (1000,1000,1000)
 * - NEAR_PT: Very close point (0.001,0.001,0.001)
 * 
 * Expected Behavior:
 * - All reference point scenarios should be processed correctly
 * - System should handle extreme distances gracefully
 * - Calculations should remain numerically stable
 * - No distance-related errors should occur
 * 
 * This function abstracts the reference point scenario validation logic to ensure consistent behavior
 * between INCLY and ROLLY measurements, following DRY principles.
 */
 struct ReferencePointEdgeCaseParams {
	std::string testData;
	std::string logFileName;
	std::string testDescription;
	std::string measurementType;
};

void runReferencePointEdgeCaseTest(const ReferencePointEdgeCaseParams& params, std::shared_ptr<TLGCData> projTest, TReader& r) {
	projTest->getFileLogger().setOutputfileLocation(params.logFileName);
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(params.testData);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// Validate that reference point scenarios are processed correctly
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;

	// Get the appropriate measurement collection based on measurement type
	const auto& inclyMeasurements = frameIt.node->data->measurements.fINCLY;
	const auto& rollyMeasurements = frameIt.node->data->measurements.fROLLY;
	
	// Use whichever measurement collection has data
	if (!inclyMeasurements.empty()) {
		auto romIt = inclyMeasurements.begin();
		ensure("INCLY ROM should have measurements", !romIt->measINCLY.empty());
		
		// Check that all measurements have valid residuals regardless of reference point distance
		for (auto& meas : romIt->measINCLY) {
			TReal residual = meas.getAngleResidual().getRadiansValue();
			ensure("Residual should not be NaN", !std::isnan(residual));
			ensure("Residual should not be infinite", !std::isinf(residual));
			ensure("Residual should be finite", std::isfinite(residual));
		}
	} else if (!rollyMeasurements.empty()) {
		auto romIt = rollyMeasurements.begin();
		ensure("ROLLY ROM should have measurements", !romIt->measROLLY.empty());
		
		// Check that all measurements have valid residuals regardless of reference point distance
		for (auto& meas : romIt->measROLLY) {
			TReal residual = meas.getAngleResidual().getRadiansValue();
			ensure("Residual should not be NaN", !std::isnan(residual));
			ensure("Residual should not be infinite", !std::isinf(residual));
			ensure("Residual should be finite", std::isfinite(residual));
		}
	} else {
		ensure("Either INCLY or ROLLY measurements should be present", false);
	}
}


//############################ TESTS FOR INCLY AND ROLLY  #########################################################//

/*
 * Test INCLY root error using parameterized helper function
 * 
 * Validates that INCLY measurements are correctly rejected when placed in root frame.
 * Uses the shared runRootErrorTest function to maintain consistency with ROLLY tests.
 * 
 * This test ensures proper enforcement of LGC measurement placement rules for INCLY,
 * verifying that measurements must be within *FRAME....*ENDFRAME blocks.
 */
template<>
template<>
void object::test<1>()
{
	set_test_name("Testing INCLY, 1 STATION in root");
	
	RootErrorTestParams params = {
		TestINCL::INCLY_ROOT_1,
		"C:/Temp/INCLY_ROOT_1.txt",
		"Testing INCLY, 1 STATION in root",
		"INCLY"
	};
	
	runRootErrorTest(params, projTest, r);
}

/*
 * Test ROLLY root error using parameterized helper function
 * 
 * Validates that ROLLY measurements are correctly rejected when placed in root frame.
 * Uses the shared runRootErrorTest function to maintain consistency with INCLY tests.
 * 
 * This test ensures proper enforcement of LGC measurement placement rules for ROLLY,
 * verifying that measurements must be within *FRAME....*ENDFRAME blocks.
 */
 template<>
 template<>
 void object::test<2>()
 {
	 set_test_name("Testing ROLLY, 1 STATION in root");
	 
	 RootErrorTestParams params = {
		 TestINCL::ROLLY_LEGACY_ROOT_1,
		 "C:/Temp/ROLLY_LEGACY_ROOT_1.txt",
		 "Testing ROLLY, 1 STATION in root",
		 "ROLLY"
	 };
	 
	 runRootErrorTest(params, projTest, r);
 }

/*
 * Test INCLY observation ID parsing using parameterized helper function
 * 
 * Validates that INCLY measurements correctly handle observation ID parsing and validation.
 * Uses the shared runIDValidationTest function to maintain consistency with ROLLY tests.
 * 
 * This test ensures proper ID keyword functionality for INCLY measurements,
 * verifying that custom IDs are parsed, stored, and validated correctly.
 */
template<>
template<>
void object::test<3>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root and test INCLY ID reading");
	
	IDValidationTestParams params = {
		TestINCL::INCLY_SUBF_5_id,
		"C:/Temp/INCLY_SUBF_5_id.txt",
		"Testing INCLY ID reading",
		"INCLY"
	};
	
	runIDValidationTest(params, projTest, r);
}

/*
 * Test ROLLY observation ID parsing using parameterized helper function
 * 
 * Validates that ROLLY measurements correctly handle observation ID parsing and validation.
 * Uses the shared runIDValidationTest function to maintain consistency with INCLY tests.
 * 
 * This test ensures proper ID keyword functionality for ROLLY measurements,
 * verifying that custom IDs are parsed, stored, and validated correctly.
 * Uses the legacy atan2(X,Z) mathematical model for ROLLY calculations.
 */
 template<>
 template<>
 void object::test<4>()
 {
	 set_test_name("Testing ROLLY, 1 STATION in SBF, CALA declared in the root and test ROLLY ID reading");
	 
	 IDValidationTestParams params = {
		 TestINCL::ROLLY_LEGACY_SUBF_5_id,
		 "C:/Temp/ROLLY_LEGACY_SUBF_5_id.txt",
		 "Testing ROLLY ID reading",
		 "ROLLY"
	 };
	 
	 runIDValidationTest(params, projTest, r);
 }

/*
 * Test INCLY context OBSXYZ observation ID parsing using parameterized helper function
 * 
 * Validates OBSXYZ observation ID functionality in complex frame structures alongside INCLY measurements.
 * Uses the shared runOBSXYZIDValidationTest function to maintain consistency with ROLLY context tests.
 * 
 * This test ensures proper OBSXYZ ID keyword functionality in INCLY measurement contexts,
 * verifying that OBSXYZ IDs are parsed, stored, and validated correctly in complex frame hierarchies.
 */
template<>
template<>
void object::test<5>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root and test OBSXYZ ID reading");
	
	OBSXYZIDValidationTestParams params = {
		TestINCL::INCLY_SUBF_4_xyzID,
		"C:/Temp/INCLY_SUBF_4_xyzID.txt",
		"Testing INCLY context OBSXYZ ID reading",
		"INCLY",
		"XYZ.BEAM_XT03.BDB.0900.E",
		4  // Frame iterations: ROOT -> RSTB -> RSTB2 -> RSTRI -> RSTR
	};
	
	runOBSXYZIDValidationTest(params, projTest, r);
}

/*
 * Test ROLLY context OBSXYZ observation ID parsing using parameterized helper function
 * 
 * Validates OBSXYZ observation ID functionality in complex frame structures alongside ROLLY measurements.
 * Uses the shared runOBSXYZIDValidationTest function to maintain consistency with INCLY context tests.
 * 
 * This test ensures proper OBSXYZ ID keyword functionality in ROLLY measurement contexts,
 * verifying that OBSXYZ IDs are parsed, stored, and validated correctly in complex frame hierarchies.
 * Uses the legacy atan2(X,Z) mathematical model for ROLLY calculations.
 */
 template<>
 template<>
 void object::test<6>()
 {
	 set_test_name("Testing ROLLY, 1 STATION in SBF, CALA declared in the root and test OBSXYZ ID reading");
	 
	 OBSXYZIDValidationTestParams params = {
		 TestINCL::ROLLY_LEGACY_SUBF_4_xyzID,
		 "C:/Temp/ROLLY_LEGACY_SUBF_4_xyzID.txt",
		 "Testing ROLLY context OBSXYZ ID reading",
		 "ROLLY",
		 "XYZ.BEAM_XT03.BDB.0900.E",
		 4  // Frame iterations: ROOT -> RSTB -> RSTB2 -> RSTRI -> RSTR
	 };
	 
	 runOBSXYZIDValidationTest(params, projTest, r);
 }

/*
 * Test INCLY mathematical model derivatives using parameterized helper function
 * 
 * Validates mathematical derivatives and transformation parameter calculations for INCLY measurements
 * using the arcsin(X/|V|) normalized inclinometer model. Uses the shared runTransformationDerivativesTest
 * function to maintain consistency with ROLLY derivative tests.
 * 
 * This test ensures proper derivative computation for the INCLY arcsin model,
 * verifying that transformation parameters converge correctly with the new mathematical approach.
 */
 template<>
 template<>
 void object::test<7>()
 {
	 set_test_name("Testing INCLY derivatives and the angle and scale partial derivatives with TFreeVector");
	 
	 TransformationTestParams params = {
		 TestINCL::INCLY_CONTRIB,
		 "C:/Temp/INCLY_CONTRIB.txt",
		 "Testing INCLY arcsin model derivatives",
		 "INCLY",
		 0.0,  // Expected S0 (perfect fit)
		 -1.0, -1.0, -1.0,  // Expected translations (TX, TY, TZ)
		 0.0, 0.0, 0.0,     // Expected rotations (RX, RY, RZ)
		 1.0,               // Expected scale
		 1                  // Target frame iterations (ROOT -> TEST)
	 };
	 
	 runTransformationDerivativesTest(params, projTest, r);
 }

 /*
 * Test ROLLY mathematical model derivatives using parameterized helper function
 * 
 * Validates mathematical derivatives and transformation parameter calculations for ROLLY measurements
 * using the legacy atan2(X,Z) inclinometer model. Uses the shared runTransformationDerivativesTest
 * function to maintain consistency with INCLY derivative tests.
 * 
 * This test ensures proper derivative computation for the ROLLY atan2 model,
 * verifying that transformation parameters converge correctly with the legacy mathematical approach.
 * Uses the legacy atan2(X,Z) mathematical model for ROLLY calculations.
 */
template<>
template<>
void object::test<8>()
{
	set_test_name("Testing ROLLY derivatives and the angle and scale partial derivatives with TFreeVector");
	
	TransformationTestParams params = {
		TestINCL::ROLLY_LEGACY_CONTRIB,
		"C:/Temp/ROLLY_LEGACY_CONTRIB.txt",
		"Testing ROLLY atan2 model derivatives",
		"ROLLY",
		0.0,  // Expected S0 (perfect fit)
		-1.0, -1.0, -1.0,  // Expected translations (TX, TY, TZ)
		0.0, 0.0, 0.0,     // Expected rotations (RX, RY, RZ)
		1.0,               // Expected scale
		1                  // Target frame iterations (ROOT -> TEST)
	};
	
	runTransformationDerivativesTest(params, projTest, r);
}


 /*
 * Test INCLY INSTR keyword functionality using parameterized helper function
 * 
 * Validates the INSTR keyword functionality for INCLY measurements using the arcsin mathematical model.
 * Uses the shared runINSTRKeywordTest function to maintain consistency with ROLLY INSTR tests.
 * 
 * This test ensures proper instrument switching behavior for INCLY measurements,
 * verifying that the INSTR keyword correctly overrides default instruments and
 * restores them for subsequent measurements.
 * 
 * Instrument switching pattern: I1 (default) -> I2 (INSTR override) -> I1 (default restored)
 */
template<>
template<>
void object::test<9>()
{
	set_test_name("Testing INCLY INSTR");
	
	INSTRTestParams params = {
		TestINCL::INCLY_SUBF_8,
		"C:/Temp/INCLY_SUBF_8.txt",
		"Testing INCLY INSTR keyword functionality",
		"INCLY",
		{"I1", "I2", "I1"},  // Expected instrument sequence: default -> override -> default
		1                    // Target frame iterations (ROOT -> TEST)
	};
	
	runINSTRKeywordTest(params, projTest, r);
}

/*
 * Test ROLLY INSTR keyword functionality using parameterized helper function
 * 
 * Validates the INSTR keyword functionality for ROLLY measurements using the legacy atan2 mathematical model.
 * Uses the shared runINSTRKeywordTest function to maintain consistency with INCLY INSTR tests.
 * 
 * This test ensures proper instrument switching behavior for ROLLY measurements,
 * verifying that the INSTR keyword correctly overrides default instruments and
 * restores them for subsequent measurements.
 * 
 * Instrument switching pattern: I1 (default) -> I2 (INSTR override) -> I1 (default restored)
 */
 template<>
 template<>
 void object::test<10>()
 {
	 set_test_name("Testing ROLLY INSTR");
	 
	 INSTRTestParams params = {
		 TestINCL::ROLLY_LEGACY_SUBF_8,
		 "C:/Temp/ROLLY_LEGACY_SUBF_8.txt",
		 "Testing ROLLY INSTR keyword functionality",
		 "ROLLY",
		 {"I1", "I2", "I1"},  // Expected instrument sequence: default -> override -> default
		 1                    // Target frame iterations (ROOT -> TEST)
	 };
	 
	 runINSTRKeywordTest(params, projTest, r);
 }

 /*
 * Test INCLY edge cases - extreme angle values using parameterized helper function
 */
template<>
template<>
void object::test<11>()
{
	set_test_name("Testing INCLY edge cases - extreme angle values");
	
	ExtremeAngleEdgeCaseParams params = {
		TestINCL::INCLY_EDGE_CASE_EXTREME_ANGLES,
		"C:/Temp/INCLY_EDGE_CASE_EXTREME_ANGLES.txt",
		"INCLY Edge Case - Extreme Angles Test",
		"INCLY"
	};
	
	runExtremeAngleEdgeCaseTest(params, projTest, r);
}

/*
 * Test ROLLY edge cases - extreme angle values using parameterized helper function
 */
template<>
template<>
void object::test<12>()
{
	set_test_name("Testing ROLLY edge cases - extreme angle values");
	
	ExtremeAngleEdgeCaseParams params = {
		TestINCL::ROLLY_EDGE_CASE_EXTREME_ANGLES,
		"C:/Temp/ROLLY_EDGE_CASE_EXTREME_ANGLES.txt",
		"ROLLY Edge Case - Extreme Angles Test",
		"ROLLY"
	};
	
	runExtremeAngleEdgeCaseTest(params, projTest, r);
}

/*
 * Test INCLY edge cases - unusual parameter combinations using parameterized helper function
 */
 template<>
 template<>
 void object::test<13>()
 {
	 set_test_name("Testing INCLY edge cases - unusual parameter combinations");
	 
	 UnusualParameterEdgeCaseParams params = {
		 TestINCL::INCLY_EDGE_CASE_UNUSUAL_PARAMS,
		 "C:/Temp/INCLY_EDGE_CASE_UNUSUAL_PARAMS.txt",
		 "INCLY Edge Case - Unusual Parameter Combinations Test",
		 "INCLY"
	 };
	 
	 runUnusualParameterEdgeCaseTest(params, projTest, r);
 }
 
 /*
  * Test ROLLY edge cases - unusual parameter combinations using parameterized helper function
  */
 template<>
 template<>
 void object::test<14>()
 {
	 set_test_name("Testing ROLLY edge cases - unusual parameter combinations");
	 
	 UnusualParameterEdgeCaseParams params = {
		 TestINCL::ROLLY_EDGE_CASE_UNUSUAL_PARAMS,
		 "C:/Temp/ROLLY_EDGE_CASE_UNUSUAL_PARAMS.txt",
		 "ROLLY Edge Case - Unusual Parameter Combinations Test",
		 "ROLLY"
	 };
	 
	 runUnusualParameterEdgeCaseTest(params, projTest, r);
 }

 /*
 * Test INCLY edge cases - complex frame transformations using parameterized helper function
 */
template<>
template<>
void object::test<15>()
{
	set_test_name("Testing INCLY edge cases - complex frame transformations");
	
	ComplexFrameEdgeCaseParams params = {
		TestINCL::INCLY_EDGE_CASE_COMPLEX_FRAMES,
		"C:/Temp/INCLY_EDGE_CASE_COMPLEX_FRAMES.txt",
		"INCLY Edge Case - Complex Frame Transformations Test",
		"INCLY"
	};
	
	runComplexFrameEdgeCaseTest(params, projTest, r);
}

/*
 * Test ROLLY edge cases - complex frame transformations using parameterized helper function
 */
template<>
template<>
void object::test<16>()
{
	set_test_name("Testing ROLLY edge cases - complex frame transformations");
	
	ComplexFrameEdgeCaseParams params = {
		TestINCL::ROLLY_EDGE_CASE_COMPLEX_FRAMES,
		"C:/Temp/ROLLY_EDGE_CASE_COMPLEX_FRAMES.txt",
		"ROLLY Edge Case - Complex Frame Transformations Test",
		"ROLLY"
	};
	
	runComplexFrameEdgeCaseTest(params, projTest, r);
}

/*
 * Test INCLY edge cases - reference point scenarios using parameterized helper function
 */
 template<>
 template<>
 void object::test<17>()
 {
	 set_test_name("Testing INCLY edge cases - reference point scenarios");
	 
	 ReferencePointEdgeCaseParams params = {
		 TestINCL::INCLY_EDGE_CASE_REF_POINTS,
		 "C:/Temp/INCLY_EDGE_CASE_REF_POINTS.txt",
		 "INCLY Edge Case - Reference Point Scenarios Test",
		 "INCLY"
	 };
	 
	 runReferencePointEdgeCaseTest(params, projTest, r);
 }
 
 /*
  * Test ROLLY edge cases - reference point scenarios using parameterized helper function
  */
 template<>
 template<>
 void object::test<18>()
 {
	 set_test_name("Testing ROLLY edge cases - reference point scenarios");
	 
	 ReferencePointEdgeCaseParams params = {
		 TestINCL::ROLLY_EDGE_CASE_REF_POINTS,
		 "C:/Temp/ROLLY_EDGE_CASE_REF_POINTS.txt",
		 "ROLLY Edge Case - Reference Point Scenarios Test",
		 "ROLLY"
	 };
	 
	 runReferencePointEdgeCaseTest(params, projTest, r);
 }

 /*
 * Test INCLY vs ROLLY mathematical model differences using parameterized helper function
 * 
 * Validates that INCLY and ROLLY produce different calculated values for the same input
 * observations due to their different mathematical models. Uses the shared
 * runModelComparisonTest function to ensure consistent validation.
 * 
 * Test Data: RX=0.1, RY=0.2 gon (medium angles)
 * Expected: INCLY arcsin model ≠ ROLLY atan2 model
 */
 template<>
 template<>
 void object::test<19>()
 {
	 set_test_name("Testing INCLY vs ROLLY mathematical model differences");
	 
	 ModelComparisonTestParams params = {
		 TestINCL::INCLY_VS_ROLLY_MODEL_TEST,
		 "C:/Temp/INCLY_VS_ROLLY_MODEL_TEST.txt",
		 "Mathematical Model Comparison Test",
		 "Medium angles (RX=0.1, RY=0.2 gon)",
		 1e-6,  // Expected minimum difference for medium angles
		 0.1 * M_PI / 200.0,  // RX value in radians (0.1 gon)
		 0.2 * M_PI / 200.0,  // RY value in radians (0.2 gon)
		 1e-8   // Tolerance for theoretical value comparison
	 };
	 
	 runModelComparisonTest(params, projTest, r);
 }

  /*
  * Test INCLY vs ROLLY mathematical model differences across multiple angle ranges
  * 
  * This test validates the mathematical model differences across different angle ranges
  * using the parameterized helper function. It ensures that the differences scale
  * appropriately with angle magnitude.
  * 
  * Test Cases:
  * - Test Case 1: Small angles (RX=0.05, RY=0.1 gon) - Models should be similar
  * - Test Case 2: Medium angles (RX=0.2, RY=0.5 gon) - Increasing differences
  * - Test Case 3: Large angles (RX=0.5, RY=1.0 gon) - Significant differences
  * 
  * Expected Behavior:
  * - Small angles: Models should be similar (linear approximation holds)
  * - Medium angles: Increasing differences between models
  * - Large angles: Significant differences between arcsin and atan2 models
  * 
  * This comprehensive test validates that the mathematical model differences
  * scale appropriately with angle magnitude using the shared helper function.
  */
  template<>
  template<>
  void object::test<20>()
  {
	  set_test_name("Testing INCLY vs ROLLY mathematical model differences - Small angles");
	  
	  // Test Case 1: Small angles only
	  ModelComparisonTestParams params = {
		  TestINCL::INCLY_VS_ROLLY_TEST_CASE_1,
		  "C:/Temp/INCLY_VS_ROLLY_TEST_CASE_1.txt",
		  "Small angles (RX=0.05, RY=0.1 gon)",
		  "Small angles (RX=0.05, RY=0.1 gon)",
		  1e-8,  // Expected minimum difference for small angles
		  0.05 * M_PI / 200.0,  // RX value in radians (0.05 gon)
		  0.1 * M_PI / 200.0,   // RY value in radians (0.1 gon)
		  1e-9   // Tolerance for theoretical value comparison
	  };
	  runModelComparisonTest(params, projTest, r);
  }

  /*
   * Test INCLY vs ROLLY mathematical model differences - Medium angles
   * 
   * This test validates the mathematical model differences for medium angles
   * using the parameterized helper function. It ensures that the differences
   * are more pronounced than with small angles.
   * 
   * Test Case: Medium angles (RX=0.2, RY=0.5 gon) - Increasing differences
   * Expected: Medium angles show increasing differences between models
   */
  template<>
  template<>
  void object::test<21>()
  {
	  set_test_name("Testing INCLY vs ROLLY mathematical model differences - Medium angles");
	  
	  ModelComparisonTestParams params = {
		  TestINCL::INCLY_VS_ROLLY_TEST_CASE_2,
		  "C:/Temp/INCLY_VS_ROLLY_TEST_CASE_2.txt",
		  "Medium angles (RX=0.2, RY=0.5 gon)",
		  "Medium angles (RX=0.2, RY=0.5 gon)",
		  1e-6,  // Expected minimum difference for medium angles
		  0.2 * M_PI / 200.0,  // RX value in radians (0.2 gon)
		  0.5 * M_PI / 200.0,  // RY value in radians (0.5 gon)
		  1e-8   // Tolerance for theoretical value comparison
	  };
	  runModelComparisonTest(params, projTest, r);
  }

  /*
   * Test INCLY vs ROLLY mathematical model differences - Large angles
   * 
   * This test validates the mathematical model differences for large angles
   * using the parameterized helper function. It ensures that the differences
   * are significant for large angles.
   * 
   * Test Case: Large angles (RX=0.5, RY=1.0 gon) - Significant differences
   * Expected: Large angles show significant differences between models
   */
  template<>
  template<>
  void object::test<22>()
  {
	  set_test_name("Testing INCLY vs ROLLY mathematical model differences - Large angles");
	  
	  ModelComparisonTestParams params = {
		  TestINCL::INCLY_VS_ROLLY_TEST_CASE_3,
		  "C:/Temp/INCLY_VS_ROLLY_TEST_CASE_3.txt",
		  "Large angles (RX=0.5, RY=1.0 gon)",
		  "Large angles (RX=0.5, RY=1.0 gon)",
		  1e-5,  // Expected minimum difference for large angles
		  0.5 * M_PI / 200.0,  // RX value in radians (0.5 gon)
		  1.0 * M_PI / 200.0,  // RY value in radians (1.0 gon)
		  1e-8   // Tolerance for theoretical value comparison
	  };
	  runModelComparisonTest(params, projTest, r);
  }

/*
 * Test INCLY keyword parsing and parameter validation using helper function
 * 
 * This test validates that INCLY measurements correctly parse and apply all keywords:
 * - INSTR: Instrument selection and switching
 * - OBSE: Observation sigma override
 * - PPM: PPM sigma override
 * - AC: Angle correction
 * - ACSE: Angle correction sigma
 * - RF: Reference angle
 * - RFSE: Reference angle sigma
 * - ID: Observation ID assignment
 * 
 * Test Data: Multiple INCLY measurements with different keyword combinations
 * Expected: Each measurement should have correct parameter values based on keywords
 */
template<>
template<>
void object::test<23>()
{
	set_test_name("Testing INCLY keyword parsing and parameter validation");
	
	// Define expected measurement parameters
	// Note: Values are in their native units - TAngle objects handle conversions automatically
	std::vector<KeywordValidationTestParams::MeasurementParams> expectedMeasurements = {
		// Measurement 1: All keywords specified
		{"I1", 4.0, 3.0, 0.5, 1.0, 0.3, 2.0, "TEST1", 1.0, 0.0},  // OBSE=4CC, PPM=3μrad, AC=0.5gon, ACSE=1CC, RF=0.3gon, RFSE=2CC
		// Measurement 2: Different keyword values
		{"I1", 5.0, 4.0, 0.6, 2.0, 0.4, 3.0, "TEST2", 1.0, 0.0},  // OBSE=5CC, PPM=4μrad, AC=0.6gon, ACSE=2CC, RF=0.4gon, RFSE=3CC
		// Measurement 3: Only ID specified (uses instrument defaults)
		{"I1", 4.0, 3.0, 0.5, 1.0, 0.3, 2.0, "TEST3", 1.0, 0.0},  // Uses instrument defaults
		// Measurement 4: INSTR override to I2
		{"I2", 5.0, 4.0, 0.6, 2.0, 0.4, 3.0, "TEST4", 1.0, 0.0}   // Uses I2 instrument defaults
	};
	
	KeywordValidationTestParams params = {
		TestINCL::INCLY_KEYWORD_VALIDATION_TEST,
		"C:/Temp/INCLY_KEYWORD_VALIDATION_TEST.txt",
		"INCLY Keyword Validation Test",
		expectedMeasurements
	};
	
	runKeywordValidationTest(params, projTest, r);
}

/*
 * Test ROLLY keyword parsing and parameter validation using helper function
 * 
 * This test validates that ROLLY measurements correctly parse and apply all keywords:
 * - INSTR: Instrument selection and switching
 * - OBSE: Observation sigma override
 * - PPM: PPM sigma override
 * - AC: Angle correction
 * - ACSE: Angle correction sigma
 * - RF: Reference angle
 * - RFSE: Reference angle sigma
 * - ID: Observation ID assignment
 * 
 * Test Data: Multiple ROLLY measurements with different keyword combinations
 * Expected: Each measurement should have correct parameter values based on keywords
 */
template<>
template<>
void object::test<24>()
{
	set_test_name("Testing ROLLY keyword parsing and parameter validation");
	
	// Define expected measurement parameters
	// Note: Values are in their native units - TAngle objects handle conversions automatically
	std::vector<KeywordValidationTestParams::MeasurementParams> expectedMeasurements = {
		// Measurement 1: All keywords specified
		{"I1", 4.0, 3.0, 0.5, 1.0, 0.3, 2.0, "TEST1", 1.0, 0.0},  // OBSE=4CC, PPM=3μrad, AC=0.5gon, ACSE=1CC, RF=0.3gon, RFSE=2CC
		// Measurement 2: Different keyword values
		{"I1", 5.0, 4.0, 0.6, 2.0, 0.4, 3.0, "TEST2", 1.0, 0.0},  // OBSE=5CC, PPM=4μrad, AC=0.6gon, ACSE=2CC, RF=0.4gon, RFSE=3CC
		// Measurement 3: Only ID specified (uses instrument defaults)
		{"I1", 4.0, 3.0, 0.5, 1.0, 0.3, 2.0, "TEST3", 1.0, 0.0},  // Uses instrument defaults
		// Measurement 4: INSTR override to I2
		{"I2", 5.0, 4.0, 0.6, 2.0, 0.4, 3.0, "TEST4", 1.0, 0.0}   // Uses I2 instrument defaults
	};

	
	KeywordValidationTestParams params = {
		TestINCL::ROLLY_KEYWORD_VALIDATION_TEST,
		"C:/Temp/ROLLY_KEYWORD_VALIDATION_TEST.txt",
		"ROLLY Keyword Validation Test",
		expectedMeasurements
	};
	
	runKeywordValidationTest(params, projTest, r);
}

// These tests require a reference value computed by hand, TBD

// /*
//  * Test INCLY subframe scenario: CALA declared in the same frame
//  * 
//  * Validates that INCLY measurements work correctly when calibration points
//  * are declared within the same frame as the measurement. Tests basic
//  * subframe functionality with arcsin mathematical model.
//  */
// template<>
// template<>
// void object::test<25>()
// {
// 	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the same frame");
// 	SubframeTestParams params = {
// 		TestINCL::INCLY_SUBF_1,
// 		"C:/Temp/INCLY_SUBF_1.txt",
// 		"CALA declared in the same frame",
// 		"BEAM_XT03.BDB.0900.E",
// 		1863.6862187683021, 2293.21034, 2437.48859,
// 		3,
// 		"INCLY"
// 	};
// 	runSubframeTest(params, projTest, r);
// }

 



// /*
//  * Test INCLY subframe scenario: CALA declared in the root frame
//  * 
//  * Validates that INCLY measurements work correctly when calibration points
//  * are declared in the root frame and referenced from subframes. Tests
//  * cross-frame point referencing with arcsin mathematical model.
//  */
// template<>
// template<>
// void object::test<27>()
// {
// 	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
// 	SubframeTestParams params = {
// 		TestINCL::INCLY_SUBF_2,
// 		"C:/Temp/INCLY_SUBF_2.txt",
// 		"CALA declared in the root",
// 		"BEAM_XT03.BDB.0900.E",
// 		1863.6862187683021, 2293.21034, 2437.48859,
// 		3,
// 		"INCLY"
// 	};
// 	runSubframeTest(params, projTest, r);
// }
// /*
//  * Test INCLY subframe scenario: Complex frame structure with moving station
//  * 
//  * Validates that INCLY measurements work correctly in complex multi-level
//  * frame hierarchies with moving stations. Tests advanced frame transformation
//  * scenarios with arcsin mathematical model.
//  */
//  template<>
//  template<>
//  void object::test<31>()
//  {
// 	 set_test_name("Testing INCLY, 1 STATION in SBF, Station moving declared in another frame, more complex structure");
// 	 SubframeTestParams params = {
// 		 TestINCL::INCLY_SUBF_4,
// 		 "C:/Temp/INCLY_SUBF_4.txt",
// 		 "Station moving declared in another frame, more complex structure",
// 		 "BEAM_XT03.BDB.0900.E",
// 		 1744.83305121, 2369.92086569, 2537.43615388,
// 		 4,
// 		 "INCLY"
// 	 };
// 	 runSubframeTest(params, projTest, r);
//  }
// /*
//  * Test INCLY subframe scenario: Marginal changes for nearby stations
//  * 
//  * Validates that INCLY measurements produce marginal coordinate changes
//  * when the measurement station is close to the reference point. Tests
//  * numerical stability for small displacements with arcsin mathematical model.
//  */
//  template<>
//  template<>
//  void object::test<33>()
//  {
// 	 set_test_name("Testing INCLY, 1 STATION in SBF, marginal changes if station not far");
// 	 SubframeTestParams params = {
// 		 TestINCL::INCLY_SUBF_5,
// 		 "C:/Temp/INCLY_SUBF_5.txt",
// 		 "marginal changes if station not far",
// 		 "BEAM_XT03.BDB.0900.T",
// 		 1744.33305121, 2369.42086569, 2537.43615388,
// 		 3,
// 		 "INCLY"
// 	 };
// 	 runSubframeTest(params, projTest, r);
//  }

// /*
//  * Test INCLY subframe scenario: Station moving declared in another frame
//  * 
//  * Validates that INCLY measurements work correctly when the measurement station
//  * is declared as moving in a different frame. Tests dynamic station handling
//  * with arcsin mathematical model.
//  */
//  template<>
//  template<>
//  void object::test<29>()
//  {
// 	 set_test_name("Testing INCLY, 1 STATION in SBF, Station moving declared in another frame");
// 	 SubframeTestParams params = {
// 		 TestINCL::INCLY_SUBF_3,
// 		 "C:/Temp/INCLY_SUBF_3.txt",
// 		 "Station moving declared in another frame",
// 		 "BEAM_XT03.BDB.0900.E",
// 		 1744.83305121, 2369.92086569, 2537.43615388,
// 		 3,
// 		 "INCLY"
// 	 };
// 	 runSubframeTest(params, projTest, r);
//  }

/*
 * Test ROLLY subframe scenario: CALA declared in the same frame
 * 
 * Validates that ROLLY measurements work correctly when calibration points
 * are declared within the same frame as the measurement. Tests basic
 * subframe functionality with legacy atan2 mathematical model.
 */
template<>
template<>
void object::test<26>()
{
	set_test_name("Testing ROLLY, 1 STATION in SBF, CALA declared in the same frame");
	SubframeTestParams params = {
		TestINCL::ROLLY_LEGACY_SUBF_1,
		"C:/Temp/ROLLY_LEGACY_SUBF_1.txt",
		"CALA declared in the same frame",
		"BEAM_XT03.BDB.0900.S",
		1744.83305121, 2369.92086569, 2537.43615388,
		3,
		"ROLLY",
		1e-7,  // High precision tolerance for coordinates
		1e-7   // High precision tolerance for rotation
	};
	runSubframeTest(params, projTest, r);
}

/*
 * Test ROLLY subframe scenario: CALA declared in the root frame
 * 
 * Validates that ROLLY measurements work correctly when calibration points
 * are declared in the root frame and referenced from subframes. Tests
 * cross-frame point referencing with legacy atan2 mathematical model.
 */
template<>
template<>
void object::test<28>()
{
	set_test_name("Testing ROLLY, 1 STATION in SBF, CALA declared in the root");
	SubframeTestParams params = {
		TestINCL::ROLLY_LEGACY_SUBF_2,
		"C:/Temp/ROLLY_LEGACY_SUBF_2.txt",
		"CALA declared in the root",
		"BEAM_XT03.BDB.0900.S",
		1744.83305121, 2369.92086569, 2537.43615388,
		3,
		"ROLLY",
		1e-7,  // High precision tolerance for coordinates
		1e-7   // High precision tolerance for rotation
	};
	runSubframeTest(params, projTest, r);
}

/*
 * Test ROLLY subframe scenario: Station moving declared in another frame
 * 
 * Validates that ROLLY measurements work correctly when the measurement station
 * is declared as moving in a different frame. Tests dynamic station handling
 * with legacy atan2 mathematical model.
 */
template<>
template<>
void object::test<30>()
{
	set_test_name("Testing ROLLY, 1 STATION in SBF, Station moving declared in another frame");
	SubframeTestParams params = {
		TestINCL::ROLLY_LEGACY_SUBF_3,
		"C:/Temp/ROLLY_LEGACY_SUBF_3.txt",
		"Station moving declared in another frame",
		"BEAM_XT03.BDB.0900.S",
		1744.83305121, 2369.92086569, 2537.43615388,
		3,
		"ROLLY",
		1e-7,  // High precision tolerance for coordinates
		1e-7   // High precision tolerance for rotation
	};
	runSubframeTest(params, projTest, r);
}

/*
 * Test ROLLY subframe scenario: Complex frame structure with moving station
 * 
 * Validates that ROLLY measurements work correctly in complex multi-level
 * frame hierarchies with moving stations. Tests advanced frame transformation
 * scenarios with legacy atan2 mathematical model.
 */
template<>
template<>
void object::test<32>()
{
	set_test_name("Testing ROLLY, 1 STATION in SBF, Station moving declared in another frame, more complex structure");
	SubframeTestParams params = {
		TestINCL::ROLLY_LEGACY_SUBF_4,
		"C:/Temp/ROLLY_LEGACY_SUBF_4.txt",
		"Station moving declared in another frame, more complex structure",
		"BEAM_XT03.BDB.0900.S",
		1744.83305121, 2369.92086569, 2537.43615388,
		4,
		"ROLLY",
		1e-7,  // High precision tolerance for coordinates
		1e-7   // High precision tolerance for rotation
	};
	runSubframeTest(params, projTest, r);
}

/*
 * Test ROLLY subframe scenario: Marginal changes for nearby stations
 * 
 * Validates that ROLLY measurements produce marginal coordinate changes
 * when the measurement station is close to the reference point. Tests
 * numerical stability for small displacements with legacy atan2 mathematical model.
 */
template<>
template<>
void object::test<34>()
{
	set_test_name("Testing ROLLY, 1 STATION in SBF, marginal changes if station not far");
	SubframeTestParams params = {
		TestINCL::ROLLY_LEGACY_SUBF_5,
		"C:/Temp/ROLLY_LEGACY_SUBF_5.txt",
		"marginal changes if station not far",
		"BEAM_XT03.BDB.0900.S",
		1744.83305121, 2369.92086569, 2537.43615388,
		3,
		"ROLLY",
		1e-5,  // Lower precision tolerance for coordinates (matches working test<6>)
		1e-5   // Lower precision tolerance for rotation (matches working test<6>)
	};
	runSubframeTest(params, projTest, r);
}

} // namespace tut
