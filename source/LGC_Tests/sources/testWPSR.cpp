#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "TXYH2CCS.h"
#include "testWPSR.h"

namespace tut
{
struct test_WPSR
{
	test_WPSR() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

	std::shared_ptr<TLGCData> projTest;
	TReader r;
};
typedef test_group<test_WPSR> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("WPSR tests");
}

namespace tut
{

//------------------------------------------ ECWI -------------------------------------------------//

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing ECWI, 1 STATION in sub-frame");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_PARSE_1.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_PARSE_1);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull : No allowed ECWI in sub-frame", succesReading, false);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing ECWI, Translation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_1.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_1);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), -0.1, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), 0.2, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_2.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_2);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(),0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), 6.3451035, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), 0, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_3.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_3);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), 0, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 400-6.3451035, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<5>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_4.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_4);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refAngle(6.3451035, TAngle::EUnits::kGons);
	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refAngle.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refAngle.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), 0 , 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<6>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_5.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_5);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refSlope(400 - 6.3451035, TAngle::EUnits::kGons);
	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(),0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), 1, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_6.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_6);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(6.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());

	auto test2 = cosq(refBearing.getRadiansValue());
	auto test = romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ();
	auto testtt = romIt->fMeasuredPlane->getReferencePoint();
	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<8>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_7.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_7);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(),0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(),0, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMMetresValue(), 1000, 1e-4);
}

template<>
template<>
void object::test<9>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_8.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_8);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), 1, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMMetresValue(), 1000, 1e-4);
}

template<>
template<>
void object::test<10>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_9.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_9);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refSlope(400 - 6.3451035, TAngle::EUnits::kGons);
	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), 1, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<11>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_10.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_10);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(6.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);
}


template<>
template<>
void object::test<12>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_11.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_11);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(206.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(),200- refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<13>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_12.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_12);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	//ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(6.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);
}


template<>
template<>
void object::test<14>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_13.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_13);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(206.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());
	

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 200 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);
	
	//Check the estimated value of the FRAME_PT2
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Translation X not matching", frameIt.node->data->frame.getEstTranslation(0).getMetresValue(), -1, 1e-7);
	ensure_equals("Translation Z not matching", frameIt.node->data->frame.getEstTranslation(2).getMetresValue(), -2, 1e-7);
}

template<>
template<>
void object::test<15>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_14.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_14);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(6.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);

	// Check the estimated value of PT2
	const TLGCData &dataset = calcul.getData();
	TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
	ensure_equals("PT2 X corrdinate should match", PT2.getX().getMetresValue(), 0, 1e-7);
	ensure_equals("PT2 Y corrdinate should match", PT2.getY().getMetresValue(), 2, 1e-7);
	ensure_equals("PT2 Z corrdinate should match", PT2.getZ().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<16>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_15.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_15);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(6.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());

	ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);

	// Check the estimated value of PT2
	const TLGCData &dataset = calcul.getData();
	TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
	ensure_equals("PT2 X corrdinate should match", PT2.getX().getMetresValue(), 1.9, 1e-7);
	ensure_equals("PT2 Y corrdinate should match", PT2.getY().getMetresValue(), 2, 1e-7);
	ensure_equals("PT2 Z corrdinate should match", PT2.getZ().getMetresValue(), -0.02, 1e-7);
}

template<>
template<>
void object::test<17>()
{
	set_test_name("Testing ECWI, Translation and rotation of the Wire");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/ECWI_OLOC_16.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestWPSR::ECWI_OLOC_16);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	//ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	// testing all the parameters of the wire
	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fECWI.begin();
	TAngle refBearing(6.3451035, TAngle::EUnits::kGons);
	TAngle refSlope(atan2(0.5, 5 / cosq(refBearing.getRadiansValue())), TAngle::EUnits::kRadians);
	TReal distForRefZ = cosq(refBearing.getRadiansValue()) * tan(refBearing.getRadiansValue());
	TReal refZ = 1 - distForRefZ * tan(refSlope.getRadiansValue());

	//ensure_equals("Wire ref distance not matching", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	//ensure_equals("Wire Bearing not matching", romIt->fMeasuredPlane->getThetaEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	//ensure_equals("Wire Height not matching", romIt->fMeasuredPlane->getReferencePoint()->getEstimatedValue().getZ(), refZ, 1e-7);
	//ensure_equals("Wire Slope not matching", romIt->fMeasuredPitch->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	//ensure_equals("Wire SAG not matching", romIt->fMeasuredSAG->getEstimatedValue().getMetresValue(), 1, 1e-7);

	// Check the estimated value of the FRAME_PT2
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Translation X not matching", frameIt.node->data->frame.getEstTranslation(0).getMetresValue(), -1, 1e-6);
	ensure_equals("Translation Z not matching", frameIt.node->data->frame.getEstTranslation(2).getMetresValue(), 2, 1e-6);
}
} // namespace tut
