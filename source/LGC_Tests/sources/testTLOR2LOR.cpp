// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "testTLOR2LOR.h"

#include <tut/tut.hpp>

#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>
#include "TLGCCalculation.h"
#include "Logger.hpp"

#include "TDirectTransformation.h"
#include "TInverseTransformation.h"

#pragma warning(disable : 4224)

using namespace std;

namespace tut
{
struct tests_TLOR2LOR
{
};
typedef test_group<tests_TLOR2LOR> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Testing of TLOR2LOR class");
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

	std::shared_ptr<TLGCData> proj(new TLGCData);
	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::plateFileOrig);
	r.read(infile);

	TDataTree tree;
	tree = proj->getTree();

	// Tree structure read from input2.h file
	//         ROOT (1)
	//        /    |    \
		//    P0     P1    P2
	//      / \     \        
		// P3    P4    P5
	//   |
	//   P6

	////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////// Testing constructor out of names ///////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR transform1(tree, "P1", "P0", "new");
	TDataTreeIter from = transform1.getFromNode();
	TDataTreeIter to = transform1.getToNode();

	ensure_equals("Names of the FROM transformation must match", from.node->data->frame.getName(), "P1");
	ensure_equals("Names of the TO transformation must match", to.node->data->frame.getName(), "P0");
	ensure_equals("Names should match", transform1.getName(), "new");

	const std::vector<TLOR2LOR::TransformAndParams> &tCh = transform1.getTransformationChain();
	ensure_equals("Size of transformation chain should match", tCh.size(), 2);

	ensure_equals("First transformation type should match", tCh[0].direct, true);
	ensure_equals("Second transformation type should match", tCh[1].direct, false);

	ensure_equals("First transformation type should match", tCh[0].adjTrafo->getName(), "P1");
	ensure_equals("Second transformation type should match", tCh[1].adjTrafo->getName(), "P0");

	const Eigen::Matrix4d &mm = tCh[0].trafo->getMatrix();
	const Eigen::Matrix4d &mm2 = tCh[1].trafo->getMatrix();

	TDirectTransformation dTM;
	auto GON(TAngle::kGons);
	TransformParameters param;
	param.omega = TAngle(38.19718634205, GON);
	param.phi = TAngle(31.83098861838, GON);
	param.kappa = TAngle(25.4647908947, GON);

	param.tX = TLength(80);
	param.tY = TLength(90);
	param.tZ = TLength(100);

	dTM.setTransformParam(param);
	ensure_equals("Matrix in chain should match", mm, dTM.getMatrix());

	TInverseTransformation dTMI;
	dTMI.setTransformParam(param);
	ensure_equals("Matrix in chain should match", mm2, dTMI.getMatrix());

	////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////// Testing constructor out of Iterators /////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR transform2(from, to, "new");
	TDataTreeIter fromI = transform2.getFromNode();
	TDataTreeIter toI = transform2.getToNode();

	ensure_equals("Transformation matrix should match", fromI.node, from.node);
	ensure_equals("Transformation matrix should match", toI.node, to.node);

	ensure_equals("Names of the FROM transformation must match", fromI.node->data->frame.getName(), from.node->data->frame.getName());
	ensure_equals("Names of the TO transformation must match", toI.node->data->frame.getName(), to.node->data->frame.getName());
	ensure_equals("Names should match", transform2.getName(), transform1.getName());

	const std::vector<TLOR2LOR::TransformAndParams> &tChI = transform2.getTransformationChain();
	ensure_equals("Size of transformation chain should match", tChI.size(), tCh.size());

	ensure_equals("First transformation type should match", tChI[0].direct, tCh[0].direct);
	ensure_equals("Second transformation type should match", tChI[1].direct, tCh[1].direct);

	ensure_equals("First transformation type should match", tChI[0].adjTrafo->getName(), tCh[0].adjTrafo->getName());
	ensure_equals("Second transformation type should match", tChI[1].adjTrafo->getName(), tCh[1].adjTrafo->getName());

	const Eigen::Matrix4d &mmI = tChI[0].trafo->getMatrix();
	const Eigen::Matrix4d &mm2I = tChI[1].trafo->getMatrix();

	ensure_equals("Transformation matrix should match", mmI, mm);
	ensure_equals("Transformation matrix should match", mm2I, mm2);

	////////////////////////////////////////////////////////////////////////////
	///////////////////////// Testing copy constructor /////////////////////////
	////////////////////////////////////////////////////////////////////////////
	TLOR2LOR transform1Copy(transform1);
	TDataTreeIter fromC = transform1Copy.getFromNode();
	TDataTreeIter toC = transform1Copy.getToNode();

	ensure_equals("Transformation matrix should match", fromC.node, from.node);
	ensure_equals("Transformation matrix should match", toC.node, to.node);

	ensure_equals("Names of the FROM transformation must match", fromC.node->data->frame.getName(), from.node->data->frame.getName());
	ensure_equals("Names of the TO transformation must match", toC.node->data->frame.getName(), to.node->data->frame.getName());
	ensure_equals("Names should match", transform1Copy.getName(), transform1.getName());

	const std::vector<TLOR2LOR::TransformAndParams> &tCh2 = transform1Copy.getTransformationChain();
	ensure_equals("Size of transformation chain should match", tCh2.size(), tCh.size());

	ensure_equals("First transformation type should match", tCh2[0].direct, tCh[0].direct);
	ensure_equals("Second transformation type should match", tCh2[1].direct, tCh[1].direct);

	ensure_equals("First transformation type should match", tCh2[0].adjTrafo->getName(), tCh[0].adjTrafo->getName());
	ensure_equals("Second transformation type should match", tCh2[1].adjTrafo->getName(), tCh[1].adjTrafo->getName());

	const Eigen::Matrix4d &mmC = tCh2[0].trafo->getMatrix();
	const Eigen::Matrix4d &mm2C = tCh[1].trafo->getMatrix();

	ensure_equals("Transformation matrix should match", mmC, mm);
	ensure_equals("Transformation matrix should match", mm2C, mm2);
}

