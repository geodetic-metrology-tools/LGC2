// SPDX-FileCopyrightText: 2025 CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <tut/tut.hpp>

//Input test files
#include <TLGCData.h>
#include <TLOR2LOR.h>
#include <TReader.h>
#include <AdjObjectsReader.h>
#include <LGCAdjustableObjectCollection.h>
#include <TXYH2CCS.h>
#include <TLGCCalculation.h>
#include "TLSInputMatricesFiller.h"
#include "TLSInputMatrices.h"
#include "TLSResultsMatricesExtractor.h"
#include "TInverseTransformation.h"
#include "TDirectTransformation.h"
#include <Eigen/Dense>

using namespace std;

namespace tut
{
    struct testUnitaireTransfo{};
    typedef test_group<testUnitaireTransfo> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Tests of Inverse and Direct transformations used in tree of local frames");
}

namespace tut
{
	
	const TCoordSysFactory::ECoordSys k3DCartesian (TCoordSysFactory::ECoordSys::k3DCartesian);

	//Test constructor of direct transformation
	template<>
	template<>
	void object::test<1>()
	{
		using namespace LGC;
		set_test_name("Test constructor of direct transformation");
			
		TDirectTransformation directTransfo;
		TransformParameters  identity = TransformParameters();

		ensure("Matrix should be equal to Identity", directTransfo.getMatrix() == Eigen::Matrix4d::Identity());
		TransformParameters  dirTrafo = directTransfo.getTransformParam();
		ensure("Estimated parameters should be equal", dirTrafo == identity);
	}

	// Test constructor of inverse transformation
	template<>
	template<>
	void object::test<2>()
	{
		using namespace LGC;
		set_test_name("Test constructor of inverse transformation");
			
		TInverseTransformation inverseTransfo;
		TransformParameters  identity = TransformParameters();

		ensure("Matrix should be equal to Identity", inverseTransfo.getMatrix() == Eigen::Matrix4d::Identity());
		TransformParameters  invTrafo = inverseTransfo.getTransformParam();
		ensure("Estimated parameters should be equal", invTrafo == identity);
	}

	// Test copy constructor of direct transformation
	template<>
	template<>
	void object::test<3>()
	{
		using namespace LGC;
		set_test_name("Test copy constructor of direct transformation");
		
		TDirectTransformation directTransfo;
		TDirectTransformation copy(directTransfo);

		ensure("Matrix should be equal to Identity", directTransfo.getMatrix() == copy.getMatrix());
		TransformParameters  dirTrafo = directTransfo.getTransformParam();
		TransformParameters  copyTrafo = copy.getTransformParam();
		ensure("Estimated parameters should be equal", dirTrafo == copyTrafo);
	}
	
	// Test copy constructor of inverse transformation
	template<>
	template<>
	void object::test<4>()
	{
		using namespace LGC;
		set_test_name("Test copy constructor of inverse transformation");
		
		TInverseTransformation inverseTransfo;
		TInverseTransformation copy(inverseTransfo);

		ensure("Matrix should be equal to Identity", inverseTransfo.getMatrix() == copy.getMatrix());
		TransformParameters  invTrafo = inverseTransfo.getTransformParam();
		TransformParameters  copyTrafo = copy.getTransformParam();
		ensure("Estimated parameters should be equal", invTrafo == copyTrafo);

	}

	// Test direct transformation
	template<>
	template<>
	void object::test<5>()
	{
		using namespace LGC;
		set_test_name("Test of direct transformation");

		auto GON(TAngle::kGons);		
		
		TDirectTransformation directTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 1;

		directTransfo.setTransformParam(param1);
		
		TReal kappacos = param1.kappa.cosine();
		TReal kappasin = param1.kappa.sine();
		TReal phicos = param1.phi.cosine();
		TReal phisin = param1.phi.sine();
		TReal omegacos = param1.omega.cosine();
		TReal omegasin = param1.omega.sine();

		//Test if the matrix is well completed
		ensure_distance("m(0,0)", directTransfo.getMmatrixIJPosition(0,0), kappacos*phicos, 1e-6);
		ensure_distance("m(0,1)", directTransfo.getMmatrixIJPosition(0,1), kappasin*phicos, 1e-6);
		ensure_distance("m(0,2)", directTransfo.getMmatrixIJPosition(0,2), -phisin, 1e-6);
		ensure_distance("m(0,3)", directTransfo.getMmatrixIJPosition(0,3), param1.tX/param1.scale, 1e-6);
		ensure_distance("m(1,0)", directTransfo.getMmatrixIJPosition(1,0), kappacos*phisin*omegasin-kappasin*omegacos, 1e-6);
		ensure_distance("m(1,1)", directTransfo.getMmatrixIJPosition(1,1), kappasin*phisin*omegasin+kappacos*omegacos, 1e-6);
		ensure_distance("m(1,2)", directTransfo.getMmatrixIJPosition(1,2), phicos*omegasin, 1e-6);
		ensure_distance("m(1,3)", directTransfo.getMmatrixIJPosition(1,3), param1.tY/param1.scale, 1e-6);
		ensure_distance("m(2,0)", directTransfo.getMmatrixIJPosition(2,0), kappacos*phisin*omegacos+kappasin*omegasin, 1e-6);
		ensure_distance("m(2,1)", directTransfo.getMmatrixIJPosition(2,1), kappasin*phisin*omegacos-kappacos*omegasin, 1e-6);
		ensure_distance("m(2,2)", directTransfo.getMmatrixIJPosition(2,2), phicos*omegacos, 1e-6);
		ensure_distance("m(2,3)", directTransfo.getMmatrixIJPosition(2,3), param1.tZ/param1.scale, 1e-6);
	
		// Apply transformation on a vector
		TPositionVector vector_to_transform(10.0, 3.0, -1.0, k3DCartesian);
		directTransfo.TTransformation::transform(vector_to_transform);

		double x_expected = directTransfo.getMmatrixIJPosition(0,0)*10 + directTransfo.getMmatrixIJPosition(0,1)*3.0 + directTransfo.getMmatrixIJPosition(0,2)*-1.0 + directTransfo.getMmatrixIJPosition(0,3)*1;
		double y_expected = directTransfo.getMmatrixIJPosition(1,0)*10 + directTransfo.getMmatrixIJPosition(1,1)*3.0 + directTransfo.getMmatrixIJPosition(1,2)*-1.0 + directTransfo.getMmatrixIJPosition(1,3)*1;
		double z_expected = directTransfo.getMmatrixIJPosition(2,0)*10 + directTransfo.getMmatrixIJPosition(2,1)*3.0 + directTransfo.getMmatrixIJPosition(2,2)*-1.0 + directTransfo.getMmatrixIJPosition(2,3)*1;

      ensure_distance("vector_to_transform(0)", vector_to_transform.getX().getMetresValue(), x_expected, 1e-6);
      ensure_distance("vector_to_transform(1)", vector_to_transform.getY().getMetresValue(), y_expected, 1e-6);
      ensure_distance("vector_to_transform(2)", vector_to_transform.getZ().getMetresValue(), z_expected, 1e-6);
	}
	
