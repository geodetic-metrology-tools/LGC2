#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testNonTSTNmeas.h"
#include "testDVER.h"
#include "TLGCCalculation.h"
#include "TXYH2CCS.h"
#include "TLOR2LOR.h"
#include "TAdjustableHelmertTransformation.h"
#include "TTransformParameters.h"
#include <Behavior.h>

namespace tut
{
    struct test_nonTSTN{};
    typedef test_group<test_nonTSTN> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Testing non TSTN measurements");
}

namespace tut
{	
//----------------------------- DSPT --------------------------------//
	template<>
	template<>
	void object::test<1>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing DSPT measurement");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDSPT.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::dspt_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PT.getX().getMetresValue(), 0  , 1e-7);
		ensure_equals("PT y coordinate should match",PT.getY().getMetresValue(), 0, 1e-7);
		ensure_equals("PT z coordinate should match",PT.getZ().getMetresValue(), 100.0  , 1e-7);
 	}

//----------------------------- DLEV --------------------------------//
	template<>
	template<>
	void object::test<2>()
	{ 

		std::shared_ptr<TLGCData> projTest(new TLGCData);
		set_test_name("Testing DLEV measurement 3");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::dlev_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		const LGCAdjustablePlane& plane1 = *dataset.getPlanes().begin();

		ensure_equals("Reference point distance should be 100", plane1.getRefPtDistEstimatedValue().getMetresValue(), -100.0, 1e-7);

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PT.getX().getMetresValue(), 0.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PT.getZ().getMetresValue(), 50.0  , 1e-7);
 	}


	template<>
	template<>
	void object::test<3>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing DLEV measurement 3");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::dlev_1_RS2K);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

        const LGCAdjustablePlane& plane1 = *dataset.getPlanes().begin();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		TPositionVector PTxyH = PT;
		TXYH2CCS::CCS2XYHg2000Machine(PTxyH);
		/*Compared to LGC1 result (there is not distance from the reference point)*/
		ensure_equals("(Point z coordinate - reference point distance) should match", PTxyH.getH().getMetresValue() - plane1.getRefPtDistEstimatedValue().getMetresValue(), 100.0000001, 1e-7);

 	}


	template<>
	template<>
	void object::test<4>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing DLEV measurement RP not given");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::dlev_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

        const LGCAdjustablePlane& plane1 = *dataset.getPlanes().begin();

		ensure_equals("Reference point distance should be -14", plane1.getRefPtDistEstimatedValue().getMetresValue(), -14.0, 1e-7);

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT z coordinate should match",PT.getZ().getMetresValue(), 50.0  , 1e-7);

		TPositionVector PTRP = dataset.getPoints().getObject("DLEV_line16").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PTRP.getX().getMetresValue(), 40  , 1e-7);
		ensure_equals("PT y coordinate should match",PTRP.getY().getMetresValue(), 34.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PTRP.getZ().getMetresValue(), 114.0  , 1e-7);
 	}


	template<>
	template<>
	void object::test<5>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing DLEV measurement RP not given, St not in ROOT");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::dlev_3);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);	
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

        const LGCAdjustablePlane& plane1 = *dataset.getPlanes().begin();

		ensure_equals("Reference point distance should be -54", plane1.getRefPtDistEstimatedValue().getMetresValue(), -74.0, 1e-7);

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT z coordinate should match",PT.getZ().getMetresValue(), 50.0  , 1e-7);

		TPositionVector PTRP = dataset.getPoints().getObject("DLEV_line20").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PTRP.getX().getMetresValue(), 40  , 1e-7);
		ensure_equals("PT y coordinate should match",PTRP.getY().getMetresValue(), 34.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PTRP.getZ().getMetresValue(), 174.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation about z axis should match",frameIt.node->data->frame.getEstTranslation(2), 100.0  , 1e-7);
 	}

