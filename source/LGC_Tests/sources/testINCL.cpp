#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>
#include "testINCL.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

namespace tut
{
	struct test_INCL {};
	typedef test_group<test_INCL> factory;
	typedef factory::object object;
}

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
		std::shared_ptr<TLGCData> projTest(new TLGCData); (new TLGCData);

		set_test_name("Testing INCLY, 1 STATION in root");
		TReader r(projTest);
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
		std::shared_ptr<TLGCData> projTest(new TLGCData); (new TLGCData);

		set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the same frame");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestINCL::INCLY_SUBF_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

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
		ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400-0.007555401, 1e-7);

	}

	template<>
	template<>
	void object::test<3>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData); (new TLGCData);

		set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_2.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestINCL::INCLY_SUBF_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

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
		ensure_equals("Rotation around y-axis should match", frameIt.node->data->frame.getEstRotation(1).getGonsValue(), 400-0.007555401, 1e-7);

	}

	template<>
	template<>
	void object::test<4>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData); (new TLGCData);

		set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_3.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestINCL::INCLY_SUBF_3);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

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
		std::shared_ptr<TLGCData> projTest(new TLGCData); (new TLGCData);

		set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_4.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestINCL::INCLY_SUBF_4);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

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
		std::shared_ptr<TLGCData> projTest(new TLGCData); (new TLGCData);

		set_test_name("Testing INCLY, 1 STATION in SBF, CALA declared in the root");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/INCLY_SUBF_5.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestINCL::INCLY_SUBF_5);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading Successfull", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

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
}

