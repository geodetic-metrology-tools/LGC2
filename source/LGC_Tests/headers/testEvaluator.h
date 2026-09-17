/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_EVALUATOR_H
#define _LGC_TESTS_EVALUATOR_H

/* Input files used by the evaluator tests. */
namespace Evaluator
{
/* Minimal network holding free unknowns of every adjustable object type, so that the
   evaluator parameter round trip and the finite difference check cover them all. */
char const *const allAdjustableTypes = R"(*TITR
Minimal network holding a free unknown of every adjustable object type
*OLOC
*PREC 7
*CONSI
*INSTR
% POLAR instrument height 1.0 m is the provisional of the free TAdjustableLength
*POLAR TS1 T1 1.0 0 0 0
T1 10 10 0.1 0 0 0 0 0 0 0
*SCALE SCL1 0.8 0.1 0.0 0.0 0.0
% LGCAdjustableSag: 4 free parameters, true ZS=XS=0.5 m, ZC=XC=1 1/m, base frame ROOT
*SAGELEMENT sag0 ROOT 0.1 0.1 0.2 0.3 ZS ZC XS XC
R1 S1
R2 S2
R3 S3
*CALA
R1 0 1 0
R2 0 2 0
R3 0 3 0
PT 40 2 31.5
% LGCAdjustablePoint: 3 free points, true (1.5,1,1.5) (4.5,2,4.5) (9.5,3,9.5)
*POIN
S1 0 1 0
S2 0 2 0
S3 0 3 0
% TAdjustableHelmertTransformation: all 7 parameters free,
% true 10 20 30 0 0 100 2, determined by the ANGL/ZEND/DIST observations of F1..F3
*FRAME defFrame 9 19 29 3 -2 95 1.9 TX TY TZ RX RY RZ SCL
*CALA
F1 0 0 0
F2 5 0 0
F3 0 5 2
*ENDFRAME
% TSTN in ROOT without IHFIX: free V0 (TAdjustableAngle, true 20 gon) and free
% instrument height (TAdjustableLength, true 1.5 m). ANGL/ZEND on the ROOT point PT
% decorrelate V0 from defFrame RZ and the instrument height from defFrame TZ.
*TSTN R2 TS1 TRGT T1
*V0
*ANGL
PT 80
F1 12.282893443419056
F2 37.04465749545545
F3 33.34754167131482
*ZEND
PT 59.03344706017331
F1 39.83124981360049
F2 26.884875774182074
F3 44.02430919972655
*DIST
F1 35.16034698349833
F2 31.244999599935987
F3 42.19300889957956
% LGCAdjustablePlane: free orientation (true 20 gon) and free normal distance (true 0)
*ECHO SCL1
R1 -0.3090169943749474
R3 0.3090169943749474
*OBSXYZ
S1 1.5 1 1.5 1 1 1
S2 4.5 2 4.5 1 1 1
S3 9.5 3 9.5 1 1 1
*END
)";

} // namespace Evaluator
#endif
