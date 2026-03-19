#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include <testSAG.h>
#include <LGCAdjustableSag.h>
#include <AdjObjectsReader.h>
#include <StringManager.h>
#include <TLGCSagConstraintPair.h>
#include <SagConstraintReader.h>
#include "TLGCCalculation.h"
#include <Behavior.h>
#include <type_traits>

namespace tut
{
	struct test_SAG 
	{
		test_SAG() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
		std::shared_ptr<TLGCData> projTest;
		TReader reader;
	};
	typedef test_group<test_SAG> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Test SAG adjustable object");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{
		set_test_name("Testing SAG element parser.");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		TKeySAGELEMENT r1(*projTest);
		r1.parse(tokenizefileString("*SAGELEMENT sag0 frame0 2 3 4 5 VS VC RS RC"), true, 0);
		r1.parse(tokenizefileString("*SAGELEMENT sag1 frame1 3 4 5 6 VS RC"), true, 0);

		ensure_equals("There need to be two sag elements", projTest->getSags().numObjects(), 2);
		LGCAdjustableSag firstSag = projTest->getSags().getObject("sag0");
		LGCAdjustableSag secondSag = projTest->getSags().getObject("sag1");


		//ensure_equals("Reading file successful", succesReading, true);

		ensure_equals("element name does not match", firstSag.getName(), "sag0");
		ensure_equals("sag name does not match", firstSag.getBaseFrame(), "frame0");

		ensure_equals("bearing is wrong", firstSag.getBearing().getEstimatedValue().getGonsValue(), 0);
		ensure_equals("vertical sag is wrong", firstSag.getVertSag().getEstimatedValue(), 2);
		ensure_equals("vertical curvature is wrong", firstSag.getVertCurv().getEstimatedValue(), 3);
		ensure_equals("radial sag is wrong", firstSag.getRadSag().getEstimatedValue(), 4);
		ensure_equals("radial curvature is wrong", firstSag.getRadCurv().getEstimatedValue(), 5);
		std::bitset<5> expected0("00000");
		ensure_equals("Bearing freedom assigned correctly", firstSag.getBearing().isFixed(), expected0.test(0));
		ensure_equals("VS freedom not assigned correctly", firstSag.getVertSag().isFixed(), expected0.test(1));
		ensure_equals("VC freedom not assigned correctly", firstSag.getVertCurv().isFixed(), expected0.test(2));
		ensure_equals("RS freedom not assigned correctly", firstSag.getRadSag().isFixed(), expected0.test(3));
		ensure_equals("RC freedom not assigned correctly", firstSag.getRadCurv().isFixed(), expected0.test(4));
		ensure_equals("wrong number of unknowns in sag element", firstSag.getNumUnkn(), 5);
		// sag1 has a different fixed state signature
		std::bitset<5> expected1("01100");	
		ensure_equals("Bearing freedom not assigned correctly", secondSag.getBearing().isFixed(), expected1.test(0));
		ensure_equals("VS freedom not assigned correctly", secondSag.getVertSag().isFixed(), expected1.test(1));
		ensure_equals("VC freedom not assigned correctly", secondSag.getVertCurv().isFixed(), expected1.test(2));
		ensure_equals("RS freedom not assigned correctly", secondSag.getRadSag().isFixed(), expected1.test(3));
		ensure_equals("RC freedom not assigned correctly", secondSag.getRadCurv().isFixed(), expected1.test(4));
		ensure_equals("wrong number of unknowns in sag element", secondSag.getNumUnkn(), 3);

		int firstIdx = 17;
		firstSag.setFirstUidx(firstIdx);
		ensure_equals("first unknown index is wrong", firstSag.getFirstUidx(), firstIdx);
		ensure_equals("VS index not assigned correctly", firstSag.getVertSag().getFirstUidx(), firstIdx + 1);
		ensure_equals("VC index not assigned correctly", firstSag.getVertCurv().getFirstUidx(), firstIdx + 2);
		ensure_equals("RS index not assigned correctly", firstSag.getRadSag().getFirstUidx(), firstIdx + 3);
		ensure_equals("RC index not assigned correctly", firstSag.getRadCurv().getFirstUidx(), firstIdx + 4);