//----------------------------- DVER --------------------------------//	
//in OLOC	
	template<>
	template<>
	void object::test<7>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("DVER in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestDVER::OLOC_DVER_cheminement);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("pt0").getEstimatedValue();
		ensure_equals("PT0 z coordinate should match",PT.getZ().getMetresValue(), 10.0  , 1e-7);
		TPositionVector PT1 = dataset.getPoints().getObject("pt1").getEstimatedValue();
		ensure_equals("PT1 z coordinate should match",PT1.getZ().getMetresValue(), 50.0  , 1e-7);
		TPositionVector PT2 = dataset.getPoints().getObject("pt2").getEstimatedValue();
		ensure_equals("PT2 z coordinate should match",PT2.getZ().getMetresValue(), 20.0  , 1e-7);
		TPositionVector PT3 = dataset.getPoints().getObject("pt3").getEstimatedValue();
		ensure_equals("PT3 z coordinate should match",PT3.getZ().getMetresValue(), 0.0  , 1e-7);
		TPositionVector PT4 = dataset.getPoints().getObject("pt4").getEstimatedValue();
		ensure_equals("PT4 z coordinate should match",PT4.getZ().getMetresValue(), 10.0  , 1e-7);

 	}

	template<>
	template<>
	void object::test<8>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("DVER in OLOC + FREE LOR");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestDVER::OLOC_DVER_cheminement_LOR);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("pt0").getEstimatedValue();
		ensure_equals("PT0 z coordinate should match",PT.getZ().getMetresValue(), 10.0  , 1e-7);
		TPositionVector PT1 = dataset.getPoints().getObject("pt1").getEstimatedValue();
		ensure_equals("PT1 z coordinate should match",PT1.getZ().getMetresValue(), 50.0  , 1e-7);
		TPositionVector PT2 = dataset.getPoints().getObject("pt2").getEstimatedValue();
		ensure_equals("PT2 z coordinate should match",PT2.getZ().getMetresValue(), 20.0  , 1e-7);
		TPositionVector PT3 = dataset.getPoints().getObject("pt3").getEstimatedValue();
		ensure_equals("PT3 z coordinate should match",PT3.getZ().getMetresValue(), 0.0  , 1e-7);
		TPositionVector PT4 = dataset.getPoints().getObject("pt4").getEstimatedValue();
		ensure_equals("PT4 z coordinate should match",PT4.getZ().getMetresValue(), 10.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation about z axis should match",frameIt.node->data->frame.getEstTranslation(2), 0.0  , 2e-7);
 	}

