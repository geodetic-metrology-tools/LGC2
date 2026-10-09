/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_CONSI_H
#define _LGC_TESTS_CONSI_H

/*Mixing TSTN observation types*/
namespace Consi
{
/* reference measurement, with only a tstn*/
char const *const CONSI_keyword_active = R"(*TITR
Testing CONSI keyword
*OLOC
*CONSI
*INSTR
*POLAR TS1 T1 0.0 0.1 0.1 0
T1  5  5  0.5  0.005  0  0.0  0.0  0.0  0.0  0.0
*CALA
STN 100 100 0
REF  0 0 0
*POIN
PT  100 1000.5 50
*TSTN STN TS1 IHFIX
*V0 ACST 200.0
*PLR3D
REF  50 100  141.421356237
PT  100 100 100
*END
)";

char const *const CONSI_good_config = R"(*TITR
SANS_TITRE
*OLOC
*FAUT
*CONSI
*HIST
*PUNC
*INSTR
*CALA
 S01  9999.90000  9999.90000   390.20500
 S02  9999.90000  9999.90000   316.94500
*VZ
LEV064     9999.9     9999.9     0.0
LEV065     9999.9     9999.9     0.0
LEV066     9999.9     9999.9     0.0
LEV067     9999.9     9999.9     0.0
*DVER   0.064
LEV064     S02     -1.36322
LEV065     S02     -1.10562
LEV065     S01     -1.38529
LEV066     S01     -1.46924
LEV066     S02     -1.18943
LEV067     S01     -1.32555
*END)";

char const *const CONSI_bad_config = R"(*TITR
SANS_TITRE
*OLOC
*FAUT
*CONSI
*HIST
*PUNC
*INSTR
*CALA
S01  9999.90000  9999.90000   390.20500
S02  9999.90000  9999.90000   316.94500
*VXZ
S03 1 1 1
*VZ
LEV064     9999.9     9999.9     0.0
LEV065     9999.9     9999.9     0.0
LEV066     9999.9     9999.9     0.0
LEV067     9999.9     9999.9     0.0
LEV068     9999.9     9999.9     0.0
*DVER   0.064
LEV065     S01     -1.38529
LEV066     S01     -1.46924
LEV068     S01     -1.38529
LEV068     S03     -1.46924
LEV064     S02     -1.36322
LEV065     S02     -1.10562
LEV066     S02     -1.18943
LEV067     S03     -1.32555
*END)";

char const *const CONSI_ConnectedComponentsTest = R"(*TITR
Goal: Test neighbor determination using sparsity pattern of A matrix
*OLOC
*CONSI
*INSTR
*EDM EDM1 ET1 0.0 0.0 0.0
ET1  0.1  0.0 0 0.0 0.0 0.0 0 0.0
% three points with distance 1 to each other -> they will form a uniform triangle but this triangle can be moved around
*VXY
P0 0 0 0
P1 1 0 0
*VZ
P2 0 1 0
*DSPT P0 EDM1
P1 1
P2 1
P1 1
P2 1
*DSPT P1 EDM1
P0 1
P2 1
P0 1
P2 1
*DSPT P2 EDM1
P0 1
P1 1
P0 1
P1 1
*END)";

} // namespace Consi
#endif
