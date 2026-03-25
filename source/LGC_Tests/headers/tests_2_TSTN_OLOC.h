/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_COMPLEX_H
#define _LGC_TESTS_COMPLEX_H

namespace TestROOT
{
/*
	TEST TWO TOTAL STATIONS : ANGLE + ZENITH + DISTANCE
	Two Total Station looking two variable points and a Cala point
	Angle, Zenith & Distance are measured by total stations
	z values are in the same plane
*/
char const *const Param_setup4 = "*TITR\n"
								 "Testing Input Matrices Filler.\n"
								 "*OLOC\n"
								 "*PREC 7\n"

								 "*INSTR\n"
								 "*POLAR TS1 T1 0.0 0.1 0.1 0\n"
								 "T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

								 "*CALA\n"
								 "STN  0 0 0\n"
								 "REF  100 0 0\n"

								 "*POIN\n"
								 "PT  0 100 0\n"
								 "PT2  100 100 0\n"

								 "*TSTN STN TS1 IHFIX\n"
								 "*V0\n"
								 "*ANGL \n"
								 "PT2 60\n"
								 "REF 110\n"
								 "PT  10\n"
								 "*ZEND\n"
								 "PT2 100\n"
								 "REF 100\n"
								 "PT  100\n"
								 "*DIST\n"
								 "PT2 141.421356237\n"
								 "REF 100\n"
								 "PT  100\n"

								 "*TSTN PT2 TS1 IHFIX\n"
								 "*V0\n"
								 "*ANGL \n"
								 "STN 240\n"
								 "REF 190\n"
								 "PT  290\n"
								 "*ZEND\n"
								 "STN 100\n"
								 "REF 100\n"
								 "PT  100\n"
								 "*DIST\n"
								 "STN 141.421356237\n"
								 "REF 100\n"
								 "PT  100\n";

/*
	TEST TWO TOTAL STATIONS : POLAR3D
	Two Total Station looking two variable points and a Cala point
	Polar3D coordinates are measured by total stations
	z values are in the same plane
*/
char const *const PLR3D_setup4 = "*TITR\n"
								 "Testing Input Matrices Filler.\n"
								 "*OLOC\n"
								 "*PREC 7\n"

								 "*INSTR\n"
								 "*POLAR TS1 T1 0.0 0.1 0.1 0\n"
								 "T1 5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

								 "*CALA\n"
								 "STN  0 0 0\n"
								 "REF  100 0 0\n"

								 "*POIN\n"
								 "PT  0 100 0\n"
								 "PT2  100 100 0\n"

								 "*TSTN STN TS1 IHFIX\n"
								 "*V0\n"
								 "*PLR3D\n"
								 "PT2  250  100  141.421356237\n"
								 "REF 300 100 100  \n"
								 "PT 200 100 100  \n"

								 "*TSTN PT2 TS1 IHFIX\n"
								 "*V0\n"
								 "*PLR3D\n"
								 "STN  240  100  141.421356237\n"
								 "REF 190 100 100  \n"
								 "PT 290 100 100  \n";

/*
TEST TWO TOTAL STATIONS : ANGLE + ZENITH + DISTANCE
Two Total Station looking two variable points and a Cala point
Angle, Zenith & Distance are measured by total stations
z values are in a different plane
*/
char const *const Param_setup4_different_z = "*TITR\n"
											 "Testing Input Matrices Filler.\n"
											 "*OLOC\n"
											 "*PREC 7\n"

											 "*INSTR\n"
											 "*POLAR TS1 T1 0.0 0.1 0.1 0\n"
											 "T1  5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

											 "*CALA\n"
											 "STN  0 0 0\n"
											 "REF  100 0 0\n"

											 "*POIN\n"
											 "PT  0 100 1\n"
											 "PT2  100 100 1\n"

											 "*TSTN STN TS1 IHFIX\n"
											 "*V0\n"
											 "*ANGL \n"
											 "PT2 60\n"
											 "REF 110\n"
											 "PT  10\n"
											 "*ZEND\n"
											 "PT2 99.549849344333\n"
											 "REF 100\n"
											 "PT  99.363401447\n"
											 "*DIST\n"
											 "PT2 141.424891727\n"
											 "REF 100\n"
											 "PT  100.004999875\n"

											 "*TSTN PT2 TS1 IHFIX\n"
											 "*V0\n"
											 "*ANGL \n"
											 "STN 240\n"
											 "REF 190\n"
											 "PT  290\n"
											 "*ZEND\n"
											 "STN 100.450150655667\n"
											 "REF 100.636598552981\n"
											 "PT  100\n"
											 "*DIST\n"
											 "STN 141.424891727\n"
											 "REF 100.004999875\n"
											 "PT  100\n"

	;

/*
	TEST TWO TOTAL STATIONS : POLAR3D
	Two Total Station looking two variable points and a Cala point
	Polar3D coordinates are measured by total stations
	z values are in a different plane
*/
char const *const PLR3D_setup4_different_z = "*TITR\n"
											 "Testing Input Matrices Filler.\n"
											 "*OLOC\n"
											 "*PREC 7\n"

											 "*INSTR\n"
											 "*POLAR TS1 T1 0.0 0.1 0.1 0\n"
											 "T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"

											 "*CALA\n"
											 "STN  0 0 0\n"
											 "REF  100 0 0\n"

											 "*POIN\n"
											 "PT  0 100 1\n"
											 "PT2  100 100 1\n"

											 "*TSTN STN TS1 IHFIX\n"
											 "*V0\n"
											 "*PLR3D\n"
											 "PT2  60  99.549849344333 141.424891727\n"
											 "REF 110 100 100\n"
											 "PT 10 99.363401447 100.004999875\n"

											 "*TSTN PT2 TS1 IHFIX\n"
											 "*V0\n"
											 "*PLR3D\n"
											 "STN  240 100.450150655667 141.424891727\n"
											 "REF 190 100.636598552981 100.004999875\n"
											 "PT 290 100 100\n";

} // namespace TestROOT

#endif
