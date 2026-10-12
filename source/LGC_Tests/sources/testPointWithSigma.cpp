// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#include <tut/tut_macros.hpp>
#pragma warning(pop)

#include <AdjObjectsReader.h>
#include <Behavior.h>
#include <StringManager.h>
#include <TLGCData.h>
#include <TReader.h>

#include "TAdjustablePoint.h"
#include "TDataAnalyzer.h"
#include "TLGCCalculation.h"
#include "testPointWithSigma.h"

namespace tut
{
struct test_pointWithSigma
{
	test_pointWithSigma() : projTest(std::make_shared<TLGCData>()), reader(projTest) {}
	std::shared_ptr<TLGCData> projTest;
	TReader reader;
};
typedef test_group<test_pointWithSigma> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test point with sigmas");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	set_test_name("Testing point definitions with Sigma");
	TKeyPOIN pr(*projTest);

	// allowed setups
	// standard point
	pr.parse(tokenizefileString("P_VXYZ 1 2 3"), true, -1);
	// point with 3 normal sigmas
	pr.parse(tokenizefileString("P1 1 2 3 SX 1 SY 2 SZ 3"), true, -1);
	// point with 2 normal sigmas, 1 sigma equal to 0
	pr.parse(tokenizefileString("P2 1 2 3 SX 1 SY 0 SZ 3"), true, -1);
	// point with 1 normal sigma, 1 sigma equal to 0, 1 sigma not specified
	pr.parse(tokenizefileString("P3 1 2 3 SX 1 SY 0"), true, -1);
	// point with rotations, 3 normal sigmas
	pr.parse(tokenizefileString("P4 1 2 3 BEAR 1 SLOPE 2 ROLL 3 HDEFL 4 SX 1 SY 2 SZ 3"), true, -1);
	// point with rotations, 2 normal sigmas, sigma zero
	pr.parse(tokenizefileString("P5 1 2 3 BEAR 1 SLOPE 2 ROLL 3 HDEFL 4 SX 0 SY 2 SZ 3"), true, -1);
	// point with apriCov matrix (full matrix)
	pr.parse(tokenizefileString("P6 1 2 3 APRICOV MAT(1,0,0,0,4,0,0,0,9)"), true, -1);
	// point with apriCov matrix (diagonal matrix)
	pr.parse(tokenizefileString("P7 1 2 3 APRICOV DIAG(1,4,9)"), true, -1);

	// all different point types
	pr.parse(tokenizefileString("P_CALA 1 2 3 SX 0 SY 0 SZ 0"), true, -1);
	pr.parse(tokenizefileString("P_VX 1 2 3 SY 0 SZ 0"), true, -1);
	pr.parse(tokenizefileString("P_VY 1 2 3 SX 0 SZ 0"), true, -1);
	pr.parse(tokenizefileString("P_VZ 1 2 3 SX 0 SY 0"), true, -1);
	pr.parse(tokenizefileString("P_VXY 1 2 3 SZ 0"), true, -1);
	pr.parse(tokenizefileString("P_VXZ 1 2 3 SY 0"), true, -1);
	pr.parse(tokenizefileString("P_VYZ 1 2 3 SX 0"), true, -1);

	// for checking if indices are set correctly
	TDataAnalyzer analyzer(*projTest);
	bool isConsistent = analyzer.dataConsistent();
	ensure_equals("Setup should still be inconsistent, i.e. because of missing observations.", isConsistent, false);

	const LGCAdjustablePointCollection &ptCollection = projTest->getPoints();
	const LGCAdjustablePoint &P_VXYZ = ptCollection.getObject("P_VXYZ");
	pointSigmaData P_VXYZ_Data = P_VXYZ.getPointSigmaData();
	ensure_equals("P_VXYZ has no fixed dimension.", P_VXYZ.getNumUnkn(), 3);
	ensure_equals("P_VXYZ should introduce no observation.", P_VXYZ_Data.fRelObsIdx.size(), 0);
	ensure_equals("P_VXYZ should introduce no constraint.", P_VXYZ_Data.fRelCIdx.size(), 0);
	ensure_equals("P_VXYZ has no angles set.", P_VXYZ_Data.fHasAngle, false);
	ensure_equals("P_VXYZ has no apriCovMat set.", P_VXYZ_Data.fHasApriCovMat, false);
	ensure_equals("P_VXYZ has no sigma data set.", P_VXYZ.hasPointSigma(), false);

