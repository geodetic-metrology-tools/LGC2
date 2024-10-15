#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <cmath>

#include <Behavior.h>
#include <TLGCData.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "testDLEV.h"

namespace tut
{
struct test_DLEV
{
	test_DLEV() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

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
typedef test_group<test_DLEV> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("DLEV tests");
}

namespace tut
{

//------------------------------------------ DLEV -------------------------------------------------//

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing reading LEVEL keyword");
	readTest(TestDLEV::DLEV_PARSE_NOK_1, "DLEV_PARSE_NOK_1");
	ensure_equals("Reading Succesful while too many LEVEL parameters are defined", succesReading, false);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing reading LEVEL keyword");
	readTest(TestDLEV::DLEV_PARSE_NOK_2, "DLEV_PARSE_NOK_2");
	ensure_equals("Reading Succesful while not enough LEVEL parameters are defined", succesReading, false);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing reading TARGET keyword");
	readTest(TestDLEV::DLEV_PARSE_NOK_3, "DLEV_PARSE_NOK_3");
	ensure_equals("Reading Succesful while too many TARGET parameters are defined", succesReading, false);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing reading TARGET keyword");
	readTest(TestDLEV::DLEV_PARSE_NOK_4, "DLEV_PARSE_NOK_4");
	ensure_equals("Reading Succesful while not enough TARGET parameters are defined", succesReading, false);
}

template<>
template<>
void object::test<5>()
{
	set_test_name("Testing reading INSTR and OBSERVATION keyword");
	readTest(TestDLEV::DLEV_OLOC_1, "DLEV_OLOC_1");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("ihfix should be false", romIt->ihfix, false);
	ensure_equals("hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("IH should be initialized to 0", romIt->instrument.instrHeight.getMetresValue(), 0.0);
	ensure_equals("IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");

	auto measDlevIt = romIt->measDLEV.begin();
	ensure_equals("First measurement: Instrument name should match", measDlevIt->target.ID, "TARG1");
	ensure_equals("First measurement: OBSE should match", measDlevIt->target.sigmaD.getMMetresValue(), 3.0);
	ensure_equals("First measurement: PPM should match", measDlevIt->target.ppmD.getMMetresValue(), 4.0);
	ensure_equals("First measurement: DCOR should match", measDlevIt->target.distCorrectionValue.getMetresValue(), 0.2);
	ensure_equals("First measurement: DCOR Sigma should match", measDlevIt->target.sigmaDCorr.getMMetresValue(), 0.3);
	ensure_equals("First measurement: TH should match", measDlevIt->target.staffHt.getMetresValue(), 0.5);
	ensure_equals("First measurement: THSE should match", measDlevIt->target.sigmaStaffHt.getMMetresValue(), 5.0);
	ensure_equals("First measurement: ID should match", measDlevIt->obsID, "TEST1");
	ensure_equals("First measurement: DLEV measurement should match", measDlevIt->getDistance().getMetresValue(), 0.5);
	ensure_equals("First measurement: DHOR measurement should match", measDlevIt->dhor->getDistance().getMetresValue(), 7.071067812);
	ensure_equals("First measurement: DHOR sigma should match", measDlevIt->dhor->getDHORSigma().getMMetresValue(), 1.0);

	measDlevIt++;
	ensure_equals("Second measurement: Instrument name should match", measDlevIt->target.ID, "TARG1");
	ensure_equals("Second measurement: OBSE should match", measDlevIt->target.sigmaD.getMMetresValue(), 0.0176);
	ensure_equals("Second measurement: PPM should match", measDlevIt->target.ppmD.getMMetresValue(), 1.0);
	ensure_equals("Second measurement: DCOR should match", measDlevIt->target.distCorrectionValue.getMetresValue(), 0.2);
	ensure_equals("Second measurement: DCOR Sigma should match", measDlevIt->target.sigmaDCorr.getMMetresValue(), 0.3);
	ensure_equals("Second measurement: TH should match", measDlevIt->target.staffHt.getMetresValue(), 0.04);
	ensure_equals("Second measurement: THSE should match", measDlevIt->target.sigmaStaffHt.getMMetresValue(), 0.03);
	ensure_equals("Second measurement: ID should match", measDlevIt->obsID, "TEST2");
	ensure_equals("Second measurement: DLEV measurement should match", measDlevIt->getDistance().getMetresValue(), 1.0);
	ensure_equals("Second measurement: DHOR measurement should match", measDlevIt->dhor->getDistance().getMetresValue(), 7.071067812);
	ensure_equals("Second measurement: DHOR sigma should match", measDlevIt->dhor->getDHORSigma().getMMetresValue(), 3.0);

	measDlevIt++;
	ensure_equals("Third measurement: Instrument name should match", measDlevIt->target.ID, "TARG2");
	ensure_equals("Third measurement: OBSE should match", measDlevIt->target.sigmaD.getMMetresValue(), 0.02);
	ensure_equals("Third measurement: PPM should match", measDlevIt->target.ppmD.getMMetresValue(), 0.0);
	ensure_equals("Third measurement: DCOR should match", measDlevIt->target.distCorrectionValue.getMetresValue(), 0.0);
	ensure_equals("Third measurement: DCOR Sigma should match", measDlevIt->target.sigmaDCorr.getMMetresValue(), 0.0);
	ensure_equals("Third measurement: TH should match", measDlevIt->target.staffHt.getMetresValue(), 0.00);
	ensure_equals("Third measurement: THSE should match", measDlevIt->target.sigmaStaffHt.getMMetresValue(), 0.00);
	ensure_equals("Third measurement: DLEV measurement should match", measDlevIt->getDistance().getMetresValue(), 1.0);
	ensure_equals("Third measurement: DHOR measurement should match", measDlevIt->dhor->getDistance().getMetresValue(), 7.071067812);
	ensure_equals("Third measurement: DHOR sigma should match", measDlevIt->dhor->getDHORSigma().getMMetresValue(), 2.0);

	measDlevIt++;
	ensure_equals("Fourth measurement: Instrument name should match", measDlevIt->target.ID, "TARG1");
	ensure_equals("Fourth measurement: OBSE should match", measDlevIt->target.sigmaD.getMMetresValue(), 0.0176);
	ensure_equals("Fourth measurement: PPM should match", measDlevIt->target.ppmD.getMMetresValue(), 1.0);
	ensure_equals("Fourth measurement: DCOR should match", measDlevIt->target.distCorrectionValue.getMetresValue(), 0.2);
	ensure_equals("Fourth measurement: DCOR Sigma should match", measDlevIt->target.sigmaDCorr.getMMetresValue(), 0.3);
	ensure_equals("Fourth measurement: TH should match", measDlevIt->target.staffHt.getMetresValue(), 0.04);
	ensure_equals("Fourth measurement: THSE should match", measDlevIt->target.sigmaStaffHt.getMMetresValue(), 0.03);
	ensure_equals("Fourth measurement: DLEV measurement should match", measDlevIt->getDistance().getMetresValue(), 1.0);
	ensure_equals("Fourth measurement: DHOR measurement should match", measDlevIt->dhor->getDistance().getMetresValue(), 7.071067812);
	ensure_equals("Fourth measurement: DHOR sigma should match", measDlevIt->dhor->getDHORSigma().getMMetresValue(), 1.0);
}

template<>
template<>
void object::test<6>()
{
	set_test_name("Testing reading IH keyword");
	readTest(TestDLEV::DLEV_OLOC_2, "DLEV_OLOC_2");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, false);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, false);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 1.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: RefPt should match", romIt->fRefPt->getName(), "ST");

