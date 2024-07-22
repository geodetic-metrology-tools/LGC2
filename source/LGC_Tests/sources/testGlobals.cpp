#include <tut/tut.hpp>
#include <tut/tut_macros.hpp>


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
		ensure_equals("Point has two unknowns", p.getRelativeUnknIndices().size(), 2);
		ensure_equals("First active point place is 1", p.getRelativeUnknIndices().at(0), 1);
		ensure_equals("Second active point place is 2", p.getRelativeUnknIndices().at(1), 2);


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
		ensure_THROW(pH.getCoordinateUnknIndex(2), std::logic_error);
	
		ensure_equals("Point has two unknowns", pH.getRelativeUnknIndices().size(), 2);
		ensure_equals("First active point place is 0", pH.getRelativeUnknIndices().at(0), 0);
		ensure_equals("Second active point place is 1", pH.getRelativeUnknIndices().at(1), 1);


		ensure_THROW(pH.getStandDev(0), std::runtime_error);
		ensure_equals("Reference frame should match",pH.getReferenceFrame(), TRefSystemFactory::ERefFrame::kCernXYHg00Machine);
		ensure_equals("Point is initialized",pH.isInitialized(), true);
		ensure_equals("No standard deviations",pH.hasStandDeviations(), false);

		pH.setCorrection(5, 150);
		pH.setCorrection(6, 2000);

		ensure_equals("Estimated values should match",pH.getEstimatedValue().getX().getMetresValue(), 151);
		ensure_equals("Estimated values should match",pH.getEstimatedValue().getY().getMetresValue(), 2002);
		ensure_equals("Estimated value of Z(H is fixed) have to change, because it depends on X a Y which has changed",pH.getEstimatedValue().getZ().getMetresValue(), 2002.73661251304, 1e-8);

		Eigen::Matrix3d covarMat;
		// according to xyz precisions 0.1, 0.2, 0.0 and xy covariance 0.01
		covarMat << 0.01, 0.01, 0, 0.01, 0.2 * 0.2, 0, 0, 0, 0;
		pH.setCovarianceMatrix(covarMat);

		ensure_equals("Estimated precision should match", pH.getXEstPrecision().getMetresValue(), 0.1);
		ensure_equals("Estimated precision should match", pH.getYEstPrecision().getMetresValue(), 0.2);
		ensure_equals("Estimated precision should match", pH.getZEstPrecision().getMetresValue(), 0.0);
		ensure_equals("Covariance should match", pH.getXYCovar(), 0.01);

		ensure_equals("Reference frame should match", pH.getReferenceFrame(), TRefSystemFactory::ERefFrame::kCernXYHg00Machine);
		ensure_equals("Point is initialized", pH.isInitialized(), true);
		ensure_equals("No standard deviations", pH.hasStandDeviations(), false);

		ensure_THROW(pH.setCorrection(10,10), std::logic_error);
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
		ensure_THROW(adjLength.setCorrection(3,0.5), std::logic_error);
		ensure_THROW(adjLength.setEstimatedPrecision(3,0.01), std::logic_error);

		ensure_equals("Correction should match", adjLength.getEstimatedPrecision(), 0.01);

		//Testing TAdjustableAngle

		// Testing AdjustableHelmertTransformation
		TransformParameters par;
		auto GON(TAngle::kGons);		
		par.omega = TAngle(10.0, GON);
		par.phi = TAngle(20.0, GON);
		par.kappa = TAngle(30.0, GON);
		par.tX = TLength(1.0);
		par.tY = TLength(2.0);
		par.tZ = TLength(3.0);
		par.scale = 2;
		std::bitset<3> transFixed(std::string("101"));
		std::bitset<3> rotFixed(std::string("010"));
		std::bitset<1> scaleFixed(std::string("0"));
		TAdjustableHelmertTransformation adjTrafo(par, transFixed, rotFixed, scaleFixed, "TrafoName");
		ensure_equals("Degree of Freedom is 4", adjTrafo.getNumUnkn(), 4);
		ensure_equals("Trafo has 4 unknown indices", adjTrafo.getRelativeUnknIndices().size(), 4);
		ensure_equals("1. Trafo unknown is at place 1", adjTrafo.getRelativeUnknIndices().at(0), 1);
		ensure_equals("2. Trafo unknown is at place 3", adjTrafo.getRelativeUnknIndices().at(1), 3);
		ensure_equals("3. Trafo unknown is at place 5", adjTrafo.getRelativeUnknIndices().at(2), 5);
		ensure_equals("4. Trafo unknown is at place 6", adjTrafo.getRelativeUnknIndices().at(3), 6);
		ensure_equals("Name should match", adjTrafo.getName(), "TrafoName");
		ensure_equals("Trafo should be initialized", adjTrafo.isInitialized(), true);

		// Testing consistency of covars and precisions for AdjustableHelmertTransformation
		Eigen::Matrix<double, 7, 7> covar = Eigen::Matrix<double, 7, 7>::Zero();
		covar.diagonal() << 2, 3, 4, 5, 6, 7, 8;
		adjTrafo.setCovar(covar);
		Eigen::Matrix<double, 7, 7> returnedCovar = adjTrafo.getCovar();
		ensure_equals("Covariance should be as set before", returnedCovar, covar);
		ensure_equals("Precisions and covariance need to be consistent.", sqrt(returnedCovar(0, 0)), adjTrafo.getEstimatedPrecisionTransl(0).getMetresValue());
		ensure_equals("Precisions and covariance need to be consistent.", sqrt(returnedCovar(1, 1)), adjTrafo.getEstimatedPrecisionTransl(1).getMetresValue());
		ensure_equals("Precisions and covariance need to be consistent.", sqrt(returnedCovar(2, 2)), adjTrafo.getEstimatedPrecisionTransl(2).getMetresValue());
		ensure_equals("Precisions and covariance need to be consistent.", sqrt(returnedCovar(3, 3)), adjTrafo.getEstimatedPrecisionRot(0).getRadiansValue());
		ensure_equals("Precisions and covariance need to be consistent.", sqrt(returnedCovar(4, 4)), adjTrafo.getEstimatedPrecisionRot(1).getRadiansValue());
		ensure_equals("Precisions and covariance need to be consistent.", sqrt(returnedCovar(5, 5)), adjTrafo.getEstimatedPrecisionRot(2).getRadiansValue());
		ensure_equals("Precisions and covariance need to be consistent.", sqrt(returnedCovar(6, 6)), adjTrafo.getEstimatedPrecisionScale());
	}

	// Test adjustable object collection
	template<>
	template<>
	void object::test<2>()
	{
		set_test_name("Testing adjustable Object collection");
		// create empty collection
		LGCAdjustablePointCollection testCollection;
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());
		// cretae a point object
		LGCAdjustablePoint point1 = LGCAdjustablePoint::createUninitialized("Point1");
		// necessary to have a initialized object
		point1.setProvisionalValue(1, 2, 3);
		// add the object
		testCollection.addObject(point1);
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());
		// test if it is added succesfully
		ensure_equals("Point1 needs to be in collection", true, testCollection.doesObjectExist("Point1"));
		LGCAdjustablePoint &ref1 = testCollection.getObject("Point1");
		ref1.setActive(false);
		LGCAdjustablePoint &ref2 = testCollection.getObject("Point1");
		ensure_equals("Point1 in collection now needs to be deactivated", false, ref2.isActive());
		// create a new point with the same name and add it to collection. the previous point content should be overwritten
		LGCAdjustablePoint secondPoint1 = LGCAdjustablePoint::createUninitialized("Point1");
		testCollection.addObject(secondPoint1);
		ensure_equals("Point1 in collection now needs to be activated again", true, testCollection.getObject("Point1").isActive());

		int nObjects = testCollection.numObjects();

		// test adding objects via getObject
		testCollection.getObject("newPoint1");
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());
		LGCAdjustablePoint &newRef1 = testCollection.back();
		ensure_equals("last point in collection needs to be \"newPoint1\"", newRef1.getName(), "newPoint1");
		ensure_equals("number of objects need to increase by one", ++nObjects, testCollection.numObjects());

		// test adding object via add method
		LGCAdjustablePoint point2 = LGCAdjustablePoint::createUninitialized("newPoint2");
		testCollection.addObject(point2);
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());
		LGCAdjustablePoint &newRef2 = testCollection.back();
		ensure_equals("last point in collection needs to be \"newPoint2\"", newRef2.getName(), "newPoint2");
		ensure_equals("number of objects need to increase by one", ++nObjects, testCollection.numObjects());

		// test remove method
		testCollection.removeObject(newRef2);
		LGCAdjustablePoint &newRef3 = testCollection.back();
		ensure_equals("New last point has to be ", newRef1.getName(), newRef3.getName());
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());

		// test the removeIf method
		// create a point collection with a few active and deactivated points
		LGCAdjustablePointCollection testCollection2;
		int numPoints = 10;
		for (int j = 0; j < numPoints; j++)
		{
			std::string nameInactive = "inactivePoint" + std::to_string(j);
			std::string nameActive = "activePoint" + std::to_string(j);
			LGCAdjustablePoint inactPt = LGCAdjustablePoint::createUninitialized(nameInactive);
			LGCAdjustablePoint actPt = LGCAdjustablePoint::createUninitialized(nameActive);
			testCollection2.addObject(inactPt);
			testCollection2.addObject(actPt);
		}
		ensure_equals("Collection contains wrong number of objects", testCollection2.numObjects(), 2 * numPoints);
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());

		// now deactivate some points
		for (int j = 0; j < numPoints; j++)
		{
			std::string nameInactive = "inactivePoint" + std::to_string(j);
			testCollection2.getObject(nameInactive).setActive(false);
		}

		// remove deactivated points from collection, as it is done e.g. in the TDataAnalyzer
		testCollection2.removeObjectIf([](const LGCAdjustablePoint &pt) -> bool { return !pt.isActive(); });
		ensure_equals("Collection contains wrong number of objects", testCollection2.numObjects(), numPoints);
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());
		// remove also the rest
		testCollection2.removeObjectIf([](const LGCAdjustablePoint &pt) -> bool { return pt.isActive(); });
		ensure_equals("Collection contains wrong number of objects", testCollection2.numObjects(), 0);
		ensure_equals("Map and List have to be consistent", true, testCollection.checkMapConsistency());
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
		const LGCAdjustablePoint pos(posit, false, false, false, "point", TRefSystemFactory::ERefFrame::kLocalRefFrame, iter);
	
		const TInstrumentData::TPOLAR instrument{};

		TTSTN tstn( pos, instrument);
		LGCAdjustablePoint ssa = *tstn.instrumentPos;

		TReader r(proj5);
		stringstream infile(LOR2LORInputFiles::plateFileOrig);
		r.read(infile);

		ensure_equals("One instrument defined", proj5->getInstruments().fPOLAR.size(),1);
		// ensure_equals("Provisional value of the pair should match", proj5->getLength().getObject("TS1T1").getProvisionalValue(),2.0);
		ensure_equals("Dist. correction is 2.0", proj5->getInstruments().fPOLAR["TS1"]->targets["T1"]->distCorrectionValue, 2.0);	
		// ensure_equals("1 adjustable scalar expected",proj5->getLength().numObjects(), 1);
	}

}