	const LGCAdjustablePoint &P1 = ptCollection.getObject("P1");
	ensure_equals("P1 sigmas are not set correctly.", P1.getPointSigmaData().fSigmas, MM2M * Eigen::Vector3d(1, 2, 3));
	const LGCAdjustablePoint &P2 = ptCollection.getObject("P2");

	pointSigmaData P2Data = P2.getPointSigmaData();
	ensure_equals("P2 sigmas are not set correctly.", P2Data.fSigmas, MM2M * Eigen::Vector3d(1, 0, 3));
	ensure_equals("y coordinate of P2 should be fixed", P2.isCoordinateFixed(1), true);
	ensure_equals("P2 should result in 2 observations.", P2Data.fRelObsIdx.size(), 2);
	ensure_equals("P2 should result in 0 constraints.", P2Data.fRelCIdx.size(), 0);

	const LGCAdjustablePoint &P3 = ptCollection.getObject("P3");
	pointSigmaData P3Data = P3.getPointSigmaData();
	ensure_equals("P3 sigmas are not set correctly.", P3Data.fSigmas.topRows(2), MM2M * Eigen::Vector<double, 2>(1, 0));
	ensure_equals("P3 z sigma should be nan.", std::isnan(P3Data.fSigmas(2)), true);
	ensure_equals("y coordinate of P2 should be fixed", P2.isCoordinateFixed(1), true);
	ensure_equals("P3 should result in 1 observations.", P3Data.fRelObsIdx.size(), 1);
	ensure_equals("P3 should result in 0 constraints.", P3Data.fRelCIdx.size(), 0);

	const LGCAdjustablePoint &P4 = ptCollection.getObject("P4");
	pointSigmaData P4Data = P4.getPointSigmaData();
	Eigen::Vector<double, 4> angles(P4Data.fAngles[0], P4Data.fAngles[1], P4Data.fAngles[2], P4Data.fAngles[3]);
	Eigen::Vector<double, 4> expectedAngles(1, 2, 3, 4);
	ensure_equals("P4 angles set wrong.", angles.isApprox(GON2RAD * expectedAngles), true);
	ensure_equals("P4 .", P4Data.fHasAngle, true);
	ensure_equals("P4 should result in 3 observations.", P4Data.fRelObsIdx.size(), 3);
	ensure_equals("P4 should result in 0 constraints.", P4Data.fRelCIdx.size(), 0);

	const LGCAdjustablePoint &P5 = ptCollection.getObject("P5");
	pointSigmaData P5Data = P5.getPointSigmaData();
	ensure_equals("sigma=0 for p5 should introduce a constraint because of the rotations.", P5Data.firstCIdx, 0);
	ensure_equals("sigma=0 for p5 should introduce a constraint because of the rotations.", P5Data.fRelCIdx.size(), 1);
	ensure_equals("P5 should introduce 2 observations.", P5Data.fRelObsIdx.size(), 2);
	const LGCAdjustablePoint &P6 = ptCollection.getObject("P6");
	pointSigmaData P6Data = P6.getPointSigmaData();
	Eigen::Matrix3d expectedCovMat;
	expectedCovMat << 1, 0, 0, 0, 4, 0, 0, 0, 9;
	ensure_equals("P6 should have a apriori covariance matrix.", P6Data.fHasApriCovMat, true);
	ensure_equals("P6 covar matrix not set correctly.", P6Data.fApriCovMat.isApprox(expectedCovMat), true);
	ensure_equals("P6 should introduce 3 observations.", P6Data.fRelObsIdx.size(), 3);

	const LGCAdjustablePoint &P7 = ptCollection.getObject("P7");
	pointSigmaData P7Data = P7.getPointSigmaData();
	expectedCovMat.setZero();
	expectedCovMat << 1, 0, 0, 0, 4, 0, 0, 0, 9;
	ensure_equals("P7 should have a apriori covariance matrix.", P7Data.fHasApriCovMat, true);
	ensure_equals("P7 covar matrix not set correctly.", P7Data.fApriCovMat.isApprox(expectedCovMat), true);

