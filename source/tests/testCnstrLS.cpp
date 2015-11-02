#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)


#include <TLGCData.h>
#include "lsalgo/TLSInputMatrices.h"
#include "lsalgo/TLSResultsMatrices.h"
#include "TLGCCalculation.h"
#include "lsalgo/TLSCnstMtdComputer.h"

#include "testCnstrLS.h"

namespace tut
{
    struct test_CnstrLS{};
    typedef test_group<test_CnstrLS> factory;
    typedef factory::object object;
}

namespace
{
    tut::factory tf("Cnstr LS tests");
}

namespace tut
{	


	template<>
	template<>
	void object::test<1>()
	{ 
		TLGCData projTest;

		set_test_name("Testing least square solution with constraints");


		std::unique_ptr<TALSComputer> fComputer;
		fComputer.reset(new TLSCnstMtdComputer());

		

		/*To define Z coordinates of P
		We intruce a constraint: ZP = ZSTN*/

		TLSInputMatrices im;
		im.setDimensions(4, 9, 9, 1, 1);
		TLSResultsMatrices rm(4,9,9);
		
		//fill in the misclV
		im.setMisclosureVectorElement(0,-3.2162453e-014);
		im.setMisclosureVectorElement(1,0);
		im.setMisclosureVectorElement(2,-1.60812265e-014);
		im.setMisclosureVectorElement(3,1.67803108e-007);
		im.setMisclosureVectorElement(4,1.67803208e-007);
		im.setMisclosureVectorElement(5,-2.27422886e-014);
		im.setMisclosureVectorElement(6,29.2893219);
		im.setMisclosureVectorElement(7,4.32978028e-015);
		im.setMisclosureVectorElement(8,-70.7106781);

		//fill in the first design matrix
		im.setFirstDgnMtrxElement(0,3,-100);
		im.setFirstDgnMtrxElement(3,3,-100);
		im.setFirstDgnMtrxElement(4,3,100);
		im.setFirstDgnMtrxElement(6,0,-1);
		im.setFirstDgnMtrxElement(7,1,-1);
		im.setFirstDgnMtrxElement(7,3,100);
		im.setFirstDgnMtrxElement(8,2,-1);

		////fill in the second design matrix
		im.setSecondDgnMtrxElement(0,0,-100);
		im.setSecondDgnMtrxElement(0,1,5.17211691e-030);
		im.setSecondDgnMtrxElement(0,2,-3.2162453e-016);
		im.setSecondDgnMtrxElement(1,0,3.2162453e-014);
		im.setSecondDgnMtrxElement(1,1,1.60812265e-014);
		im.setSecondDgnMtrxElement(1,2,-1);
		im.setSecondDgnMtrxElement(2,1,-100);
		im.setSecondDgnMtrxElement(2,2,-1.60812265e-016);
		
		im.setSecondDgnMtrxElement(3,3,-99.9999998);
		im.setSecondDgnMtrxElement(3,4,1.60812265e-014);
		im.setSecondDgnMtrxElement(3,5,-0.707106781);
		im.setSecondDgnMtrxElement(4,3,99.9999998);
		im.setSecondDgnMtrxElement(4,4,1.60812265e-014);
		im.setSecondDgnMtrxElement(4,5,-0.707106781);
		im.setSecondDgnMtrxElement(5,4,-141.421356);
		im.setSecondDgnMtrxElement(5,5,-1.60812265e-016);
		
		im.setSecondDgnMtrxElement(6,6,4.32978028e-015);
		im.setSecondDgnMtrxElement(6,7,70.7106781);
		im.setSecondDgnMtrxElement(6,8,-0.707106781);
		im.setSecondDgnMtrxElement(7,6,70.7106781);
		im.setSecondDgnMtrxElement(7,7,-4.32978028e-015);
		im.setSecondDgnMtrxElement(7,8,4.32978028e-017);
		im.setSecondDgnMtrxElement(8,7,-70.7106781);
		im.setSecondDgnMtrxElement(8,8,-0.707106781);

		//fill in the weight matrix
		im.setWeightMtrxElement(0,0,1.62113894e+010);
		TReal a = 1/1.62113894e+010;
		TReal b = 1/4000000.0;
		im.setWeightInvMtrxElement(0,0,a);
		im.setWeightMtrxElement(1,1,1.62113894e+010);
		im.setWeightInvMtrxElement(1,1,a);
		im.setWeightMtrxElement(2,2,4000000.0);
		im.setWeightInvMtrxElement(2,2,b);
		im.setWeightMtrxElement(3,3,1.62113894e+010);
		im.setWeightInvMtrxElement(3,3,a);
		im.setWeightMtrxElement(4,4,1.62113894e+010);
		im.setWeightInvMtrxElement(4,4,a);
		im.setWeightMtrxElement(5,5,4000000.0);
		im.setWeightInvMtrxElement(5,5,b);
		im.setWeightMtrxElement(6,6,1.62113894e+010);
		im.setWeightInvMtrxElement(6,6,a);
		im.setWeightMtrxElement(7,7,1.62113894e+010);
		im.setWeightInvMtrxElement(7,7,a);
		im.setWeightMtrxElement(8,8,4000000.0);
		im.setWeightInvMtrxElement(8,8,b);

		//fill in the cnstr misclV
		im.setCnstrMisclosureVectorElement(0,0);

		//fill in the first cnstr design matrix
		im.setCnstrFirstDgnMtrxElement(0,0,0);
		im.setCnstrFirstDgnMtrxElement(0,1,0);
		im.setCnstrFirstDgnMtrxElement(0,2,-1);
		im.setCnstrFirstDgnMtrxElement(0,3,0);

		//
		//im.saveMatricesToFile(9999);
		bool computationOK = fComputer->computeResults(&im, &rm);
		if (computationOK)
		{
			fComputer->calcResiduAndVarCovMatrice(&im, &rm);
		}

		//rm.saveMatricesToFile(9999);

		ensure_equals("dX should match",rm.getSolutionVctrElmt(0), -0.635374937 , 1e-2);
		ensure_equals("dY should match",rm.getSolutionVctrElmt(1), -3.4919087e-014, 1e-2);
		ensure_equals("dZ should match",rm.getSolutionVctrElmt(2), 9.58689209e-016, 1e-2);
		ensure_equals("dV0 should match",rm.getSolutionVctrElmt(3),-4.10812265e-016  , 1e-2);

		ensure_equals("residual 0 should match",rm.getResidualsVctrElmt(0),-8.91877353e-017, 1e-7);
		ensure_equals("residual 1 should match",rm.getResidualsVctrElmt(1),1.60812265e-016, 1e-7);
		ensure_equals("residual 2 should match",rm.getResidualsVctrElmt(2),-2.82437888e-031	, 1e-7);
		ensure_equals("residual 3 should match",rm.getResidualsVctrElmt(3),8.91877348e-017, 1e-7);
		ensure_equals("residual 4 should match",rm.getResidualsVctrElmt(4),1.60812265e-016, 1e-7);
		ensure_equals("residual 5 should match",rm.getResidualsVctrElmt(5),-2.37309502e-007	, 1e-7);
		ensure_equals("residual 6 should match",rm.getResidualsVctrElmt(6),6.16297582e-033, 1e-7);
		ensure_equals("residual 7 should match",rm.getResidualsVctrElmt(7),0.711599561, 1e-7);
		ensure_equals("residual 8 should match",rm.getResidualsVctrElmt(8),28.8400439, 1e-7);

	}
}
