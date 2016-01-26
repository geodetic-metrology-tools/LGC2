#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include "Utils.h"
#include <TLGCData.h>
#include <TReader.h>
#include "tests_with_ScaleInstr.h"
#include "TLGCCalculation.h"
#include <vector>
#include <ctime>

namespace tut
{
	struct test_scale{};
	typedef test_group<test_scale> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Test with scale instrument");
}

namespace tut
{
	//----------------------------- ECTH --------------------------------//
	template<>
	template<>
	void object::test<1>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECTH measurement in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECTH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecth_OLOC);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY

		AS1          3024.9999995   3000.3000000    451.0000000   0.0095   0.1348           -0.0005   0.0000
		BS1          3050.0000171   3000.4000200    452.0000000   0.0094   0.1421            0.0171   0.0200
		CS1          3100.0000003   3000.3000000    453.0000000   0.0094   0.1682            0.0003   0.0000
		DS1          3024.9999995   2999.7000000    451.0000000   0.0095   0.1348           -0.0005 -600.0000
		ES1          3049.9999972   2999.5999900    452.0000000   0.0094   0.1421           -0.0028 -800.0100
		FS1          3100.0000103   2999.7000000    453.0000000   0.0094   0.1682            0.0103 -600.0000
		AS2          2985.5480653   2979.5982278    451.0000000   0.1102   0.0783           -0.0087  -0.0122
		BS2          2970.9343975   2959.3140075    452.0000000   0.1158   0.0830            0.0535  -0.0285
		CS2          2941.4641709   2918.9219526    453.0000000   0.1365   0.0988           -0.0091  -0.0124
		DS2          2985.0626398   2979.9509290    449.0000000   0.1083   0.0809          -485.4342 352.6890
		ES2          2970.3165362   2959.8247377    447.0000000   0.1144   0.0848          -617.8078 510.7017
		FS2          2940.9787754   2919.2746440    453.0000000   0.1359   0.0996          -485.4046 352.6790
		AS3          2992.0840393   3023.7155759    451.0000000   0.1279   0.0436          7021.3753 3666.4869
		BS3          2984.2635989   3047.4607776    452.0000000   0.1349   0.0456          13976.4679 7245.0416
		CS3          2968.9080024   3095.0440832    453.0000000   0.1599   0.0532          27929.2324 14318.7192
		DS3          2992.5598923   3023.8691175    449.0000000   0.1287   0.0411          7497.2283 3820.0285
		ES3          2984.9295928   3047.6764361    448.0000000   0.1355   0.0438          14642.4618 7460.7001
		FS3          2969.3836298   3095.1983190    447.0000000   0.1602   0.0523          28404.8598 14472.9550
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector BS1 = dataset.getPoints().getObject("BS1").getEstimatedValue();
		ensure_equals("BS1 x coordinate should match", BS1.getX().getMetresValue(), 3050.0000171, 1e-7);
		ensure_equals("BS1 y coordinate should match", BS1.getY().getMetresValue(), 3000.4000200, 1e-7);
		ensure_equals("BS1 z coordinate should match", BS1.getZ().getMetresValue(), 452.0000000, 1e-7);
		ensure_equals("BS1 Sx should match", dataset.getPoints().getObject("BS1").getXEstPrecision().getMMetresValue(), 0.0094, 1e-4);
		ensure_equals("BS1 Sy should match", dataset.getPoints().getObject("BS1").getYEstPrecision().getMMetresValue(), 0.1421, 1e-4);

		TPositionVector CS3 = dataset.getPoints().getObject("CS3").getEstimatedValue();
		ensure_equals("CS3 x coordinate should match", CS3.getX().getMetresValue(), 2968.8129740, 1e-7);
		ensure_equals("CS3 y coordinate should match", CS3.getY().getMetresValue(), 3095.0129437, 1e-7);
		ensure_equals("CS3 z coordinate should match", CS3.getZ().getMetresValue(), 453.0000000, 1e-7);
		ensure_equals("CS3 Sx should match", dataset.getPoints().getObject("CS3").getXEstPrecision().getMMetresValue(), 0.1599, 1e-4);
		ensure_equals("CS3 Sy should match", dataset.getPoints().getObject("CS3").getYEstPrecision().getMMetresValue(), 0.0532, 1e-4);
		
	}

	template<>
	template<>
	void object::test<2>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECTH measurement in LEP");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECTH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecth_LEP);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		AS1          3025.0001525   3000.3001432   2450.8566494    451.0000000   0.0095   0.1348            0.1525   0.1432
		BS1          3050.0003269   3000.4003066   2451.8527088    452.0000000   0.0094   0.1421            0.3269   0.3066
		CS1          3100.0004758   3000.3004300   2452.8445883    453.0000000   0.0094   0.1682            0.4758   0.4300
		DS1          3025.0001525   2999.7001431   2450.8567353    451.0000000   0.0095   0.1348            0.1525 -599.8569
		ES1          3050.0003069   2999.6002763   2451.8528234    452.0000000   0.0094   0.1421            0.3069 -799.7237
		FS1          3100.0004859   2999.7004297   2452.8446742    453.0000000   0.0094   0.1682            0.4859 -599.5703
		AS2          2985.5482162   2979.5983695   2450.8655837    451.0000000   0.1102   0.0783            0.1422   0.1295
		BS2          2970.9346973   2959.3142876   2451.8705530    452.0000000   0.1158   0.0830            0.3533   0.2516
		CS2          2941.4646145   2918.9223631   2452.8802108    453.0000000   0.1365   0.0988            0.4345   0.3981
		DS2          2985.0624902   2979.9507877   2448.8656068    449.0000000   0.1083   0.0809          -485.5838 352.5477
		ES2          2970.3160898   2959.8243183   2446.8705741    447.0000000   0.1144   0.0848          -618.2542 510.2823
		FS2          2940.9792188   2919.2750547   2452.8802340    453.0000000   0.1359   0.0996          -484.9612 353.0897
		AS3          2992.0841897   3023.7157207   2450.8582794    451.0000000   0.1279   0.0436          7021.5257 3666.6317
		BS3          2984.2638985   3047.4610708   2451.8559179    452.0000000   0.1349   0.0456          13976.7675 7245.3348
		CS3          2968.9084481   3095.0445340   2452.8508447    453.0000000   0.1599   0.0531          27929.6781 14319.1700
		DS3          2992.5597409   3023.8689722   2448.8581854    449.0000000   0.1287   0.0411          7497.0769 3819.8832
		ES3          2984.9292911   3047.6761417   2447.8557862    448.0000000   0.1355   0.0438          14642.1601 7460.4057
		FS3          2969.3831801   3095.1978658   2446.8507507    447.0000000   0.1602   0.0523          28404.4101 14472.5018
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector BS1 = dataset.getPoints().getObject("BS1").getEstimatedValue();
		ensure_equals("BS1 x coordinate should match", BS1.getX().getMetresValue(), 3050.0003269, 1e-7);
		ensure_equals("BS1 y coordinate should match", BS1.getY().getMetresValue(), 3000.4003066, 1e-7);
		ensure_equals("BS1 z coordinate should match", BS1.getZ().getMetresValue(), 2451.8527088, 1e-7);
		ensure_equals("BS1 Sx should match", dataset.getPoints().getObject("BS1").getXEstPrecision().getMMetresValue(), 0.0094, 1e-4);
		ensure_equals("BS1 Sy should match", dataset.getPoints().getObject("BS1").getYEstPrecision().getMMetresValue(), 0.1421, 1e-4);
		
		TPositionVector FS3 = dataset.getPoints().getObject("FS3").getEstimatedValue();
		ensure_equals("FS3 x coordinate should match", FS3.getX().getMetresValue(), 2969.3831801, 1e-7);
		ensure_equals("FS3 y coordinate should match", FS3.getY().getMetresValue(), 3095.1978658, 1e-7);
		ensure_equals("FS3 z coordinate should match", FS3.getZ().getMetresValue(), 2446.8507507, 1e-7);
		ensure_equals("FS3 Sx should match", dataset.getPoints().getObject("FS3").getXEstPrecision().getMMetresValue(), 0.1602, 1e-4);
		ensure_equals("FS3 Sy should match", dataset.getPoints().getObject("FS3").getYEstPrecision().getMMetresValue(), 0.0523, 1e-4);

	}

	template<>
	template<>
	void object::test<3>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECTH measurement in RS2K");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECTH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecth_RS2K);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		AS1          3025.0001525   3000.3001421   2450.8577815   0.0095   0.1348            0.1525   0.1421
		BS1          3050.0003269   3000.4003043   2451.8538404   0.0094   0.1421            0.3269   0.3043
		CS1          3100.0004759   3000.3004267   2452.8457177   0.0094   0.1682            0.4759   0.4267
		DS1          3025.0001525   2999.7001420   2450.8578665   0.0095   0.1348            0.1525 -599.8580
		ES1          3050.0003070   2999.6002741   2451.8539538   0.0094   0.1421            0.3070 -799.7259
		FS1          3100.0004860   2999.7004264   2452.8458027   0.0094   0.1682            0.4860 -599.5736
		AS2          2985.5482162   2979.5983681   2450.8666871   0.1102   0.0783            0.1422   0.1281
		BS2          2970.9346973   2959.3142848   2451.8716283   0.1158   0.0830            0.3533   0.2488
		CS2          2941.4646143   2918.9223591   2452.8812311   0.1365   0.0988            0.4343   0.3941
		DS2          2985.0624902   2979.9507891   2448.8667107   0.1083   0.0809          -485.5838 352.5491
		ES2          2970.3160898   2959.8243224   2446.8716501   0.1144   0.0848          -618.2542 510.2864
		FS2          2940.9792186   2919.2750506   2452.8812548   0.1359   0.0996          -484.9614 353.0856
		AS3          2992.0841900   3023.7157194   2450.8594460   0.1279   0.0436          7021.5260 3666.6304
		BS3          2984.2638991   3047.4610681   2451.8571190   0.1349   0.0456          13976.7681 7245.3321
		CS3          2968.9084491   3095.0445300   2452.8521155   0.1599   0.0531          27929.6791 14319.1660
		DS3          2992.5597406   3023.8689736   2448.8593522   0.1287   0.0411          7497.0766 3819.8846
		ES3          2984.9292905   3047.6761444   2447.8569876   0.1355   0.0438          14642.1595 7460.4084
		FS3          2969.3831792   3095.1978699   2446.8520216   0.1602   0.0523          28404.4092 14472.5059
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector BS1 = dataset.getPoints().getObject("BS1").getEstimatedValue();
		ensure_equals("BS1 x coordinate should match", BS1.getX().getMetresValue(), 3050.0003269, 1e-7);
		ensure_equals("BS1 y coordinate should match", BS1.getY().getMetresValue(), 3000.4003043, 1e-7);
		ensure_equals("BS1 z coordinate should match", BS1.getZ().getMetresValue(), 2451.8538404, 1e-7);
		ensure_equals("BS1 Sx should match", dataset.getPoints().getObject("BS1").getXEstPrecision().getMMetresValue(), 0.0094, 1e-4);
		ensure_equals("BS1 Sy should match", dataset.getPoints().getObject("BS1").getYEstPrecision().getMMetresValue(), 0.1421, 1e-4);

		TPositionVector FS3 = dataset.getPoints().getObject("FS3").getEstimatedValue();
		ensure_equals("FS3 x coordinate should match", FS3.getX().getMetresValue(), 2969.3831792, 1e-7);
		ensure_equals("FS3 y coordinate should match", FS3.getY().getMetresValue(), 3095.1978699, 1e-7);
		ensure_equals("FS3 z coordinate should match", FS3.getZ().getMetresValue(), 2446.8520216, 1e-7);
		ensure_equals("FS3 Sx should match", dataset.getPoints().getObject("FS3").getXEstPrecision().getMMetresValue(), 0.1602, 1e-4);
		ensure_equals("FS3 Sy should match", dataset.getPoints().getObject("FS3").getYEstPrecision().getMMetresValue(), 0.0523, 1e-4);

	}

	template<>
	template<>
	void object::test<4>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECTH measurement in SPHE");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECTH.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecth_SPHE);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		AS1          3025.0001581   3000.3001417   2450.8544226   0.0095   0.1348            0.1581   0.1417
		BS1          3050.0003382   3000.4003034   2451.8503375   0.0094   0.1421            0.3382   0.3034
		CS1          3100.0004934   3000.3004252   2452.8419156   0.0094   0.1682            0.4934   0.4252
		DS1          3025.0001581   2999.7001416   2450.8545076   0.0095   0.1348            0.1581 -599.8584
		ES1          3050.0003183   2999.6002732   2451.8504508   0.0094   0.1421            0.3183 -799.7268
		FS1          3100.0005034   2999.7004249   2452.8420006   0.0094   0.1682            0.5034 -599.5751
		AS2          2985.5482217   2979.5983680   2450.8635460   0.1102   0.0783            0.1477   0.1280
		BS2          2970.9347082   2959.3142847   2451.8685647   0.1158   0.0830            0.3642   0.2487
		CS2          2941.4646304   2918.9223590   2452.8783211   0.1365   0.0988            0.4504   0.3940
		DS2          2985.0624847   2979.9507892   2448.8635723   0.1083   0.0809          -485.5893 352.5492
		ES2          2970.3160736   2959.8243226   2446.8685899   0.1144   0.0848          -618.2704 510.2866
		FS2          2940.9792347   2919.2750505   2452.8783473   0.1359   0.0996          -484.9453 353.0855
		AS3          2992.0841952   3023.7157191   2450.8562731   0.1279   0.0436          7021.5312 3666.6301
		BS3          2984.2639093   3047.4610677   2451.8539910   0.1349   0.0456          13976.7783 7245.3317
		CS3          2968.9084640   3095.0445293   2452.8490746   0.1599   0.0531          27929.6940 14319.1653
		DS3          2992.5597354   3023.8689738   2448.8561767   0.1287   0.0411          7497.0714 3819.8848
		ES3          2984.9292802   3047.6761448   2447.8538560   0.1355   0.0438          14642.1492 7460.4088
		FS3          2969.3831642   3095.1978707   2446.8489783   0.1602   0.0523          28404.3942 14472.5067
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector BS1 = dataset.getPoints().getObject("BS1").getEstimatedValue();
		ensure_equals("BS1 x coordinate should match", BS1.getX().getMetresValue(), 3050.0003382, 1e-7);
		ensure_equals("BS1 y coordinate should match", BS1.getY().getMetresValue(), 3000.4003034, 1e-7);
		ensure_equals("BS1 z coordinate should match", BS1.getZ().getMetresValue(), 2451.8503375, 1e-7);
		ensure_equals("BS1 Sx should match", dataset.getPoints().getObject("BS1").getXEstPrecision().getMMetresValue(), 0.0094, 1e-4);
		ensure_equals("BS1 Sy should match", dataset.getPoints().getObject("BS1").getYEstPrecision().getMMetresValue(), 0.1421, 1e-4);

		TPositionVector FS3 = dataset.getPoints().getObject("FS3").getEstimatedValue();
		ensure_equals("FS3 x coordinate should match", FS3.getX().getMetresValue(), 2969.3831642, 1e-7);
		ensure_equals("FS3 y coordinate should match", FS3.getY().getMetresValue(), 3095.1978707, 1e-7);
		ensure_equals("FS3 z coordinate should match", FS3.getZ().getMetresValue(), 2446.8489783, 1e-7);
		ensure_equals("FS3 Sx should match", dataset.getPoints().getObject("FS3").getXEstPrecision().getMMetresValue(), 0.1602, 1e-4);
		ensure_equals("FS3 Sy should match", dataset.getPoints().getObject("FS3").getYEstPrecision().getMMetresValue(), 0.0523, 1e-4);

	}

	//----------------------------- ECVE --------------------------------//
	template<>
	template<>
	void object::test<5>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECVE measurement in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECVE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecve_OLOC_PtLine);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		C1   2058.7785255   2180.9016976    450.0000212   0.4644   0.3643           -0.0045  -0.0024
		D1   2088.1677910   2221.3525452    450.0010136   1.0183   0.7650            0.0010  -0.0048
		E1   2117.5570742   2261.8034171    450.0023975   1.6934   1.2531            0.0242   0.0171
		F1   2146.9463397   2302.2542647    450.0041728   2.4721   1.8170            0.0297   0.0147
		G1   2176.3356346   2342.7051527    450.0063395   3.3424   2.4478            0.0546   0.0527
		H1   2205.7249002   2383.1560003    450.0088976   4.2957   3.1391            0.0602   0.0503
		I1   2235.1141657   2423.6068480    450.0118471   5.3252   3.8861            0.0657   0.0480
		J1   2264.5034547   2464.0577279    450.0151880   6.4258   4.6848            0.0947   0.0779
		K1   2293.8927203   2504.5085755    450.0189202   7.5932   5.5322            0.0903   0.0755
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector D1 = dataset.getPoints().getObject("D1").getEstimatedValue();
		ensure_equals("D1 x coordinate should match", D1.getX().getMetresValue(), 2088.1677910, 1e-7);
		ensure_equals("D1 y coordinate should match", D1.getY().getMetresValue(), 2221.3525452, 1e-7);
		ensure_equals("D1 z coordinate should match", D1.getZ().getMetresValue(), 450.0010136, 1e-7);
		ensure_equals("D1 Sx should match", dataset.getPoints().getObject("D1").getXEstPrecision().getMMetresValue(), 1.0183, 1e-4);
		ensure_equals("D1 Sy should match", dataset.getPoints().getObject("D1").getYEstPrecision().getMMetresValue(), 0.7650, 1e-4);

		TPositionVector I1 = dataset.getPoints().getObject("I1").getEstimatedValue();
		ensure_equals("I1 x coordinate should match", I1.getX().getMetresValue(), 2235.1141657, 1e-7);
		ensure_equals("I1 y coordinate should match", I1.getY().getMetresValue(), 2423.6068480, 1e-7);
		ensure_equals("I1 z coordinate should match", I1.getZ().getMetresValue(), 450.0118471, 1e-7);
		ensure_equals("I1 Sx should match", dataset.getPoints().getObject("I1").getXEstPrecision().getMMetresValue(), 5.3252, 1e-4);
		ensure_equals("I1 Sy should match", dataset.getPoints().getObject("I1").getYEstPrecision().getMMetresValue(), 3.8861, 1e-4);

	}

	template<>
	template<>
	void object::test<6>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECVE measurement in RS2K");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECVE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecve_RS2K_PtLine);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		C1   2058.7785255   2180.9016976   2450.0000769   0.4644   0.3643           -0.0045  -0.0024
		D1   2088.1677911   2221.3525452   2450.0001195   1.0183   0.7650            0.0011  -0.0048
		E1   2117.5570742   2261.8034171   2450.0001649   1.6934   1.2531            0.0242   0.0171
		F1   2146.9463398   2302.2542648   2450.0002131   2.4721   1.8170            0.0298   0.0148
		G1   2176.3356347   2342.7051529   2450.0002638   3.3424   2.4478            0.0547   0.0529
		H1   2205.7249003   2383.1560005   2450.0003172   4.2957   3.1391            0.0603   0.0505
		I1   2235.1141659   2423.6068482   2450.0003730   5.3252   3.8861            0.0659   0.0482
		J1   2264.5034550   2464.0577283   2450.0004313   6.4258   4.6848            0.0950   0.0783
		K1   2293.8927206   2504.5085760   2450.0004919   7.5932   5.5322            0.0906   0.0760
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector D1 = dataset.getPoints().getObject("D1").getEstimatedValue();
		ensure_equals("D1 x coordinate should match", D1.getX().getMetresValue(), 2088.1677911, 1e-7);
		ensure_equals("D1 y coordinate should match", D1.getY().getMetresValue(), 2221.3525452, 1e-7);
		ensure_equals("D1 z coordinate should match", D1.getZ().getMetresValue(), 2450.0001195, 1e-7);
		ensure_equals("D1 Sx should match", dataset.getPoints().getObject("D1").getXEstPrecision().getMMetresValue(), 1.0183, 1e-4);
		ensure_equals("D1 Sy should match", dataset.getPoints().getObject("D1").getYEstPrecision().getMMetresValue(), 0.7650, 1e-4);

		TPositionVector I1 = dataset.getPoints().getObject("I1").getEstimatedValue();
		ensure_equals("I1 x coordinate should match", I1.getX().getMetresValue(), 2235.1141659, 1e-7);
		ensure_equals("I1 y coordinate should match", I1.getY().getMetresValue(), 2423.6068482, 1e-7);
		ensure_equals("I1 z coordinate should match", I1.getZ().getMetresValue(), 2450.0003730, 1e-7);
		ensure_equals("I1 Sx should match", dataset.getPoints().getObject("I1").getXEstPrecision().getMMetresValue(), 5.3252, 1e-4);
		ensure_equals("I1 Sy should match", dataset.getPoints().getObject("I1").getYEstPrecision().getMMetresValue(), 3.8861, 1e-4);

	}

	template<>
	template<>
	void object::test<7>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECVE measurement in LEP");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECVE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecve_LEP_PtLine);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		C1   2058.7785255   2180.9016976   2450.0000022   0.4644   0.3643           -0.0045  -0.0024
		D1   2088.1677911   2221.3525452   2450.0000050   1.0183   0.7650            0.0011  -0.0048
		E1   2117.5570742   2261.8034171   2450.0000091   1.6934   1.2531            0.0242   0.0171
		F1   2146.9463398   2302.2542648   2450.0000145   2.4721   1.8170            0.0298   0.0148
		G1   2176.3356347   2342.7051529   2450.0000210   3.3424   2.4478            0.0547   0.0529
		H1   2205.7249003   2383.1560005   2450.0000288   4.2957   3.1391            0.0603   0.0505
		I1   2235.1141659   2423.6068482   2450.0000378   5.3252   3.8861            0.0659   0.0482
		J1   2264.5034550   2464.0577283   2450.0000481   6.4258   4.6848            0.0950   0.0783
		K1   2293.8927206   2504.5085760   2450.0000594   7.5932   5.5322            0.0906   0.0760
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector D1 = dataset.getPoints().getObject("D1").getEstimatedValue();
		ensure_equals("D1 x coordinate should match", D1.getX().getMetresValue(), 2088.1677911, 1e-7);
		ensure_equals("D1 y coordinate should match", D1.getY().getMetresValue(), 2221.3525452, 1e-7);
		ensure_equals("D1 z coordinate should match", D1.getZ().getMetresValue(), 2450.0000050, 1e-7);
		ensure_equals("D1 Sx should match", dataset.getPoints().getObject("D1").getXEstPrecision().getMMetresValue(), 1.0183, 1e-4);
		ensure_equals("D1 Sy should match", dataset.getPoints().getObject("D1").getYEstPrecision().getMMetresValue(), 0.7650, 1e-4);
		
		TPositionVector I1 = dataset.getPoints().getObject("I1").getEstimatedValue();
		ensure_equals("I1 x coordinate should match", I1.getX().getMetresValue(), 2235.1141659, 1e-7);
		ensure_equals("I1 y coordinate should match", I1.getY().getMetresValue(), 2423.6068482, 1e-7);
		ensure_equals("I1 z coordinate should match", I1.getZ().getMetresValue(), 2450.0000378, 1e-7);
		ensure_equals("I1 Sx should match", dataset.getPoints().getObject("I1").getXEstPrecision().getMMetresValue(), 5.3252, 1e-4);
		ensure_equals("I1 Sy should match", dataset.getPoints().getObject("I1").getYEstPrecision().getMMetresValue(), 3.8861, 1e-4);

	}

	template<>
	template<>
	void object::test<8>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECVE measurement in SPHE");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECVE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecve_SPHE_PtLine);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		C1   2058.7785255   2180.9016976   2449.9999980   0.4644   0.3643           -0.0045  -0.0024
		D1   2088.1677911   2221.3525452   2449.9999955   1.0183   0.7650            0.0011  -0.0048
		E1   2117.5570742   2261.8034171   2449.9999921   1.6934   1.2531            0.0242   0.0171
		F1   2146.9463398   2302.2542648   2449.9999877   2.4721   1.8170            0.0298   0.0148
		G1   2176.3356347   2342.7051529   2449.9999823   3.3424   2.4478            0.0547   0.0529
		H1   2205.7249003   2383.1560005   2449.9999759   4.2957   3.1391            0.0603   0.0505
		I1   2235.1141659   2423.6068482   2449.9999686   5.3252   3.8861            0.0659   0.0482
		J1   2264.5034550   2464.0577283   2449.9999603   6.4258   4.6848            0.0950   0.0783
		K1   2293.8927206   2504.5085760   2449.9999509   7.5932   5.5322            0.0906   0.0760
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector D1 = dataset.getPoints().getObject("D1").getEstimatedValue();
		ensure_equals("D1 x coordinate should match", D1.getX().getMetresValue(), 2088.1677911, 1e-7);
		ensure_equals("D1 y coordinate should match", D1.getY().getMetresValue(), 2221.3525452, 1e-7);
		ensure_equals("D1 z coordinate should match", D1.getZ().getMetresValue(), 2449.9999955, 1e-7);
		ensure_equals("D1 Sx should match", dataset.getPoints().getObject("D1").getXEstPrecision().getMMetresValue(), 1.0183, 1e-4);
		ensure_equals("D1 Sy should match", dataset.getPoints().getObject("D1").getYEstPrecision().getMMetresValue(), 0.7650, 1e-4);
		
		TPositionVector I1 = dataset.getPoints().getObject("I1").getEstimatedValue();
		ensure_equals("I1 x coordinate should match", I1.getX().getMetresValue(), 2235.1141659, 1e-7);
		ensure_equals("I1 y coordinate should match", I1.getY().getMetresValue(), 2423.6068482, 1e-7);
		ensure_equals("I1 z coordinate should match", I1.getZ().getMetresValue(), 2449.9999686, 1e-7);
		ensure_equals("I1 Sx should match", dataset.getPoints().getObject("I1").getXEstPrecision().getMMetresValue(), 5.3252, 1e-4);
		ensure_equals("I1 Sy should match", dataset.getPoints().getObject("I1").getYEstPrecision().getMMetresValue(), 3.8861, 1e-4);
	}

	template<>
	template<>
	void object::test<9>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECVE measurement in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECVE.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecve_OLOC);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY 
		STN        0.0000000       -0.0000000        0.0000000 
		ECVE_line24        0.0000295       -0.0000295       60.0000000
		*/

		//
		TPositionVector STN = dataset.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 0.0, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 0.0, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 0.0, 1e-7);

		//the point on the line is the mean of all the measured points at the beginning and Z is fixed
		ensure_equals("ECVE_line24 x coordinate should match", dataset.getPoints().getObject("ECVE_line24").getProvisionalValue().getX().getMetresValue(), 20.0, 1e-7);
		ensure_equals("ECVE_line24 y coordinate should match", dataset.getPoints().getObject("ECVE_line24").getProvisionalValue().getY().getMetresValue(), 0.0, 1e-7);
		ensure_equals("ECVE_line24 z coordinate should match", dataset.getPoints().getObject("ECVE_line24").getEstimatedValue().getZ().getMetresValue(), 60.0, 1e-7);
	}

	//----------------------------- ECSP --------------------------------//
	template<>
	template<>
	void object::test<10>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing ECSP measurement in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/outECSP.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestScaleInstr::ecsp_OLOC);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& dataset = calcul.getData();

		/* LGC1 results:  VXY
		P1       -0.0000000      100.1000000        0.0000000   0.0001062   0.0001041
		P2      100.5000000       -0.0000000        0.0000000   0.0001041   0.0001062
		*/

		//We check only 2 points, which are choose with not particular meaning
		TPositionVector P1 = dataset.getPoints().getObject("P1").getEstimatedValue();
		ensure_equals("P1 x coordinate should match", P1.getX().getMetresValue(), 0.0, 1e-7);
		ensure_equals("P1 y coordinate should match", P1.getY().getMetresValue(), 100.1, 1e-7);
		ensure_equals("P1 z coordinate should match", P1.getZ().getMetresValue(), 0.0, 1e-7);
		ensure_equals("P1 Sx should match", dataset.getPoints().getObject("P1").getXEstPrecision().getMMetresValue(), 0.0001062, 1e-4);
		ensure_equals("P1 Sy should match", dataset.getPoints().getObject("P1").getYEstPrecision().getMMetresValue(), 0.0001041, 1e-4);

		TPositionVector P2 = dataset.getPoints().getObject("P2").getEstimatedValue();
		ensure_equals("P2 x coordinate should match", P2.getX().getMetresValue(), 100.5, 1e-7);
		ensure_equals("P2 y coordinate should match", P2.getY().getMetresValue(), 0.0, 1e-7);
		ensure_equals("P2 z coordinate should match", P2.getZ().getMetresValue(), 0.0, 1e-7);
		ensure_equals("P2 Sx should match", dataset.getPoints().getObject("P2").getXEstPrecision().getMMetresValue(), 0.0001041, 1e-4);
		ensure_equals("P2 Sy should match", dataset.getPoints().getObject("P2").getYEstPrecision().getMMetresValue(), 0.0001062, 1e-4);
	}

}
