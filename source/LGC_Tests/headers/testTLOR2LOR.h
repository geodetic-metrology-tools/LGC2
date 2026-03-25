/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_UNITTRANSFO_H
#define _LGC_TESTS_UNITTRANSFO_H

namespace LOR2LORInputFiles
{
char const *const plateFileOrig = "*TITR\n"
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

char const *const test1 = "*TITR\n"
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
						  "*ENDFRAME\n";

char const *const test2 = "*TITR\n"
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

						  "*ENDFRAME\n";

char const *const test_with_scale = "*TITR\n"
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
									"*ENDFRAME\n";

char const *const composedHelmertParameter0 = R"(*TITR
test derivatives of chained Helmert frames with respect to Helmert parameters of the chain
*OLOC
*CONSI
*INSTR
*CALA
ux 1 0 0
uy 0 1 0
uz 0 0 1
*FRAME Frame1 1 2 3 4 5 6 1 TX TY TZ RX RY RZ SCL
*OBSXYZ
ux 1 0.01 0.023 1 1 1
uy 0.053 1 0.017 1 1 1
uz 0.091 0.0123 1 1 1 1
*ENDFRAME
*END)";

char const *const composedHelmertParameter1 = R"(*TITR
test derivatives of chained Helmert frames with respect to Helmert parameters of the chain The setup is done such that the chain Frame3-Frame1 is per Construction identical to the single frame "singleFrame" DANS CE FICHIER, LES OBSERVATIONS SONT SIMULEES !
*OLOC
*CONSI
*INSTR
*CALA
ux   1.00000   0.00000   0.00000
uy   0.00000   1.00000   0.00000
uz   0.00000   0.00000   1.00000
*FRAME   Frame1   3.00000   4.00000   5.00000   6.00000   7.00000   8.00000   1.00000   TX   TY   TZ   RX   RY   RZ   SCL
*CALA
ux1   1.00000   0.00000   0.00000
uy1   0.00000   1.00000   0.00000
uz1   0.00000   0.00000   1.00000
*OBSXYZ
ux   -2.11501   -3.80681   -5.10244   0.01000   0.01000   0.01000
uy   -3.21568   -2.94240   -4.89916   0.01000   0.01000   0.01000
uz   -2.98097   -4.01106   -4.00315   0.01000   0.01000   0.01000
*FRAME   Frame2   9.00000   10.00000   11.00000   12.00000   13.00000   14.00000   1.00000   TX   TY   TZ   RX   RY   RZ   SCL
*OBSXYZ
ux1   -8.46118   -9.84453   -10.79324   0.01000   0.01000   0.01000
uy1   -9.59401   -9.09121   -10.40695   0.01000   0.01000   0.01000
uz1   -9.18152   -10.19753   -9.62856   0.01000   0.01000   0.01000
*FRAME   Frame3   0.00000   0.00000   0.00000   0.00000   0.00000   0.00000   1.00000   TX   TY   TZ   RX   RY   RZ   SCL
*OBSXYZ
ux   -0.07356   -1.82269   -3.11004   0.00100   0.00100   0.00100
uy   -1.15507   -0.92245   -2.96899   0.00100   0.00100   0.00100
uz   -0.98219   -1.97166   -2.03664   0.00100   0.00100   0.00100
*ENDFRAME
*ENDFRAME
*ENDFRAME
*FRAME   singleFrame   1.00000   2.00000   3.00000   4.00000   5.00000   6.00000   1.00000   TX   TY   TZ   RX   RY   RZ   SCL
*OBSXYZ
ux   -0.07356   -1.82269   -3.11004   0.00100   0.00100   0.00100
uy   -1.15507   -0.92245   -2.96899   0.00100   0.00100   0.00100
uz   -0.98219   -1.97166   -2.03664   0.00100   0.00100   0.00100
*ENDFRAME
*END)";

char const *const composedHelmertParameter2 = R"(*TITR
test derivatives of chained Helmert frames with respect to Helmert parameters of the chain
The setup is done such that the chain Frame3-Frame1 is per Construction identical to the single frame "singleFrame"
Freedoms of involved frames is more irregular then in composedHelmertParameter1 case
*OLOC
*CONSI
*INSTR
*CALA
ux 1 0 0
uy 0 1 0
uz 0 0 1
*FRAME Frame1 0 0 0 0 0 0 1 TX RY TZ
*CALA
ux1 1 0 0
uy1 0 1 0
uz1 0 0 1
*OBSXYZ
ux 1 0.01 0.023 1 1 1
uy 0.053 1 0.017 1 1 1
uz 0.091 0.0123 1 1 1 1
*FRAME Frame2 0 0 0 0 0 0 1 RX TY RZ
*OBSXYZ
ux1 1 0.00123 0.054 1 1 1
uy1 0.067 1 0.012 1 1 1
uz1 0.00345 0.00786 1 1 1 1
*FRAME fixFrame 1 2 3 4 5 6 1.6
*FRAME Frame3 0 0 0 0 0 0 1 TX TY TZ RX RY RZ SCL
*OBSXYZ
ux 1 0.12 -0.23 1 1 1
uy 0.027 1 -0.013 1 1 1
uz 0.117 -0.034 0.956 1 1 1
*ENDFRAME
*ENDFRAME
*ENDFRAME
*ENDFRAME
% the single frame is determined  by obsxyz observations identical to the observations in Frame3
% The Chain of Frame1-Frame3 then should have the same combined Helmert parameter as the single frame
*FRAME singleFrame 0 0 0 0 0 0 1 TX TY TZ RX RY RZ SCL
*OBSXYZ
ux 1 0.12 -0.23 1 1 1
uy 0.027 1 -0.013 1 1 1
uz 0.117 -0.034 0.956 1 1 1
*ENDFRAME
*END)";

char const *const composedGimbalLock = R"(*TITR
two frames that combined give a gimbal lock ry=100gon
*OLOC
*INSTR
*CALA
ux 1 0 0
uy 0 1 0
uz 0 0 1
*FRAME Frame1 0 0 0 0 0 0 1 TX TY TZ RX RY RZ SCL
*OBSXYZ
% random values
ux 1.1 0.05 -0.02 1 1 1
uy 0.01753 1.088 -0.099 1 1 1
uz 0.03 -0.02 1.1 1 1 1
*FRAME Frame2 0 0 0 0 0 0 1 TX TY TZ RX RY RZ SCL
% going to root should correspond to ry=100 gon gimbal lock
*OBSXYZ
ux 0 0 -1 1 1 1
uy 0 1 0 1 1 1
uz 1 0 0 1 1 1
*ENDFRAME
*ENDFRAME
*END)";

} // namespace LOR2LORInputFiles
#endif
