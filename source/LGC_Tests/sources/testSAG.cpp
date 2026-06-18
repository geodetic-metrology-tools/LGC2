// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

		// Helper: run and verify DEFORM expansion tests common to both point-level and frame-level syntax
		void runDeformExpansionChecks(const char *inputData, const std::string &testLabel)
		{
			projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
			std::stringstream infiler(inputData);
			projTest->getFileLogger().writeReportHeader("LGC output file");

			bool succesReading = reader.read(infiler);
			ensure_equals(testLabel + ": Reading file successful", succesReading, true);
			TLGCCalculation calcul(projTest);
			std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
			Behavior succesCalc = calcul.computeResults(fileWriter);
			ensure_equals(testLabel + ": Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

			// Test a-posteriori sigma
			ensure_equals(testLabel + ": a-posteriori sigma must be 0", projTest->getS0APosteriori(), 0.0, 1e-9);

			// Test estimated sag parameters
			LGCAdjustableSag &testSag = projTest->getSags().getObject("mySag1");
			ensure_equals(testLabel + ": Z sag must be 0", testSag.getZSag().getEstimatedValue().getMetresValue(), 0.0, 1e-9);
			ensure_equals(testLabel + ": Z curvature must be -1", testSag.getZCurv().getEstimatedValue().getMetresValue(), -1.0, 1e-9);

			// Test P1_1 (sagged point): fully free, no pointSigma
			LGCAdjustablePoint &P1_1 = projTest->getPoints().getObject("P1_1");
			ensure_equals(testLabel + ": P1_1 X should be free", P1_1.isCoordinateFixed(0), false);
			ensure_equals(testLabel + ": P1_1 Y should be free", P1_1.isCoordinateFixed(1), false);
			ensure_equals(testLabel + ": P1_1 Z should be free", P1_1.isCoordinateFixed(2), false);
			ensure_equals(testLabel + ": P1_1 should have no pointSigma", P1_1.hasPointSigma(), false);

			// Test P1_1_ref_mySag1: fully free (original was *POIN with SX 1 SY 1 SZ 1), has pointSigma
			LGCAdjustablePoint &P1_1_ref = projTest->getPoints().getObject("P1_1_ref_mySag1");
			ensure_equals(testLabel + ": P1_1_ref X should be free", P1_1_ref.isCoordinateFixed(0), false);
			ensure_equals(testLabel + ": P1_1_ref Y should be free", P1_1_ref.isCoordinateFixed(1), false);
			ensure_equals(testLabel + ": P1_1_ref Z should be free", P1_1_ref.isCoordinateFixed(2), false);
			ensure_equals(testLabel + ": P1_1_ref should have pointSigma", P1_1_ref.hasPointSigma(), true);

			// Test P5_1 (sagged point): fully free
			LGCAdjustablePoint &P5_1 = projTest->getPoints().getObject("P5_1");
			ensure_equals(testLabel + ": P5_1 X should be free", P5_1.isCoordinateFixed(0), false);
			ensure_equals(testLabel + ": P5_1 Y should be free", P5_1.isCoordinateFixed(1), false);
			ensure_equals(testLabel + ": P5_1 Z should be free", P5_1.isCoordinateFixed(2), false);

			// Test P5_1_ref_mySag1: only Y free (SX 0, SY 1, SZ 0), has pointSigma
			LGCAdjustablePoint &P5_1_ref = projTest->getPoints().getObject("P5_1_ref_mySag1");
			ensure_equals(testLabel + ": P5_1_ref X should be fixed", P5_1_ref.isCoordinateFixed(0), true);
			ensure_equals(testLabel + ": P5_1_ref Y should be free", P5_1_ref.isCoordinateFixed(1), false);
			ensure_equals(testLabel + ": P5_1_ref Z should be fixed", P5_1_ref.isCoordinateFixed(2), true);
			ensure_equals(testLabel + ": P5_1_ref should have pointSigma", P5_1_ref.hasPointSigma(), true);

			// Test that sag constraint pairs have correct structure
			for (const auto &pair : projTest->getSagPointPairs())
			{
				ensure(testLabel + ": Ref point should contain _ref_<sagname> marker",
					pair.getRefPoint().find("_ref_") != std::string::npos);
				ensure(testLabel + ": Assoc point should not contain _ref_ marker",
					pair.getAssocPoint().find("_ref_") == std::string::npos);
				ensure(testLabel + ": Reference point should exist", projTest->getPoints().doesObjectExist(pair.getRefPoint()));
				ensure(testLabel + ": Associated point should exist", projTest->getPoints().doesObjectExist(pair.getAssocPoint()));
			}
		}
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
		r1.parse(tokenizefileString("*SAGELEMENT sag0 frame0 2 3 4 5 ZS ZC XS XC"), true, 0);
		r1.parse(tokenizefileString("*SAGELEMENT sag1 frame1 3 4 5 6 ZS XC"), true, 0);

		ensure_equals("There need to be two sag elements", projTest->getSags().numObjects(), 2);
		LGCAdjustableSag firstSag = projTest->getSags().getObject("sag0");
		LGCAdjustableSag secondSag = projTest->getSags().getObject("sag1");

		ensure_equals("element name does not match", firstSag.getName(), "sag0");
		ensure_equals("sag name does not match", firstSag.getBaseFrame(), "frame0");

		ensure_equals("z sag is wrong", firstSag.getZSag().getEstimatedValue(), 2);
		ensure_equals("z curvature is wrong", firstSag.getZCurv().getEstimatedValue(), 3);
		ensure_equals("x sag is wrong", firstSag.getXSag().getEstimatedValue(), 4);
		ensure_equals("x curvature is wrong", firstSag.getXCurv().getEstimatedValue(), 5);
		std::bitset<4> expected0("0000");
		ensure_equals("ZS freedom not assigned correctly", firstSag.getZSag().isFixed(), expected0.test(0));
		ensure_equals("ZC freedom not assigned correctly", firstSag.getZCurv().isFixed(), expected0.test(1));
		ensure_equals("XS freedom not assigned correctly", firstSag.getXSag().isFixed(), expected0.test(2));
		ensure_equals("XC freedom not assigned correctly", firstSag.getXCurv().isFixed(), expected0.test(3));
		ensure_equals("wrong number of unknowns in sag element", firstSag.getNumUnkn(), 4);
		// sag1 has a different fixed state signature (ZS and XC free)
		std::bitset<4> expected1("0110");
		ensure_equals("ZS freedom not assigned correctly", secondSag.getZSag().isFixed(), expected1.test(0));
		ensure_equals("ZC freedom not assigned correctly", secondSag.getZCurv().isFixed(), expected1.test(1));
		ensure_equals("XS freedom not assigned correctly", secondSag.getXSag().isFixed(), expected1.test(2));
		ensure_equals("XC freedom not assigned correctly", secondSag.getXCurv().isFixed(), expected1.test(3));
		ensure_equals("wrong number of unknowns in sag element", secondSag.getNumUnkn(), 2);

		int firstIdx = 17;
		firstSag.setFirstUidx(firstIdx);
		ensure_equals("first unknown index is wrong", firstSag.getFirstUidx(), firstIdx);
		ensure_equals("ZS index not assigned correctly", firstSag.getZSag().getFirstUidx(), firstIdx + 0);
		ensure_equals("ZC index not assigned correctly", firstSag.getZCurv().getFirstUidx(), firstIdx + 1);
		ensure_equals("XS index not assigned correctly", firstSag.getXSag().getFirstUidx(), firstIdx + 2);
		ensure_equals("XC index not assigned correctly", firstSag.getXCurv().getFirstUidx(), firstIdx + 3);

		secondSag.setFirstUidx(firstIdx);
		ensure_equals("first unknown index is wrong", secondSag.getFirstUidx(), firstIdx);
		ensure_equals("last unknown index is wrong", secondSag.getLastUidx(), firstIdx + secondSag.getNumUnkn() - 1);
		ensure_equals("ZS index not assigned correctly", secondSag.getZSag().getFirstUidx(), firstIdx + 0);
		try
		{
			secondSag.getZCurv().getFirstUidx();
			fail("should have thrown exception because of invalid index");
		}
		catch (const std::logic_error &e)
		{
		}
		try
		{
			secondSag.getXSag().getFirstUidx();
			fail("should have thrown exception because of invalid index");
		}
		catch (const std::logic_error &e)
		{
		}
		ensure_equals("XC index not assigned correctly", secondSag.getXCurv().getFirstUidx(), firstIdx + 1);
	}

	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Testing standard SAG use");
		TLength zSag(2);
		TLength zCurv(3);
		TLength xSag(4);
		TLength xCurv(5);
		std::bitset<4> freedomSignature("0000");
		LGCAdjustableSag sagObject("firstSagElement", "baseFrame", zSag, zCurv, xSag, xCurv, freedomSignature);
		int testIdx = 7;
		sagObject.setFirstUidx(testIdx);
		ensure_equals("index mismatch", sagObject.getFirstUidx(), testIdx);

		ensure_equals("degree of freedom mismatch", sagObject.getNumUnkn(), 4);

		// test correction method
		Eigen::Vector<double, 4> currentEst;
		currentEst << sagObject.getZSag().getEstimatedValue(), sagObject.getZCurv().getEstimatedValue(),
			sagObject.getXSag().getEstimatedValue(), sagObject.getXCurv().getEstimatedValue();
		// test set correction method
		Eigen::Vector<double, 4> difference;
		for (int j = 0; j < sagObject.getNumUnkn(); j++)
		{
			sagObject.setCorrection(j + sagObject.getFirstUidx(), j);
			difference(j) = j;
		}
		Eigen::Vector<double, 4> updatedEst;
		updatedEst << sagObject.getZSag().getEstimatedValue(), sagObject.getZCurv().getEstimatedValue(),
			sagObject.getXSag().getEstimatedValue(), sagObject.getXCurv().getEstimatedValue();

		ensure_equals("udpate of adjustable sag estimate failed.", (updatedEst - currentEst - difference).norm(), 0);
	}

	template<>
	template<>
	void object::test<3>()
	{
		set_test_name("Testing basic z sag calculations");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_z);
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
		ensure_equals("Z sag must be 0.5m", testSag.getZSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("Z curvature must be 1", testSag.getZCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
	}

	template<>
	template<>
	void object::test<4>()
	{
		set_test_name("Testing basic x sag calculations");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_x);
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
		ensure_equals("X sag must be 0.5m", testSag.getXSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("X curvature must be 1", testSag.getXCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
	}

	template<>
	template<>
	void object::test<5>()
	{
		set_test_name("Testing estimation of all 4 sag parameters plus sagFrame TY");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_TY_and_all_sag_params);
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
		ensure_equals("Z sag must be 0.5m", testSag.getZSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("Z curvature must be 1", testSag.getZCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
		ensure_equals("X sag must be 0.5m", testSag.getXSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("X curvature must be 1", testSag.getXCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
		// sagFrame TY is the only free frame parameter; must converge to truthYOffsetFrame's TY
		double truthYOffset = projTest.get()->locateNode("truthYOffsetFrame").node->data.get()->frame.getEstTranslation(1).getMetresValue();
		double estimatedYOffsetViaSagFrame = projTest.get()->locateNode("sagFrame").node->data.get()->frame.getEstTranslation(1).getMetresValue();
		ensure_equals("estimated sag frame y offset should equal truth y offset", truthYOffset, estimatedYOffsetViaSagFrame, 1e-9);
	}

	template<>
	template<>
	void object::test<6>()
	{
		set_test_name("Testing *SAGELEMENT pair continuation block + DEFORM tag on a CALA point");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_mixed_pairs_and_DEFORM);
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
		ensure_equals("Z sag must be -0.5m", testSag.getZSag().getEstimatedValue().getMetresValue(), -0.5, 1e-9);
		ensure_equals("Z curvature must be 0.5", testSag.getZCurv().getEstimatedValue().getMetresValue(), 0.5, 1e-9);

		// verify the DEFORM-tagged CALA point gets sagged
		LGCAdjustablePoint &unobservedDeformed = projTest.get()->getPoints().getObject("P_unobserved_deformed");
		TPositionVector estCoords = unobservedDeformed.getEstimatedValue();
		TPositionVector provCoords = unobservedDeformed.getProvisionalValue();

		ensure_equals("X-coordinate should be at provisional value", (estCoords - provCoords).getX().getMetresValue(), 0, 1e-9);
		ensure_equals("Y-coordinate should be at provisional value", (estCoords - provCoords).getY().getMetresValue(), 0, 1e-9);
		ensure_equals("Z-coordinate should be at the sagged value", estCoords.getZ().getMetresValue(), -0.5, 1e-9);
	}

	template<>
	template<>
	void object::test<7>()
	{
		set_test_name("Testing DEFORM-expanded point with no observation converges to sag offset");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_DEFORM_unobserved_point);
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
		ensure_equals("Z sag must be -0.5m", testSag.getZSag().getEstimatedValue().getMetresValue(), -0.5, 1e-9);
		ensure_equals("Z curvature must be 0.5", testSag.getZCurv().getEstimatedValue().getMetresValue(), 0.5, 1e-9);

		// verify the unobserved DEFORM-tagged point converges to the sagged provisional position
		LGCAdjustablePoint &unobservedSagged = projTest.get()->getPoints().getObject("P_unobserved_sagged");
		TPositionVector estCoords = unobservedSagged.getEstimatedValue();
		TPositionVector provCoords = unobservedSagged.getProvisionalValue();

		ensure_equals("X-coordinate should be at provisional value", (estCoords - provCoords).getX().getMetresValue(), 0, 1e-9);
		ensure_equals("Y-coordinate should be at provisional value", (estCoords - provCoords).getY().getMetresValue(), 0, 1e-9);
		ensure_equals("Z-coordinate should be at the sagged value", estCoords.getZ().getMetresValue(), -0.5, 1e-9);
	}

	template<>
	template<>
	void object::test<8>()
	{
		set_test_name("Testing point-level DEFORM syntax with point sigma transfer to _ref_<sagname> points");
		runDeformExpansionChecks(Sag::SAG_test_DEFORM_syntax, "point-level DEFORM");
	}

	template<>
	template<>
	void object::test<9>()
	{
		set_test_name("Testing frame-level DEFORM syntax with point sigma transfer to _ref_<sagname> points");
		runDeformExpansionChecks(Sag::SAG_test_DEFORM_frame_syntax, "frame-level DEFORM");
	}

	template<>
	template<>
	void object::test<10>()
	{
		set_test_name("Testing z sag with 2D points identifies frame rotation");

		projTest->getFileLogger().setOutputfileLocation("C:/Temp/SAG.txt");
		std::stringstream infiler(Sag::SAG_test_zWithRotation);
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
		ensure_equals("Z sag must be 0.5m", testSag.getZSag().getEstimatedValue().getMetresValue(), 0.5, 1e-9);
		ensure_equals("Z curvature must be 1", testSag.getZCurv().getEstimatedValue().getMetresValue(), 1.0, 1e-9);
		double estimatedRZ = projTest.get()->locateNode("sagFrame").node->data.get()->frame.getEstRotation(2).getGonsValue();
		ensure_equals("sagFrame RZ must be 150 gon", estimatedRZ, 150.0, 1e-9);
	}

};