// Testing TLOR2LOR transformations between nodes
template<>
template<>
void object::test<2>()
{
	using namespace LGC;
	set_test_name("Testing Transformations between nodes");

	std::shared_ptr<TLGCData> proj(new TLGCData);
	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::test1);
	r.read(infile);
	TDataTree tree = proj->getTree();

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR trFPT2ToFREF(tree, "FPT2", "FREF", "FPT2FREF");
	TLOR2LOR trFREFToFPT2(tree, "FREF", "FPT2", "FREFFPT2");

	const Eigen::Matrix4d &om = trFPT2ToFREF.getLOR2LORTransformation().getMatrix();

	ensure_equals("m element should match", om(0, 0), 0.0, 1e-8);
	ensure_equals("m element should match", om(0, 1), -1.0, 1e-8);
	ensure_equals("m element should match", om(0, 2), 0.0, 1e-8);
	ensure_equals("m element should match", om(0, 3), -100.0, 1e-8);

	ensure_equals("m element should match", om(1, 0), 1.0, 1e-8);
	ensure_equals("m element should match", om(1, 1), 0.0, 1e-8);
	ensure_equals("m element should match", om(1, 2), 0.0, 1e-8);
	ensure_equals("m element should match", om(1, 3), -100.0, 1e-8);

	ensure_equals("m element should match", om(2, 0), 0.0, 1e-8);
	ensure_equals("m element should match", om(2, 1), 0.0, 1e-8);
	ensure_equals("m element should match", om(2, 2), 1.0, 1e-8);
	ensure_equals("m element should match", om(2, 3), 0.0, 1e-8);

	ensure_equals("m element should match", om(3, 0), 0.0, 1e-8);
	ensure_equals("m element should match", om(3, 1), 0.0, 1e-8);
	ensure_equals("m element should match", om(3, 2), 0.0, 1e-8);
	ensure_equals("m element should match", om(3, 3), 1.0, 1e-8);

	TPositionVector PT2 = proj->getPoints().getObject("PT2").getEstimatedValue();

	trFPT2ToFREF.transform(PT2);

	ensure_equals("X-Coordinates should match", PT2.getX().getMetresValue(), -100.0, 1e-8);
	ensure_equals("Y-Coordinates should match", PT2.getY().getMetresValue(), -100.0, 1e-8);
	ensure_equals("Z-Coordinates should match", PT2.getZ().getMetresValue(), 0.0, 1e-8);

	trFREFToFPT2.transform(PT2);

	ensure_equals("X-Coordinates should match", PT2.getX().getMetresValue(), 0.0, 1e-8);
	ensure_equals("Y-Coordinates should match", PT2.getY().getMetresValue(), 0.0, 1e-8);
	ensure_equals("Z-Coordinates should match", PT2.getZ().getMetresValue(), 0.0, 1e-8);

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR ROOT2FPT2(tree, "ROOT", "FPT2", "ROOT2FPT2");
	TLOR2LOR FPT22ROOT(tree, "FPT2", "ROOT", "FPT22ROOT");

	TPositionVector STN2 = proj->getPoints().getObject("STN2").getEstimatedValue();

	ROOT2FPT2.transform(STN2);
	ensure_equals("X-Coordinates should match", STN2.getX().getMetresValue(), 200.0, 1e-8);
	ensure_equals("Y-Coordinates should match", STN2.getY().getMetresValue(), -100.0, 1e-8);
	ensure_equals("Z-Coordinates should match", STN2.getZ().getMetresValue(), 0.0, 1e-8);

	FPT22ROOT.transform(STN2);
	ensure_equals("X-Coordinates should match", STN2.getX().getMetresValue(), 100.0, 1e-8);
	ensure_equals("Y-Coordinates should match", STN2.getY().getMetresValue(), 100.0, 1e-8);
	ensure_equals("Z-Coordinates should match", STN2.getZ().getMetresValue(), 0.0, 1e-8);

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR FREF22ROOT(tree, "FREF2", "ROOT", "FREF22ROOT");
	TLOR2LOR ROOT2FREF2(tree, "ROOT", "FREF2", "ROOT2FREF2");

	TPositionVector P4 = proj->getPoints().getObject("P4").getEstimatedValue();

	FREF22ROOT.transform(P4);
	ensure_equals("X-Coordinates should match", P4.getX().getMetresValue(), 100.0, 1e-8);
	ensure_equals("Y-Coordinates should match", P4.getY().getMetresValue(), -100.0, 1e-8);
	ensure_equals("Z-Coordinates should match", P4.getZ().getMetresValue(), 0.0, 1e-8);

	ROOT2FREF2.transform(P4);
	ensure_equals("X-Coordinates should match", P4.getX().getMetresValue(), -100.0, 1e-8);
	ensure_equals("Y-Coordinates should match", P4.getY().getMetresValue(), -200.0, 1e-8);
	ensure_equals("Z-Coordinates should match", P4.getZ().getMetresValue(), 0.0, 1e-8);

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR FREF2FREF2(tree, "FREF", "FREF2", "FREF2FREF2");
	TLOR2LOR FREF22FREF(tree, "FREF2", "FREF", "FREF22FREF");

	TPositionVector REF = proj->getPoints().getObject("REF").getEstimatedValue();

	FREF2FREF2.transform(REF);
	ensure_equals("X-Coordinates should match", REF.getX().getMetresValue(), 0.0, 1e-8);
	ensure_equals("Y-Coordinates should match", REF.getY().getMetresValue(), -200.0, 1e-8);
	ensure_equals("Z-Coordinates should match", REF.getZ().getMetresValue(), 0.0, 1e-8);

	FREF22FREF.transform(REF);
	ensure_equals("X-Coordinates should match", REF.getX().getMetresValue(), 0.0, 1e-8);
	ensure_equals("Y-Coordinates should match", REF.getY().getMetresValue(), 0.0, 1e-8);
	ensure_equals("Z-Coordinates should match", REF.getZ().getMetresValue(), 0.0, 1e-8);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////// Composition which leads to an identity transformation on another tree //////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::shared_ptr<TLGCData> proj2(new TLGCData);
	;
	TReader r2(proj2);
	stringstream infile2(LOR2LORInputFiles::plateFileOrig);
	r2.read(infile2);

	TDataTree tree2;
	tree2 = proj2->getTree();
	TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector p(50.0, 60.0, 70.0, k3DCartesian);
	TLOR2LOR transform1(tree2, "P1", "P0", "new");
	TLOR2LOR transform2(tree2, "P0", "P1", "new");

	transform1.transform(p);

	ensure_equals("Coordinates should match", p.getX().getMetresValue(), 50.0, 1e-8);
	ensure_equals("Coordinates should match", p.getY().getMetresValue(), 60.0, 1e-8);
	ensure_equals("Coordinates should match", p.getZ().getMetresValue(), 70.0, 1e-8);

	transform2.transform(p);
	ensure_equals("Coordinates should match", p.getX().getMetresValue(), 50.0, 1e-8);
	ensure_equals("Coordinates should match", p.getY().getMetresValue(), 60.0, 1e-8);
	ensure_equals("Coordinates should match", p.getZ().getMetresValue(), 70.0, 1e-8);
}

