#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <Behavior.h>
#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>

#include "TLGCCalculation.h"
#include "testINCL.h"
#include <Quad.h>

namespace tut
{
struct test_INCL
{
	test_INCL() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

	std::shared_ptr<TLGCData> projTest;
	TReader r;
};
typedef test_group<test_INCL> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("INCL tests");
}

namespace tut
{

//------------------------------------------ INCLY -------------------------------------------------//

template<>
template<>
void object::test<1>()
{
	set_test_name("Testing INCLY, 1 STATION in root");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_ROOT_1.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_ROOT_1);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull : No allowed INCLY in root", succesReading, false);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the same frame");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_1.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_1);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();

	TLOR2LOR RSTRToROOT(tree, "RSTR_XT03.BDB.0900", "ROOT", "RSTRTOROOT");

	TPositionVector PointS = dataset.getPoints().getObject("BEAM_XT03.BDB.0900.S").getEstimatedValue();
	RSTRToROOT.transform(PointS);

	ensure_equals("BEAM_XT03.BDB.0900.S x coordinate should match", PointS.getX().getMetresValue(), 1744.83305121, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S y coordinate should match", PointS.getY().getMetresValue(), 2369.92086569, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S z coordinate should match", PointS.getZ().getMetresValue(), 2537.43615388, 1e-7);

	TDataTreeIterator frameIt = tree.begin();
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400 - 0.007555401, 1e-7);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_2.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_2);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();

	TLOR2LOR RSTRToROOT(tree, "RSTR_XT03.BDB.0900", "ROOT", "RSTRTOROOT");

	TPositionVector PointS = dataset.getPoints().getObject("BEAM_XT03.BDB.0900.S").getEstimatedValue();
	RSTRToROOT.transform(PointS);

	ensure_equals("BEAM_XT03.BDB.0900.S x coordinate should match", PointS.getX().getMetresValue(), 1744.83305121, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S y coordinate should match", PointS.getY().getMetresValue(), 2369.92086569, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S z coordinate should match", PointS.getZ().getMetresValue(), 2537.43615388, 1e-7);

	TDataTreeIterator frameIt = tree.begin();
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400 - 0.007555401, 1e-7);
}

template<>
template<>
void object::test<4>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_3.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_3);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();

	TLOR2LOR RSTRToROOT(tree, "RSTR_XT03.BDB.0900", "ROOT", "RSTRTOROOT");

	TPositionVector PointS = dataset.getPoints().getObject("BEAM_XT03.BDB.0900.S").getEstimatedValue();
	RSTRToROOT.transform(PointS);

	ensure_equals("BEAM_XT03.BDB.0900.S x coordinate should match", PointS.getX().getMetresValue(), 1744.83305121, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S y coordinate should match", PointS.getY().getMetresValue(), 2369.92086569, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S z coordinate should match", PointS.getZ().getMetresValue(), 2537.43615388, 1e-7);

	TDataTreeIterator frameIt = tree.begin();
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400 - 0.007555401, 1e-7);
}

template<>
template<>
void object::test<5>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_4.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_4);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();

	TLOR2LOR RSTRToROOT(tree, "RSTR_XT03.BDB.0900", "ROOT", "RSTRTOROOT");

	TPositionVector PointS = dataset.getPoints().getObject("BEAM_XT03.BDB.0900.S").getEstimatedValue();
	RSTRToROOT.transform(PointS);

	ensure_equals("BEAM_XT03.BDB.0900.S x coordinate should match", PointS.getX().getMetresValue(), 1744.83305121, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S y coordinate should match", PointS.getY().getMetresValue(), 2369.92086569, 1e-7);
	ensure_equals("BEAM_XT03.BDB.0900.S z coordinate should match", PointS.getZ().getMetresValue(), 2537.43615388, 1e-7);

	TDataTreeIterator frameIt = tree.begin();
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400 - 0.007555401, 1e-7);
}

