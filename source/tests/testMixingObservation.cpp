#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <readers/TReader.h>
#include "testMixingObservation.h"
#include "TLGCCalculation.h"

namespace tut
{
    struct test_MixingObs{};
    typedef test_group<test_MixingObs> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Mixing Observation Types");
}

namespace tut
{	
	template<>
	template<>
	void object::test<1>()
	{ 
		TLGCData projTest;

		set_test_name("Testing ANGL_ZEND_DIST");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZEND_DIST.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(MixObs::ANGL_ZEND_DIST);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();
		//results with lgc1: 99.9347599     99.9416635     99.9270390
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 99.9347599  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 99.9416635, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 99.9270390  , 1e-7);

 	}
/*THIS TEST IS NOT WORKING*/
#if 0
	template<>
	template<>
	void object::test<2>()
	{ 
		TLGCData projTest;

		set_test_name("Testing ANGL_ZEND_DHOR");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZEND_DHOR.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(MixObs::ANGL_ZEND_DHOR);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		//results lgc1: 99.9984771    100.0006479     99.9995526
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 99.9984771  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 100.0006479, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 99.9995526  , 1e-7);
 	}
#endif
	template<>
	template<>
	void object::test<3>()
	{ 
		TLGCData projTest;

		set_test_name("Testing ANGL_ZEND_DSPT");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZEND_DSPT.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(MixObs::ANGL_ZEND_DSPT);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		//results with lgc1: 99.9347599     99.9416635     99.9270390
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 99.9347599  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 99.9416635, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 99.9270390  , 1e-7);
 	}
/*THIS TEST IS NOT WORKING*/
#if 0
	template<>
	template<>
	void object::test<4>()
	{ 
		TLGCData projTest;

		set_test_name("Testing ANGL_DHOR_DLEV");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/ANGL_DHOR_DLEV.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(MixObs::ANGL_DHOR_DLEV);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(&projTest);
		bool succesCalc = calcul.computeLSResults();
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		//results with lgc1: 100.0001787    100.0006044     99.9996700
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 100.0001787  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 100.0006044, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 99.9996700  , 1e-7);
 	}
#endif
};