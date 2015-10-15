
#ifndef _LGC_TESTS_COMPLEXMLA_H
#define _LGC_TESTS_COMPLEXMLA_H


namespace TestROOT {

	/*1 station, PARAMETRIC, MLA*/
	char const *const Param_PLR_Rs2k_Parametric_1ST = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1  5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  2000 2097.79265 433.659210201995\n"
		"REF  2100 2097.79265 433.659862635419\n"
		"STN2  2100 2197.79265 433.660640629133\n"

	"*POIN\n"
		"PT  2000.1 2198.79265 433.659988514388\n"
		"PT2  2100.2 2198.89265 433.660640629133\n"
		//Correct expected values
	/*	"PT  2000 2197.79265 433.659988514388\n"
		"PT2  2100 2197.79265 433.660640629133\n"*/

	"*TSTN STN TS1 IHFIX\n"
		"*V0\n" /*Expected angle 390*/
		"*DIST\n"
			"PT2   141.421356237309\n"
			"REF 100  \n"
			"PT 100  \n"
		"*ZEND\n"
			"PT2 100  \n"
			"REF  100   \n"
			"PT  100   \n"
		"*ANGL\n"
			"PT2 40  \n"
			"REF 90   \n"
			"PT -10 \n"
	;

	/*1 station, PARAMETRIC, MLA*/
	/*does not have a proper precision in x,y and very high in Z coordinate, V0 of the station is not 0, but it was expected (deflection of the vertical)*/
	char const *const Param_PLR_Rs2k_Parametric_1ST_NOTATP0 = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1 5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  2000 2097.79265 433.659210201995\n"
		"STN2  2100 2197.79265 433.660640629133\n"
		"REF  2100 2097.79265 433.659862635419\n"

	"*POIN\n" 
		//Correct expected values
		"PT  2000 2197.79265 433.659988514388\n"
		//"PT2  2100 2197.79265 433.660640629133\n"

	"*TSTN STN2 TS1 IHFIX\n"
		"*V0\n" /*Expected angle 389.99858528*/
		"*DIST\n"
			"STN   141.421356237309504880168\n"
			"REF 100  \n"
			"PT 100  \n"
		"*ZEND\n"
			"STN 100  \n"
			"REF  100   \n"
			"PT  100   \n"
		"*ANGL\n"
			"STN  250  \n"
			"REF 200   \n"
			"PT 300 \n"
	;


	/*2 stations, PARAMETRIC, MLA*/
	/*If 2nd statin added, does not have a proper precision in Z coordinate and V0 of the second station, IT IS PROBABLY BECAUSE THE SECOND STATION IS FAR FROM P0 AND THE VALUES THEREFORE 'CORRECT'*/
	char const *const Param_PLR_Rs2k_Parametric_2ST = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  2000 2097.79265 433.659210201995\n"
		"REF  2100 2097.79265 433.659862635419\n"
		"STN2  2100 2197.79265 433.660640629133\n"

	"*CALA\n" /*"*POIN\n"*/
		/*"PT  2000.1 2197.79265 433.659988514388\n"
		"PT2  2100.2 2197.89265 433.660640629133\n"*/
		//Correct expected values
		"PT  2000 2197.79265 433.659988514388\n"
		"PT2  2100 2197.79265 433.660640629133\n"

	"*TSTN STN TS1 IHFIX\n"
		"*V0\n" /*Expected angle 10*/
		"*DIST\n"
			"PT2   141.421356237309504880168\n"
			"REF 100  \n"
			"PT 100  \n"
		"*ZEND\n"
			"PT2 100  \n"
			"REF  100   \n"
			"PT  100   \n"
		"*ANGL\n"
			"PT2  60  \n"
			"REF 110   \n"
			"PT 10 \n"

	"*TSTN STN2 TS1 IHFIX\n"
		"*V0\n" /*Expected angle 389.99858528*/
		"*DIST\n"
			"STN   141.421356237309504880168\n"
			"REF 100  \n"
			"PT 100  \n"
		"*ZEND\n"
			"STN 100  \n"
			"REF  100   \n"
			"PT  100   \n"
		"*ANGL\n"
			"STN  240  \n"
			"REF 190   \n"
			"PT 290 \n"
	;
















	/*1 station, PLR3D, MLA*/
	char const *const Param_PLR_Rs2k_PLR3D_1ST = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  2000 2097.79265 433.659210201995\n"
			"REF  2100 2097.79265 433.659862635419\n"