	// Test inverse transformation
	template<>
	template<>
	void object::test<6>()
	{
		using namespace LGC;
		set_test_name("Test  inverse transformation");

		auto GON(TAngle::kGons);		

		TInverseTransformation invTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 1;

		invTransfo.setTransformParam(param1);

		//Test if the matrix is well completed
		TReal kappacos = param1.kappa.cosine();
		TReal kappasin = param1.kappa.sine();
		TReal phicos = param1.phi.cosine();
		TReal phisin = param1.phi.sine();
		TReal omegacos = param1.omega.cosine();
		TReal omegasin = param1.omega.sine();

		ensure_distance("m(0,0)", invTransfo.getMmatrixIJPosition(0,0), kappacos*phicos, 1e-6);
		ensure_distance("m(0,1)", invTransfo.getMmatrixIJPosition(0,1), kappacos*phisin*omegasin-kappasin*omegacos, 1e-6);
		ensure_distance("m(0,2)", invTransfo.getMmatrixIJPosition(0,2), kappacos*phisin*omegacos+kappasin*omegasin, 1e-6);
		ensure_distance("m(0,3)", invTransfo.getMmatrixIJPosition(0,3), -(kappacos*phicos)*param1.tX.getMetresValue() - (kappacos*phisin*omegasin-kappasin*omegacos)*param1.tY.getMetresValue() - (kappacos*phisin*omegacos+kappasin*omegasin)*param1.tZ.getMetresValue(), 1e-6);
		ensure_distance("m(1,0)", invTransfo.getMmatrixIJPosition(1,0), kappasin*phicos, 1e-6);
		ensure_distance("m(1,1)", invTransfo.getMmatrixIJPosition(1,1), kappasin*phisin*omegasin+kappacos*omegacos, 1e-6);
		ensure_distance("m(1,2)", invTransfo.getMmatrixIJPosition(1,2), kappasin*phisin*omegacos-kappacos*omegasin, 1e-6);
		ensure_distance("m(1,3)", invTransfo.getMmatrixIJPosition(1,3), -(kappasin*phicos)*param1.tX.getMetresValue() - (kappasin*phisin*omegasin+kappacos*omegacos)*param1.tZ.getMetresValue() - (kappasin*phisin*omegacos-kappacos*omegasin)*param1.tZ.getMetresValue(), 1e-6);
		ensure_distance("m(2,0)", invTransfo.getMmatrixIJPosition(2,0), -phisin, 1e-6);
		ensure_distance("m(2,1)", invTransfo.getMmatrixIJPosition(2,1), phicos*omegasin, 1e-6);
		ensure_distance("m(2,2)", invTransfo.getMmatrixIJPosition(2,2), phicos*omegacos, 1e-6);
		ensure_distance("m(2,3)", invTransfo.getMmatrixIJPosition(2,3), -(-phisin)*param1.tX.getMetresValue() - (phicos*omegasin)*param1.tY.getMetresValue() - (phicos*omegacos)*param1.tZ.getMetresValue(), 1e-6);
	}

	// Test direct * his inverse transformation
	template<>
	template<>
	void object::test<7>()
	{
		using namespace LGC;
		set_test_name("Test direct * his inverse transformation");

		auto GON(TAngle::kGons);		

		TInverseTransformation invTransfo;
		TDirectTransformation dirTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 1;

		invTransfo.setTransformParam(param1);
		dirTransfo.setTransformParam(param1);

		// dir*inv should give identy
		TTransformation dirInv = dirTransfo*invTransfo;
		TPositionVector vector_to_transform(10.0, 10.0, 10.0,k3DCartesian);
		dirInv.transform(vector_to_transform);

		//Test if p_root has the correct coordinate
      ensure_distance("X component must match reference", vector_to_transform.getX().getMetresValue(), 10.0, 1e-6);
      ensure_distance("Y component must match reference", vector_to_transform.getY().getMetresValue(), 10.0, 1e-6);
      ensure_distance("Z component must match reference", vector_to_transform.getZ().getMetresValue(), 10.0, 1e-6);
	}

	// Test inv * his direct transformation
	template<>
	template<>
	void object::test<8>()
	{
		using namespace LGC;
		set_test_name("Test inv * his direct transformation");

		auto GON(TAngle::kGons);		
		
		TInverseTransformation invTransfo;
		TDirectTransformation dirTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 1;

		invTransfo.setTransformParam(param1);
		dirTransfo.setTransformParam(param1);

		// dir*inv should give identy
		TTransformation invDir = invTransfo*dirTransfo;
		TPositionVector vector_to_transform(0.0, 20.0, 0.0,k3DCartesian);
		invDir.transform(vector_to_transform);
		

		//Test if p_root has the correct coordinate
		ensure_distance("X component must match reference", vector_to_transform.getX().getMetresValue(), 0.0, 1e-6);
		ensure_distance("Y component must match reference", vector_to_transform.getY().getMetresValue(), 20.0, 1e-6);
		ensure_distance("Z component must match reference", vector_to_transform.getZ().getMetresValue(), 0.0, 1e-6);
	}

