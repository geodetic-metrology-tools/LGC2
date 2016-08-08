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
#include <LGCAdjustableObjectCollection.h>
#include <TXYH2CCS.h>
#include <TLGCCalculation.h>
#include "TLSInputMatricesFiller.h"
#include "TLSInputMatrices.h"
#include "TLSResultsMatricesExtractor.h"
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
		// Test adjustable objects
	template<>
	template<>
	void object::test<1>()
	{
		using namespace LGC;
		set_test_name("Testing Adjustable objects");
		TPositionVector position1(1.0,2.0,3.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TDataTreeIterator iter;
		LGCAdjustablePoint p(position1, true, false, false, "point1", TRefSystemFactory::ERefFrame::kLocalRefFrame, iter);
		TFreeVector ll(1.0,2.0,3.0,TCoordSysFactory::ECoordSys::k3DCartesian);

		
		//////////////////////// Testing LGCAdjustableLine ////////////////////////
		std::bitset<3> line;
		line.set(0);
		LGCAdjustableLine mmm(&p, ll, line, "line1");
		ensure_equals("Number of unknowns should match", mmm.getNumUnkn(), 2);
		ensure_equals("Line is not fixed", mmm.isFixed(), false);
		mmm.setFirstUidx(3);
		ensure_equals("First uidx", mmm.getFirstUidx(), 3);
		ensure_equals("First uidx", mmm.getLastUidx(), 4); //the line vector (3 => x, y,z)

		mmm.setCorrection(3,2.0);
		ensure_equals("Correction", mmm.getLineVectorCorrection().getY().getMetresValue(), 2.0);
		ensure_equals("Estimated value", mmm.getLineVectorEstimatedValue().getY().getMetresValue(), 4.0);
		
		std::bitset<3> line2;
		LGCAdjustableLine mmm2(&p, ll, line2, "line2");
		ensure_equals("Line is not fixed", mmm2.isFixed(), false);

		LGCAdjustableLine unita = mmm2.createUninitialized("cop");
		ensure_equals("Point is not initialized", unita.isInitialized(), false);

		//////////////////////// Testing LGCAdjustablePoint ////////////////////////
		//TDataTreeIterator iter;
		//LGCAdjustablePoint p(position1,true,false,false,"point1",TRefSystemFactory::ERefFrame::kLocalRefFrame, iter);
		ensure_equals("Number of unknowns should match", p.getNumUnkn(), 2);
		ensure_equals("Point is initialized", p.isInitialized(), true);
		p.setFirstUidx(3);
		ensure_equals("Last uidx is on 4th position", p.getLastUidx(), 4);
		p.setCorrection(3,1.0);
		ensure_equals("Last uidx is on 4th position", p.getCorrection(1).getMetresValue(), 1.0);
		ensure_equals("Last uidx is on 4th position", p.getEstimatedValue().getY().getMetresValue(), 3.0);


		TPositionVector position2(1.0,2.0,3.0,TCoordSysFactory::ECoordSys::k2DPlusH);
		LGCAdjustablePoint pH(position2, false, false, true, "pointH1",TRefSystemFactory::ERefFrame::kCernXYHg00Machine, iter);
		pH.setFirstUidx(5);
		ensure_equals("Reference system of provisional value should match", pH.getProvisionalValue().getCoordSys(), TCoordSysFactory::ECoordSys::k2DPlusH);
		ensure_equals("Provisional value should match", pH.getProvisionalValue().getX().getMetresValue(), 1.0);
		ensure_equals("Provisional value should match", pH.getProvisionalValue().getY().getMetresValue(), 2.0);
		ensure_equals("Provisional value should match", pH.getProvisionalValue().getH().getMetresValue(), 3.0);

		ensure_equals("Reference system of estimated value should match", pH.getEstimatedValue().getCoordSys(), TCoordSysFactory::ECoordSys::k3DCartesian);
		ensure_equals("2 unknowns introduced", pH.getNumUnkn(), 2);
		ensure_equals("Contains a variable", pH.hasVariable(), true);
		ensure_equals("Point is not fixed", pH.isFixed(), false);
		ensure_equals("First uidx should match", pH.getFirstUidx(), 5);
		ensure_equals("Last uidx should match", pH.getLastUidx(), 6);
		ensure_equals("Last uidx should match", pH.getName(), "pointH1");
		ensure_equals("Estimated values should match", pH.getEstimatedValue().getX().getMetresValue(), 1.0);
		ensure_equals("Estimated values should match", pH.getEstimatedValue().getY().getMetresValue(), 2.0);
		ensure_equals("Estimated values should match", pH.getEstimatedValue().getZ().getMetresValue(), 2002.34689402122, 1e-8);

		ensure_equals("Coordinates fixed state should match",pH.isCoordinateFixed(0), false);
		ensure_equals("Coordinates fixed state should match",pH.isCoordinateFixed(1), false);
		ensure_equals("Coordinates fixed state should match",pH.isCoordinateFixed(2), true);

		ensure_equals("Coordinates fixed state should match",pH.getCoordinateUnknIndex(0), 5);
		ensure_equals("Coordinates fixed state should match",pH.getCoordinateUnknIndex(1), 6);
		EXPECT_FAIL(pH.getCoordinateUnknIndex(2));


		EXPECT_FAIL(pH.getStandDev(0));
		ensure_equals("Reference frame should match",pH.getReferenceFrame(), TRefSystemFactory::ERefFrame::kCernXYHg00Machine);
		ensure_equals("Point is initialized",pH.isInitialized(), true);
		ensure_equals("No standard deviations",pH.hasStandDeviations(), false);

		pH.setCorrection(5, 150);
		pH.setCorrection(6, 2000);

		ensure_equals("Estimated values should match",pH.getEstimatedValue().getX().getMetresValue(), 151);
		ensure_equals("Estimated values should match",pH.getEstimatedValue().getY().getMetresValue(), 2002);
		ensure_equals("Estimated value of Z(H is fixed) have to change, because it depends on X a Y which has changed",pH.getEstimatedValue().getZ().getMetresValue(), 2002.73661251304, 1e-8);

		pH.setEstimatedPrecision(5, 0.1);
		pH.setEstimatedPrecision(6, 0.2);
		ensure_equals("Estimated precision should match", pH.getXEstPrecision().getMetresValue(), 0.1);
		ensure_equals("Estimated precision should match", pH.getYEstPrecision().getMetresValue(), 0.2);
		ensure_equals("Estimated precision should match", pH.getZEstPrecision().getMetresValue(), 0.0);

		pH.setXYEstimatedCovariance(0.01);
		EXPECT_FAIL(pH.setYZEstimatedCovariance(0.02));
		EXPECT_FAIL(pH.setXZEstimatedCovariance(0.02));
		ensure_equals("Covariance should match", pH.getXYCovar(), 0.01);

		ensure_equals("Reference frame should match", pH.getReferenceFrame(), TRefSystemFactory::ERefFrame::kCernXYHg00Machine);
		ensure_equals("Point is initialized", pH.isInitialized(), true);
		ensure_equals("No standard deviations", pH.hasStandDeviations(), false);

		EXPECT_FAIL(pH.setCorrection(10,10));
		pH.setStandardDeviations(0.1,0.2,0.3);
		ensure_equals("Standard deviations should match", pH.getStandDev(0), 0.1);
		ensure_equals("Standard deviations should match", pH.getStandDev(1), 0.2);
		ensure_equals("Standard deviations should match", pH.getStandDev(2), 0.3);
		ensure_equals("No standard deviations",pH.hasStandDeviations(), true);

		ensure_equals("Reference system should match", pH.getProvisionalValue().getCoordSys(), position2.getCoordSys());

		LGCAdjustablePoint unit = p.createUninitialized("point2");
		ensure_equals("Point is not initialized", unit.isInitialized(), false);

		//////////////////////// Testing TAdjustableScalar ////////////////////////

		//Testing TAdjustableLength
		TAdjustableLength adjLength(TLength(2.0),false, "adjlen");
		adjLength.setFirstUidx(2);

		ensure_equals("Provisional value should match", adjLength.getProvisionalValue(), 2.0);
		ensure_equals("Correction should match", adjLength.getCorrection(), 0.0);
		ensure_equals("Estimated value should match", adjLength.getEstimatedValue(), 2.0);
		ensure_equals("Estimated precision should match", adjLength.getEstimatedPrecision(), 0.0);
		ensure_equals("Number of unknowns should match", adjLength.getNumUnkn(), 1);
		ensure_equals("Scalar is variable", adjLength.isFixed(), false);
		ensure_equals("Scalar first index is 2", adjLength.getFirstUidx(), 2);
		ensure_equals("Scalar last index is 2", adjLength.getLastUidx(), 2);
		ensure_equals("Name should match", adjLength.getName(), "adjlen");
		ensure_equals("Name should match", adjLength.isInitialized(), true);

		adjLength.setCorrection(2,0.5);
		adjLength.setEstimatedPrecision(2,0.01);
		ensure_equals("Correction should match", adjLength.getCorrection(), 0.5);
		ensure_equals("New estimated value should match", adjLength.getEstimatedValue(), 2.5);
		EXPECT_FAIL(adjLength.setCorrection(3,0.5));
		EXPECT_FAIL(adjLength.setEstimatedPrecision(3,0.01));

		ensure_equals("Correction should match", adjLength.getEstimatedPrecision(), 0.01);

		//Testing TAdjustableAngle
	}

	// Test measurments
	template<>
	template<>
	void object::test<2>()
	{
		std::shared_ptr<TLGCData> proj5(new TLGCData);
		using namespace LGC;
		set_test_name("Testing Measurements");
		//Measurements are tested more properly in testReaders -- Testing measurement input -- 
		TPositionVector posit(1.0,1.0,1.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TDataTreeIterator iter;
		const LGCAdjustablePoint pos(posit, false, false, false, "point", TRefSystemFactory::ERefFrame::kLocalRefFrame, iter);
	
#ifdef __linux__
		const TInstrumentData::TPOLAR instrument{};
#else
		const TInstrumentData::TPOLAR instrument;
#endif
		TTSTN tstn( pos, instrument);
		LGCAdjustablePoint ssa = *tstn.instrumentPos;
		ssa.setXYEstimatedCovariance(3.0);

		TReader r(proj5);
		stringstream infile(LOR2LORInputFiles::plateFileOrig);
		r.read(infile);

		ensure_equals("One instrument defined", proj5->getInstruments().fPOLAR.size(),1);
		ensure_equals("Provisional value of the pair should match", proj5->getLength().getObject("TS1T1").getProvisionalValue(),2.0);
		ensure_equals("Dist. correction is 2.0",(*proj5->getInstruments().fPOLAR["TS1"].targets["T1"].distCorrectionAdjustable).getProvisionalValue(),2.0);	
		ensure_equals("2 adjustable scalars expected",proj5->getLength().numObjects(), 9);
	}

}