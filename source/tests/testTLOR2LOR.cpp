#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include "input.h"
#include <TLGCData.h>
#include <ContributionsGenerators/TLOR2LOR.h>
#include <readers/TReader.h>
#include "TestUnitTransfo.h"


#define EXPECT_FAIL(x) \
try {x;} catch (std::exception& e) {cout << endl << "expected failure: " << e.what();}

#pragma warning (disable:4224)

using namespace std;

namespace tut
{
    struct tests_TLOR2LOR{};
    typedef test_group<tests_TLOR2LOR> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test of TLOR2LOR");
}

namespace tut
{
	typedef TDataTree::iterator TDataTreeIter;

	// Testing TLOR2LOR Constructors
	template<>
	template<>
	void object::test<1>()
	{
		using namespace LGC;
		set_test_name("Testing constructors of TLOR2LOR class");

		TLGCData proj;
		TReader r(&proj);
		stringstream infile(LGCTestInput::platesfile2);
		r.read(infile);

		TDataTree tree;
		tree = proj.getTree();

		//Tree structure read from input2.h file
		//        ROOT (1)
		//      /    |    \
		//    P0     P1    P2
		//    / \     \        
		// P3    P4    P5
		//  |
		//  P6

		////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////// Testing constructor out of names ///////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR transform1(tree,"P1","P0","new"); 
		TDataTreeIter from = transform1.getFromNode();
		TDataTreeIter to = transform1.getToNode();

		ensure_equals("Names of the FROM transformation must match",from.node->data->frame.getName(),"P1");
		ensure_equals("Names of the TO transformation must match",to.node->data->frame.getName(),"P0");
		ensure_equals("Names should match",transform1.getName(),"new");

		const std::vector<TLOR2LOR::TransformAndParams>& tCh = transform1.getTransformationChain();
		ensure_equals("Size of transformation chain should match",tCh.size(),2);

		ensure_equals("First transformation type should match",tCh[0].direct,true);
		ensure_equals("Second transformation type should match",tCh[1].direct,false);

		ensure_equals("First transformation type should match",tCh[0].adjTrafo->getName(),"P1");
		ensure_equals("Second transformation type should match",tCh[1].adjTrafo->getName(),"P0");	

		const Eigen::Matrix4d&  mm = tCh[0].trafo->getMatrix();
		const Eigen::Matrix4d&  mm2 = tCh[1].trafo->getMatrix();

		TDirectTransformation dTM;
		auto GON(LGC::TAngle::kGons);			
		//TransformParameters param = {LGC::TAngle(GON,38.19718634205),LGC::TAngle(GON,31.83098861838),LGC::TAngle(GON,25.4647908947	),80,90,100,1};
		TransformParameters param;
		param.omega = LGC::TAngle(GON,38.19718634205);
		param.phi = LGC::TAngle(GON,31.83098861838);
		param.kappa = LGC::TAngle(GON,25.4647908947);

		param.t1 = 80;
		param.t2 = 90;
		param.t3 = 100;


		dTM.setMatrix(param);
		ensure_equals("Matrix in chain should match", mm, dTM.getMatrix());

		TInverseTransformation dTMI;			
		dTMI.setMatrix(param);
		ensure_equals("Matrix in chain should match", mm2, dTMI.getMatrix());

		////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////// Testing constructor out of Iterators /////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR transform2(from,to,"new");
		TDataTreeIter fromI = transform2.getFromNode();
		TDataTreeIter toI = transform2.getToNode();

		ensure_equals("Transformation matrix should match",fromI.node,from.node);
		ensure_equals("Transformation matrix should match",toI.node,to.node);	


		ensure_equals("Names of the FROM transformation must match",fromI.node->data->frame.getName(),from.node->data->frame.getName());
		ensure_equals("Names of the TO transformation must match",toI.node->data->frame.getName(),to.node->data->frame.getName());
		ensure_equals("Names should match",transform2.getName(),transform1.getName());

		const std::vector<TLOR2LOR::TransformAndParams>& tChI = transform2.getTransformationChain();
		ensure_equals("Size of transformation chain should match",tChI.size(),tCh.size());

		ensure_equals("First transformation type should match",tChI[0].direct,tCh[0].direct);
		ensure_equals("Second transformation type should match",tChI[1].direct,tCh[1].direct);

		ensure_equals("First transformation type should match",tChI[0].adjTrafo->getName(),tCh[0].adjTrafo->getName());
		ensure_equals("Second transformation type should match",tChI[1].adjTrafo->getName(),tCh[1].adjTrafo->getName());	

		const Eigen::Matrix4d&  mmI = tChI[0].trafo->getMatrix();
		const Eigen::Matrix4d&  mm2I = tChI[1].trafo->getMatrix();

		ensure_equals("Transformation matrix should match",mmI,mm);
		ensure_equals("Transformation matrix should match",mm2I,mm2);	

		////////////////////////////////////////////////////////////////////////////
		///////////////////////// Testing copy constructor /////////////////////////
		////////////////////////////////////////////////////////////////////////////
		TLOR2LOR transform1Copy(transform1);
		TDataTreeIter fromC = transform1Copy.getFromNode();
		TDataTreeIter toC = transform1Copy.getToNode();

		ensure_equals("Transformation matrix should match",fromC.node,from.node);
		ensure_equals("Transformation matrix should match",toC.node,to.node);	


		ensure_equals("Names of the FROM transformation must match",fromC.node->data->frame.getName(),from.node->data->frame.getName());
		ensure_equals("Names of the TO transformation must match",toC.node->data->frame.getName(),to.node->data->frame.getName());
		ensure_equals("Names should match",transform1Copy.getName(),transform1.getName());

		const std::vector<TLOR2LOR::TransformAndParams>& tCh2 = transform1Copy.getTransformationChain();
		ensure_equals("Size of transformation chain should match",tCh2.size(),tCh.size());

		ensure_equals("First transformation type should match",tCh2[0].direct,tCh[0].direct);
		ensure_equals("Second transformation type should match",tCh2[1].direct,tCh[1].direct);

		ensure_equals("First transformation type should match",tCh2[0].adjTrafo->getName(),tCh[0].adjTrafo->getName());
		ensure_equals("Second transformation type should match",tCh2[1].adjTrafo->getName(),tCh[1].adjTrafo->getName());	

		const Eigen::Matrix4d&  mmC = tCh2[0].trafo->getMatrix();
		const Eigen::Matrix4d&  mm2C = tCh[1].trafo->getMatrix();

		ensure_equals("Transformation matrix should match",mmC,mm);
		ensure_equals("Transformation matrix should match",mm2C,mm2);	
		}