template<>
template<>
void object::test<6>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_5.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_5);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();

	TLOR2LOR RSTRToROOT(tree, "RSTR_XT03.BDB.0900", "ROOT", "RSTRTOROOT");

	TPositionVector PointS = dataset.getPoints().getObject("BEAM_XT03.BDB.0900.S").getEstimatedValue();
	RSTRToROOT.transform(PointS);

	ensure_equals("BEAM_XT03.BDB.0900.S x coordinate should match", PointS.getX().getMetresValue(), 1744.83305121, 1e-5);
	ensure_equals("BEAM_XT03.BDB.0900.S y coordinate should match", PointS.getY().getMetresValue(), 2369.92086569, 1e-5);
	ensure_equals("BEAM_XT03.BDB.0900.S z coordinate should match", PointS.getZ().getMetresValue(), 2537.43615388, 1e-5);

	TDataTreeIterator frameIt = tree.begin();
	frameIt++;
	frameIt++;
	frameIt++;
	ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400 - 0.007555401, 1e-5);
}

template<>
template<>
void object::test<7>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root and test INCLY ID reading");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_5_id.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_5_id);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();

	// Check the observation ID and the max observation ID length
	std::vector<std::string> ctrlIDincl{"test.BEAM_XT03.BDB.0900.T"};
	for (std::size_t i = 0; i < tree.size(); i++)
	{
		if (frameIt.node->data->measurements.fINCLY.size() != 0)
		{
			int maxObsIdLength = 0;
			int i = 0;
			for (auto const &data : frameIt.node->data->measurements.fINCLY.begin()->measINCLY)
			{
				ensure_equals("The observation ID is correct", data.obsID, ctrlIDincl[i]);
				if (data.obsID.size() > maxObsIdLength)
				{
					maxObsIdLength = data.obsID.size();
				}
				i++;
			}
			ensure_equals("The length of the biggest observation ID is correct", dataset.getConfig().obsIDwidth, maxObsIdLength);
		}
		frameIt++;
	}
}

template<>
template<>
void object::test<8>()
{
	set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root and test OBSXYZ ID reading");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_4_xyzID.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_4_xyzID);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	const TLGCData &dataset = calcul.getData();

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;
	frameIt++;
	frameIt++;
	frameIt++;
	// Check the observation ID
	ensure_equals("The first observation ID is correct", frameIt.node->data->measurements.fOBSXYZ.begin()->obsID, "XYZ.BEAM_XT03.BDB.0900.E");

	// Check the max observation ID length
	int maxObsIdLength = 0;
	for (auto const &data : frameIt.node->data->measurements.fOBSXYZ)
	{
		if (data.obsID.size() > maxObsIdLength)
		{
			maxObsIdLength = data.obsID.size();
		}
	}
	ensure_equals("The length of the biggest observation ID is correct", dataset.getConfig().obsIDwidth, maxObsIdLength);
}

template<>
template<>
void object::test<9>()
{
	set_test_name("Testing INCLY derivatives and the angle and scale partial derivatives with TFreeVector");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_CONTRIB.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_CONTRIB);

	bool succesReading = r.read(infiler);
	ensure_equals("Reading Successfull", succesReading, true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-9);

	TDataTree tree = projTest->getTree();
	TDataTreeIterator frameIt = tree.begin();
	frameIt++;
	ensure_equals("Translation X does not match", frameIt.node->data->frame.getEstTranslation(0).getMetresValue(), -1, 1e-6);
	ensure_equals("Translation Y does not match", frameIt.node->data->frame.getEstTranslation(1).getMetresValue(), -1, 1e-6);
	ensure_equals("Translation Z does not match", frameIt.node->data->frame.getEstTranslation(2).getMetresValue(), -1, 1e-6);
	ensure_equals("Rotation X does not match", frameIt.node->data->frame.getEstRotation(0).getGonsValue(), 0, 1e-6);
	ensure_equals("Rotation Y does not match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 0, 1e-6);
	ensure_equals("Rotation Z does not match", frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0, 1e-6);
	ensure_equals("Scale not does not match", frameIt.node->data->frame.getEstScale(), 1, 1e-6);
}

