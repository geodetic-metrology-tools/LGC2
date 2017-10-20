/*
© Copyright CERN 2000-2017. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_UNITTRANSFO_H
#define _LGC_TESTS_UNITTRANSFO_H

namespace LOR2LORInputFiles {

	char const *const plateFileOrig = 
		"*TITR\n"
		"HIMAS TEST FILE.\n"
		"*OLOC\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0 0 0 0\n"
		"T1  5.0  5.0  0.5  0.5  1  2.0  0.2  0.05  0.0  0.05 \n"

		"*TSTN P3C TS1\n"
			"*V0\n"
			"*PLR3D\n"
				"P8C  1 2 3\n"
		"*TSTN P4C TS1\n"
			"*V0\n"
			"*PLR3D\n"
				"P9C  1 2 3\n"
		"*TSTN P5C TS1\n"
			"*V0\n"
			"*PLR3D\n"
				"P10C 1 2 3\n"
		"*TSTN P6C TS1\n"
			"*V0\n"
			"*PLR3D\n"
				"P11C 1 2 3\n"

		"*FRAME P0 80 90 100  38.19718634205 31.83098861838 25.4647908947  1.0 TY TZ RX RY RZ\n"
			"*CALA\n"
			"P3C 10 10 10\n"
			"P4C 10 30 10\n"
			"P5C 10 60 10\n"
			"P6C 10 90 10\n"

			 "*FRAME P3 10 10 40 38.19718634205 31.83098861838 25.4647908947 1.0 TY TZ RX RY RZ\n"
				 "*CALA\n"
			   	 "P12C 10 10 10\n"

				"*FRAME P6 80 90 100  38.19718634205 31.83098861838 25.4647908947 2.0 ty tz rx ry rz\n"
					 "*CALA\n"
			   		 "P16C 10 10 10\n"
				"*ENDFRAME\n"
		     "*ENDFRAME\n"

			 "*FRAME P4 10 10 40  38.19718634205 31.83098861838 25.4647908947 1.0 ty tz rx ry rz\n"
				 "*CALA\n"
			   	 "P14C 10 10 10\n"
		     "*ENDFRAME\n"
		"*ENDFRAME\n"

		"*TSTN P8C TS1\n"
			"*V0\n"
			"*PLR3D\n"
				"P3C  1 2 3\n"
		"*TSTN P9C TS1\n"
			"*V0\n"
			"*PLR3D\n"
				"P4C  1 2 3\n"
		"*TSTN P10C TS1\n"
			"*V0\n"
			"*PLR3D\n"
				"P5C 1 2 3\n"
		"*TSTN P11C TS1\n"
			"*V0\n"
			"*PLR3D\n"
					"P6C 1 2 3\n"

		"*FRAME P1 80 90 100  38.19718634205 31.83098861838 25.4647908947 1.0 ty ty rx ry rz\n"
		"*CALA\n"
		"P8C  100 10 10\n"
		"P9C  100 30 10\n"
		"P10C 100 60 10\n"
		"P11C 100 90 10\n"

			"*FRAME P5 20 30 40  38.19718634205 31.83098861838 25.4647908947 1.0 ty tz rx ry rz\n"
				 "*CALA\n"
			   	 "P15C 10 10 10\n"
		    "*ENDFRAME\n"
		"*ENDFRAME\n"
		
		
		"*FRAME P2 20 30 40  38.19718634205 31.83098861838 25.4647908947 1.0 ty tz rx ry rz\n"
				 "*CALA\n"
			   	 "P13C 10 10 10\n"
		"*ENDFRAME\n";


	char const *const test1 =
		"*TITR\n"
		"TEST TREE AND TRANSFO\n"
		"*OLOC\n"
		"*PREC 7\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"
		"*POLAR TS2 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  0 0 0\n"
			"STN2 100 100 0\n"
			"STN3 -100 100 0\n"

		"*POIN\n"
			"PT  0 100 0\n"

		"*FRAME FREF2 -100.0 0.0 0.0 0.0 0.0 300.0 1\n"
		"*CALA\n"
			"REF2  0 0 0\n"
			"P3  -100.0 0.0 0.0\n"
			"P4  -100.0 -200.0 0.0\n"
			"*FRAME FPT2 -100.0 -100.0 0.0 0.0 0.0 0.0 1\n"
				"*POIN\n"
				"PT2  0.0 0.0 0.0 SX 10.0 SY 10.0 SZ 10.0\n"
			"*ENDFRAME\n"
		"*ENDFRAME\n"

		"*FRAME FREF 100.0 0.0 0.0 0.0 0.0 0.0 1\n"
		"*CALA\n"
			"REF  0.0 0.0 0.0\n"
		"*ENDFRAME\n"
		;


		char const *const test2=
		"*TITR\n"
		"TEST COMPLEX TRANSFORMATION CHAIN\n"
		"*OLOC\n"
		"*PREC 7\n"

		"*CALA\n"
			"STN  0 0 0\n"
			"STN2 100 100 0\n"
			"STN3 -100 100 0\n"

		"*POIN\n"
			"PT  0 100 0\n"

		"*FRAME FREF2 150.0 9758.0 -160.0 32.0 5.0 -148.0 2.0\n"
			"*CALA\n"
			"REF2  0 0 0\n"
			"*FRAME FPT2 -606.0 -1291.0 -1.0 77.0 -56.0 195.0 1.0\n"
				"*POIN\n"
				"PT2  -10.597132 0.005198 4.55658 SX 10.0 SY 10.0 SZ 10.0\n"
			"*ENDFRAME\n"
		"*ENDFRAME\n"

		"*FRAME FREF 6.0 -191.0 1561.0 3.0 -201 19 1.005\n"
		"*CALA\n"
			"REF  0.0 0.0 0.0\n"

		
		"*ENDFRAME\n"
		;



		char const *const test_with_scale=
		"*TITR\n"
		"TEST TREE AND TRANSFO\n"
		"*OLOC\n"
		"*PREC 7\n"

		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"
		"*POLAR TS2 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.1  0.0	  0  0.0  0.0  0.0  0.0  0.0 \n"

		"*CALA\n"
			"STN  0 0 0\n"
			"STN2 100 100 0\n"
			"STN3 -100 100 0\n"

		"*POIN\n"
			"PT  0 100 0\n"

		"*FRAME FREF2 -100.0 0.0 0.0 0.0 0.0 300.0 2\n"
		"*CALA\n"
			"REF2  0 0 0\n"
			"P3  -100.0 0.0 0.0\n"
			"P4  -100.0 -200.0 0.0\n"
			"*FRAME FPT2 -100.0 -100.0 0.0 0.0 0.0 0.0 4\n"
				"*POIN\n"
				"PT2  0.0 0.0 0.0 SX 10.0 SY 10.0 SZ 10.0\n"
			"*ENDFRAME\n"
		"*ENDFRAME\n"

		"*FRAME FREF 100.0 0.0 0.0 0.0 0.0 0.0 16\n"
		"*CALA\n"
			"REF  0.0 0.0 0.0\n"
		"*ENDFRAME\n"
		;
	}
#endif