	// Testing TLOR2LOR transformations between nodes
	template<>
	template<>
	void object::test<2>()
	{
		using namespace LGC;
		set_test_name("Testing Transformations between nodes");

		TLGCData proj;
		TReader r(&proj);
		stringstream infile(LGCTestTransfo::testTransformations);
		r.read(infile);				
		TDataTree tree = proj.getTree();

		//////////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR trFPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");
		TLOR2LOR trFREFToFPT2(tree,"FREF","FPT2","FREFFPT2");

		const Eigen::Matrix4d& om = trFPT2ToFREF.getLOR2LORTransformation().getMatrix();

		ensure_equals("m element should match",om(0,0),0.0,1e-8);
		ensure_equals("m element should match",om(0,1),-1.0,1e-8);
		ensure_equals("m element should match",om(0,2),0.0,1e-8);
		ensure_equals("m element should match",om(0,3),-100.0,1e-8);

		ensure_equals("m element should match",om(1,0),1.0,1e-8);
		ensure_equals("m element should match",om(1,1),0.0,1e-8);
		ensure_equals("m element should match",om(1,2),0.0,1e-8);
		ensure_equals("m element should match",om(1,3),-100.0,1e-8);

		ensure_equals("m element should match",om(2,0),0.0,1e-8);
		ensure_equals("m element should match",om(2,1),0.0,1e-8);
		ensure_equals("m element should match",om(2,2),1.0,1e-8);
		ensure_equals("m element should match",om(2,3),0.0,1e-8);

		ensure_equals("m element should match",om(3,0),0.0,1e-8);
		ensure_equals("m element should match",om(3,1),0.0,1e-8);
		ensure_equals("m element should match",om(3,2),0.0,1e-8);
		ensure_equals("m element should match",om(3,3),1.0,1e-8);


		TPositionVector PT2 = proj.getPoints().getObject("PT2").getEstimatedValue();

		trFPT2ToFREF.transform(PT2);

		ensure_equals("X-Coordinates should match",PT2.getX().getValue(), -100.0,1e-8);
		ensure_equals("Y-Coordinates should match",PT2.getY().getValue(), -100.0,1e-8);
		ensure_equals("Z-Coordinates should match",PT2.getZ().getValue(), 0.0,1e-8);

		trFREFToFPT2.transform(PT2);

		ensure_equals("X-Coordinates should match",PT2.getX().getValue(), 0.0,1e-8);
		ensure_equals("Y-Coordinates should match",PT2.getY().getValue(), 0.0,1e-8);
		ensure_equals("Z-Coordinates should match",PT2.getZ().getValue(), 0.0,1e-8);

		//////////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR ROOT2FPT2(tree,"ROOT","FPT2","ROOT2FPT2");
		TLOR2LOR FPT22ROOT(tree,"FPT2","ROOT","FPT22ROOT");

		TPositionVector STN2 = proj.getPoints().getObject("STN2").getEstimatedValue();

		ROOT2FPT2.transform(STN2);
		ensure_equals("X-Coordinates should match",STN2.getX().getValue(), 200.0,1e-8);
		ensure_equals("Y-Coordinates should match",STN2.getY().getValue(), -100.0,1e-8);
		ensure_equals("Z-Coordinates should match",STN2.getZ().getValue(), 0.0,1e-8);

		FPT22ROOT.transform(STN2);
		ensure_equals("X-Coordinates should match",STN2.getX().getValue(), 100.0,1e-8);
		ensure_equals("Y-Coordinates should match",STN2.getY().getValue(), 100.0,1e-8);
		ensure_equals("Z-Coordinates should match",STN2.getZ().getValue(), 0.0,1e-8);

		//////////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR FREF22ROOT(tree,"FREF2","ROOT","FREF22ROOT");
		TLOR2LOR ROOT2FREF2(tree,"ROOT","FREF2","ROOT2FREF2");

		TPositionVector P4 = proj.getPoints().getObject("P4").getEstimatedValue();

		FREF22ROOT.transform(P4);
		ensure_equals("X-Coordinates should match",P4.getX().getValue(), 100.0,1e-8);
		ensure_equals("Y-Coordinates should match",P4.getY().getValue(), -100.0,1e-8);
		ensure_equals("Z-Coordinates should match",P4.getZ().getValue(), 0.0,1e-8);

		ROOT2FREF2.transform(P4);
		ensure_equals("X-Coordinates should match",P4.getX().getValue(), -100.0,1e-8);
		ensure_equals("Y-Coordinates should match",P4.getY().getValue(), -200.0,1e-8);
		ensure_equals("Z-Coordinates should match",P4.getZ().getValue(), 0.0,1e-8);

		//////////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR FREF2FREF2(tree,"FREF","FREF2","FREF2FREF2");
		TLOR2LOR FREF22FREF(tree,"FREF2","FREF","FREF22FREF");

		TPositionVector REF = proj.getPoints().getObject("REF").getEstimatedValue();

		FREF2FREF2.transform(REF);
		ensure_equals("X-Coordinates should match",REF.getX().getValue(), 0.0,1e-8);
		ensure_equals("Y-Coordinates should match",REF.getY().getValue(), -200.0,1e-8);
		ensure_equals("Z-Coordinates should match",REF.getZ().getValue(), 0.0,1e-8);

		FREF22FREF.transform(REF);
		ensure_equals("X-Coordinates should match",REF.getX().getValue(), 0.0,1e-8);
		ensure_equals("Y-Coordinates should match",REF.getY().getValue(), 0.0,1e-8);
		ensure_equals("Z-Coordinates should match",REF.getZ().getValue(), 0.0,1e-8);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////// Composition which leads to an identity transformation on another tree //////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		TLGCData proj2;
		TReader r2(&proj2);
		stringstream infile2(LGCTestInput::platesfile2);
		r2.read(infile2);

		TDataTree tree2;
		tree2 = proj2.getTree();
		TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
		TPositionVector p(50.0, 60.0, 70.0,k3DCartesian);
		TLOR2LOR transform1(tree2,"P1","P0","new");
		TLOR2LOR transform2(tree2,"P0","P1","new");

		transform1.transform(p);

		ensure_equals("Coordinates should match",p.getX().getValue(), 50.0,1e-8);
		ensure_equals("Coordinates should match",p.getY().getValue(), 60.0,1e-8);
		ensure_equals("Coordinates should match",p.getZ().getValue(), 70.0,1e-8);

		transform2.transform(p);
		ensure_equals("Coordinates should match",p.getX().getValue(), 50.0,1e-8);
		ensure_equals("Coordinates should match",p.getY().getValue(), 60.0,1e-8);
		ensure_equals("Coordinates should match",p.getZ().getValue(), 70.0,1e-8);
	}

