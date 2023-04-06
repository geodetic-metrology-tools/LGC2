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

	void readTest(char const *const testCase, std::string testCaseName)
	{
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/" + testCaseName + ".txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(testCase);

		succesReading = r.read(infiler);
	}

	std::shared_ptr<TLGCData> projTest;
	TReader r;
	bool succesReading;
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
	readTest(TestWPSR::ECWI_PARSE_1, "ECWI_PARSE_1");
	ensure_equals("Reading Successfull : No allowed ECWI in sub-frame", succesReading, false);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing ECWI, Translation of the Wire");
	readTest(TestWPSR::ECWI_OLOC_1, "ECWI_OLOC_1");
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

	const TLGCData &dataset = calcul.getData();

	int maxObsIdLength = 0;
	int i = 0;
	for (auto const &data : frameIt.node->data->measurements.fECWI.begin()->measECWI)
	{
		if (data.obsID.size() > maxObsIdLength)
		{
			maxObsIdLength = data.obsID.size();
		}
	}
	ensure_equals("The length of the biggest observation ID is correct", dataset.getConfig().obsIDwidth, maxObsIdLength);

	// Check the observation ID
	ensure_equals("The first observation ID is correct", frameIt.node->data->measurements.fECWI.begin()->measECWI.begin()->obsID, "First");
	ensure_equals("The first observation ID is correct", frameIt.node->data->measurements.fECWI.begin()->measECWI.back().obsID, "Second");

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), -0.1, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 0.2, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 0, 1e-7);

	ensure_equals("Computation should have 5 unknows", dataset.fUEOIndices.UIndex, 5);
	ensure_equals("Computation should have 12 equations", dataset.fUEOIndices.EIndex, 14);
	ensure_equals("Computation should have 12 observations", dataset.fUEOIndices.OIndex, 14);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing ECWI, rotation (bearing) of the Wire");
	readTest(TestWPSR::ECWI_OLOC_2, "ECWI_OLOC_2");
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
	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), 6.3451035, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing ECWI, Rotation (slope) of the Wire");
	readTest(TestWPSR::ECWI_OLOC_3, "ECWI_OLOC_3");
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
	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 400 - 6.3451035, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<5>()
{
	set_test_name("Testing ECWI, Translation (DRefX) and rotation (Bearing) of the Wire");
	readTest(TestWPSR::ECWI_OLOC_4, "ECWI_OLOC_4");
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
	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refAngle.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refAngle.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<6>()
{
	set_test_name("Testing ECWI, Translation (DRefZ) and rotation (Slope) of the Wire");
	readTest(TestWPSR::ECWI_OLOC_5, "ECWI_OLOC_5");
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
	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 1, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Testing ECWI, Translation (all) and Rotation (all) of the Wire, no sag");
	readTest(TestWPSR::ECWI_OLOC_6, "ECWI_OLOC_6");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 0, 1e-7);
}

template<>
template<>
void object::test<8>()
{
	set_test_name("Testing ECWI, sag only of the Wire");
	readTest(TestWPSR::ECWI_OLOC_7, "ECWI_OLOC_7");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMMetresValue(), 1000, 1e-4);
}

template<>
template<>
void object::test<9>()
{
	set_test_name("Testing ECWI, Translation (along Z) and sag of the Wire");
	readTest(TestWPSR::ECWI_OLOC_8, "ECWI_OLOC_8");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 1, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMMetresValue(), 1000, 1e-4);
}

template<>
template<>
void object::test<10>()
{
	set_test_name("Testing ECWI, Translation (along Z), sag and slope of the Wire");
	readTest(TestWPSR::ECWI_OLOC_9, "ECWI_OLOC_9");
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
	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), 0, 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), 0, 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), 1, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<11>()
{
	set_test_name("Testing ECWI, Translation, rotation and sag of the Wire");
	readTest(TestWPSR::ECWI_OLOC_10, "ECWI_OLOC_10");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<12>()
{
	set_test_name("Testing ECWI, inversion of the anchor points");
	readTest(TestWPSR::ECWI_OLOC_11, "ECWI_OLOC_11");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 200 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<13>()
{
	set_test_name("Testing ECWI, Non-Zero station point and frame parameters");
	readTest(TestWPSR::ECWI_OLOC_12, "ECWI_OLOC_12");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<14>()
{
	set_test_name("Testing ECWI, frame parameter contributions");
	readTest(TestWPSR::ECWI_OLOC_13, "ECWI_OLOC_13");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 200 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);

	// Check the estimated value of the FRAME_PT2
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
	set_test_name("Testing ECWI, OLOC point in root");
	readTest(TestWPSR::ECWI_OLOC_14, "ECWI_OLOC_14");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);

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
	set_test_name("Testing ECWI,  OLOC point in root + *VXZ");
	readTest(TestWPSR::ECWI_OLOC_15, "ECWI_OLOC_15");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 400 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);

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
	set_test_name("Testing ECWI, Translation and rotation of the Wire + more sub-frame");
	readTest(TestWPSR::ECWI_OLOC_16, "ECWI_OLOC_16");
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

	ensure_equals("Wire ref distance not matching", romIt->fWireDx->getEstimatedValue().getMetresValue(), cosq(refBearing.getRadiansValue()), 1e-7);
	ensure_equals("Wire Bearing not matching", romIt->fWireBearing->getEstimatedValue().getGonsValue(), refBearing.getGonsValue(), 1e-7);
	ensure_equals("Wire Height not matching", romIt->fWireDz->getEstimatedValue().getMetresValue(), refZ, 1e-7);
	ensure_equals("Wire Slope not matching", romIt->fWireSlope->getEstimatedValue().getGonsValue(), 200 - refSlope.getGonsValue(), 1e-7);
	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 1, 1e-7);

	// Check the estimated value of the FRAME_PT2
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Translation X not matching", frameIt.node->data->frame.getEstTranslation(0).getMetresValue(), -1, 1e-6);
	ensure_equals("Translation Z not matching", frameIt.node->data->frame.getEstTranslation(2).getMetresValue(), 2, 1e-6);
}

