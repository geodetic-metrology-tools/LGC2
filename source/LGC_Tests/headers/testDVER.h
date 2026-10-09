/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_DVER_H
#define _LGC_TESTS_DVER_H

namespace TestDVER
{
//-----------------------------------OLOC------------------------------//
char const *const OLOC_DVER_cheminement = "*TITR\n"
										  "Testing Input Matrices Filler.\n"
										  "*OLOC\n"

										  "*CALA\n"
										  "stn	0   0	0\n"
										  "pt5	305	10	50\n"

										  "*VZ\n"
										  "pt0	50	0	9.0\n" // we expect Z = 10
										  "pt1	100	10	50\n" // we expect Z = 50
										  "pt2	140	-10	15.0\n" // we expect Z = 20
										  "pt3	200	0	0.5\n" // we expect Z = 0
										  "pt4	255	5	10\n" // we expect Z = 10

										  "*DVER\n"
										  "stn  pt1 50\n"
										  "stn  pt0   10\n"
										  "pt0  pt1   40\n"
										  "pt1  pt2  -30\n"
										  "pt2  pt3  -20\n"
										  "pt3  pt4   10\n"
										  "pt4  pt5   40\n";

char const *const OLOC_DVER_cheminement_id = "*TITR\n"
											 "Testing ID for DVER observation.\n"
											 "*OLOC\n"

											 "*CALA\n"
											 "stn	0   0	0\n"
											 "pt5	305	10	50\n"

											 "*VZ\n"
											 "pt0	50	0	9.0\n" // we expect Z = 10
											 "pt1	100	10	50\n" // we expect Z = 50
											 "pt2	140	-10	15.0\n" // we expect Z = 20
											 "pt3	200	0	0.5\n" // we expect Z = 0
											 "pt4	255	5	10\n" // we expect Z = 10

											 "*DVER\n"
											 "stn  pt1 50 ID STN_pt1\n"
											 "stn  pt0   10 ID STN_pt0\n"
											 "pt0  pt1   40 ID pt0_pt1\n"
											 "pt1  pt2  -30 ID pt1_pt2\n"
											 "pt2  pt3  -20 ID pt2_pt3\n"
											 "pt3  pt4   10 ID pt3_pt4\n"
											 "pt4  pt5   40 ID pt4_pt5\n";

char const *const OLOC_DVER_cheminement_LOR = "*TITR\n"
											  "Testing Input Matrices Filler.\n"
											  "*OLOC\n"

											  "*CALA\n"
											  "pt1	100	10	50\n"

											  "*FRAME f 0.1 0 100 0 0 0 1 TZ \n"
											  "*CALA\n"
											  "stn	0   0	0\n"
											  "pt5	305	10	50\n"
											  "*ENDFRAME\n"

											  "*VZ\n"
											  "pt0	50	0	9.0\n" // we expect Z = 10
											  "pt2	140	-10	15.0\n" // we expect Z = 20
											  "pt3	200	0	0.5\n" // we expect Z = 0
											  "pt4	255	5	10\n" // we expect Z = 10

											  "*DVER\n"
											  "stn  pt1 50\n"
											  "stn  pt0   10\n"
											  "pt0  pt1   40\n"
											  "pt1  pt2  -30\n"
											  "pt2  pt3  -20\n"
											  "pt3  pt4   10\n"
											  "pt4  pt5   40\n";

//------------------------------RS2K----------------------------------------------//

char const *const RS2K__DVER = "*TITR\n"
							   "Testing Input Matrices Filler.\n"
							   "*RS2K\n"
							   "*PREC 7\n"

							   "*CALA\n"
							   "stn	-1026.53292000015	10475.4085800003	419.8338381\n" // 0	0	0
							   "pt0	-1007.20955606953	10429.3085047217	429.8340334\n" // 50	0	10
							   "pt5	-899.432717608214	10198.0492909704	469.8410426\n" // 305	10	50

							   "*VZ\n"
							   "pt1	-978.677280641929	10387.1133502013	469.8\n" // 100	10	50   expect H = 469.834623440303
							   "pt2	-981.646484690779	10342.452290241	    439.0\n" // 140	-10	20   expect H = 439.835364301096
							   "pt3	-949.22115118667	10290.9558392871	419.8\n" // 200	0	0    expect H = 419.836935885329
							   "pt4	-923.353669877933	10242.1772410198	430.0\n" // 255	5	10   expect H = 429.838872751758

							   "*DVER 0.1\n"
							   "stn  pt0   10.000195275931\n"
							   "pt0  pt1   40.000590054603\n"
							   "pt1  pt2  -29.999259139207\n"
							   "pt2  pt3  -19.998428415767\n"
							   "pt3  pt4   10.001936866429\n"
							   "pt4  pt5   40.002169854217\n"

	//"pt5  pt4    10.000195275931\n"
	//"pt4  pt3    40.000590054603\n"
	//"pt3  pt2   -29.999259139207\n"
	//"pt2  pt1   -19.998428415767\n"
	//"pt1  pt0    10.001936866429\n"
	//"pt0  stn    40.002169854217\n"

	;

/*
	TSTSN + DVER measurements
	Observations are simulated with lgc1
*/

char const *const RS2K_TSTN_DVER = "*TITR\n"
								   "Testing Input Matrices Filler.\n"
								   "*RS2K\n"
								   "*FAUT\n"
								   "*PREC 7\n"
								   "*INSTR\n"
								   "*POLAR TS1 T1 0.0 0.1 1.0 0\n"
								   "T1  5  5  0.5  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

								   "*CALA\n"
								   "STN  -1026.53292000015	10475.4085800003	419.80000000\n" // 0 0 0
								   "REF  -987.881613866058	10383.1953195432	429.20000000\n" // 100 0 10
								   "*POIN\n"
								   "PT  -934.329348216433	10514.1300400932	420.00000000\n" // 0 100 50
								   "PT2  -895.650572445877	10421.8381202376	419.10000000\n" // 100 100 0

								   "*TSTN STN TS1 IHFIX\n"
								   "*V0\n" /*Expected angle */
								   "*DIST\n"
								   "PT2   141.4226500\n"
								   "REF   100.4395544\n"
								   "PT   100.0048963\n"
								   "*ZEND\n"
								   "PT2  100.3156838\n"
								   "REF   94.0337659\n"
								   "PT   99.8732457\n"
								   "*ANGL\n"
								   "PT2 124.7322818\n"
								   "REF 174.7327288\n"
								   "PT  74.6887806\n"

								   "*DVER 0.1\n"
								   "STN   REF       9.4000018\n"
								   "STN   PT       0.2000125\n"
								   "STN   PT2      -0.7000144\n"
								   "REF   PT      -9.2003892\n"
								   "REF   PT2     -10.0999030 \n";

} // namespace TestDVER
#endif //_LGC_TESTS_DVER_H