	romIt++;
	ensure_equals("Second Instr: ihfix should be true", romIt->ihfix, true);
	ensure_equals("Second Instr: hasDHOR should be true", romIt->hasDHOR, false);
	ensure_equals("Second Instr: IH should be 2m", romIt->instrument.instrHeight.getMetresValue(), 2.0);
	ensure_equals("Second Instr: IHSE should be to 0.4mm", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.4);
	ensure_equals("Second Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("Second Instr: RefPt should match", romIt->fRefPt->getName(), "ST");
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Testing reading IH keyword: creating a VXY with DHOR");
	readTest(TestDLEV::DLEV_OLOC_3, "DLEV_OLOC_3");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, false);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 1.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: Station point should have 2 unknowns", romIt->fRefPt->getNumUnkn(), 2);
	ensure_equals("First Instr: Station point X shoulf be 15", romIt->fRefPt->getEstimatedValue().getX().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Y shoulf be 15", romIt->fRefPt->getEstimatedValue().getY().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Z shoulf be 2.2", romIt->fRefPt->getEstimatedValue().getZ().getMetresValue(), 2.2, 1e-6);
	ensure_equals("First Instr: Reference plane prov distance should match", romIt->fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 1.0, 1e-6);
	ensure_equals("First Instr: Reference plane est distance should match", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0.8, 1e-6);

