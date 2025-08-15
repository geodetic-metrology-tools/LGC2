/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_SIGMA_H
#define _LGC_TESTS_SIGMA_H


namespace TestSUBFRAME {
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////// PLR3D
//////////////////////////////////////////////////////////////////////////////////////////////////

	/*FIXED TRANSFORMATION*/
char const *const BCAM_PLR3D_setup3= 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

		"*CALA\n"
			"STN  0 0 0\n"

		"*POIN\n"
			"PT  0 100 0\n"
			"PT2  100 100 0\n"

		"*FRAME BCAMFrame 100 0 0 0.0 0.0 0.0 1.0\n"
		"*CALA\n"
			"REF  0 0 0\n"
		"*CAM REF TS1\n"
			"*UVEC\n"
				"STN  0  0  1\n"
				"PT 0  0  1\n"
				"PT2 0  0  1 \n"
		"*ENDFRAME\n"
		;


#if 0
/*ROTATION RZ TRANSFORMATION, CAMERA AT THE ORIGIN, JUST TO PROVE  THAT FOR RZ = 200 ALSO DOES NOT WORK*/
char const *const BCAM_PLR3D_CAMORIG = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

		"*CALA\n"
			"REF  200 100 0\n"

		"*POIN\n"
			"PT  100 200 0\n"
			"PT2  200 200 0\n"


		"*FRAME BCAMFrame 100 100 0 0.0 0.0 0.0 1.0 RZ\n"
		"*CALA\n"
			"STN  0 0 0\n"
		"*CAM STN TS1\n"
			"*PLR3D\n"
				"REF  310  100  100\n"
				"PT 210 100 100\n"
				"PT2 260 100 141.421356237 \n"
		"*ENDFRAME\n"
			;

/*RZ free parameter of transformation
EXPECTED
	RZ = 100
	PT  0 100 0\n
	PT2  100 100 0\n

*/
char const *const BCAM_PLR3D_RZfree_setup3= 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

		"*CALA\n"
			"STN  0 0 0\n"

		"*POIN\n"
			"PT  450 15400 0\n"
			"PT2  14500 14500 540\n"


		"*FRAME BCAMFrame 100 0 0 0.0 0.0 0.0 1.0 RZ\n"
		"*CALA\n"
			"REF  0 0 0\n"
			"REF2  100 0 0\n"
			"REF3  0 50 0\n"
		"*CAM REF TS1\n"
			"*PLR3D\n"
				"STN  200  100  100\n"
				"PT 250 100 141.421356237\n"
				"PT2 -100 100 100 \n"
		"*ENDFRAME\n"
			;

/*TX free parameter of transformation
EXPECTED
	TX = 100
*/
char const *const BCAM_PLR3D_TXfree_setup3= 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

		"*CALA\n"
			"STN  0 0 0\n"

		"*POIN\n"
			"PT  0 100 0\n"
			"PT2  100 100 0\n"


		"*FRAME BCAMFrame 100 0 0 0.0 0.0 0.0 1.0 TX\n"
		"*CALA\n"
			"REF  0 0 0\n"
			"REF2  100 0 0\n"
			"REF3  0 50 0\n"
		"*CAM REF TS1\n"
			"*PLR3D\n"
				"STN  300  100  200\n"
				"PT 329.51672353009 100 223.6067977499\n"
				"PT2 350 100 141.421356237 \n"
		"*ENDFRAME\n"
			;

/*TX and RZ free parameters*/
/* EXPECTED 
		RZ = 390 gons
		TX = 200 meters

	ANOTHER SOLUTION IS FOUND, BUT ALSO 'WORKS', network is not unique
*/
char const *const BCAM_PLR3D_TXRZfree_setup3_2solutions= 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

		"*CALA\n"
			"STN  0 0 0\n"

		"*POIN\n"
			"PT  0 100 0\n"
			"PT2  100 100 0\n"


		"*FRAME BCAMFrame 100 0 0 0.0 0.0 0.0 1.0 RZ TX\n"
		"*CALA\n"
			"REF  0 0 0\n"
			"REF2  100 0 0\n"
			"REF3  0 50 0\n"
		"*CAM REF TS1\n"
			"*PLR3D\n"
				"STN  200  100  200\n"
				"PT 229.51672353009 100 223.6067977499\n"
				"PT2 250 100 141.421356237 \n"
		"*ENDFRAME\n"
		;

/*TX and RZ free parameters*/
/* EXPECTED 
		RZ = 390 gons
		TX = 200 meters
*/
char const *const BCAM_PLR3D_TXRZfree_setup3_TSTNADD = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