template<>
template<>
void object::test<10>()
{
	set_test_name("Testing INCLY AC, RF and PPM flags");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_6.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_6);

	ensure_equals("Reading Successfull", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-9);

	TDataTreeIterator frameIt = projTest->getTree().begin();
	frameIt++;
	ensure_equals("Rotation Y does not match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 0.2, 1e-6);

	auto romIt = frameIt.node->data->measurements.fINCLY.begin();
	TAngle obsse(4, TAngle::EUnits::kCCs);
	TAngle ppmse(3, TAngle::EUnits::kMicroRadians);
	TAngle refse(2, TAngle::EUnits::kCCs);
	TAngle acse(1, TAngle::EUnits::kCCs);
	TAngle combinedse(sqrt(pow2q(obsse.getRadiansValue() + ppmse.getRadiansValue()) + pow2q(refse.getRadiansValue()) + pow2q(acse.getRadiansValue())), TAngle::EUnits::kRadians);

	ensure_equals("AC Flag does not match", romIt->measINCLY.begin()->target.angleCorrectionValue.getGonsValue(), 0.5, 1e-7);
	ensure_equals("ACSE Flag does not match", romIt->measINCLY.begin()->target.sigmaCorrectionValue.getGonsValue(), acse.getGonsValue(), 1e-7);
	ensure_equals("RF Flag does not match", romIt->measINCLY.begin()->target.refAngleCorrectionValue.getGonsValue(), 0.3, 1e-7);
	ensure_equals("RFSE Flag does not match", romIt->measINCLY.begin()->target.refSigmaCorrectionValue.getGonsValue(), refse.getGonsValue(), 1e-7);
	ensure_equals("OBSE Flag does not match", romIt->measINCLY.begin()->target.sigmaAngl.getGonsValue(), obsse.getGonsValue(), 1e-7);
	ensure_equals("PPM Flag does not match", romIt->measINCLY.begin()->target.sigmaPpm.getGonsValue(), ppmse.getGonsValue(), 1e-7);
	ensure_equals("Combined Sigma from Flags does not match", romIt->measINCLY.begin()->target.sigmaCombinedAngle.getGonsValue(), combinedse.getGonsValue(), 1e-7);
}