	// Test direct transformation with scale
	template<>
	template<>
	void object::test<9>()
	{
		using namespace LGC;
		set_test_name("Test of direct transformation with scale");

		auto GON(TAngle::kGons);		
		TPositionVector vector_to_transform(5.0, 15.0, 5.0,k3DCartesian);

		TDirectTransformation directTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		directTransfo.setTransformParam(param1);

		// Apply transformation
		directTransfo.TTransformation::transform(vector_to_transform);

		//Test if the matrix is well completed
		TReal kappacos = param1.kappa.cosine();
		TReal kappasin = param1.kappa.sine();
		TReal phicos = param1.phi.cosine();
		TReal phisin = param1.phi.sine();
		TReal omegacos = param1.omega.cosine();
		TReal omegasin = param1.omega.sine();

		ensure_distance("m(0,0)", directTransfo.getMmatrixIJPosition(0,0), kappacos*phicos, 1e-6);
		ensure_distance("m(0,1)", directTransfo.getMmatrixIJPosition(0,1), kappasin*phicos, 1e-6);
		ensure_distance("m(0,2)", directTransfo.getMmatrixIJPosition(0,2), -phisin, 1e-6);
		ensure_distance("m(0,3)", directTransfo.getMmatrixIJPosition(0,3), param1.tX.getMetresValue()/param1.scale, 1e-6);
		ensure_distance("m(1,0)", directTransfo.getMmatrixIJPosition(1,0), kappacos*phisin*omegasin-kappasin*omegacos, 1e-6);
		ensure_distance("m(1,1)", directTransfo.getMmatrixIJPosition(1,1), kappasin*phisin*omegasin+kappacos*omegacos, 1e-6);
		ensure_distance("m(1,2)", directTransfo.getMmatrixIJPosition(1,2), phicos*omegasin, 1e-6);
		ensure_distance("m(1,3)", directTransfo.getMmatrixIJPosition(1,3), param1.tY.getMetresValue()/param1.scale, 1e-6);
		ensure_distance("m(2,0)", directTransfo.getMmatrixIJPosition(2,0), kappacos*phisin*omegacos+kappasin*omegasin, 1e-6);
		ensure_distance("m(2,1)", directTransfo.getMmatrixIJPosition(2,1), kappasin*phisin*omegacos-kappacos*omegasin, 1e-6);
		ensure_distance("m(2,2)", directTransfo.getMmatrixIJPosition(2,2), phicos*omegacos, 1e-6);
		ensure_distance("m(2,3)", directTransfo.getMmatrixIJPosition(2,3), param1.tZ.getMetresValue()/param1.scale, 1e-6);
		
		
		//Test if p_root has the correct coordinate
		/*
		At the end, we would have the fourth component of the TPositionVector = 1
		so expected_point = 1/scl * (scl*x_root, scl*y_root, scl*z,1_root)  but the transformation returns only (scl*x_root, scl*y_root, scl*z,1_root)
		*/

		TPositionVector p_root2(20.0, 20.0, 20.0,k3DCartesian);
		ensure_distance("X component must match reference", vector_to_transform.getX().getMetresValue(), p_root2.getX().getMetresValue(), 1e-6);
		ensure_distance("Y component must match reference", vector_to_transform.getY().getMetresValue(), p_root2.getY().getMetresValue(), 1e-6);
		ensure_distance("Z component must match reference", vector_to_transform.getZ().getMetresValue(), p_root2.getZ().getMetresValue(), 1e-6);
	}

	// Test inverse transformation with scale
	template<>
	template<>
	void object::test<10>()
	{
		using namespace LGC;
		set_test_name("Test  inverse transformation with scale");

		auto GON(TAngle::kGons);		
		TPositionVector vector_to_transform(20.0, 20.0, 20.0,k3DCartesian);
		TPositionVector p_subF(5.0, 15.0, 5.0,k3DCartesian);

		TInverseTransformation invTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		invTransfo.setTransformParam(param1);

		// Apply transformation
		invTransfo.TTransformation::transform(vector_to_transform);

		//Test if the matrix is well completed
		TReal kappacos = param1.kappa.cosine();
		TReal kappasin = param1.kappa.sine();
		TReal phicos = param1.phi.cosine();
		TReal phisin = param1.phi.sine();
		TReal omegacos = param1.omega.cosine();
		TReal omegasin = param1.omega.sine();

		ensure_distance("m(0,0)", invTransfo.getMmatrixIJPosition(0,0), kappacos*phicos, 1e-6);
		ensure_distance("m(0,1)", invTransfo.getMmatrixIJPosition(0,1), kappacos*phisin*omegasin-kappasin*omegacos, 1e-6);
		ensure_distance("m(0,2)", invTransfo.getMmatrixIJPosition(0,2), kappacos*phisin*omegacos+kappasin*omegasin, 1e-6);
		ensure_distance("m(0,3)", invTransfo.getMmatrixIJPosition(0,3), -(kappacos*phicos)*param1.tX.getMetresValue() - (kappacos*phisin*omegasin-kappasin*omegacos)*param1.tY.getMetresValue() - (kappacos*phisin*omegacos+kappasin*omegasin)*param1.tZ.getMetresValue(), 1e-6);
		ensure_distance("m(1,0)", invTransfo.getMmatrixIJPosition(1,0), kappasin*phicos, 1e-6);
		ensure_distance("m(1,1)", invTransfo.getMmatrixIJPosition(1,1), kappasin*phisin*omegasin+kappacos*omegacos, 1e-6);
		ensure_distance("m(1,2)", invTransfo.getMmatrixIJPosition(1,2), kappasin*phisin*omegacos-kappacos*omegasin, 1e-6);
		ensure_distance("m(1,3)", invTransfo.getMmatrixIJPosition(1,3), -(kappasin*phicos)*param1.tX.getMetresValue() - (kappasin*phisin*omegasin+kappacos*omegacos)*param1.tY.getMetresValue() - (kappasin*phisin*omegacos-kappacos*omegasin)*param1.tZ.getMetresValue(), 1e-6);
		ensure_distance("m(2,0)", invTransfo.getMmatrixIJPosition(2,0), -phisin, 1e-6);
		ensure_distance("m(2,1)", invTransfo.getMmatrixIJPosition(2,1), phicos*omegasin, 1e-6);
		ensure_distance("m(2,2)", invTransfo.getMmatrixIJPosition(2,2), phicos*omegacos, 1e-6);
		ensure_distance("m(2,3)", invTransfo.getMmatrixIJPosition(2,3), -(-phisin)*param1.tX.getMetresValue() - (phicos*omegasin)*param1.tY.getMetresValue() - (phicos*omegacos)*param1.tZ.getMetresValue(), 1e-6);

		ensure_distance("X component must match reference", vector_to_transform.getX().getMetresValue(), p_subF.getX().getMetresValue(), 1e-6);
		ensure_distance("Y component must match reference", vector_to_transform.getY().getMetresValue(), p_subF.getY().getMetresValue(), 1e-6);
		ensure_distance("Z component must match reference", vector_to_transform.getZ().getMetresValue(), p_subF.getZ().getMetresValue(), 1e-6);
	}