template<>
template<>
void object::test<18>()
{
	set_test_name("Testing ECWI, flags");
	readTest(TestWPSR::ECWI_OLOC_17, "ECWI_OLOC_17");
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// testing all the flags
	TDataTree tree = projTest->getTree();
	auto romIt = tree.begin().node->data->measurements.fECWI.begin();
	auto measIt = romIt->measECWI.begin();

	TInstrumentData finstruments = projTest->getInstruments();
	TInstrumentData::TWPSR instrumentWPSR1 = finstruments.getDevice(finstruments.fWPSR, "WP_SNSR_1");
	TInstrumentData::TWPSR instrument = instrumentWPSR1;

	auto testFlags = [&instrument, &measIt, &romIt](const TInstrumentData::TWPSR &instrumentDefault, bool checkObsID = false, std::string obsID = "") {
		auto measName = romIt->romName + "/" + measIt->targetPos->getName();
		ensure_equals("Sigma X not matching on " + measName, instrument.sigmaX, measIt->target.sigmaX, 1e-7);
		ensure_equals("Sigma Z not matching on " + measName, instrument.sigmaZ, measIt->target.sigmaZ, 1e-7);
		ensure_equals("Sigma instrument centering X not matching on " + measName, instrument.sigmaInstrCenteringX, measIt->target.sigmaInstrCenteringX, 1e-7);
		ensure_equals("Sigma instrument centering X not matching on " + measName, instrument.sigmaInstrCenteringZ, measIt->target.sigmaInstrCenteringZ, 1e-7);
		ensure_equals("Sigma Wire not matching on " + measName, instrument.sigmaWire, measIt->target.sigmaWire, 1e-7);
		ensure_equals("Sigma Sag Wire not matching on " + measName, instrument.sigmaSagWire, romIt->instrument.sigmaSagWire, 1e-7);
		ensure_equals("Wire ID not matching on " + measName, instrument.ID, measIt->target.ID);
		if (checkObsID)
			ensure_equals("Observation ID not matching on " + measName, obsID, measIt->obsID);
		// re-initialise the intrument values
		instrument = instrumentDefault;
	};

	// go through the measurements of WPS1
	testFlags(instrumentWPSR1); // PTMINUS
	measIt++; // PT0
	instrument.sigmaX = TLength(0.05, TLength::EUnits::kMillimetres);
	testFlags(instrumentWPSR1);

	measIt++; // PT1
	instrument.sigmaInstrCenteringX = TLength(0.05, TLength::EUnits::kMillimetres);
	testFlags(instrumentWPSR1);

	measIt++; // PT2
	instrument.sigmaInstrCenteringZ = TLength(0.05, TLength::EUnits::kMillimetres);
	testFlags(instrumentWPSR1);

	measIt++; // PT3
	testFlags(instrumentWPSR1, true, "TEST");

	measIt++; // PT4
	instrument.sigmaZ = TLength(0.05, TLength::EUnits::kMillimetres);
	testFlags(instrumentWPSR1);

	measIt++; // PT5
	testFlags(instrumentWPSR1);

	// go through the measurements of WPS1
	romIt++;
	measIt = romIt->measECWI.begin();

	// Update instruments
	TInstrumentData::TWPSR instrumentWPSR2 = finstruments.getDevice(finstruments.fWPSR, "WP_SNSR_2");
	instrumentWPSR2.sagWire = TLength(0.1, TLength::EUnits::kMillimetres);
	instrumentWPSR2.sigmaSagWire = TLength(0.5, TLength::EUnits::kMillimetres);
	instrumentWPSR1.sagWire = TLength(0.1, TLength::EUnits::kMillimetres);
	instrumentWPSR1.sigmaSagWire = TLength(0.5, TLength::EUnits::kMillimetres);
	instrument = instrumentWPSR2;

	testFlags(instrumentWPSR1); // PTMINUS

	measIt++; // PT0
	testFlags(instrumentWPSR1);

	measIt++; // PT1
	testFlags(instrumentWPSR2);

	measIt++; // PT2
	testFlags(instrumentWPSR2);

	measIt++; // PT3
	testFlags(instrumentWPSR1);

	measIt++; // PT4
	testFlags(instrumentWPSR1);

	measIt++; // PT5
	testFlags(instrumentWPSR1);
}

