#include <TCCS2CGRFTransformation.h>
#include <TCGRF2LGTransformation.h>
#include <TILG2ILATransformation.h>
#include <TLA2MLATransformation.h>
#include <TXYH2CCS.h>

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


namespace tut
{
    struct test_Transformations{};
    typedef test_group<test_Transformations> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Test of geodetic transformations, between CCS, CGRF, LG and LA");
}

namespace tut
{
	template<>
	template<>
	void object::test<1>()
	{
		using namespace LGC;
		set_test_name("Testing TCCS2CGRFTransformation class");
		TCCS2CGRFTransformation toCGRF(true); //uses ellipsoid

		TPositionVector p(2000.00000, 2097.79265, 2433.66000,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(p);
		ensure_distance("X0 coordinate of P0 in CGRF must match",p.getX().getMetresValue(),4395400.36378173,1e-8);
		ensure_distance("Y0 coordinate of P0 in CGRF must match",p.getY().getMetresValue(),465785.056735627,1e-8);
		ensure_distance("Z0 coordinate of P0 in CGRF must match",p.getZ().getMetresValue(),4583458.22601372,1e-8);
		
		//Inverse should lead to the original point
		toCGRF.transformIverse(p);
		ensure_distance("X0 coordinate of P0 in CCS must match",p.getX().getMetresValue(),2000.00000,1e-8);
		ensure_distance("Y0 coordinate of P0 in CCS must match",p.getY().getMetresValue(), 2097.79265,1e-8);
		ensure_distance("Z0 coordinate of P0 in CCS must match",p.getZ().getMetresValue(),2433.66000,1e-8);

		TPositionVector p2(0.0, 0.0, 0.0,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(p2);
		ensure_distance("X0 coordinate of P1 in CGRF must match",p2.getX().getMetresValue(),4394470.39636727,1e-8);
		ensure_distance("Y0 coordinate of P1 in CGRF must match",p2.getY().getMetresValue(),462839.500477364,1e-8);
		ensure_distance("Z0 coordinate of P1 in CGRF must match",p2.getZ().getMetresValue(),4581271.38035491,1e-8);

		//Inverse transformation should lead to the original point
		toCGRF.transformIverse(p2);
		ensure_distance("X0 coordinate of P1 in CCS must match",p2.getX().getMetresValue(),0.0,1e-8);
		ensure_distance("Y0 coordinate of P1 in CCS must match",p2.getY().getMetresValue(),0.0,1e-8);
		ensure_distance("Z0 coordinate of P1 in CCS must match",p2.getZ().getMetresValue(),0.0,1e-8);

		TPositionVector p3(5.9134567891,1.1234879528,3.0178921478,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(p3);
		ensure_distance("X0 coordinate of P2 in CGRF must match",p3.getX().getMetresValue(),4394473.59536092,1e-8);
		ensure_distance("Y0 coordinate of P2 in CGRF must match",p3.getY().getMetresValue(),462845.401137905,1e-8);
		ensure_distance("Z0 coordinate of P2 in CGRF must match",p3.getZ().getMetresValue(),4581271.91655842,1e-8);


		toCGRF.reInitialize(false); //uses sphere
		TPositionVector p4(2000.00000, 2097.79265, 2433.66000,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(p4);

		ensure_distance("X0 coordinate of P0 in CGRF must match", p4.getX().getMetresValue(), 4382812.29940346, 1e-8);
		ensure_distance("Y0 coordinate of P0 in CGRF must match", p4.getY().getMetresValue(), 464451.086722263, 1e-8);
		ensure_distance("Z0 coordinate of P0 in CGRF must match", p4.getZ().getMetresValue(), 4601131.20981681, 1e-8);

		toCGRF.transformIverse(p4);

		ensure_distance("X0 coordinate of P0 in CCS must match",p4.getX().getMetresValue(), 2000.00000,1e-8);
		ensure_distance("Y0 coordinate of P0 in CCS must match",p4.getY().getMetresValue(), 2097.79265,1e-8);
		ensure_distance("Z0 coordinate of P0 in CCS must match",p4.getZ().getMetresValue(), 2433.66000,1e-8);
	}

	template<>
	template<>
	void object::test<2>()
	{
		using namespace LGC;
		set_test_name("Testing TCGRF2LGTransformation class");

		//Origin of the local geodetic system in the CGRF
		TPositionVector originCCS(4395400.36378173, 465785.056735627, 4583458.22601372,TCoordSysFactory::k3DCartesian);
		TCGRF2LGTransformation toILG(originCCS, false);

		TPositionVector p(4395400.36378173, 465785.056735627, 4583458.22601372,TCoordSysFactory::k3DCartesian);
		toILG.transform(p);
		ensure_distance("X0 coordinate of P0 in ISG must match",p.getX().getMetresValue(),0.0,1e-8);
		ensure_distance("Y0 coordinate of P0 in ISG must match",p.getY().getMetresValue(),0.0,1e-8);
		ensure_distance("Z0 coordinate of P0 in ISG must match",p.getZ().getMetresValue(),0.0,1e-8);

		//Inverse transformation should lead to the original point
		toILG.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in CGRF must match",p.getX().getMetresValue(),4395400.36378173,1e-8);
		ensure_distance("Y0 coordinate of P0 in CGRF must match",p.getY().getMetresValue(),465785.056735627,1e-8);
		ensure_distance("Z0 coordinate of P0 in CGRF must match",p.getZ().getMetresValue(),4583458.22601372,1e-8);

		TPositionVector p2(4394473.59536092, 462845.401137905, 4581271.91655842,TCoordSysFactory::k3DCartesian);
		toILG.transform(p2);
		ensure_distance("X0 coordinate of P2 in CGRF must match",p2.getX().getMetresValue(),-2825.6236229666,1e-8);
		ensure_distance("Y0 coordinate of P2 in CGRF must match",p2.getY().getMetresValue(),-623.100199187882,1e-8);
		ensure_distance("Z0 coordinate of P2 in CGRF must match",p2.getZ().getMetresValue(),-2430.6421078521,1e-8);

		TPositionVector p3(4394470.39636727,462839.500477364,4581271.38035491,TCoordSysFactory::k3DCartesian);
		toILG.transform(p3);
		ensure_distance("X0 coordinate of P3 in LG must match",p3.getX().getMetresValue(),-2831.15431514712,1e-8);
		ensure_distance("Y0 coordinate of P3 in LG must match",p3.getY().getMetresValue(),-620.724774934958,1e-8);
		ensure_distance("Z0 coordinate of P3 in LG must match",p3.getZ().getMetresValue(),-2433.65999999975  ,1e-8);
	}

	template<>
	template<>
	void object::test<3>()
	{
		using namespace LGC;
		set_test_name("Testing TISG2ILATransformation class");

		//Origin of the LG and LA system in the CCS
		TPositionVector origin(1900, 2000, 2500,TCoordSysFactory::k3DCartesian);

		//CG2000Machine
		TPositionVector p2(2700.0, 650.0, 2450.0,TCoordSysFactory::k3DCartesian);
		TILG2ILATransformation toILA(origin,TRefSystemFactory::EGeoid::kCG2000Machine);
		toILA.transform(p2);
		ensure_distance(" LG2LA X-coordinate should be equal", p2.getX().getMetresValue(), 2699.99936001302,1e-8);
		ensure_distance("LG2LA Y-coordinate should be equal", p2.getY().getMetresValue(),649.999404985115,1e-8);
		ensure_distance("LG2LA Z-coordinate should be equal", p2.getZ().getMetresValue(),2450.00086315287,1e-8);
		toILA.transformInverse(p2);
		ensure_distance("Inverse transformation should lead to the original point X", p2.getX().getMetresValue(), 2700.0,1e-8);
		ensure_distance("Inverse transformation should lead to the original point Y", p2.getY().getMetresValue(),650.0,1e-8);
		ensure_distance("Inverse transformation should lead to the original point Z", p2.getZ().getMetresValue(),2450.0,1e-8);

		//CG1985Machine
		TPositionVector p3(2700.0, 650.0, 2450.0,TCoordSysFactory::k3DCartesian);	
		TILG2ILATransformation toILA2(origin,TRefSystemFactory::EGeoid::kCG1985Machine);
		toILA2.transform(p3);
		ensure_distance(" LG2LA X-coordinate should be equal", p3.getX().getMetresValue(), 2699.99946056365,1e-8);
		ensure_distance("LG2LA Y-coordinate should be equal", p3.getY().getMetresValue(),650.000213846347,1e-8);
		ensure_distance("LG2LA Z-coordinate should be equal", p3.getZ().getMetresValue(),2450.00053774631,1e-8);
		toILA2.transformInverse(p3);
		ensure_distance("Inverse transformation should lead to the original point X", p3.getX().getMetresValue(), 2700.0,1e-8);
		ensure_distance("Inverse transformation should lead to the original point Y", p3.getY().getMetresValue(),650.0,1e-8);
		ensure_distance("Inverse transformation should lead to the original point Z", p3.getZ().getMetresValue(),2450.0,1e-8);

		//sphere
		TPositionVector p4(2700.0, 650.0, 2450.0, TCoordSysFactory::k3DCartesian);
		TILG2ILATransformation toILA3(origin, TRefSystemFactory::EGeoid::kCGSphere);
		toILA3.transform(p4);
		ensure_distance(" LG2LA X-coordinate should be equal", p4.getX().getMetresValue(), 2700.0, 1e-8);
		ensure_distance("LG2LA Y-coordinate should be equal", p4.getY().getMetresValue(), 650.0, 1e-8);
		ensure_distance("LG2LA Z-coordinate should be equal", p4.getZ().getMetresValue(), 2450.0, 1e-8);
		toILA3.transformInverse(p4);
		ensure_distance("Inverse transformation should lead to the original point X", p4.getX().getMetresValue(), 2700.0, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Y", p4.getY().getMetresValue(), 650.0, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Z", p4.getZ().getMetresValue(), 2450.0, 1e-8);
	}

	template<>
	template<>
	void object::test<4>()
	{
		using namespace LGC;
		set_test_name("Testing transformation from CCS to a Instrument Local Astronomical");

		TCCS2CGRFTransformation toCGRF;

		//Origin of the LG and LA system in the CCS
		TPositionVector originCCS(1000, 1000, 1000,TCoordSysFactory::k3DCartesian);
		TPositionVector origin(1000,1000, 1000,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(origin);
		TCGRF2LGTransformation toILG(origin, false);
		TILG2ILATransformation toILA(originCCS,TRefSystemFactory::EGeoid::kCG2000Machine);
		//82.9161404299426

		TPositionVector p2(1100, 1000, 1000,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(p2);
		toILG.transform(p2);
		toILA.transform(p2);

		ensure_distance("X-coordinate should be equal", p2.getX().getMetresValue(), 82.9161404299426,1e-9);
		ensure_distance("Y-coordinate should be equal", p2.getY().getMetresValue(), -55.9009249696545,1e-9);
		ensure_distance("Z-coordinate should be equal", p2.getZ().getMetresValue(), -0.0156127427608507,1e-8);


		TPositionVector p3(1100, 1100, 1100,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(p3);
		toILG.transform(p3);
		toILA.transform(p3);

		ensure_distance("X-coordinate should be equal", p3.getX().getMetresValue(), 138.839579558267,1e-9);
		ensure_distance("Y-coordinate should be equal", p3.getY().getMetresValue(), 27.0206854988607,1e-9);
		ensure_distance("Z-coordinate should be equal", p3.getZ().getMetresValue(), 99.9672631579586,1e-8);
	}


	template<>
	template<>
	void object::test<5>()
	{
		set_test_name("Testing transformations between XYH systems and CCS.");

		TPositionVector posit(1.0,1.0,50.0,TCoordSysFactory::ECoordSys::k2DPlusH);
		TXYH2CCS::XYHg2000Machine2CCS(posit);
		ensure_equals("Transformed value should match", posit.getZ().getMetresValue(),2049.34656894826 ,1e-9);

		TPositionVector posit2(1.0,1.0,50.0,TCoordSysFactory::ECoordSys::k2DPlusH);
		TXYH2CCS::XYHg1985Machine2CCS(posit2);
		ensure_equals("Transformed value should match",posit2.getZ().getMetresValue(), 2049.3495494283,1e-9);
		

		TPositionVector posit3(1.0,1.0,-1949.34193663989,TCoordSysFactory::ECoordSys::k2DPlusH);
		TXYH2CCS::XYHs2CCS(posit3);
		ensure_equals("Transformed value should match",posit3.getZ().getMetresValue(), 50.0,1e-9);

		TPositionVector posit3inv(1.0,1.0,50.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TXYH2CCS::CCS2XYHs(posit3inv);
		ensure_equals("Transformed value should match",posit3inv.getH().getMetresValue(), -1949.34193663989,1e-9);


		TPositionVector posit4(1.0,1.0,100.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TXYH2CCS::CCS2XYHg2000Machine(posit4);
		ensure_equals("Transformed value should match",posit4.getH().getMetresValue(),-1899.34636845479 ,1e-9);

		TPositionVector posit5(1.0,1.0,100.0,TCoordSysFactory::ECoordSys::k3DCartesian);
		TXYH2CCS::CCS2XYHg1985Machine(posit5);
		ensure_equals("Transformed value should match",posit5.getH().getMetresValue(), -1899.34934893453,1e-9);
	}


	template<>
	template<>
	void object::test<6>()
	{
		using namespace LGC;
		set_test_name("Testing TLA2MLATransformation class");

		TPositionVector p(-20.8903160236016, 13.7359693104267, -1.0000542759957, TCoordSysFactory::k3DCartesian); //3000  3000  2449.8616566
		TPositionVector origin(3025, 3000.3, 2450.8577816, TCoordSysFactory::k3DCartesian);
		TLA2MLATransformation fLA2MLA(origin, TRefSystemFactory::EGeoid::kCG2000Machine, TAngle(0.0), TAngle(0.0));
		fLA2MLA.transform(p);
		ensure_distance("X0 coordinate of P0 in MLA must match", p.getX().getMetresValue(), -24.9998448080709, 1e-8);
		ensure_distance("Y0 coordinate of P0 in MLA must match", p.getY().getMetresValue(), -0.299857904969592, 1e-8);
		ensure_distance("Z0 coordinate of P0 in MLA must match", p.getZ().getMetresValue(), -1.00005429952316, 1e-8);

		//Inverse should lead to the original point
		fLA2MLA.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in LA must match", p.getX().getMetresValue(), 20.8903160236016, 1e-8);
		ensure_distance("Y0 coordinate of P0 in LA must match", p.getY().getMetresValue(), 13.7359693104267, 1e-8);
		ensure_distance("Z0 coordinate of P0 in LA must match", p.getZ().getMetresValue(), -1.0000542759957, 1e-8);


		TLA2MLATransformation fLA2MLA2(origin,TRefSystemFactory::EGeoid::kCG2000Machine, TAngle(10.0, TAngle::EUnits::kGons), TAngle(-5.0, TAngle::EUnits::kGons));
		fLA2MLA2.transform(p);
		ensure_distance("X0 coordinate of P0 in MLA must match", p.getX().getMetresValue(), -24.6470134236698, 1e-8);
		ensure_distance("Y0 coordinate of P0 in MLA must match", p.getY().getMetresValue(), -4.19605582472899, 1e-8);
		ensure_distance("Z0 coordinate of P0 in MLA must match", p.getZ().getMetresValue(), -1.00005429952316, 1e-8);

		//Inverse should lead to the original point
		fLA2MLA2.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in LA must match", p.getX().getMetresValue(), 20.8903160236016, 1e-8);
		ensure_distance("Y0 coordinate of P0 in LA must match", p.getY().getMetresValue(), 13.7359693104267, 1e-8);
		ensure_distance("Z0 coordinate of P0 in LA must match", p.getZ().getMetresValue(), 1.0000542759957, 1e-8);
	}
}