// Test for complicated tree FPT2 to FREF THROUGH FREF2
template<>
template<>
void object::test<3>()
{
	using namespace LGC;
	set_test_name("Testing Transformations between nodes");

	std::shared_ptr<TLGCData> proj(new TLGCData);

	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::test2);
	r.read(infile);
	TDataTree tree = proj->getTree();

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR trFPT2ToFREF(tree, "FPT2", "FREF", "FPT2FREF");
	TLOR2LOR trFREFToFPT2(tree, "FREF", "FPT2", "FREFFPT2");

	const Eigen::Matrix4d &om = trFPT2ToFREF.getLOR2LORTransformation().getMatrix();

	ensure_equals("m element should match", om(0, 0), 3.05354267671126e-001, 1e-8);
	ensure_equals("m element should match", om(0, 1), -3.76366445649920e-001, 1e-8);
	ensure_equals("m element should match", om(0, 2), 8.74703989818200e-001, 1e-8);
	ensure_equals("m element should match", om(0, 3), -2.97494956591710e+003, 1e-8);

	ensure_equals("m element should match", om(1, 0), -6.29818077180488e-001, 1e-8);
	ensure_equals("m element should match", om(1, 1), 6.09131747045521e-001, 1e-8);
	ensure_equals("m element should match", om(1, 2), 4.81962347489868e-001, 1e-8);
	ensure_equals("m element should match", om(1, 3), 4.78978823789032e+003, 1e-8);

	ensure_equals("m element should match", om(2, 0), -7.14204425127501e-001, 1e-8);
	ensure_equals("m element should match", om(2, 1), -6.98073644632225e-001, 1e-8);
	ensure_equals("m element should match", om(2, 2), -5.10414125801684e-002, 1e-8);
	ensure_equals("m element should match", om(2, 3), 7.02568993272967e+002, 1e-8);

	ensure_equals("m element should match", om(3, 0), 0.0, 1e-8);
	ensure_equals("m element should match", om(3, 1), 0.0, 1e-8);
	ensure_equals("m element should match", om(3, 2), 0.0, 1e-8);
	ensure_equals("m element should match", om(3, 3), 5.02500000000000e-001, 1e-8);

	TPositionVector PT2 = proj->getPoints().getObject("PT2").getEstimatedValue();

	trFPT2ToFREF.transform(PT2);

	ensure_equals("X-Coordinates should match", PT2.getX().getMetresValue(), -5918.80943889599348, 1e-8);
	ensure_equals("Y-Coordinates should match", PT2.getY().getMetresValue(), 9549.57566059768396, 1e-8);
	ensure_equals("Z-Coordinates should match", PT2.getZ().getMetresValue(), 1412.73892333231424, 1e-8);

	trFREFToFPT2.transform(PT2);

	ensure_equals("X-Coordinates should match", PT2.getX().getMetresValue(), -1.05971319999987e+001, 1e-8);
	ensure_equals("Y-Coordinates should match", PT2.getY().getMetresValue(), 5.19800000031410e-003, 1e-8);
	ensure_equals("Z-Coordinates should match", PT2.getZ().getMetresValue(), 4.55657999999895e+000, 1e-8);
}

