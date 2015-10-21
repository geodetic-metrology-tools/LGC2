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
		
		stringstream infiler(TestNonTSTN::dspt_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

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
		
		stringstream infiler(TestNonTSTN::dlev_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("DLEVPLANE0");

		ensure_equals("Reference point distance should be 100", plane1.getRefPtDistEstimatedValue().getValue(), -100.0, 1e-7);

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
		
		stringstream infiler(TestNonTSTN::dlev_1_RS2K);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("DLEVPLANE0");
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		TPositionVector PTxyH = PT;
		TXYH2CCS::CCS2XYHg2000Machine(PTxyH);
		/*Compared to LGC1 result (there is not distance from the reference point)*/
		ensure_equals("(Point z coordinate - reference point distance) should match", PTxyH.getH().getMetresValue() - plane1.getRefPtDistEstimatedValue().getValue(), 100.0000001, 1e-7);

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
		
		stringstream infiler(TestNonTSTN::dlev_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("DLEVPLANE0");

		ensure_equals("Reference point distance should be -14", plane1.getRefPtDistEstimatedValue().getValue(), -14.0 , 1e-7);

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
		
		stringstream infiler(TestNonTSTN::dlev_3);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);	
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("DLEVPLANE0");

		ensure_equals("Reference point distance should be -54", plane1.getRefPtDistEstimatedValue().getValue(), -74.0 , 1e-7);

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
		
		stringstream infiler(TestDVER::OLOC_DVER_cheminement);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

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
		
		stringstream infiler(TestDVER::OLOC_DVER_cheminement_LOR);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

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
		
		stringstream infiler(TestDVER::RS2K__DVER);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

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
		
		stringstream infiler(TestDVER::RS2K_TSTN_DVER);
		
		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);
		
		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		// Results with LGC1
		/*              X            Y            Z              H
		  PT     -934.3289435    10514.1302755     2413.7946543      419.9998124  0.6305   1.0423   0.0704
		 PT2     -895.6502606    10421.8383549     2413.0328245      419.1000422  0.7538   1.4660   0.0706
		V0 = 399.9999208  => we expected here 0.02349 gon
		*/
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getMetresValue(), -934.3289435  , 1e-7);
		ensure_equals("Pt y coordinate should match",PT.getY().getMetresValue(), 10514.1302755, 1e-7);
		ensure_equals("Pt z coordinate should match",PT.getZ().getMetresValue(), 2413.7946543   , 1e-7);
		//sigma are store in m in lgc2
		ensure_equals("Pt sx should match",dataset.getPoints().getObject("PT").getXEstPrecision(), 0.00063  , 1e-5);
		ensure_equals("Pt sy should match",dataset.getPoints().getObject("PT").getYEstPrecision(), 0.00104, 1e-5);
		ensure_equals("Pt sz should match",dataset.getPoints().getObject("PT").getZEstPrecision(), 0.00007  , 1e-5);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getMetresValue(), -895.6502606, 1e-7);
		ensure_equals("Pt y coordinate should match",PT2.getY().getMetresValue(), 10421.8383549, 1e-7);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getMetresValue(), 2413.0328245  , 1e-7);
		//sigma are store in m in lgc2
		ensure_equals("PT2 sx should match",dataset.getPoints().getObject("PT2").getXEstPrecision(), 0.00075  , 1e-5);
		ensure_equals("PT2 sy should match",dataset.getPoints().getObject("PT2").getYEstPrecision(), 0.00147, 1e-5);
		ensure_equals("PT2 sz should match",dataset.getPoints().getObject("PT2").getZEstPrecision(), 0.00007  , 1e-5);
		
		TReal ST1_V0 = dataset.getAngles().getObject("ROOTV00").getEstimatedValue().gon();
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
		
		stringstream infiler(TestNonTSTN::echo_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		ensure_equals("Reference point distance should match",dataset.getPlanes().getObject("ECHOPLANE0").getRefPtDistEstimatedValue().getValue(), -120,1e-8);
		ensure_equals("Theta of the plane should match",dataset.getPlanes().getObject("ECHOPLANE0").getThetaEstimatedValue().gon(), 0,1e-8);

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
		
		stringstream infiler(TestNonTSTN::echo_1_plus_on_left_side);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		ensure_equals("Reference point distance should match",dataset.getPlanes().getObject("ECHOPLANE0").getRefPtDistEstimatedValue().getValue(), 130,1e-8);
		ensure_equals("Theta of the plane should match",dataset.getPlanes().getObject("ECHOPLANE0").getThetaEstimatedValue().gon(), 0,1e-8);

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
		
		stringstream infiler(TestNonTSTN::echo_2_line_exact);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		ensure_equals("Reference point distance should match",dataset.getPlanes().getObject("ECHOPLANE0").getRefPtDistEstimatedValue().getValue(), -70.710678118654752,1e-8);
		ensure_equals("Theta of the plane should match",dataset.getPlanes().getObject("ECHOPLANE0").getThetaEstimatedValue().gon(), 50,1e-8);

 	}
}