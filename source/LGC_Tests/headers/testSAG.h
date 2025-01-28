/*
© Copyright CERN 2000-2023. All rigths reserved. This software is released under a CERN proprietary software licence.
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
P_provisional_deformed 0 0 0
*CALA
dummy 0 0 0
*SAGCONNECT sag0
P_Left  P_Left_Sagged
P_Middle P_Middle_Sagged
P_Right P_Right_Sagged
P_provisional_deformed
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
*FRAME sagFrame 0 0 0 0 0 0 1 TY RZ
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


char const *const SAG_test_Provisional= R"(*TITR
testing sag connection with provisional value
*OLOC
*JSON
*CONSI
*INSTR
*POIN
P_Left 0 -1 1 
P_Middle 0 0 1 
P_Right 0 1 1 

P_will_be_fixed_to_sagged_provisional 0 0 0
*OBSXYZ
P_Left 0 -1 1  0.01 0.01 0.01 
P_Middle 0 0 0.5   0.01 0.01 0.01 
P_Right 0 1 1   0.01 0.01 0.01 
*SAGCONNECT sag0 ROOT 0 0 0 0 0 VS VC
P_Left  
P_Middle 
P_Right 
% only one point in a sagconnect line will have the following effect:
% a constraint is added that forces this point to the coordinates that correspond to the sagged version of the provisional coordinates of this point.
% the point will need 3 degrees of freedom but is constrained
% If this point is defined before, lets say as CALA, there will be a logical conflict: CALA is already compeletely fixed at provisional coordinates 
% and sagconnect will try to constrain it also to the sagged version of the provisional coordinates. This most likely is only possible if the sag parameters are all zero. => we raise an error in this case
P_will_be_fixed_to_sagged_provisional
% demonstrating error message:
% fixedPoint
*END)";



}
#endif