// in RS2K

	template<>
	template<>
	void object::test<9>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("DVER in RS2K");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestDVER::RS2K__DVER);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("pt1").getEstimatedValue();

		TXYH2CCS::CCS2XYHg2000Machine(PT);
		ensure_equals("PT1 x coordinate should match",PT.getX().getMetresValue(), -978.6772806  , 1e-7);
		ensure_equals("PT1 h coordinate should match",PT.getH().getMetresValue(), 469.8346234  , 1e-7);

		TPositionVector PT2 = dataset.getPoints().getObject("pt2").getEstimatedValue();
		TXYH2CCS::CCS2XYHg2000Machine(PT2);
		ensure_equals("PT2 h coordinate should match",PT2.getH().getMetresValue(), 439.8353643  , 1e-7);

		TPositionVector PT3 = dataset.getPoints().getObject("pt3").getEstimatedValue();
		TXYH2CCS::CCS2XYHg2000Machine(PT3);
		ensure_equals("PT3 h coordinate should match",PT3.getH().getMetresValue(), 419.8369359  , 1e-7);

		TPositionVector PT4 = dataset.getPoints().getObject("pt4").getEstimatedValue();
		TXYH2CCS::CCS2XYHg2000Machine(PT4);
		ensure_equals("PT4 h coordinate should match",PT4.getH().getMetresValue(), 429.8388728  , 1e-7);


 	}

	template<>
	template<>
	void object::test<11>()
	{
		// Works, 1 station, ZEND, DIST and ANGL calculated in MLA system, 4 iterations

		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("DVER + TSTN in RS2K");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestDVER::RS2K_TSTN_DVER);
		
		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		// Results for TSTN ICSE value = 1.0, after the bug fix decribed in SUS-1880.

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT x coordinate should match", PT.getX().getMetresValue(), -934.3289437, 1e-7);
		ensure_equals("PT y coordinate should match", PT.getY().getMetresValue(), 10514.1302754, 1e-7);
		ensure_equals("PT z coordinate should match", PT.getZ().getMetresValue(), 2413.7946543, 1e-7);
		//sigma are store in m in lgc2
		ensure_equals("PT sx should match", dataset.getPoints().getObject("PT").getXEstPrecision(), 0.001244, 1e-5);
		ensure_equals("PT sy should match", dataset.getPoints().getObject("PT").getYEstPrecision(), 0.001714, 1e-5);
		ensure_equals("PT sz should match", dataset.getPoints().getObject("PT").getZEstPrecision(), 0.000071, 1e-5);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("PT2 x coordinate should match", PT2.getX().getMetresValue(), -895.6502597, 1e-7);
		ensure_equals("PT2 y coordinate should match", PT2.getY().getMetresValue(), 10421.8383546, 1e-7);
		ensure_equals("PT2 z coordinate should match", PT2.getZ().getMetresValue(), 2413.0328245, 1e-7);
		//sigma are store in m in lgc2
		ensure_equals("PT2 sx should match", dataset.getPoints().getObject("PT2").getXEstPrecision(), 0.001362, 1e-5);
		ensure_equals("PT2 sy should match", dataset.getPoints().getObject("PT2").getYEstPrecision(), 0.002205, 1e-5);
		ensure_equals("PT2 sz should match", dataset.getPoints().getObject("PT2").getZEstPrecision(), 0.000071, 1e-5);

		//TReal ST1_V0 = dataset.getAngles().begin()->getEstimatedValue().getGonsValue();
		//ensure_equals("V0 calculation should match for total station ST1", ST1_V0, 0.02349, 1e-5);
	}