		secondSag.setFirstUidx(firstIdx);
		ensure_equals("first unknown index is wrong", secondSag.getFirstUidx(), firstIdx);
		ensure_equals("last unknown index is wrong", secondSag.getLastUidx(), firstIdx + secondSag.getNumUnkn() - 1);
		ensure_equals("VS index not assigned correctly", secondSag.getVertSag().getFirstUidx(), firstIdx + 1);
		try
		{
			secondSag.getVertCurv().getFirstUidx();
			fail("should have thrown exception because of invalid index");
		}
		catch (const std::logic_error &e)
		{
		}
		try
		{
			secondSag.getRadSag().getFirstUidx();
			fail("should have thrown exception because of invalid index");
		}
		catch (const std::logic_error &e)
		{
		}
		ensure_equals("RC index not assigned correctly", secondSag.getRadCurv().getFirstUidx(), firstIdx + 2);
	}

	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Testing standard SAG use");
		TLength vertSag(2);
		TLength vertCurv(3);
		TLength radSag(4);
		TLength radCurv(5);
		TLength slack(6);
		std::bitset<5> freedomSignature("00000");
		LGCAdjustableSag sagObject("firstSagElement", "baseFrame",vertSag,vertCurv,radSag,radCurv,freedomSignature);
		int testIdx = 7;
		sagObject.setFirstUidx(testIdx);
		ensure_equals("index mismatch", sagObject.getFirstUidx(), testIdx);

		ensure_equals("degree of freedom mismatch", sagObject.getNumUnkn(), 5);
		
		// test correction method
		Eigen::Vector<double, 5> currentEst;
		currentEst << sagObject.getBearing().getEstimatedValue(), sagObject.getVertSag().getEstimatedValue(), sagObject.getVertCurv().getEstimatedValue(),
			sagObject.getRadSag().getEstimatedValue(), sagObject.getRadCurv().getEstimatedValue();
		// test set correction method
		Eigen::Vector<double, 5> difference;
		for (int j = 0; j < sagObject.getNumUnkn(); j++)
		{
			// add j to index j
			sagObject.setCorrection(j + sagObject.getFirstUidx(), j);
			difference(j) = j;
		}
		Eigen::Vector<double, 5> updatedEst;
		updatedEst << sagObject.getBearing().getEstimatedValue(), sagObject.getVertSag().getEstimatedValue(), sagObject.getVertCurv().getEstimatedValue(),
			sagObject.getRadSag().getEstimatedValue(), sagObject.getRadCurv().getEstimatedValue();

		ensure_equals("udpate of adjustable sag estimate failed.", (updatedEst - currentEst - difference).norm(), 0);
	}

	template<>
	template<>
	void object::test<3>()
	{
		set_test_name("Testing basic vertical sag calculations");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_vertical);
		projTest->getFileLogger().writeReportHeader("LGC output file");

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);


		LGCAdjustableSagCollection &sags = projTest.get()->getSags();
		LGCAdjustableSag testSag = sags.getObject("sag0");
		ensure_equals("a-posteriori sigma must be 0", projTest->getS0APosteriori(), 0.0, 1e-9);
		ensure_equals("element name does not match", testSag.getName(), "sag0");
		ensure_equals("Vertical sag must be 0.5m", testSag.getVertSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("Vertical curvature must be 1", testSag.getVertCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
	}
	
	template<>
	template<>
	void object::test<4>()
	{
		set_test_name("Testing basic radial sag calculations");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_radial);
		projTest->getFileLogger().writeReportHeader("LGC output file");

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);


		LGCAdjustableSagCollection& sags = projTest.get()->getSags();
		LGCAdjustableSag testSag = sags.getObject("sag0");
		ensure_equals("a-posteriori sigma must be 0", projTest->getS0APosteriori(), 0.0, 1e-9);
		ensure_equals("element name does not match", testSag.getName(), "sag0");
		ensure_equals("Radial sag must be 0.5m", testSag.getRadSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("Radial curvature must be 1", testSag.getRadCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
	}

	template<>
	template<>
	void object::test<5>()
	{
		set_test_name("Testing estimation of sag element bearing and y offset");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_bearingAndYOffset);
		projTest->getFileLogger().writeReportHeader("LGC output file");

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		LGCAdjustableSagCollection &sags = projTest.get()->getSags();
		LGCAdjustableSag &testSag = sags.getObject("sag0");
		ensure_equals("a-posteriori sigma must be 0", projTest->getS0APosteriori(), 0.0, 1e-9);
		ensure_equals("element name does not match", testSag.getName(), "sag0");
		ensure_equals("Vertical sag must be 0.5m", testSag.getVertSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("Vertical curvature must be 1", testSag.getVertCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
		ensure_equals("Radial sag must be 0.5m", testSag.getRadSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("Radial curvature must be 1", testSag.getRadCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
		// get the sag element frame and look at RZ rotation, has to match with the 'giveBearingFrame'
		double givenBearing = projTest.get()->locateNode("giveBearingAndTranslationFrame").node->data.get()->frame.getEstRotation(2).getGonsValue();
		double givenYOffset = projTest.get()->locateNode("giveBearingAndTranslationFrame").node->data.get()->frame.getEstTranslation(1).getMetresValue();
		double estimatedYOffsetViaSagFrame = projTest.get()->locateNode("sagFrame").node->data.get()->frame.getEstTranslation(1).getMetresValue();
		// both estimated bearings are connected via the bearing constraint, so they should be equal
		double estimatedBearingViaSagframe = projTest.get()->locateNode("sagFrame").node->data.get()->frame.getEstRotation(2).getGonsValue();
		double estimatedBearingViaSagElement = projTest.get()->getSags().getObject("sag0").getBearing().getEstimatedValue().getGonsValue();
		ensure_equals("Sag frame rz value should be equal to given bearing", givenBearing, estimatedBearingViaSagframe, 1e-9);
		ensure_equals("Sag element estimated bearing should be equal to given bearing", givenBearing, estimatedBearingViaSagElement, 1e-9);
		ensure_equals("estimated sag frame y offset should be equal to given y offset", givenYOffset, estimatedYOffsetViaSagFrame, 1e-9);
	}

	template<>
	template<>
	void object::test<6>()
	{
		set_test_name("Testing sag pair constructor with only one point.");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_pairs);
		projTest->getFileLogger().writeReportHeader("LGC output file");

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		
		// test for the correct estimation of the sag parameters
		LGCAdjustableSagCollection &sags = projTest.get()->getSags();
		LGCAdjustableSag &testSag = sags.getObject("sag0");
		ensure_equals("Vertical sag must be -0.5m", testSag.getVertSag().getEstimatedValue().getMetresValue(), -0.5, 1e-9);
		ensure_equals("Vertical curvature must be 0.5", testSag.getVertCurv().getEstimatedValue().getMetresValue(), 0.5, 1e-9);

		// test if the sag transformation was correctly transfered to the provisional point
		LGCAdjustablePoint &provDeformed = projTest.get()->getPoints().getObject("P_provisional_deformed");
		TPositionVector estCoords = provDeformed.getEstimatedValue();
		TPositionVector provCoords = provDeformed.getProvisionalValue();

		ensure_equals("X-coordinate should be at provisional value", (estCoords - provCoords).getX().getMetresValue(), 0, 1e-9);
		ensure_equals("Y-coordinate should be at provisional value", (estCoords - provCoords).getY().getMetresValue(), 0, 1e-9);
		ensure_equals("Z-coordinate should be at the sagged provisional value z coordinate", estCoords.getZ().getMetresValue(), -0.5, 1e-9);
	}

	template<>
	template<>
	void object::test<7>()
	{
		set_test_name("Testing sag pair constructor and estimation purely with sagged provisional values.");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_Provisional);
		projTest->getFileLogger().writeReportHeader("LGC output file");

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
		
		// test for the correct estimation of the sag parameters
		LGCAdjustableSagCollection &sags = projTest.get()->getSags();
		LGCAdjustableSag &testSag = sags.getObject("sag0");
		ensure_equals("Vertical sag must be -0.5m", testSag.getVertSag().getEstimatedValue().getMetresValue(), -0.5, 1e-9);
		ensure_equals("Vertical curvature must be 0.5", testSag.getVertCurv().getEstimatedValue().getMetresValue(), 0.5, 1e-9);

		// test if the sag transformation was correctly transfered to the provisional point
		LGCAdjustablePoint &provDeformed = projTest.get()->getPoints().getObject("P_will_be_fixed_to_sagged_provisional");
		TPositionVector estCoords = provDeformed.getEstimatedValue();
		TPositionVector provCoords = provDeformed.getProvisionalValue();

		ensure_equals("X-coordinate should be at provisional value", (estCoords - provCoords).getX().getMetresValue(), 0, 1e-9);
		ensure_equals("Y-coordinate should be at provisional value", (estCoords - provCoords).getY().getMetresValue(), 0, 1e-9);
		ensure_equals("Z-coordinate should be at the sagged provisional value z coordinate", estCoords.getZ().getMetresValue(), -0.5, 1e-9);
	}

	template<>
	template<>
	void object::test<8>()
	{
		set_test_name("Testing DEFORM syntax with point sigma transfer to _ref points");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_DEFORM_syntax);
		projTest->getFileLogger().writeReportHeader("LGC output file");

		bool succesReading = reader.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		// Test a-posteriori sigma
		ensure_equals("a-posteriori sigma must be 0", projTest->getS0APosteriori(), 0.0, 1e-9);

		// Test estimated sag parameters
		LGCAdjustableSag &testSag = projTest->getSags().getObject("mySag1");
		ensure_equals("Vertical sag must be 0", testSag.getVertSag().getEstimatedValue().getMetresValue(), 0.0, 1e-9);
		ensure_equals("Vertical curvature must be -1", testSag.getVertCurv().getEstimatedValue().getMetresValue(), -1.0, 1e-9);

		// Test P1_1 (sagged point): fully free, no pointSigma
		LGCAdjustablePoint &P1_1 = projTest->getPoints().getObject("P1_1");
		ensure_equals("P1_1 X should be free", P1_1.isCoordinateFixed(0), false);
		ensure_equals("P1_1 Y should be free", P1_1.isCoordinateFixed(1), false);
		ensure_equals("P1_1 Z should be free", P1_1.isCoordinateFixed(2), false);
		ensure_equals("P1_1 should have no pointSigma", P1_1.hasPointSigma(), false);

		// Test P1_1_ref: fully free (original was *POIN with SX 1 SY 1 SZ 1), has pointSigma
		LGCAdjustablePoint &P1_1_ref = projTest->getPoints().getObject("P1_1_ref");
		ensure_equals("P1_1_ref X should be free", P1_1_ref.isCoordinateFixed(0), false);
		ensure_equals("P1_1_ref Y should be free", P1_1_ref.isCoordinateFixed(1), false);
		ensure_equals("P1_1_ref Z should be free", P1_1_ref.isCoordinateFixed(2), false);
		ensure_equals("P1_1_ref should have pointSigma", P1_1_ref.hasPointSigma(), true);

		// Test P5_1 (sagged point): fully free
		LGCAdjustablePoint &P5_1 = projTest->getPoints().getObject("P5_1");
		ensure_equals("P5_1 X should be free", P5_1.isCoordinateFixed(0), false);
		ensure_equals("P5_1 Y should be free", P5_1.isCoordinateFixed(1), false);
		ensure_equals("P5_1 Z should be free", P5_1.isCoordinateFixed(2), false);

		// Test P5_1_ref: only Y free (SX 0, SY 1, SZ 0), has pointSigma
		LGCAdjustablePoint &P5_1_ref = projTest->getPoints().getObject("P5_1_ref");
		ensure_equals("P5_1_ref X should be fixed", P5_1_ref.isCoordinateFixed(0), true);
		ensure_equals("P5_1_ref Y should be free", P5_1_ref.isCoordinateFixed(1), false);
		ensure_equals("P5_1_ref Z should be fixed", P5_1_ref.isCoordinateFixed(2), true);
		ensure_equals("P5_1_ref should have pointSigma", P5_1_ref.hasPointSigma(), true);

		// Test that sag constraint pairs have correct structure:
		// - refPoint should end with "_ref"
		// - assocPoint should be the original name (sagged point, no "_ref" suffix)
		for (const auto &pair : projTest->getSagPointPairs())
		{
			// Reference point should end with "_ref"
			ensure("Ref point should end with _ref suffix",
				pair.refPoint.size() > 4 && pair.refPoint.substr(pair.refPoint.size() - 4) == "_ref");
			// Associated (sagged) point should NOT have "_ref" suffix
			ensure("Assoc point should not have _ref suffix",
				pair.assocPoint.find("_ref") == std::string::npos);
			// Both points should exist
			ensure("Reference point should exist", projTest->getPoints().doesObjectExist(pair.refPoint));
			ensure("Associated point should exist", projTest->getPoints().doesObjectExist(pair.assocPoint));
		}
	}

};
