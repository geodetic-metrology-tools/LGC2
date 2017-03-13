#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testMixingObservation.h"
#include "TLGCCalculation.h"
#include <Behavior.h>

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
		stringstream cpinfiler(MixObs::ANGL_ZEND_DIST);

		bool succesReading = r.read(infiler, cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

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
		stringstream cpinfiler(MixObs::ANGL_ZEND_DHOR);

		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

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
		stringstream cpinfiler(MixObs::ANGL_ZEND_DSPT);

		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

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
		stringstream cpinfiler(MixObs::PDOR_1CALA_GIS);

		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		/*part of results with lgc1
		QFA____61610S   4176.4377368   6627.9083784   2400.7350565   0.2732   0.1101          5392.4768 1162.4784          
		QDA____61710E   4205.6083632   6627.6994929   2400.7261137   1.4187   0.4154          61057.4832 13161.6429          
		QDA____61710S   4208.4349988   6627.6791832   2400.7251390   1.4448   0.4464          66451.3888 14324.3132          
		...
		MST____61779E   4228.0494246   6627.5950878   2400.7516599   1.7305   0.7786          103860.1246 22492.1978
		MST____61779S   4230.3293623   6627.5797501   2400.7508631   1.7518   0.8167          108210.1723 23432.4001
		VEBX___61793E   4230.7062787   6627.5777204   2400.5644826   1.7779   0.8231          108929.3787 23588.4104
		...
		MSE____61872S   4257.4832675   6627.4261818   2400.7414491   2.3148   1.2797          160009.6475 34679.5018
		VEBW___61876E   4257.8602335   6627.4266159   2400.5550652   2.3346   1.2862          160727.5935 34840.8259
		VEBW___61876S   4258.0602385   6627.4274141   2400.5549937   2.3505   1.2896          161108.4885 34926.9641
		*/

		TAdjustablePoint QFA = dataset.getPoints().getObject("QFA____61610S");
		ensure_equals("QFA x coordinate should match", QFA.getEstimatedValue().getX().getMetresValue(), 4176.4377368, 1e-7);
		ensure_equals("QFA y coordinate should match", QFA.getEstimatedValue().getY().getMetresValue(), 6627.9083784, 1e-7);
		ensure_equals("QFA z coordinate should match", QFA.getEstimatedValue().getZ().getMetresValue(), 2400.7350565, 1e-7);
		ensure_equals("QFA sx coordinate should match", QFA.getXEstPrecision().getMMetresValue(), 0.2732, 1e-4);
		ensure_equals("QFA sy coordinate should match", QFA.getYEstPrecision().getMMetresValue(), 0.1101, 1e-4);

		TAdjustablePoint MST = dataset.getPoints().getObject("MST____61779S");
		ensure_equals("MST x coordinate should match", MST.getEstimatedValue().getX().getMetresValue(), 4230.3293623, 1e-7);
		ensure_equals("MST y coordinate should match", MST.getEstimatedValue().getY().getMetresValue(), 6627.5797501, 1e-7);
		ensure_equals("MST z coordinate should match", MST.getEstimatedValue().getZ().getMetresValue(), 2400.7508631, 1e-7);
		ensure_equals("MST sx coordinate should match", MST.getXEstPrecision().getMMetresValue(), 1.7518, 1e-4);
		ensure_equals("MST sy coordinate should match", MST.getYEstPrecision().getMMetresValue(), 0.8167, 1e-4);

		TAdjustablePoint VEBW = dataset.getPoints().getObject("VEBW___61876E");
		ensure_equals("VEBW x coordinate should match", VEBW.getEstimatedValue().getX().getMetresValue(), 4257.8602335, 1e-7);
		ensure_equals("VEBW y coordinate should match", VEBW.getEstimatedValue().getY().getMetresValue(), 6627.4266159, 1e-7);
		ensure_equals("VEBW z coordinate should match", VEBW.getEstimatedValue().getZ().getMetresValue(), 2400.5550652, 1e-7);
		ensure_equals("VEBW sx coordinate should match", VEBW.getXEstPrecision().getMMetresValue(), 2.3346, 1e-4);
		ensure_equals("VEBW sy coordinate should match", VEBW.getYEstPrecision().getMMetresValue(), 1.2862, 1e-4);
	}

	template<>
	template<>
	void object::test<5>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing PDOR");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/PDOR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(MixObs::PDOR_1CALA_0);
		stringstream cpinfiler(MixObs::PDOR_1CALA_0);

		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		/*part of results with lgc1
		QFA____61610S   4171.0442076   6626.7482039   2400.7377214   0.2522   0.1521           -1.0524   2.3039
		QDA____61710E   4144.5399430   6614.5624426   2400.7562879   1.3016   0.7008          -10.9370  24.5926
		QDA____61710S   4141.9716490   6613.3817036   2400.7579787   1.3269   0.7253          -11.9610  26.8336
		...
		VEBX___61778E   4124.6977353   6605.3788684   2400.6164693   1.5627   0.9889          -15.4547  34.1984
		VEBX___61778S   4124.5161914   6605.2949434   2400.6165892   1.5824   0.9976          -15.4186  34.1034
		MST____61779E   4124.1740144   6605.1369692   2400.8030651   1.6068   1.0096          -15.2856  34.0792
		MST____61779S   4122.1028954   6604.1836508   2400.8044196   1.6290   1.0403          -16.2946  36.3008
		...
		VEBX___61871S   4099.8569652   6593.9248235   2400.6328278   2.1336   1.4708          -24.7048  55.1835
		MSE____61872E   4099.5155817   6593.7651522   2400.8193030   2.1519   1.4808          -24.3183  54.6222
		MSE____61872S   4097.4483525   6592.8033393   2400.8206625   2.1701   1.5122          -25.2675  56.6593
		VEBW___61876E   4097.1071077   6592.6427484   2400.6346380   2.1882   1.5221          -25.5323  56.9584
		VEBW___61876S   4096.9263304   6592.5571783   2400.6347579   2.2025   1.5287          -25.4196  56.7283
		*/
		TAdjustablePoint QFA = dataset.getPoints().getObject("QFA____61610S");
		ensure_equals("QFA x coordinate should match", QFA.getEstimatedValue().getX().getMetresValue(), 4171.0442076, 1e-7);
		ensure_equals("QFA y coordinate should match", QFA.getEstimatedValue().getY().getMetresValue(), 6626.7482039, 1e-7);
		ensure_equals("QFA z coordinate should match", QFA.getEstimatedValue().getZ().getMetresValue(), 2400.7377214, 1e-7);
		ensure_equals("QFA sx coordinate should match", QFA.getXEstPrecision().getMMetresValue(), 0.2522, 1e-4);
		ensure_equals("QFA sy coordinate should match", QFA.getYEstPrecision().getMMetresValue(), 0.1521, 1e-4);


		TAdjustablePoint MST = dataset.getPoints().getObject("MST____61779S");
		ensure_equals("MST x coordinate should match", MST.getEstimatedValue().getX().getMetresValue(), 4122.1028954, 1e-7);
		ensure_equals("MST y coordinate should match", MST.getEstimatedValue().getY().getMetresValue(), 6604.1836508, 1e-7);
		ensure_equals("MST z coordinate should match", MST.getEstimatedValue().getZ().getMetresValue(), 2400.8044196, 1e-7);
		ensure_equals("MST sx coordinate should match", MST.getXEstPrecision().getMMetresValue(), 1.6290, 1e-4);
		ensure_equals("MST sy coordinate should match", MST.getYEstPrecision().getMMetresValue(), 1.0403, 1e-4);

		TAdjustablePoint VEBW = dataset.getPoints().getObject("VEBW___61876E");
		ensure_equals("VEBW x coordinate should match", VEBW.getEstimatedValue().getX().getMetresValue(), 4097.1071077, 1e-7);
		ensure_equals("VEBW y coordinate should match", VEBW.getEstimatedValue().getY().getMetresValue(), 6592.6427484, 1e-7);
		ensure_equals("VEBW z coordinate should match", VEBW.getEstimatedValue().getZ().getMetresValue(), 2400.6346380, 1e-7);
		ensure_equals("VEBW sx coordinate should match", VEBW.getXEstPrecision().getMMetresValue(), 2.1882, 1e-4);
		ensure_equals("VEBW sy coordinate should match", VEBW.getYEstPrecision().getMMetresValue(), 1.5221, 1e-4);
	}

	template<>
	template<>
	void object::test<6>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing PDOR");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/PDOR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(MixObs::PDOR_2CALA);
		stringstream cpinfiler(MixObs::PDOR_2CALA);

		bool succesReading = r.read(infiler,cpinfiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		const TLGCData& dataset = calcul.getData();

		/*part of results with lgc1
		QFA____61610S   4171.0419520   6626.7526980   2400.7377190   1.4802   0.7192           -3.3080   6.7980
		QDA____61710E   4144.5478999   6614.5451520   2400.7562974   6.1860   2.9359           -2.9801   7.3020
		QDA____61710S   4141.9806353   6613.3623246   2400.7579894   6.2254   2.8870           -2.9747   7.4546
		...
		MST____61779E   4124.1894389   6605.1030340   2400.8030840   6.3999   2.9892            0.1389   0.1440
		MST____61779S   4122.1190879   6604.1480000   2400.8044394   6.3832   2.9752           -0.1021   0.6500
		VEBX___61793E   4121.7769551   6603.9893703   2400.6184155   6.3556   2.9617            0.0551   0.0603
		..
		MSE____61872E   4099.5400014   6593.7106624   2400.8193334   2.6253   1.2613            0.1014   0.1324
		MSE____61872S   4097.4735554   6592.7471312   2400.8206938   2.2018   1.0722           -0.0646   0.4512
		VEBW___61876E   4097.1324292   6592.5862509   2400.6346696   1.4892   0.7709           -0.2108   0.4609
		*/
		TAdjustablePoint QFA = dataset.getPoints().getObject("QFA____61610S");
		ensure_equals("QFA x coordinate should match", QFA.getEstimatedValue().getX().getMetresValue(), 4171.0419520, 1e-7);
		ensure_equals("QFA y coordinate should match", QFA.getEstimatedValue().getY().getMetresValue(), 6626.7526980, 1e-7);
		ensure_equals("QFA z coordinate should match", QFA.getEstimatedValue().getZ().getMetresValue(), 2400.7377190, 1e-7);
		ensure_equals("QFA sx coordinate should match", QFA.getXEstPrecision().getMMetresValue(), 1.4802, 1e-4);
		ensure_equals("QFA sy coordinate should match", QFA.getYEstPrecision().getMMetresValue(), 0.7192, 1e-4);

		TAdjustablePoint MST = dataset.getPoints().getObject("MST____61779S");
		ensure_equals("MST x coordinate should match", MST.getEstimatedValue().getX().getMetresValue(), 4122.1190879, 1e-7);
		ensure_equals("MST y coordinate should match", MST.getEstimatedValue().getY().getMetresValue(), 6604.1480000, 1e-7);
		ensure_equals("MST z coordinate should match", MST.getEstimatedValue().getZ().getMetresValue(), 2400.8044394, 1e-7);
		ensure_equals("MST sx coordinate should match", MST.getXEstPrecision().getMMetresValue(), 6.3832, 1e-4);
		ensure_equals("MST sy coordinate should match", MST.getYEstPrecision().getMMetresValue(), 2.9752, 1e-4);

		TAdjustablePoint VEBW = dataset.getPoints().getObject("VEBW___61876E");
		ensure_equals("VEBW x coordinate should match", VEBW.getEstimatedValue().getX().getMetresValue(), 4097.1324292, 1e-7);
		ensure_equals("VEBW y coordinate should match", VEBW.getEstimatedValue().getY().getMetresValue(), 6592.5862509, 1e-7);
		ensure_equals("VEBW z coordinate should match", VEBW.getEstimatedValue().getZ().getMetresValue(), 2400.6346696, 1e-7);
		ensure_equals("VEBW sx coordinate should match", VEBW.getXEstPrecision().getMMetresValue(), 1.4892, 1e-4);
		ensure_equals("VEBW sy coordinate should match", VEBW.getYEstPrecision().getMMetresValue(), 0.7709, 1e-4);
	}
};
