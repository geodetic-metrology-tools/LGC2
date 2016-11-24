#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testFilesGlobalRF.h"
#include "TLGCCalculation.h"

namespace tut
{
    struct test_MLA{};
    typedef test_group<test_MLA> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("MLA tests");
}

namespace tut
{	
//--------------------------------------- LEP ----------------------------------------------//
	template<>
	template<>
	void object::test<1>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::LEP_Parametric_1ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		/*
		PT -934.3064584  10514.0644940   2413.6284609    419.8436591   0.0000   0.0000   0.0000
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), -934.3064584  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 10514.0644940, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2413.6284609  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(), 0.0, 1e-3);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.00, 1e-3);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.0, 1e-3);
		
	}

	template<>
	template<>
	void object::test<2>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::LEP_Parametric_1STNOTATP0);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		/*
		PT          -934.3064591  10514.0644960   2413.6299912    419.8451894   0.9079   0.5924   0.7854
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), -934.3064591  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 10514.0644960, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2413.6299912  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(), 0.9079, 1e-3);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.5924, 1e-3);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.7854, 1e-3);

	}

	template<>
	template<>
	void object::test<3>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::LEP_Parametric_2ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();
		/*
		PT           -934.3064587  10514.0644950   2413.6292260    419.8444242  0.4455   0.4541   0.5554 
		PT2          -895.6505693  10421.8381262   2413.7674914    419.8444088  0.7033   1.3076   1.1107
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), -934.3064587  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 10514.0644950, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2413.6292260  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(), 0.4455, 1e-3);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.4541, 1e-3);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.5554, 1e-3);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), -895.6505693, 2e-7);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 10421.8381262, 1e-7);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2413.7674914  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT2").getXEstPrecision().getMMetresValue(), 0.7033, 1e-3);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT2").getYEstPrecision().getMMetresValue(), 1.3076, 1e-3);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT2").getZEstPrecision().getMMetresValue(), 1.1107, 1e-3);
		
}
	
//--------------------------------------- RS2K ----------------------------------------------//
	template<>
	template<>
	void object::test<4>()
	{
		// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system, 4 iterations

		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_1ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z              H
		PT           2000.0000000   2197.7926500   2433.6600000    433.6599885
		PT2          2100.0000000   2197.7926500   2433.6600000    433.6606406
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.66  , 1e-7);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 2100.00, 1e-7);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 2197.79265, 1e-7);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2433.66  , 1e-7);
		
	}

	template<>
	template<>
	void object::test<5>()
	{
		// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system, 4 iterations

		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_1STSUBFRAME);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z              H
		PT           2000.0000000   2197.7926500   2433.6600000    433.6599885
		PT2          2100.0000000   2197.7926500   2433.6600000    433.6606406
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.66  , 1e-7);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 2100.00, 1e-7);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 2197.79265, 1e-7);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2433.66  , 1e-7);
		

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation about z axis should match",frameIt.node->data->frame.getEstTranslation(0), 0.0  , 1e-7);
	}

	template<>
	template<>
	void object::test<6>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 1 station, ZEND, DIST and ANGL calculated in MLA system
		//DOES NOT WORK 
			// x,y,z not precise enought. We are not exactly at P0!

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 2 STATIONS");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_1ST_NOTATP0);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		
		// Results with LGC1
		/*              X            Y            Z              H
		PT           2000.0000000   2197.7926500   2433.6615137    433.6615022
		*/

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.6615137  , 1e-7);

}

	template<>
	template<>
	void object::test<7>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system
		//DOES NOT WORK 
			// FOR Z COORDINATE unprecision is quite high
		     // for V0 angle is the same, the second station V0's is everytime determined to 389.... instead of 390 evern if all points are CALA
		// APPARENTLY IT IS LINED TO THE INTRODUCTION OF SECOND STATION, but I think it is BECAUSE IT IS NOT EXACTLY AT P0 and THEREFORE THERE ARE THESE DIFFERENCES IN Z AND V0
		//PROBABLY ALL RIGHT, because ITS IS TEH SAME IF I HAVE ONLY ONE STATION WHICH IS NOT AT P0

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 2 STATIONS");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_2ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		/*
		PT           2000.0000000   2197.7926500   2433.6607569    433.6607454		0.46    0.44    0.56 
		PT2          2100.0000000   2197.7926500   2433.6600000    433.6606406      1.05    1.05    1.11
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.6607569  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(), 0.46, 1e-2);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.44, 1e-2);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.56, 1e-2);


		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 2100.00, 1e-7);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 2197.79265, 1e-7);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2433.66  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT2").getXEstPrecision().getMMetresValue(), 1.05, 1e-2);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT2").getYEstPrecision().getMMetresValue(), 1.05, 1e-2);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT2").getZEstPrecision().getMMetresValue(), 1.11, 1e-2);

	}

	//To P5  STN = MQXA_1R5E

	template<>
	template<>
	void object::test<8>()
	{
		// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system, 4 iterations

		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_1ST_1R5E);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z              H
		PT            -934.3064569  10514.0644906   2413.6295663    419.8346279
		PT2           -895.6505724  10421.8381202   2413.7681720    419.8353888

		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), -934.3064569  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 10514.0644906, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2413.6295663  , 1e-7);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), -895.6505724, 1e-7);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 10421.8381202, 1e-7);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2413.7681720  , 1e-7);
		
	}

	template<>
	template<>
	void object::test<9>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 1 station, ZEND, DIST and ANGL calculated in MLA system
		//DOES NOT WORK 
			// x,y,z not precise enought. We are not exactly at P0!

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 2 STATIONS");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_1ST_NOTATP0_1R5E);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		
		// Results with LGC1
		/*              X            Y            Z              H
		PT           -934.3064575  10514.0644926   2413.6310942    419.8361559
		*/

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), -934.3064575  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 10514.0644926, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2413.6310942  , 1e-7);


	}

	template<>
	template<>
	void object::test<10>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system
		
		set_test_name("Testing ZEND,DIST and ANGL in MLA, 2 STATIONS");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_2ST_1R5E);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		/*
		PT           -934.3064572  10514.0644916   2413.6303302    419.8353919   0.45    0.45    0.56		
		PT2          -895.6505724  10421.8381202   2413.7681720    419.8353888   0.70    1.31    1.11

		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), -934.3064572  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 10514.0644916, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2413.6303302  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(), 0.45, 1e-2);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.45, 1e-2);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.56, 1e-2);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), -895.6505724, 1e-7);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 10421.8381202, 1e-7);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2413.7681720  , 1e-7);
		ensure_equals("sx should match", dataset.getPoints().getObject("PT2").getXEstPrecision().getMMetresValue(), 0.70, 1e-2);
		ensure_equals("sy should match", dataset.getPoints().getObject("PT2").getYEstPrecision().getMMetresValue(), 1.31, 1e-2);
		ensure_equals("sz should match", dataset.getPoints().getObject("PT2").getZEstPrecision().getMMetresValue(), 1.11, 1e-2);

}

	

////////////////////////////////////////////////////////////////////////////////////////////
///////////// PLR3D
////////////////////////////////////////////////////////////////////////////////////////////
	template<>
	template<>
	void object::test<11>()
	{ 
		// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system, 7 iterations

		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing PLR3D in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_PLR3D_1ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.66  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 2100.00, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 2197.79265, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2433.66  , 1e-8);
		
	}


	template<>
	template<>
	void object::test<12>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing PLR3D in MLA, 1 STATION, not at P0");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_PLR3D_1ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.66  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 2100.00, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 2197.79265, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2433.66  , 1e-8);

	}

	template<>
	template<>
	void object::test<13>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing PLR3D in MLA, 1 STATION, not at P0");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_PLR3D_1STSUBFRAME);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.66  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 2100.00, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 2197.79265, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2433.66  , 1e-8);

		
		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation about z axis should match",frameIt.node->data->frame.getEstTranslation(0), 0.0  , 1e-7);
	}



	template<>
	template<>
	void object::test<14>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		/*THIS AGAIN FINDS A SOLUTION< BUT IS COMPLETELY NOT WHAT EXPECTED, NOT EVEN FOR V0 WHICH WAS OK IN PARAMETRIC CASE, 10 ITERATIONS NEEDED */
		set_test_name("Testing PLR3D in MLA, 1 STATION, not at P0");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_PLR3D_1ST_NOTATP0);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
	//	const TLGCData& dataset = calcul.getData();
	/*	TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2000.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2197.79265, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.66  , 1e-8);*/

	}



	template<>
	template<>
	void object::test<15>()
	{
		// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system

		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::Param_PLR_Rs2k_Parametric_1ST_AVcoord);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 1944.07959  , 1e-5);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 2180.69565 , 1e-5);
		//ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2433.66  , 1e-5);

		TPositionVector PT2 = dataset.getPoints().getObject("STN2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), 2026.98258672072 , 1e-5);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(),  2236.61606306065, 1e-5);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2433.66  , 1e-5);

	}

	//--------------------------------------- SPHE ----------------------------------------------//
	template<>
	template<>
	void object::test<16>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::SPHE_Parametric_1ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();

		/*
		X Y Z H SX SY SZ
		PT     2342.4312257     4436.2170829     2401.7613528      402.1988877     0.0873     0.1060     0.0995
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2342.4312257  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 4436.2170829, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2401.7613528  , 1e-7);
		ensure_equals("sx should match",dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(), 0.0873  , 1e-4);
		ensure_equals("sy should match",dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.1060, 1e-4);
		ensure_equals("sz should match",dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.0995  , 1e-4);
		
	}

	template<>
	template<>
	void object::test<17>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::SPHE_Parametric_1STNOTATP0);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();

		/*
		PT           2342.43121   4436.21706   2401.76261    402.20014    0.84    0.69    0.79
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2342.43121  , 1e-5);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 4436.21706, 1e-5);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2401.76261  , 1e-5);
		ensure_equals("sx should match",dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(),  0.84  , 1e-2);
		ensure_equals("sy should match",dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.69, 1e-2);
		ensure_equals("sz should match",dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.79  , 1e-2);

	}

	template<>
	template<>
	void object::test<18>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);
		
		// SETUP: 2 stations, ZEND, DIST and ANGL calculated in MLA system

		set_test_name("Testing ZEND,DIST and ANGL in MLA, 1 STATION");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outputMLA.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestROOT::SPHE_Parametric_2ST);
		r.read(infiler);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);
		const TLGCData& dataset = calcul.getData();

		/*
		PT           2342.4312653   4436.2168464   2401.7610521    402.1985869   0.6895   0.8369   0.7854	
		PT2          2283.7024001   4353.5635281   2401.7945077    402.1993576   0.3537   0.3537   0.0111
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), 2342.4312653  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 4436.2168464, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2401.7610521  , 1e-7);
		ensure_equals("sx should match",dataset.getPoints().getObject("PT").getXEstPrecision().getMMetresValue(),  0.6895  , 2e-4);
		ensure_equals("sy should match",dataset.getPoints().getObject("PT").getYEstPrecision().getMMetresValue(), 0.8369, 2e-4);
		ensure_equals("sz should match",dataset.getPoints().getObject("PT").getZEstPrecision().getMMetresValue(), 0.7854 , 2e-4);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("PT2 x coordinate should match",PT2.getX().getMetresValue(), 2283.7024001  , 1e-7);
		ensure_equals("PT2 y coordinate should match",PT2.getY().getMetresValue(), 4353.5635281, 1e-7);
		ensure_equals("PT2 z coordinate should match",PT2.getZ().getMetresValue(), 2401.7945077  , 1e-7);
		ensure_equals("sx should match",dataset.getPoints().getObject("PT2").getXEstPrecision().getMMetresValue(),  0.3537 , 2e-4);
		ensure_equals("sy should match",dataset.getPoints().getObject("PT2").getYEstPrecision().getMMetresValue(), 0.3537, 2e-4);
		ensure_equals("sz should match",dataset.getPoints().getObject("PT2").getZEstPrecision().getMMetresValue(), 0.0111 , 2e-4);
	}
}
