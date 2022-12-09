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
	struct test_INCL
	{
	test_INCL() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

	std::shared_ptr<TLGCData> projTest;
	TReader r;
	};
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
			if(frameIt.node->data->measurements.fINCLY.size() != 0)
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
	}