	// Test direct * his inverse transformation with scale
	template<>
	template<>
	void object::test<11>()
	{
		using namespace LGC;
		set_test_name("Test direct * his inverse transformation with scale");

		auto GON(TAngle::kGons);		

		TInverseTransformation invTransfo;
		TDirectTransformation dirTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		invTransfo.setTransformParam(param1);
		dirTransfo.setTransformParam(param1);

		// dir*inv should give identy
		TTransformation dirInv = dirTransfo*invTransfo;

		ensure_distance("m(0,0)", dirInv.getMmatrixIJPosition(0,0), 1.0, 1e-6);
		ensure_distance("m(0,1)", dirInv.getMmatrixIJPosition(0,1), 0.0, 1e-6);
		ensure_distance("m(0,2)", dirInv.getMmatrixIJPosition(0,2), 0.0, 1e-6);
		ensure_distance("m(0,3)", dirInv.getMmatrixIJPosition(0,3), 0.0, 1e-6);
		ensure_distance("m(1,0)", dirInv.getMmatrixIJPosition(1,0), 0.0, 1e-6);
		ensure_distance("m(1,1)", dirInv.getMmatrixIJPosition(1,1), 1.0, 1e-6);
		ensure_distance("m(1,2)", dirInv.getMmatrixIJPosition(1,2), 0.0, 1e-6);
		ensure_distance("m(1,3)", dirInv.getMmatrixIJPosition(1,3), 0.0, 1e-6);
		ensure_distance("m(2,0)", dirInv.getMmatrixIJPosition(2,0), 0.0, 1e-6);
		ensure_distance("m(2,1)", dirInv.getMmatrixIJPosition(2,1), 0.0, 1e-6);
		ensure_distance("m(2,2)", dirInv.getMmatrixIJPosition(2,2), 1.0, 1e-6);
		ensure_distance("m(2,3)", dirInv.getMmatrixIJPosition(2,3), 0.0, 1e-6);
		ensure_distance("m(3,0)", dirInv.getMmatrixIJPosition(3,0), 0.0, 1e-6);
		ensure_distance("m(3,1)", dirInv.getMmatrixIJPosition(3,1), 0.0, 1e-6);
		ensure_distance("m(3,2)", dirInv.getMmatrixIJPosition(3,2), 0.0, 1e-6);
		ensure_distance("m(3,3)", dirInv.getMmatrixIJPosition(3,3), 1.0, 1e-6);
	}

	// Test inv * his direct transformation with scale
	template<>
	template<>
	void object::test<12>()
	{
		using namespace LGC;
		set_test_name("Test inv * his direct transformation with scale");
		auto GON(TAngle::kGons);		

		TInverseTransformation invTransfo;
		TDirectTransformation dirTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		invTransfo.setTransformParam(param1);
		dirTransfo.setTransformParam(param1);

		// dir*inv should give identy
		TTransformation invDir = invTransfo*dirTransfo;
		Eigen::Matrix4d Id;
		Id.setIdentity();

		ensure("invDir is identity ", invDir.getMatrix().isApprox(Id));
	}
	
	// Test  angle partial derivative for a direct transformation
	template<>
	template<>
	void object::test<13>()
	{
		using namespace LGC;
		set_test_name("Rotation partial derivative for a direct transformation");

		auto GON(TAngle::kGons);		

		TDirectTransformation dirTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		dirTransfo.setTransformParam(param1);

		TDerivativeTransformation rot0 = dirTransfo.differentiatedTransformationAngle(0);
		TDerivativeTransformation rot1 = dirTransfo.differentiatedTransformationAngle(1);
		TDerivativeTransformation rot2 = dirTransfo.differentiatedTransformationAngle(2);

		//Test if the matrix is well completed
		TReal kappaCos = param1.kappa.cosine();
		TReal kappaSin = param1.kappa.sine();
		TReal phiCos = param1.phi.cosine();
		TReal phiSin = param1.phi.sine();
		TReal omegaCos = param1.omega.cosine();
		TReal omegaSin = param1.omega.sine();

		// rot around X axis
		Eigen::Matrix4d rotX;
		rotX.setZero();
		rotX(1,0) = kappaCos*phiSin*omegaCos + kappaSin*omegaSin;
		rotX(1,1) = kappaSin*phiSin*omegaCos-kappaCos*omegaSin;
		rotX(1,2) = phiCos*omegaCos;
		rotX(2,0) = -kappaCos*phiSin*omegaSin+kappaSin*omegaCos;
		rotX(2,1) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
		rotX(2,2) = -phiCos*omegaSin;
		rotX(3,3) = 1/param1.scale ;

		// rot around Y axis
		Eigen::Matrix4d rotY;
		rotY.setZero();
		rotY(0,0) = -kappaCos*phiSin;
		rotY(0,1) = -kappaSin*phiSin;
		rotY(0,2) = -phiCos;
		rotY(1,0) = kappaCos*phiCos*omegaSin;
		rotY(1,1) = kappaSin*phiCos*omegaSin;
		rotY(1,2) = -phiSin*omegaSin;
		rotY(2,0) = kappaCos*phiCos*omegaCos;
		rotY(2,1) = kappaSin*phiCos*omegaCos;
		rotY(2,2) = -phiSin*omegaCos;
		rotY(3,3) = 1/param1.scale ;

		// rot around Z axis
		Eigen::Matrix4d rotZ;
		rotZ.setZero();
   		rotZ(0,0) = -kappaSin*phiCos;
		rotZ(0,1) = kappaCos*phiCos;
		rotZ(1,0) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
		rotZ(1,1) = kappaCos*phiSin*omegaSin-kappaSin*omegaCos;
		rotZ(2,0) = -kappaSin*phiSin*omegaCos+kappaCos*omegaSin;
		rotZ(2,1) = kappaCos*phiSin*omegaCos+kappaSin*omegaSin;
		rotZ(3,3) = 1/param1.scale ;
/*
		Eigen::Vector4d pTemp(50, 60, 70, 1.0);
		Eigen::Vector4d result = rot0.getMatrix()*pTemp;
*/
		ensure(" rotation around x axis ", rot0.getMatrix() == rotX);
		ensure(" rotation around y axis ", rot1.getMatrix() == rotY);
		ensure(" rotation around z axis ", rot2.getMatrix() == rotZ);


	}
	
