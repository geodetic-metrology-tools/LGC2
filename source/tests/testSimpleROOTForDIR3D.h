#ifndef _LGC_TESTS_SIMPLE_H
#define _LGC_TESTS_SIMPLE_H

namespace TestDIR_ROOT {

char const *const DIR = 
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
            "PT2  100 100 0\n"

		"*POIN\n"
			"PT  0 100 0\n"
			

		"*TSTN STN TS1 IHFIX\n"
		  "*V0\n"
			"*DIR3D\n"
				"PT2  60  100\n"
				"REF 110 100\n"
				"PT 10 100\n"
			"*DIST\n"
				"PT2  141.421356237\n"
				"REF 100  \n"
				"PT 100  \n"
;
}
#endif /*_LGC_TESTS_SIMPLE_H*/