template<>
template<>
void object::test<19>()
{
	set_test_name("Testing ECWI, SAGFIX test");
	readTest(TestWPSR::ECWI_OLOC_18, "ECWI_OLOC_18");
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

	const TLGCData &dataset = calcul.getData();

	ensure_equals("Wire SAG not matching", romIt->sagAdjustable->getEstimatedValue().getMetresValue(), 0, 1e-7);

	ensure_equals("Computation should have 4 unknows", dataset.fUEOIndices.UIndex, 4);
	ensure_equals("Computation should have 12 equations", dataset.fUEOIndices.EIndex, 14);
	ensure_equals("Computation should have 12 observations", dataset.fUEOIndices.OIndex, 14);
}

template<>
template<>
void object::test<20>()
{
	set_test_name("Testing ECWI, IP8");
	readTest(TestWPSR::ECWI_LEP_1, "ECWI_LEP_1");
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();

	// Check the coordinates of points in the root (from a reference computation)
	TPositionVector PT1 = dataset.getPoints().getObject("BEAM_LHC.MQXA.3L8.E").getEstimatedValue();
	TLOR2LOR trafo1(tree, "LHC.MQXA.3L8.103756.RSTB", "ROOT", "T1");
	trafo1.transform(PT1);
	ensure_equals("BEAM_LHC.MQXA.3L8.E X coordinate should match", PT1.getX().getMetresValue(), 4492.732871, 1.5e-5);
	ensure_equals("BEAM_LHC.MQXA.3L8.E Y coordinate should match", PT1.getY().getMetresValue(), 5055.338695, 1.5e-5);
	ensure_equals("BEAM_LHC.MQXA.3L8.E Z coordinate should match", PT1.getZ().getMetresValue(), 2329.826748, 1.5e-5);

	TPositionVector PT2 = dataset.getPoints().getObject("BEAM_LHC.MCBX.3R8.E").getEstimatedValue();
	TLOR2LOR trafo2(tree, "LHC.MQXA.3R8.103761.RSTB", "ROOT", "T2");
	trafo2.transform(PT2);
	ensure_equals("BEAM_LHC.MCBX.3R8.E X coordinate should match", PT2.getX().getMetresValue(), 4452.234637, 1.5e-5);
	ensure_equals("BEAM_LHC.MCBX.3R8.E Y coordinate should match", PT2.getY().getMetresValue(), 4956.401203, 1.5e-5);
	ensure_equals("BEAM_LHC.MCBX.3R8.E Z coordinate should match", PT2.getZ().getMetresValue(), 2330.272103, 1.5e-5);

	TPositionVector PT3 = dataset.getPoints().getObject("LHC.MQXB.B2R8.A").getEstimatedValue();
	ensure_equals("LHC.MQXB.B2R8.A X coordinate should match", PT3.getX().getMetresValue(), 4459.348359, 1.5e-5);
	ensure_equals("LHC.MQXB.B2R8.A Y coordinate should match", PT3.getY().getMetresValue(), 4974.880233, 1.5e-5);
	ensure_equals("LHC.MQXB.B2R8.A Z coordinate should match", PT3.getZ().getMetresValue(), 2330.734785, 1.5e-5);
}


template<>
template<>
void object::test<21>()
{
	set_test_name("Testing WPSR, 4 arguments (3 declared) in INSTR section");
	readTest(TestWPSR::ECWI_PARSE_2, "ECWI_PARSE_2");
	ensure_equals("Reading Successfull : Too little WPSR arguments not detected", succesReading, false);
}

template<>
template<>
void object::test<22>()
{
	set_test_name("Testing WPSR, 4 arguments (5 declared) in INSTR section");
	readTest(TestWPSR::ECWI_PARSE_3, "ECWI_PARSE_3");
	ensure_equals("Reading Successfull : Too Many WPSR arguments not detected", succesReading, false);
}

template<>
template<>
void object::test<23>()
{
	set_test_name("Testing ECWI, Wire name duplicate");
	readTest(TestWPSR::ECWI_PARSE_4, "ECWI_PARSE_4");
	ensure_equals("Reading Successfull : Same wire name not detected", succesReading, false);
}

template<>
template<>
void object::test<24>()
{
	set_test_name("Testing ECWI, 5 arguments (4 declared) ");
	readTest(TestWPSR::ECWI_PARSE_5, "ECWI_PARSE_5");
	ensure_equals("Reading Successfull : Too Little ECWI arguments not detected", succesReading, false);
}

template<>
template<>
void object::test<25>()
{
	set_test_name("Testing ECWI,SIMULATION ");
	readTest(TestWPSR::ECWI_SIMU_1, "ECWI_SIMU_1");
	ensure_equals("Reading Successfull : Simulation failed", succesReading, true);
}
} // namespace tut
