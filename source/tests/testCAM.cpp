#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <readers/TReader.h>
#include "testCAM.h"
#include "TLGCCalculation.h"

namespace tut
{
    struct test_CAM{};
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
		TLGCData projTest;

		set_test_name("Testing UVD, 1 STATION at origin");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOT_1.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_ROOT_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<2>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 1 STATION at origin, CALA in fixed SUB");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOT_1_SUBF1.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_ROOT_1_SUBF1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<3>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 1 STATION at origin, CALA in free SUBF");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOT_1_SUBF2.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_ROOT_1_SUBF2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);
		
		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 200.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(0), 100.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<4>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 1 STATION not at origin, CALA in free SUBF");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_ROOTNOTORIG_1_SUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_ROOTNOTORIG_1_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 200.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(0), 100.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<5>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 1 STATION not at origin in free SUBF");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_SUBF_1.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_SUBF_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<6>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 1 STATION in free SUBF");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_SUBF_2.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_SUBF_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(0), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<7>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 1 STATION not at the origin in free SUBF");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_SUBFNOTORIG_1.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_SUBFNOTORIG_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(1), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<8>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 2 STATIONS in ROOT, 1 POIN to determine ");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_2ST.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_2ST_ROOT);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 100.0  , 1e-7);
	}

	template<>
	template<>
	void object::test<9>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 2 STATIONS ROOT, CALA points in SUBFRAME ");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_2STROOT_SUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_2STROOT_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 0.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 0.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 0.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 200.0  , 1e-7);

}

	template<>
	template<>
	void object::test<10>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 2 STATIONS with 1 in subf, 1 POIN to determine ");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_2ST_1STINSUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_2ST_1STINSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<11>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVD, 2 STATIONS in subf, 1 POIN to determine ");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVD_2STSUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVD_2STSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2 = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);

 	}

	//------------------------------------------ UVEC -------------------------------------------------//

	template<>
	template<>
	void object::test<12>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 1 STATION at origin");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_ROOT_1.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_ROOT_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);


		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<13>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 1 STATION at origin, CALA in fixed SUB");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_ROOT_1_SUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_ROOT_1_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(0), 100.0  , 1e-7);
 	}
	
	template<>
	template<>
	void object::test<14>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 1 STATION at origin, POIN in SUBF with RZ rotation");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_ROOTNOTORIG_1_SUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_ROOTNOTORIG_1_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(0), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<15>()
	{ 
		
		TLGCData projTest;

		set_test_name("Testing UVEC, 1 STATION at origin in subf,1 POIN");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_SUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(0).gon(), 0.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(2), 0.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<16>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 1 STATION not at origin in subf,1 POIN");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_SUBFNOTORIG_1.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_SUBFNOTORIG_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 400.0  , 1e-7);
		ensure_equals("Translation about x-axis should match",frameIt.node->data->frame.getTranslation(0), 100.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<17>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 2 STATIONS in root");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2ST_ROOT.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_2ST_ROOT);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<18>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 2 STATIONS in root, points in subf");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2STROOT_SUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_2STROOT_SUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 0.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 0.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 0.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation along z-axis should match",frameIt.node->data->frame.getTranslation(2), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<19>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 2 STATIONS 1 in root 1 in subf");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2ST_1STINSUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_2ST_1STINSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<20>()
	{ 
		TLGCData projTest;

		set_test_name("Testing UVEC, 2 STATIONS in subf");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/UVEC_2STSUBF.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestCAM::UVEC_2STSUBF);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector P2VAR = dataset.getPoints().getObject("P2VAR").getEstimatedValue();
		ensure_equals("P2VAR x coordinate should match",P2VAR.getX().getValue(), 100.0  , 1e-7);
		ensure_equals("P2VAR y coordinate should match",P2VAR.getY().getValue(), 100.0, 1e-7);
		ensure_equals("P2VAR z coordinate should match",P2VAR.getZ().getValue(), 100.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);
		frameIt++;
		ensure_equals("Rotation about z-axis should match",frameIt.node->data->frame.getAngle(2).gon(), 0.0  , 1e-7);

 	}

}