	// check the point type definitions
	auto freedomSignature = [](auto obj) -> std::array<bool, 3> { return {obj.isCoordinateFixed(0), obj.isCoordinateFixed(1), obj.isCoordinateFixed(2)}; };
	ensure_equals("Fixed coordinate signature not read correctly.", freedomSignature(ptCollection.getObject("P_CALA")) == std::array<bool, 3>{true, true, true}, true);
	ensure_equals("Fixed coordinate signature not read correctly.", freedomSignature(ptCollection.getObject("P_VX")) == std::array<bool, 3>{false, true, true}, true);
	ensure_equals("Fixed coordinate signature not read correctly.", freedomSignature(ptCollection.getObject("P_VY")) == std::array<bool, 3>{true, false, true}, true);
	ensure_equals("Fixed coordinate signature not read correctly.", freedomSignature(ptCollection.getObject("P_VZ")) == std::array<bool, 3>{true, true, false}, true);
	ensure_equals("Fixed coordinate signature not read correctly.", freedomSignature(ptCollection.getObject("P_VXY")) == std::array<bool, 3>{false, false, true}, true);
	ensure_equals("Fixed coordinate signature not read correctly.", freedomSignature(ptCollection.getObject("P_VXZ")) == std::array<bool, 3>{false, true, false}, true);
	ensure_equals("Fixed coordinate signature not read correctly.", freedomSignature(ptCollection.getObject("P_VYZ")) == std::array<bool, 3>{true, false, false}, true);

	// the following setups should fail
	// negative sigma

	ensure_THROW(pr.parse(tokenizefileString("P10 1 2 3 SX 1 SY -2 SZ 3"), true, -1), std::runtime_error);
	// sigma and apriCov at the same time
	ensure_THROW(pr.parse(tokenizefileString("P11 1 2 3 SX 1 SY 2 SZ 3 APRICOV DIAG(1,2,3)"), true, -1), std::runtime_error);
	// unsymmetric apriCov mat
	ensure_THROW(pr.parse(tokenizefileString("P13 1 2 3 SX 1 SY 2 SZ 3 APRICOV MAT(1,0,0,0,1,0,0,1,1)"), true, -1), std::runtime_error);
	// apriCov mat not positive definite
	ensure_THROW(pr.parse(tokenizefileString("P14 1 2 3 SX 1 SY 2 SZ 3 APRICOV DIAG(1,-1,1)"), true, -1), std::runtime_error);
	// point with full apriCov matrix and rotation not allowed
	ensure_THROW(pr.parse(tokenizefileString("P8 1 2 3 BEAR 1 SLOPE 2 ROLL 3 HDEFL 4 APRICOV MAT(1,0,0,0,4,0,0,0,9)"), true, -1), std::runtime_error);
	// specified standard deviation positive but too small
	ensure_THROW(pr.parse(tokenizefileString("P9 1 2 3 SX 1 SY 1 SZ 5e-13"), true, -1), std::runtime_error);

	// point with sigma should only be allowed in a *POIN section, not in *VZ etc..
	TKeyVZ prVZ(*projTest);
	ensure_THROW(prVZ.parse(tokenizefileString("P15 1 2 3 SX 1 SY 1 SZ 3"), true, -1), std::runtime_error);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing calculations with points with sigma.");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(pointWithSigma::computation1);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const LGCAdjustablePointCollection &ptCollection = projTest->getPoints();
	LGCAdjustablePoint P1 = ptCollection.getObject("P1");
	LGCAdjustablePoint P1_aux = ptCollection.getObject("P1_aux");
	// covariance matrices in root have to be identical, the frame setup simulates the rotated coordinate system defined by the point sigma angles of p1
	ensure_equals("P1 and P1_aux need to have equal covariance matrices in ROOT.", P1.getCovarianceMatrix().isApprox(P1_aux.getCovarianceMatrixInRoot()), true);

	LGCAdjustablePoint independent_point = ptCollection.getObject("independentPoint");
	Eigen::Matrix3d expectedCovMat;
	expectedCovMat << 1, 0, 0, 0, 4, 0, 0, 0, 9;
	expectedCovMat *= pow2(MM2M);
	ensure_equals("covariance matrix of point is wrong", independent_point.getCovarianceMatrix().isApprox(expectedCovMat), true);