	// Test for complicated tree FPT2 to FREF THROUGH FREF2
	template<>
	template<>
	void object::test<3>()
	{
		using namespace LGC;
		set_test_name("Testing Transformations between nodes");

		TLGCData proj;
		TReader r(&proj);
		stringstream infile(LGCTestTransfo::testTransformationsComplex);
		r.read(infile);				
		TDataTree tree = proj.getTree();

		//////////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR trFPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");
		TLOR2LOR trFREFToFPT2(tree,"FREF","FPT2","FREFFPT2");

		const Eigen::Matrix4d& om = trFPT2ToFREF.getLOR2LORTransformation().getMatrix();

		ensure_equals("m element should match",om(0,0),3.05354267671126e-001,1e-8);
		ensure_equals("m element should match",om(0,1),-3.76366445649920e-001,1e-8);
		ensure_equals("m element should match",om(0,2),8.74703989818200e-001,1e-8);
		ensure_equals("m element should match",om(0,3),-2.97494956591710e+003,1e-8);

		ensure_equals("m element should match",om(1,0),-6.29818077180488e-001,1e-8);
		ensure_equals("m element should match",om(1,1),6.09131747045521e-001,1e-8);
		ensure_equals("m element should match",om(1,2),4.81962347489868e-001,1e-8);
		ensure_equals("m element should match",om(1,3),4.78978823789032e+003,1e-8);

		ensure_equals("m element should match",om(2,0),-7.14204425127501e-001,1e-8);
		ensure_equals("m element should match",om(2,1),-6.98073644632225e-001,1e-8);
		ensure_equals("m element should match",om(2,2),-5.10414125801684e-002,1e-8);
		ensure_equals("m element should match",om(2,3),7.02568993272967e+002,1e-8);

		ensure_equals("m element should match",om(3,0),0.0,1e-8);
		ensure_equals("m element should match",om(3,1),0.0,1e-8);
		ensure_equals("m element should match",om(3,2),0.0,1e-8);
		ensure_equals("m element should match",om(3,3),5.02500000000000e-001,1e-8);


		TPositionVector PT2 = proj.getPoints().getObject("PT2").getEstimatedValue();

		trFPT2ToFREF.transform(PT2);

		ensure_equals("X-Coordinates should match",PT2.getX().getValue(), -5918.80943889599348,1e-8);
		ensure_equals("Y-Coordinates should match",PT2.getY().getValue(), 9549.57566059768396,1e-8);
		ensure_equals("Z-Coordinates should match",PT2.getZ().getValue(), 1412.73892333231424,1e-8);

		trFREFToFPT2.transform(PT2);

		ensure_equals("X-Coordinates should match",PT2.getX().getValue(), -1.05971319999987e+001,1e-8);
		ensure_equals("Y-Coordinates should match",PT2.getY().getValue(), 5.19800000031410e-003,1e-8);
		ensure_equals("Z-Coordinates should match",PT2.getZ().getValue(), 4.55657999999895e+000,1e-8);
	}