template<>
template<>
void object::test<4>()
{
	using namespace LGC;
	set_test_name("Testing partial derivatives with respect to point coordinates");

	TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector p(50, 60, 70, k3DCartesian);

	std::shared_ptr<TLGCData> proj(new TLGCData);
	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::test_with_scale);
	r.read(infile);
	TDataTree tree = proj->getTree();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR FREF22ROOT(tree, "FREF2", "ROOT", "FREF22ROOT");
	TFreeVector pDX0 = FREF22ROOT.partDerivWRespToX0();
	ensure_equals("Partial deriv wr2 X0 should match", pDX0.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0.getY().getMetresValue(), 2, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 X0 should match", FREF22ROOT.partDerivWRespToX0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", FREF22ROOT.partDerivWRespToX0(1), 2, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", FREF22ROOT.partDerivWRespToX0(2), 0, 1e-8);

	TFreeVector pDY0 = FREF22ROOT.partDerivWRespToY0();
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0.getX().getMetresValue(), -2, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 Y0 should match", FREF22ROOT.partDerivWRespToY0(0), -2, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", FREF22ROOT.partDerivWRespToY0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", FREF22ROOT.partDerivWRespToY0(2), 0, 1e-8);

	TFreeVector pDZ0 = FREF22ROOT.partDerivWRespToZ0();
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0.getZ().getMetresValue(), 2, 1e-8);

	ensure_equals("Partial deriv wr2 Z0 should match", FREF22ROOT.partDerivWRespToZ0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", FREF22ROOT.partDerivWRespToZ0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", FREF22ROOT.partDerivWRespToZ0(2), 2, 1e-8);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR ROOT2FREF2(tree, "ROOT", "FREF2", "ROOT2FREF2");
	TFreeVector pDX0RF = ROOT2FREF2.partDerivWRespToX0();
	ensure_equals("Partial deriv wr2 X0 should match", pDX0RF.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0RF.getY().getMetresValue(), -0.5, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0RF.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 X0 should match", ROOT2FREF2.partDerivWRespToX0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", ROOT2FREF2.partDerivWRespToX0(1), -0.5, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", ROOT2FREF2.partDerivWRespToX0(2), 0, 1e-8);

	TFreeVector pDY0RF = ROOT2FREF2.partDerivWRespToY0();
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0RF.getX().getMetresValue(), 0.5, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0RF.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0RF.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 Y0 should match", ROOT2FREF2.partDerivWRespToY0(0), 0.5, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", ROOT2FREF2.partDerivWRespToY0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", ROOT2FREF2.partDerivWRespToY0(2), 0, 1e-8);

	TFreeVector pDZ0RF = ROOT2FREF2.partDerivWRespToZ0();
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0RF.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0RF.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0RF.getZ().getMetresValue(), 0.5, 1e-8);

	ensure_equals("Partial deriv wr2 Z0 should match", ROOT2FREF2.partDerivWRespToZ0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", ROOT2FREF2.partDerivWRespToZ0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", ROOT2FREF2.partDerivWRespToZ0(2), 0.5, 1e-8);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR FPT2ToFREF(tree, "FPT2", "FREF", "FPT2FREF");

	TFreeVector pDX0FPTF = FPT2ToFREF.partDerivWRespToX0();
	ensure_equals("Partial deriv wr2 X0 should match", pDX0FPTF.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0FPTF.getY().getMetresValue(), 0.5, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0FPTF.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 X0 should match", FPT2ToFREF.partDerivWRespToX0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", FPT2ToFREF.partDerivWRespToX0(1), 0.5, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", FPT2ToFREF.partDerivWRespToX0(2), 0, 1e-8);

	TFreeVector pDY0FPTF = FPT2ToFREF.partDerivWRespToY0();
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0FPTF.getX().getMetresValue(), -0.5, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0FPTF.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0FPTF.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 Y0 should match", FPT2ToFREF.partDerivWRespToY0(0), -0.5, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", FPT2ToFREF.partDerivWRespToY0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", FPT2ToFREF.partDerivWRespToY0(2), 0, 1e-8);

	TFreeVector pDZ0FFPT = FPT2ToFREF.partDerivWRespToZ0();
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0FFPT.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0FFPT.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0FFPT.getZ().getMetresValue(), 0.5, 1e-8);

	ensure_equals("Partial deriv wr2 Z0 should match", FPT2ToFREF.partDerivWRespToZ0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", FPT2ToFREF.partDerivWRespToZ0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", FPT2ToFREF.partDerivWRespToZ0(2), 0.5, 1e-8);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR FREFToFPT2(tree, "FREF", "FPT2", "FREFFPT2");

	TFreeVector pDX0FFPT2 = FREFToFPT2.partDerivWRespToX0();
	ensure_equals("Partial deriv wr2 X0 should match", pDX0FFPT2.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0FFPT2.getY().getMetresValue(), -2, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", pDX0FFPT2.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 X0 should match", FREFToFPT2.partDerivWRespToX0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", FREFToFPT2.partDerivWRespToX0(1), -2, 1e-8);
	ensure_equals("Partial deriv wr2 X0 should match", FREFToFPT2.partDerivWRespToX0(2), 0, 1e-8);

	TFreeVector pDY0FFPT2 = FREFToFPT2.partDerivWRespToY0();
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0FFPT2.getX().getMetresValue(), 2, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0FFPT2.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", pDY0FFPT2.getZ().getMetresValue(), 0, 1e-8);

	ensure_equals("Partial deriv wr2 Y0 should match", FREFToFPT2.partDerivWRespToY0(0), 2, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", FREFToFPT2.partDerivWRespToY0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Y0 should match", FREFToFPT2.partDerivWRespToY0(2), 0, 1e-8);

	TFreeVector pDZ0FFPT2 = FREFToFPT2.partDerivWRespToZ0();
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0FFPT2.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0FFPT2.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", pDZ0FFPT2.getZ().getMetresValue(), 2, 1e-8);

	ensure_equals("Partial deriv wr2 Z0 should match", FREFToFPT2.partDerivWRespToZ0(0), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", FREFToFPT2.partDerivWRespToZ0(1), 0, 1e-8);
	ensure_equals("Partial deriv wr2 Z0 should match", FREFToFPT2.partDerivWRespToZ0(2), 2, 1e-8);
}

template<>
template<>
void object::test<5>()
{
	using namespace LGC;
	set_test_name("Testing partial derivatives with respect to translations");

	std::shared_ptr<TLGCData> proj(new TLGCData);
	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::test1);
	r.read(infile);
	TDataTree tree = proj->getTree();

	TLOR2LOR FPT2ToROOT(tree, "FREF2", "ROOT", "FREF22ROOT");
	TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector p(50, 60, 70, k3DCartesian);

	TFreeVector pd = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 0);
	ensure_equals("PD (0) should match", pd.getX().getMetresValue(), 1, 1e-8);
	ensure_equals("PD (1) should match", pd.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("PD (2) should match", pd.getZ().getMetresValue(), 0, 1e-8);

	TFreeVector pd2 = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 1);
	ensure_equals("PD (0) should match", pd2.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("PD (1) should match", pd2.getY().getMetresValue(), 1, 1e-8);
	ensure_equals("PD (2) should match", pd2.getZ().getMetresValue(), 0, 1e-8);

	TFreeVector pd3 = FPT2ToROOT.partialDerivativesTranslation("FREF2", p, 2);
	ensure_equals("PD (0) should match", pd3.getX().getMetresValue(), 0, 1e-8);
	ensure_equals("PD (1) should match", pd3.getY().getMetresValue(), 0, 1e-8);
	ensure_equals("PD (2) should match", pd3.getZ().getMetresValue(), 1, 1e-8);
}