	const TLGCData &dataset = calcul.getData();
	TPositionVector PT5 = dataset.getPoints().getObject("PT5").getEstimatedValue();
	ensure_equals("PT5 Z coordinate should match", PT5.getZ().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<8>()
{
	set_test_name("Testing reading IH keyword: creating a POIN with DHOR");
	readTest(TestDLEV::DLEV_OLOC_4, "DLEV_OLOC_4");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, true);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 1.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: Station point should have 3 unknowns", romIt->fRefPt->getNumUnkn(), 3);
	ensure_equals("First Instr: Station point X shoulf be 15", romIt->fRefPt->getEstimatedValue().getX().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Y shoulf be 15", romIt->fRefPt->getEstimatedValue().getY().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Z shoulf be 2", romIt->fRefPt->getEstimatedValue().getZ().getMetresValue(), 2.0, 1e-6);
	ensure_equals("First Instr: Reference plane prov distance should match", romIt->fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 1.0, 1e-6);
	ensure_equals("First Instr: Reference plane est distance should match", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 1.0, 1e-6);

	const TLGCData &dataset = calcul.getData();
	TPositionVector PT5 = dataset.getPoints().getObject("PT5").getEstimatedValue();
	ensure_equals("PT5 Z coordinate should match", PT5.getZ().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<9>()
{
	set_test_name("Testing reading IH keyword: creating a POIN with DHOR and a change in IH");
	readTest(TestDLEV::DLEV_OLOC_5, "DLEV_OLOC_5");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, true);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 4.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.3);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: Station point should have 3 unknowns", romIt->fRefPt->getNumUnkn(), 3);
	ensure_equals("First Instr: Station point X shoulf be 15", romIt->fRefPt->getEstimatedValue().getX().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Y shoulf be 15", romIt->fRefPt->getEstimatedValue().getY().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Z shoulf be -1", romIt->fRefPt->getEstimatedValue().getZ().getMetresValue(), -1.0, 1e-6);
	ensure_equals("First Instr: Reference plane prov distance should match", romIt->fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 4.0, 1e-6);
	ensure_equals("First Instr: Reference plane est distance should match", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 4.0, 1e-6);

	const TLGCData &dataset = calcul.getData();
	TPositionVector PT5 = dataset.getPoints().getObject("PT5").getEstimatedValue();
	ensure_equals("PT5 Z coordinate should match", PT5.getZ().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<10>()
{
	set_test_name("Testing reading IH keyword: IH and IHSE without IHFIX should be ignored");
	readTest(TestDLEV::DLEV_OLOC_6, "DLEV_OLOC_6");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-10);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, false);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 1.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: Station point should have 2 unknowns", romIt->fRefPt->getNumUnkn(), 2);
	ensure_equals("First Instr: Station point X shoulf be 15", romIt->fRefPt->getEstimatedValue().getX().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Y shoulf be 15", romIt->fRefPt->getEstimatedValue().getY().getMetresValue(), 15.0, 1e-6);
	ensure_equals("First Instr: Station point Z shoulf be 2.2", romIt->fRefPt->getEstimatedValue().getZ().getMetresValue(), 2.2, 1e-6);
	ensure_equals("First Instr: Reference plane prov distance should match", romIt->fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 1.0, 1e-6);
	ensure_equals("First Instr: Reference plane est distance should match", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0.8, 1e-6);

	const TLGCData &dataset = calcul.getData();
	TPositionVector PT5 = dataset.getPoints().getObject("PT5").getEstimatedValue();
	ensure_equals("PT5 Z coordinate should match", PT5.getZ().getMetresValue(), 1, 1e-7);
}

template<>
template<>
void object::test<11>()
{
	set_test_name("Testing a computation in SPHE");
	readTest(TestDLEV::DLEV_SPHE_1, "DLEV_SPHE_1");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, false);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 1.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: Station point should have 2 unknowns", romIt->fRefPt->getNumUnkn(), 2);
	ensure_equals("First Instr: Station point X shoulf be 2015", romIt->fRefPt->getEstimatedValue().getX().getMetresValue(), 2015.0, 1e-5);
	ensure_equals("First Instr: Station point Y shoulf be 2015", romIt->fRefPt->getEstimatedValue().getY().getMetresValue(), 2015.0, 1e-5);
	ensure_equals("First Instr: Station point Z shoulf be 2330.2", romIt->fRefPt->getEstimatedValue().getZ().getMetresValue(), 2330.20023, 1e-5);
	ensure_equals("First Instr: Station point H shoulf be 330.19999", romIt->fRefPt->getEstimatedHeightInRoot().getMetresValue(), 330.19999, 1e-5);
	ensure_equals("First Instr: Station point prov H shoulf be 330.19999", romIt->fRefPt->getProvisionalHeightInRoot().getMetresValue(), 330.19999, 1e-5);
	ensure_equals("First Instr: Reference plane prov distance should match", romIt->fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 1.0, 1e-6);
	ensure_equals("First Instr: Reference plane est distance should match", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0.8, 1e-5);

	const TLGCData &dataset = calcul.getData();
	LGCAdjustablePoint PT5 = dataset.getPoints().getObject("PT5");
	ensure_equals("PT5 Z coordinate should match", PT5.getEstimatedValue().getZ().getMetresValue(), 2329.00039, 1e-5);
	ensure_equals("PT5 H coordinate should match", PT5.getEstimatedHeightInRoot().getMetresValue(), 329.00003, 1e-5);
}

template<>
template<>
void object::test<12>()
{
	set_test_name("Testing a computation in LEP");
	readTest(TestDLEV::DLEV_LEP_1, "DLEV_LEP_1");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, false);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 1.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: Station point should have 2 unknowns", romIt->fRefPt->getNumUnkn(), 2);
	ensure_equals("First Instr: Station point X shoulf be 2015", romIt->fRefPt->getEstimatedValue().getX().getMetresValue(), 2015.0, 1e-5);
	ensure_equals("First Instr: Station point Y shoulf be 2015", romIt->fRefPt->getEstimatedValue().getY().getMetresValue(), 2015.0, 1e-5);
	ensure_equals("First Instr: Station point Z shoulf be 2330.2", romIt->fRefPt->getEstimatedValue().getZ().getMetresValue(), 2330.20022, 1e-5);
	ensure_equals("First Instr: Station point H shoulf be 330.19999", romIt->fRefPt->getEstimatedHeightInRoot().getMetresValue(), 330.19999, 1e-5);
	ensure_equals("First Instr: Station point prov H shoulf be 330.19999", romIt->fRefPt->getProvisionalHeightInRoot().getMetresValue(), 330.19999, 1e-5);
	ensure_equals("First Instr: Reference plane prov distance should match", romIt->fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 1.0, 1e-6);
	ensure_equals("First Instr: Reference plane est distance should match", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0.8, 1e-5);
	const TLGCData &dataset = calcul.getData();
	LGCAdjustablePoint PT5 = dataset.getPoints().getObject("PT5");
	ensure_equals("PT5 Z coordinate should match", PT5.getEstimatedValue().getZ().getMetresValue(), 2329.00039, 1e-5);
	ensure_equals("PT5 H coordinate should match", PT5.getEstimatedHeightInRoot().getMetresValue(), 329.00003, 1e-5);
}

