#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


//Input test files
#include "input.h"
#include "contributionsGeneratorTest.h"
#include "inpMatrFillerTest.h"

#include <TLGCData.h>
#include <ContributionsGenerators/TLOR2LOR.h>

#include <readers/TReader.h>
#include <readers/AdjObjectsReader.h>

#include <AdjustableObjects/TAdjustableObjectCollection.h>

#include <refframetransformations\TXYH2CCS.h>
#include <TLGCCalculation.h>

#include "LSCalculation\TLSInputMatricesFiller.h"
#include "lsalgo/TLSInputMatrices.h"
#include "LSCalculation\TLSResultsMatricesExtractor.h"
#include "TAffineTransformFactory.h"
#include "SubFrameTest.h"


#define EXPECT_FAIL(x) \
try {x;} catch (std::exception& e) {cout << endl << "expected failure: " << e.what();}

#pragma warning (disable:4224)

using namespace std;

namespace tut
{
    struct test_Globals{};
    typedef test_group<test_Globals> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test of utility classes");
}

namespace tut
{
	// Test LGC::TAngle
	template<>
	template<>
	void object::test<1>()
	{
		using namespace LGC;
		set_test_name("Testing Angle class");
		const LGC::TAngle::EUnits kDeg(LGC::TAngle::kDegrees);
		const TReal prec(1e-12);
		
		LGC::TAngle a0(kDeg, 0);
		LGC::TAngle a30(kDeg, 30);
		LGC::TAngle a60(kDeg, 60);
		LGC::TAngle a90(kDeg, 90);
		LGC::TAngle a120(kDeg, 120);
		
		ensure("Equal angles must be equal compared", a30 == a30);
		ensure("Unequal angles must be unequal when compared", a0 != a60);
		
		ensure("30 must be smaller than 90", a30 < a90);
		ensure("60 must be greater than 30", a60 > a30);
		
		LGC::TAngle c30p60e90(a30+a60);
		//ensure_distance("Raw value of 30+60 must be -PI/2", c30p60e90.raw(), r90, prec);
		ensure_distance("90 degrees calculated: sin must be 1.0", c30p60e90.sin(), 1.0, prec);
		ensure_distance("90 degrees calculated: cos must be 0.0", c30p60e90.cos(), 0.0, prec);
		ensure_distance("90 degrees calculated: rad value must be PI/2", c30p60e90.rad(), PI_2, prec);
		ensure_distance("90 degrees calculated: deg value must be 90",  c30p60e90.deg(),  90.0, prec);
		ensure_distance("90 degrees calculated: gon value must be 100", c30p60e90.gon(), 100.0, prec);
		ensure_distance("90 degrees calculated: cc  value must be 100e4", c30p60e90.cc(), 100.0e4, 10.0);
		
		LGC::TAngle c30m60e90(a90-a60);
		//ensure_distance("Raw value of 90-60 must be -5/6*PI", c30m60e90.raw(), r30, 1e-4);
	}

