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

	// test ZEND (Hi), DIST (Hi + Cs) and ANGL (V0) in SPHE
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



};