		"*POIN\n"
			"PT  2000.1 2197.79265 433.659988514388\n"
			"PT2  2100.2 2197.89265 433.660640629133\n"
			//Correct expected values
			/*"PT  2000 2197.79265 433.659988514388\n"
			"PT2  2100 2197.79265 433.660640629133\n"*/

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n" /*Expected angle 390*/
			"*PLR3D\n"
				"PT2  60  100  141.421356237309\n"
				"REF 110 100 100  \n"
				"PT 10 100 100  \n"
		;

	/*1 station, PLR3D, MLA*/
	char const *const Param_PLR_Rs2k_PLR3D_1ST_NOTATP0 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  2000 2097.79265 433.659210201995\n"
			"REF  2100 2097.79265 433.659862635419\n"
			"STN2  2100.2 2197.89265 433.660640629133\n"


		"*POIN\n"
			"PT  2000.1 2197.79265 433.659988514388\n"	
			//Correct expected values
			/*"PT  2000 2197.79265 433.659988514388\n"*/

		"*TSTN STN2 TS1 IHFIX\n"
		  "*V0\n" /*Expected angle 399....*/
			"*PLR3D\n"
				"STN  250  100  141.421356237309\n"
				"REF 200 100 100  \n"
				"PT 300 100 100  \n"
		;

	/* 
		TEST TWO TOTAL STATIONS in CCS Rs2k
		Two Total Station looking two variable points and a CALA point
		Direct3D & distance coordinates are measured by total stations
	*/
	char const *const Param_PLR_Rs2k_2ST = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  2000 2097.79265 433.659210201995\n"
			"REF  2100 2097.79265 433.659862635419\n"

		"*CALA\n"
			"PT  2000 2197.79265 433.659988514388\n"
			"PT2  2100 2197.79265 433.660640629133\n"

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
			"*PLR3D\n"
				"PT2  100  100  141.421356237309\n"
				"REF 150 100 100  \n"
				"PT 50 100 100  \n"

		/*"*TSTN PT2 TS1 IHFIX\n"
		  "*V0\n"
			"*PLR3D\n"
				"STN  240  100  141.421356237309\n"
				"REF 190 100 100  \n"
				"PT 290 100 100  \n"*/
		;














		char const *const Param_PLR_Rs2k_AVSETUP = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  2000 2097.79265 433.659210201995\n"
			"REF  2082.90299989079 2153.71306317001 433.659884522085\n"

		"*POIN\n"
			"PT  1944.07958682994 2180.69564989063 433.660049561012\n"
			"PT2  2026.98258672072 2236.61606306065 433.660736301884\n"

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
			"*PLR3D\n"
				"PT2  60  100  141.421356237309\n"
				"REF 110 100 100  \n"
				"PT 10 100 100  \n"

		"*TSTN PT2 TS1 IHFIX\n"
		  "*V0\n"
			"*PLR3D\n"
				"STN  240  100  141.421356237309\n"
				"REF 190 100 100  \n"
				"PT 290 100 100  \n"
		;

	/*1 station, PARAMETRIC, MLA, NOT AT p0*/
	char const *const Param_PLR_Rs2k_Parametric_1ST_AVcoord = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN   2000                2097.79265               433.659210201995  \n"
		"REF  2082.90299989079                2153.71306317001                 433.659884522085  \n"
		"STN2 2026.98258672072                 2236.61606306065             433.660736301884\n"

	"*POIN\n" 
	    /* Correct values*/
		"PT  1944.07958682994            2180.69564989063         433.660049561012\n"
		//"PT2 2026.98258672072                 2236.61606306065             433.660736301884\n"

	"*TSTN STN2 TS1 IHFIX\n"
		"*V0\n" /*Expected angle 389.99858528*/
		"*DIST\n"
			"STN   141.421356237309504880168\n"
			"REF 100  \n"
			"PT 100  \n"
		"*ZEND\n"
			"STN 100  \n"
			"REF  100   \n"
			"PT  100   \n"
		"*ANGL\n"
			"STN  250  \n"
			"REF 200   \n"
			"PT 300 \n"
	;
		}


	#endif
















		/*
	char const *const DIR3D_setup2 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"
		"*POLAR TS2 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN 100 100 0\n"
			"REF  0 0 0\n"

		"*POIN\n"
			"PT  0.5 100.5 0\n"
				
		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
			
		;
*/
