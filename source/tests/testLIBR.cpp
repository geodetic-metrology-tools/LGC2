#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testLIBR.h"
#include "TLGCCalculation.h"


namespace tut
{
	struct test_LIBR{};
	typedef test_group<test_LIBR> factory;
	typedef factory::object object;
}

namespace
{
	tut::factory tf("Tests LIBR");
}

namespace tut
{

	// test ZEND, DIST and ANGL in SPHE
	template<>
	template<>
	void object::test<1>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing AZD measurement in SPHE");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/LIBR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLIBR::LIBR1);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& data = calcul.getData();

		/* lgc1 results
		STN          2260.7774073   4493.9463772   2401.7426406   0.0000   0.0000   0.0000   0.0373   0.2072  -0.0294
		REF          2203.0484484   4412.2919770   2401.7757707   0.0000   0.0000   0.0000   0.1876  -0.3574   0.0295
		STN2         2284.7017749   4354.5631251   2401.7940957   0.0000   0.0000   0.0000  -0.3280  -0.1029  -0.0142
		PT           2342.4313479   4436.2171177   2401.7610678   0.0000   0.0000   0.0000   0.1358   0.0541   0.0291
		PT2          2283.7020702   4353.5634271   2401.7944936   0.0000   0.0000   0.0000  -0.0327   0.1991  -0.0150
		*/

		TPositionVector PT = data.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2342.4313479, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 4436.2171177, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 2401.7610678, 1e-7);
		TPositionVector STN = data.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 2260.7774073, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 4493.9463772, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 2401.7426406, 1e-7);
	}

	// test ZEND, DHOR and ANGL in OLOC
	template<>
	template<>
	void object::test<2>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing AZDhor measurement in OLOC");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/LIBR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLIBR::LIBR2);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& data = calcul.getData();

		/* lgc1 results
		STN          2260.7773786   4493.9462033    402.1981709   0.0000   0.0000   0.0000   0.0086   0.0333   0.3821 
		REF          2203.0482892   4412.2923046    402.1981910   0.0000   0.0000   0.0000   0.0284  -0.0299  -0.3826 
		STN2         2284.7021160   4354.5632592    402.1995487   0.0000   0.0000   0.0000   0.0131   0.0311   0.1904 
		PT           2342.4311500   4436.2169968    402.1981941   0.0000   0.0000   0.0000  -0.0621  -0.0667  -0.3795 
		PT2          2283.7021149   4353.5632603    402.1995478   0.0000   0.0000   0.0000   0.0119   0.0323   0.1895
		*/

		TPositionVector PT = data.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2342.4311500, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 4436.2169968, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 402.1981941, 1e-7);
		TPositionVector STN = data.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 2260.7773786, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 4493.9462033, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 402.1981709, 1e-7);
	}

	// test PLR in SPHE
	template<>
	template<>
	void object::test<3>()
	{
		std::shared_ptr<TLGCData> projTest(new TLGCData);

		set_test_name("Testing PLR measurement in SPHE");
		TReader r(projTest);
		projTest->getFileLogger().setOutputfileLocation("C:/Temp/LIBR.txt");
		projTest->getFileLogger().writeReportHeader("LGC output file");

		stringstream infiler(TestLIBR::LIBR3);

		bool succesReading = r.read(infiler);
		ensure_equals("Reading file successful", succesReading, true);

		TLGCCalculation calcul(projTest);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		bool succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc, true);

		const TLGCData& data = calcul.getData();

		/* lgc1 results
		STN          2260.7774073   4493.9463772   2401.7426406   0.0000   0.0000   0.0000   0.0373   0.2072  -0.0294
		REF          2203.0484484   4412.2919770   2401.7757707   0.0000   0.0000   0.0000   0.1876  -0.3574   0.0295
		STN2         2284.7017749   4354.5631251   2401.7940957   0.0000   0.0000   0.0000  -0.3280  -0.1029  -0.0142
		PT           2342.4313479   4436.2171177   2401.7610678   0.0000   0.0000   0.0000   0.1358   0.0541   0.0291
		PT2          2283.7020702   4353.5634271   2401.7944936   0.0000   0.0000   0.0000  -0.0327   0.1991  -0.0150
		*/

		TPositionVector PT = data.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match", PT.getX().getMetresValue(), 2342.4313479, 1e-7);
		ensure_equals("Pt y coordinate should match", PT.getY().getMetresValue(), 4436.2171177, 1e-7);
		ensure_equals("Pt z coordinate should match", PT.getZ().getMetresValue(), 2401.7610678, 1e-7);
		TPositionVector STN = data.getPoints().getObject("STN").getEstimatedValue();
		ensure_equals("STN x coordinate should match", STN.getX().getMetresValue(), 2260.7774073, 1e-7);
		ensure_equals("STN y coordinate should match", STN.getY().getMetresValue(), 4493.9463772, 1e-7);
		ensure_equals("STN z coordinate should match", STN.getZ().getMetresValue(), 2401.7426406, 1e-7);
	}



};
