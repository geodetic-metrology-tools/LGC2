/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_SAG_H
#define _LGC_TESTS_SAG_H


/*Sag adjustable element tests*/
namespace Sag{

    char const *const SAG_test_mixed_pairs_and_DEFORM = R"(*TITR
mixed *SAGELEMENT pair continuation block with a DEFORM-tagged CALA point
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% defining the adjustable sag element with its point pairs as continuation lines
*SAGELEMENT sag0 ROOT 0 0 0 0 ZS ZC
P_Left  P_Left_Sagged
P_Middle P_Middle_Sagged
P_Right P_Right_Sagged
*POIN
P_Left 0 -1 1 SX 1 SY 1 SZ 1
P_Middle 0 0 1 SX 1 SY 1 SZ 1
P_Right 0 1 1 SX 1 SY 1 SZ 1
P_Left_Sagged 0 -1 1 SX 1 SY 1 SZ 1
P_Middle_Sagged 0 0 0.5 SX 1 SY 1 SZ 1
P_Right_Sagged 0 1 1 1 SX 1 SY 1 SZ 1
*CALA
dummy 0 0 0
P_unobserved_deformed 0 0 0 DEFORM sag0
*END
)";

char const *const SAG_test_z = R"(*TITR
basic example: estimating z sag and curvature
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% defining the adjustable sag element with its point pairs as continuation lines
% result should be curvature=1, sag=0.5m
*SAGELEMENT sag0 ROOT 0.1 0.1 0 0 ZS ZC
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*CALA
P1_ref 0 -2 0
P2_ref 0 -1 0
P3_ref 0 0 0
P4_ref 0 1 0
P5_ref 0 2 0
*POIN
P1_sag 0 -2 4
P2_sag 0 -1 1
P3_sag 0 0 0
P4_sag 0 1 1
P5_sag 0 2 4
*OBSXYZ
P1_sag 0 -2 4.5 1 1 1
P2_sag 0 -1 1.5 1 1 1
P3_sag 0 0 0.5 1 1 1
P4_sag 0 1 1.5 1 1 1
P5_sag 0 2 4.5 1 1 1
*END)";

char const *const SAG_test_x = R"(*TITR
basic example: estimating x sag and curvature
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% defining the adjustable sag element with its point pairs as continuation lines
% result should be curvature=1, sag=0.5m
*SAGELEMENT sag0 ROOT 0 0 0.1 0.1 XS XC
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*CALA
P1_ref 0 -2 0
P2_ref 0 -1 0
P3_ref 0 0 0
P4_ref 0 1 0
P5_ref 0 2 0
*POIN
P1_sag 0 -2 0
P2_sag 0 -1 0
P3_sag 0 0 0
P4_sag 0 1 0
P5_sag 0 2 0
*OBSXYZ
P1_sag 4.5 -2 0 1 1 1
P2_sag 1.5 -1 0 1 1 1
P3_sag 0.5 0 0 1 1 1
P4_sag 1.5 1 0 1 1 1
P5_sag 4.5 2 0 1 1 1
*END)";

char const *const SAG_test_TY_and_all_sag_params = R"(*TITR
estimating all 4 sag parameters plus sagFrame TY (free translation)
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% sagFrame has RZ fixed at 19 gon, TY free and expected to converge to 17m
% (matching truthYOffsetFrame's translation)
% curvature=1, sag=0.5m both for x and z sag
*SAGELEMENT sag0 sagFrame 0.1 0.1 0.5 1 ZS ZC XS XC
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*FRAME sagFrame 0 0 0 0 0 19 1 TY
*ENDFRAME

*FRAME truthYOffsetFrame 0 17 0 0 0 19 1
*POIN
P1_sag 0 -2 0
P2_sag 0 -1 0
P3_sag 0 0 0
P4_sag 0 1 0
P5_sag 0 2 0
*CALA
P1_ref 0 -2 0
P2_ref 0 -1 0
P3_ref 0 0 0
P4_ref 0 1 0
P5_ref 0 2 0
*OBSXYZ
P1_sag 4.5 -2 4.5 1 1 1
P2_sag 1.5 -1 1.5 1 1 1
P3_sag 0.5 0 0.5 1 1 1
P4_sag 1.5 1 1.50 1 1 1
P5_sag 4.5 2 4.5 1 1 1
*ENDFRAME
*END)";


