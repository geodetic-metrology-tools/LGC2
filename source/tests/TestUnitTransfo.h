#ifndef _LGC_TESTS_UNITTRANSFO_H
#define _LGC_TESTS_UNITTRANSFO_H

namespace LGCTestTransfo {
	char const *const testTransformations=
		"*TITR\n"
		"TEST TREE AND TRANSFO\n"
		"*OLOC\n"

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


		char const *const testTransformationsComplex=
		"*TITR\n"
		"TEST COMPLEX TRANSFORMATION CHAIN\n"
		"*OLOC\n"

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



		char const *const testTransformationsWithScale=
		"*TITR\n"
		"TEST TREE AND TRANSFO\n"
		"*OLOC\n"

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