template<>
template<>
void object::test<11>()
{
	set_test_name("Testing INCL AC and RF correction");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_7.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_7);

	ensure_equals("Reading Successfull", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	ensure_equals("S0 is not 0", projTest->getS0APosteriori(), 0, 1e-9);

	TDataTreeIterator frameIt = projTest->getTree().begin();
	frameIt++;
	ensure_equals("Rotation Y does not match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 0.2, 1e-6);

	auto romIt = frameIt.node->data->measurements.fINCLY.begin();
	TAngle obsse(4, TAngle::EUnits::kCCs);
	TAngle ppmse(3, TAngle::EUnits::kMicroRadians);
	TAngle refse(2, TAngle::EUnits::kCCs);
	TAngle acse(1, TAngle::EUnits::kCCs);
	TAngle combinedse(sqrt(pow2q(obsse.getRadiansValue() + ppmse.getRadiansValue()) + pow2q(refse.getRadiansValue()) + pow2q(acse.getRadiansValue())), TAngle::EUnits::kRadians);

	ensure_equals("AC from INCL does not match", romIt->measINCLY.begin()->target.angleCorrectionValue.getGonsValue(), 0.5, 1e-7);
	ensure_equals("ACSE from INCL does not match", romIt->measINCLY.begin()->target.sigmaCorrectionValue.getGonsValue(), acse.getGonsValue(), 1e-7);
	ensure_equals("RF from INCL does not match", romIt->measINCLY.begin()->target.refAngleCorrectionValue.getGonsValue(), 0.3, 1e-7);
	ensure_equals("RFSE from INCL does not match", romIt->measINCLY.begin()->target.refSigmaCorrectionValue.getGonsValue(), refse.getGonsValue(), 1e-7);
	ensure_equals("OBSE from INCL does not match", romIt->measINCLY.begin()->target.sigmaAngl.getGonsValue(), obsse.getGonsValue(), 1e-7);
	ensure_equals("PPM Flag does not match", romIt->measINCLY.begin()->target.sigmaPpm.getGonsValue(), ppmse.getGonsValue(), 1e-7);
	ensure_equals("Combined Sigma from INCL does not match", romIt->measINCLY.begin()->target.sigmaCombinedAngle.getGonsValue(), combinedse.getGonsValue(), 1e-7);
}

template<>
template<>
void object::test<12>()
{
	set_test_name("Testing INCL INSTR");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_8.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_8);

	ensure_equals("Reading Successfull", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);

	TDataTreeIterator frameIt = projTest->getTree().begin();
	frameIt++;

	auto obsIt = frameIt.node->data->measurements.fINCLY.begin()->measINCLY.begin();
	ensure_equals("INCLY instrument for this observation should be the default one", obsIt->target.ID, "I1");
	obsIt++;
	ensure_equals("INCLY instrument for this observation should be the modified one", obsIt->target.ID, "I2");
	obsIt++;
	ensure_equals("INCLY instrument for this observation should be the default one", obsIt->target.ID, "I1");
}

template<>
template<>
void object::test<13>()
{
	set_test_name("Testing Wyler Incli model");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_8.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_9);

	ensure_equals("Reading Successfull", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	std::vector<TINCLY> inclyMeas;
	for (TDataTreeIterator itTree = projTest->getTree().begin(); itTree != projTest->getTree().end(); itTree++)
	{
		for (auto &itINCLYRom : itTree.node->data->measurements.fINCLY)
		{
			for (auto &itINCLY : itINCLYRom.measINCLY)
			{
				inclyMeas.push_back(itINCLY);
			}
		}
	}
	ensure_equals("Wyler model:observed angle should be equal to predicted angle for 0 secondary rotation", inclyMeas.at(0).getAngleResidual(), 0, 1e-6);
	ensure_equals("Default model:observed angle should be equal to predicted angle for 0 secondary rotation", inclyMeas.at(1).getAngleResidual(), 0, 1e-6);

	double rx = 10 * GON2RAD;
	double ryGon = 7.145846;
	double ry = ryGon * GON2RAD;
	double wylerPredicted = asin(sin(ry) * cos(rx));
	double wylerResidual = wylerPredicted - ry;


	ensure_equals("wyler model: observed angle should not be equal to primary rotation because of nonzero secondary rotation.", inclyMeas.at(2).getAngleResidual(), wylerResidual, 1e-6);

}

template<>
template<>
void object::test<14>()
{
	set_test_name("Testing Wyler Incli model");
	projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_8.txt");
	projTest->getFileLogger().writeReportHeader("LGC output file");

	std::stringstream infiler(TestINCL::INCLY_SUBF_10);

	ensure_equals("Reading Successfull", r.read(infiler), true);

	TLGCCalculation calcul(projTest);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);
	std::vector<TINCLY> inclyMeas;
	for (TDataTreeIterator itTree = projTest->getTree().begin(); itTree != projTest->getTree().end(); itTree++)
	{
		for (auto &itINCLYRom : itTree.node->data->measurements.fINCLY)
		{
			for (auto &itINCLY : itINCLYRom.measINCLY)
			{
				inclyMeas.push_back(itINCLY);
			}
		}
	}
	ensure_equals("Inclinometer instrument type not read correctly", inclyMeas.at(0).target.type, 1);
	ensure_equals("Inclinometer instrument type not read correctly", inclyMeas.at(1).target.type, 0);
}

} // namespace tut
