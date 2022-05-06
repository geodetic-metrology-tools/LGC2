/*
© Copyright CERN 2000-2022. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_CONSI_H
#define _LGC_TESTS_CONSI_H


/*Mixing TSTN observation types*/
namespace Consi {
	/* reference measurement, with only a tstn*/
	char const *const CONSI_keyword_active =
		"*TITR\n"
		"Testing CONSI keyword\n"
		"*OLOC\n"
		"*CONSI\n"
		"*INSTR\n"
		"*POLAR TS1 T1 0.0 0.1 0.1 0\n"
		"T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0\n"
		"*CALA\n"
		"STN 100 100 0\n"
		"REF  0 0 0\n"
		"*POIN\n"
		"PT  100 1000.5 50  \n"
		"*TSTN STN TS1 IHFIX\n"
		"*V0 ACST 200.0\n"
		"*PLR3D\n"
		"REF  50 100  141.421356237 \n"
		"PT  100 100 100 \n"
		"*END\n";

	char const* const CONSI_good_config =
		"*TITR                        \n"
		"SANS_TITRE                        \n"
		"*OLOC\n"
		"*FAUT\n"
		"*CONSI                        \n"
		"*HIST\n"
		"*PUNC\n"
		"*INSTR                         \n"
		"*CALA                        \n"
		" S01  9999.90000  9999.90000   390.20500\n"
		" S02  9999.90000  9999.90000   316.94500\n"
		"*VZ                        \n"
		"LEV064     9999.9     9999.9     0.0\n"
		"LEV065     9999.9     9999.9     0.0\n"
		"LEV066     9999.9     9999.9     0.0\n"
		"LEV067     9999.9     9999.9     0.0\n"
		"*DVER   0.064\n"
		"LEV064     S02     -1.36322\n"
		"LEV065     S02     -1.10562\n"
		"LEV065     S01     -1.38529\n"
		"LEV066     S01     -1.46924\n"
		"LEV066     S02     -1.18943\n"
		"LEV067     S01     -1.32555\n"
		"*END\n"
		;


	char const* const CONSI_bad_config =
		"*TITR                        \n"
		"SANS_TITRE                        \n"
		"*OLOC\n"
		"*FAUT\n"
		"*CONSI                        \n"
		"*HIST\n"
		"*PUNC\n"
		"*INSTR                         \n"
		"*CALA                        \n"
		"S01  9999.90000  9999.90000   390.20500\n"
		"S02  9999.90000  9999.90000   316.94500\n"
		"*VXZ\n"
		"S03 1 1 1 \n"
		"*VZ                        \n"
		"LEV064     9999.9     9999.9     0.0\n"
		"LEV065     9999.9     9999.9     0.0\n"
		"LEV066     9999.9     9999.9     0.0\n"
		"LEV067     9999.9     9999.9     0.0\n"
		"LEV068     9999.9     9999.9     0.0\n"
		"*DVER   0.064\n"
		"LEV065     S01     -1.38529\n"
		"LEV066     S01     -1.46924\n"
		"LEV068     S01     -1.38529\n"
		"LEV068     S03     -1.46924\n"
		"LEV064     S02     -1.36322\n"
		"LEV065     S02     -1.10562\n"
		"LEV066     S02     -1.18943\n"
		"LEV067     S03     -1.32555\n"
		"*END\n"
		;


}
#endif