	// Test  translation partial derivative for a direct transformation
	template<>
	template<>
	void object::test<14>()
	{
		using namespace LGC;
		set_test_name("Translation partial derivative for a direct transformation");

		auto GON(TAngle::kGons);		

		TDirectTransformation dirTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		dirTransfo.setTransformParam(param1);

		TDerivativeTransformation transl0 = dirTransfo.differentiatedTransformationTranslation(0);
		TDerivativeTransformation transl1 = dirTransfo.differentiatedTransformationTranslation(1);
		TDerivativeTransformation transl2 = dirTransfo.differentiatedTransformationTranslation(2);


		// translation around X axis
		ensure_distance(" transl0 -> m(0,0) ", transl0.getMmatrixIJPosition(0,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(0,1) ", transl0.getMmatrixIJPosition(0,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(0,2) ", transl0.getMmatrixIJPosition(0,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(0,3) ", transl0.getMmatrixIJPosition(0,3), 1.0, 1e-6);
		ensure_distance(" transl0 -> m(1,0) ", transl0.getMmatrixIJPosition(1,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(1,1) ", transl0.getMmatrixIJPosition(1,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(1,2) ", transl0.getMmatrixIJPosition(1,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(1,3) ", transl0.getMmatrixIJPosition(1,3), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,0) ", transl0.getMmatrixIJPosition(2,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,1) ", transl0.getMmatrixIJPosition(2,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,2) ", transl0.getMmatrixIJPosition(2,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,3) ", transl0.getMmatrixIJPosition(2,3), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,0) ", transl0.getMmatrixIJPosition(3,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,1) ", transl0.getMmatrixIJPosition(3,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,2) ", transl0.getMmatrixIJPosition(3,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,3) ", transl0.getMmatrixIJPosition(3,3), 1.0, 1e-6);
		
		// translation around Y axis
		ensure_distance(" transl1 -> m(0,0) ", transl1.getMmatrixIJPosition(0,0), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(0,3) ", transl1.getMmatrixIJPosition(0,3), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(1,2) ", transl1.getMmatrixIJPosition(1,2), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(1,3) ", transl1.getMmatrixIJPosition(1,3), 1.0, 1e-6);
		ensure_distance(" transl1 -> m(2,1) ", transl1.getMmatrixIJPosition(2,1), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(2,3) ", transl1.getMmatrixIJPosition(2,3), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(2,3) ", transl1.getMmatrixIJPosition(3,3), 1.0, 1e-6);
		// translation around Z axis
		ensure_distance(" transl2 -> m(0,2) ", transl2.getMmatrixIJPosition(0,2), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(0,3) ", transl2.getMmatrixIJPosition(0,3), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(1,0) ", transl2.getMmatrixIJPosition(1,0), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(1,3) ", transl2.getMmatrixIJPosition(1,3), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(2,1) ", transl2.getMmatrixIJPosition(2,1), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(2,3) ", transl2.getMmatrixIJPosition(2,3), 1.0, 1e-6);
		ensure_distance(" transl2 -> m(2,3) ", transl2.getMmatrixIJPosition(3,3), 1.0, 1e-6);
	}
	
	// Test  scale factor partial derivative for a direct transformation
	template<>
	template<>
	void object::test<15>()
	{
		using namespace LGC;
		set_test_name("Scale factor partial derivative for a direct transformation");

		auto GON(TAngle::kGons);		

		TDirectTransformation directTransfo;
		TDerivativeTransformation diffScale;

		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		directTransfo.setTransformParam(param1);
		diffScale = directTransfo.differentiatedTransformationScaleFactor();
		
		const auto& m(directTransfo.getMatrix());

		ensure_distance(" transl0 -> m(0,0) ", diffScale.getMmatrixIJPosition(0,0), m(0,0), 1e-6);
		ensure_distance(" transl0 -> m(0,1) ", diffScale.getMmatrixIJPosition(0,1), m(0,1), 1e-6);
		ensure_distance(" transl0 -> m(0,2) ", diffScale.getMmatrixIJPosition(0,2), m(0,2), 1e-6);
		ensure_distance(" transl0 -> m(0,3) ", diffScale.getMmatrixIJPosition(0,3), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(1,0) ", diffScale.getMmatrixIJPosition(1,0), m(1,0), 1e-6);
		ensure_distance(" transl0 -> m(1,1) ", diffScale.getMmatrixIJPosition(1,1), m(1,1), 1e-6);
		ensure_distance(" transl0 -> m(1,2) ", diffScale.getMmatrixIJPosition(1,2), m(1,2), 1e-6);
		ensure_distance(" transl0 -> m(1,3) ", diffScale.getMmatrixIJPosition(1,3), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,0) ", diffScale.getMmatrixIJPosition(2,0), m(2,0), 1e-6);
		ensure_distance(" transl0 -> m(2,1) ", diffScale.getMmatrixIJPosition(2,1), m(2,1), 1e-6);
		ensure_distance(" transl0 -> m(2,2) ", diffScale.getMmatrixIJPosition(2,2), m(2,2), 1e-6);
		ensure_distance(" transl0 -> m(2,3) ", diffScale.getMmatrixIJPosition(2,3), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(3,0) ", diffScale.getMmatrixIJPosition(3,0), m(3,0), 1e-6);
		ensure_distance(" transl0 -> m(3,1) ", diffScale.getMmatrixIJPosition(3,1), m(3,1), 1e-6);
		ensure_distance(" transl0 -> m(3,2) ", diffScale.getMmatrixIJPosition(3,2), m(3,2), 1e-6);
		ensure_distance(" transl0 -> m(3,3) ", diffScale.getMmatrixIJPosition(3,3), 1.0, 1e-6);

	}
	
	// Test  angle partial derivative for an inverse transformation
	template<>
	template<>
	void object::test<16>()
	{
		using namespace LGC;
		set_test_name("Rotation partial derivative for an inverse transformation");

		auto GON(TAngle::kGons);		

		TInverseTransformation invTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		invTransfo.setTransformParam(param1);

		TDerivativeTransformation rot0 = invTransfo.differentiatedTransformationAngle(0);
		TDerivativeTransformation rot1 = invTransfo.differentiatedTransformationAngle(1);
		TDerivativeTransformation rot2 = invTransfo.differentiatedTransformationAngle(2);

		//Test if the matrix is well completed
		TReal kappaCos = param1.kappa.cosine();
		TReal kappaSin = param1.kappa.sine();
		TReal phiCos = param1.phi.cosine();
		TReal phiSin = param1.phi.sine();
		TReal omegaCos = param1.omega.cosine();
		TReal omegaSin = param1.omega.sine();

		// rot around X axis
		Eigen::Matrix4d rotX;
		rotX.setZero();

		rotX(0,1) = kappaCos*phiSin*omegaCos+kappaSin*omegaSin;
		rotX(0,2) = -kappaCos*phiSin*omegaSin+kappaSin*omegaCos;
		rotX(0,3) = -rotX(0,0)*param1.tX - rotX(0,1)*param1.tY - rotX(0,2)*param1.tZ;
		rotX(1,1) = kappaSin*phiSin*omegaCos-kappaCos*omegaSin;
		rotX(1,2) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
		rotX(1,3) = -rotX(1,0)*param1.tX - rotX(1,1)*param1.tY - rotX(1,2)*param1.tZ;
		rotX(2,1) = phiCos*omegaCos;
		rotX(2,2) = -phiCos*omegaSin;
		rotX(2,3) = -rotX(2,0)*param1.tX - rotX(2,1)*param1.tY - rotX(2,2)*param1.tZ;		
		rotX(3,3) = param1.scale;	


		// rot around Y axis
		Eigen::Matrix4d rotY;
		rotY.setZero();
		rotY(0,0) = -kappaCos*phiSin;
		rotY(0,1) = kappaCos*phiCos*omegaSin;
		rotY(0,2) = kappaCos*phiCos*omegaCos;
		rotY(0,3) = -rotY(0,0)*param1.tX - rotY(0,1)*param1.tY - rotY(0,2)*param1.tZ;
		rotY(1,0) = -kappaSin*phiSin;
		rotY(1,1) = kappaSin*phiCos*omegaSin;
		rotY(1,2) = kappaSin*phiCos*omegaCos;
		rotY(1,3) = -rotY(1,0)*param1.tX - rotY(1,1)*param1.tY - rotY(1,2)*param1.tZ;
		rotY(2,0) = -phiCos;
		rotY(2,1) = -phiSin*omegaSin;
		rotY(2,2) = -phiSin*omegaCos;
		rotY(2,3) = -rotY(2,0)*param1.tX - rotY(2,1)*param1.tY - rotY(2,2)*param1.tZ;
		rotY(3,3) = param1.scale;	

		// rot around Z axis
		Eigen::Matrix4d rotZ;
		rotZ.setZero();
		rotZ(0,0) = -kappaSin*phiCos;
		rotZ(0,1) = -kappaSin*phiSin*omegaSin-kappaCos*omegaCos;
		rotZ(0,2) = -kappaSin*phiSin*omegaCos+kappaCos*omegaSin;
		rotZ(0,3) = -rotZ(0,0)*param1.tX - rotZ(0,1)*param1.tY - rotZ(0,2)*param1.tZ;
		rotZ(1,0) = kappaCos*phiCos;
		rotZ(1,1) = kappaCos*phiSin*omegaSin-kappaSin*omegaCos;
		rotZ(1,2) = kappaCos*phiSin*omegaCos+kappaSin*omegaSin;
		rotZ(1,3) = -rotZ(1,0)*param1.tX - rotZ(1,1)*param1.tY - rotZ(1,2)*param1.tZ;
		rotZ(3,3) = param1.scale;	
		ensure(" rotation around x axis ", rot0.getMatrix() == rotX);
		ensure(" rotation around y axis ", rot1.getMatrix() == rotY);
		ensure(" rotation around z axis ", rot2.getMatrix() == rotZ);

		}
	
	// Test  translation partial derivative for an inverse transformation
	template<>
	template<>
	void object::test<17>()
	{
		using namespace LGC;
		set_test_name("Testing translation partial derivative for an inverse transformation");

		auto GON(TAngle::kGons);		

		TInverseTransformation invTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		invTransfo.setTransformParam(param1);
		TDerivativeTransformation transl0 = invTransfo.differentiatedTransformationTranslation(0);
		TDerivativeTransformation transl1 = invTransfo.differentiatedTransformationTranslation(1);
		TDerivativeTransformation transl2 = invTransfo.differentiatedTransformationTranslation(2);

		//Test if the matrix is well completed
		TReal kappaCos = param1.kappa.cosine();
		TReal kappaSin = param1.kappa.sine();
		TReal phiCos = param1.phi.cosine();
		TReal phiSin = param1.phi.sine();
		TReal omegaCos = param1.omega.cosine();
		TReal omegaSin = param1.omega.sine();

		// translation around X axis
		ensure_distance(" transl0 -> m(0,0) ", transl0.getMmatrixIJPosition(0,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(0,1) ", transl0.getMmatrixIJPosition(0,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(0,2) ", transl0.getMmatrixIJPosition(0,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(0,3) ", transl0.getMmatrixIJPosition(0,3), -kappaCos*phiCos, 1e-6);
		ensure_distance(" transl0 -> m(1,0) ", transl0.getMmatrixIJPosition(1,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(1,1) ", transl0.getMmatrixIJPosition(1,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(1,2) ", transl0.getMmatrixIJPosition(1,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(1,3) ", transl0.getMmatrixIJPosition(1,3), -kappaSin*phiCos, 1e-6);
		ensure_distance(" transl0 -> m(2,0) ", transl0.getMmatrixIJPosition(2,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,1) ", transl0.getMmatrixIJPosition(2,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,2) ", transl0.getMmatrixIJPosition(2,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,3) ", transl0.getMmatrixIJPosition(2,3), phiSin, 1e-6);
		ensure_distance(" transl0 -> m(2,0) ", transl0.getMmatrixIJPosition(3,0), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,1) ", transl0.getMmatrixIJPosition(3,1), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,2) ", transl0.getMmatrixIJPosition(3,2), 0.0, 1e-6);
		ensure_distance(" transl0 -> m(2,3) ", transl0.getMmatrixIJPosition(3,3), param1.scale, 1e-6);

		// translation around Z axis
		ensure_distance(" transl2 -> m(0,2) ", transl2.getMmatrixIJPosition(0,2), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(0,3) ", transl2.getMmatrixIJPosition(0,3), -kappaCos*phiSin*omegaCos-kappaSin*omegaSin, 1e-6);
		ensure_distance(" transl2 -> m(1,0) ", transl2.getMmatrixIJPosition(1,0), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(1,3) ", transl2.getMmatrixIJPosition(1,3), -kappaSin*phiSin*omegaCos+kappaCos*omegaSin, 1e-6);
		ensure_distance(" transl2 -> m(2,1) ", transl2.getMmatrixIJPosition(2,1), 0.0, 1e-6);
		ensure_distance(" transl2 -> m(2,3) ", transl2.getMmatrixIJPosition(2,3), -phiCos*omegaCos, 1e-6);
		ensure_distance(" transl0 -> m(2,3) ", transl2.getMmatrixIJPosition(3,3), param1.scale, 1e-6);

		// translation around Y axis
		ensure_distance(" transl1 -> m(0,0) ", transl1.getMmatrixIJPosition(0,0), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(0,3) ", transl1.getMmatrixIJPosition(0,3), -kappaCos*phiSin*omegaSin+kappaSin*omegaCos, 1e-6);
		ensure_distance(" transl1 -> m(1,2) ", transl1.getMmatrixIJPosition(1,2), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(1,3) ", transl1.getMmatrixIJPosition(1,3), -kappaSin*phiSin*omegaSin-kappaCos*omegaCos, 1e-6);
		ensure_distance(" transl1 -> m(2,1) ", transl1.getMmatrixIJPosition(2,1), 0.0, 1e-6);
		ensure_distance(" transl1 -> m(2,3) ", transl1.getMmatrixIJPosition(2,3), -phiCos*omegaSin, 1e-6);
		ensure_distance(" transl0 -> m(2,3) ", transl1.getMmatrixIJPosition(3,3), param1.scale, 1e-6);
	}
	
	// Test  scale factor partial derivative for an inverse transformation
	template<>
	template<>
	void object::test<18>()
	{
		using namespace LGC;
		set_test_name("Testing scale factor partial derivative for an inverse transformation");

		auto GON(TAngle::kGons);		

		TInverseTransformation invTransfo;
		TDerivativeTransformation derivTransfo;
		
		TransformParameters param1;
		param1.omega = TAngle(200.0, GON);
		param1.phi = TAngle(-100.0, GON);
		param1.kappa = TAngle(100.0, GON);
		param1.tX = TLength(10.0);
		param1.tY = TLength(10.0);
		param1.tZ = TLength(-10.0);
		param1.scale = 2;

		invTransfo.setTransformParam(param1);
		derivTransfo=invTransfo.differentiatedTransformationScaleFactor();

		//Test if the matrix is well completed
		TReal kappacos = param1.kappa.cosine();
		TReal kappasin = param1.kappa.sine();
		TReal phicos = param1.phi.cosine();
		TReal phisin = param1.phi.sine();
		TReal omegacos = param1.omega.cosine();
		TReal omegasin = param1.omega.sine();

		//Test if the matrix is well completed
		ensure_distance("m(0,0)", derivTransfo.getMmatrixIJPosition(0,0), kappacos*phicos, 1e-6);
		ensure_distance("m(0,1)", derivTransfo.getMmatrixIJPosition(0,1), kappacos*phisin*omegasin-kappasin*omegacos, 1e-6);
		ensure_distance("m(0,2)", derivTransfo.getMmatrixIJPosition(0,2), kappacos*phisin*omegacos+kappasin*omegasin, 1e-6);
		ensure_distance("m(0,3)", derivTransfo.getMmatrixIJPosition(0,3), -(kappacos*phicos)*param1.tX.getMetresValue() - (kappacos*phisin*omegasin-kappasin*omegacos)*param1.tY.getMetresValue() - (kappacos*phisin*omegacos+kappasin*omegasin)*param1.tZ.getMetresValue(), 1e-6);
		ensure_distance("m(1,0)", derivTransfo.getMmatrixIJPosition(1,0), kappasin*phicos, 1e-6);
		ensure_distance("m(1,1)", derivTransfo.getMmatrixIJPosition(1,1),  kappasin*phisin*omegasin+kappacos*omegacos, 1e-6);
		ensure_distance("m(1,2)", derivTransfo.getMmatrixIJPosition(1,2), kappasin*phisin*omegacos-kappacos*omegasin, 1e-6);
		ensure_distance("m(1,3)", derivTransfo.getMmatrixIJPosition(1,3), -(kappasin*phicos)*param1.tX.getMetresValue() - (kappasin*phisin*omegasin+kappacos*omegacos)*param1.tY.getMetresValue() - (kappasin*phisin*omegacos-kappacos*omegasin)*param1.tZ.getMetresValue(), 1e-6);
		ensure_distance("m(2,0)", derivTransfo.getMmatrixIJPosition(2,0), -phisin, 1e-6);
		ensure_distance("m(2,1)", derivTransfo.getMmatrixIJPosition(2,1), phicos*omegasin, 1e-6);
		ensure_distance("m(2,2)", derivTransfo.getMmatrixIJPosition(2,2), phicos*omegacos, 1e-6);
		ensure_distance("m(2,3)", derivTransfo.getMmatrixIJPosition(2,3), -(-phisin)*param1.tX.getMetresValue() - (phicos*omegasin)*param1.tY.getMetresValue() - (phicos*omegacos)*param1.tZ.getMetresValue(), 1e-6);
		ensure_distance("m(2,0)", derivTransfo.getMmatrixIJPosition(3,0), 0.0, 1e-6);
		ensure_distance("m(2,1)", derivTransfo.getMmatrixIJPosition(3,1), 0.0, 1e-6);
		ensure_distance("m(2,2)", derivTransfo.getMmatrixIJPosition(3,2), 0.0, 1e-6);
		ensure_distance("m(3,3)", derivTransfo.getMmatrixIJPosition(3,3), -pow2q(param1.scale), 1e-6);

	}

	// Test  scale factor partial derivative for an inverse transformation
	template<>
	template<>
	void object::test<19>()
	{
		using namespace LGC;
		set_test_name("Testing direct trafo with translation");

		auto GON(TAngle::kGons);		

		TDirectTransformation transfo;
		
		TransformParameters param1;
		param1.omega = TAngle(0, GON);
		param1.phi = TAngle(0, GON);
		param1.kappa = TAngle(0, GON);
		param1.tX = TLength(200.0);
		param1.tY = TLength(0.0);
		param1.tZ = TLength(0.0);
		param1.scale = 1;

		transfo.setTransformParam(param1);

		TPositionVector vector_to_transform(0, 0, 0, k3DCartesian);		
		TInverseTransformation transfoInv;
		
		TransformParameters param2;
		param2.omega = TAngle(0, GON);
		param2.phi = TAngle(0, GON);
		param2.kappa = TAngle(0, GON);
		param2.tX = TLength(0.0);
		param2.tY = TLength(100.0);
		param2.tZ = TLength(0.0);
		param2.scale = 1;

		transfoInv.setTransformParam(param2);

		TTransformation a;
		a = transfo*transfoInv;
		a.transform(vector_to_transform);

		// To Do
	}

	// Test partial derivatives with respect to angles using the chainrule
	template<>
	template<>
	void object::test<20>()
	{
		using namespace LGC;
		set_test_name("Testing angle partial derivatives");
		auto GON(TAngle::kGons);		
		TDirectTransformation transfo;
		TransformParameters param;
		param.omega = TAngle(10, GON);
		param.phi = TAngle(20, GON);
		param.kappa = TAngle(30, GON);
		param.tX = TLength(1.0);
		param.tY = TLength(2.0);
		param.tZ = TLength(3.0);
		param.scale = 4;
		transfo.setTransformParam(param);

		TPositionVector point(1, 2, 3, k3DCartesian);
		TInverseTransformation transfoInv;
		transfoInv.setTransformParam(param);
		TPositionVector pointInverse = transfoInv * point;
		// compute partial derivatives with respect to p of (p,x)->H(p,H_inv(p,x))
		// (all partial derivatives with respect to helmert parameters need to vanish because the map is equal to (p,x)->x and thus does not depend on p)
		for (int i = 0; i < 3; i++)
		{
			// partial derivative wrt position is scale*rotationmatrix, get rotation matrix from homogenuous trafo matrix
			TDenseMatrix dHdxAtPointInverse(3, 3);
			TDenseMatrix Mhomogenous = transfo.getMatrix();
			dHdxAtPointInverse = transfo.getScaleFactor() * Mhomogenous.topLeftCorner(3, 3);
			TDerivativeTransformation dInvHdp = transfoInv.differentiatedTransformationAngle(i);
			Eigen::VectorXd dinvHdpAtPoint(3);
			dinvHdpAtPoint << (dInvHdp * point).getX() , (dInvHdp * point).getY() , (dInvHdp * point).getZ();
			TDerivativeTransformation dHdp = transfo.differentiatedTransformationAngle(i);
			Eigen::VectorXd dHdpAtPointInverse(3);
			dHdpAtPointInverse << (dHdp * pointInverse).getX(), (dHdp * pointInverse).getY(), (dHdp * pointInverse).getZ();
			Eigen::VectorXd testZero = dHdpAtPointInverse + dHdxAtPointInverse * dinvHdpAtPoint;
			ensure("Partial derivatives with respect to angles need to satisfy chain rule", testZero.isZero());
		}
	}

	// Test partial derivatives with respect to translations using the chainrule
	template<>
	template<>
	void object::test<21>()
	{
		using namespace LGC;
		set_test_name("Testing translation partial derivatives");
		auto GON(TAngle::kGons);		
		TDirectTransformation transfo;
		TransformParameters param;
		param.omega = TAngle(10, GON);
		param.phi = TAngle(20, GON);
		param.kappa = TAngle(30, GON);
		param.tX = TLength(1.0);
		param.tY = TLength(2.0);
		param.tZ = TLength(3.0);
		param.scale = 4;
		transfo.setTransformParam(param);

		TPositionVector point(1, 2, 3, k3DCartesian);
		TInverseTransformation transfoInv;
		transfoInv.setTransformParam(param);
		TPositionVector pointInverse = transfoInv * point;
		// compute partial derivatives with respect to p of (p,x)->H(p,H_inv(p,x))
		// (all partial derivatives with respect to helmert parameters need to vanish because the map is equal to (p,x)->x and thus does not depend on p)
		for (int i = 0; i < 3; i++)
		{
			// partial derivative wrt position is scale*rotationmatrix, get rotation matrix from homogenuous trafo matrix
			TDenseMatrix dHdxAtPointInverse(3, 3);
			TDenseMatrix Mhomogenous = transfo.getMatrix();
			dHdxAtPointInverse = transfo.getScaleFactor() * Mhomogenous.topLeftCorner(3, 3);
			TDerivativeTransformation dInvHdp = transfoInv.differentiatedTransformationTranslation(i);
			Eigen::VectorXd dinvHdpAtPoint(3);
			dinvHdpAtPoint << (dInvHdp * point).getX() , (dInvHdp * point).getY() , (dInvHdp * point).getZ();
			TDerivativeTransformation dHdp = transfo.differentiatedTransformationTranslation(i);
			Eigen::VectorXd dHdpAtPointInverse(3);
			dHdpAtPointInverse << (dHdp * pointInverse).getX(), (dHdp * pointInverse).getY(), (dHdp * pointInverse).getZ();
			Eigen::VectorXd testZero = dHdpAtPointInverse + dHdxAtPointInverse * dinvHdpAtPoint;
			ensure("Partial derivatives with respect to translations need to satisfy chain rule", testZero.isZero());
		}
	}
	
	// Test partial derivatives with respect to scale using the chainrule
	template<>
	template<>
	void object::test<22>()
	{
		using namespace LGC;
		set_test_name("Testing scale partial derivatives");
		auto GON(TAngle::kGons);		
		TDirectTransformation transfo;
		TransformParameters param;
		param.omega = TAngle(10, GON);
		param.phi = TAngle(20, GON);
		param.kappa = TAngle(30, GON);
		param.tX = TLength(1.0);
		param.tY = TLength(2.0);
		param.tZ = TLength(3.0);
		param.scale = 4;
		transfo.setTransformParam(param);

		TPositionVector point(1, 2, 3, k3DCartesian);
		TInverseTransformation transfoInv;
		transfoInv.setTransformParam(param);
		TPositionVector pointInverse = transfoInv * point;
		// compute partial derivatives with respect to p of (p,x)->H(p,H_inv(p,x))
		// (all partial derivatives with respect to helmert parameters need to vanish because the map is equal to (p,x)->x and thus does not depend on p)
		// partial derivative wrt position is scale*rotationmatrix, get rotation matrix from homogenuous trafo matrix
		TDenseMatrix dHdxAtPointInverse(3, 3);
		TDenseMatrix Mhomogenous = transfo.getMatrix();
		dHdxAtPointInverse = transfo.getScaleFactor() * Mhomogenous.topLeftCorner(3, 3);
		TDerivativeTransformation dInvHdp = transfoInv.differentiatedTransformationScaleFactor();
		Eigen::VectorXd dinvHdpAtPoint(3);
		dinvHdpAtPoint << (dInvHdp * point).getX(), (dInvHdp * point).getY(), (dInvHdp * point).getZ();
		TDerivativeTransformation dHdp = transfo.differentiatedTransformationScaleFactor();
		Eigen::VectorXd dHdpAtPointInverse(3);
		dHdpAtPointInverse << (dHdp * pointInverse).getX(), (dHdp * pointInverse).getY(), (dHdp * pointInverse).getZ();
		Eigen::VectorXd testZero = dHdpAtPointInverse + dHdxAtPointInverse * dinvHdpAtPoint;
		ensure("Partial derivative with respect to scale need to satisfy chain rule", testZero.isZero());
	}

}
