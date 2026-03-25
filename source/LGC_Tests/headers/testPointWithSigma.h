/*
 * SPDX-FileCopyrightText: CERN
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _LGC_TESTS_POINTWITHSIGMA_H
#define _LGC_TESTS_POINTWITHSIGMA_H

namespace pointWithSigma
{
char const *const computation1 = R"(*TITR
testFile
*OLOC
*APRI
*INSTR
*POIN
% points are fixed setting the unit vectors
ex 1 0 0 SX 0 SY 0 SZ 0
ey 0 1 0 SX 0 SY 0 SZ 0
ez 0 0 1 SX 0 SY 0 SZ 0
P0 1 2 3 SX 1 SY 2 SZ 3
% P1 and P1_aux should have the same covar in root
P1 11 12 13 BEAR 10 SLOPE 20 ROLL 30 HDEFL 20 SX 5 SY 6 SZ 7
P1_aux 7 8 9
% defining a point parallel to x axis (point moves on line (1,1,1) + t*(1,0,0))
xParallel 1 1 1 SY 0 SZ 0
% point on line (0,1,0)+t*(0,1,0)
xLine 0 1 0 BEAR 50 SY 0 SZ 0
% points with aprioti covariance matrices
P2 1 2 3 APRICOV DIAG(1,2,3)
P3 1 2 3 APRICOV MAT(10,2,3,2,40,2,3,2,30)
*OBSXYZ
% this will force xParallel to (0 1 1) (closest point to (0 0 0) on the line), nevertheless huge residual
xParallel 0 0 0 1 1 1
% will force the point to (0.5 0.5 0), huge residual
xLine 0 0 0 1 1 1
*FRAME trans 11 12 13 0 0 0 1
*FRAME P1_RSTI 0 0 0 0 0 10 1
*FRAME P1_RST 0 0 0 -20 30 -20 1
*OBSXYZ
P1_aux 0 0 0 5 6 7
*ENDFRAME
*ENDFRAME
*ENDFRAME
*FRAME movingFrame 1 2 3 4 5 6 1 TX TY TZ RX RY RZ SCL
*POIN
independentPoint 1 2 3 SX 1 SY 2 SZ 3
*OBSXYZ
ex 1 0 0 1 1 1
ey 0 1 0 1 1 1
ez 0 0 1 1 1 1
*ENDFRAME

*END
)";

char const *const computationHeightFixed = R"(*TITR
% to test if H is really fixed when using the sz=0 method instead of vxy
*RS2K
*APRI
*INSTR
*POIN
P0 1 2 3 SZ 0
*VXY
P1 1 2 3
*OBSXYZ
% move the points alot in the xy plane -> internally the cartesian z needs to be updated even though its fixed
% (transformEstimatedValue method)
P0 1000 2000 3000 1 1 1
P1 1000 2000 3000 1 1 1
*END
)";

char const *const computationAnglesDefinition = R"(*TITR
% check if the point with sigma works correctly with angles with respect to the definition in https://gitlab.cern.ch/apc/susofts/interfaces/scripts/PyEllipsoidRotation
*OLOC
*APRI
*INSTR
*POIN
pointWithSigmaInRoot 0 0 0 BEAR 10 SLOPE 20 ROLL 30 HDEFL 20 SX 3 SY 4 SZ 5
pointWithSigmaInRootPerturbed 0 0 0 BEAR 10 SLOPE 20 ROLL 30 HDEFL 20 SX 3 SY 4 SZ 5
% equivalent setup with frames, using the 4 angles in the point sigma definition
*FRAME RSTI 0 0 0 0 0 10 1
*FRAME RST 0 0 0 -20 30 -20 1
*POIN
pointWithSigmaInRST 0 0 0 SX 3 SY 4 SZ 5
pointWithSigmaInRSTPerturbed 0 0 0 SX 3 SY 4 SZ 5
*ENDFRAME
*ENDFRAME
*OBSXYZ
% additional contradicting perturbing observation
pointWithSigmaInRootPerturbed 1 2 3 7 8 9
pointWithSigmaInRSTPerturbed 1 2 3 7 8 9
*END
)";

} // namespace pointWithSigma
#endif