		template<>
		template<>
		void object::test<4>()
		{
			using namespace LGC;
			set_test_name("Testing partial derivatives with respect to point coordinates");

			TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
			TPositionVector p(50, 60, 70,k3DCartesian);

			TLGCData proj;
			TReader r(&proj);
			stringstream infile(LGCTestTransfo::testTransformationsWithScale);
			r.read(infile);				
			TDataTree tree = proj.getTree();

//////////////////////////////////////////////////////////////////////////////////////////////////////////		
			TLOR2LOR FREF22ROOT(tree,"FREF2","ROOT","FREF22ROOT");
			TFreeVector pDX0 = FREF22ROOT.partDerivWRespToX0();
			ensure_equals("Partial deriv wr2 X0 should match",pDX0.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0.getY().getValue(),2,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 X0 should match",FREF22ROOT.partDerivWRespToX0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",FREF22ROOT.partDerivWRespToX0(1),2,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",FREF22ROOT.partDerivWRespToX0(2), 0,1e-8);

			TFreeVector pDY0 = FREF22ROOT.partDerivWRespToY0();
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0.getX().getValue(), -2,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 Y0 should match",FREF22ROOT.partDerivWRespToY0(0), -2,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",FREF22ROOT.partDerivWRespToY0(1), 0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",FREF22ROOT.partDerivWRespToY0(2), 0,1e-8);

			TFreeVector pDZ0 = FREF22ROOT.partDerivWRespToZ0();
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0.getZ().getValue(), 2,1e-8);

			ensure_equals("Partial deriv wr2 Z0 should match",FREF22ROOT.partDerivWRespToZ0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",FREF22ROOT.partDerivWRespToZ0(1), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",FREF22ROOT.partDerivWRespToZ0(2), 2,1e-8);

//////////////////////////////////////////////////////////////////////////////////////////////////////////	
			TLOR2LOR ROOT2FREF2(tree,"ROOT","FREF2","ROOT2FREF2");
			TFreeVector pDX0RF = ROOT2FREF2.partDerivWRespToX0();
			ensure_equals("Partial deriv wr2 X0 should match",pDX0RF.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0RF.getY().getValue(),-0.5,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0RF.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 X0 should match",ROOT2FREF2.partDerivWRespToX0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",ROOT2FREF2.partDerivWRespToX0(1),-0.5,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",ROOT2FREF2.partDerivWRespToX0(2), 0,1e-8);

			TFreeVector pDY0RF = ROOT2FREF2.partDerivWRespToY0();
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0RF.getX().getValue(), 0.5,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0RF.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0RF.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 Y0 should match",ROOT2FREF2.partDerivWRespToY0(0), 0.5,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",ROOT2FREF2.partDerivWRespToY0(1), 0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",ROOT2FREF2.partDerivWRespToY0(2), 0,1e-8);

			TFreeVector pDZ0RF = ROOT2FREF2.partDerivWRespToZ0();
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0RF.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0RF.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0RF.getZ().getValue(), 0.5,1e-8);

			ensure_equals("Partial deriv wr2 Z0 should match",ROOT2FREF2.partDerivWRespToZ0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",ROOT2FREF2.partDerivWRespToZ0(1), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",ROOT2FREF2.partDerivWRespToZ0(2), 0.5,1e-8);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
			TLOR2LOR FPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");
			
			TFreeVector pDX0FPTF = FPT2ToFREF.partDerivWRespToX0();
			ensure_equals("Partial deriv wr2 X0 should match",pDX0FPTF.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0FPTF.getY().getValue(),0.5,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0FPTF.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 X0 should match",FPT2ToFREF.partDerivWRespToX0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",FPT2ToFREF.partDerivWRespToX0(1), 0.5,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",FPT2ToFREF.partDerivWRespToX0(2), 0,1e-8);

			TFreeVector pDY0FPTF = FPT2ToFREF.partDerivWRespToY0();
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0FPTF.getX().getValue(), -0.5,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0FPTF.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0FPTF.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 Y0 should match",FPT2ToFREF.partDerivWRespToY0(0), -0.5,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",FPT2ToFREF.partDerivWRespToY0(1), 0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",FPT2ToFREF.partDerivWRespToY0(2), 0,1e-8);
			
			TFreeVector pDZ0FFPT = FPT2ToFREF.partDerivWRespToZ0();
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0FFPT.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0FFPT.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0FFPT.getZ().getValue(), 0.5,1e-8);

			ensure_equals("Partial deriv wr2 Z0 should match",FPT2ToFREF.partDerivWRespToZ0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",FPT2ToFREF.partDerivWRespToZ0(1),0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",FPT2ToFREF.partDerivWRespToZ0(2), 0.5,1e-8);
//////////////////////////////////////////////////////////////////////////////////////////////////////////		
			TLOR2LOR FREFToFPT2(tree,"FREF","FPT2","FREFFPT2");

			TFreeVector pDX0FFPT2 = FREFToFPT2.partDerivWRespToX0();
			ensure_equals("Partial deriv wr2 X0 should match",pDX0FFPT2.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0FFPT2.getY().getValue(),-2,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",pDX0FFPT2.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 X0 should match",FREFToFPT2.partDerivWRespToX0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",FREFToFPT2.partDerivWRespToX0(1), -2,1e-8);
			ensure_equals("Partial deriv wr2 X0 should match",FREFToFPT2.partDerivWRespToX0(2), 0,1e-8);

			TFreeVector pDY0FFPT2  = FREFToFPT2.partDerivWRespToY0();
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0FFPT2.getX().getValue(), 2,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0FFPT2.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",pDY0FFPT2.getZ().getValue(), 0,1e-8);

			ensure_equals("Partial deriv wr2 Y0 should match",FREFToFPT2.partDerivWRespToY0(0), 2,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",FREFToFPT2.partDerivWRespToY0(1), 0,1e-8);
			ensure_equals("Partial deriv wr2 Y0 should match",FREFToFPT2.partDerivWRespToY0(2), 0,1e-8);
			
			TFreeVector pDZ0FFPT2  = FREFToFPT2.partDerivWRespToZ0();
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0FFPT2.getX().getValue(), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0FFPT2.getY().getValue(),0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",pDZ0FFPT2.getZ().getValue(), 2,1e-8);

			ensure_equals("Partial deriv wr2 Z0 should match",FREFToFPT2.partDerivWRespToZ0(0), 0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",FREFToFPT2.partDerivWRespToZ0(1),0,1e-8);
			ensure_equals("Partial deriv wr2 Z0 should match",FREFToFPT2.partDerivWRespToZ0(2), 2,1e-8);

		}



		template<>
		template<>
		void object::test<5>()
		{
			using namespace LGC;
			set_test_name("Testing partial derivatives with respect to translations");

			TLGCData proj;
			TReader r(&proj);
			stringstream infile(LGCTestTransfo::testTransformations);
			r.read(infile);				
			TDataTree tree = proj.getTree();
	
			TLOR2LOR FPT2ToROOT(tree,"FREF2","ROOT","FREF22ROOT");
			TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
			TPositionVector p(50, 60, 70,k3DCartesian);


			TFreeVector pd = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 0);
			ensure_equals("PD (0) should match",pd.getX().getValue(), 1,1e-8);
			ensure_equals("PD (1) should match",pd.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pd.getZ().getValue(), 0,1e-8);

			TFreeVector pd2 = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 1);
			ensure_equals("PD (0) should match",pd2.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pd2.getY().getValue(),1,1e-8);
			ensure_equals("PD (2) should match",pd2.getZ().getValue(), 0,1e-8);


			TFreeVector pd3 = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 2);
			ensure_equals("PD (0) should match",pd3.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pd3.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pd3.getZ().getValue(), 1,1e-8);
		}

