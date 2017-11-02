
/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
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