		"*POIN\n"
			"PT  0 0 0\n"
			"PT2  100 100 0\n"

		/*	RZ CALC IS 390 TX CALC IS 200*/
		"*FRAME BCAMFrame 200 0 0 0.0 0.0 0.0 1.0 RZ\n"
		"*CALA\n"
			"REF  0 0 0\n"
			"REF2  100 0 0\n"
			"REF3  0 50 0\n"
		"*CAM REF TS1\n"
			"*PLR3D\n"
				"PT  260  100  200\n"
				/*"BCAM2PT 289.51672353009 100 223.6067977499\n"*/
				"TSTNREF 289.51672353009 100 223.6067977499\n"
				"PT2 310 100 141.421356237 \n"
		"*ENDFRAME\n"


		"*CALA\n"
			"TSTNREF  0 100 0\n"

		"*TSTN TSTNREF TS1 IHFIX\n"
		  "*V0\n"
			"*PLR3D\n"
				"PT  200  100  100\n"
				"REF 129.51672353009 100 223.6067977499\n"
				"PT2 100 100 100 \n"
		;
/*2 SUBFRAMES, one is fixed, second is free in TX and RZ
	Correct value:
	TX = 200
	RZ = 200

*/
char const *const BCAM_PLR3D_TXRZfree_setup3_2BCAMs= 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

	
		"*POIN\n"
			"PT  0 0 0\n"
			"PT2  100 100 0\n"

		"*FRAME BCAMFrame 100 0 0 0.0 0.0 20.0 1.0  TX RZ\n"
		"*CALA\n"
			"REF  0 0 0\n"
			"REF2  100 0 0\n"
			"REF3  0 50 0\n"
			"REF7  -200 0 0\n"
		"*CAM REF TS1\n"
			"*PLR3D\n"
				"PT  100  100  200\n"
				"BCAM2PT 129.51672353009 100 223.6067977499\n"
				"PT2 150 100 141.421356237 \n"
		"*ENDFRAME\n"


		"*FRAME BCAMFrame2 0 100 0 0.0 0.0 0.0 1.0\n"
		"*CALA\n"
			"BCAM2PT  0 0 0\n"
			"REF4  100 0 0\n"
			"REF5  0 50 0\n"
		"*CAM BCAM2PT TS1\n"
			"*PLR3D\n"
				"PT  200  100  100\n"
				"REF 129.51672353009 100 223.6067977499\n"
				"PT2 100 100 100 \n"
		"*ENDFRAME\n"
		;
//////////////////////////////////////////////////////////////////////////////////////////////////
///////////// DIR3D
//////////////////////////////////////////////////////////////////////////////////////////////////
/*This so far does not work because of the failure case in DIR3D equation*/
/*
	EXPECTED
		REF = ( 100 0 0)
		PT = ( 0 100 0)

*/
char const *const BCAM_DIR3D_setup1 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*CAMD TS1 T1\n"
		"T1  5  5  0.5 0.5\n"

		"*CALA\n"
			"REF  100 0 0\n"
			"REF2  100 100 0\n"

		"*POIN\n"
			"PT  0 100 0\n"

		"*FRAME BCAMFrame 0 0 0 0.0 0.0 0.0 1.0\n"
		"*CALA\n"
			"STN  0 0 0\n"
		"*CAM STN TS1\n"
			"*DIR3D\n"
				"REF  100  100 \n"
				"PT 0 100 \n"
				"REF2  50  100 \n"
/*
			"*PLR3D\n"
				"REF  100  100 100\n"
				"PT 0 100 100\n"
				"REF2  50  100 141.4213562373095048801\n"*/
		"*ENDFRAME\n"

		
		"*CALA\n"
			"TSTNREF  0 0 0\n"

		"*TSTN TSTNREF TS1 IHFIX\n"
		  "*V0\n"
			"*DIST\n"
				"PT  100\n"
				"REF 100\n"
				"REF2  141.4213562373095048801\n"
			"*ZEND\n"
				"PT  100\n"
				"REF 100\n"
				"REF2  100\n"
			"*ANGL\n"
				"PT  0\n"
				"REF 100\n"
				"REF2  50\n"

/*	"*TSTN TSTNREF TS1 IHFIX\n"
		  "*V0\n"
			"*PLR3D\n"
				"REF  100  100 100\n"
				"PT 0 100 100\n"
				"REF2  50  100 141.4213562373095048801\n"*/

				;
#endif
}
#endif