// Test derivative methods along "long" chain (5 trafos)
template<>
template<>
void object::test<6>()
{
	using namespace LGC;
	set_test_name("Testing derivative methods along Helmert Transformation chain");

	std::shared_ptr<TLGCData> proj(new TLGCData);

	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::plateFileOrig);
	r.read(infile);
	TDataTree tree = proj->getTree();

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR forward(tree, "P6", "P5", "FPT2FREF");
	TLOR2LOR backward(tree, "P5", "P6", "FREFFPT2");

	TPositionVector startPoint(50, 60, 70, TCoordSysFactory::ECoordSys::k3DCartesian);
	TPositionVector endPoint(50, 60, 70, TCoordSysFactory::ECoordSys::k3DCartesian);
	forward.transform(endPoint);

	// Testing chain rule of alpha*beta(=Identity) where beta:(p,x)->(p,forward(p,x)), alpha:(p,x)->(p,backward(p,x)). p are the parameters defining the helmert trafos of the chain
	int numberTrafos = forward.getTransformationChain().size();
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> forwardHelmertSensitivities = forward.getPartialDerivativesWrtHelmertParameters(startPoint);
	std::vector<std::pair<TAdjustableHelmertTransformation, TDenseMatrix>> backwardHelmertSensitivities = backward.getPartialDerivativesWrtHelmertParameters(endPoint);
	TDenseMatrix forwardPointSensitivity = forward.getPartialDerivativeWrtPosition();
	TDenseMatrix backwardPointSensitivity = backward.getPartialDerivativeWrtPosition();
	// prepare derivative of forward and backward with respect to Helmert parameters
	TDenseMatrix dforwardDparameters(3, 7 * numberTrafos);
	dforwardDparameters.setZero();
	TDenseMatrix dbackwardDparameters(3, 7 * numberTrafos);
	dbackwardDparameters.setZero();
	int counter = 0;
	for (auto pair : forwardHelmertSensitivities)
	{
		dforwardDparameters.middleCols(counter * 7, 7) = pair.second;
		counter++;
	}
	counter--;
	// now we go the chain backwards so to ensure same indices we have to start filling at the end
	for (auto pair : backwardHelmertSensitivities)
	{
		dbackwardDparameters.middleCols(counter * 7, 7) = pair.second;
		counter--;
	}

	// compute the derivative of alpha*beta with chainrule
	TDenseMatrix Zero(numberTrafos * 7, numberTrafos * 7);
	Zero.setZero();
	TDenseMatrix Id3(3, 3);
	Id3.setZero();
	// evaluate the chainrule
	Zero = dbackwardDparameters + backwardPointSensitivity * dforwardDparameters;
	Id3 = backwardPointSensitivity * forwardPointSensitivity;

	TDenseMatrix referenceId3(3, 3);
	referenceId3.setIdentity();
	ensure("ChainRule Test along Trafo chain violated.", Zero.isZero());
	ensure("ChainRule Test along Trafo chain violated.", (Id3 - referenceId3).isZero());
}
// Change the value in the partial derivatives with respect to rotations
#if 0
		template<>
		template<>
		void object::test<6>()
		{
			using namespace LGC;
			set_test_name("Testing partial derivatives with respect to rotations");

			TLGCData proj;
			TReader r(&proj);
			stringstream infile(LOR2LORInputFiles::testTransformations);
			r.read(infile);				
			TDataTree tree = proj.tree;

//////////////////////////////////////////////////////////////////////////////////////////////////////////		
			TLOR2LOR FPT2ToFREF(tree,"FPT2","FREF","FPT2FREF");
			TCoordSysFactory::ECoordSys k3DCartesian(TCoordSysFactory::ECoordSys::k3DCartesian);
			TPositionVector p(50, 60, 70,k3DCartesian);


			TFreeVector pdOmega = FPT2ToFREF.partialDerivativesAngle("FREF2", p, 0);
			ensure_equals("PD (0) should match",pdOmega.getX().getMetresValue(), 0,1e-8);
			ensure_equals("PD (1) should match",pdOmega.getY().getMetresValue(),70,1e-8);
			ensure_equals("PD (2) should match",pdOmega.getZ().getMetresValue(), 50,1e-8);
			
			TFreeVector pdPhi = FPT2ToFREF.partialDerivativesAngle("FREF2", p, 1);
			ensure_equals("PD (0) should match",pdPhi.getX().getMetresValue(), -70,1e-8);
			ensure_equals("PD (1) should match",pdPhi.getY().getMetresValue(),0,1e-8);
			ensure_equals("PD (2) should match",pdPhi.getZ().getMetresValue(), 40,1e-8);


			TFreeVector pdKappa = FPT2ToFREF.partialDerivativesAngle("FREF2", p, 2);
			ensure_equals("PD (0) should match",pdKappa.getX().getMetresValue(), -50,1e-8);
			ensure_equals("PD (1) should match",pdKappa.getY().getMetresValue(),-40,1e-8);
			ensure_equals("PD (2) should match",pdKappa.getZ().getMetresValue(), 0,1e-8);

			//Testing with scales included
		}
