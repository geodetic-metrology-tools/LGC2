/*
 * SPDX-FileCopyrightText: 2025 CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_COMPLEXMLA_H
#define _LGC_TESTS_COMPLEXMLA_H


/*Test input files which are in global RF (RS2K, LEP, SPHE)*/
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
		//Expected calculated values
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

	/*1 station, PARAMETRIC, MLA, rewritten with use of sub-frames, station is free in TX and this parameter is determined, target is in a fixed sub frame*/
	char const *const Param_PLR_Rs2k_Parametric_1STSUBFRAME = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1  5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		//"STN  2000 2097.79265 433.659210201995\n"
		//"REF  2100 2097.79265 433.659862635419\n"
		"STN2  2100 2197.79265 433.660640629133\n"

	"*FRAME stFr -54 0 0 0.0 0.0 0.0 1.0 TX\n"
		"*CALA\n"
			"STN  2000 2097.79265 2433.6600000001868\n"
	"*ENDFRAME\n"

	"*FRAME tgFr -68 0 0 0.0 0.0 0.0 1.0\n"
		"*CALA\n"
			"REF  2168 2097.79265 2433.6600000001872\n"
	"*ENDFRAME\n"	

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
	
	//In P5
	char const *const Param_PLR_Rs2k_Parametric_1ST_1R5E = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1  5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  -1026.53292000015 10475.4085800003 419.833838109769\n"
		"REF  -987.877035593263 10383.1822096434 419.83461491806\n"
		"STN2  -895.650572445877 10421.8381202376 419.835388831601\n"
	"*POIN\n"
		"PT  -934.306456853123 10514.064490595 419.834627918856\n"
		"PT2  -896.650572445877 10420.8381202376 419.835388831601\n"


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

	char const *const Param_PLR_Rs2k_Parametric_1ST_NOTATP0_1R5E = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1 5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  -1026.53292000015 10475.4085800003 419.833838109769\n"
		"REF  -987.877035593263 10383.1822096434 419.83461491806\n"
		"STN2  -895.650572445877 10421.8381202376 419.835388831601\n"
	"*POIN\n"
		"PT  -934.306456853123 10514.064490595 419.834627918856\n"
		"%PT2  -896.650572445877 10420.8381202376 419.835388831601\n"

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

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.0 0.0 0\n"
	"T1  5  5  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  2000 2097.79265 433.659210201995\n"
		"REF  2100 2097.79265 433.659862635419\n"
		"STN2  2100 2197.79265 433.660640629133\n"

	"*POIN\n"
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


	char const *const Param_PLR_Rs2k_Parametric_2ST_1R5E = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*RS2K\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.0 0.0 0\n"
	"T1  5  5  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  -1026.53292000015 10475.4085800003 419.833838109769\n"
		"REF  -987.877035593263 10383.1822096434 419.83461491806\n"
		"STN2  -895.650572445877 10421.8381202376 419.835388831601\n"
	"*POIN\n"
		"PT  -934.306456853123 10514.064490595 419.834627918856\n"
		"PT2  -896.650572445877 10420.8381202376 419.835388831601\n"


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
	char const *const Param_PLR_Rs2k_PLR3D_1STSUBFRAME = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*RS2K\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		//"*CALA\n"
			//"STN  2000 2097.79265 433.659210201995\n"
			//"REF  2100 2097.79265 433.659862635419\n"

		"*POIN\n"
			"PT  2000.1 2197.79265 433.659988514388\n"
			"PT2  2100.2 2197.89265 433.660640629133\n"
			//Correct expected values
			/*"PT  2000 2197.79265 433.659988514388\n"
			"PT2  2100 2197.79265 433.660640629133\n"*/

		"*FRAME stFr -54 0 0 0.0 0.0 0.0 1.0 TX\n"
			"*CALA\n"
				"STN  2000 2097.79265 2433.6600000001868\n"
		"*ENDFRAME\n"

		"*FRAME tgFr -68 0 0 0.0 0.0 0.0 1.0\n"
			"*CALA\n"
				"REF  2168 2097.79265 2433.6600000001872\n"
		"*ENDFRAME\n"	

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

	//--------------------------------------- SPHE ----------------------------------------------//
	char const *const SPHE_Parametric_1ST = 
	"*TITR\n"
	" Testing Input Matrices Filler. \n" 
	"*SPHE \n" 
	"*PREC 7 \n" 
	"*INSTR \n" 
	"*POLAR TS1 T1 0.0 0.0 0.0 0\n" 
	"T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0\n" 
	"*CALA\n" 
	"STN  2260.77736999979 4493.94616999989 402.197788807562\n" 
	"REF  2203.04826079728 4412.29233446857 402.198573563337\n" 
	"STN2  2284.70210292283 4354.56322804373 402.199358321128\n" 
	"*POIN\n" 
	"PT  2342.43121212581 4436.21706357459 402.19857356349\n" 
	
	"*TSTN STN TS1 IHFIX \n" 
	"*V0 \n" 
	"*ANGL\n" 
		"STN2  60.00001\n" 
		"REF 110.000000\n" 
		"PT 9.99999\n" 
	"*ZEND \n" 
		"STN2 100.0001\n" 
		"REF 100.0001\n" 
		"PT 99.9998\n" 
	"*DIST\n" 
		"STN2 141.4213562373095048801\n" 
		"REF 100\n" 
		"PT 100	\n" 

	;

		char const *const SPHE_Parametric_1STNOTATP0= 
	"*TITR\n"
	"Testing Input Matrices Filler.\n" 
	"*SPHE\n" 
	"*PREC 7\n" 
	"*INSTR\n" 
	"*POLAR TS1 T1 0.0 0.0 0.0 0\n" 
	"T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0\n" 
	"*CALA\n" 
	"STN  2260.77736999979 4493.94616999989 402.197788807562\n" 
	"REF  2203.04826079728 4412.29233446857 402.198573563337\n" 
	"STN2  2284.70210292283 4354.56322804373 402.199358321128\n" 
	"*POIN\n" 
	"PT  2342.43121212581 4436.21706357459 402.19857356349\n" 
	
	"*TSTN STN2 TS1 IHFIX\n" 
	"*V0\n" 
	"*ANGL\n" 
		"STN  240.0\n" 
		"REF 190.0\n" 
		"PT 290.0\n" 
	"*ZEND\n" 
		"STN 100.000\n" 
		"REF 100.000\n" 
		"PT 100.000\n" 
	"*DIST\n" 
		"STN 141.421356237309504880\n" 
		"REF 100\n" 
		"PT 100\n" 
	;

				char const *const SPHE_Parametric_2ST = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*SPHE\n"
	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.0 0.0 0\n"
	"T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0\n"
	"*CALA\n"
		"STN  2260.77736999979 4493.94616999989 402.197788807562\n"
		"REF  2203.04826079728 4412.29233446857 402.198573563337\n"
		"STN2  2284.70210292283 4354.56322804373 402.199358321128\n"
	"*POIN\n"
		"PT  2342.43121212581 4436.21706357459 402.19857356349\n"
		"PT2  2283.70210292195 4353.56322804373 402.199358321128\n"


	"*TSTN STN TS1 IHFIX  \n" 
	"*V0\n" 
		"*ANGL\n" 
			"STN2     189.1779908 \n" 
			"REF      239.1778383 \n" 
			"PT       139.1780079 \n" 
		"*ZEND\n" 
			"STN2       99.9999882\n" 
			"REF       99.9999354 \n" 
			"PT       99.9999915  \n" 
		"*DIST\n" 
			"STN2      141.4219572\n" 
			"REF      100.0003740 \n" 
			"PT      100.0001688  \n"

	"*TSTN STN2 TS1 IHFIX \n" 
	"*V0\n" 
		"*ANGL\n" 
			"STN      389.1781395 \n" 
			"REF      339.1780103 \n" 
			"PT2      250.0001218 \n" 
		"*ZEND\n" 
			"STN      100.0013861 \n" 
			"REF      100.0009627 \n" 
			"PT2      100.0000393 \n" 
		"*DIST\n" 
			"STN      141.4213366 \n" 
			"REF       99.9993092 \n" 
			"PT2        1.4137913 \n" 
	;

	//--------------------------------------- LEP ----------------------------------------------//
		char const *const LEP_Parametric_1ST = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*LEP\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
	"T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  -1026.53292000015 10475.4085800003 419.842897808546\n"
		"REF  -987.877030951952 10383.1822122285 419.843678479201\n"
		"STN2  -895.650569329243 10421.8381262027 419.84440883692\n"
	"*POIN\n"
		"PT  -934.306458377388 10514.0644939745 419.843659056997\n"
		"%PT2  -894.650569329243 10420.8381262027 419.84440883692\n"


	"*TSTN STN TS1 IHFIX\n"
		"*V0\n" 
		"*DIST\n"
			"STN2   141.421356237309504880168\n"
			"REF 100  \n"
			"PT 100  \n"
		"*ZEND\n"
			"STN2 100  \n"
			"REF  100   \n"
			"PT  100   \n"
		"*ANGL\n"
			"STN2  60  \n"
			"REF 110   \n"
			"PT 10 \n"

	;

		char const *const LEP_Parametric_1STNOTATP0= 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*LEP\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.0 0.0 0\n"
	"T1  5  5  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  -1026.53292000015 10475.4085800003 419.842897808546\n"
		"REF  -987.877030951952 10383.1822122285 419.843678479201\n"
		"STN2  -895.650569329243 10421.8381262027 419.84440883692\n"
	"*POIN\n"
		"PT  -934.306458377388 10514.0644939745 419.843659056997\n"
		"%PT2  -894.650569329243 10420.8381262027 419.84440883692\n"

	"*TSTN STN2 TS1 IHFIX\n"
		"*V0\n" 
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

	char const *const LEP_Parametric_2ST = 
	"*TITR\n"
	"Testing Input Matrices Filler.\n"
	"*LEP\n"
	"*FAUT\n"

	"*INSTR\n"
	"*POLAR TS1 T1 0.0 0.0 0.0 0\n"
	"T1  5  5  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0 \n"

	"*CALA\n"
		"STN  -1026.53292000015 10475.4085800003 419.842897808546\n"
		"REF  -987.877030951952 10383.1822122285 419.843678479201\n"
		"STN2  -895.650569329243 10421.8381262027 419.84440883692\n"
	"*POIN\n"
		"PT  -934.306458377388 10514.0644939745 419.843659056997\n"
		"PT2  -894.650569329243 10420.8381262027 419.84440883692\n"


	"*TSTN STN TS1 IHFIX\n"
		"*V0\n" 
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
		"*V0\n" 
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

	char const *const OBSXYZ_coords_heights_transformation_in_ROOT = R"(*TITR
Test
*RS2K
*PREC 6
*PUNC  OUT1
*FAUT     .01     .10
*INSTR
*CALA
PT1	1600     2001    434  
PT2	1601     2001    435
PT3 1601     2000    436
*FRAME  passif  0  0  0  0  0  0  1 TX TY TZ RX RY RZ
*CALA
PT4 -1 -1 -1
*POIN
PT5 -2 -2 -2
*OBSXYZ
PT1	0     1    0	0.1	0.1	0.1
PT2	1     1    1	0.1	0.1	0.1
PT3 1     0    2	0.1	0.1	0.1
PT5 -2 	-2 	-2	0.1	0.1	0.1
*ENDFRAME
*END)";

}


	#endif