//----------------------------- ECHO --------------------------------//
	template<>
	template<>
	void object::test<13>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("ECHO plane paralel with y-axis,measurements on the right side (-)");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECHO.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::echo_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		ensure_equals("Reference point distance should match", dataset.getPlanes().begin()->getRefPtDistEstimatedValue().getMetresValue(), -120, 1e-8);
        ensure_equals("Theta of the plane should match", dataset.getPlanes().begin()->getThetaEstimatedValue().getGonsValue(), 0, 1e-8);

 	}


	template<>
	template<>
	void object::test<14>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("ECHO plane paralel with y-axis, measurements on the left side (+)");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECHO.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::echo_1_plus_on_left_side);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

        ensure_equals("Reference point distance should match", dataset.getPlanes().begin()->getRefPtDistEstimatedValue().getMetresValue(), 130, 1e-8);
        ensure_equals("Theta of the plane should match", dataset.getPlanes().begin()->getThetaEstimatedValue().getGonsValue(), 0, 1e-8);

 	}

	template<>
	template<>
	void object::test<15>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("ECHO exact line measurement, meas on the right (-)");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECHO.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		std::stringstream infiler(TestNonTSTN::echo_2_line_exact);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

        ensure_equals("Reference point distance should match", dataset.getPlanes().begin()->getRefPtDistEstimatedValue().getMetresValue(), -70.710678118654752, 1e-8);
        ensure_equals("Theta of the plane should match", dataset.getPlanes().begin()->getThetaEstimatedValue().getGonsValue(), 50, 1e-8);

 	}

	//----------------------------- ORIE --------------------------------//
	
	template<>
	template<>
	void object::test<16>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ORIE measurement in SPHE");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outORIE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestNonTSTN::orie_SPHE);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

	    /* LGC1 results:
		B_1          2029.3889489   2140.4511000   2449.9999995   0.3388   0.2819           -0.3111   0.2500          
		C_1          2058.7777666   2180.9023101   2449.9999980   0.4231   0.3637           -0.7634   0.6101          
		D_1          2088.1662669   2221.3537383   2449.9999955   0.4933   0.4302           -1.5231   1.1883          
		E_1          2117.5544314   2261.8053859   2449.9999921   0.5546   0.4878           -2.6186   1.9859          
		F_1          2146.9423003   2302.2572481   2449.9999877   0.6098   0.5392           -4.0097   2.9981          
		G_1          2176.3298287   2342.7093701   2449.9999823   0.6604   0.5861           -5.7513   4.2701          
		H_1          2205.7170589   2383.1616964   2449.9999759   0.7074   0.6296           -7.7811   5.7464          
		I_1          2235.1039917   2423.6142882   2449.9999686   0.7515   0.6702          -10.1083   7.4882
		*/
	
		//We check only 2 points, which are choose with not particular meaning
		TPositionVector B1 = dataset.getPoints().getObject("B_1").getEstimatedValue();
		ensure_equals("B_1 x coordinate should match", B1.getX().getMetresValue(), 2029.3889489, 1e-7);
		ensure_equals("B_1 y coordinate should match", B1.getY().getMetresValue(), 2140.4511000, 1e-7);
		ensure_equals("B_1 z coordinate should match", B1.getZ().getMetresValue(), 2449.9999995, 1e-7);
		ensure_equals("B_1 Sx should match", dataset.getPoints().getObject("B_1").getXEstPrecision().getMMetresValue(), 0.3388, 1e-4);
		ensure_equals("B_1 Sy should match", dataset.getPoints().getObject("B_1").getYEstPrecision().getMMetresValue(), 0.2819, 1e-4);

		TPositionVector G1 = dataset.getPoints().getObject("G_1").getEstimatedValue();
		ensure_equals("G_1 x coordinate should match", G1.getX().getMetresValue(), 2176.3298287, 1e-7);
		ensure_equals("G_1 y coordinate should match", G1.getY().getMetresValue(), 2342.7093701, 1e-7);
		ensure_equals("G_1 z coordinate should match", G1.getZ().getMetresValue(), 2449.9999823, 1e-7);
		ensure_equals("G_1 Sx should match", dataset.getPoints().getObject("G_1").getXEstPrecision().getMMetresValue(), 0.6604, 1e-4);
		ensure_equals("G_1 Sy should match", dataset.getPoints().getObject("G_1").getYEstPrecision().getMMetresValue(), 0.5861, 1e-4);
	}

	template<>
	template<>
	void object::test<17>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ORIE measurement in RS2K");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outORIE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestNonTSTN::orie_RS2K);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:
		B_1          2029.3889551   2140.4510955   2450.0000370   0.3388   0.2819           -0.3049   0.2455
		C_1          2058.7777824   2180.9022986   2450.0000769   0.4231   0.3637           -0.7476   0.5986
		D_1          2088.1662990   2221.3537150   2450.0001195   0.4933   0.4302           -1.4910   1.1650
		E_1          2117.5544862   2261.8053461   2450.0001649   0.5546   0.4878           -2.5638   1.9461
		F_1          2146.9423847   2302.2571868   2450.0002130   0.6098   0.5392           -3.9253   2.9368
		G_1          2176.3299495   2342.7092823   2450.0002638   0.6604   0.5861           -5.6305   4.1823
		H_1          2205.7172227   2383.1615774   2450.0003172   0.7074   0.6296           -7.6173   5.6274
		I_1          2235.1042049   2423.6141333   2450.0003730   0.7515   0.6702           -9.8951   7.3333
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector B1 = dataset.getPoints().getObject("B_1").getEstimatedValue();
		ensure_equals("B_1 x coordinate should match", B1.getX().getMetresValue(), 2029.3889551, 1e-7);
		ensure_equals("B_1 y coordinate should match", B1.getY().getMetresValue(), 2140.4510955, 1e-7);
		ensure_equals("B_1 z coordinate should match", B1.getZ().getMetresValue(), 2450.0000370, 1e-7);
		ensure_equals("B_1 Sx should match", dataset.getPoints().getObject("B_1").getXEstPrecision().getMMetresValue(), 0.3388, 1e-4);
		ensure_equals("B_1 Sy should match", dataset.getPoints().getObject("B_1").getYEstPrecision().getMMetresValue(), 0.2819, 1e-4);

		TPositionVector G1 = dataset.getPoints().getObject("G_1").getEstimatedValue();
		ensure_equals("G_1 x coordinate should match", G1.getX().getMetresValue(), 2176.3299495, 1e-7);
		ensure_equals("G_1 y coordinate should match", G1.getY().getMetresValue(), 2342.7092823, 1e-7);
		ensure_equals("G_1 z coordinate should match", G1.getZ().getMetresValue(), 2450.0002638, 1e-7);
		ensure_equals("G_1 Sx should match", dataset.getPoints().getObject("G_1").getXEstPrecision().getMMetresValue(), 0.6604, 1e-4);
		ensure_equals("G_1 Sy should match", dataset.getPoints().getObject("G_1").getYEstPrecision().getMMetresValue(), 0.5861, 1e-4);
	}

	template<>
	template<>
	void object::test<18>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ORIE measurement in LEP");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outORIE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestNonTSTN::orie_LEP);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:
		B_1          2029.3889528   2140.4510972   2450.0000005   0.3388   0.2819           -0.3072   0.2472
		C_1          2058.7777763   2180.9023030   2450.0000022   0.4231   0.3637           -0.7537   0.6030
		D_1          2088.1662866   2221.3537240   2450.0000050   0.4933   0.4302           -1.5034   1.1740
		E_1          2117.5544649   2261.8053615   2450.0000091   0.5546   0.4878           -2.5851   1.9615
		F_1          2146.9423517   2302.2572108   2450.0000145   0.6098   0.5392           -3.9583   2.9608
		G_1          2176.3299020   2342.7093169   2450.0000210   0.6604   0.5861           -5.6780   4.2169
		H_1          2205.7171581   2383.1616244   2450.0000288   0.7074   0.6296           -7.6819   5.6744
		I_1          2235.1041207   2423.6141945   2450.0000378   0.7515   0.6702           -9.9793   7.3945
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector B1 = dataset.getPoints().getObject("B_1").getEstimatedValue();
		ensure_equals("B_1 x coordinate should match", B1.getX().getMetresValue(), 2029.3889528, 1e-7);
		ensure_equals("B_1 y coordinate should match", B1.getY().getMetresValue(), 2140.4510972, 1e-7);
		ensure_equals("B_1 z coordinate should match", B1.getZ().getMetresValue(), 2450.0000005, 1e-7);
		ensure_equals("B_1 Sx should match", dataset.getPoints().getObject("B_1").getXEstPrecision().getMMetresValue(), 0.3388, 1e-4);
		ensure_equals("B_1 Sy should match", dataset.getPoints().getObject("B_1").getYEstPrecision().getMMetresValue(), 0.2819, 1e-4);

		TPositionVector G1 = dataset.getPoints().getObject("G_1").getEstimatedValue();
		ensure_equals("G_1 x coordinate should match", G1.getX().getMetresValue(), 2176.3299020, 1e-7);
		ensure_equals("G_1 y coordinate should match", G1.getY().getMetresValue(), 2342.7093169, 1e-7);
		ensure_equals("G_1 z coordinate should match", G1.getZ().getMetresValue(), 2450.0000210, 1e-7);
		ensure_equals("G_1 Sx should match", dataset.getPoints().getObject("G_1").getXEstPrecision().getMMetresValue(), 0.6604, 1e-4);
		ensure_equals("G_1 Sy should match", dataset.getPoints().getObject("G_1").getYEstPrecision().getMMetresValue(), 0.5861, 1e-4);
	}

	template<>
	template<>
	void object::test<19>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ORIE measurement in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outORIE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestNonTSTN::orie_OLOC);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:
		B_1          2046.9848720   2117.1003842    449.9994201   0.2310   0.3753          17595.6120 -23350.4658          
		C_1          2093.9697682   2134.2007506    450.0000212   0.3126   0.4622          35191.2382 -46700.9494          
		D_1          2140.9546551   2151.3011134    450.0010136   0.3769   0.5351          52786.8651 -70051.4366          
		E_1          2187.9395177   2168.4014843    450.0023975   0.4317   0.5993          70382.4677 -93401.9157          
		F_1          2234.9243870   2185.5018366    450.0041728   0.4803   0.6572          87978.0770 -116752.4134          
		G_1          2281.9092510   2202.6022330    450.0063395   0.5245   0.7104          105573.6710 -140102.8670          
		H_1          2328.8941135   2219.7026038    450.0088976   0.5652   0.7599          123169.2735 -163453.3462          
		I_1          2375.8790097   2236.8029993    450.0118471   0.6031   0.8064          140764.9097 -186803.8007 
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector B1 = dataset.getPoints().getObject("B_1").getEstimatedValue();
		ensure_equals("B_1 x coordinate should match", B1.getX().getMetresValue(), 2046.9848720, 1e-7);
		ensure_equals("B_1 y coordinate should match", B1.getY().getMetresValue(), 2117.1003842, 1e-7);
		ensure_equals("B_1 z coordinate should match", B1.getZ().getMetresValue(), 449.9994201, 1e-7);
		ensure_equals("B_1 Sx should match", dataset.getPoints().getObject("B_1").getXEstPrecision().getMMetresValue(), 0.2310, 1e-4);
		ensure_equals("B_1 Sy should match", dataset.getPoints().getObject("B_1").getYEstPrecision().getMMetresValue(), 0.3753, 1e-4);

		TPositionVector G1 = dataset.getPoints().getObject("G_1").getEstimatedValue();
		ensure_equals("G_1 x coordinate should match", G1.getX().getMetresValue(), 2281.9092510, 1e-7);
		ensure_equals("G_1 y coordinate should match", G1.getY().getMetresValue(), 2202.6022330, 1e-7);
		ensure_equals("G_1 z coordinate should match", G1.getZ().getMetresValue(), 450.0063395, 1e-7);
		ensure_equals("G_1 Sx should match", dataset.getPoints().getObject("G_1").getXEstPrecision().getMMetresValue(), 0.5245, 1e-4);
		ensure_equals("G_1 Sy should match", dataset.getPoints().getObject("G_1").getYEstPrecision().getMMetresValue(), 0.7104, 1e-4);
	}

	template<>
	template<>
	void object::test<20>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing OBSXYZ with rotated subframe");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outOBSXYZ_subframe.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestNonTSTN::OBSXYZ_subframe);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		TPositionVector Zero = dataset.getPoints().getObject("Zero").getEstimatedValue();
		ensure_equals("Point \"Zero\" x coordinate should match", Zero.getX().getMetresValue(), 0, 1e-7);
		ensure_equals("Point \"Zero\" y coordinate should match", Zero.getY().getMetresValue(), 0, 1e-7);
		ensure_equals("Point \"Zero\" z coordinate should match", Zero.getZ().getMetresValue(), 0, 1e-7);
	}

	template<>
	template<>
	void object::test<21>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing OBSXYZ weights handling");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outOBSXYZ_weights.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestNonTSTN::OBSXYZ_weights);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData &dataset = calcul.getData();
		TDataTree tree = projTest->getTree();

		TDataTreeIterator frameIterator = dataset.getTree().begin();
		frameIterator++;
		// Check the estimated Helmert rotation Parameters
		TAdjustableHelmertTransformation frameTrafo(frameIterator.node->data.get()->frame);
		TransformParameters result = frameTrafo.getEstParam();

		// Only RZ rotation should be nonzero.
		ensure_equals("Estimated RX should be 0 gon", result.omega.getGonsValue(), 0.0, 1e-7);
		ensure_equals("Estimated RY should be 0 gon", result.phi.getGonsValue(), 0.0, 1e-7);
		ensure_equals("Estimated RZ should be 300 gon", result.kappa.getGonsValue(), 300.0, 1e-7);
	}


	template<>
	template<>
	void object::test<22>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing Sigma2Root/Sigma2Subframe using OBSXYZ");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outOBSXYZ_sigma2root.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		std::stringstream infiler(TestNonTSTN::OBSXYZ_sigma2root);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();
		LGCAdjustablePoint point = dataset.getPoints().getObject("Test2");
		// point is a cala point in subframe, so it has covariance 0 there. We test if its covariance in root is correct and equal to the obsxyz covar.
		TFreeVector sigmaInRoot = point.transformSigmaInRoot(point,projTest.get());
		ensure_equals("X Precision needs to be equal as defined via OBSXYZ", sigmaInRoot.getX(), 0.004);
		ensure_equals("Y Precision needs to be equal as defined via OBSXYZ", sigmaInRoot.getY(), 0.005);
		ensure_equals("Z Precision needs to be equal as defined via OBSXYZ", sigmaInRoot.getZ(), 0.006);

		// test transformation 200 gon rot around to subframe that flips x and y precisions signs -> precisions should remain the same	
		TFreeVector sigmaInSubframeFlippedSign = point.transformSigma(point, projTest.get(), "RotZ200");
		ensure_equals("X Precision needs to be equal as defined via OBSXYZ", sigmaInSubframeFlippedSign.getX(), 0.004);
		ensure_equals("Y Precision needs to be equal as defined via OBSXYZ", sigmaInSubframeFlippedSign.getY(), 0.005);
		ensure_equals("Z Precision needs to be equal as defined via OBSXYZ", sigmaInSubframeFlippedSign.getZ(), 0.006);
	
		// test transformation to subframe that switches x and y precisions
		TFreeVector sigmaInSubframeSwitched= point.transformSigma(point, projTest.get(), "RotZ100");
		ensure_equals("X Precision transformation to subframe problem", sigmaInSubframeSwitched.getX(), 0.005);
		ensure_equals("Y Precision transformation to subframe problem", sigmaInSubframeSwitched.getY(), 0.004);
		ensure_equals("Z Precision transformation to subframe problem", sigmaInSubframeSwitched.getZ(), 0.006);

		// test transformation to subframe where point is defined (no transformation should happen)
		TFreeVector sigmaInSubframe = point.transformSigma(point, projTest.get(), "Testframe3");
		ensure_equals("X Precision trivial transformation problem", sigmaInSubframe.getX(), point.getXEstPrecision());
		ensure_equals("Y Precision trivial transformation problem", sigmaInSubframe.getY(), point.getYEstPrecision());
		ensure_equals("Z Precision trivial transformation problem", sigmaInSubframe.getZ(), point.getZEstPrecision());

		// call transformation function for non existing frame
		try
		{
			TFreeVector sigmaSubframe = point.transformSigma(point, projTest.get(), "NonExistingFrameName");
			fail("Expect exception because destination frame does not exist.");
		}
		catch (std::exception const &excp)
		{
			std::string test = excp.what();
			ensure_equals("Wrong exception thrown", test, "Frame not found");
		}
	}

}

