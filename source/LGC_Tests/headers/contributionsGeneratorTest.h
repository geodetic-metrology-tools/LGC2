/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_CG_H
#define _LGC_TESTS_CG_H

namespace LGCTestInputCG {
		char const *const CGfile = 
		"*TITR\n"
		"Testing Contributions.\n"
		"*OLOC\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  0.1  0.2  0.1  0.1	  0  0.0  0.1  0.2  0.0  0.0 \n"

		"*LEVEL LEV1 ST1 0 10.0\n"
		"ST1 1 2 0 0 0 0 \n"
		"ST2 1 2 0 0 0 0\n"

		"*CALA\n"
			"P0C  0 0 0\n"
		"*VXY\n"
			"P1C  10 0 0\n"
			"P1Cb  10 10 10\n"

		"*DLEV LEV1 RefPt P0C\n"
			"P1Cb 5\n"

		"*DLEV LEV1 RefPt P0C\n"
			"P1C 7 DHOR 1.0 TRGT ST1 OBSE 0.1 PPM 0.01 TH 1.0 THSE 0.1 DSE 0.1\n"

		"*TSTN P0C TS1\n"
		  "*V0 ACST 10\n"
			"*DIST\n"
				"P1C  10.0\n"
			"*DIST\n"
				"P1C  10.1\n"
			"*DIST\n"
				"P1Cb  17\n"
			"*ANGL\n"
				"P1Cb 50\n"
			"*ZEND\n"
				"P1Cb 52\n"
			"*DHOR\n"
				"P1Cb 10\n"
			"*PLR3D\n"
				"P1Cb  50 52 17\n"

		"*TSTN P1C TS1\n"
		  "*V0\n"
			"*DIST\n"
				"P2C  20.2\n"

		"*CAM P0C TS1\n"
			"*PLR3D\n"
				"P2C  100 100 10\n"
			"*DIR3D\n"
				"P2C  100 100\n"

		"*FRAME fr1 0 0 0  0 0 100 1 RZ\n"
		"*CALA\n"
			"P2C  0 10 0\n"

		"*ENDFRAME\n";


	char const *const CGfileMLA = 
		"*TITR\n"
		"Testing Contributions.\n"
		"*RS2K\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  0.1  0.2  0.1  0.1	  0  0.0  0.1  0.2  0.0  0.0 \n"

		"*CALA\n"
			"P0C  1000 1000 -999.829156146493\n"
		"*VXY\n"
			"P1C  1100 1000 -999.843838976361\n"
			"P1Cb  1100 1100 -899.860239294984\n"

		"*TSTN P0C TS1\n"
		  "*V0\n"
			"*DIST\n"
				"P1C  100.0\n"
			"*DIST\n"
				"P1C  10\n"
			"*DIST\n"
				"P1Cb  170\n"
			"*PLR3D\n"
				"P1Cb  50 52 17\n"

		"*FRAME fr1 0 0 0  0 0 100 1.0\n"
		"*CALA\n"
			"P2C  1000 -900 1000\n"

		"*CAM P2C TS1\n"
			"*PLR3D\n"
				"P1C  100 100 10\n"
			"*DIR3D\n"
				"P1C  100 100\n"

		"*ENDFRAME\n";
}

#endif