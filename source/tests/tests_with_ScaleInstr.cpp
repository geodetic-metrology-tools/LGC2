#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include "Utils.h"
#include <TLGCData.h>
#include <readers/TReader.h>
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
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
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
		std::shared_ptr<TResSimFileWriter> fileWriter(nullptr);
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
		//TPositionVector BS1 = dataset.getPoints().getObject("BS1").getEstimatedValue();
		//ensure_equals("BS1 x coordinate should match", BS1.getX().getMetresValue(), 3050.0003269, 1e-7);
		//ensure_equals("BS1 y coordinate should match", BS1.getY().getMetresValue(), 3000.4003066, 1e-7);
		//ensure_equals("BS1 z coordinate should match", BS1.getZ().getMetresValue(), 2451.8527088, 1e-7);
		//ensure_equals("BS1 Sx should match", dataset.getPoints().getObject("BS1").getXEstPrecision().getMMetresValue(), 0.0094, 1e-4);
		//ensure_equals("BS1 Sy should match", dataset.getPoints().getObject("BS1").getYEstPrecision().getMMetresValue(), 0.1421, 1e-4);
		//
		//TPositionVector FS3 = dataset.getPoints().getObject("FS3").getEstimatedValue();
		//ensure_equals("FS3 x coordinate should match", FS3.getX().getMetresValue(), 2969.3831801, 1e-7);
		//ensure_equals("FS3 y coordinate should match", FS3.getY().getMetresValue(), 3095.1978658, 1e-7);
		//ensure_equals("FS3 z coordinate should match", FS3.getZ().getMetresValue(), 2446.8507507, 1e-7);
		//ensure_equals("FS3 Sx should match", dataset.getPoints().getObject("FS3").getXEstPrecision().getMMetresValue(), 0.1602, 1e-4);
		//ensure_equals("FS3 Sy should match", dataset.getPoints().getObject("FS3").getYEstPrecision().getMMetresValue(), 0.0523, 1e-4);

	}
}