#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include <TReader.h>
#include "testComplexSUBFRAME.h"
#include "TLGCCalculation.h"
#include <vector>

namespace tut
{
    struct test_SUBFRAME{};
    typedef test_group<test_SUBFRAME> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test SUBFRAME");
}

namespace tut
{	

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////// PLR3D
//////////////////////////////////////////////////////////////////////////////////////////////////
	template<>
	template<>
	void object::test<1>()
	{
		TLGCData projTest;
		
		set_test_name("Testing plr3D for CAMERA in fixed subframe, setup3");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outputUVECUVD.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestSUBFRAME::BCAM_PLR3D_setup3);
		r.read(infiler);
		
	/*	TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);*/
	}

#if 0
	template<>
	template<>
	void object::test<2>()
	{
		TLGCData projTest;
		
		set_test_name("Testing plr3D for CAMERA, RZ free, setup3");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		//Working for different V0s, Z parameter of the transformation is free, substitues the V0 for TSTN
		stringstream infiler(TestSUBFRAME::BCAM_PLR3D_RZfree_setup3);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();

		TDataTreeIterator it = dataset.getTree().begin();
		it++;

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);

		ensure_equals("Z rotation must match expected (-10 gons)",it.node->data->frame.getEstParam().kappa.gon(), 100.0  , 1e-8);
	}


	template<>
	template<>
	void object::test<3>()
	{
		TLGCData projTest;
		
		set_test_name("Testing plr3D for CAMERA, TX free, setup3");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		//Working for different V0s, Z parameter of the transformation is free, substitues the V0 for TSTN
		stringstream infiler(TestSUBFRAME::BCAM_PLR3D_TXfree_setup3);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();

		TDataTreeIterator it = dataset.getTree().begin();
		it++;

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-4);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);

		ensure_equals("Z rotation must match expected (-10 gons)",it.node->data->frame.getEstParam().kappa.gon(), 0  , 1e-8);
		ensure_equals("X translation must match expected (200 gons)",it.node->data->frame.getEstParam().t1, 200  , 1e-8);
	}



	template<>
	template<>
	void object::test<4>()
	{
		TLGCData projTest;
		
		set_test_name("Testing plr3D for CAMERA, TX and RZ free, 2BCAMs, setup3");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outputPLR111.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		//Working for different V0s, Z parameter of the transformation is free, substitues the V0 for TSTN
		stringstream infiler(TestSUBFRAME::BCAM_PLR3D_TXRZfree_setup3_2BCAMs);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();



		TDataTreeIterator it = dataset.getTree().begin();
		it++;

		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 0.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 100.0, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 100.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);
		
		ensure_equals("Z rotation must match expected (-10 gons)",it.node->data->frame.getEstParam().kappa.gon(), 200  , 1e-8);
		ensure_equals("X translation must match expected (200 gons)",it.node->data->frame.getEstParam().t1, 200  , 1e-8);
	}

	/*The same thing appear with inversion if RZ = 200, point values inverted!!!*/
	template<>
	template<>
	void object::test<5>()
	{
		TLGCData projTest;
		
		set_test_name("Test with station the origin CAM PLR3D");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outputPLR.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestSUBFRAME::BCAM_PLR3D_CAMORIG);
		r.read(infiler);
		
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 100.0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 200.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);

		TPositionVector PT2 = dataset.getPoints().getObject("PT2").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT2.getX().getValue(), 200, 1e-8);
		ensure_equals("Pt y coordinate should match",PT2.getY().getValue(), 200.0, 1e-8);
		ensure_equals("Pt z coordinate should match",PT2.getZ().getValue(), 0.0  , 1e-8);
		
		TDataTreeIterator it = dataset.getTree().begin();
		it++;
		ensure_equals("Z rotation must match expected (190 gons)",it.node->data->frame.getEstParam().kappa.gon(), 190  , 1e-8);
	}
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////// DIR3D
//////////////////////////////////////////////////////////////////////////////////////////////////

	
	template<>
	template<>
	void object::test<6>()
	{
		TLGCData projTest;
		
		set_test_name("DIR3D, TEST1");
		TReader r(&projTest);
		projTest.getFileLogger().setOutputfileLocation("C:/Temp/outputDIR3D.txt");
		projTest.getFileLogger().writeReportHeader("LGC output file");
		
		//One POINT and one TSTN (V0) = 4 unknowns)
		stringstream infiler(TestSUBFRAME::BCAM_DIR3D_setup1);
		r.read(infiler);
	/*	
		TLGCCalculation calcul(&projTest);
		calcul.computeLSResults();

		const TLGCData& dataset = calcul.getData();
		TPositionVector PT = dataset.getPoints().getObject("PT").getEstimatedValue();
		ensure_equals("Pt x coordinate should match",PT.getX().getValue(), 0  , 1e-8);
		ensure_equals("Pt y coordinate should match",PT.getY().getValue(), 100, 1e-8);
		ensure_equals("Pt z coordinate should match",PT.getZ().getValue(), 0.0  , 1e-8);
		
		TDataTreeIterator it = dataset.getTree().begin();
		it++;
		ensure_equals("Z rotation must match expected (190 gons)",it.node->data->frame.getEstParam().kappa.gon(), 0  , 1e-8);*/
	}
#endif
}