		template<>
		template<>
		void object::test<6>()
		{
#if 0
			using namespace LGC;
			set_test_name("Testing partial derivatives with respect to translations");

			//Scale does not have affect on partial derivative with respect to translation
			TLGCData proj;
			TReader r(&proj);
			stringstream infile(LGCTestTransfo::testTransformationsWithScale);
			r.read(infile);				
			TDataTree tree = proj.tree;
	
			TLOR2LOR FPT2ToROOT(tree,"FREF2","ROOT","FREF22ROOT");
			TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
			TPositionVector p(50, 60, 70,k3DCartesian);


			TFreeVector pd = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 0);
			ensure_equals("PD (0) should match",pd.getX().getValue(), 1,1e-8);
			ensure_equals("PD (1) should match",pd.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pd.getZ().getValue(), 0,1e-8);

			TFreeVector pd2 = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 1);
			ensure_equals("PD (0) should match",pd2.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pd2.getY().getValue(),1,1e-8);
			ensure_equals("PD (2) should match",pd2.getZ().getValue(), 0,1e-8);


			TFreeVector pd3 = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 2);
			ensure_equals("PD (0) should match",pd3.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pd3.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pd3.getZ().getValue(), 1,1e-8);


			TLOR2LOR FPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");

			TFreeVector pdS = FPT2ToFREF.partialDerivativesTranslation("FREF2", p, 0);
			ensure_equals("PD (0) should match",pdS.getX().getValue(), -6.1875,1e-8);
			ensure_equals("PD (1) should match",pdS.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pdS.getZ().getValue(), 0,1e-8);

			TFreeVector pd2S = FPT2ToFREF.partialDerivativesTranslation("FREF2", p, 1);
			ensure_equals("PD (0) should match",pd2S.getX().getValue(), -6.25,1e-8);
			ensure_equals("PD (1) should match",pd2S.getY().getValue(),0.0625,1e-8);
			ensure_equals("PD (2) should match",pd2S.getZ().getValue(), 0,1e-8);


			TFreeVector pd3S = FPT2ToFREF.partialDerivativesTranslation("FREF2", p, 2);
			ensure_equals("PD (0) should match",pd3S.getX().getValue(), -6.25,1e-8);
			ensure_equals("PD (1) should match",pd3S.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pd3S.getZ().getValue(), 0.0625,1e-8);
