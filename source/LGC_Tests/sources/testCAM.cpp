#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testCAM.h"
#include "TLGCCalculation.h"
#include <Behavior.h>
#include <cmath>

namespace tut
{
    struct test_CAM
	{
		test_CAM() : projTest(std::make_shared<TLGCData>()), r(projTest) {}

		std::shared_ptr<TLGCData> projTest;
		TReader r;
	};
    typedef test_group<test_CAM> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("CAM tests");
}

namespace tut
{	

//------------------------------------------ UVD -------------------------------------------------//
	
	template<>
	template<>
	void object::test<1>()
	{
		set_test_name("Testing UVD, 1 STATION at origin");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOT_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_ROOT_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<2>()
	{ 
		set_test_name("Testing UVD, 1 STATION at origin, CALA in fixed SUB");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOT_1_SUBF1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_ROOT_1_SUBF1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<3>()
	{ 
		set_test_name("Testing UVD, 1 STATION at origin, CALA in free SUBF");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOT_1_SUBF2.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_ROOT_1_SUBF2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);
		
		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 200.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(0), 100.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<4>()
	{ 
		set_test_name("Testing UVD, 1 STATION not at origin, CALA in free SUBF");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOTNOTORIG_1_SUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_ROOTNOTORIG_1_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 200.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(0), 100.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<5>()
	{ 
		set_test_name("Testing UVD, 1 STATION not at origin in free SUBF");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_SUBF_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_SUBF_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<6>()
	{ 
		set_test_name("Testing UVD, 1 STATION in free SUBF");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_SUBF_2.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_SUBF_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(0), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<7>()
	{ 
		set_test_name("Testing UVD, 1 STATION not at the origin in free SUBF");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_SUBFNOTORIG_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_SUBFNOTORIG_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(1), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<8>()
	{ 
		set_test_name("Testing UVD, 2 STATIONS in ROOT, 1 POIN to determine ");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_2ST.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_2ST_ROOT);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getMetresValue(), 100.0  , 1e-7);
	}

	template<>
	template<>
	void object::test<9>()
	{ 
		set_test_name("Testing UVD, 2 STATIONS ROOT, CALA points in SUBFRAME ");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_2STROOT_SUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_2STROOT_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getMetresValue(), 0.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getMetresValue(), 0.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getMetresValue(), 0.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 200.0  , 1e-7);

}

	template<>
	template<>
	void object::test<10>()
	{ 
		set_test_name("Testing UVD, 2 STATIONS with 1 in subf, 1 POIN to determine ");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_2ST_1STINSUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_2ST_1STINSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<11>()
	{ 
		set_test_name("Testing UVD, 2 STATIONS in subf, 1 POIN to determine ");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_2STSUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVD_2STSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<12>()
	{
		set_test_name("Testing UVD, 2 STATIONS in subf, 1 POIN to determine and read observation ID");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVD_2STSUBF_id.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestCAM::UVD_2STSUBF_id);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData &dataset = calcul.getData();
		TDataTreeIterator frameIt = projTest->getTree().begin();
		int maxObsIdLength = 0;
		int i = 0;
		// Check the observation ID and the max observation ID length
		frameIt++;
		std::vector<std::string> ctrlIDf1{"BCAM1_P1", "BCAM1_P3", "BCAM1_P4", "BCAM1_P2VAR"};
		for (auto const &data : frameIt.node->data->measurements.fCAM.begin()->measUVD)
		{
			ensure_equals("The observation ID is correct", data.obsID, ctrlIDf1[i]);
			i++;
			if (data.obsID.size() > maxObsIdLength)
			{
				maxObsIdLength = data.obsID.size();
			}
		}
		frameIt++;
		i = 0;
		std::vector<std::string> ctrlIDf2{"BCAM1_STN2_P2VAR", "BCAM1_STN2_P1", "BCAM1_STN2_P4", "BCAM1_STN2_P3"};
		for (auto const &data : frameIt.node->data->measurements.fCAM.begin()->measUVD)
		{
			ensure_equals("The observation ID is correct", data.obsID, ctrlIDf2[i]);
			i++;
			if (data.obsID.size() > maxObsIdLength)
			{
				maxObsIdLength = data.obsID.size();
			}
		}
		ensure_equals("The length of the biggest observation ID is correct", dataset.getConfig().obsIDwidth, maxObsIdLength);
		
	}

	//------------------------------------------ UVEC -------------------------------------------------//

	template<>
	template<>
	void object::test<13>()
	{ 
		set_test_name("Testing UVEC, 1 STATION at origin");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_ROOT_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_ROOT_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);


		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<14>()
	{ 
		set_test_name("Testing UVEC, 1 STATION at origin, CALA in fixed SUB");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_ROOT_1_SUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_ROOT_1_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(0), 100.0  , 1e-7);
 	}
	
	template<>
	template<>
	void object::test<15>()
	{ 
		set_test_name("Testing UVEC, 1 STATION at origin, POIN in SUBF with RZ rotation");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_ROOTNOTORIG_1_SUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_ROOTNOTORIG_1_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(0), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<16>()
	{ 
		set_test_name("Testing UVEC, 1 STATION at origin in subf,1 POIN");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_SUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(0).getGonsValue(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(2), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<17>()
	{ 
		set_test_name("Testing UVEC, 1 STATION not at origin in subf,1 POIN");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_SUBFNOTORIG_1.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_SUBFNOTORIG_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match", std::fmod(frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 400.0), 0.0, 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getEstTranslation(0), 100.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<18>()
	{ 
		set_test_name("Testing UVEC, 2 STATIONS in root");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2ST_ROOT.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_2ST_ROOT);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);
 	}

#if 0
	template<>
	template<>
	void object::test<19>()
	{ 
		set_test_name("Testing UVEC, 2 STATIONS in root, points in subf");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2STROOT_SUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_2STROOT_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 0.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 0.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 0.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation along z-axis should match",frameIt.node->data->frame.getEstTranslation(2), 100.0  , 1e-7);
 	}
#endif

	template<>
	template<>
	void object::test<20>()
	{ 
		set_test_name("Testing UVEC, 2 STATIONS 1 in root 1 in subf");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2ST_1STINSUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_2ST_1STINSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<21>()
	{ 
		set_test_name("Testing UVEC, 2 STATIONS in subf");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2STSUBF.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestCAM::UVEC_2STSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getMetresValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getMetresValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getMetresValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getEstRotation(2).getGonsValue(), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<22>()
	{
		set_test_name("Testing UVEC, 2 STATIONS in subf and read observation id");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2STSUBF_ID.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestCAM::UVEC_2STSUBF_id);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData &dataset = calcul.getData();
		TDataTreeIterator frameIt = projTest->getTree().begin();
		int maxObsIdLength = 0;
		int i = 0;

		// Check the observation ID and the max observation ID length
		frameIt++;
		std::vector<std::string> ctrlIDf1{"STN_BCAM1_P1", "STN_BCAM1_P3", "STN_BCAM1_P4", "STN_BCAM1_P2VAR"};
		for (auto const &data : frameIt.node->data->measurements.fCAM.begin()->measUVEC)
		{
			ensure_equals("The observation ID is correct", data.obsID, ctrlIDf1[i]);
			i++;
			if (data.obsID.size() > maxObsIdLength)
			{
				maxObsIdLength = data.obsID.size();
			}
		}
		frameIt++;
		std::vector<std::string> ctrlIDf2{"STN2_BCAM1_P2VAR", "STN2_BCAM1_P1", "STN2_BCAM1_P4", "STN2_BCAM1_P3"};
		i = 0;
		for (auto const &data : frameIt.node->data->measurements.fCAM.begin()->measUVEC)
		{
			ensure_equals("The observation ID is correct", data.obsID, ctrlIDf2[i]);
			i++;
			if (data.obsID.size() > maxObsIdLength)
			{
				maxObsIdLength = data.obsID.size();
			}
		}
		ensure_equals("The length of the biggest observation ID is correct", dataset.getConfig().obsIDwidth, maxObsIdLength);
	}

	template<>
	template<>
	void object::test<23>()
	{
		set_test_name("Testing CAM TRGT keywords");
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/CAM_READ.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestCAM::CAM_TRGT_READ);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);

		// testing the first UVD ROM
		auto obsUVDIt = projTest->getTree().begin().node->data->measurements.fCAM.begin()->measUVD.begin();
		ensure_equals("observation TRGT should be one defined on the *UVD line", obsUVDIt->target.ID, "T2");
		obsUVDIt++;
		ensure_equals("observation TRGT should be one defined on the measurement line", obsUVDIt->target.ID, "T1");
		obsUVDIt++;
		ensure_equals("observation TRGT should be one defined on the *UVD line", obsUVDIt->target.ID, "T2");

		// testing the second UVD ROM
		obsUVDIt++;
		ensure_equals("observation TRGT should be one defined on the *CAM line", obsUVDIt->target.ID, "T1");
		obsUVDIt++;
		ensure_equals("observation TRGT should be one defined on the measurement line", obsUVDIt->target.ID, "T2");
		obsUVDIt++;
		ensure_equals("observation TRGT should be one defined on the *CAM line", obsUVDIt->target.ID, "T1");

		// testing the first UVEC ROM
		auto obsUVECIt = projTest->getTree().begin().node->data->measurements.fCAM.begin()->measUVEC.begin();
		ensure_equals("observation TRGT should be one defined on the *UVEC line", obsUVECIt->target.ID, "T2");
		obsUVECIt++;
		ensure_equals("observation TRGT should be one defined on the measurement line", obsUVECIt->target.ID, "T1");
		obsUVECIt++;
		ensure_equals("observation TRGT should be one defined on the *UVEC line", obsUVECIt->target.ID, "T2");

		// testing the second UVEC ROM
		obsUVECIt++;
		ensure_equals("observation TRGT should be one defined on the *CAM line", obsUVECIt->target.ID, "T1");
		obsUVECIt++;
		ensure_equals("observation TRGT should be one defined on the measurement line", obsUVECIt->target.ID, "T2");
		obsUVECIt++;
		ensure_equals("observation TRGT should be one defined on the *CAM line", obsUVECIt->target.ID, "T1");
	}
	} // namespace tut