#endif
		
template<>
template<>
void object::test<7>()
{
	using namespace LGC;
	set_test_name("Testing derivative of helmert parameters representing a chain with only one trafo.");

	std::shared_ptr<TLGCData> proj(new TLGCData);

	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::composedHelmertParameter0);
	bool succesReading = r.read(infile);
	ensure_equals("Reading Successfull.", succesReading, true);
	TDataTree tree = proj->getTree();

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR upTrafo(tree, "Frame1", "ROOT", "up");

	TLGCCalculation calcul(proj);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);


	// the transformation chains need to be updated as the values of the involved adj helmert trafos have changed due to the computation
	upTrafo.updateTree();

	// compare the composed transformation Frame1-> Root with the single trasformation represented by the singleFrame. They should be equal.
	const TLGCData &dataset = calcul.getData();
	TDataTreeIterator singleFrame = dataset.locateNode("Frame1");

	// the frame itself
	TransformParameters singleFrameParams = singleFrame->get()->frame.getEstParam();
	// the trafo resulting from the chain defined by only one frame
	TransformParameters composedFrameParams = upTrafo.computeComposedTransformationParameters();

	// parameters should be approximately equal. Cant use == operator because the parameters here are results of LS iterations, so no perfect match an be expected
	ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.tX, singleFrameParams.tX, 1e-6);
	ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.tY, singleFrameParams.tY, 1e-6);
	ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.tZ, singleFrameParams.tZ, 1e-6);
	ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.kappa, singleFrameParams.kappa, 1e-6);
	ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.omega, singleFrameParams.omega, 1e-6);
	ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.phi, singleFrameParams.phi, 1e-6);
	ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.scale, singleFrameParams.scale, 1e-6);

	// test the derivatives of the composed trafo with respect to the involved helmert parameters
	double epsilon = 1e-6;

	// as the chain is only consisting of one trafo, the resulting parameters are identical with the parameters of this single trafo.
	// therefore the derivatives of the composed trafo parameter with respect to this trafo should be the identity matrix
	std::vector<std::pair<TAdjustableHelmertTransformation, Eigen::Matrix<double, 7, 7>>> derWRTFrames = upTrafo.computeComposedTransformationDerivatives();

	Eigen::MatrixXd identity(7, 7);
	identity.setIdentity();
	Eigen::MatrixXd computedDerivative = derWRTFrames.at(0).second;
	ensure("Derivative of composed Helmert parameter should be the identity as the chain is defined by a single Frame.", identity.isApprox(computedDerivative));

}