#endif
		}

#if 0
		template<>
		template<>
		void object::test<4>()
		{
			using namespace LGC;
			set_test_name("Testing partial derivatives with respect to translations");

			TLGCData proj;
			TReader r(&proj);
			stringstream infile(LGCTestTransfo::testTransformations);
			r.read(infile);				
			TDataTree tree = proj.tree;
	
			TLOR2LOR FPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");
			TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
			TPositionVector p(50, 60, 70,k3DCartesian);


			TFreeVector pd = FPT2ToFREF.partialDerivativesTranslation("FREF2", p, 0);
			ensure_equals("PD (0) should match",pd.getX().getValue(), 1,1e-8);
			ensure_equals("PD (1) should match",pd.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pd.getZ().getValue(), 0,1e-8);

			TFreeVector pd2 = FPT2ToFREF.partialDerivativesTranslation("FREF2", p, 1);
			ensure_equals("PD (0) should match",pd2.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pd2.getY().getValue(),1,1e-8);
			ensure_equals("PD (2) should match",pd2.getZ().getValue(), 0,1e-8);


			TFreeVector pd3 = FPT2ToFREF.partialDerivativesTranslation("FREF2", p, 2);
			ensure_equals("PD (0) should match",pd3.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pd3.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pd3.getZ().getValue(), 1,1e-8);


			//Testing with scales included
		}

		template<>
		template<>
		void object::test<5>()
		{
			using namespace LGC;
			set_test_name("Testing partial derivatives with respect to angles");

			TLGCData proj;
			TReader r(&proj);
			stringstream infile(LGCTestTransfo::testTransformations);
			r.read(infile);				
			TDataTree tree = proj.tree;

//////////////////////////////////////////////////////////////////////////////////////////////////////////		
			TLOR2LOR FPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");
			TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
			TPositionVector p(50, 60, 70,k3DCartesian);


			TFreeVector pdOmega = FPT2ToFREF.partialDerivativesAngle("FREF2", p, 0);
			ensure_equals("PD (0) should match",pdOmega.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pdOmega.getY().getValue(),70,1e-8);
			ensure_equals("PD (2) should match",pdOmega.getZ().getValue(), 50,1e-8);
			
			TFreeVector pdPhi = FPT2ToFREF.partialDerivativesAngle("FREF2", p, 1);
			ensure_equals("PD (0) should match",pdPhi.getX().getValue(), -70,1e-8);
			ensure_equals("PD (1) should match",pdPhi.getY().getValue(),0,1e-8);
			ensure_equals("PD (2) should match",pdPhi.getZ().getValue(), 40,1e-8);


			TFreeVector pdKappa = FPT2ToFREF.partialDerivativesAngle("FREF2", p, 2);
			ensure_equals("PD (0) should match",pdKappa.getX().getValue(), -50,1e-8);
			ensure_equals("PD (1) should match",pdKappa.getY().getValue(),-40,1e-8);
			ensure_equals("PD (2) should match",pdKappa.getZ().getValue(), 0,1e-8);

			//Testing with scales included
		}

		template<>
		template<>
		void object::test<6>()
		{
			using namespace LGC;
			set_test_name("Testing partial derivatives with respect to scale");

			TLGCData proj;
			TReader r(&proj);
			stringstream infile(LGCTestTransfo::testTransformationsWithScale);
			r.read(infile);				
			TDataTree tree = proj.tree;

//////////////////////////////////////////////////////////////////////////////////////////////////////////		
			TLOR2LOR FPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");
			TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
			TPositionVector p(50, 60, 70,k3DCartesian);

/*
			TFreeVector pdScale = FPT2ToFREF.partialDerivativesScale("FREF2", p);
			ensure_equals("PD (0) should match",pdScale.getX().getValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pdScale.getY().getValue(),70,1e-8);
			ensure_equals("PD (2) should match",pdScale.getZ().getValue(), 50,1e-8);
			*/
		}
#endif
}