	// test if the xParallel point moved
	LGCAdjustablePoint xParallel = ptCollection.getObject("xParallel");
	Eigen::Vector3d expectedPosXPar(0, 1, 1);
	ensure_equals("point should be at expected Position.", expectedPosXPar.isApprox(xParallel.getEstimatedValue().toRealVector()), true);
	LGCAdjustablePoint xLine = ptCollection.getObject("xLine");
	Eigen::Vector3d expectedPosXLine(0.5, 0.5, 0);
	ensure_equals("point should be at expected Position.", expectedPosXLine.isApprox(xLine.getEstimatedValue().toRealVector()), true);

	// test if apriori covariance matrices were set correctly and computation gives corresponding result
	LGCAdjustablePoint P2 = ptCollection.getObject("P2");
	Eigen::Matrix3d expectedApriCovP2;
	expectedApriCovP2.setZero();
	expectedApriCovP2.diagonal() << 1, 2, 3;
	ensure_equals("apriori covariance matrix not set correctly.", (expectedApriCovP2 - P2.getPointSigmaData().fApriCovMat).norm(), 0.0);
	ensure_equals("a-posteriori covariance matrix has to match a-priori covariance matrix for P2", P2.getCovarianceMatrix().isApprox(P2.getPointSigmaData().fApriCovMat), true);

	LGCAdjustablePoint P3 = ptCollection.getObject("P3");
	Eigen::Matrix3d expectedApriCovP3;
	expectedApriCovP3 << 10, 2, 3, 2, 40, 2, 3, 2, 30;
	ensure_equals("apriori covariance matrix not set correctly.", (expectedApriCovP3 - P3.getPointSigmaData().fApriCovMat).norm(), 0.0);
	ensure_equals("a-posteriori covariance matrix has to match a-priori covariance matrix for P3", P3.getCovarianceMatrix().isApprox(P3.getPointSigmaData().fApriCovMat), true);

	LGCAdjustablePoint ex = ptCollection.getObject("ex");
	ensure_equals("Offset of a fully fixed point should be zero.", (ex.getPointSigmaData().fRotRes).norm(), 0.0);
	ensure_equals("The diagonal of the weight matrix of a fixed point should be NaN per convention.", ex.getPointSigmaData().fWeightMatrix.diagonal().array().isNaN().all(), true);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing computation with fixed height.");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(pointWithSigma::computationHeightFixed);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const LGCAdjustablePointCollection &ptCollection = projTest->getPoints();
	LGCAdjustablePoint P0 = ptCollection.getObject("P0");
	LGCAdjustablePoint P1 = ptCollection.getObject("P1");
	// both points should have a fixed height value -> should not change during computation
	// P0 is defined using the new point sigma syntax, P1 is defined via *VXY
	double P0Height = P0.getEstimatedHeightInRoot();
	double P1Height = P1.getEstimatedHeightInRoot();
	ensure_equals("Point with SZ 0 in non-OLOC has to imply fixed height.", P0Height, P1Height);
	ensure_equals("Point definition *VXY should lead to same computation result as *POIN with SZ 0, also in non-OLOC case.",
		(P0.getEstimatedValue() - P1.getEstimatedValue()).toRealVector().norm(), 0);
}
template<>
template<>
void object::test<4>()
{
	set_test_name("Testing computation with fixed height.");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/test.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(pointWithSigma::computationAnglesDefinition);

	bool succesReading = reader.read(infiler);
	ensure_equals("Reading file successful", succesReading, true);
	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	const LGCAdjustablePointCollection &ptCollection = projTest->getPoints();
	// get teh adjusted points and compare their estimated covariances
	LGCAdjustablePoint pointWithSigmaInRoot = ptCollection.getObject("pointWithSigmaInRoot");
	LGCAdjustablePoint pointWithSigmaInRootPerturbed = ptCollection.getObject("pointWithSigmaInRootPerturbed");
	LGCAdjustablePoint pointWithSigmaInRST = ptCollection.getObject("pointWithSigmaInRoot");
	LGCAdjustablePoint pointWithSigmaInRSTPerturbed = ptCollection.getObject("pointWithSigmaInRootPerturbed");
	ensure_equals("Covariance matrices in root have to be identical", pointWithSigmaInRoot.getCovarianceMatrix().isApprox(pointWithSigmaInRST.getCovarianceMatrixInRoot()), true);
	ensure_equals("Covariance matrices in root have to be identical",
		pointWithSigmaInRootPerturbed.getCovarianceMatrix().isApprox(pointWithSigmaInRSTPerturbed.getCovarianceMatrixInRoot()), true);
}
}; // namespace tut
