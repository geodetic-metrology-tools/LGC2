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
    tut::factory tf("Test files with mixed observation types");
}

namespace tut
{	
	template<>
	template<>
	void object::test<1>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZEND_DIST");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZEND_DIST.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(MixObs::ANGL_ZEND_DIST);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();
		//P2  99.79174     99.99156     99.89158  237.61  237.45  247.38
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 99.79174  , 1e-5);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 99.99156, 1e-5);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 99.89158  , 1e-5);
		//sigma are store in m in lgc2
		ensure_equals("P2 sx should match",dataset.getPoints().getObject("P2").getXEstPrecision().getValue(), 0.23761  , 1e-5);
		ensure_equals("P2 sy should match",dataset.getPoints().getObject("P2").getYEstPrecision().getValue(), 0.23745, 1e-5);
		ensure_equals("P2 sz should match",dataset.getPoints().getObject("P2").getZEstPrecision().getValue(), 0.24738  , 1e-5);

 	}

	template<>
	template<>
	void object::test<2>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZEND_DHOR");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZEND_DHOR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(MixObs::ANGL_ZEND_DHOR);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();
		//P2     99.89996    100.09999     99.98884   74.85   74.74  130.85
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 99.89996  , 1e-5);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 100.09999, 1e-5);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 99.98884  , 1e-5);
		//sigma are store in m in lgc2
		ensure_equals("P2 sx should match",dataset.getPoints().getObject("P2").getXEstPrecision().getValue(), 0.07485  , 1e-5);
		ensure_equals("P2 sy should match",dataset.getPoints().getObject("P2").getYEstPrecision().getValue(), 0.07474, 1e-5);
		ensure_equals("P2 sz should match",dataset.getPoints().getObject("P2").getZEstPrecision().getValue(), 0.13085  , 1e-5);

 	}

	template<>
	template<>
	void object::test<3>()
	{ 
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ANGL_ZEND_DSPT");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/ANGL_ZEND_DSPT.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");
		
		stringstream infiler(MixObs::ANGL_ZEND_DSPT);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		//results with lgc1: 99.9347599     99.9416635     99.9270390
		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match",P2.getX().getValue(), 99.9347599  , 1e-7);
		ensure_equals("P2 y coordinate should match",P2.getY().getValue(), 99.9416635, 1e-7);
		ensure_equals("P2 z coordinate should match",P2.getZ().getValue(), 99.9270390  , 1e-7);
 	}
};