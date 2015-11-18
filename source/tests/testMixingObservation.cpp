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
		ensure_equals("P2 x coordinate should match",P2.getX().getMetresValue(), 99.79174  , 1e-5);
		ensure_equals("P2 y coordinate should match",P2.getY().getMetresValue(), 99.99156, 1e-5);
		ensure_equals("P2 z coordinate should match",P2.getZ().getMetresValue(), 99.89158  , 1e-5);
		//sigma are store in m in lgc2
		ensure_equals("P2 sx should match",dataset.getPoints().getObject("P2").getXEstPrecision(), 0.23761  , 1e-5);
		ensure_equals("P2 sy should match",dataset.getPoints().getObject("P2").getYEstPrecision(), 0.23745, 1e-5);
		ensure_equals("P2 sz should match",dataset.getPoints().getObject("P2").getZEstPrecision(), 0.24738  , 1e-5);

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
      ensure_equals("P2 x coordinate should match", P2.getX().getMetresValue(), 99.89996, 1e-5);
      ensure_equals("P2 y coordinate should match", P2.getY().getMetresValue(), 100.09999, 1e-5);
      ensure_equals("P2 z coordinate should match", P2.getZ().getMetresValue(), 99.98884, 1e-5);
		//sigma are store in m in lgc2
		ensure_equals("P2 sx should match",dataset.getPoints().getObject("P2").getXEstPrecision(), 0.07485  , 1e-5);
		ensure_equals("P2 sy should match",dataset.getPoints().getObject("P2").getYEstPrecision(), 0.07474, 1e-5);
		ensure_equals("P2 sz should match",dataset.getPoints().getObject("P2").getZEstPrecision(), 0.13085  , 1e-5);

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
      ensure_equals("P2 x coordinate should match", P2.getX().getMetresValue(), 99.9347599, 1e-7);
      ensure_equals("P2 y coordinate should match", P2.getY().getMetresValue(), 99.9416635, 1e-7);
      ensure_equals("P2 z coordinate should match", P2.getZ().getMetresValue(), 99.9270390, 1e-7);
 	}

	template<>
	template<>
	void object::test<4>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing PDOR");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/PDOR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(MixObs::PDOR_1CALA_GIS);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/*part of results with lgc1
		QFA____61610S   4171.0452667   6626.7459029   2400.7377227   0.0000   0.0000            0.0067   0.0029
		QDA____61710E   4144.5519355   6614.5363895   2400.7563023   0.0000   0.0000            1.0555  -1.4605
		QDA____61710S   4141.9847008   6613.3533489   2400.7579944   0.0000   0.0000            1.0908  -1.5211
		QFA____61810E   4115.4924821   6601.1429977   2400.7751578   0.0000   0.0000            2.4821  -3.9223
		QFA____61810S   4112.9252145   6599.9598266   2400.7765458   0.0000   0.0000            2.5545  -4.0734
		QDA____61910E   4086.4324861   6587.7507845   2400.7949119   0.0000   0.0000            3.5161  -5.5255
		QDA____61910S   4083.8656270   6586.5680576   2400.7967353   0.0000   0.0000            3.4670  -5.4224
		VEBZ___61632E   4169.5733559   6625.9904680   2400.5870009   0.0000   0.0000            3.5759  -6.9920
		VEBZ___61632S   4169.3916727   6625.9069384   2400.5871222   0.0000   0.0000            3.6227  -7.0116
		VEBY___61638E   4166.0207081   6624.3574698   2400.5893401   0.0000   0.0000            3.6981  -7.2202
		VEBY___61638S   4165.8389414   6624.2739851   2400.5894612   0.0000   0.0000            3.6614  -7.1949
		ZS_____61638E   4165.3438480   6624.0463687   2400.7760412   0.0000   0.0000            3.7780  -7.2713
		ZS_____61638S   4162.9632494   6622.9521406   2400.7776071   0.0000   0.0000            3.8094  -7.4294
		...
		VEBX___61778E   4124.7179673   6605.3350335   2400.6164936   0.0000   0.0000            4.7773  -9.6365
		VEBX___61778S   4124.5364987   6605.2509459   2400.6166136   0.0000   0.0000            4.8887  -9.8941
		MST____61779E   4124.1944636   6605.0926649   2400.8030896   0.0000   0.0000            5.1636 -10.2251
		MST____61779S   4122.1241999   6604.1374905   2400.8044452   0.0000   0.0000            5.0099  -9.8595
		VEBX___61793E   4121.7820967   6603.9788450   2400.6184213   0.0000   0.0000            5.1967 -10.4650
		VEBX___61793S   4121.6005110   6603.8950705   2400.6185410   0.0000   0.0000            5.2010 -10.4195
		MST____61794E   4121.2580862   6603.7376905   2400.8050163   0.0000   0.0000            5.0762  -9.8395
		...
		VEBX___61856S   4102.8205039   6595.2211573   2400.6309254   0.0000   0.0000            6.2739 -11.9327
		MSE____61857E   4102.4785083   6595.0627844   2400.8173901   0.0000   0.0000            6.2483 -11.6356
		MSE____61857S   4100.4110772   6594.1014474   2400.8187605   0.0000   0.0000            6.3372 -11.8126
		VEBX___61871E   4100.0689967   6593.9427082   2400.6327352   0.0000   0.0000            6.0167 -11.3718
		VEBX___61871S   4099.8874740   6593.8587273   2400.6328643   0.0000   0.0000            5.8040 -10.9127
		MSE____61872E   4099.5462338   6593.6987500   2400.8193397   0.0000   0.0000            6.3338 -11.7800
		MSE____61872S   4097.4798676   6592.7350845   2400.8207003   0.0000   0.0000            6.2476 -11.5955
		VEBW___61876E   4097.1387668   6592.5741879   2400.6346760   0.0000   0.0000            6.1268 -11.6021
		VEBW___61876S   4096.9580663   6592.4884558   2400.6347959   0.0000   0.0000            6.3163 -11.9942
		*/
		TAdjustablePoint QFA = dataset.getPoints().getObject("QFA____61610S");
		ensure_equals("QFA x coordinate should match", QFA.getEstimatedValue().getX().getMetresValue(), 4171.0452667, 1e-7);
		ensure_equals("QFA y coordinate should match", QFA.getEstimatedValue().getY().getMetresValue(), 6626.7459029, 1e-7);
		ensure_equals("QFA z coordinate should match", QFA.getEstimatedValue().getZ().getMetresValue(), 2400.7377227, 1e-7);
		ensure_equals("QFA sx coordinate should match", QFA.getXEstPrecision().getMMetresValue(), 99.9347599, 1e-4);
		ensure_equals("QFA sy coordinate should match", QFA.getYEstPrecision().getMMetresValue(), 99.9416635, 1e-4);
		ensure_equals("QFA sz coordinate should match", QFA.getZEstPrecision().getMMetresValue(), 99.9270390, 1e-4);

		TAdjustablePoint MST = dataset.getPoints().getObject("MST____61779S");
		ensure_equals("MST x coordinate should match", MST.getEstimatedValue().getX().getMetresValue(), 4122.1241999, 1e-7);
		ensure_equals("MST y coordinate should match", MST.getEstimatedValue().getY().getMetresValue(), 6604.1374905, 1e-7);
		ensure_equals("MST z coordinate should match", MST.getEstimatedValue().getZ().getMetresValue(), 2400.8044452, 1e-7);
		ensure_equals("MST sx coordinate should match", MST.getXEstPrecision().getMMetresValue(), 99.9347599, 1e-4);
		ensure_equals("MST sy coordinate should match", MST.getYEstPrecision().getMMetresValue(), 99.9416635, 1e-4);
		ensure_equals("MST sz coordinate should match", MST.getZEstPrecision().getMMetresValue(), 99.9270390, 1e-4);

		TAdjustablePoint VEBW = dataset.getPoints().getObject("VEBW___61876E");
		ensure_equals("VEBW x coordinate should match", VEBW.getEstimatedValue().getX().getMetresValue(), 4097.1387668, 1e-7);
		ensure_equals("VEBW y coordinate should match", VEBW.getEstimatedValue().getY().getMetresValue(), 6592.5741879, 1e-7);
		ensure_equals("VEBW z coordinate should match", VEBW.getEstimatedValue().getZ().getMetresValue(), 2400.6346760, 1e-7);
		ensure_equals("VEBW sx coordinate should match", VEBW.getXEstPrecision().getMMetresValue(), 99.9347599, 1e-4);
		ensure_equals("VEBW sy coordinate should match", VEBW.getYEstPrecision().getMMetresValue(), 99.9416635, 1e-4);
		ensure_equals("VEBW sz coordinate should match", VEBW.getZEstPrecision().getMMetresValue(), 99.9270390, 1e-4);
	}
};