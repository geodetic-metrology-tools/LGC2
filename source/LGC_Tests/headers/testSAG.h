/*
� Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
Any permission to use it shall be granted in writing. Request shall be adressed to CERN through mail-KT@cern.ch
*/

#ifndef _LGC_TESTS_SAG_H
#define _LGC_TESTS_SAG_H


/*Sag adjustable element tests*/
namespace Sag{

    char const *const SAG_pairs = R"(*TITR
testing sag connection with provisional value
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% defining the adjustable sag element
*SAGELEMENT sag0 ROOT 0 0 0 0 VS VC
*POIN
P_Left 0 -1 1 SX 1 SY 1 SZ 1
P_Middle 0 0 1 SX 1 SY 1 SZ 1
P_Right 0 1 1 SX 1 SY 1 SZ 1
P_Left_Sagged 0 -1 1 SX 1 SY 1 SZ 1
P_Middle_Sagged 0 0 0.5 SX 1 SY 1 SZ 1
P_Right_Sagged 0 1 1 1 SX 1 SY 1 SZ 1
*CALA
dummy 0 0 0
P_provisional_deformed 0 0 0 DEFORM sag0
*SAGCONNECT sag0
P_Left  P_Left_Sagged
P_Middle P_Middle_Sagged
P_Right P_Right_Sagged
*END
)";

char const *const SAG_test_vertical = R"(*TITR
basic example: estimating vertical sag and curvature
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% defining the adjustable sag element
% result should be curvature=1, sag=0.5m
*SAGELEMENT sag0 ROOT 0.1 0.1 0 0 VS VC
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
*SAGCONNECT sag0
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*END)";

char const *const SAG_test_radial = R"(*TITR
basic example: estimating radial sag and curvature
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% defining the adjustable sag element
% result should be curvature=1, sag=0.5m
*SAGELEMENT sag0 ROOT 0 0 0.1 0.1 RS RC
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
*SAGCONNECT sag0
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*END)";

char const *const SAG_test_bearingAndYOffset = R"(*TITR
basic example: estimating radial and vertical sag and curvature and the position of the sag element along y axis
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% defining the adjustable sag element
% result should be bearing = rz angle of giveBearingFrame
% curvature=1, sag=0.5m both for radial and vertical sag
*SAGELEMENT sag0 sagFrame 0.1 0.1 0.5 1 VS VC RS RC
*FRAME sagFrame 0 0 0 0 0 19 1 TY
*ENDFRAME

*FRAME giveBearingAndTranslationFrame 0 17 0 0 0 19 1
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
*SAGCONNECT sag0
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*END)";


char const *const SAG_test_DEFORM_syntax = R"(*TITR
Deformation Illustration - 5 points with vertical sag and curvature
*OLOC
*CONSI
*PREC 6
*INSTR
% SAG ELEMENT DEFINITION
*SAGELEMENT mySag1 sagFrame1  0.5 0.6 0 0 VS VC
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
*SAGELEMENT mySag1 sagFrame1  0.5 0.6 0 0 VS VC
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

char const *const SAG_test_verticalWithRotation = R"(*TITR
estimating vertical sag with 2D point spread to identify sag frame rotation
*OLOC
*APRI
*JSON
*CONSI
*INSTR
% true values: VS=0.5, VC=1.0, sagFrame RZ=350 gon (=-45 deg)
% inverse convention: y_sag = x*sin(RZ) + y*cos(RZ), so dy = (-x+y)/sqrt(2) for RZ=350
% offset_z = VS + VC*dy^2
*SAGELEMENT sag0 sagFrame 0.1 0.1 0 0 VS VC
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
*SAGCONNECT sag0
P1_ref P1_sag
P2_ref P2_sag
P3_ref P3_sag
P4_ref P4_sag
P5_ref P5_sag
*END)";

char const *const SAG_test_Provisional= R"(*TITR
testing sag connection with provisional value
*OLOC
*JSON
*CONSI
*INSTR
*SAGELEMENT sag0 ROOT 0 0 0 0 VS VC
*CALA
P_Left 0 -1 1 DEFORM sag0
P_Middle 0 0 1 DEFORM sag0
P_Right 0 1 1 DEFORM sag0
P_will_be_fixed_to_sagged_provisional 0 0 0 DEFORM sag0
*OBSXYZ
P_Left 0 -1 1  0.01 0.01 0.01
P_Middle 0 0 0.5   0.01 0.01 0.01
P_Right 0 1 1   0.01 0.01 0.01
*END)";



}
#endif