template<>
template<>
void object::test<8>()
{
	using namespace LGC;
	set_test_name("Testing derivative of helmert parameters representing a chain of several Helmert transformations.");
	std::vector<const char*> fileStrings;
	// two slightly different files
	fileStrings.push_back(LOR2LORInputFiles::composedHelmertParameter1);
	fileStrings.push_back(LOR2LORInputFiles::composedHelmertParameter2);

	for (const char *fileString : fileStrings)
	{
		std::shared_ptr<TLGCData> proj(new TLGCData);

		TReader r(proj);
		stringstream infile(fileString);
		bool succesReading = r.read(infile);
		ensure_equals("Reading Successfull.", succesReading, true);
		TDataTree tree = proj->getTree();

		//////////////////////////////////////////////////////////////////////////////////////////////
		TLOR2LOR upTrafo(tree, "Frame3", "ROOT", "up");

		TLGCCalculation calcul(proj);
		std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
		Behavior succesCalc = calcul.computeResults(fileWriter);
		ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

		// the transformation chains need to be updated as the values of the involved adj helmert trafos have changed due to the computation
		upTrafo.updateTree();

		// compare the composed transformation Frame3-> Root with the single trasformation represented by the singleFrame. They should be equal.
		const TLGCData &dataset = calcul.getData();
		TDataTreeIterator singleFrame = dataset.locateNode("singleFrame");

		TransformParameters singleFrameParams = singleFrame->get()->frame.getEstParam();
		TransformParameters composedFrameParams = upTrafo.computeComposedTransformationParameters();

		// parameters should be approximately equal. Cant use == operator because the parameters here are results of LS iterations, so no perfect match an be expected
		ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.tX, singleFrameParams.tX, 1e-6);
		ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.tY, singleFrameParams.tY, 1e-6);
		ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.tZ, singleFrameParams.tZ, 1e-6);
		ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.kappa, singleFrameParams.kappa, 1e-6);
		ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.omega, singleFrameParams.omega, 1e-6);
		ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.phi, singleFrameParams.phi, 1e-6);
		ensure_equals("Frame parameters need to be approximately equal.", composedFrameParams.scale, singleFrameParams.scale, 1e-6);

		// computing the covariances of both transformations
		TDenseMatrix singleCovar = singleFrame->get()->frame.getCovar();
		TDenseMatrix composedCovar = upTrafo.getComposedParameterCovariance(&dataset);

		ensure("Frame parameter covariance should be equal for the composed frame and the equivalent single frame.", (composedCovar - singleCovar).norm() < 1e-8);
	}

}

