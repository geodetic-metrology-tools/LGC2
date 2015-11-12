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
		TCCS2CGRFTransformation toCGRF(false); //uses ellipsoid

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

		TPositionVector p2(-1026.53292000015, 10475.4085800003, 2413.63802000013, TCoordSysFactory::k3DCartesian);  //h=419.833838109769
		toCGRF.transform(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getX().getMetresValue(), 4388954.29688504, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getY().getMetresValue(), 467289.853669467, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getZ().getMetresValue(), 4589418.80977854, 1e-8);

		//Inverse transformation should lead to the original point
		toCGRF.transformIverse(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in CCS must match", p2.getX().getMetresValue(), -1026.53292000015, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CCS must match", p2.getY().getMetresValue(), 10475.4085800003, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CCS must match", p2.getZ().getMetresValue(), 2413.63802000013, 1e-8);

		TPositionVector p3(5.9134567891,1.1234879528,3.0178921478,TCoordSysFactory::k3DCartesian);
		toCGRF.transform(p3);
		ensure_distance("X0 coordinate of P2 in CGRF must match",p3.getX().getMetresValue(),4394473.59536092,1e-8);
		ensure_distance("Y0 coordinate of P2 in CGRF must match",p3.getY().getMetresValue(),462845.401137905,1e-8);
		ensure_distance("Z0 coordinate of P2 in CGRF must match",p3.getZ().getMetresValue(),4581271.91655842,1e-8);


		toCGRF.reInitialize(true); //uses sphere
		toCGRF.transform(p);

		ensure_distance("X0 coordinate of P0 in CGRFs must match", p.getX().getMetresValue(), 4382812.29940346, 1e-8);
		ensure_distance("Y0 coordinate of P0 in CGRFs must match", p.getY().getMetresValue(), 464451.086722263, 1e-8);
		ensure_distance("Z0 coordinate of P0 in CGRFs must match", p.getZ().getMetresValue(), 4601131.20981681, 1e-8);

		toCGRF.transformIverse(p);

		ensure_distance("X0 coordinate of P0 in CCS must match",p.getX().getMetresValue(), 2000.00000,1e-8);
		ensure_distance("Y0 coordinate of P0 in CCS must match",p.getY().getMetresValue(), 2097.79265,1e-8);
		ensure_distance("Z0 coordinate of P0 in CCS must match",p.getZ().getMetresValue(), 2433.66000,1e-8);

		toCGRF.transform(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRFs must match", p2.getX().getMetresValue(), 4376366.23250677, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRFs must match", p2.getY().getMetresValue(), 465955.883656103, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRFs must match", p2.getZ().getMetresValue(), 4607091.79358163, 1e-8);

		//Inverse transformation should lead to the original point
		toCGRF.transformIverse(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in CCS must match", p2.getX().getMetresValue(), -1026.53292000015, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CCS must match", p2.getY().getMetresValue(), 10475.4085800003, 1e-8);
		ensure_distance("MQXA_1R5E coordinate of P1 in CCS must match", p2.getZ().getMetresValue(), 2413.63802000013, 1e-8);
	}

	template<>
	template<>
	void object::test<2>()
	{
		using namespace LGC;
		set_test_name("Testing TCGRF2LGTransformation class");

		//Origin of the local geodetic system in the CGRF at P0
		TPositionVector originCCS(2000.00000, 2097.79265, 2433.66000, TCoordSysFactory::k3DCartesian);
		TCGRF2LGTransformation toILG(originCCS, false);
		
		TPositionVector p(4395400.36378173, 465785.056735627, 4583458.22601372,TCoordSysFactory::k3DCartesian);
		toILG.transform(p);
		ensure_distance("X0 coordinate of P0 in LG must match",p.getX().getMetresValue(),0.0,1e-8);
		ensure_distance("Y0 coordinate of P0 in LG must match",p.getY().getMetresValue(),0.0,1e-8);
		ensure_distance("Z0 coordinate of P0 in LG must match",p.getZ().getMetresValue(),0.0,1e-8);

		//Inverse transformation should lead to the original point
		toILG.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in CGRF must match", p.getX().getMetresValue(), 4395400.36378173, 1e-8);
		ensure_distance("Y0 coordinate of P0 in CGRF must match", p.getY().getMetresValue(), 465785.056735627, 1e-8);
		ensure_distance("Z0 coordinate of P0 in CGRF must match", p.getZ().getMetresValue(), 4583458.22601372, 1e-8);

		//To point 5
		TPositionVector p2(4388954.29688504, 467289.853669467, 4589418.80977854, TCoordSysFactory::k3DCartesian);
		toILG.transform(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2.getX().getMetresValue(), 2175.71085853853, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2.getY().getMetresValue(), 8637.7446388872, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2.getZ().getMetresValue(), -20.0219800545765, 1e-7);

		//Inverse transformation should lead to the original point
		toILG.transformInverse(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getX().getMetresValue(), 4388954.29688504, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getY().getMetresValue(), 467289.853669467, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getZ().getMetresValue(), 4589418.80977854, 1e-7);


		// Origin not at P0
		TPositionVector origin2(-1026.53292000015, 10475.4085800003, 2413.63802000013, TCoordSysFactory::k3DCartesian);
		TCGRF2LGTransformation toILG2(origin2, false);
		toILG2.transform(p);
		ensure_distance("X0 coordinate of P0 in LG must match", p.getX().getMetresValue(), -2178.79224525759, 1e-7);
		ensure_distance("Y0 coordinate of P0 in LG must match", p.getY().getMetresValue(), -8636.98779438108, 1e-7);
		ensure_distance("Z0 coordinate of P0 in LG must match", p.getZ().getMetresValue(), 7.566891260789820, 1e-7);
		toILG2.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in CGRF must match", p.getX().getMetresValue(), 4395400.36378173, 1e-7);
		ensure_distance("Y0 coordinate of P0 in CGRF must match", p.getY().getMetresValue(), 465785.056735627, 1e-7);
		ensure_distance("Z0 coordinate of P0 in CGRF must match", p.getZ().getMetresValue(), 4583458.22601372, 1e-7);

		//To point 5
		toILG2.transform(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2.getX().getMetresValue(),0.0 , 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2.getY().getMetresValue(),0.0 , 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2.getZ().getMetresValue(),0.0 , 1e-7);
		toILG2.transformInverse(p2);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getX().getMetresValue(), 4388954.29688504, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getY().getMetresValue(), 467289.853669467, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRF must match", p2.getZ().getMetresValue(), 4589418.80977854, 1e-7);

		//if we are using sphere
		// Origin not at P0
		TPositionVector origin3(-1026.53292000015, 10475.4085800003, 2413.63802000013, TCoordSysFactory::k3DCartesian);
		TCGRF2LGTransformation toILG3(origin3, true);
		TPositionVector ps(4382812.29940346, 464451.086722263, 4601131.20981681, TCoordSysFactory::k3DCartesian);
		toILG3.transform(ps);
		ensure_distance("X0 coordinate of P0 in LG must match", ps.getX().getMetresValue(), -2178.80110724816, 1e-7);
		ensure_distance("Y0 coordinate of P0 in LG must match", ps.getY().getMetresValue(), -8636.98555715803, 1e-7);
		ensure_distance("Z0 coordinate of P0 in LG must match", ps.getZ().getMetresValue(), 7.56879428475168, 1e-7);
		toILG3.transformInverse(ps);
		ensure_distance("X0 coordinate of P0 in CGRFs must match", ps.getX().getMetresValue(), 4382812.29940346, 1e-7);
		ensure_distance("Y0 coordinate of P0 in CGRFs must match", ps.getY().getMetresValue(), 464451.086722263, 1e-7);
		ensure_distance("Z0 coordinate of P0 in CGRFs must match", ps.getZ().getMetresValue(), 4601131.20981681, 1e-7);

		//To point 5
		TPositionVector p2s(4376366.23250677, 465955.883656103, 4607091.79358163, TCoordSysFactory::k3DCartesian);
		toILG3.transform(p2s);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2s.getX().getMetresValue(), 0.0, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2s.getY().getMetresValue(), 0.0, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in LG must match", p2s.getZ().getMetresValue(), 0.0, 1e-7);
		toILG3.transformInverse(p2s);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRFs must match", p2s.getX().getMetresValue(), 4376366.23250677, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRFs must match", p2s.getY().getMetresValue(), 465955.883656103, 1e-7);
		ensure_distance("MQXA_1R5E coordinate of P1 in CGRFs must match", p2s.getZ().getMetresValue(), 4607091.79358163, 1e-7);
	}

	template<>
	template<>
	void object::test<3>()
	{
		using namespace LGC;
		set_test_name("Testing TLG2LATransformation class");

		//Origin of the LG and LA system in the CCS
		TPositionVector origin(-1026.53292000015, 10475.4085800003, 2413.63802000013, TCoordSysFactory::k3DCartesian);

		//CG2000Machine
		TPositionVector p(-2178.79224525759, -8636.98779438108, 7.566891260789820, TCoordSysFactory::k3DCartesian);
		TPositionVector p2(0.0, 0.0, 0.0, TCoordSysFactory::k3DCartesian);

		TILG2ILATransformation toILA(origin, TRefSystemFactory::EGeoid::kCG2000Machine);
		toILA.transform(p);
		ensure_distance("LG2LA X-coordinate should be equal", p.getX().getMetresValue(), -2178.91036336503, 1e-8);
		ensure_distance("LG2LA Y-coordinate should be equal", p.getY().getMetresValue(), -8636.95792050725, 1e-8);
		ensure_distance("LG2LA Z-coordinate should be equal", p.getZ().getMetresValue(), 7.65336091143967, 1e-8);
		toILA.transformInverse(p);
		ensure_distance("Inverse transformation should lead to the original point X", p.getX().getMetresValue(), -2178.79224525759, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Y", p.getY().getMetresValue(), -8636.98779438108, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Z", p.getZ().getMetresValue(), 7.566891260789820, 1e-8);
		toILA.transform(p2);
		ensure_distance("LG2LA X-coordinate should be equal", p2.getX().getMetresValue(), 0.0,1e-8);
		ensure_distance("LG2LA Y-coordinate should be equal", p2.getY().getMetresValue(), 0.0,1e-8);
		ensure_distance("LG2LA Z-coordinate should be equal", p2.getZ().getMetresValue(), 0.0,1e-8);
		toILA.transformInverse(p2);
		ensure_distance("Inverse transformation should lead to the original point X", p2.getX().getMetresValue(), 0.0, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Y", p2.getY().getMetresValue(), 0.0, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Z", p2.getZ().getMetresValue(), 0.0, 1e-8);

		//CG1985Machine	
		TILG2ILATransformation toILA2(origin,TRefSystemFactory::EGeoid::kCG1985Machine);
		toILA2.transform(p);
		ensure_distance("LG2LA X-coordinate should be equal", p.getX().getMetresValue(), -2179.01597572789, 1e-8);
		ensure_distance("LG2LA Y-coordinate should be equal", p.getY().getMetresValue(), -8636.93131395841, 1e-8);
		ensure_distance("LG2LA Z-coordinate should be equal", p.getZ().getMetresValue(), 7.61062501169382, 1e-8);
		toILA2.transformInverse(p);
		ensure_distance("Inverse transformation should lead to the original point X", p.getX().getMetresValue(), -2178.79224525759, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Y", p.getY().getMetresValue(), -8636.98779438108, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Z", p.getZ().getMetresValue(), 7.566891260789820, 1e-8);

		//sphere  (no transformation in sphere LA=LG)
		TILG2ILATransformation toILA3(origin, TRefSystemFactory::EGeoid::kCGSphere);
		toILA3.transform(p);
		ensure_distance("LG2LA X-coordinate should be equal", p.getX().getMetresValue(), -2178.79224525759, 1e-8);
		ensure_distance("LG2LA Y-coordinate should be equal", p.getY().getMetresValue(), -8636.98779438108, 1e-8);
		ensure_distance("LG2LA Z-coordinate should be equal", p.getZ().getMetresValue(), 7.566891260789820, 1e-8);
		toILA3.transformInverse(p);
		ensure_distance("Inverse transformation should lead to the original point X", p.getX().getMetresValue(), -2178.79224525759, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Y", p.getY().getMetresValue(), -8636.98779438108, 1e-8);
		ensure_distance("Inverse transformation should lead to the original point Z", p.getZ().getMetresValue(), 7.566891260789820, 1e-8);
	}


	template<>
	template<>
	void object::test<4>()
	{
		using namespace LGC;
		set_test_name("Testing successive transformation class");

		//Originin the CCS
		TPositionVector origin(-1026.53292000015, 10475.4085800003, 2413.63802000013, TCoordSysFactory::k3DCartesian);
		TPositionVector p(2000.00000, 2097.79265, 2433.66000, TCoordSysFactory::k3DCartesian);

		TCCS2CGRFTransformation toCGRF(false); //uses ellipsoid
		toCGRF.transform(p);
		ensure_equals("X0 coordinate of P0 in CGRF must match", p.getX().getMetresValue(), 4395400.36378173, 1e-8);
		ensure_equals("Y0 coordinate of P0 in CGRF must match", p.getY().getMetresValue(), 465785.056735627, 1e-8);
		ensure_equals("Z0 coordinate of P0 in CGRF must match", p.getZ().getMetresValue(), 4583458.22601372, 1e-8);

		TCGRF2LGTransformation toILG(origin, false);
		toILG.transform(p);
		ensure_equals("X0 coordinate of P0 in LG must match", p.getX().getMetresValue(), -2178.79224525759, 1e-7);
		ensure_equals("Y0 coordinate of P0 in LG must match", p.getY().getMetresValue(), -8636.98779438108, 1e-7);
		ensure_equals("Z0 coordinate of P0 in LG must match", p.getZ().getMetresValue(), 7.566891260789820, 1e-7);

		TILG2ILATransformation toILA(origin, TRefSystemFactory::EGeoid::kCG2000Machine);
		toILA.transform(p);
		ensure_equals("X0 coordinate of P0 in LA should be equal", p.getX().getMetresValue(), -2178.91036336503, 1e-8);
		ensure_equals("Y0 coordinate of P0 in LA should be equal", p.getY().getMetresValue(), -8636.95792050725, 1e-8);
		ensure_equals("Z0 coordinate of P0 in LA should be equal", p.getZ().getMetresValue(), 7.65336091143967, 1e-8);

		TLA2MLATransformation toMLA(origin, TRefSystemFactory::EGeoid::kCG2000Machine, TAngle(0.0), TAngle(0.0));
		toMLA.transform(p);
		ensure_equals("X0 coordinate of P0 in MLA should be equal", p.getX().getMetresValue(), 3026.54176942626, 1e-8);
		ensure_equals("Y0 coordinate of P0 in MLA should be equal", p.getY().getMetresValue(), -8377.63316277997, 1e-8);
		ensure_equals("Z0 coordinate of P0 in MLA should be equal", p.getZ().getMetresValue(), 7.65336091143967, 1e-8);
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
		set_test_name("Testing TLA2MLATransformation class with ellipsoid");

		TPositionVector p(3000.0, 3000.0, 2449.8616566, TCoordSysFactory::k3DCartesian); 
		TPositionVector origin(3025, 3000.3, 2450.8577816, TCoordSysFactory::k3DCartesian);
		TCCS2CGRFTransformation toCGRF(false); //uses ellipsoid
		toCGRF.transform(p);
		TCGRF2LGTransformation toILG(origin, false);
		toILG.transform(p);
		TILG2ILATransformation toILA(origin, TRefSystemFactory::EGeoid::kCG2000Machine);
		toILA.transform(p);

		TLA2MLATransformation fLA2MLA(origin, TRefSystemFactory::EGeoid::kCG2000Machine, TAngle(0.0), TAngle(0.0));
		fLA2MLA.transform(p);
		ensure_distance("X0 coordinate of P0 in MLA must match", p.getX().getMetresValue(), -24.9998448080709, 1e-7);
		ensure_distance("Y0 coordinate of P0 in MLA must match", p.getY().getMetresValue(), -0.299857904969592, 1e-7);
		ensure_distance("Z0 coordinate of P0 in MLA must match", p.getZ().getMetresValue(), -1.00005429952316, 1e-7);

		//Inverse should lead to the original point
		fLA2MLA.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in LA must match", p.getX().getMetresValue(), -20.890316023601, 1e-7);
		ensure_distance("Y0 coordinate of P0 in LA must match", p.getY().getMetresValue(), 13.735969310426, 1e-7);
		ensure_distance("Z0 coordinate of P0 in LA must match", p.getZ().getMetresValue(), -1.0000542759957, 1e-7);


		TLA2MLATransformation fLA2MLA2(origin,TRefSystemFactory::EGeoid::kCG2000Machine, TAngle(10.0, TAngle::EUnits::kGons), TAngle(-5.0, TAngle::EUnits::kRadians));
		fLA2MLA2.transform(p);
		ensure_distance("X0 coordinate of P0 in MLA must match", p.getX().getMetresValue(), -24.6470134480399, 1e-7);
		ensure_distance("Y0 coordinate of P0 in MLA must match", p.getY().getMetresValue(), -4.19605583316872, 1e-7);
		ensure_distance("Z0 coordinate of P0 in MLA must match", p.getZ().getMetresValue(), -1.00005429952316, 1e-7);

		//Inverse should lead to the original point
		fLA2MLA2.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in LA must match", p.getX().getMetresValue(), -20.8903160236016, 1e-7);
		ensure_distance("Y0 coordinate of P0 in LA must match", p.getY().getMetresValue(), 13.7359693104267, 1e-7);
		ensure_distance("Z0 coordinate of P0 in LA must match", p.getZ().getMetresValue(), -1.0000542759957, 1e-7);
	}

	template<>
	template<>
	void object::test<7>()
	{
		using namespace LGC;
		set_test_name("Testing TLA2MLATransformation class with sphere");

		TPositionVector p(3000.0, 3000.0, 2449.8616566, TCoordSysFactory::k3DCartesian);
		TPositionVector origin(3025, 3000.3, 2450.8577816, TCoordSysFactory::k3DCartesian);
		TCCS2CGRFTransformation toCGRF(true); //uses ellipsoid
		toCGRF.transform(p);
		TCGRF2LGTransformation toILG(origin, true);
		toILG.transform(p);
		TILG2ILATransformation toILA(origin, TRefSystemFactory::EGeoid::kCGSphere);
		toILA.transform(p);

		TLA2MLATransformation fLA2MLA(origin, TRefSystemFactory::EGeoid::kCGSphere, TAngle(0.0), TAngle(0.0));
		fLA2MLA.transform(p);
		ensure_distance("X0 coordinate of P0 in MLA must match", p.getX().getMetresValue(), -24.9998394260523, 1e-7);
		ensure_distance("Y0 coordinate of P0 in MLA must match", p.getY().getMetresValue(), -0.29985832751021, 1e-7);
		ensure_distance("Z0 coordinate of P0 in MLA must match", p.getZ().getMetresValue(), -1.00018931852648, 1e-7);

		//Inverse should lead to the original point
		fLA2MLA.transformInverse(p);
		ensure_distance("X0 coordinate of P0 in LA must match", p.getX().getMetresValue(), -20.8902510188546, 1e-7);
		ensure_distance("Y0 coordinate of P0 in LA must match", p.getY().getMetresValue(), 13.7360583397938, 1e-7);
		ensure_distance("Z0 coordinate of P0 in LA must match", p.getZ().getMetresValue(), -1.00018931852648, 1e-7);

	}
}