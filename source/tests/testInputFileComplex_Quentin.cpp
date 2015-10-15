#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <readers/TReader.h>
#include "testComplexROOT.h"
#include "testComplexROOT_Quentin.h"
#include "TLGCCalculation.h"

namespace tut
{
    struct test_complex{};
    typedef test_group<test_complex> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Complex network testing");
}

namespace tut
{	
	// Test Ang Zen & Dist, with z in the same plane
	template<>
	template<>
	void object::test<1>()
	{
		TLGCData projTest;
		
		set_test_name("Testing param case in ROOT, setup4");
		TReader r(&projTest);
		projTest.fOutputFileWriter.setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest.fOutputFileWriter.writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestROOT::Param_setup4);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.points.getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.points.getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);


		TReal ST1_V0 = dataset.angles.getObject("ROOTV00").getEstimatedValue().gon();
		ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 10, 1e-8);

		TReal ST2_V0 = dataset.angles.getObject("ROOTV01").getEstimatedValue().gon();
		ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 390, 1e-8);

	}

	// Test Plr3D, with z in the same plane
	template<>
	template<>
	void object::test<2>()
	{
		TLGCData projTest;
		
		set_test_name("Testing plr3D case in ROOT, setup4");
		TReader r(&projTest);
		projTest.fOutputFileWriter.setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest.fOutputFileWriter.writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestROOT::PLR3D_setup4);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.points.getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.points.getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);


		TReal ST1_V0 = dataset.angles.getObject("ROOTV00").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 200, 1e-8);

		TReal ST2_V0 = dataset.angles.getObject("ROOTV01").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);

	}

	// Test Dir3D, with z in the same plane
	template<>
	template<>
	void object::test<3>()
	{
#if 0
		TLGCData projTest;
		
		set_test_name("Testing dir3D case in ROOT, setup4");
		TReader r(&projTest);
		projTest.fOutputFileWriter.setOutputfileLocation("C:/Temp/outputPLRqqq.txt");
		projTest.fOutputFileWriter.writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns) DIR3D_setup1  DIR3D_setup4
		stringstream infiler(TestROOT::DIR3D_setup4);
		r.read(infiler);
 
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.points.getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.points.getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);

		TReal ST1_V0 = dataset.angles.getObject("ROOTV00").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 390, 1e-8);

		TReal ST2_V0 = dataset.angles.getObject("ROOTV01").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);
#endif
	}

	// Test Ang Zen & Dist, with z in a different plane
	template<>
	template<>
	void object::test<4>()
	{
		TLGCData projTest;
		
		set_test_name("Testing param case in ROOT, setup4");
		TReader r(&projTest);
		projTest.fOutputFileWriter.setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest.fOutputFileWriter.writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestROOT::Param_setup4_different_z);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.points.getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 1.0  , 1e-8);

		TPositionVector PT2 = dataset.points.getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 1.0  , 1e-8);


		TReal ST1_V0 = dataset.angles.getObject("ROOTV00").getEstimatedValue().gon();
		ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 10, 1e-8);

		TReal ST2_V0 = dataset.angles.getObject("ROOTV01").getEstimatedValue().gon();
		ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 390, 1e-8);
	}

	// Test Plr3D, with z in a different plane
	template<>
	template<>
	void object::test<5>()
	{
		TLGCData projTest;
		
		set_test_name("Testing plr3D case in ROOT, setup4");
		TReader r(&projTest);
		projTest.fOutputFileWriter.setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest.fOutputFileWriter.writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestROOT::PLR3D_setup4_different_z);
		r.read(infiler);

		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.points.getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 1.0  , 1e-8);

		TPositionVector PT2 = dataset.points.getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 1.0  , 1e-8);


		TReal ST1_V0 = dataset.angles.getObject("ROOTV00").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 390, 1e-8);

		TReal ST2_V0 = dataset.angles.getObject("ROOTV01").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);
	}

	// Test Dir3D, with z in a different plane
	template<>
	template<>
	void object::test<6>()
	{
		TLGCData projTest;
#if 0
		set_test_name("Testing dir3D case in ROOT, setup4");
		TReader r(&projTest);
		projTest.fOutputFileWriter.setOutputfileLocation("C:/Temp/outputPLRqqq.txt");
		projTest.fOutputFileWriter.writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestROOT::DIR3D_setup4_different_z);
		r.read(infiler);

		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.points.getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 1.0  , 1e-8);

		TPositionVector PT2 = dataset.points.getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 1.0  , 1e-8);

		TReal ST1_V0 = dataset.angles.getObject("ROOTV00").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST1",ST1_V0, 390, 1e-8);

		TReal ST2_V0 = dataset.angles.getObject("ROOTV01").getEstimatedValue().gon();
                ensure_equals("V0 calculation should match for total station ST2",ST2_V0, 10, 1e-8);
#endif
	}

}