template<>
template<>
void object::test<9>()
{
	using namespace LGC;
	set_test_name("Testing derivative of helmert parameters representing a whole chain");

	std::shared_ptr<TLGCData> proj(new TLGCData);

	TReader r(proj);
	stringstream infile(LOR2LORInputFiles::composedGimbalLock);
	bool succesReading = r.read(infile);
	ensure_equals("Reading Successfull.", succesReading, true);
	TDataTree tree = proj->getTree();

	//////////////////////////////////////////////////////////////////////////////////////////////
	TLOR2LOR upTrafo(tree, "Frame2", "ROOT", "up");

	TLGCCalculation calcul(proj);
	std::shared_ptr<TSimulationOutputFileWriter> fileWriter(nullptr);
	Behavior succesCalc = calcul.computeResults(fileWriter);
	ensure_equals("Calculation successful", succesCalc.code(), Behavior::BehaviorCode::ERR_noError);

	// the transformation chains need to be updated as the values of the involved adj helmert trafos have changed due to the computation
	upTrafo.updateTree();

	// after the computation this composed trafo should produce a gimbal loc ry=100gon
	const TLGCData &dataset = calcul.getData();

	Logger::getLogger().clearCounters();
	int numberWarningsBeforeExtraction = Logger::getLogger().warningNumber();
	ensure_equals("Number of warnings should be reset to 0.", 0, numberWarningsBeforeExtraction);

	// extract the Helmert trafo parameters of the composed transformation
	TransformParameters composedFrameParams = upTrafo.computeComposedTransformationParameters();

	int numberWarningsAfterExtraction = Logger::getLogger().warningNumber();
	ensure_equals("Gimbal lock case should have triggered a warning message.", 1, numberWarningsAfterExtraction);

	ensure_equals("In Gimbal lock case, rz=0 should be applied per convention.", composedFrameParams.kappa, 0, 1e-12);
}


} // namespace tut
