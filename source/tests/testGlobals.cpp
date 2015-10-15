#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


//Input test files

#include "contributionsGeneratorTest.h"

#include <TLGCData.h>
#include <TLOR2LOR.h>

#include <TReader.h>
#include <AdjObjectsReader.h>

#include <TAdjustableObjectCollection.h>

#include <TXYH2CCS.h>
#include <TLGCCalculation.h>

#include "TLSInputMatricesFiller.h"
#include "TLSInputMatrices.h"
#include "TLSResultsMatricesExtractor.h"
#include "TAffineTransformFactory.h"

#include "testTLOR2LOR.h"


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
		ensure_equals("Number of unknowns should match", mmm.getNumUnkn(), 4);
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
		ensure_equals("Number of unknowns should match", p.getNumUnkn(), 2);
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
		ensure_equals("2 unknowns introduced", pH.getNumUnkn(), 2);
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
		ensure_equals("Number of unknowns should match", adjScalar.getNumUnkn(), 1);
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
		std::shared_ptr<TLGCData> proj5(new TLGCData);
		using namespace LGC;
		set_test_name("Testing Measurements");
		//Measurements are tested more properly in testReaders -- Testing measurement input -- 
		TPositionVector posit(1.0,1.0,1.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TDataTreeIterator iter;
		const TAdjustablePoint pos(posit, false, false, false, "point", TLGCRefFrame::ERefs::kOLOC, iter);
	
#ifdef __linux__
		const TInstrumentData::TPOLAR instrument{};
#else
		const TInstrumentData::TPOLAR instrument;
#endif
		TTSTN tstn( pos, instrument);
		TAdjustablePoint ssa = *tstn.instrumentPos;
		ssa.setXYEstimatedCovariance(3.0);

		TReader r(proj5);
		stringstream infile(LOR2LORInputFiles::plateFileOrig);
		r.read(infile);

		ensure_equals("One instrument defined", proj5->getInstruments().fPOLAR.size(),1);
		ensure_equals("Provisional value of the pair should match", proj5->getScalars().getObject("TS1T1").getProvisionalValue().getValue(),2.0);
		ensure_equals("Dist. correction is 2.0",(*proj5->getInstruments().fPOLAR["TS1"].targets["T1"].distCorrectionAdjustable).getProvisionalValue().getValue(),2.0);	
		ensure_equals("2 adjustable scalars expected",proj5->getScalars().numObjects(), 9);
	}

}