template<>
template<>
void object::test<13>()
{
	set_test_name("Testing a computation in RS2K");
	readTest(TestDLEV::DLEV_RS2K_1, "DLEV_RS2K_1");
	ensure_equals("Reading Successfull ", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	TDataTree tree = projTest->getTree();

	TDataTreeIterator frameIt = tree.begin();
	auto romIt = frameIt.node->data->measurements.fLEVEL.begin();
	ensure_equals("First Instr: ihfix should be false", romIt->ihfix, false);
	ensure_equals("First Instr: hasDHOR should be true", romIt->hasDHOR, true);
	ensure_equals("First Instr: IH should be initialized to 1", romIt->instrument.instrHeight.getMetresValue(), 1.0);
	ensure_equals("First Instr: IHSE should be initialized to 0", romIt->instrument.sigmaInstrHeight.getMMetresValue(), 0.0);
	ensure_equals("First Instr: Instrument name ROM should match", romIt->instrument.ID, "LEVEL1");
	ensure_equals("First Instr: Station point should have 2 unknowns", romIt->fRefPt->getNumUnkn(), 2);
	ensure_equals("First Instr: Station point X shoulf be 2015", romIt->fRefPt->getEstimatedValue().getX().getMetresValue(), 2015.0, 1e-5);
	ensure_equals("First Instr: Station point Y shoulf be 2015", romIt->fRefPt->getEstimatedValue().getY().getMetresValue(), 2015.0, 1e-5);
	ensure_equals("First Instr: Station point Z shoulf be 2330.2", romIt->fRefPt->getEstimatedValue().getZ().getMetresValue(), 2330.20024, 1e-5);
	ensure_equals("First Instr: Station point H shoulf be 330.19999", romIt->fRefPt->getEstimatedHeightInRoot().getMetresValue(), 330.19999, 1e-5);
	ensure_equals("First Instr: Station point prov H shoulf be 330.19999", romIt->fRefPt->getProvisionalHeightInRoot().getMetresValue(), 330.19999, 1e-5);
	ensure_equals("First Instr: Reference plane prov distance should match", romIt->fMeasuredPlane->getRefPtDistProvisionalValue().getMetresValue(), 1.0, 1e-6);
	ensure_equals("First Instr: Reference plane est distance should match", romIt->fMeasuredPlane->getRefPtDistEstimatedValue().getMetresValue(), 0.8, 1e-5);

	const TLGCData &dataset = calcul.getData();
	LGCAdjustablePoint PT5 = dataset.getPoints().getObject("PT5");
	ensure_equals("PT5 Z coordinate should match", PT5.getEstimatedValue().getZ().getMetresValue(), 2329.00041, 1e-5);
	ensure_equals("PT5 H coordinate should match", PT5.getEstimatedHeightInRoot().getMetresValue(), 329.00001, 1e-5);
}

} // namespace tut
