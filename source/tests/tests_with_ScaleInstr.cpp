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


}