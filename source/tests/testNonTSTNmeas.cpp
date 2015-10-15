#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <readers/TReader.h>
#include "testNonTSTNmeas.h"
#include "TLGCCalculation.h"
#include "refframetransformations\TXYH2CCS.h"
#include "ContributionsGenerators\TLOR2LOR.h"

namespace tut
{
    struct test_nonTSTN{};
    typedef test_group<test_nonTSTN> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Non TSTN measurements");
}

namespace tut
{	
	template<>
	template<>
	void object::test<1>()
	{ 
		TLGCData projTest;

		set_test_name("Testing DSPT measurement");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDSPT.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dspt_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PT.getX().getValue(), 0  , 1e-7);
		ensure_equals("PT y coordinate should match",PT.getY().getValue(), 0, 1e-7);
		ensure_equals("PT z coordinate should match",PT.getZ().getValue(), 100.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<2>()
	{ 
		TLGCData projTest;

		set_test_name("Testing DLEV measurement 3");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dlev_1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("LEVPLANE0");

		ensure_equals("Reference point distance should be 100", plane1.getRefPtDistEstimatedValue().getValue(), -100.0, 1e-7);

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PT.getX().getValue(), 0.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PT.getZ().getValue(), 50.0  , 1e-7);
 	}

	//I think this works OK, just not sure about the interpretation

	template<>
	template<>
	void object::test<22>()
	{ 
		TLGCData projTest;

		set_test_name("Testing DLEV measurement 3");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dlev_1_RS2K);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("LEVPLANE0");

		//ensure_equals("Reference point distance should be 100", plane1.getRefPtDistEstimatedValue().getValue(), -100.0, 1e-7);
/*
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		TPositionVector PTxyH = PT;
		TXYH2CCS::CCS2XYHg2000Machine(PTxyH);
		ensure_equals("PT x coordinate should match",PTxyH.getX().getValue(), 0.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PTxyH.getH().getValue(), 50.0  , 1e-7);*/
 	}


	template<>
	template<>
	void object::test<3>()
	{ 
		TLGCData projTest;

		set_test_name("Testing DLEV measurement RP not given");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dlev_2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("LEVPLANE0");

		ensure_equals("Reference point distance should be -14", plane1.getRefPtDistEstimatedValue().getValue(), -14.0 , 1e-7);

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT z coordinate should match",PT.getZ().getValue(), 50.0  , 1e-7);

		TPositionVector PTRP = dataset.getPoints().getObject("RefPtDLEV1").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PTRP.getX().getValue(), 40  , 1e-7);
		ensure_equals("PT y coordinate should match",PTRP.getY().getValue(), 34.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PTRP.getZ().getValue(), 114.0  , 1e-7);
 	}

	/*This test needs to be adapted in order to meet the fact that RS2K is used, just copy of dlev_2 and RS2K replaced OLOC */
#if 0
	template<>
	template<>
	void object::test<20>()
	{ 
		TLGCData projTest;

		set_test_name("Testing DLEV measurement RP not given");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dlev_3_RS2K);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("LEVPLANE0");

		//ensure_equals("Reference point distance should be 100", plane1.getRefPtDistEstimatedValue().getValue(), 14.0 , 1e-7);

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		TXYH2CCS::CCS2XYHg2000Machine(PT);
		ensure_equals("PT z coordinate should match",PT.getH().getValue(), 50.0  , 1e-7);
	/*	ensure_equals("PT z coordinate should match",PT.getZ().getValue(), 50.0  , 1e-7);*/

		TPositionVector PTRP = dataset.getPoints().getObject("RefPtDLEV1").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PTRP.getX().getValue(), 40  , 1e-7);
		ensure_equals("PT y coordinate should match",PTRP.getY().getValue(), 34.0  , 1e-7);
		/*ensure_equals("PT z coordinate should match",PTRP.getZ().getValue(), 114.0  , 1e-7);*/
 	}
#endif
	template<>
	template<>
	void object::test<4>()
	{ 
		TLGCData projTest;

		set_test_name("Testing DLEV measurement RP not given, St not in ROOT");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDLEV3.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dlev_3);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		const TAdjustablePlane& plane1 = dataset.getPlanes().getObject("LEVPLANE0");

		ensure_equals("Reference point distance should be -54", plane1.getRefPtDistEstimatedValue().getValue(), -54.0 , 1e-7);

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT z coordinate should match",PT.getZ().getValue(), 50.0  , 1e-7);

		TPositionVector PTRP = dataset.getPoints().getObject("RefPtDLEV1").getEstimatedValue();
		ensure_equals("PT x coordinate should match",PTRP.getX().getValue(), 40  , 1e-7);
		ensure_equals("PT y coordinate should match",PTRP.getY().getValue(), 34.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PTRP.getZ().getValue(), 154.0  , 1e-7);

		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation about z axis should match",frameIt.node->data->frame.getTranslation(2), 100.0  , 1e-7);
 	}


	template<>
	template<>
	void object::test<5>()
	{ 
		TLGCData projTest;

		set_test_name("DVER in OLOC");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dver_oloc);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("PT z coordinate should match",PT.getZ().getValue(), 50.0  , 1e-7);
 	}

	template<>
	template<>
	void object::test<6>()
	{ 
		TLGCData projTest;

		set_test_name("DVER in RS2K");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dver_rs2k);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();

		TXYH2CCS::CCS2XYHg2000Machine(PT);
		ensure_equals("PT x coordinate should match",PT.getX().getValue(), 0.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PT.getH().getValue(), 50.0  , 1e-7);

 	}
/*This test needs to be adapted in order to meet the fact that RS2K is used. */
#if 0
	template<>
	template<>
	void object::test<7>()
	{ 
		/*The TX value is not adjusted at all!!!!!!!!!!*/
		TLGCData projTest;

		set_test_name("DVER in RS2K + LOR");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outDVER.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(TestNonTSTN::dver_rs2k_lor_free);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		TLOR2LOR f12Root(dataset.getPoints().getObject("PT").getFrameTreePosition(), dataset.getTree().begin(),"f12Root");
		f12Root.transform(PT);

		TXYH2CCS::CCS2XYHg2000Machine(PT);
		ensure_equals("PT x coordinate should match",PT.getX().getValue(), 0.0  , 1e-7);
		ensure_equals("PT z coordinate should match",PT.getH().getValue(), 50.0  , 1e-7);


		TDataTreeIterator frameIt =  dataset.getTree().begin();
		frameIt++;
		ensure_equals("Translation about z axis should match",frameIt.node->data->frame.getTranslation(0), -10.0  , 1e-7);

 	}
#endif
}