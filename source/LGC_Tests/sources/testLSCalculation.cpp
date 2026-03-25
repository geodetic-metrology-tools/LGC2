// SPDX-FileCopyrightText: CERN
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma warning(push)
#pragma warning(disable : 4512)
#include <tut/tut.hpp>
#pragma warning(pop)

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <Eigen/LU>

#include "TSparseMatrix.h"
#include "lsalgo/TLSCombinedMtdComputer.h"
#include "lsalgo/TLSInputMatrices.h"
#include "lsalgo/TLSParametricMtdComputer.h"
#include "lsalgo/TLSResultsMatrices.h"
#include "lsalgo/TLSWeightedUnkMtdComputer.h"

namespace tut
{
struct test_LSCalculations
{
};
typedef test_group<test_LSCalculations> factory;
typedef factory::object object;
} // namespace tut

namespace
{
tut::factory tf("Test of least squares calculation classes");
}

namespace tut
{
template<>
template<>
void object::test<1>()
{
	set_test_name("Testing TLSParametricMtdComputer class");

	TLSInputMatrices in = TLSInputMatrices();
	in.setDimensions(2, 5, 5, 0);
	// Weight matrix
	for (int i = 0; i < 3; i++)
	{
		in.setWeightMtrxElement(i, i, 4);
		in.setWeightInvMtrxElement(i, i, 0.25);
	}
	in.setWeightInvMtrxElement(3, 3, 0.0001);
	in.setWeightInvMtrxElement(4, 4, 0.0001);
	in.setWeightMtrxElement(3, 3, 10000);
	in.setWeightMtrxElement(4, 4, 10000);

	// First DM
	for (int i = 0; i < 3; i++)
	{
		in.setFirstDgnMtrxElement(i, 0, 0.7346);
		in.setFirstDgnMtrxElement(i, 1, 0.6785);
	}
	for (int i = 3; i < 5; i++)
	{
		in.setFirstDgnMtrxElement(i, 0, 0.3393);
		in.setFirstDgnMtrxElement(i, 1, -0.3673);
	}

	// misclosure vector
	in.setMisclosureVectorElement(0, -0.0572);
	in.setMisclosureVectorElement(1, 0.4328);
	in.setMisclosureVectorElement(2, 0.2028);
	in.setMisclosureVectorElement(3, 46.6536);
	in.setMisclosureVectorElement(4, 46.6537);

	TLSResultsMatrices rm = TLSResultsMatrices(2, 5);

	TLSParametricMtdComputer ParamMtd;
	ParamMtd = TLSParametricMtdComputer();
	ParamMtd.computeResultsMtrs(&in, &rm);
	ensure_equals("dx(0)  is equal in matlab to -63.4464", rm.getSolutionVctrElmt(0), -63.4464, 1e-3);
	ensure_equals("dx(1)  is equal in matlab to 68.4081", rm.getSolutionVctrElmt(1), 68.4081, 1e-3);

	ParamMtd.calcResiduAndVarCovMatrice(&in, &rm);
	ensure_equals("V(0)  is equal in matlab to 0.25", rm.getResidualsVctrElmt(0), 0.25, 1e-3);
	ensure_equals("V(1)  is equal in matlab to -0.24", rm.getResidualsVctrElmt(1), -0.24, 1e-3);
	ensure_equals("V(2)  is equal in matlab to -0.01", rm.getResidualsVctrElmt(2), -0.01, 1e-3);
	ensure_equals("V(3)  is equal in matlab to 0", rm.getResidualsVctrElmt(3), 0, 1e-3);
	ensure_equals("V(4)  is equal in matlab to 0.0001", rm.getResidualsVctrElmt(4), 0.0001, 1e-3);

	ensure_equals("CovVarV(0,0)  is equal in matlab to 0.1667", rm.getUnknownsCovarMtrxElmt(0, 0), 0.1667, 1e-3);
	ensure_equals("CovVarV(1,1)  is equal in matlab to 0.1667", rm.getUnknownsCovarMtrxElmt(1, 1), 0.1667, 1e-3);
	ensure_equals("CovVarV(2,2)  is equal in matlab to 0.1667", rm.getUnknownsCovarMtrxElmt(2, 2), 0.1667, 1e-3);
	ensure_equals("CovVarV(3,3)  is equal in matlab to 0.0001", rm.getUnknownsCovarMtrxElmt(3, 3), 0.0001, 1e-3);
	ensure_equals("CovVarV(4,4)  is equal in matlab to 0.0001", rm.getUnknownsCovarMtrxElmt(4, 4), 0.0001, 1e-3);

	// rm.saveMatricesToFile(0);
}

template<>
template<>
void object::test<2>()
{
	set_test_name("Testing CombinedMtd");

	TLSInputMatrices in = TLSInputMatrices();
	in.setDimensions(2, 9, 18, 0);
	// Weight matrix
	for (int i = 0; i < 18; i++)
	{
		in.setWeightMtrxElement(i, i, 1);
		in.setWeightInvMtrxElement(i, i, 1);
	}

	// First DM
	for (int i = 0; i < 9; i++)
	{
		in.setFirstDgnMtrxElement(i, 1, -1);
	}
	in.setFirstDgnMtrxElement(0, 0, 3.1);
	in.setFirstDgnMtrxElement(1, 0, 1.2);
	in.setFirstDgnMtrxElement(2, 0, -1.9);
	in.setFirstDgnMtrxElement(3, 0, -4.2);
	in.setFirstDgnMtrxElement(4, 0, -6.7);
	in.setFirstDgnMtrxElement(5, 0, -8.6);
	in.setFirstDgnMtrxElement(6, 0, -10.9);
	in.setFirstDgnMtrxElement(7, 0, -13.9);
	in.setFirstDgnMtrxElement(8, 0, -18.1);

	// Second DM
	for (int i = 0; i < 9; i++)
	{
		in.setSecondDgnMtrxElement(i, 2 * i, -0.63);
		in.setSecondDgnMtrxElement(i, 2 * i + 1, 1);
	}

	// misclosure vector
	in.setMisclosureVectorElement(0, -0.007);
	in.setMisclosureVectorElement(1, -0.004);
	in.setMisclosureVectorElement(2, -0.457);
	in.setMisclosureVectorElement(3, -0.906);
	in.setMisclosureVectorElement(4, -0.481);
	in.setMisclosureVectorElement(5, -1.278);
	in.setMisclosureVectorElement(6, -1.927);
	in.setMisclosureVectorElement(7, -1.817);
	in.setMisclosureVectorElement(8, -2.863);

	TLSResultsMatrices rm = TLSResultsMatrices(2, 9);

	TLSCombinedMtdComputer CombinedMtd;
	CombinedMtd = TLSCombinedMtdComputer();
	CombinedMtd.computeResultsMtrs(&in, &rm);

	ensure_equals("dx(0)  is equal in matlab to -0.13309", rm.getSolutionVctrElmt(0), -0.13309, 1e-3);
	ensure_equals("dx(1)  is equal in matlab to -0.19497", rm.getSolutionVctrElmt(1), -0.19497, 1e-3);

	CombinedMtd.calcResiduAndVarCovMatrice(&in, &rm);
	ensure_equals("V(0)  is equal in matlab to 0.1013", rm.getResidualsVctrElmt(0), 0.1013, 1e-3);
	ensure_equals("V(1)  is equal in matlab to -0.1608", rm.getResidualsVctrElmt(1), -0.1608, 1e-3);
	ensure_equals("V(2)  is equal in matlab to -0.0141", rm.getResidualsVctrElmt(2), -0.0141, 1e-3);
	ensure_equals("V(3)  is equal in matlab to 0.0224", rm.getResidualsVctrElmt(3), 0.0224, 1e-3);
	ensure_equals("V(4)  is equal in matlab to 0.0041", rm.getResidualsVctrElmt(4), 0.0041, 1e-3);
	ensure_equals("V(5)  is equal in matlab to -0.0066", rm.getResidualsVctrElmt(5), -0.0066, 1e-3);
	ensure_equals("V(6)  is equal in matlab to 0.0686", rm.getResidualsVctrElmt(6), 0.0686, 1e-3);
	ensure_equals("V(7)  is equal in matlab to -0.1089", rm.getResidualsVctrElmt(7), -0.1089, 1e-3);
	ensure_equals("V(8)  is equal in matlab to -0.2732", rm.getResidualsVctrElmt(8), -0.2732, 1e-3);
	ensure_equals("V(9)  is equal in matlab to 0.4336", rm.getResidualsVctrElmt(9), 0.4336, 1e-3);
	ensure_equals("V(10)  is equal in matlab to -0.0277", rm.getResidualsVctrElmt(10), -0.0277, 1e-3);
	ensure_equals("V(11)  is equal in matlab to 0.0440", rm.getResidualsVctrElmt(11), 0.0440, 1e-3);
	ensure_equals("V(12)  is equal in matlab to 0.1269", rm.getResidualsVctrElmt(12), 0.1269, 1e-3);
	ensure_equals("V(13)  is equal in matlab to -0.2014", rm.getResidualsVctrElmt(13), -0.2014, 1e-3);
	ensure_equals("V(14)  is equal in matlab to -0.1028", rm.getResidualsVctrElmt(14), -0.1028, 1e-3);
	ensure_equals("V(15)  is equal in matlab to 0.1631", rm.getResidualsVctrElmt(15), 0.1631, 1e-3);
	ensure_equals("V(16)  is equal in matlab to 0.1169", rm.getResidualsVctrElmt(16), 0.1169, 1e-3);
	ensure_equals("V(17)  is equal in matlab to -0.1855", rm.getResidualsVctrElmt(17), -0.1855, 1e-3);

	ensure_equals("CovVarV(0,0)  is equal in matlab to 0.1832", rm.getUnknownsCovarMtrxElmt(0, 0), 0.1832, 1e-3);
	ensure_equals("CovVarV(1,1)  is equal in matlab to 0.4616", rm.getUnknownsCovarMtrxElmt(1, 1), 0.4616, 1e-3);
	ensure_equals("CovVarV(2,2)  is equal in matlab to 0.2076", rm.getUnknownsCovarMtrxElmt(2, 2), 0.2076, 1e-3);
	ensure_equals("CovVarV(3,3)  is equal in matlab to 0.5230", rm.getUnknownsCovarMtrxElmt(3, 3), 0.5230, 1e-3);
	ensure_equals("CovVarV(4,4)  is equal in matlab to 0.2360", rm.getUnknownsCovarMtrxElmt(4, 4), 0.2360, 1e-3);
	ensure_equals("CovVarV(5,5)  is equal in matlab to 0.5947", rm.getUnknownsCovarMtrxElmt(5, 5), 0.5947, 1e-3);
	ensure_equals("CovVarV(6,6)  is equal in matlab to 0.2481", rm.getUnknownsCovarMtrxElmt(6, 6), 0.2481, 1e-3);
	ensure_equals("CovVarV(7,7)  is equal in matlab to 0.6252", rm.getUnknownsCovarMtrxElmt(7, 7), 0.6252, 1e-3);
	ensure_equals("CovVarV(8,8)  is equal in matlab to 0.2526", rm.getUnknownsCovarMtrxElmt(8, 8), 0.2526, 1e-3);
	ensure_equals("CovVarV(9,9)  is equal in matlab to 0.6363", rm.getUnknownsCovarMtrxElmt(9, 9), 0.6363, 1e-3);
	ensure_equals("CovVarV(10,10)  is equal in matlab to 0.2498", rm.getUnknownsCovarMtrxElmt(10, 10), 0.2498, 1e-3);
	ensure_equals("CovVarV(11,11)  is equal in matlab to 0.6295", rm.getUnknownsCovarMtrxElmt(11, 11), 0.6295, 1e-3);
	ensure_equals("CovVarV(12,12)  is equal in matlab to 0.2395", rm.getUnknownsCovarMtrxElmt(12, 12), 0.2395, 1e-3);
	ensure_equals("CovVarV(13,13)  is equal in matlab to 0.6035", rm.getUnknownsCovarMtrxElmt(13, 13), 0.6035, 1e-3);
	ensure_equals("CovVarV(14,14)  is equal in matlab to 0.2145", rm.getUnknownsCovarMtrxElmt(14, 14), 0.2145, 1e-3);
	ensure_equals("CovVarV(15,15)  is equal in matlab to 0.5405", rm.getUnknownsCovarMtrxElmt(15, 15), 0.5405, 1e-3);
	ensure_equals("CovVarV(16,16)  is equal in matlab to 0.1575", rm.getUnknownsCovarMtrxElmt(16, 16), 0.1575, 1e-3);
	ensure_equals("CovVarV(17,17)  is equal in matlab to 0.3969", rm.getUnknownsCovarMtrxElmt(17, 17), 0.3969, 1e-3);

	// rm.saveMatricesToFile(1);
}

template<>
template<>
void object::test<3>()
{
	set_test_name("Testing WeightedUnkMtd");

	TLSInputMatrices in = TLSInputMatrices();
	in.setDimensions(2, 9, 18, 0);
	// Weight matrix
	for (int i = 0; i < 18; i++)
	{
		in.setWeightMtrxElement(i, i, 1);
		in.setWeightInvMtrxElement(i, i, 1);
	}
	in.setWeightUnkMtrxElement(0, 0, 1);
	in.setWeightUnkMtrxElement(1, 1, 2);

	// First DM
	for (int i = 0; i < 9; i++)
	{
		in.setFirstDgnMtrxElement(i, 1, -1);
	}
	in.setFirstDgnMtrxElement(0, 0, 3.1);
	in.setFirstDgnMtrxElement(1, 0, 1.2);
	in.setFirstDgnMtrxElement(2, 0, -1.9);
	in.setFirstDgnMtrxElement(3, 0, -4.2);
	in.setFirstDgnMtrxElement(4, 0, -6.7);
	in.setFirstDgnMtrxElement(5, 0, -8.6);
	in.setFirstDgnMtrxElement(6, 0, -10.9);
	in.setFirstDgnMtrxElement(7, 0, -13.9);
	in.setFirstDgnMtrxElement(8, 0, -18.1);

	// Second DM
	for (int i = 0; i < 9; i++)
	{
		in.setSecondDgnMtrxElement(i, 2 * i, -0.63);
		in.setSecondDgnMtrxElement(i, 2 * i + 1, 1);
	}

	// misclosure vector
	in.setMisclosureVectorElement(0, -0.007);
	in.setMisclosureVectorElement(1, -0.004);
	in.setMisclosureVectorElement(2, -0.457);
	in.setMisclosureVectorElement(3, -0.906);
	in.setMisclosureVectorElement(4, -0.481);
	in.setMisclosureVectorElement(5, -1.278);
	in.setMisclosureVectorElement(6, -1.927);
	in.setMisclosureVectorElement(7, -1.817);
	in.setMisclosureVectorElement(8, -2.863);

	TLSResultsMatrices rm = TLSResultsMatrices(2, 9);

	TLSWeightedUnkMtdComputer WeightedMtd;
	WeightedMtd = TLSWeightedUnkMtdComputer();
	WeightedMtd.computeResultsMtrs(&in, &rm);

	ensure_equals("dx(0)  is equal in matlab to -0.13840", rm.getSolutionVctrElmt(0), -0.13840, 1e-3);
	ensure_equals("dx(1)  is equal in matlab to -0.12177", rm.getSolutionVctrElmt(1), -0.12177, 1e-3);

	WeightedMtd.calcResiduAndVarCovMatrice(&in, &rm);
	ensure_equals("V(0)  is equal in matlab to  -0.141730", rm.getResidualsVctrElmt(0), -0.141730, 1e-3);
	ensure_equals("V(1)  is equal in matlab to 0.224968", rm.getResidualsVctrElmt(1), 0.224968, 1e-3);
	ensure_equals("V(2)  is equal in matlab to -0.021785", rm.getResidualsVctrElmt(2), -0.021785, 1e-3);
	ensure_equals("V(3)  is equal in matlab to 0.034579", rm.getResidualsVctrElmt(3), 0.034579, 1e-3);
	ensure_equals("V(4)  is equal in matlab to -0.032595", rm.getResidualsVctrElmt(4), -0.032595, 1e-3);
	ensure_equals("V(5)  is equal in matlab to  0.051737", rm.getResidualsVctrElmt(5), 0.051737, 1e-3);
	ensure_equals("V(6)  is equal in matlab to -0.091534", rm.getResidualsVctrElmt(6), -0.091534, 1e-3);
	ensure_equals("V(7)  is equal in matlab to 0.145292", rm.getResidualsVctrElmt(7), 0.145292, 1e-3);
	ensure_equals("V(8)  is equal in matlab to 0.256183", rm.getResidualsVctrElmt(8), 0.256183, 1e-3);
	ensure_equals("V(9)  is equal in matlab to -0.406639", rm.getResidualsVctrElmt(9), -0.406639, 1e-3);
	ensure_equals("V(10)  is equal in matlab to 0.015329", rm.getResidualsVctrElmt(10), 0.015329, 1e-3);
	ensure_equals("V(11)  is equal in matlab to -0.024332", rm.getResidualsVctrElmt(11), -0.024332, 1e-3);
	ensure_equals("V(12)  is equal in matlab to -0.133810", rm.getResidualsVctrElmt(12), -0.133810, 1e-3);
	ensure_equals("V(13)  is equal in matlab to 0.212397", rm.getResidualsVctrElmt(13), 0.212397, 1e-3);
	ensure_equals("V(14)  is equal in matlab to 0.103050", rm.getResidualsVctrElmt(14), 0.103050, 1e-3);
	ensure_equals("V(15)  is equal in matlab to -0.163572", rm.getResidualsVctrElmt(15), -0.163572, 1e-3);
	ensure_equals("V(16)  is equal in matlab to -0.106543", rm.getResidualsVctrElmt(16), -0.106543, 1e-3);
	ensure_equals("V(17)  is equal in matlab to 0.169117", rm.getResidualsVctrElmt(17), 0.169117, 1e-3);

	ensure_equals("Sigma02 : 0.029860 ", rm.getSigmaZero2(), 0.029860, 1e-3);

	ensure_equals("CovVarV(0,0)  is equal in matlab to 0.0096802", rm.getUnknownsCovarMtrxElmt(0, 0), 0.0096802, 1e-3);
	ensure_equals("CovVarV(1,1)  is equal in matlab to 0.0243896", rm.getUnknownsCovarMtrxElmt(1, 1), 0.0243896, 1e-3);
	ensure_equals("CovVarV(2,2)  is equal in matlab to 0.0093534", rm.getUnknownsCovarMtrxElmt(2, 2), 0.0093534, 1e-3);
	ensure_equals("CovVarV(3,3)  is equal in matlab to 0.0235661", rm.getUnknownsCovarMtrxElmt(3, 3), 0.0235661, 1e-3);
	ensure_equals("CovVarV(4,4)  is equal in matlab to 0.0090493", rm.getUnknownsCovarMtrxElmt(4, 4), 0.0090493, 1e-3);
	ensure_equals("CovVarV(5,5)  is equal in matlab to 0.0227999", rm.getUnknownsCovarMtrxElmt(5, 5), 0.0227999, 1e-3);
	ensure_equals("CovVarV(6,6)  is equal in matlab to 0.0090073", rm.getUnknownsCovarMtrxElmt(6, 6), 0.0090073, 1e-3);
	ensure_equals("CovVarV(7,7)  is equal in matlab to 0.0226941", rm.getUnknownsCovarMtrxElmt(7, 7), 0.0226941, 1e-3);
	ensure_equals("CovVarV(8,8)  is equal in matlab to 0.0091391", rm.getUnknownsCovarMtrxElmt(8, 8), 0.0091391, 1e-3);
	ensure_equals("CovVarV(9,9)  is equal in matlab to 0.0230261", rm.getUnknownsCovarMtrxElmt(9, 9), 0.0230261, 1e-3);
	ensure_equals("CovVarV(10,10)  is equal in matlab to 0.0093629", rm.getUnknownsCovarMtrxElmt(10, 10), 0.0093629, 1e-3);
	ensure_equals("CovVarV(11,11)  is equal in matlab to 0.0235900", rm.getUnknownsCovarMtrxElmt(11, 11), 0.0235900, 1e-3);
	ensure_equals("CovVarV(12,12)  is equal in matlab to 0.0097766", rm.getUnknownsCovarMtrxElmt(12, 12), 0.0097766, 1e-3);
	ensure_equals("CovVarV(13,13)  is equal in matlab to 0.0246323", rm.getUnknownsCovarMtrxElmt(13, 13), 0.0246323, 1e-3);
	ensure_equals("CovVarV(14,14)  is equal in matlab to 0.0105513", rm.getUnknownsCovarMtrxElmt(14, 14), 0.0105513, 1e-3);
	ensure_equals("CovVarV(15,15)  is equal in matlab to 0.0265843", rm.getUnknownsCovarMtrxElmt(15, 15), 0.0265843, 1e-3);
	ensure_equals("CovVarV(16,16)  is equal in matlab to 0.0120830", rm.getUnknownsCovarMtrxElmt(16, 16), 0.0120830, 1e-3);
	ensure_equals("CovVarV(17,17)  is equal in matlab to 0.0304435", rm.getUnknownsCovarMtrxElmt(17, 17), 0.0304435, 1e-3);

	// rm.saveMatricesToFile(2);
}

} // namespace tut