char const *const SAG_test_DEFORM_syntax = R"(*TITR
Deformation Illustration - 5 points with z sag and curvature
*OLOC
*CONSI
*PREC 6
*INSTR
% SAG ELEMENT DEFINITION
*SAGELEMENT mySag1 sagFrame1  0.5 0.6 0 0 ZS ZC
*FRAME test1 0 0 0 0 0 0 1
*FRAME sagFrame1 0 0 0 0 0 0 1
*ENDFRAME
*POIN
P1_1    0.0000 -2 0.0000 SX 1 SY 1 SZ 1 DEFORM mySag1
P2_1    0.0000 -1 0.0000 SX 1 SY 1 SZ 1 DEFORM mySag1
P3_1    0.0000  0 0.0000 SX 1 SY 1 SZ 1 DEFORM mySag1
P4_1    0.0000  1 0.0000 SX 1 SY 1 SZ 1 DEFORM mySag1
P5_1    0.0000  2 0.0000 SX 0 SY 1 SZ 0 DEFORM mySag1
*ENDFRAME
*OBSXYZ
P1_1 0.0000  -2 -4.0000 1 1 0.001
P2_1 0.0000  -1 -1 1 1 0.001
P3_1 0.0000   0 0 1 1 0.001
P4_1 0.0000   1 -1 1 1 0.001
P5_1 0.0000   2 -4 1 1 0.001
*END)";

char const *const SAG_test_DEFORM_frame_syntax = R"(*TITR
Deformation Illustration - frame-level DEFORM with point sigma transfer
*OLOC
*CONSI
*PREC 6
*INSTR
% SAG ELEMENT DEFINITION
*SAGELEMENT mySag1 sagFrame1  0.5 0.6 0 0 ZS ZC
*FRAME test1 0 0 0 0 0 0 1 DEFORM mySag1
*FRAME sagFrame1 0 0 0 0 0 0 1
*ENDFRAME
*POIN
P1_1    0.0000 -2 0.0000 SX 1 SY 1 SZ 1
P2_1    0.0000 -1 0.0000 SX 1 SY 1 SZ 1
P3_1    0.0000  0 0.0000 SX 1 SY 1 SZ 1
P4_1    0.0000  1 0.0000 SX 1 SY 1 SZ 1
P5_1    0.0000  2 0.0000 SX 0 SY 1 SZ 0
*ENDFRAME
*OBSXYZ
P1_1 0.0000  -2 -4.0000 1 1 0.001
P2_1 0.0000  -1 -1 1 1 0.001
P3_1 0.0000   0 0 1 1 0.001
P4_1 0.0000   1 -1 1 1 0.001
P5_1 0.0000   2 -4 1 1 0.001
*END)";

char const *const SAG_test_zWithRotation = R"(*TITR
estimating z sag with 2D point spread to identify sag frame rotation
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% true values: ZS=0.5, ZC=1.0, sagFrame RZ=350 gon (=-45 deg)
% inverse convention: y_sag = x*sin(RZ) + y*cos(RZ), so dy = (-x+y)/sqrt(2) for RZ=350
% offset_z = ZS + ZC*dy^2
*SAGELEMENT sag0 sagFrame 0.1 0.1 0 0 ZS ZC
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*FRAME sagFrame 0 0 0 0 0 130 1 RZ
*ENDFRAME
*CALA
P1_ref 0 0 0
P2_ref 0 2 0
P3_ref 2 0 0
P4_ref 2 2 0
P5_ref 1 0 0
*POIN
P1_sag 0 0 0.5
P2_sag 0 2 2.5
P3_sag 2 0 2.5
P4_sag 2 2 0.5
P5_sag 1 0 1
*OBSXYZ
P1_sag 0 0 0.5 1 1 1
P2_sag 0 2 2.5 1 1 1
P3_sag 2 0 2.5 1 1 1
P4_sag 2 2 0.5 1 1 1
P5_sag 1 0 1.0 1 1 1
*END)";

char const *const SAG_test_DEFORM_unobserved_point = R"(*TITR
DEFORM-expanded point with no observation converges to sag offset
*OLOC
*JSON
*CONSI
*INSTR
*SAGELEMENT sag0 ROOT 0 0 0 0 ZS ZC
*CALA
P_Left 0 -1 1 DEFORM sag0
P_Middle 0 0 1 DEFORM sag0
P_Right 0 1 1 DEFORM sag0
P_unobserved_sagged 0 0 0 DEFORM sag0
*OBSXYZ
P_Left 0 -1 1  0.01 0.01 0.01
P_Middle 0 0 0.5   0.01 0.01 0.01
P_Right 0 1 1   0.01 0.01 0.01
*END)";



}
#endif