		// Test adjustable objects
	template<>
	template<>
	void object::test<2>()
	{
		using namespace LGC;
		set_test_name("Testing Adjustable objects");
		TPositionVector position1(1.0,2.0,3.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TFreeVector ll(1.0,2.0,3.0,TCoordSysFactory::ECoordSys::k3DCartesian);

		
		//////////////////////// Testing TAdjustableLine ////////////////////////
		std::bitset<3> point;
		point.set(0);
		std::bitset<3> line;
		line.set(0);
		TAdjustableLine mmm(position1, ll, point, line, "line1");
		ensure_equals("Number of unknowns should match", mmm.getNumUkn(), 4);
		ensure_equals("Line is not fixed", mmm.isFixed(), false);
		mmm.setFirstUidx(3);
		ensure_equals("First uidx", mmm.getFirstUidx(), 3);
		ensure_equals("First uidx", mmm.getLastUidx(), 6);

		mmm.setCorrection(5,2.0);
		ensure_equals("Correction", mmm.getLineVectorCorrection().getY().getValue(), 2.0);
		ensure_equals("Estimated value", mmm.getLineVectorEstimatedValue().getY().getValue(), 4.0);
		
		std::bitset<3> point2;
		std::bitset<3> line2;
		TAdjustableLine mmm2(position1, ll, point2, line2, "line2");
		ensure_equals("Line is not fixed", mmm2.isFixed(), false);

		TAdjustableLine unita = mmm2.createUninitialized("cop");
		ensure_equals("Point is not initialized", unita.isInitialized(), false);

		//////////////////////// Testing TAdjustablePoint ////////////////////////
		TDataTreeIterator iter;
		TAdjustablePoint p(position1,true,false,false,"point1",TLGCRefFrame::ERefs::kOLOC, iter);
		ensure_equals("Number of unknowns should match", p.getNumUkn(), 2);
		ensure_equals("Point is initialized", p.isInitialized(), true);
		p.setFirstUidx(3);
		ensure_equals("Last uidx is on 4th position", p.getLastUidx(), 4);
		p.setCorrection(3,1.0);
		ensure_equals("Last uidx is on 4th position", p.getCorrection().getY().getValue(), 1.0);
		ensure_equals("Last uidx is on 4th position", p.getEstimatedValue().getY().getValue(), 3.0);


		TPositionVector position2(1.0,2.0,3.0,TCoordSysFactory::ECoordSys::k2DPlusH);
		TAdjustablePoint pH(position2, false, false, true, "pointH1",TLGCRefFrame::ERefs::kRS2K, iter);
		pH.setFirstUidx(5);
		ensure_equals("Reference system of provisional value should match", pH.getProvisionalValue().getCoordSys(), TCoordSysFactory::ECoordSys::k2DPlusH);
		ensure_equals("Provisional value should match", pH.getProvisionalValue().getX().getValue(), 1.0);
		ensure_equals("Provisional value should match", pH.getProvisionalValue().getY().getValue(), 2.0);
		ensure_equals("Provisional value should match", pH.getProvisionalValue().getH().getValue(), 3.0);

		ensure_equals("Reference system of estimated value should match", pH.getEstimatedValue().getCoordSys(), TCoordSysFactory::ECoordSys::k3DCartesian);
		ensure_equals("2 unknowns introduced", pH.getNumUkn(), 2);
		ensure_equals("Contains a variable", pH.hasVariable(), true);
		ensure_equals("Point is not fixed", pH.isFixed(), false);
		ensure_equals("First uidx should match", pH.getFirstUidx(), 5);
		ensure_equals("Last uidx should match", pH.getLastUidx(), 6);
		ensure_equals("Last uidx should match", pH.getName(), "pointH1");
		ensure_equals("Estimated values should match",pH.getEstimatedValue().getX().getValue(), 1.0);
		ensure_equals("Estimated values should match",pH.getEstimatedValue().getY().getValue(), 2.0);
		ensure_equals("Estimated values should match",pH.getEstimatedValue().getZ().getValue(), 2002.34689402122, 1e-8);

		ensure_equals("Coordinates fixed state should match",pH.isCoordinateFixed(0), false);
		ensure_equals("Coordinates fixed state should match",pH.isCoordinateFixed(1), false);
		ensure_equals("Coordinates fixed state should match",pH.isCoordinateFixed(2), true);

		ensure_equals("Coordinates fixed state should match",pH.getCoordinateUnknIndex(0), 5);
		ensure_equals("Coordinates fixed state should match",pH.getCoordinateUnknIndex(1), 6);
		EXPECT_FAIL(pH.getCoordinateUnknIndex(2));


		EXPECT_FAIL(pH.getStandDev(0));
		ensure_equals("Reference frame should match",pH.getReferenceFrame(), TLGCRefFrame::ERefs::kRS2K);
		ensure_equals("Point is initialized",pH.isInitialized(), true);
		ensure_equals("No standard deviations",pH.hasStandDeviations(), false);

		pH.setCorrection(5, 150);
		pH.setCorrection(6, 2000);

		ensure_equals("Estimated values should match",pH.getEstimatedValue().getX().getValue(), 151);
		ensure_equals("Estimated values should match",pH.getEstimatedValue().getY().getValue(), 2002);
		ensure_equals("Estimated value of Z(H is fixed) have to change, because it depends on X a Y which has changed",pH.getEstimatedValue().getZ().getValue(), 2002.73661251304, 1e-8);

		pH.setEstimatedPrecision(5, 0.1);
		pH.setEstimatedPrecision(6, 0.2);
		ensure_equals("Estimated precision should match", pH.getXEstPrecision().getValue(), 0.1);
		ensure_equals("Estimated precision should match", pH.getYEstPrecision().getValue(), 0.2);
		ensure_equals("Estimated precision should match", pH.getZEstPrecision().getValue(), 0.0);

		pH.setXYEstimatedCovariance(0.01);
		EXPECT_FAIL(pH.setYZEstimatedCovariance(0.02));
		EXPECT_FAIL(pH.setXZEstimatedCovariance(0.02));
		ensure_equals("Covariance should match", pH.getXYCovar().getValue(), 0.01);

		ensure_equals("Reference frame should match", pH.getReferenceFrame(), TLGCRefFrame::ERefs::kRS2K);
		ensure_equals("Point is initialized", pH.isInitialized(), true);
		ensure_equals("No standard deviations", pH.hasStandDeviations(), false);

		EXPECT_FAIL(pH.setCorrection(10,10));
		pH.setStandardDeviations(0.1,0.2,0.3);
		ensure_equals("Standard deviations should match", pH.getStandDev(0), 0.1);
		ensure_equals("Standard deviations should match", pH.getStandDev(1), 0.2);
		ensure_equals("Standard deviations should match", pH.getStandDev(2), 0.3);
		ensure_equals("No standard deviations",pH.hasStandDeviations(), true);

		ensure_equals("Reference system should match", pH.getProvisionalValue().getCoordSys(), position2.getCoordSys());

		TAdjustablePoint unit = p.createUninitialized("point2");
		ensure_equals("Point is not initialized", unit.isInitialized(), false);

		//////////////////////// Testing TAdjustableScalar ////////////////////////
		TScalar scalar(TReal(2.0));
		TAdjustableScalar adjScalar(scalar, false, "adjscl");
		adjScalar.setFirstUidx(2);

		ensure_equals("Provisional value should match", adjScalar.getProvisionalValue().getValue(), 2.0);
		ensure_equals("Correction should match", adjScalar.getCorrection().getValue(), 0.0);
		ensure_equals("Estimated value should match", adjScalar.getEstimatedValue().getValue(), 2.0);
		ensure_equals("Estimated precision should match", adjScalar.getEstimatedPrecision().getValue(), 0.0);
		ensure_equals("Number of unknowns should match", adjScalar.getNumUkn(), 1);
		ensure_equals("Scalar is variable", adjScalar.isFixed(), false);
		ensure_equals("Scalar first index is 2", adjScalar.getFirstUidx(), 2);
		ensure_equals("Scalar last index is 2", adjScalar.getLastUidx(), 2);
		ensure_equals("Name should match", adjScalar.getName(), "adjscl");
		ensure_equals("Name should match", adjScalar.isInitialized(), true);

		adjScalar.setCorrection(2,0.5);
		adjScalar.setEstimatedPrecision(2,0.01);
		ensure_equals("Correction should match", adjScalar.getCorrection().getValue(), 0.5);
		ensure_equals("New estimated value should match", adjScalar.getEstimatedValue().getValue(), 2.5);
		EXPECT_FAIL(adjScalar.setCorrection(3,0.5));
		EXPECT_FAIL(adjScalar.setEstimatedPrecision(3,0.01));

		ensure_equals("Correction should match", adjScalar.getEstimatedPrecision().getValue(), 0.01);


		//Testing TAdjustablePlane


		//Testing TAdjustableAngle
	}

	// Test measurments
	template<>
	template<>
	void object::test<3>()
	{
		TLGCData proj5;
		using namespace LGC;
		set_test_name("Testing Measurements");
		//Measurements are tested more properly in testReaders -- Testing measurement input -- 
		TPositionVector posit(1.0,1.0,1.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TDataTreeIterator iter;
		const TAdjustablePoint pos(posit, false, false, false, "point", TLGCRefFrame::ERefs::kOLOC, iter);
		const TInstrumentData::TPOLAR instrument;

		TTSTN tstn( pos, instrument);
		TAdjustablePoint ssa = *tstn.instrumentPos;
		ssa.setXYEstimatedCovariance(3.0);

		TReader r(&proj5);
		stringstream infile(LGCTestInput::platesfile2);
		r.read(infile);

		ensure_equals("One instrument defined", proj5.getInstruments().fPOLAR.size(),1);
		ensure_equals("Provisional value of the pair should match", proj5.getScalars().getObject("TS1T1").getProvisionalValue().getValue(),2.0);
		ensure_equals("Dist. correction is 2.0",(*proj5.getInstruments().fPOLAR["TS1"].targets["T1"].distCorrectionAdjustable).getProvisionalValue().getValue(),2.0);	
		ensure_equals("2 adjustable scalars expected",proj5.getScalars().numObjects(), 9);
	}

	// Test matrix filler
	template<>
	template<>
	void object::test<4>()
	{

		TLGCData inpMatFillTest;
		set_test_name("Testing Input matrices filler.");
		TReader rr(&inpMatFillTest);
		stringstream infiler(LGCTestInputMF::MFfile);

		
		inpMatFillTest.getFileLogger().setOutputfileLocation("C:/Temp/output.txt");
		rr.read(infiler);

		TDataAnalyzer analyzer(inpMatFillTest);
		analyzer.dataConsistent(); //Assign unknown indices
		TCalcType ct = analyzer.getCalculationType();
		TLSInputMatricesFiller imf(&inpMatFillTest.getTree(), ct);
		TLSInputMatrices inputMtr;

		imf.fillMatrices(&inpMatFillTest, &inputMtr);
		inputMtr.saveMatricesToFile(1);

	}
/*
	TLGCData project2;
	TLGCData project2b;
	template<>
	template<>
	void object::test<7>()
	{
		set_test_name("Testing Contribution Generator.");
////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////Testing contributionsGeneratorTest.h file (OLOC option)
////////////////////////////////////////////////////////////////////////////////////////////////////////
		TReader r(&project2);
		stringstream infile(LGCTestInputCG::CGfile);
		r.read(infile);
		//Check consistency and identify calculation type
		TDataAnalyzer analyzer(project2);
		ensure_equals("Data should be consistent", analyzer.dataConsistent(), true);
		TCalcType cType = analyzer.getCalculationType();

		TContributionsGenerator cg(&project2.getTree(), cType.fReferential);
//DLEV measurement in ROOT
		TLEVEL level1 = *project2.getTree().begin()->get()->measurements.fLEVEL.begin();
		TDLEV dlev1 = project2.getTree().begin()->get()->measurements.fLEVEL.begin()->measDLEV[0];
		DLEVContrib dlevcontrib1 = cg.getDLEVContrib(level1, dlev1);
		ensure_equals("Calculation meas of DLEV should match", dlevcontrib1.fCalcMeas, -10 - sqrt(200) * tanq(0.1570796326795), 1e-8);
		ensure_equals("Collimination angle contribution should match", dlevcontrib1.fCollAngleContrib, - sqrt(200) * (1 + pow2(tanq(0.1570796326795))), 1e-8);
		ensure_equals("DLEV target's X coordinate contrib should match", dlevcontrib1.fStaffTargetContrib.getX().getValue(), 0, 1e-8);
		ensure_equals("DLEV target's Y coordinate contrib should match", dlevcontrib1.fStaffTargetContrib.getY().getValue(), 0, 1e-8);
		ensure_equals("DLEV target's Z coordinate contrib should match", dlevcontrib1.fStaffTargetContrib.getZ().getValue(), - 1, 1e-8);

		ensure_equals("Ref. point distance contribution should match", dlevcontrib1.fRefPtDistContrib, -1, 1e-8);
//FIRST measurement ROOT
		TTSTN station = *project2.getTree().begin()->get()->measurements.fTSTN.begin();
		TLINE distCalc = station.roms.begin()->measDIST[0];
		//Testing contributions values
		DistMeasContrib dist1Contrib = cg.getSpatialDistanceContrib(station, distCalc);
		ensure_equals("Meas calculation is supposed to be 10", dist1Contrib.fCalcMeas, 10, 1e-8);
		ensure_equals("Station's X coord contribution should be -1", dist1Contrib.fStCoordContrib.getX().getValue(), -1, 1e-8);
		ensure_equals("Station's Y coord contribution should be 0", dist1Contrib.fStCoordContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Station's Z coord contribution should be 0", dist1Contrib.fStCoordContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Target's X coord contribution should be 1", dist1Contrib.fTgCoordContrib.getX().getValue(), 1.0, 1e-8);
		ensure_equals("Target's Y coord contribution should be 0", dist1Contrib.fTgCoordContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Target's Z coord contribution should be 0", dist1Contrib.fTgCoordContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("No target's transformation contributions", dist1Contrib.fTgTransformContrib.size(), 0);
		//ensure_equals("Observation's variance should match", dist1Contrib.fObsVariance), );  //VARIANCE

//SECOND measurement ROOT
		TLINE distCalc2 = station.roms.begin()->measDIST[1];
		//Testing contributions values
		DistMeasContrib dist2Contrib = cg.getSpatialDistanceContrib(station, distCalc2);

		ensure_equals("Meas calculation is supposed to be 10", dist2Contrib.fCalcMeas, 10,1e-8);

//		ensure_equals("Misclosure vector is supposed to be -0.1", dist2Contrib.fMisclVector, -0.1, 1e-15);

//THIRD measurement ROOT
		TLINE distCalc2b = station.roms.begin()->measDIST[2];
		//Testing contributions values
		DistMeasContrib dist2bContrib = cg.getSpatialDistanceContrib(station, distCalc2b);
		ensure_equals("Meas calculation is supposed to be 10*sqrt(3) (cube's diagonal)", dist2bContrib.fCalcMeas, 10*sqrt(3), 1e-8);
		ensure_equals("Sum of squared station's contributions should be one", pow2(dist2bContrib.fStCoordContrib.getX().getValue())+ pow2(dist2bContrib.fStCoordContrib.getY().getValue())+pow2(dist2bContrib.fStCoordContrib.getZ().getValue()), 1.0, 1e-8);
		ensure_equals("Contributions for x,y,z station's coordinate should be equal", dist2bContrib.fStCoordContrib.getX().getValue(), dist2bContrib.fStCoordContrib.getY().getValue(), 1e-8);
		ensure_equals("Contributions for x,y,z station's coordinate should be equal",dist2bContrib.fStCoordContrib.getY().getValue(), dist2bContrib.fStCoordContrib.getZ().getValue(), 1e-8);

		ensure_equals("Sum of squared station's contributions should be one", pow2(dist2bContrib.fTgCoordContrib.getX().getValue())+ pow2(dist2bContrib.fTgCoordContrib.getY().getValue())+pow2(dist2bContrib.fTgCoordContrib.getZ().getValue()), 1.0, 1e-8);
		ensure_equals("Target contribution should be opposit to station one", dist2bContrib.fStCoordContrib.getX().getValue(), - dist2bContrib.fTgCoordContrib.getX().getValue(), 1e-8);
		ensure_equals("Target contribution should be opposit to station one",dist2bContrib.fStCoordContrib.getY().getValue(), - dist2bContrib.fTgCoordContrib.getY().getValue(), 1e-8);

		ensure_equals("Target contribution should be opposit to station one",dist2bContrib.fHIContrib, - 0.57735026918962573, 1e-8);
		ensure_equals("Target contribution should be opposit to station one",dist2bContrib.fDistCorrection, - 1.0, 1e-8);

//		ensure_equals("Misclosure vector is supposed to be 10*sqrt(3) - 17", dist2bContrib.fMisclVector, 10*sqrt(3)-17, 1e-8);

//FOURTH
		TANGL distCalcAngl1 = station.roms.begin()->measANGL[0];
		TTSTN::TROM  rom = *station.roms.begin();
		AnglMeasContrib ANGLContrib = cg.getHorAnglContrib(station, rom, distCalcAngl1);
		ensure_equals("Meas calculation of ANGL contribt should match", ANGLContrib.fCalcMeas.rad(), atanq(1) + 0.1570796326795, 1e-8);
		ensure_equals("X station's contribution should match", ANGLContrib.fStCoordContrib.getX().getValue(), -10.0/(200), 1e-8);
		ensure_equals("Y station's contribution should match", ANGLContrib.fStCoordContrib.getY().getValue(), 10.0/(200), 1e-8);
		ensure_equals("Z station's contribution should match", ANGLContrib.fStCoordContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("X station's contribution should match", ANGLContrib.fTgCoordContrib.getX().getValue(), 10.0/(200), 1e-8);
		ensure_equals("Y station's contribution should match", ANGLContrib.fTgCoordContrib.getY().getValue(), -10.0/(200), 1e-8);
		ensure_equals("Z station's contribution should match", ANGLContrib.fTgCoordContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("V0 contribution should match", ANGLContrib.fV0Contrib, 1.0, 1e-8);
		ensure_equals("Variance should match", ANGLContrib.fObsVariance, pow2q(0.1*LGC::MM2M) + (1.0/200.0) * (pow2q(0.1*LGC::MM2M)+pow2q(0.2*LGC::MM2M)), 1e-8);
//FIFTH
		TZEND distCalcZend1 = station.roms.begin()->measZEND[0];
		AnglMeasContrib ZENDContrib = cg.getZenDistContrib(station, distCalcZend1);
		ensure_equals("Meas calculation of ANGL contribt should match", ZENDContrib.fCalcMeas.rad(), acosq(10/sqrtq(300)), 1e-8);
		ensure_equals("X station's contribution should match", ZENDContrib.fStCoordContrib.getX().getValue(), -100.0/(powq(sqrtq(300.0),3)*sinq(acosq(10/sqrtq(300.0)))), 1e-8);
		ensure_equals("Y station's contribution should match", ZENDContrib.fStCoordContrib.getY().getValue(), -100.0/(powq(sqrtq(300.0),3)*sinq(acosq(10/sqrtq(300.0)))), 1e-8);
		ensure_equals("Z station's contribution should match", ZENDContrib.fStCoordContrib.getZ().getValue(), (1/(sqrtq(300.0)*sinq(acosq(10/sqrtq(300))))) - (100.0/(powq(sqrtq(300.0),3)*sinq(acosq(10/sqrtq(300))))), 1e-8);

		ensure_equals("X station's contribution should match", ZENDContrib.fTgCoordContrib.getX().getValue(), 100.0/(powq(sqrtq(300.0),3)*sinq(acosq(10/sqrtq(300.0)))), 1e-8);
		ensure_equals("Y station's contribution should match", ZENDContrib.fTgCoordContrib.getY().getValue(), 100.0/(powq(sqrtq(300.0),3)*sinq(acosq(10/sqrtq(300.0)))), 1e-8);
		ensure_equals("Z station's contribution should match", ZENDContrib.fTgCoordContrib.getZ().getValue(), -(1/(sqrtq(300.0)*sinq(acosq(10/sqrtq(300))))) + (100.0/(powq(sqrtq(300.0),3)*sinq(acosq(10/sqrtq(300))))), 1e-8);

		//ensure_equals("V0 contribution should match", ZENDContrib.fHIContrib, -1.0, 1e-8);
		//ensure_equals("Variance should match", ZENDContrib.fObsVariance, pow2q(0.1) + (1.0/200.0) * (pow2q(0.1)+pow2q(0.2)), 1e-8);
//SIXTH
		TLINE distCalcDHOR1 = station.roms.begin()->measDHOR[0];
		HorDistContrib DHORContrib = cg.getHorDistContrib(station, distCalcDHOR1);
//SEVENTH
		TPLR3D distCalcPLR3DTSTN = station.roms.begin()->measPLR3D[0];
		PLR3DContrib contribTSTNPLR3D = cg.getPolar3DContrib(station, rom, distCalcPLR3DTSTN);


////////////////////////////////////////////////////////////////////
// THIS WHOLE IS NOT SURE
/////////////////////////////////////////////

//FIRST measurement of second TSTN in ROOT
		TTSTN stationTgLOR = project2.getTree().begin()->get()->measurements.fTSTN[1];
		TLINE distCalcTgLOR = stationTgLOR.roms.begin()->measDIST[0];
		//Testing contributions values
		DistMeasContrib distCalcTgLORContrib = cg.getSpatialDistanceContrib(stationTgLOR, distCalcTgLOR);
		ensure_equals("Meas calculation is supposed to be 20", distCalcTgLORContrib.fCalcMeas, 20.0, 1e-8); 
		ensure_equals("Station's X coord contribution should be 1", distCalcTgLORContrib.fStCoordContrib.getX().getValue(), 1.0, 1e-8);
		ensure_equals("Station's Y coord contribution should be 0", distCalcTgLORContrib.fStCoordContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Station's Z coord contribution should be 0", distCalcTgLORContrib.fStCoordContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Target's X coord contribution should be 1", distCalcTgLORContrib.fTgCoordContrib.getX().getValue(),0.0,1e-8);
		ensure_equals("Target's Y coord contribution should be 0", distCalcTgLORContrib.fTgCoordContrib.getY().getValue(),1.0,1e-8);
		ensure_equals("Target's Z coord contribution should be 0", distCalcTgLORContrib.fTgCoordContrib.getZ().getValue(),0.0,1e-8);

		ensure_equals("No station's transformation contributions", distCalcTgLORContrib.fTgTransformContrib.size(),1);
//!!!!!!!!!!!!!!!!!!!!!Need to be adapted!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if 0
		//Contributions for transformations parameters
		ensure_equals("Contribution  to a rotation around x axis should match", distCalcTgLORContrib.fTgTransformContrib[0].second.fRotationContrib.getX().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a rotation around y axis should match", distCalcTgLORContrib.fTgTransformContrib[0].second.fRotationContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a rotation around z axis should match", distCalcTgLORContrib.fTgTransformContrib[0].second.fRotationContrib.getZ().getValue(), -10.0, 1e-8); //NOT SURE

		ensure_equals("Contribution  to a translation around x axis should match", distCalcTgLORContrib.fTgTransformContrib[0].second.fTranslContrib.getX().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a translation around y axis should match", distCalcTgLORContrib.fTgTransformContrib[0].second.fTranslContrib.getY().getValue(), -1.0, 1e-8);
		ensure_equals("Contribution  to a translation around z axis should match", distCalcTgLORContrib.fTgTransformContrib[0].second.fTranslContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Contribution  to a scale", distCalcTgLORContrib.fTgTransformContrib[0].second.fScaleContrib, 0.0, 1e-8);
#endif
////////////////////////////////////////////////////////////////////
// HERE IT ENDS
/////////////////////////////////////////////


//FIRST measurement of (PLR3D done by CAM) in the ROOT
		TCAM cam1 = *project2.getTree().begin()->get()->measurements.fCAM.begin();
		TPLR3D plrCAM1 = cam1.measPLR3D[0];
		PLR3DContrib a = cg.getPolar3DContrib(cam1,plrCAM1);
		TDIR3D dirCAM1 = cam1.measDIR3D[0];
		DIR3DContrib b = cg.getDIR3DContrib(cam1,dirCAM1);  // Transformation's rotation contribution is non zero



		ensure_equals("Meas calculation is supposed to be 10*sqrt(3) (cube's diagonal)", dist2bContrib.fCalcMeas.getMetresValue(), 10*sqrt(3), 1e-8);
		ensure_equals("Sum of squared station's contributions should be one", pow2(dist2bContrib.fStCoordContrib.getX().getValue())+ pow2(dist2bContrib.fStCoordContrib.getY().getValue())+pow2(dist2bContrib.fStCoordContrib.getZ().getValue()), 1.0, 1e-8);
		ensure_equals("Contributions for x,y,z station's coordinate should be equal", dist2bContrib.fStCoordContrib.getX().getValue(), dist2bContrib.fStCoordContrib.getY().getValue(), 1e-8);
		ensure_equals("Contributions for x,y,z station's coordinate should be equal",dist2bContrib.fStCoordContrib.getY().getValue(), dist2bContrib.fStCoordContrib.getZ().getValue(), 1e-8);

		ensure_equals("Sum of squared station's contributions should be one", pow2(dist2bContrib.fTgCoordContrib.getX().getValue())+ pow2(dist2bContrib.fTgCoordContrib.getY().getValue())+pow2(dist2bContrib.fTgCoordContrib.getZ().getValue()), 1.0, 1e-8);
		ensure_equals("Target contribution should be opposit to station one", dist2bContrib.fStCoordContrib.getX().getValue(), - dist2bContrib.fTgCoordContrib.getX().getValue(), 1e-8);
		ensure_equals("Target contribution should be opposit to station one",dist2bContrib.fStCoordContrib.getY().getValue(), - dist2bContrib.fTgCoordContrib.getY().getValue(), 1e-8);

		ensure_equals("Target contribution should be opposit to station one",dist2bContrib.fHIContrib, - 0.57735026918962573, 1e-8);
		ensure_equals("Target contribution should be opposit to station one",dist2bContrib.fDistCorrection, - 1.0, 1e-8);

		ensure_equals("Misclosure vector is supposed to be 10*sqrt(3) - 17", dist2bContrib.fMisclVector, 10*sqrt(3)-17, 1e-8);


#if 0
//FIRST measurement in the LOR1, 90 deg z-axis rotation
		TDataTreeIterator it = project2.getTree().begin();
		it++;
		TTSTN station2 = *it->get()->measurements.fTSTN.begin();
		TLINE distCalc3 = station2.roms.begin()->measDIST[0];
		//Testing contributions values
		DistMeasContrib dist3Contrib = cg.getSpatialDistanceContrib(station2, distCalc3);


		ensure_equals("Meas calculation is supposed to be 10", dist3Contrib.fCalcMeas.getMetresValue(), 20, 1e-8);
		ensure_equals("Station's X coord contribution should be -1", dist3Contrib.fStCoordContrib.getX().getValue(), 0.0, 1e-8);
		ensure_equals("Station's Y coord contribution should be 0", dist3Contrib.fStCoordContrib.getY().getValue(), 1.0, 1e-8);
		ensure_equals("Station's Z coord contribution should be 0", dist3Contrib.fStCoordContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Target's X coord contribution should be 1", dist3Contrib.fTgCoordContrib.getX().getValue(),1.0,1e-8);
		ensure_equals("Target's Y coord contribution should be 0", dist3Contrib.fTgCoordContrib.getY().getValue(),0.0,1e-8);
		ensure_equals("Target's Z coord contribution should be 0", dist3Contrib.fTgCoordContrib.getZ().getValue(),0.0,1e-8);

		ensure_equals("One station's transformation contributions", dist3Contrib.fStTransformContrib.size(),1);
		ensure_equals("No station's transformation contributions", dist3Contrib.fTgTransformContrib.size(),0);

		ensure_equals("Misclosure vector is supposed to be -0.1", dist3Contrib.fMisclVector, -0.2, 1e-15);

		//Contributions for transformations parameters
		ensure_equals("Contribution  to a rotation around x axis should match", dist3Contrib.fStTransformContrib[0].second.fRotationContrib.getX().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a rotation around y axis should match", dist3Contrib.fStTransformContrib[0].second.fRotationContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a rotation around z axis should match", dist3Contrib.fStTransformContrib[0].second.fRotationContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Contribution  to a translation around x axis should match", dist3Contrib.fStTransformContrib[0].second.fTranslContrib.getX().getValue(), -1.0, 1e-8);
		ensure_equals("Contribution  to a translation around y axis should match", dist3Contrib.fStTransformContrib[0].second.fTranslContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a translation around z axis should match", dist3Contrib.fStTransformContrib[0].second.fTranslContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Contribution  to a scale", dist3Contrib.fStTransformContrib[0].second.fScaleContrib, 0.0, 1e-8);
//SECOND measurement in the LOR1, 90 deg z-axis rotation
		TLINE distCalc3A = station2.roms.begin()->measDIST[0];
		//Testing contributions values
		DistMeasContrib dist3ContribA = cg.getSpatialDistanceContrib(station2, distCalc3A);
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////Testing contributionsGeneratorTestMLA.h file (RS2K option, i.e. transformation to MLA)
////////////////////////////////////////////////////////////////////////////////////////////////////////
		TReader r2(&project2b);
		stringstream infile2(LGCTestInputCGMLA::CGfileMLA);
		r2.read(infile2);
		//Check consistency and identify calculation type
		TDataAnalyzer analyzer2(project2b);
		ensure_equals("Data should be consistent", analyzer2.dataConsistent(), true);
		TCalcType cType2 = analyzer2.getCalculationType();

		TContributionsGenerator cg2(&project2b.tree, cType2.fReferential);
//FIRST measurement ROOT
		TTSTN stationB = *project2b.getTree().begin()->get()->measurements.fTSTN.begin();
		TLINE distCalcB = stationB.roms.begin()->measDIST[0];
		//Testing contributions values
		DistMeasContrib dist1ContribB = cg2.getSpatialDistanceContrib(stationB, distCalcB);

		ensure_equals("Meas calculation is supposed to be 100", dist1ContribB.fCalcMeas, 100, 1e-10);
		ensure_equals("Station's X coord contribution should be -1", dist1ContribB.fStCoordContrib.getX().getValue(), -1, 1e-10);
		ensure_equals("Station's Y coord contribution should be 0", dist1ContribB.fStCoordContrib.getY().getValue(), 0.0, 1e-10);
		ensure_equals("Station's Z coord contribution should be 0", dist1ContribB.fStCoordContrib.getZ().getValue(), 0.0, 1e-10);

		ensure_equals("Target's X coord contribution should be 1", dist1ContribB.fTgCoordContrib.getX().getValue(), 1.0,1e-10);
		ensure_equals("Target's Y coord contribution should be 0", dist1ContribB.fTgCoordContrib.getY().getValue(), 0.0,1e-10);
		ensure_equals("Target's Z coord contribution should be 0", dist1ContribB.fTgCoordContrib.getZ().getValue(), 0.0,1e-10);

		ensure_equals("No target's transformation contributions", dist1ContribB.fTgTransformContrib.size(), 0);

		//ensure_equals("Misclosure vector is supposed to be 0.0", dist1ContribB.fMisclVector, 0.0, 1e-10);

//Second measurement ROOT
		TLINE distCalc2B = stationB.roms.begin()->measDIST[1];
		//Testing contributions values
		DistMeasContrib dist2ContribB = cg2.getSpatialDistanceContrib(stationB, distCalc2B);
//		ensure_equals("Misclosure vector is supposed to be 90.0", dist2ContribB.fMisclVector, 90.0, 1e-10);

//Third measurement ROOT
		TLINE distCalc2B2 = stationB.roms.begin()->measDIST[2];
		//Testing contributions values
		DistMeasContrib dist2ContribB2 = cg2.getSpatialDistanceContrib(stationB, distCalc2B2);
		//For 1e-10 test does not pass
		ensure_equals("Meas calculation is supposed to be 100*sqrt(3)", dist2ContribB2.fCalcMeas, 100*sqrt(3), 1e-9);


#if 0
//FIRST measurement in the LOR1, 90 deg z-axis rotation
		TDataTreeIterator itB = project2b.tree.begin();
		itB++;
		TTSTN station2b = *itB->get()->measurements.fTSTN.begin();
		TLINE distCalc3b = station2b.roms.begin()->measDIST[0];
		//Testing contributions values
		DistMeasContrib dist3ContribB = cg2.getSpatialDistanceContrib(station2b, distCalc3b);


		ensure_equals("Meas calculation is supposed to be 10", dist3Contrib.fCalcMeas.getMetresValue(), 20, 1e-8);
		ensure_equals("Station's X coord contribution should be -1", dist3Contrib.fStCoordContrib.getX().getValue(), 0.0, 1e-8);
		ensure_equals("Station's Y coord contribution should be 0", dist3Contrib.fStCoordContrib.getY().getValue(), 1.0, 1e-8);
		ensure_equals("Station's Z coord contribution should be 0", dist3Contrib.fStCoordContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Target's X coord contribution should be 1", dist3Contrib.fTgCoordContrib.getX().getValue(),1.0,1e-8);
		ensure_equals("Target's Y coord contribution should be 0", dist3Contrib.fTgCoordContrib.getY().getValue(),0.0,1e-8);
		ensure_equals("Target's Z coord contribution should be 0", dist3Contrib.fTgCoordContrib.getZ().getValue(),0.0,1e-8);

		ensure_equals("One station's transformation contributions", dist3Contrib.fStTransformContrib.size(),1);
		ensure_equals("No station's transformation contributions", dist3Contrib.fTgTransformContrib.size(),0);

		TReal misclosureVector3 = -1.0 * (distCalc3.getScalar() -  dist3Contrib.fCalcMeas.getMetresValue());
		ensure_equals("Misclosure vector is supposed to be -0.1", misclosureVector3, -0.2, 1e-15);

		//Contributions for transformations parameters
		ensure_equals("Contribution  to a rotation around x axis should match", dist3Contrib.fStTransformContrib[0].second.fRotationContrib.getX().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a rotation around y axis should match", dist3Contrib.fStTransformContrib[0].second.fRotationContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a rotation around z axis should match", dist3Contrib.fStTransformContrib[0].second.fRotationContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Contribution  to a translation around x axis should match", dist3Contrib.fStTransformContrib[0].second.fTranslContrib.getX().getValue(), -1.0, 1e-8);
		ensure_equals("Contribution  to a translation around y axis should match", dist3Contrib.fStTransformContrib[0].second.fTranslContrib.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Contribution  to a translation around z axis should match", dist3Contrib.fStTransformContrib[0].second.fTranslContrib.getZ().getValue(), 0.0, 1e-8);

		ensure_equals("Contribution  to a scale", dist3Contrib.fStTransformContrib[0].second.fScaleContrib, 0.0, 1e-8);

#endif
	}

	*/
}