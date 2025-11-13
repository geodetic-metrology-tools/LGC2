/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_SIGMA_H
#define _LGC_TESTS_SIGMA_H

namespace TestROOT {

	char const *const Param_case = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  100 0 0\n"

		"*POIN\n"
			"PT  0 100 0\n"   

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
				"*ZEND\n"
				"REF 100\n"
				"PT  100\n"
			"*DIST\n"
				"REF 100\n"
				"PT  100\n"
			//This is added in the respective test file with different value in ...
			/*
			"*ANGL\n"
					"REF ...\n"
					"PT  ...\n"
			*/
			;



	char const *const Param_case_setup2 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1   5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0 \n"


		"*CALA\n"
			"STN 100 100 0\n"
			"REF  0 0 0\n"

		"*POIN\n"
			"PT  0 100 0\n"

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
			"*ZEND\n"
				"REF 100\n"
				"PT  100\n"
			"*DIST\n"
				"REF 141.421356237\n"
				"PT  100\n"
		//This is added in the respective test file with different value in ...
		/*		
			"*ANGL\n"
				"REF 240\n"
				"PT  290\n"*/
				;

	char const *const Param_case_setup3 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  100 0 0\n"

		"*POIN\n"
			"PT2  100 100 0\n"
			"PT  0 100 0\n"

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
			"*ZEND\n"
				"PT2 100\n"
				"REF 100\n"
				"PT  100\n"
			"*DIST\n"
				"PT2 141.421356237\n"
				"REF 100\n"
				"PT  100\n"
		//This is added in the respective test file with different value in ...
		/*				
			"*ANGL \n"
				"PT2 ...\n"
				"REF ...\n"
				"PT  ...\n"	*/
			;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PLR3D
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char const *const Root_PLR_setup2 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1   5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN 100 100 0\n"
			"REF  0 0 0\n"

		"*POIN\n"
			"PT  100 1000.5 50\n"  /* Correct values 0 100 0*/
				
		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
		//This is added in the respective test file with different value in ...
		/*
			"*PLR3D\n"
				"REF  240  100  141.421356237\n"
				"PT 290 100 100  \n"
		*/
			;




	char const *const Root_PLR_setup3 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.0 0.0 0\n"
		"T1   5  5  0.5  0.0  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  100 0 0\n"
			
		"*POIN\n"
			"PT2  100 100 0\n"
			"PT  0 100 0\n"

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
		//This is added in the respective test file with different value in ...
		/*
			  "*PLR3D\n"
					"REF  ...  ...  ...\n"
					"PT ... ... ...  \n"
					"PT2 ... ... ...  \n"
		*/
		;

	char const *const Root_PLR_setup3_SEP = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1   5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  0 0 0\n"
			"REF  100 0 0\n"
			
		"*POIN\n"
			"PT2  100 100 0\n"
			"PT  0 100 0\n"

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
		  	"*PLR3D\n"
			"REF  -100 100  100\n"
			"PT -200 100 100  \n"
			"PT2 -150 100 141.421356237  \n"
					;


	char const *const Root_PLR_setup2_SEP = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.5  0.005	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN 100 100 0\n"
			"REF  0 0 0\n"

		"*POIN\n"
			"PT  100 1000.5 50\n"  /* Correct values 0 100 0*/
				
		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
		  	"*PLR3D\n"
				"REF  451 100  141.421356237\n"  
				"PT 501 100 100  \n"  
					;
	char const *const ROT3D = R"(*TITR
testing Rot3D
*OLOC
*INSTR
*POLAR A C .1898 0 0 0
C 3 3 .02 6 0 0 0 0 0 0
*FRAME rotFrame 0 0 0 12 34 56 1
*CALA
zero 0 0 0 
ex 1 0 0 
ey 0 1 0
*ENDFRAME
*TSTN   zero   A ROT3D  TRGT C
*V0   
*PLR3D
ex 100 100 1
ey 0 100 1
*END
)";

	char const *const CombinedIssue = R"(*TITR
This input file was solved with the combined method until LGC 2.6 (PLR3D)
Due to an inaccuracy in the GN algorithm the estimated parameters are slightly wrong
See issue https://its.cern.ch/jira/browse/SUS-2014
*OLOC
*INSTR
*POLAR Station1 Target1 0 0 0 0
Target1 0.1 1 1 0 0 0 0 0 0 0   
*CALA
Station1Pos              0 0 0
RefPoint 1 0 0 
*POIN
FataMorgana  1 0 0
*TSTN  Station1Pos    Station1    IHFIX     TRGT Target1  
*V0 
% the same measurement in ANGL ZEND DIST is using a parametric formulation which is solved correct in LGC <2.6
% the estimated x coordinate of FataMorgana point needs to be at 1m. In LGC <2.6 it is estimated at 0.8m
%*ANGL
%RefPoint  0   % to give the station a horizontal orientation
%FataMorgana -0.005
%FataMorgana 0.005
%*ZEND 
%RefPoint  100
%FataMorgana 100
%FataMorgana 100
%*DIST   
%RefPoint  1
%FataMorgana 1
%FataMorgana 1
*PLR3D
RefPoint 0  100 1
FataMorgana -0.005 100 1
FataMorgana 0.005 100 1
*END
)";



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DHOR and ECTH measurement
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char const *const DHORTest_1 = 
		"*TITR\n"
		"Testing Input Matrices Filler.\n"
		"*OLOC\n"
		"*FAUT\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.8  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN 0 0 0\n"
			"REF  100 100 0\n"

		"*VXY\n"
			"PT  200 -500 0\n"

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
			"*ANGL\n"
				"REF 50\n"
				"PT  100\n"
			"*DHOR\n"
				"REF 141.421356237\n"
				"PT  100\n"
			;
	}





#endif